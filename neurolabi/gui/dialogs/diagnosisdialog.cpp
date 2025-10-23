#include "diagnosisdialog.h"
#include "ui_diagnosisdialog.h"

#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QScrollBar>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextStream>
#include <QTabWidget>

#include "neutube.h"  // NeuTube::getErrorFile/getWarnFile/getInfoFile

DiagnosisDialog::DiagnosisDialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::DiagnosisDialog)
  , m_watcher(new QFileSystemWatcher(this))
{
  ui->setupUi(this);

  // Paths must match what main.cpp writes
  m_errorPath = QString::fromStdString(NeuTube::getErrorFile());
  m_warnPath  = QString::fromStdString(NeuTube::getWarnFile());
  m_infoPath  = QString::fromStdString(NeuTube::getInfoFile());

  // Initial load
  reloadAll();

  // Watch the three files for live updates
  auto addIfExists = [&](const QString& p){
    if (!p.isEmpty() && QFileInfo::exists(p)) m_watcher->addPath(p);
  };
  addIfExists(m_errorPath);
  addIfExists(m_warnPath);
  addIfExists(m_infoPath);

  connect(m_watcher.get(), &QFileSystemWatcher::fileChanged,
          this, &DiagnosisDialog::onFilesChanged);

  if (ui->tabWidget) {
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &DiagnosisDialog::reloadCurrentTab);
  }
}

DiagnosisDialog::~DiagnosisDialog()
{
  delete ui;
}

static void loadFileInto(const QString& path, QTextBrowser* tb)
{
  if (!tb) return;
  QFile f(path);
  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream ts(&f);
    tb->setPlainText(ts.readAll());
  } else {
    tb->setPlainText(QString("[Cannot open: %1]").arg(path));
  }
}

void DiagnosisDialog::loadErrorFile()
{
  loadFileInto(m_errorPath, ui->errorTextBrowser);
}

void DiagnosisDialog::loadWarnFile()
{
  loadFileInto(m_warnPath, ui->warnTextBrowser);
}

void DiagnosisDialog::loadInfoFile()
{
  loadFileInto(m_infoPath, ui->infoTextBrowser);
}

void DiagnosisDialog::scrollToBottom(int index)
{
  auto scrollTB = [](QTextBrowser* tb){
    if (!tb) return;
    if (auto* sb = tb->verticalScrollBar())
      sb->setValue(sb->maximum());
  };
  switch (index) {
    case 0: scrollTB(ui->errorTextBrowser); break;
    case 1: scrollTB(ui->warnTextBrowser);  break;
    case 2: scrollTB(ui->infoTextBrowser);  break;
    default: break;
  }
}

void DiagnosisDialog::scrollToBottom()
{
  scrollToBottom(0);
  scrollToBottom(1);
  scrollToBottom(2);
}

void DiagnosisDialog::setVideoCardInfo(const QString &str)
{
  if (ui->videoCardInfoTextEdit)
    ui->videoCardInfoTextEdit->setPlainText(str);
}

void DiagnosisDialog::setVideoCardInfo(const QStringList &info)
{
  setVideoCardInfo(info.join("\n"));
}

void DiagnosisDialog::reloadAll()
{
  loadErrorFile();
  loadWarnFile();
  loadInfoFile();
  if (ui->errorTextBrowser) ui->errorTextBrowser->moveCursor(QTextCursor::End);
}

void DiagnosisDialog::reloadCurrentTab()
{
  if (!ui->tabWidget) return;
  switch (ui->tabWidget->currentIndex()) {
    case 0: loadErrorFile(); break;
    case 1: loadWarnFile();  break;
    case 2: loadInfoFile();  break;
    default: break;
  }
}

void DiagnosisDialog::onFilesChanged(const QString &path)
{
  // Re-add watch (some backends drop it on change/rotate)
  if (QFileInfo::exists(path)) m_watcher->addPath(path);

  if (path == m_errorPath)      loadErrorFile();
  else if (path == m_warnPath)  loadWarnFile();
  else if (path == m_infoPath)  loadInfoFile();
}
