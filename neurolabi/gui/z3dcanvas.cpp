#include "z3dcanvas.h"

#include <QPainter>
#include <QWindow>
#include "QsLog/QsLog.h"

#include <algorithm>

#include "z3dnetworkevaluator.h"
#include "z3dcanvaseventlistener.h"
#include "zpainter.h"
#include "zstackdrawable.h"
#include "zglviewport.h"

Z3DCanvas::Z3DCanvas(const QString &title, int width, int height, QWidget* parent, Qt::WindowFlags f)
  : QGraphicsView(parent)
  , m_fullscreen(false)
  , m_glWidget(NULL)
  , m_3dScene(NULL)
  , m_networkEvaluator(NULL)
  , m_fakeStereoOnce(false)
{
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  resize(width, height);
  m_glWidget = new ZGLViewport(this, f);     // parent the viewport to the view
  m_isStereoScene = m_glWidget->format().stereo();
  m_3dScene = new QGraphicsScene(0, 0, width, height, this);
  setViewport(m_glWidget);
  setViewportUpdateMode(FullViewportUpdate);
  setScene(m_3dScene);
  setWindowTitle(title);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
  setAcceptDrops(true);
  setFocusPolicy(Qt::StrongFocus);
#ifndef __APPLE__
  setStyleSheet("border-style: none;");
#endif
  setMouseTracking(true);
  connect(&m_interaction, SIGNAL(decorationUpdated()), this->viewport(), SLOT(update()));
  connect(&m_interaction, SIGNAL(strokePainted(ZStroke2d*)), this, SIGNAL(strokePainted(ZStroke2d*)));
}

Z3DCanvas::~Z3DCanvas() {}

void Z3DCanvas::toggleFullScreen()
{
  if (m_fullscreen) {
    m_fullscreen = false;
    showNormal();
  } else {
    showFullScreen();
    m_fullscreen = !m_fullscreen;
  }
}

void Z3DCanvas::enterEvent(QEvent* e)
{
  broadcastEvent(e, width(), height());
}

void Z3DCanvas::leaveEvent(QEvent* e)
{
  broadcastEvent(e, width(), height());
}

void Z3DCanvas::mousePressEvent(QMouseEvent* e)
{
  broadcastEvent(e, width(), height());
  m_interaction.processMousePressEvent(e);
}

bool Z3DCanvas::suppressingContextMenu() const
{
  if (m_interaction.isStateOn(ZInteractionEngine::STATE_DRAW_STROKE) ||
      m_interaction.isStateOn(ZInteractionEngine::STATE_DRAW_RECT)) {
    return true;
  }
  return false;
}

void Z3DCanvas::mouseReleaseEvent (QMouseEvent* e)
{
  broadcastEvent(e, width(), height());
  m_interaction.processMouseReleaseEvent(e);
  setCursor(m_interaction.getCursorShape());
}

void Z3DCanvas::mouseMoveEvent(QMouseEvent*  e)
{
  m_interaction.processMouseMoveEvent(e);
  if (!m_interaction.lockingMouseMoveEvent()) {
    broadcastEvent(e, width(), height());
  }
}

void Z3DCanvas::mouseDoubleClickEvent(QMouseEvent* e)
{
  broadcastEvent(e, width(), height());
}

void Z3DCanvas::wheelEvent(QWheelEvent* e)
{
  broadcastEvent(e, width(), height());
}

void Z3DCanvas::keyPressEvent(QKeyEvent* event)
{
  if (!m_interaction.processKeyPressEvent(event)) {
    broadcastEvent(event, width(), height());
  }
  setCursor(m_interaction.getCursorShape());
}

void Z3DCanvas::keyReleaseEvent(QKeyEvent* event)
{
  broadcastEvent(event, width(), height());
}

void Z3DCanvas::resizeEvent(QResizeEvent *event)
{
  CHECK_GL_ERROR_NOTE("Z3DCanvas::resizeEvent start");
  getGLFocus();
  QGraphicsView::resizeEvent(event);
  if (m_3dScene) m_3dScene->setSceneRect(QRect(QPoint(0, 0), event->size()));
  emit canvasSizeChanged(event->size().width() * getDevicePixelRatio(), event->size().height() * getDevicePixelRatio());
  CHECK_GL_ERROR_NOTE("Z3DCanvas::resizeEvent end");
}

void Z3DCanvas::paintEvent(QPaintEvent *event)
{
  getGLFocus();
  QGraphicsView::paintEvent(event);
}

void Z3DCanvas::dragEnterEvent(QDragEnterEvent *event)
{
  event->ignore();
}

void Z3DCanvas::dropEvent(QDropEvent *event)
{
  event->ignore();
}

void Z3DCanvas::drawBackground(QPainter *painter, const QRectF &)
{
  CHECK_GL_ERROR_NOTE("Z3DCanvas::drawBackground start");
  if (!m_networkEvaluator) {
    return;
  }
  // QPainter set glclearcolor to white, we set it back
  glClearColor(0.f, 0.f, 0.f, 0.f);
  m_networkEvaluator->process(m_isStereoScene || m_fakeStereoOnce);
  m_fakeStereoOnce = false;
  QList<ZStackObject*> drawableList = m_interaction.getDecorationList();
  foreach (ZStackObject *drawable, drawableList) {
    drawable->display(painter, 0, ZStackObject::NORMAL, ZStackObject::DISPLAY_SLICE_SINGLE, NeuTube::Z_AXIS);
  }
  if (m_interaction.getKeyMode() == ZInteractionEngine::KM_SWC_SELECTION) {
    painter->setPen(QColor(255, 255, 255));
    QFont font("Helvetica [Cronyx]", 24);;
    painter->setFont(font);
    painter->drawText(
          QRectF(10, 10, 300, 200),
          "Selection mode on: \n"
          "  1: downstream;\n"
          "  2: upstream;\n"
          "  3: connected nodes;\n"
          "  4: inverse selection;\n"
          "  5: select small trees;");
  }
  CHECK_GL_ERROR_NOTE("Z3DCanvas::drawBackground end");
}

void Z3DCanvas::timerEvent(QTimerEvent* e)
{
  broadcastEvent(e, width(), height());
}

void Z3DCanvas::setNetworkEvaluator(Z3DNetworkEvaluator *n)
{
  m_networkEvaluator = n;
  if (n)
    n->setOpenGLContext(this);
}

void Z3DCanvas::setFakeStereoOnce()
{
  m_fakeStereoOnce = true;
}

void Z3DCanvas::addEventListenerToBack(Z3DCanvasEventListener *e)
{
  if (e)
    m_listeners.push_back(e);
}

void Z3DCanvas::addEventListenerToFront(Z3DCanvasEventListener *e)
{
  if (e)
    m_listeners.push_front(e);
}

void Z3DCanvas::removeEventListener(Z3DCanvasEventListener *e)
{
  std::deque<Z3DCanvasEventListener*>::iterator pos;
  pos = std::find(m_listeners.begin(), m_listeners.end(), e);

  if (pos != m_listeners.end())
    m_listeners.erase(pos);
}

void Z3DCanvas::clearEventListeners()
{
  m_listeners.clear();
}

void Z3DCanvas::broadcastEvent(QEvent *e, int w, int h)
{
  getGLFocus();
  for (size_t i = 0 ; i < m_listeners.size() ; ++i) {
    m_listeners[i]->onEvent(e, w, h);
    if (e->isAccepted()) {
      break;
    }
  }
}

void Z3DCanvas::setKeyMode(ZInteractionEngine::EKeyMode mode)
{
  m_interaction.setKeyMode(mode);
  update(QRect(QPoint(0, 0), size()));
}

double Z3DCanvas::getDevicePixelRatio()
{
  return (window() && window()->windowHandle()) ? window()->windowHandle()->devicePixelRatio() : 1.0;
}

void Z3DCanvas::disableKeyEvent()
{
  m_interaction.setKeyEventEnabled(false);
}

void Z3DCanvas::set3DInteractionHandler(Z3DTrackballInteractionHandler *handler)
{
  m_interaction.set3DInteractionHandler(handler);
}

void Z3DCanvas::updateCursor()
{
  setCursor(m_interaction.getCursorShape());
}

