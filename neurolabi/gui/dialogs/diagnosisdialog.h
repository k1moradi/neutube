#ifndef DIAGNOSISDIALOG_H
#define DIAGNOSISDIALOG_H

#include <QDialog>
#include <memory>              // std::unique_ptr

class QTextBrowser;            // fwd decl OK
class QFileSystemWatcher;      // fwd decl OK

namespace Ui {
  class DiagnosisDialog;
}

class DiagnosisDialog : public QDialog
{
  Q_OBJECT
public:
  explicit DiagnosisDialog(QWidget *parent = 0);
  ~DiagnosisDialog();

  void reloadAll();
  void scrollToBottom();
  void setVideoCardInfo(const QString &str);
  void setVideoCardInfo(const QStringList &info);

public slots:
  void scrollToBottom(int index);
  void reloadCurrentTab();
  void onFilesChanged(const QString &path);

private:
  void loadErrorFile();
  void loadWarnFile();
  void loadInfoFile();
  void loadFileIntoBrowser(const QString &filePath, QTextBrowser *tb);

private:
  Ui::DiagnosisDialog *ui;

  // Cached absolute paths (so we always point at the same files)
  QString m_errorPath;
  QString m_warnPath;
  QString m_infoPath;

  std::unique_ptr<QFileSystemWatcher> m_watcher;
};

#endif // DIAGNOSISDIALOG_H
