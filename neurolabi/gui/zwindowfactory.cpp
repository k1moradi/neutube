#include "zwindowfactory.h"
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QMessageBox>
#include "neutubeconfig.h"
#include "z3dapplication.h"
#include "z3dcanvas.h"
#include "z3dcompositor.h"
#include "z3dpunctafilter.h"
#include "z3dswcfilter.h"
#include "z3dutils.h"
#include "z3dvolume.h"
#include "z3dvolumeraycaster.h"
#include "z3dvolumesource.h"
#include "z3dwindow.h"
#include "zscalablestack.h"
ZWindowFactory::ZWindowFactory() {  init(); }
ZWindowFactory::~ZWindowFactory() {}
void ZWindowFactory::init() {
  m_parentWidget = NULL;
  m_showVolumeBoundBox = false;
  m_showControlPanel = true;
  m_showObjectView = true;
  m_volumeMode = NeuTube3D::VR_AUTO;
}
Z3DWindow* ZWindowFactory::make3DWindow(
  ZStackDoc* doc, Z3DWindow::EInitMode mode) {
  ZSharedPointer<ZStackDoc> sharedDoc(doc);
  return make3DWindow(sharedDoc, mode);
}
Z3DWindow* ZWindowFactory::open3DWindow(
  ZStackDoc* doc, Z3DWindow::EInitMode mode) {
  Z3DWindow* window = make3DWindow(doc, mode);
  window->show();
  window->raise();
  return window;
}
Z3DWindow* ZWindowFactory::make3DWindow(ZSharedPointer<ZStackDoc> doc,
  Z3DWindow::EInitMode mode) {
  if(Z3DApplication::app() == NULL) {
    QMessageBox::information(
      NULL, "3D Unavailable", "The 3D visualization is unavailable in this"
                              "plug-in because of some technical problems. To obtain a "
                              "fully-functioing version of neuTube, because visit "
                              "<a href=www.neutracing.com>www.neutracing.com</a>");
    return NULL;
  }
  Z3DWindow* window = NULL;
  if(doc) {
    window = new Z3DWindow(doc, mode, false, m_parentWidget);
    if(m_windowTitle.isEmpty()) {
      window->setWindowTitle("3D View");
    } else {
      window->setWindowTitle(m_windowTitle);
    }
    if(!NeutubeConfig::getInstance().getZ3DWindowConfig().isBackgroundOn()) {
      window->getCompositor()->setShowBackground(false);
    }
    if(doc->getTag() == NeuTube::Document::FLYEM_SPLIT) {
      window->getSwcFilter()->setRenderingPrimitive("Sphere");
      window->getPunctaFilter()->setColorMode("Original Point Color");
    }
    if(m_volumeMode == NeuTube3D::VR_AUTO) {
      if(doc->getTag() == NeuTube::Document::FLYEM_BODY ||
         doc->getTag() == NeuTube::Document::FLYEM_SPLIT) {
        window->getVolumeRaycasterRenderer()->setCompositeMode(
          "Direct Volume Rendering");
      } else {
        window->getVolumeRaycasterRenderer()->setCompositeMode("MIP Opaque");
      }
    } else {
      window->getVolumeRaycasterRenderer()->setCompositeMode(
        Z3DUtils::GetVolumeRenderingName(m_volumeMode).c_str());
    }
    if(doc->getTag() != NeuTube::Document::FLYEM_SPLIT &&
       doc->getTag() != NeuTube::Document::SEGMENTATION_TARGET &&
       doc->getTag() != NeuTube::Document::FLYEM_PROOFREAD) {
    }
    window->setZScale(doc->getPreferredZScale());
    if(!m_showVolumeBoundBox) {
      window->getVolumeRaycaster()->hideBoundBox();
    }
    if(m_windowGeometry.isEmpty()) {
      // Prefer the parent widget's screen; fall back to the window's, then primary.
      QScreen* screen =
          (m_parentWidget && m_parentWidget->screen())
              ? m_parentWidget->screen()
              : (window->screen() ? window->screen() : QGuiApplication::primaryScreen());

      // If no screen is available (very unlikely), use a sensible default rect.
      const QRect screenRect = screen ? screen->availableGeometry() : QRect(0, 0, 1280, 800);
      const int w = screenRect.width();
      const int h = screenRect.height();
      const int x = screenRect.x() + w / 10;
      const int y = screenRect.y() + h / 10;

      window->setGeometry(x, y, w - w / 5, h - h / 5);
    } else {
      window->setGeometry(m_windowGeometry);
    }
    for(QMap<Z3DWindow::ERendererLayer, bool>::const_iterator iter = m_layerVisible.begin(); iter != m_layerVisible.end(); ++iter) {
      window->setVisible(iter.key(), iter.value());
    }
    if(!isControlPanelVisible()) {
      window->hideControlPanel();
    }
    if(!isObjectViewVisible()) {
      window->hideObjectView();
    }
    configure(window);
  }
  return window;
}
Z3DWindow* ZWindowFactory::make3DWindow(ZScalableStack* stack) {
  if(!stack) return nullptr;
  // Capture metadata before releasing the stack
  const glm::vec3 scale(stack->getXScale(), stack->getYScale(), stack->getZScale());
  const ZPoint offset = stack->getOffset();
  // Build a document from the stack (no GL work here)
  ZStackDoc* doc = new ZStackDoc;
  doc->loadStack(stack->getStack());
  // Release stack resources regardless of what happens next
  stack->releaseStack();
  delete stack;
  stack = nullptr;
  // IMPORTANT: do NOT gate on is3DSupported() here; it may be false until the
  // first context is created. Create the window; GL init will happen asynchronously.
  Z3DWindow* window = make3DWindow(doc);
  if(!window) {
    // If your overload transfers ownership of 'doc' into the window,
    // this 'delete' must be removed. Keep whichever semantics your code uses.
    delete doc;
    return nullptr;
  }
  // Apply non-GL properties (safe before GL init)
  if(auto* vs = window->getVolumeSource()) {
    if(auto* v0 = vs->getVolume(0)) {
      v0->setScaleSpacing(scale);
      v0->setOffset(glm::vec3(offset.x(), offset.y(), offset.z()));
    }
  }
  // Do NOT call updateVolumeBoundBox / resetCamera here.
  // Those run in Z3DWindow::finishInitGL(), after GL is actually ready.
  configure(window); // any non-GL per-window tweaks
  // Show to trigger QOpenGLWidget::initializeGL → finishInitGL()
  window->show();
  window->raise();
  return window;
}
void ZWindowFactory::setWindowTitle(const QString& title) {
  m_windowTitle = title;
}
void ZWindowFactory::setParentWidget(QWidget* parentWidget) {
  m_parentWidget = parentWidget;
}
void ZWindowFactory::setWindowGeometry(const QRect& rect) {
  m_windowGeometry = rect;
}
void ZWindowFactory::configure(Z3DWindow* /*window*/) {
}
void ZWindowFactory::setVisible(Z3DWindow::ERendererLayer layer, bool visible) {
  m_layerVisible[layer] = visible;
}
