#ifndef ZGLEW_H
#define ZGLEW_H

// Qt GL includes + function tables
#include <QtGui/qopengl.h>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLExtraFunctions>
#include <QtCore/qglobal.h>
#include <QtCore/QByteArray>

#if defined(_WIN32)
  #include <GL/glu.h>
#elif defined(__APPLE__)
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

/* ---- Per-context function table ---- */
static inline QOpenGLExtraFunctions* zgl()
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zgl()", "A current QOpenGLContext is required before calling GL functions");
  return ctx->extraFunctions();
}

// convenience alias used in some source files
#define Z_QT_GL_FUNCS zgl()

/* ---- Version / extension shims (replacing GLEW_VERSION_* and GLEW_* flags) ---- */
static inline bool zglHasVersion(int maj, int min)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  if (!ctx) return false;
  const QSurfaceFormat fmt = ctx->format();
  const int M = fmt.majorVersion();
  const int m = fmt.minorVersion();
  return (M > maj) || (M == maj && m >= min);
}
static inline bool zglHasExtension(const char* ext)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  return ctx && ctx->hasExtension(QByteArray(ext));
}

#define GLEW_VERSION_2_0  (zglHasVersion(2,0))
#define GLEW_VERSION_2_1  (zglHasVersion(2,1))
#define GLEW_VERSION_3_0  (zglHasVersion(3,0))
#define GLEW_VERSION_3_1  (zglHasVersion(3,1))
#define GLEW_VERSION_3_2  (zglHasVersion(3,2))
#define GLEW_VERSION_3_3  (zglHasVersion(3,3))
#define GLEW_VERSION_4_0  (zglHasVersion(4,0))
#define GLEW_VERSION_4_1  (zglHasVersion(4,1))
#define GLEW_VERSION_4_2  (zglHasVersion(4,2))
#define GLEW_VERSION_4_3  (zglHasVersion(4,3))
#define GLEW_VERSION_4_4  (zglHasVersion(4,4))
#define GLEW_VERSION_4_5  (zglHasVersion(4,5))
#define GLEW_VERSION_4_6  (zglHasVersion(4,6))

#define GLEW_KHR_debug                 (zglHasExtension("GL_KHR_debug"))
#define GLEW_ARB_debug_output          (zglHasExtension("GL_ARB_debug_output"))
#define GLEW_ARB_vertex_array_object   (zglHasExtension("GL_ARB_vertex_array_object"))
#define GLEW_ARB_texture_float         (zglHasExtension("GL_ARB_texture_float"))
#define GLEW_EXT_clip_cull_distance    (zglHasExtension("GL_EXT_clip_cull_distance"))

/* ---- glBindFragDataLocation resolver (Qt ExtraFunctions may not expose it) ---- */
static inline void zglBindFragDataLocation(GLuint program, GLuint colorNumber, const char* name)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglBindFragDataLocation", "Requires a current context");

  // Try core symbol
  auto core = reinterpret_cast<void (*)(GLuint, GLuint, const GLchar*)>(
      ctx->getProcAddress("glBindFragDataLocation"));
  if (core) { core(program, colorNumber, name); return; }

  // Try EXT symbol
  auto ext = reinterpret_cast<void (*)(GLuint, GLuint, const GLchar*)>(
      ctx->getProcAddress("glBindFragDataLocationEXT"));
  if (ext) { ext(program, colorNumber, name); return; }

  // If neither is available, noop (driver too old or ES context).
}

/* ---- Map common GL entry points to Qt per-context functions ---- */
/* IMPORTANT: We intentionally DO NOT define macros for glGetError/glGetBooleanv/
              glGetIntegerv/glGetFloatv/glGetString to avoid colliding with
              existing Z_QT_GL_FUNCS->glGet*() usages. */

// Buffers / VAO
#define glGenBuffers(...)                 zgl()->glGenBuffers(__VA_ARGS__)
#define glDeleteBuffers(...)              zgl()->glDeleteBuffers(__VA_ARGS__)
#define glBindBuffer(...)                 zgl()->glBindBuffer(__VA_ARGS__)
#define glBufferData(...)                 zgl()->glBufferData(__VA_ARGS__)
#define glBufferSubData(...)              zgl()->glBufferSubData(__VA_ARGS__)
#define glGenVertexArrays(...)            zgl()->glGenVertexArrays(__VA_ARGS__)
#define glBindVertexArray(...)            zgl()->glBindVertexArray(__VA_ARGS__)
#define glDeleteVertexArrays(...)         zgl()->glDeleteVertexArrays(__VA_ARGS__)
#define glBindVertexBuffer(...)           zgl()->glBindVertexBuffer(__VA_ARGS__)
#define glEnableVertexAttribArray(...)    zgl()->glEnableVertexAttribArray(__VA_ARGS__)
#define glDisableVertexAttribArray(...)   zgl()->glDisableVertexAttribArray(__VA_ARGS__)
#define glVertexAttribPointer(...)        zgl()->glVertexAttribPointer(__VA_ARGS__)
#define glVertexAttribDivisor(...)        zgl()->glVertexAttribDivisor(__VA_ARGS__)
#define glGetVertexAttribiv(...)          zgl()->glGetVertexAttribiv(__VA_ARGS__)
#define glIsVertexArray(...)              zgl()->glIsVertexArray(__VA_ARGS__)

// Programs / uniforms
#define glUseProgram(...)                 zgl()->glUseProgram(__VA_ARGS__)
#define glGetUniformLocation(...)         zgl()->glGetUniformLocation(__VA_ARGS__)
#define glUniform1i(...)                  zgl()->glUniform1i(__VA_ARGS__)
#define glUniform1f(...)                  zgl()->glUniform1f(__VA_ARGS__)
#define glUniform2i(...)                  zgl()->glUniform2i(__VA_ARGS__)
#define glUniform3i(...)                  zgl()->glUniform3i(__VA_ARGS__)
#define glUniform4i(...)                  zgl()->glUniform4i(__VA_ARGS__)
#define glUniform2ui(...)                 zgl()->glUniform2ui(__VA_ARGS__)
#define glUniform3ui(...)                 zgl()->glUniform3ui(__VA_ARGS__)
#define glUniform4ui(...)                 zgl()->glUniform4ui(__VA_ARGS__)
#define glUniform2iv(...)                 zgl()->glUniform2iv(__VA_ARGS__)
#define glUniform3iv(...)                 zgl()->glUniform3iv(__VA_ARGS__)
#define glUniform4iv(...)                 zgl()->glUniform4iv(__VA_ARGS__)
#define glUniform2uiv(...)                zgl()->glUniform2uiv(__VA_ARGS__)
#define glUniform3uiv(...)                zgl()->glUniform3uiv(__VA_ARGS__)
#define glUniform4uiv(...)                zgl()->glUniform4uiv(__VA_ARGS__)
#define glUniform2fv(...)                 zgl()->glUniform2fv(__VA_ARGS__)
#define glUniform3fv(...)                 zgl()->glUniform3fv(__VA_ARGS__)
#define glUniform4fv(...)                 zgl()->glUniform4fv(__VA_ARGS__)
#define glUniformMatrix2fv(...)           zgl()->glUniformMatrix2fv(__VA_ARGS__)
#define glUniformMatrix3fv(...)           zgl()->glUniformMatrix3fv(__VA_ARGS__)
#define glUniformMatrix4fv(...)           zgl()->glUniformMatrix4fv(__VA_ARGS__)
#define glBindFragDataLocation(...)       zglBindFragDataLocation(__VA_ARGS__)

// Textures
#define glActiveTexture(...)              zgl()->glActiveTexture(__VA_ARGS__)
#define glTexImage3D(...)                 zgl()->glTexImage3D(__VA_ARGS__)
#define glGenerateMipmap(...)             zgl()->glGenerateMipmap(__VA_ARGS__)

// --- FBOs (Qt exposes some, we shim the rest) ---
#define glGenFramebuffers(...)            zgl()->glGenFramebuffers(__VA_ARGS__)
#define glDeleteFramebuffers(...)         zgl()->glDeleteFramebuffers(__VA_ARGS__)
#define glBindFramebuffer(...)            zgl()->glBindFramebuffer(__VA_ARGS__)
#define glFramebufferTexture2D(...)       zgl()->glFramebufferTexture2D(__VA_ARGS__)
#define glFramebufferTextureLayer(...)    zgl()->glFramebufferTextureLayer(__VA_ARGS__)
#define glCheckFramebufferStatus(...)     zgl()->glCheckFramebufferStatus(__VA_ARGS__)
#define glDrawBuffers(...)                zgl()->glDrawBuffers(__VA_ARGS__)

// Drawing
#define glDrawArrays(...)                 zgl()->glDrawArrays(__VA_ARGS__)
#define glDrawElements(...)               zgl()->glDrawElements(__VA_ARGS__)

// ---- Shims for APIs Qt doesn't wrap on some builds ----
typedef void (QOPENGLF_APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DPROC)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef void (QOPENGLF_APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DPROC)(GLenum, GLenum, GLenum, GLuint, GLint, GLint);
typedef void (QOPENGLF_APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC)(GLenum, GLenum, GLuint, GLint);
typedef void (QOPENGLF_APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC)(GLenum, GLenum, GLuint, GLint, GLint);

static inline void zglFramebufferTextureLayer(GLenum target, GLenum attachment,
                                              GLuint texture, GLint level, GLint layer)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglFramebufferTextureLayer", "Requires a current context");

  auto pLayer = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURELAYERPROC>(
      ctx->getProcAddress("glFramebufferTextureLayer"));
  if (!pLayer)
    pLayer = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURELAYERPROC>(
      ctx->getProcAddress("glFramebufferTextureLayerEXT"));
  if (pLayer) { pLayer(target, attachment, texture, level, layer); return; }

  // Fallback (GL 3.2+): attaches the entire level — not per-layer, but better than failing.
  auto pGen = reinterpret_cast<PFNGLFRAMEBUFFERTEXTUREPROC>(
      ctx->getProcAddress("glFramebufferTexture"));
  if (pGen) { pGen(target, attachment, texture, level); }
}

static inline void zglFramebufferTexture1D(GLenum target, GLenum attachment,
                                           GLenum textarget, GLuint texture, GLint level)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglFramebufferTexture1D", "Requires a current context");

  auto p1D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(
      ctx->getProcAddress("glFramebufferTexture1D"));
  if (!p1D)
    p1D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(
      ctx->getProcAddress("glFramebufferTexture1DEXT"));
  if (p1D) { p1D(target, attachment, textarget, texture, level); return; }

  auto pGen = reinterpret_cast<PFNGLFRAMEBUFFERTEXTUREPROC>(
      ctx->getProcAddress("glFramebufferTexture"));
  if (pGen) { pGen(target, attachment, texture, level); }
}

static inline void zglFramebufferTexture3D(GLenum target, GLenum attachment,
                                           GLenum /*textarget*/, GLuint texture,
                                           GLint level, GLint zoffset)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglFramebufferTexture3D", "Requires a current context");

  auto p3D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(
      ctx->getProcAddress("glFramebufferTexture3D"));
  if (!p3D)
    p3D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(
      ctx->getProcAddress("glFramebufferTexture3DEXT"));
  if (p3D) { p3D(target, attachment, GL_TEXTURE_3D, texture, level, zoffset); return; }

  // Fallback: attach the specific slice as a layer
  zglFramebufferTextureLayer(target, attachment, texture, level, zoffset);
}

// Undef any previous macros then map to our shims
#ifdef glFramebufferTextureLayer
#  undef glFramebufferTextureLayer
#endif
#ifdef glFramebufferTexture1D
#  undef glFramebufferTexture1D
#endif
#ifdef glFramebufferTexture3D
#  undef glFramebufferTexture3D
#endif

#define glFramebufferTextureLayer(...)  zglFramebufferTextureLayer(__VA_ARGS__)
#define glFramebufferTexture1D(...)     zglFramebufferTexture1D(__VA_ARGS__)
#define glFramebufferTexture3D(...)     zglFramebufferTexture3D(__VA_ARGS__)

// ---- Extra shims: blend equation + occlusion queries ----
typedef void (QOPENGLF_APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum);
static inline void zglBlendEquation(GLenum mode)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglBlendEquation", "Requires a current context");

  auto p = reinterpret_cast<PFNGLBLENDEQUATIONPROC>(ctx->getProcAddress("glBlendEquation"));
  if (!p) p = reinterpret_cast<PFNGLBLENDEQUATIONPROC>(ctx->getProcAddress("glBlendEquationEXT"));
  if (p) p(mode);  // else: noop if not available (e.g., very old/ES context)
}

typedef void (QOPENGLF_APIENTRYP PFNGLGENQUERIESPROC)(GLsizei, GLuint*);
typedef void (QOPENGLF_APIENTRYP PFNGLDELETEQUERIESPROC)(GLsizei, const GLuint*);
typedef void (QOPENGLF_APIENTRYP PFNGLBEGINQUERYPROC)(GLenum, GLuint);
typedef void (QOPENGLF_APIENTRYP PFNGLENDQUERYPROC)(GLenum);
typedef void (QOPENGLF_APIENTRYP PFNGLGETQUERYOBJECTUIVPROC)(GLuint, GLenum, GLuint*);

static inline void zglGenQueries(GLsizei n, GLuint* ids)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglGenQueries", "Requires a current context");
  auto p = reinterpret_cast<PFNGLGENQUERIESPROC>(ctx->getProcAddress("glGenQueries"));
  if (!p) p = reinterpret_cast<PFNGLGENQUERIESPROC>(ctx->getProcAddress("glGenQueriesARB"));
  if (p) p(n, ids);
}

static inline void zglDeleteQueries(GLsizei n, const GLuint* ids)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglDeleteQueries", "Requires a current context");
  auto p = reinterpret_cast<PFNGLDELETEQUERIESPROC>(ctx->getProcAddress("glDeleteQueries"));
  if (!p) p = reinterpret_cast<PFNGLDELETEQUERIESPROC>(ctx->getProcAddress("glDeleteQueriesARB"));
  if (p) p(n, ids);
}

static inline void zglBeginQuery(GLenum target, GLuint id)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglBeginQuery", "Requires a current context");
  auto p = reinterpret_cast<PFNGLBEGINQUERYPROC>(ctx->getProcAddress("glBeginQuery"));
  if (!p) p = reinterpret_cast<PFNGLBEGINQUERYPROC>(ctx->getProcAddress("glBeginQueryARB"));
  if (p) p(target, id);
}

static inline void zglEndQuery(GLenum target)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglEndQuery", "Requires a current context");
  auto p = reinterpret_cast<PFNGLENDQUERYPROC>(ctx->getProcAddress("glEndQuery"));
  if (!p) p = reinterpret_cast<PFNGLENDQUERYPROC>(ctx->getProcAddress("glEndQueryARB"));
  if (p) p(target);
}

static inline void zglGetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params)
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  Q_ASSERT_X(ctx, "zglGetQueryObjectuiv", "Requires a current context");
  auto p = reinterpret_cast<PFNGLGETQUERYOBJECTUIVPROC>(ctx->getProcAddress("glGetQueryObjectuiv"));
  if (!p) p = reinterpret_cast<PFNGLGETQUERYOBJECTUIVPROC>(ctx->getProcAddress("glGetQueryObjectuivARB"));
  if (p) p(id, pname, params);
}

// Public macros for the above
#ifdef glBlendEquation
#  undef glBlendEquation
#endif
#ifdef glGenQueries
#  undef glGenQueries
#endif
#ifdef glDeleteQueries
#  undef glDeleteQueries
#endif
#ifdef glBeginQuery
#  undef glBeginQuery
#endif
#ifdef glEndQuery
#  undef glEndQuery
#endif
#ifdef glGetQueryObjectuiv
#  undef glGetQueryObjectuiv
#endif

#define glBlendEquation(...)         zglBlendEquation(__VA_ARGS__)
#define glGenQueries(...)            zglGenQueries(__VA_ARGS__)
#define glDeleteQueries(...)         zglDeleteQueries(__VA_ARGS__)
#define glBeginQuery(...)            zglBeginQuery(__VA_ARGS__)
#define glEndQuery(...)              zglEndQuery(__VA_ARGS__)
#define glGetQueryObjectuiv(...)     zglGetQueryObjectuiv(__VA_ARGS__)

#endif /* ZGLEW_H */
