#include "zglew.h"
#include "z3dshaderprogram.h"
#include <QFile>

#include "z3dapplication.h"
#include "QsLog.h"
#include "z3dvolume.h"
#include "z3d_attrib_locations.h"

Z3DShaderProgram::Z3DShaderProgram(QObject * parent)
  : QOpenGLShaderProgram(parent)
  , m_logUniformLocationError(false)
{
}

Z3DShaderProgram::~Z3DShaderProgram()
{
}

bool Z3DShaderProgram::bind()
{
  m_textureUnitManager.reset();
  return QOpenGLShaderProgram::bind();
}

void Z3DShaderProgram::bindFragDataLocation(GLuint colorNumber, const QString &name)
{
  if (GLEW_VERSION_3_0) {
    glBindFragDataLocation(programId(), colorNumber, name.toLocal8Bit().constData());
  }
}

void Z3DShaderProgram::bindTexture(const QString &name, const Z3DTexture *texture)
{
  if (texture) {
    m_textureUnitManager.nextAvailableUnit();
    m_textureUnitManager.activateCurrentUnit();
    texture->bind();
    setUniformValue(name, m_textureUnitManager.getCurrentUnitNumber());
    glActiveTexture(GL_TEXTURE0);
    CHECK_GL_ERROR;
  }
}

void Z3DShaderProgram::bindTexture(const QString &name, GLenum target, GLuint textureId)
{
  m_textureUnitManager.nextAvailableUnit();
  m_textureUnitManager.activateCurrentUnit();
  glBindTexture(target, textureId);
  setUniformValue(name, m_textureUnitManager.getCurrentUnitNumber());
  glActiveTexture(GL_TEXTURE0);
  CHECK_GL_ERROR;
}

void Z3DShaderProgram::bindVolume(const QString &name, Z3DVolume *volume)
{
  if (!volume)
    return;

  m_textureUnitManager.nextAvailableUnit();
  m_textureUnitManager.activateCurrentUnit();

  if (!volume->getTexture()) {
    LWARN() << "volume do not contains any texture";
    glActiveTexture(GL_TEXTURE0);
    return;
  }

  volume->getTexture()->bind();
  volume->setUniform(*this, name, m_textureUnitManager.getCurrentUnitNumber());
  glActiveTexture(GL_TEXTURE0);
  CHECK_GL_ERROR;
}

void Z3DShaderProgram::bindVolume(const QString &name, Z3DVolume *volume,
                                  GLint minFilter, GLint magFilter)
{
  if (!volume)
    return;

  m_textureUnitManager.nextAvailableUnit();
  m_textureUnitManager.activateCurrentUnit();

  if (!volume->getTexture()) {
    LWARN() << "volume do not contains any texture";
    glActiveTexture(GL_TEXTURE0);
    return;
  }

  volume->getTexture()->bind();

  GLenum target = volume->getTexture()->getTextureTarget();
  // texture filtering
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);

  volume->setUniform(*this, name, m_textureUnitManager.getCurrentUnitNumber());
  glActiveTexture(GL_TEXTURE0);
  CHECK_GL_ERROR;
}

void Z3DShaderProgram::loadFromSourceFile(const QString &vertFilename, const QString &geomFilename,
                                          const QString &fragFilename, const QString &header)
{
  m_vertSrcs.clear();
  m_geomSrcs.clear();
  m_fragSrcs.clear();

  // vert
  QString vFilename = Z3DApplication::app()->getShaderPath(vertFilename);
  QFile vFile(vFilename);

  if (!vFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw Exception(QString("Can not open vertex shader file: %1.  Error String: %2").arg(vFilename).arg(vFile.errorString()).toStdString());
  }

  m_vertSrcs.push_back(QString());
  m_vertSrcs[0] = vFile.readAll();
  vFile.close();

  // geom
  if (!geomFilename.isEmpty()) {
    QString gFilename = Z3DApplication::app()->getShaderPath(geomFilename);
    QFile gFile(gFilename);

    if (!gFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      throw Exception(QString("Can not open geometry shader file: %1.  Error String: %2").arg(gFilename).arg(gFile.errorString()).toStdString());
    }

    m_geomSrcs.push_back(QString());
    m_geomSrcs[0] = gFile.readAll();
    gFile.close();
  }

  // frag
  QString fFilename = Z3DApplication::app()->getShaderPath(fragFilename);
  QFile fFile(fFilename);

  if (!fFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw Exception(QString("Can not open fragment shader file: %1.  Error String: %2").arg(fFilename).arg(fFile.errorString()).toStdString());
  }

  m_fragSrcs.push_back(QString());
  m_fragSrcs[0] = fFile.readAll();
  fFile.close();

  // load
  loadFromSourceCode(m_vertSrcs, m_geomSrcs, m_fragSrcs, header);
}

void Z3DShaderProgram::loadFromSourceFile(const QString &vertFilename, const QString &fragFilename,
                                          const QString &header)
{
  loadFromSourceFile(vertFilename, "", fragFilename, header);
}

void Z3DShaderProgram::loadFromSourceFile(const QStringList &shaderFilenames, const QString &header)
{
  m_vertSrcs.clear();
  m_geomSrcs.clear();
  m_fragSrcs.clear();

  for (int i=0; i<shaderFilenames.size(); ++i) {
    QString filename = Z3DApplication::app()->getShaderPath(shaderFilenames[i]);
    if (filename.endsWith(".vert", Qt::CaseInsensitive)) {
      QFile file(filename);

      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Exception(QString("Can not open vertex shader file: %1.  Error String: %2").arg(filename).arg(file.errorString()).toStdString());
      }

      QString vertSrc = file.readAll();
      m_vertSrcs.push_back(vertSrc);
    } else if (filename.endsWith(".geom", Qt::CaseInsensitive)) {
      QFile file(filename);

      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Exception(QString("Can not open geometry shader file: %1.  Error String: %2").arg(filename).arg(file.errorString()).toStdString());
      }

      QString geomSrc = file.readAll();
      m_geomSrcs.push_back(geomSrc);
    } else if (filename.endsWith(".frag", Qt::CaseInsensitive)) {
      QFile file(filename);

      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Exception(QString("Can not open fragment shader file: %1.  Error String: %2").arg(filename).arg(file.errorString()).toStdString());
      }

      QString fragSrc = file.readAll();
      m_fragSrcs.push_back(fragSrc);
    } else {
      throw Exception(QString("Not supported file extension: %1. Use .vert, .geom or .frag as shader extension").arg(filename).toStdString());
    }
  }

  loadFromSourceCode(m_vertSrcs, m_geomSrcs, m_fragSrcs, header);
}

void Z3DShaderProgram::loadFromSourceCode(const QStringList &vertSrcs, const QStringList &geomSrcs,
                                          const QStringList &fragSrcs, const QString &header)
{
  removeAllShaders();
  for (int i=0; i<vertSrcs.size(); ++i) {
    QString vertSrc = header + vertSrcs[i];
    if (!addShaderFromSourceCode(QOpenGLShader::Vertex, vertSrc)) {
      LDEBUG() << vertSrc;
      throw Exception("Can not compile vertex shader. Error log: " + log().toStdString());
    }
  }

  for (int i=0; i<geomSrcs.size(); ++i) {
    QString geomSrc = header + geomSrcs[i];
    if (!addShaderFromSourceCode(QOpenGLShader::Geometry, geomSrc)) {
      LDEBUG() << geomSrc;
      throw Exception("Can not compile geometry shader. Error log: " + log().toStdString());
    }
  }

  for (int i=0; i<fragSrcs.size(); ++i) {
    QString fragSrc = header + fragSrcs[i];
    if (!addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc)) {
      LDEBUG() << fragSrc;
      throw Exception("Can not compile fragment shader. Error log: " + log().toStdString());
    }
  }

  // Bind every declared vertex attribute name to its fixed, compile-time location
  // *before* linking the shader program. This guarantees stable attribute indices
  // across all programs and enables the rest of the renderer to use the fast,
  // integer-based OpenGL/Qt APIs (e.g., enableAttributeArray/setAttributeBuffer)
  // without any runtime name lookups or string hashing.
  for (const Z3DAttr::Binding& attributeBindingRecord : Z3DAttr::kBindings) {
      bindAttributeLocation(
          attributeBindingRecord.name,
          Z3DAttr::loc(attributeBindingRecord.attr)  // MUST be called before link
      );
  }

  if (!link()) {
    throw Exception("Can not link shader. Error log: " + log().toStdString());
  }

#ifdef _DEBUG_
  // Debug-only verification:
  // For each declared binding, check the linked program's location **if** the attribute exists.
  // If the attribute is not present or optimized out in this program, Qt returns -1; skip in that case.
  for (const Z3DAttr::Binding& attributeBindingRecord : Z3DAttr::kBindings) {
    const int queriedLocationFromLinkedProgram = attributeLocation(attributeBindingRecord.name);
    if (queriedLocationFromLinkedProgram == -1) {
      // Attribute not active in this program – nothing to verify.
      continue;
    }
    const int expectedFixedLocationForAttribute = Z3DAttr::loc(attributeBindingRecord.attr);
    if (queriedLocationFromLinkedProgram != expectedFixedLocationForAttribute) {
      LERROR() << "Attrib binding mismatch for"
               << attributeBindingRecord.name
               << "expected" << expectedFixedLocationForAttribute
               << "got" << queriedLocationFromLinkedProgram;
    }
    Q_ASSERT(queriedLocationFromLinkedProgram == expectedFixedLocationForAttribute);
  }
#endif  // _DEBUG_
}

void Z3DShaderProgram::loadFromSourceCode(const QStringList &vertSrcs, const QStringList &fragSrcs,
                                          const QString &header)
{
  loadFromSourceCode(vertSrcs, QStringList(), fragSrcs, header);
}

void Z3DShaderProgram::setHeaderAndRebuild(const QString &header)
{
  loadFromSourceCode(m_vertSrcs, m_geomSrcs, m_fragSrcs, header);
}

GLint Z3DShaderProgram::getUniformLocation(const QString &name)
{
  GLint l = uniformLocation(name);
  if (l == -1 && logUniformLocationError()) {
    LWARN() << "Failed to locate uniform:" << name;
  }
  return l;
}

void Z3DShaderProgram::setUniformValue(GLint loc, GLint v1, GLint v2)
{
  if (loc != -1) {
    glUniform2i(loc, v1, v2);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, GLint v1, GLint v2, GLint v3)
{
  if (loc != -1) {
    glUniform3i(loc, v1, v2, v3);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, GLint v1, GLint v2, GLint v3, GLint v4)
{
  if (loc != -1) {
    glUniform4i(loc, v1, v2, v3, v4);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, GLuint v1, GLuint v2)
{
  if (loc != -1) {
    glUniform2ui(loc, v1, v2);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, GLuint v1, GLuint v2, GLuint v3)
{
  if (loc != -1) {
    glUniform3ui(loc, v1, v2, v3);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, GLuint v1, GLuint v2, GLuint v3, GLuint v4)
{
  if (loc != -1) {
    glUniform4ui(loc, v1, v2, v3, v4);
  }
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLfloat value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLint value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLuint value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLfloat x, GLfloat y)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, x, y);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLfloat x, GLfloat y, GLfloat z)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, x, y, z);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, x, y, z, w);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLint v1, GLint v2)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, v1, v2);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLint v1, GLint v2, GLint v3)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, v1, v2, v3);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLint v1, GLint v2, GLint v3, GLint v4)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, v1, v2, v3, v4);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLuint v1, GLuint v2)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, v1, v2);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLuint v1, GLuint v2, GLuint v3)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, v1, v2, v3);
}

void Z3DShaderProgram::setUniformValue(const QString &name, GLuint v1, GLuint v2, GLuint v3, GLuint v4)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, v1, v2, v3, v4);
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::vec2 &value)
{
  if (loc != -1) {
    glUniform2fv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::vec3 &value)
{
  if (loc != -1) {
    glUniform3fv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::vec4 &value)
{
  if (loc != -1) {
    glUniform4fv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::ivec2 &value)
{
  if (loc != -1) {
    glUniform2iv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::ivec3 &value)
{
  if (loc != -1) {
    glUniform3iv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::ivec4 &value)
{
  if (loc != -1) {
    glUniform4iv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::uvec2 &value)
{
  if (loc != -1) {
    glUniform2uiv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::uvec3 &value)
{
  if (loc != -1) {
    glUniform3uiv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::uvec4 &value)
{
  if (loc != -1) {
    glUniform4uiv(loc, 1, &value[0]);
  }
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::vec2 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::vec3 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::vec4 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::ivec2 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::ivec3 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::ivec4 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::uvec2 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::uvec3 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::uvec4 &value)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value);
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::vec2 *values, int count)
{
  if (loc != -1) {
    glUniform2fv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::vec3 *values, int count)
{
  if (loc != -1) {
    glUniform3fv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::vec4 *values, int count)
{
  if (loc != -1) {
    glUniform4fv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::ivec2 *values, int count)
{
  if (loc != -1) {
    glUniform2iv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::ivec3 *values, int count)
{
  if (loc != -1) {
    glUniform3iv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::ivec4 *values, int count)
{
  if (loc != -1) {
    glUniform4iv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::uvec2 *values, int count)
{
  if (loc != -1) {
    glUniform2uiv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::uvec3 *values, int count)
{
  if (loc != -1) {
    glUniform3uiv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::uvec4 *values, int count)
{
  if (loc != -1) {
    glUniform4uiv(loc, count, &(values[0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::vec2 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::vec3 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::vec4 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::ivec2 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::ivec3 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::ivec4 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::uvec2 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::uvec3 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::uvec4 *values, int count)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count);
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::mat2 &value, bool transpose)
{
  if (loc != -1) {
    glUniformMatrix2fv(loc, 1, transpose, &value[0][0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::mat3 &value, bool transpose)
{
  if (loc != -1) {
    glUniformMatrix3fv(loc, 1, transpose, &value[0][0]);
  }
}

void Z3DShaderProgram::setUniformValue(GLint loc, const glm::mat4 &value, bool transpose)
{
  if (loc != -1) {
    glUniformMatrix4fv(loc, 1, transpose, &value[0][0]);
  }
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::mat2 &value, bool transpose)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value, transpose);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::mat3 &value, bool transpose)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value, transpose);
}

void Z3DShaderProgram::setUniformValue(const QString &name, const glm::mat4 &value, bool transpose)
{
  GLint l = getUniformLocation(name);
  setUniformValue(l, value, transpose);
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::mat2 *values, int count, bool transpose)
{
  if (loc != -1) {
    glUniformMatrix2fv(loc, count, transpose, &(values[0][0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::mat3 *values, int count, bool transpose)
{
  if (loc != -1) {
    glUniformMatrix3fv(loc, count, transpose, &(values[0][0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(GLint loc, const glm::mat4 *values, int count, bool transpose)
{
  if (loc != -1) {
    glUniformMatrix4fv(loc, count, transpose, &(values[0][0][0]));
  }
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::mat2 *values, int count, bool transpose)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count, transpose);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::mat3 *values, int count, bool transpose)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count, transpose);
}

void Z3DShaderProgram::setUniformValueArray(const QString &name, const glm::mat4 *values, int count, bool transpose)
{
  GLint l = getUniformLocation(name);
  setUniformValueArray(l, values, count, transpose);
}
