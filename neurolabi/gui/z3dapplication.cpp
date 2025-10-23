#include "z3dapplication.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QSurfaceFormat>
#include <QSysInfo>

#include "QsLog.h"
#include "z3dgpuinfo.h"
#include "zerror.h"
#include "z3dgl.h"

Z3DApplication* Z3DApplication::m_app = nullptr;

Z3DApplication::Z3DApplication(const QString &appDir)
  : m_applicationDirPath(appDir)
  , m_initialized(false)
  , m_glInitialized(false)
  , m_stereoViewSupported(false)
{
  m_app = this;
}

Z3DApplication::~Z3DApplication() = default;

void Z3DApplication::initialize()
{
  if (m_initialized) return;
  detectOS();
  // Compact, future-proof OS logging (Windows 11+, macOS, Linux)
  LINFO() << "OS:" << QSysInfo::prettyProductName();
  LINFO() << "Kernel:" << QSysInfo::kernelType() << QSysInfo::kernelVersion();
  LINFO() << "Build ABI:" << QSysInfo::buildAbi();
  LINFO() << "CPU Arch:" << QSysInfo::currentCpuArchitecture();
  // Resource roots
  m_shaderPath = ":/Resources/shader";
  m_fontPath   = ":/Resources/fonts";
  m_initialized = true;
}

void Z3DApplication::deinitialize()
{
  m_initialized = false;
}

bool Z3DApplication::initializeGL()
{
  if (!m_initialized) initialize();  // your non-GL one-time init (sets up logging, etc.)
  // A current context is required for all GL queries below.
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  if (!ctx) {
    m_errorMsg = "No current QOpenGLContext. Call initializeGL() after a context is current.";
    RECORD_WARNING_UNCOND(m_errorMsg.toStdString());
    RECORD_WARNING_UNCOND("3D functions will be disabled.");
    return false;
  }
  // If we've already initialized, treat it as SUCCESS (idempotent).
  // Still remember / adapt to the *new* current context when it changes.
  if (m_glInitialized) {
#ifdef _DEBUG_
    // Attach a debug callback for this new context too (debug is per-context).
    if (!m_initCtx || m_initCtx != ctx) {
      Z3DGL_InitDebug();
      Z3DGL_Marker("Z3DGL: init debug for new context");
      CHECK_GL_ERROR_NOTE("after InitDebug (existing init, new ctx)");
    }
#endif
    m_initCtx = ctx;
    m_errorMsg.clear();
    return true;
  }
  // First-time initialization path (with a current context)
  QOpenGLExtraFunctions* f = ctx->extraFunctions();
  if (!f) {
    m_errorMsg = "Failed to obtain QOpenGLExtraFunctions from the current context.";
    RECORD_WARNING_UNCOND(m_errorMsg.toStdString());
    RECORD_WARNING_UNCOND("3D functions will be disabled.");
    return false;
  }
  // --- Informational logging (safe only with a current context) ---
  const char* vendor   = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  const char* version  = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char* glsl     = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
  RECORD_TITLED_INFORMATION("OpenGL version:",            version  ? version  : "unknown");
  RECORD_TITLED_INFORMATION("GLSL version:",              glsl     ? glsl     : "unknown");
  RECORD_TITLED_INFORMATION("GL vendor:",                 vendor   ? vendor   : "unknown");
  RECORD_TITLED_INFORMATION("GL renderer:",               renderer ? renderer : "unknown");

  const QSurfaceFormat fmt = ctx->format();
  const QString fmtStr = QString("%1.%2, profile=%3, samples=%4, swapBehavior=%5")
      .arg(fmt.majorVersion()).arg(fmt.minorVersion())
      .arg(fmt.profile() == QSurfaceFormat::CoreProfile ? "core"
           : fmt.profile() == QSurfaceFormat::CompatibilityProfile ? "compat" : "none")
      .arg(fmt.samples())
      .arg(fmt.swapBehavior() == QSurfaceFormat::DoubleBuffer ? "double" : "single");
  RECORD_TITLED_INFORMATION("Qt surface format:", fmtStr.toUtf8().constData());

  const QString r = renderer ? QString::fromLatin1(renderer) : QString();
  if (r.contains("ANGLE") && r.contains("Direct3D 9", Qt::CaseInsensitive)) {
    LWARN() << "ANGLE D3D9 backend detected. Nsight Graphics cannot capture D3D9."
            << " Prefer QT_OPENGL=desktop or QT_ANGLE_PLATFORM=d3d11 at launch.";
  }

  // --- GPU capability probe (idempotent inside the probe helper) ---
  Z3DGpuInfoInstance.probeCurrentIfNeeded();
  Z3DGpuInfoInstance.logGpuInfo();
  if (!Z3DGpuInfoInstance.isSupported()) {
    m_errorMsg = Z3DGpuInfoInstance.getNotSupportedReason();
    RECORD_WARNING_UNCOND(m_errorMsg.toStdString());
    m_glInitialized = false;  // explicit, though default
    return false;
  }
#ifdef _DEBUG_
  // Install KHR_debug callback etc. for this (first) context
  Z3DGL_InitDebug();
  Z3DGL_Marker("Z3DGL: init smoke test");
  CHECK_GL_ERROR_NOTE("after InitDebug + marker");
#endif
  // Success: mark initialized and remember which context we did it under
  m_glInitialized = true;
  m_initCtx = ctx;
  m_errorMsg.clear();
  return true;
}

bool Z3DApplication::initializeGLLazyForCurrentContext()
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  if (!ctx) {
    m_errorMsg = "No current QOpenGLContext. Call after a canvas has made one current.";
    return false;
  }
  return initializeGL();  // idempotent; also attaches debug for a new ctx
}

bool Z3DApplication::deinitializeGL()
{
  Z3DGL_ShutdownDebug();
  m_glInitialized = false;
  return true;
}

QString Z3DApplication::getShaderPath(const QString& filename) const
{
  return m_shaderPath + (filename.isEmpty() ? QString() : QString("/") + filename);
}

QString Z3DApplication::getFontPath(const QString& filename) const
{
  return m_fontPath + (filename.isEmpty() ? QString() : QString("/") + filename);
}

void Z3DApplication::detectOS()
{
  // Using Qt5's QSysInfo universally — supports Windows 11+ and future versions
  m_osString = QSysInfo::prettyProductName();
}
