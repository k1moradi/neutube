#include "z3dgl.h"

#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>
#include <QOpenGLFunctions>
#include <QString>
#include <QByteArray>
#include "QsLog/QsLog.h"

#include <atomic>
#include <cstring>
#include <mutex>
#include <deque>
#include <string>
#include <algorithm>

// ---------- Static state ----------
static QOpenGLDebugLogger* s_qtLogger = nullptr;
static bool                s_debugEnabled = false;
static bool                s_notificationsEnabled = true; // runtime toggle

// ---------- Deduplication storage ----------
namespace {
struct DedupKey {
  const char* file;     // __FILE__ pointer (stable within TU)
  int         line;     // __LINE__
  bool        hasCtx;   // presence/absence of current context
  bool        isNote;   // CHECK_GL_ERROR_NOTE vs CHECK_GL_ERROR (the latter aliases NOTE(nullptr))
  std::string note;     // content-based to avoid pointer aliasing issues

  bool equals(const DedupKey& o) const noexcept {
    return file == o.file &&
           line == o.line &&
           hasCtx == o.hasCtx &&
           isNote == o.isNote &&
           note == o.note;
  }
};

// Very small LRU-ish cache of seen call-sites
static std::mutex           g_dedupMutex;
static std::deque<DedupKey> g_dedupSeen;
static const std::size_t    g_dedupCap = 1024; // tune if needed
} // anonymous

bool Z3DGL_ShouldLogOnce(const char* file,
                         int         line,
                         const char* note,
                         bool        hasContext,
                         bool        isNoteVariant) noexcept
{
  try {
    const char* safeFile = file ? file : "";
    const char* safeNote = note ? note : "";

    DedupKey key{ safeFile, line, hasContext, isNoteVariant, std::string(safeNote) };

    std::lock_guard<std::mutex> lk(g_dedupMutex);

    for (const auto& k : g_dedupSeen) {
      if (k.equals(key)) return false; // seen already
    }
    if (g_dedupSeen.size() >= g_dedupCap) {
      g_dedupSeen.pop_front();
    }
    g_dedupSeen.push_back(std::move(key));
    return true;
  } catch (...) {
    // In low-memory or any unexpected condition, fail open (allow logging).
    return true;
  }
}

static void logMessage(const QOpenGLDebugMessage& m)
{
  if (!s_notificationsEnabled &&
      m.severity() == QOpenGLDebugMessage::NotificationSeverity) {
    return;
  }
  // Fast small helpers that return const char* (no heap, no QStrings)
  auto src = [](QOpenGLDebugMessage::Source s) -> const char* {
    switch (s) {
      case QOpenGLDebugMessage::APISource:            return "API";
      case QOpenGLDebugMessage::WindowSystemSource:   return "WINDOW";
      case QOpenGLDebugMessage::ShaderCompilerSource: return "SHADER";
      case QOpenGLDebugMessage::ThirdPartySource:     return "3RD";
      case QOpenGLDebugMessage::ApplicationSource:    return "APP";
      case QOpenGLDebugMessage::OtherSource:          return "OTHER";
      default:                                        return "UNK";
    }
  };
  auto typ = [](QOpenGLDebugMessage::Type t) -> const char* {
    switch (t) {
      case QOpenGLDebugMessage::ErrorType:              return "ERROR";
      case QOpenGLDebugMessage::DeprecatedBehaviorType: return "DEPRECATED";
      case QOpenGLDebugMessage::UndefinedBehaviorType:  return "UNDEFINED";
      case QOpenGLDebugMessage::PortabilityType:        return "PORTABILITY";
      case QOpenGLDebugMessage::PerformanceType:        return "PERFORMANCE";
      case QOpenGLDebugMessage::MarkerType:             return "MARKER";
      case QOpenGLDebugMessage::OtherType:              return "OTHER";
      default:                                          return "";
    }
  };
  auto sev = [](QOpenGLDebugMessage::Severity s) -> const char* {
    switch (s) {
      case QOpenGLDebugMessage::HighSeverity:         return "HIGH";
      case QOpenGLDebugMessage::MediumSeverity:       return "MEDIUM";
      case QOpenGLDebugMessage::LowSeverity:          return "LOW";
      case QOpenGLDebugMessage::NotificationSeverity: return "NOTE";
      default:                                        return "";
    }
  };

  // Call the lambdas ONCE and use the returned const char*
  const char* sevStr = sev(m.severity());
  const char* typStr = typ(m.type());
  const char* srcStr = src(m.source());

  // Build the line efficiently (no .arg chain)
  QString line =
      QLatin1String("[GL] ") +
      QLatin1String(sevStr) +
      QLatin1String("  ") +
      QLatin1String(typStr) +
      QLatin1String(" ( ") +
      QLatin1String(srcStr) +
      QLatin1String(" , id=") +
      QString::number(m.id()) +
      QLatin1String(" ): ") +
      m.message();

#ifdef _DEBUG_
  // Verbose in debug
  if (m.severity() == QOpenGLDebugMessage::HighSeverity) {
    LERROR() << line;
  } else if (m.severity() == QOpenGLDebugMessage::MediumSeverity ||
             m.severity() == QOpenGLDebugMessage::LowSeverity) {
    LWARN() << line;
  } else {
    LDEBUG() << line;
  }
#else
  // Lean in release: only High/Medium
  if (m.severity() == QOpenGLDebugMessage::HighSeverity) {
    LERROR() << line;
  } else if (m.severity() == QOpenGLDebugMessage::MediumSeverity) {
    LWARN() << line;
  }
#endif
}

static const char* errStr(GLenum err) {
  switch (err) {
    case GL_INVALID_ENUM:                   return "openGL invalid enumerant";
    case GL_INVALID_VALUE:                  return "openGL invalid value";
    case GL_INVALID_OPERATION:              return "openGL invalid operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION:  return "openGL invalid framebuffer operation";
    case GL_STACK_OVERFLOW:                 return "openGL stack overflow";
    case GL_STACK_UNDERFLOW:                return "openGL stack underflow";
    case GL_OUT_OF_MEMORY:                  return "openGL out of memory";
#ifdef GL_CONTEXT_LOST
    case GL_CONTEXT_LOST:                   return "openGL context lost";
#endif
    default:                                return "openGL unknown error";
  }
}

// ---------- Public API ----------

void Z3DGL_InitDebug()
{
  if (s_debugEnabled) return;

  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  if (!ctx) {
    LWARN() << "[GL] No current context; Z3DGL_InitDebug() deferred.";
    return;
  }

#ifdef GL_DEBUG_OUTPUT
  glEnable(GL_DEBUG_OUTPUT);
#endif
#ifdef GL_DEBUG_OUTPUT_SYNCHRONOUS
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

  // Attach a Qt logger (KHR_debug pathway)
  if (!s_qtLogger) {
    s_qtLogger = new QOpenGLDebugLogger();
    if (!s_qtLogger->initialize()) {
      delete s_qtLogger; s_qtLogger = nullptr;
      LWARN() << "[GL] QOpenGLDebugLogger not available on this context.";
      // We still keep CHECK_GL_ERROR working even without the logger.
    } else {
      QObject::connect(s_qtLogger, &QOpenGLDebugLogger::messageLogged,
                       [](const QOpenGLDebugMessage& m) { logMessage(m); });

#ifdef _DEBUG_
      s_qtLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
#else
      s_qtLogger->startLogging(QOpenGLDebugLogger::AsynchronousLogging);
#endif
      // Make shutdown idempotent and happen BEFORE the context vanishes.
      // This signal is emitted while the context is current.
      QObject::connect(ctx, &QOpenGLContext::aboutToBeDestroyed, ctx,
                       [](){ Z3DGL_ShutdownDebug(); }, Qt::DirectConnection);
    }
  }

  s_debugEnabled = true;
}

void Z3DGL_ShutdownDebug()
{
  if (s_qtLogger) {
    s_qtLogger->stopLogging();
    delete s_qtLogger;
    s_qtLogger = nullptr;
  }
  s_debugEnabled = false;
}

void Z3DGL_SetNotificationsEnabled(bool enabled)
{
  s_notificationsEnabled = enabled;
}

// Groups/markers via QOpenGLDebugLogger; no-ops if logger missing.
void Z3DGL_PushDebugGroup(const char* label, GLuint id)
{
  if (s_qtLogger) s_qtLogger->pushGroup(QString::fromUtf8(label ? label : ""), id);
}

void Z3DGL_PopDebugGroup()
{
  if (s_qtLogger) s_qtLogger->popGroup();
}

void Z3DGL_Marker(const char* label) {
  if (!QOpenGLContext::currentContext()) {
#ifdef _DEBUG_
    QLOG_WARN() << "Z3DGL_Marker skipped (no current QOpenGLContext). Note=\"" << label << '"';
#endif
    return;
  }
  const char* msg = (label && *label) ? label : "APP_MARKER";

  // Always log to QsLog so you can grep for it even if KHR_debug filters it
  LDEBUG() << "[GL][MARKER]" << msg;

  if (!s_qtLogger) return;

  static const GLuint kAppMarkerId = 0x534D4F4B; // 'SMOK'
  const auto qmsg = QOpenGLDebugMessage::createApplicationMessage(
      QString::fromUtf8(msg),
      kAppMarkerId,
      QOpenGLDebugMessage::NotificationSeverity);

  s_qtLogger->logMessage(qmsg);
}

// Drain all GL errors and log them with file:line:function and optional note.
GLenum _CheckGLErrorDrainAll(const char* file, int line, const char* function, const char* note)
{
  GLenum firstErr = GL_NO_ERROR;
  for (;;) {
    const GLenum err = glGetError();
    if (err == GL_NO_ERROR) break;
    if (firstErr == GL_NO_ERROR) firstErr = err;

    if (note) {
      LERROR() << file << ":" << line << " [" << (function ? function : "")
               << "] " << errStr(err) << " — note: " << note;
    } else {
      LERROR() << file << ":" << line << " [" << (function ? function : "")
               << "] " << errStr(err);
    }
  }
  return firstErr;
}

// ---------- Lightweight state probes ----------

bool checkGLState(GLenum pname, bool value)
{
  GLboolean b = GL_FALSE;
  Z_QT_GL_FUNCS->glGetBooleanv(pname, &b);
  return (b == GL_TRUE) == value;
}

bool checkGLState(GLenum pname, GLint value)
{
  GLint v = 0;
  Z_QT_GL_FUNCS->glGetIntegerv(pname, &v);
  return v == value;
}

bool checkGLState(GLenum pname, GLfloat value)
{
  GLfloat v = 0.0f;
  Z_QT_GL_FUNCS->glGetFloatv(pname, &v);
  return v == value;
}

bool checkGLState(GLenum pname, const glm::vec4 value)
{
  GLfloat v[4] = {0,0,0,0};
  Z_QT_GL_FUNCS->glGetFloatv(pname, v);
  return (v[0] == value[0] && v[1] == value[1] &&
          v[2] == value[2] && v[3] == value[3]);
}
