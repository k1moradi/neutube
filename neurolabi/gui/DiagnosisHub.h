// DiagnosisHub.h
#pragma once
#include <QObject>
#include <QsLog.h>
#include <QsLogDest.h>

class DiagnosisHub : public QObject {
    Q_OBJECT
  public:
    static DiagnosisHub& instance() {
        static DiagnosisHub h; return h;
    }

    // Called by logger destination (any thread)
    void push(QsLogging::Level lvl, const QString& msg) {
        // Hop to GUI thread safely
        switch (lvl) {
        case QsLogging::LevelError:
        case QsLogging::LevelFatal:
            emit error(msg);    break;
        case QsLogging::LevelWarn:
            emit warning(msg);  break;
        default:
            emit info(msg);     break;   // General tab
        }
    }

    signals:
      void error(const QString&);    // connect to Diagnosis/Error tab appender
    void warning(const QString&);  // connect to Diagnosis/Warning tab appender
    void info(const QString&);     // connect to Diagnosis/General tab appender

private:
    DiagnosisHub() = default;
};

class DiagnosisDestination : public QObject, public QsLogging::Destination {
    Q_OBJECT
  public:
    explicit DiagnosisDestination(QObject* parent = nullptr) : QObject(parent) {}
    bool isValid() override { return true; }
    void write(const QString& message, QsLogging::Level level) override {
        DiagnosisHub::instance().push(level, message);
    }
};

void setupLoggingToDiagnosisUI() {
    using namespace QsLogging;
    Logger& logger = Logger::instance();

    // keep your existing file destinations (already set by ZLogMessageReporter)
    // add a UI destination
    DestinationPtr uiDest(new DiagnosisDestination());
    logger.addDestination(uiDest);
}