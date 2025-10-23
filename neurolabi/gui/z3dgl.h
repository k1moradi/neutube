#ifndef Z3DGL_H
#define Z3DGL_H

#include "zglew.h"
#include "zglmutils.h"

#include <QtGui/qopengl.h>
#include <QsLog.h>
#include <QOpenGLContext>
#ifndef LWARN
  #include <QtGlobal>
  #define LWARN() qWarning()
#endif

#include <exception>
#include <string>

// ---------- Public enums (unchanged) ----------
enum Z3DEye { LeftEye, CenterEye, RightEye };
enum Z3DScreenShotType { MonoView, HalfSideBySideStereoView, FullSideBySideStereoView };

// ---------- Debug logger lifecycle ----------
void Z3DGL_InitDebug();                            // attaches logger when a context is current
void Z3DGL_ShutdownDebug();                        // stops logger / cleanup
void Z3DGL_SetNotificationsEnabled(bool enabled);  // optional: mute/unmute NotificationSeverity

// ---------- Debug groups & markers (unchanged API) ----------
void Z3DGL_PushDebugGroup(const char* label, GLuint id = 0);
void Z3DGL_PopDebugGroup();
void Z3DGL_Marker(const char* label);

// ---------- Error drain (unchanged signature) ----------
GLenum _CheckGLErrorDrainAll(const char* file, int line, const char* function, const char* note);

// ---------- Macro helpers (names/behavior preserved) ----------
#ifndef Z3DGL_FUNCNAME
# if defined(__GNUC__) || defined(__clang__)
#  define Z3DGL_FUNCNAME __PRETTY_FUNCTION__
# else
#  define Z3DGL_FUNCNAME __FUNCTION__
# endif
#endif

static inline bool Z3DGL_HasText(const char* s) noexcept {
  return s != nullptr && s[0] != '\0';
}

// ---------- Deduplication helper (new public helper used by macros) ----------
// Returns true if we should log this call-site (first time seen), false if it was seen recently.
// Keyed by (file, line, note, hasContext, isNoteVariant) so presence/absence of current context
// and macro variant are both part of the signature.
bool Z3DGL_ShouldLogOnce(const char* file,
                         int         line,
                         const char* note,
                         bool        hasContext,
                         bool        isNoteVariant) noexcept;

#ifdef _DEBUG_
  #define CHECK_GL_ERROR_NOTE(note)                                              \
    do {                                                                         \
      const char* __note_cstr = (note);                                          \
      QOpenGLContext* __ctx = QOpenGLContext::currentContext();                  \
      const bool __hasCtx = (__ctx != nullptr);                                  \
      if (__hasCtx) {                                                            \
        if (Z3DGL_HasText(__note_cstr)) Z3DGL_Marker(__note_cstr);               \
        _CheckGLErrorDrainAll(__FILE__, __LINE__, Z3DGL_FUNCNAME, __note_cstr);  \
      } else {                                                                   \
        if (Z3DGL_ShouldLogOnce(__FILE__, __LINE__, __note_cstr, false, true)) { \
          if (Z3DGL_HasText(__note_cstr)) {                                      \
            LWARN() << "CHECK_GL_ERROR_NOTE skipped (no current QOpenGLContext). "\
                    << "Note=\"" << __note_cstr << "\" @" << __FILE__ << ":"     \
                    << __LINE__ << " [" << Z3DGL_FUNCNAME << "]";                \
          } else {                                                               \
            LWARN() << "CHECK_GL_ERROR_NOTE skipped (no current QOpenGLContext) @"\
                    << __FILE__ << ":" << __LINE__ << " [" << Z3DGL_FUNCNAME << "]";\
          }                                                                      \
        }                                                                        \
      }                                                                          \
    } while (0)
#else
  // In non-debug builds: still dedupe and log *once per call-site* if there is no context.
  // This preserves signal without spam. When a context exists, we just drain errors quietly.
  #define CHECK_GL_ERROR_NOTE(note)                                              \
    do {                                                                         \
      const char* __note_cstr = (note);                                          \
      QOpenGLContext* __ctx = QOpenGLContext::currentContext();                  \
      const bool __hasCtx = (__ctx != nullptr);                                  \
      if (__hasCtx) {                                                            \
        _CheckGLErrorDrainAll(__FILE__, __LINE__, Z3DGL_FUNCNAME, __note_cstr);  \
      } else {                                                                   \
        if (Z3DGL_ShouldLogOnce(__FILE__, __LINE__, __note_cstr, false, true)) { \
          if (Z3DGL_HasText(__note_cstr)) {                                      \
            LWARN() << "CHECK_GL_ERROR_NOTE skipped (no current QOpenGLContext). "\
                    << "Note=\"" << __note_cstr << "\" @" << __FILE__ << ":"     \
                    << __LINE__ << " [" << Z3DGL_FUNCNAME << "]";                \
          } else {                                                               \
            LWARN() << "CHECK_GL_ERROR_NOTE skipped (no current QOpenGLContext) @"\
                    << __FILE__ << ":" << __LINE__ << " [" << Z3DGL_FUNCNAME << "]";\
          }                                                                      \
        }                                                                        \
      }                                                                          \
    } while (0)
#endif

// Alias preserved
#define CHECK_GL_ERROR CHECK_GL_ERROR_NOTE(nullptr)

// ---------- Lightweight GL state probes (unchanged names) ----------
bool checkGLState(GLenum pname, bool value);
bool checkGLState(GLenum pname, GLint value);
bool checkGLState(GLenum pname, GLfloat value);
bool checkGLState(GLenum pname, const glm::vec4 value);

// ---------- Local exception (unchanged) ----------
class Exception : public std::exception {
public:
  explicit Exception(const std::string& what = "") : m_what(what) {}
  const char* what() const throw() override { return m_what.c_str(); }
private:
  std::string m_what;
};

#endif // Z3DGL_H
