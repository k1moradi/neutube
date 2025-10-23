#ifndef ZFSPATH_H
#define ZFSPATH_H

#include <string>
#include <QString>
#include <QDir>

namespace fs {
  class path {
    std::string m_path;
  public:
    path() = default;
    path(const std::string& s) { assign(s); }
    path(const char* s)        { assign(std::string(s)); }
    path(const QString& s)     { assign(s.toStdString()); }

    void assign(const std::string& s) {
      // normalize to forward slashes internally
      m_path = QDir::fromNativeSeparators(QString::fromStdString(s)).toStdString();
    }
    path operator/(const path& other) const {
      QDir d(QString::fromStdString(m_path));
      return path(d.filePath(QString::fromStdString(other.m_path)).toStdString());
    }
    std::string string() const { return m_path; }
    const char* c_str() const { return m_path.c_str(); }
  };
}
inline std::string fs_string(const fs::path& p) { return p.string(); }
inline const char* fs_c_str(const fs::path& p) { return p.c_str(); }

#endif // ZFSPATH_H
