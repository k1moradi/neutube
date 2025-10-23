#ifndef NEUTUBE_ZGLVIEWPORT_H
#define NEUTUBE_ZGLVIEWPORT_H

#include <QOpenGLWidget>

class ZGLViewport : public QOpenGLWidget {
    Q_OBJECT
  public:
    explicit ZGLViewport(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
      : QOpenGLWidget(parent) { Q_UNUSED(f); }

    signals:
      void glReady();   // emitted after Qt makes the GL context current for this widget

protected:
    void initializeGL() override {
        emit glReady();
    }
};

#endif // NEUTUBE_ZGLVIEWPORT_H
