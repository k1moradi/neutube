#include "ztraceproject.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "tz_string.h"
#include "tz_utilities.h"
#include "tz_xml_utils.h"
#include "zfiletype.h"
#include "zstack.hxx"
#include "zstackdoc.h"
#include "zstackframe.h"
ZTraceProject::ZTraceProject(ZStackFrame* parent) {
  m_parent = parent;
}
ZTraceProject::~ZTraceProject() {
}
int ZTraceProject::load(const QString /*path*/) {
  return 0;
}
void ZTraceProject::save() {
  QString filePath = m_projFilePath;
  QDir workDir(rootPath());
  if(!workDir.exists()) {
    workDir.mkpath(rootPath());
  }
  QDir tubeDir(m_workspacePath);
  if(tubeDir.exists()) {
    QDir bkTubeDir(rootPath() + "/" + "~" + tubeDir.dirName());
    if(bkTubeDir.exists()) {
      rmpath(bkTubeDir.absolutePath().toLocal8Bit().constData());
    }
    tubeDir.rename(tubeDir.absolutePath(), bkTubeDir.absolutePath());
  }
  workDir.mkdir(tubeFolder());
  qDebug() << filePath;
  QFile file(filePath);
  file.open(QIODevice::WriteOnly);
  QTextStream out(&file);
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << Qt::endl;
  out << "<trace version = \"1.0\">" << Qt::endl;
  out << "<data>" << Qt::endl;
  out << "<image type=";
  const ZStackFile& stackSource = m_parent->document()->getStack()->source();
  // Stack_Document *stackSource = m_parent->document()->stack()->source();
  switch(stackSource.type()) {
  case ZStackFile::SINGLE_FILE: {
    switch(ZFileType::fileType(stackSource.firstUrl())) {
    case ZFileType::TIFF_FILE:
      out << "\"tif\">" << Qt::endl;
      out << "<url>" << stackSource.firstUrl().c_str() << "</url>" << Qt::endl;
      break;
    case ZFileType::LSM_FILE:
      out << "\"lsm\">" << Qt::endl;
      out << "<url>" << stackSource.firstUrl().c_str() << "</url>" << Qt::endl;
      ;
      break;
    case ZFileType::V3D_RAW_FILE:
      out << "\"raw\">" << Qt::endl;
      out << "<url>" << stackSource.firstUrl().c_str() << "</url>" << Qt::endl;
      ;
      break;
    default:
      out << "unknown>" << stackSource.firstUrl().c_str() << Qt::endl;
    }
  } break;
  case ZStackFile::FILE_BUNDLE: {
    out << "\"bundle\">" << Qt::endl;
    out << "<prefix>" << stackSource.prefix().c_str() << "</prefix>" << Qt::endl;
    out << "<suffix>" << stackSource.suffix().c_str() << "</suffix" << Qt::endl;
    out << "<num_width>" << stackSource.numWidth() << "</num_width>" << Qt::endl;
    out << "<first_num>" << stackSource.firstNum() << "</first_num>" << Qt::endl;
  } break;
  default:
    out << "unknown>" << stackSource.firstUrl().c_str() << Qt::endl;
  }
  out << "</image>" << Qt::endl;
  ZResolution resolution = m_parent->document()->getResolution();
  out << "<resolution>" << "<x>" << resolution.voxelSizeX() << "</x>"
      << "<y>" << resolution.voxelSizeY() << "</y>"
      << "<z>" << resolution.voxelSizeZ() << "</z>"
      << "</resolution>" << Qt::endl;
  out << "<unit>" << resolution.unit() << "</unit>" << Qt::endl;
  out << "<channel>" << stackSource.channel() << "</channel>" << Qt::endl;
  out << "</data>" << Qt::endl;
  out << "<output>" << Qt::endl;
  out << "<workdir>" << m_workspacePath << "</workdir>" << Qt::endl;
  if(m_parent->document()->tubePrefix() != NULL) {
    out << "<tube>" << m_parent->document()->tubePrefix() << "</tube>" << Qt::endl;
  }
  out << "</output>" << Qt::endl;
  if(!m_decorationPath.isEmpty()) {
    out << "<object>" << Qt::endl;
    for(int i = 0; i < m_decorationPath.length(); i++) {
      out << "<" << m_decorationTag.at(i) << ">";
      out << m_decorationPath.at(i);
      out << "</" << m_decorationTag.at(i) << ">" << Qt::endl;
    }
    out << "</object>" << Qt::endl;
  }
  out << "</trace>" << Qt::endl;
  file.close();
  m_parent->document()->exportBinary((m_workspacePath + "/" + m_parent->document()->tubePrefix()).toLocal8Bit().constData());
}
void ZTraceProject::saveAs(QString workspacePath) {
  m_workspacePath = workspacePath + "/traced";
  m_projFilePath = workspacePath + "/project.xml";
  save();
}
void ZTraceProject::setWorkDir(const char* workDir) {
  m_workspacePath = QDir(workDir).canonicalPath();
}
void ZTraceProject::setProjFilePath(const char* path) {
  m_projFilePath = path;
}
void ZTraceProject::addDecoration(const QString& path, const QString& tag) {
  m_decorationPath.append(path);
  m_decorationTag.append(tag);
}
QString ZTraceProject::rootPath() const {
  return QDir(m_workspacePath + "/../").absolutePath();
}
QString ZTraceProject::tubeFolder() const {
  return QDir(m_workspacePath).dirName();
}
