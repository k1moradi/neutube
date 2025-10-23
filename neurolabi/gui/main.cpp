// ===== main.cpp (Qt5-only, restored essentials) =====
#include <iostream>
#include <cstring>

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSurfaceFormat>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QStack>
#include <QPointer>
#include "QsLog/QsLog.h"
#include "QsLog/QsLogDest.h"

#include "mainwindow.h"
#include "zcommandline.h"
#include "zerror.h"
#include "z3dapplication.h"
#include "zneurontracer.h"
#include "tz_utilities.h"
#include "neutubeconfig.h"
#include "zneurontracerconfig.h"
#ifdef _FLYEM_
  #include "zflyem/ztest.h"
#else
  #include "ztest.h"
#endif

// -------- macOS dialog activation workaround (from your original) --------
struct MacEventFilter : public QObject
{
  QStack<QPointer<QWidget> > m_activationstack;

  explicit MacEventFilter(QObject *parent = NULL) : QObject(parent) {}

  bool eventFilter(QObject *obj, QEvent *ev) override
  {
    switch (ev->type()) {
    case QEvent::Show:
      if ((obj->inherits("QDialog") || obj->inherits("QDockWidget")) && qApp->activeWindow()) {
        m_activationstack.push(qApp->activeWindow());
      }
      break;
    case QEvent::Hide:
      if ((obj->inherits("QDialog") || obj->inherits("QDockWidget")) && !m_activationstack.isEmpty()) {
        QPointer<QWidget> w = m_activationstack.pop();
        if (w) { w->activateWindow(); w->raise(); }
      }
      break;
    default: break;
    }
    return QObject::eventFilter(obj, ev);
  }
};

// -------- optional Qt → QsLog bridge (kept disabled by default) --------
static void myMessageOutput(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
  switch (type) {
  case QtDebugMsg:    LDEBUGF(ctx.file, ctx.line, ctx.function) << msg; break;
  case QtInfoMsg:     LINFOF (ctx.file, ctx.line, ctx.function) << msg; break;
  case QtWarningMsg:  LWARNF (ctx.file, ctx.line, ctx.function) << msg; break;
  case QtCriticalMsg: LERRORF(ctx.file, ctx.line, ctx.function) << msg; break;
  case QtFatalMsg:    LFATALF(ctx.file, ctx.line, ctx.function) << msg; abort();
  }
}

// ----- Level-filtered file destination (no Qt signals involved) -----
class LevelFileDestination : public QsLogging::Destination {
public:
  LevelFileDestination(QString path, QsLogging::Level minL, QsLogging::Level maxL)
    : m_filePath(std::move(path)), m_min(minL), m_max(maxL) {}
  void write(const QString& message, QsLogging::Level level) override {
    if (level < m_min || level > m_max) return;
    QFile f(m_filePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
      QTextStream ts(&f);
      ts << message << '\n';
    }
  }
  bool isValid() override {
    // Consider it valid if the parent directory exists (we created it earlier)
    return !m_filePath.isEmpty();
  }
private:
  QString m_filePath;
  QsLogging::Level m_min, m_max;
};

// -------- helper: configure default OpenGL surface format --------
static void configureSurfaceFormat()
{
  QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
  fmt.setRenderableType(QSurfaceFormat::OpenGL);
  fmt.setVersion(4, 1);
  //fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
  fmt.setDepthBufferSize(24);
  fmt.setStencilBufferSize(8);
#ifdef _DEBUG_
  fmt.setOption(QSurfaceFormat::DebugContext, true);
#endif
  QSurfaceFormat::setDefaultFormat(fmt);
}

// ----- Level-filtered file sink for QsLog -----
class LevelFileSink : public QObject {
  Q_OBJECT
public:
  LevelFileSink(QString path, QsLogging::Level minL, QsLogging::Level maxL)
      : m_path(std::move(path)), m_min(minL), m_max(maxL) {}
  Q_SLOT void onLogMessage(const QString& formatted, QsLogging::Level lvl) {
    if (lvl < m_min || lvl > m_max) return;
    QFile f(m_path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
      QTextStream ts(&f);
      ts << formatted << '\n';
    }
  }

private:
  QString m_path;
  QsLogging::Level m_min, m_max;
};

// -------- entry point --------
int main(int argc, char *argv[])
{
  // These attributes must be set BEFORE QApplication is constructed.
  QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
  QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  configureSurfaceFormat();
  bool runCommandLine = false;
  bool runUnitTest    = false;
  QStringList fileList; // files to open in GUI
  // Light CLI parsing compatible with your previous behavior
  for (int i = 1; i < argc; ++i) {
    const QString a = argv[i];
    if (a == "--help") {
      std::cout <<
        "Usage: " << argv[0] << " [--help] [--test] [--command] [--load <files...>] [files...]\n"
        "  --help      Show this help\n"
        "  --test      Run unit tests and exit (no GUI)\n"
        "  --command   Run command-line mode and exit\n"
        "  --load f1 f2 ...  Open files on startup\n";
      return 0;
    } else if (a == "--test") {
      runUnitTest = true;
    } else if (a == "--command") {
      runCommandLine = true;
      break; // zcommandline parses the rest
    } else if (a == "--load") {
      for (int j = i + 1; j < argc; ++j) fileList << argv[j];
      break;
    } else if (!a.startsWith("-")) {
      QFileInfo fi(a); if (fi.exists() && fi.isFile()) fileList << fi.absoluteFilePath();
    }
  }
  if (runCommandLine) {
    ZCommandLine cmd;
    return cmd.run(argc, argv);
  }
  if (runUnitTest) {
    return ZTest::runUnitTest(argc, argv);
  }
  // GUI application
  QApplication app(argc, argv);
  // Load configuration (must happen before 3D init)
  NeutubeConfig &config = NeutubeConfig::getInstance();
  config.setApplicationDir(QApplication::applicationDirPath().toStdString());
  if (!config.load(config.getConfigPath())) {
    std::cout << "Unable to load configuration: " << config.getConfigPath() << std::endl;
  }
  // Load tracer config (legacy behavior)
  if (GET_APPLICATION_NAME == "Biocytin") {
    ZNeuronTracerConfig::getInstance().load(config.getApplicatinDir() + "/json/trace_config_biocytin.json");
  } else {
    ZNeuronTracerConfig::getInstance().load(config.getApplicatinDir() + "/json/trace_config.json");
  }
  // ---------- Logging: three separate files (General/Warning/Error) ----------
  QsLogging::Logger& logger = QsLogging::Logger::instance();
  // ensure directories exist
  auto ensureDir = [](const QString& filePath) {
    QFileInfo fi(filePath);
    QDir d = fi.dir();
    if (!d.exists()) d.mkpath(".");
  };
  auto truncateFile = [](const QString& filePath) {
    QFileInfo fi(filePath);
    if (fi.exists()) QFile::remove(filePath);
    QFile f(filePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
      f.close();
    }
  };
  // General (Trace..Info) -> log_appout.txt
  {
    const QString path = QString::fromStdString(NeutubeConfig::getInstance().getPath(NeutubeConfig::LOG_APPOUT));
    ensureDir(path);
    truncateFile(path);
    QsLogging::DestinationPtr d(new LevelFileDestination(path, QsLogging::TraceLevel, QsLogging::InfoLevel));
    logger.addDestination(d);
  }
  // Warnings only -> log_warn.txt
  {
    const QString path = QString::fromStdString(NeutubeConfig::getInstance().getPath(NeutubeConfig::LOG_WARN));
    ensureDir(path);
    truncateFile(path);
    QsLogging::DestinationPtr d(new LevelFileDestination(path, QsLogging::WarnLevel, QsLogging::WarnLevel));
    logger.addDestination(d);
  }
  // Errors & fatals -> log_error.txt
  {
    const QString path = QString::fromStdString(NeutubeConfig::getInstance().getPath(NeutubeConfig::LOG_ERROR));
    ensureDir(path);
    truncateFile(path);
    QsLogging::DestinationPtr d(new LevelFileDestination(path, QsLogging::ErrorLevel, QsLogging::FatalLevel));
    logger.addDestination(d);
  }
  // Optional developer mirror to debugger/IDE
  logger.addDestination(QsLogging::DestinationFactory::MakeDebugOutputDestination());
#ifdef _DEBUG_
  logger.setLoggingLevel(QsLogging::DebugLevel);
#else
  logger.setLoggingLevel(QsLogging::InfoLevel);
#endif
  RECORD_INFORMATION("************* Start ******************");
  // Route Qt’s qDebug/qWarning/... into L* macros (which use QsLog levels)
  qInstallMessageHandler(myMessageOutput);
#if defined(__APPLE__)
  qApp->installEventFilter(new MacEventFilter(qApp));
#endif
  LINFO() << "Start " + GET_SOFTWARE_NAME + " - " + GET_APPLICATION_NAME;
  {
    const QStringList fullApplicationArgumentList = QCoreApplication::arguments();
    QStringList quotedAndEscapedArgumentList;
    quotedAndEscapedArgumentList.reserve(fullApplicationArgumentList.size());
    for (const QString &originalArgument : fullApplicationArgumentList) {
      QString escapedArgument = originalArgument;
      escapedArgument.replace(QStringLiteral("\""), QStringLiteral("\\\""));
      const bool containsWhitespace = escapedArgument.contains(QLatin1Char(' '));
      const bool containsQuote      = escapedArgument.contains(QLatin1Char('"'));
      if (containsWhitespace || containsQuote) {
        quotedAndEscapedArgumentList << QStringLiteral("\"%1\"").arg(escapedArgument);
      } else {
        quotedAndEscapedArgumentList << escapedArgument;
      }
    }
    const QString reconstructedCommandLine = quotedAndEscapedArgumentList.join(QLatin1Char(' '));
    const QString currentWorkingDirectoryPath = QDir::currentPath();
    LINFO() << "Command line argument count:" << fullApplicationArgumentList.size();
    LINFO() << "Command line (reconstructed):" << reconstructedCommandLine;
    LINFO() << "Current working directory:" << currentWorkingDirectoryPath;
  }
  // -------- 3D subsystem (critical for avoiding “3D unavailable”) --------
  RECORD_INFORMATION("Initializing 3D ...");
  Z3DApplication z3dApp(QCoreApplication::applicationDirPath());
  z3dApp.initialize();
  // -------- Main window --------
  MainWindow::createWorkDir();
  MainWindow *mainWin = new MainWindow();
  mainWin->configure();
  mainWin->show();
  mainWin->raise();
  if (!fileList.isEmpty()) {
    QTimer::singleShot(0, mainWin, [mainWin, fileList]() {
      mainWin->openFile(fileList);
    });
  }
  const int result = app.exec();
  delete mainWin;
  z3dApp.deinitializeGL();
  z3dApp.deinitialize();
  return result;
}

#include "main.moc"
