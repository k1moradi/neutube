#include "zstackframe.h"
#include <QUndoCommand>
#include <QtConcurrentRun>
#include <QProgressDialog>

#include <iostream>
#include "tz_error.h"
#include "zstackview.h"
#include "zstackdoc.h"
#include "zstackpresenter.h"
#include "widgets/zimagewidget.h"
//#include "dialogs/settingdialog.h"
#include "dialogs/zstackframesettingdialog.h"
#include "zlocsegchain.h"
#include "tz_xml_utils.h"
#include "tz_string.h"
#include "ztraceproject.h"
#include "zstack.hxx"
#include "zcurve.h"
#include "z3dapplication.h"
#include "z3dwindow.h"
#include "zstackfile.h"
#include "zdoublevector.h"
#include "zfiletype.h"
#include <QtGui>
#ifdef _QT5_
#include <QtWidgets>
#endif
#include "zobjsmanagerwidget.h"
#include "neutubeconfig.h"
#include "zstackviewlocator.h"
#include "zstackstatistics.h"
#include "tz_stack_bwmorph.h"
#include "zobject3dscan.h"
#include "tz_stack_math.h"
#include "z3dcompositor.h"
#include "zstring.h"
#include "biocytin/zbiocytinfilenameparser.h"
#include "zpunctumio.h"
#include "ztilegraphicsitem.h"
#include "ztileinfo.h"
#include "mainwindow.h"
#include "z3dcanvas.h"
#include "zwindowfactory.h"
#include "zobject3dscanarray.h"
#include "zsparseobject.h"
#include "zmessage.h"
#include "zmessagemanager.h"
#include "zdialogfactory.h"
#include "zobject3dfactory.h"
#include "dialogs/zautotracedialog.h"

using namespace std;

ZStackFrame::ZStackFrame(QWidget *parent, Qt::WindowFlags flags) :
  QMdiSubWindow(parent, flags), m_parentFrame(NULL),
  m_tile(NULL), m_traceProject(NULL), m_isClosing(false),
  m_isWidgetReady(false), m_messageManager(NULL)
{
  setAttribute(Qt::WA_DeleteOnClose, true);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  setAcceptDrops(true);
  m_settingDlg = new ZStackFrameSettingDialog(this);
  m_settingDlg->setFromTracingConfig(ZNeuronTracerConfig::getInstance());
  m_manageObjsDlg = NULL;
  m_autoTraceDlg = new ZAutoTraceDialog(this);
  m_progress = NULL;
  qDebug() << m_doc.get();
  m_presenter = NULL;
  m_view = NULL;
  connect(this, SIGNAL(progressDone()), this, SLOT(endProgress()));
}
ZStackFrame::~ZStackFrame()
{
#ifdef _DEBUG_
  std::cout << "Frame " << this << " deconstructed" << std::endl;
#endif
  clear();
}

void ZStackFrame::BaseConstruct(
    ZStackFrame *frame, ZSharedPointer<ZStackDoc> doc)
{
  if (frame != NULL) {
    frame->constructFrame(doc);
  #ifdef Q_OS_WIN
    frame->showMaximized();
    frame->showNormal();
  #endif
  }
}

ZStackFrame*
ZStackFrame::Make(QMdiArea *parent, NeuTube::Document::ETag docTag)
{
  ZSharedPointer<ZStackDoc> doc =
      ZSharedPointer<ZStackDoc>(new ZStackDoc(NULL));
  doc->setTag(docTag);

  return Make(parent, doc);
}

ZStackFrame*
ZStackFrame::Make(QMdiArea *parent)
{
  return Make(parent, ZSharedPointer<ZStackDoc>(new ZStackDoc(NULL)));
}


ZStackFrame*
ZStackFrame::Make(QMdiArea *parent, ZSharedPointer<ZStackDoc> doc)
{
  ZStackFrame *frame = new ZStackFrame(parent);

  BaseConstruct(frame, doc);

  if (parent != NULL) {
    frame->enableMessageManager();
  }
  return frame;
}

void ZStackFrame::constructFrame(ZSharedPointer<ZStackDoc> doc)
{
  dropDocument(ZSharedPointer<ZStackDoc>(doc));
  createView();
  createPresenter();
  updateDocument();
  if (document()->getTag() == NeuTube::Document::BIOCYTIN_PROJECTION) {
    m_presenter->setViewMode(ZInteractiveContext::VIEW_OBJECT_PROJECT);
  }
  setView(m_view);
  m_view->prepareDocument();
  m_presenter->prepareView();
  if (doc.get() != NULL) {
    customizeWidget();
  }
}

void ZStackFrame::createDocument()
{
  setDocument(ZSharedPointer<ZStackDoc>(new ZStackDoc(NULL)));
}

void ZStackFrame::enableMessageManager()
{
  if (m_messageManager == NULL) {
    m_messageManager = ZMessageManager::Make<MessageProcessor>(this);
  }
  view()->enableMessageManager();
}

void ZStackFrame::createPresenter()
{
  if (m_presenter == NULL) {
    m_presenter = ZStackPresenter::Make(this);
  }
}

void ZStackFrame::createView()
{
  if (m_view == NULL) {
    m_view = new ZStackView(this);
  }
}

QProgressDialog* ZStackFrame::getProgressDialog()
{
  if (m_progress == NULL) {
    m_progress = new QProgressDialog(this);
    m_progress->setRange(0, 100);
    m_progress->setWindowModality(Qt::WindowModal);
    m_progress->setAutoClose(true);
    m_progress->setCancelButton(0);
  }
  return m_progress;
}

QProgressBar* ZStackFrame::getProgressBar()
{
  return getProgressDialog()->findChild<QProgressBar*>();
}

void ZStackFrame::endProgress()
{
  getProgressDialog()->reset();
}

void ZStackFrame::startProgress(const QString &title)
{
  getProgressDialog()->setLabelText(title);
  getProgressDialog()->setValue(1);
  getProgressDialog()->show();
}

void ZStackFrame::updateTraceConfig()
{
  m_doc->updateTraceWorkspaceResolution(xResolution(), yResolution(), zResolution());
  m_settingDlg->updateTracingConfig(m_doc->getNeuronTracer().getConfigRef());
}

void ZStackFrame::addDocData(const ZStackDocReader &reader)
{
  if (m_doc.get() == NULL) {
    createDocument();
  }
  m_doc->addData(reader);
  updateTraceConfig();
  if (m_doc->hasStackData()) {
    m_presenter->optimizeStackBc();
    m_view->reset();
  }
  setWindowTitle(m_doc->stackSourcePath().c_str());
}

void ZStackFrame::autoTraceFunc()
{
  ZQtBarProgressReporter reporter;
  reporter.setProgressBar(getProgressBar());
  ZProgressReporter *oldReporter = document()->getProgressReporter();
  document()->setProgressReporter(&reporter);
  executeAutoTraceCommand(m_autoTraceDlg->getTraceLevel(), m_autoTraceDlg->resampling(), m_autoTraceDlg->getChannel());
  document()->setProgressReporter(oldReporter);
  emit progressDone();
}

void ZStackFrame::autoTrace()
{
  int channelNumber = document()->getStack()->channelNumber();
  m_autoTraceDlg->setChannelCount(channelNumber);
  if (m_autoTraceDlg->exec()) {
    startProgress("Tracing");
    QtConcurrent::run(this, &ZStackFrame::autoTraceFunc);
  }
}

void ZStackFrame::consumeDocument(ZStackDoc *doc)
{
  ZSharedPointer<ZStackDoc> docPtr(doc);
  setDocument(docPtr);
}

void ZStackFrame::updateDocSignalSlot(FConnectAction connectAction)
{
  connectAction(m_doc.get(), SIGNAL(locsegChainSelected(ZLocsegChain*)), this, SLOT(setLocsegChainInfo(ZLocsegChain*)));
  if (m_doc->getTag() != NeuTube::Document::BIOCYTIN_PROJECTION) {
    connectAction(m_doc.get(), SIGNAL(zoomingToSelectedSwcNode()), this, SLOT(zoomToSelectedSwcNodes()));
  }
  connectAction(m_doc.get(), SIGNAL(stackLoaded()), this, SIGNAL(stackLoaded()));
  connectAction(m_doc.get(), SIGNAL(stackModified()), m_view, SLOT(updateChannelControl()));
  connectAction(m_doc.get(), SIGNAL(stackModified()), m_view, SLOT(updateThresholdSlider()));
  connectAction(m_doc.get(), SIGNAL(stackModified()), m_view, SLOT(updateSlider()));
  connectAction(m_doc.get(), SIGNAL(stackModified()), m_presenter, SLOT(updateStackBc()));
  connectAction(m_doc.get(), SIGNAL(stackModified()), m_view, SLOT(redraw()));
  connectAction(m_doc.get(), SIGNAL(objectModified()), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(objectModified(ZStackObject::ETarget)), m_view, SLOT(paintObject(ZStackObject::ETarget)));
  connectAction(m_doc.get(), SIGNAL(objectModified()), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(objectModified(QSet<ZStackObject::ETarget>)), m_view, SLOT(paintObject(QSet<ZStackObject::ETarget>)));
  connectAction(m_doc.get(), SIGNAL(cleanChanged(bool)), this, SLOT(changeWindowTitle(bool)));
  connectAction(m_doc.get(), SIGNAL(holdSegChanged()), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(chainSelectionChanged(QList<ZLocsegChain*>, QList<ZLocsegChain*>)), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(swcTreeNodeSelectionChanged(QList<Swc_Tree_Node*>, QList<Swc_Tree_Node*>)), this, SLOT(updateSwcExtensionHint()));
  connectAction(m_doc.get(), SIGNAL(swcTreeNodeSelectionChanged(QList<Swc_Tree_Node*>,QList<Swc_Tree_Node*>)), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(objectSelectionChanged(QList<ZStackObject*>,QList<ZStackObject*>)), m_view, SLOT(paintObject(QList<ZStackObject*>,QList<ZStackObject*>)));
  connectAction(m_doc.get(), SIGNAL(punctaSelectionChanged(QList<ZPunctum*>,QList<ZPunctum*>)), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(chainVisibleStateChanged(ZLocsegChain*,bool)), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(swcVisibleStateChanged(ZSwcTree*,bool)), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(punctumVisibleStateChanged()), m_view, SLOT(paintObject()));
  connectAction(m_doc.get(), SIGNAL(statusMessageUpdated(QString)), this, SLOT(notifyUser(QString)));
  connectAction(m_doc.get(), SIGNAL(stackTargetModified()), m_view, SLOT(paintStack()));
  connectAction(m_doc.get(), SIGNAL(thresholdChanged(int)), m_view, SLOT(setThreshold(int)));
  connectAction(m_view, SIGNAL(viewChanged(ZStackViewParam)), this, SLOT(notifyViewChanged(ZStackViewParam)));
  connectAction(m_view, SIGNAL(closingChildFrame()), this, SLOT(closeAllChildFrame()));
  connectAction(m_view, SIGNAL(showingSetting()), this, SLOT(showSetting()));
  connectAction(m_view, SIGNAL(autoTracing()), this, SLOT(autoTrace()));
}

void ZStackFrame::updateSignalSlot(FConnectAction connectAction)
{
  updateDocSignalSlot(connectAction);
  connectAction(this, SIGNAL(stackLoaded()), this, SLOT(setupDisplay()));
}

bool ZStackFrame::connectFunc(const QObject* obj1, const char *signal, const QObject *obj2, const char *slot)
{
  return connect(obj1, signal, obj2, slot);
}

void ZStackFrame::connectSignalSlot()
{
  updateSignalSlot(connectFunc);
}

void ZStackFrame::disconnectAll()
{
  updateSignalSlot(disconnect);
}

void ZStackFrame::dropDocument(ZSharedPointer<ZStackDoc> doc)
{
  if (m_doc.get() != doc.get()) {
    if (m_doc) {
      updateSignalSlot(disconnect);
      m_doc->removeUser(this);
    }
    m_doc = doc;
    m_doc->registerUser(this);
  }
}

void ZStackFrame::updateDocument()
{
  updateSignalSlot(connectFunc);
  updateTraceConfig();
  if (m_doc->hasStackData()) {
    if (m_presenter != NULL) {
      m_presenter->optimizeStackBc();
    }
  }
  if (m_doc->hasStack()) {
    if (m_view != NULL) {
      m_view->reset();
    }
  }
  setWindowTitle(m_doc->stackSourcePath().c_str());
  m_progressReporter.setProgressBar(m_view->progressBar());
  m_doc->setProgressReporter(&m_progressReporter);
}

void ZStackFrame::setDocument(ZSharedPointer<ZStackDoc> doc)
{
  if (m_doc.get() != doc.get()) {
    dropDocument(doc);
    updateDocument();
  }
}

void ZStackFrame::takeScreenshot(const QString &filename)
{
  if (m_view != NULL)
    m_view->takeScreenshot(filename);
}

void ZStackFrame::updateSwcExtensionHint()
{
  if (m_presenter != NULL) {
    m_presenter->updateSwcExtensionHint();
  }
}

void ZStackFrame::clearData()
{
  document()->clearData();
  presenter()->clearData();
}

void ZStackFrame::clear()
{
  disconnectAll();
  detachParentFrame();
  removeAllChildFrame();
  document()->setProgressReporter(NULL);
  // will be deleted by parent (this), so don't need, otherwise will crash
  if (m_tile != NULL) {
      delete m_tile;
      m_tile = NULL;
  }
}

void ZStackFrame::loadStack(Stack *stack, bool isOwner)
{
  Q_ASSERT(m_doc.get() != NULL);
  m_doc->loadStack(stack, isOwner);
  prepareDisplay();
}

void ZStackFrame::loadStack(ZStack *stack)
{
  Q_ASSERT(m_doc.get() != NULL);
  m_doc->loadStack(stack);
  prepareDisplay();
}

void ZStackFrame::prepareDisplay()
{
  setWindowTitle(document()->stackSourcePath().c_str());
  m_statusInfo =  QString("%1 loaded").arg(document()->stackSourcePath().c_str());
  m_presenter->optimizeStackBc();
  m_view->reset();
}

void ZStackFrame::setupDisplay()
{
  prepareDisplay();
  qDebug() << "ready(this) emitted";
  //To prevent strange duplcated signal emit
  disconnect(this, SIGNAL(stackLoaded()), this, SLOT(setupDisplay()));
  emit ready(this);
}

void ZStackFrame::setSizeHintOption(NeuTube::ESizeHintOption option)
{
  if (view() != NULL) {
    view()->setSizeHintOption(option);
  }
}

int ZStackFrame::readStack(const char *filePath)
{
  Q_ASSERT(m_doc.get() != NULL);
  switch (ZFileType::fileType(filePath)) {
  case ZFileType::SWC_FILE:
    m_doc->readSwc(filePath);
    if (!m_doc->hasSwc()) {
      return ERROR_IO_READ;
    }
#ifdef _DEBUG_
    cout << "emit stackLoaded()" << endl;
#endif
    emit stackLoaded();
    break;
  case ZFileType::V3D_APO_FILE:
  case ZFileType::V3D_MARKER_FILE:
    m_doc->importPuncta(filePath);
#ifdef _DEBUG_
    cout << "emit stackLoaded()" << endl;
#endif
    emit stackLoaded();
    break;
  case ZFileType::LOCSEG_CHAIN_FILE: {
    QStringList files;
    files.push_back(filePath);
    m_doc->importLocsegChain(files);
#ifdef _DEBUG_
    cout << "emit stackLoaded()" << endl;
#endif
    emit stackLoaded();
    break;
  }
  case ZFileType::SWC_NETWORK_FILE:
    m_doc->loadSwcNetwork(filePath);
#ifdef _DEBUG_
    cout << "emit stackLoaded()" << endl;
#endif
    emit stackLoaded();
    break;
  case ZFileType::JSON_FILE:
    if (!m_doc->importSynapseAnnotation(filePath, 0)) {
      return ERROR_IO_READ;
    }
    break;
  default:
    m_doc->readStack(filePath);
    break;
  }
  return SUCCESS;
}

int ZStackFrame::importImageSequence(const char *filePath)
{
  Q_ASSERT(m_doc.get() != NULL);
  if (m_doc->importImageSequence(filePath)) {
    if (!m_doc->hasStackData()) {
      return ERROR_IO_READ;
    }
  }
  setWindowTitle(filePath);
  m_statusInfo =  QString("%1 loaded").arg(filePath);
  m_presenter->optimizeStackBc();
  m_view->reset();
  return SUCCESS;
}

void ZStackFrame::saveTraceProject(const QString &filePath, const QString &output, const QString &prefix)
{
  QFile file(filePath);
  file.open(QIODevice::WriteOnly);
  QTextStream stream(&file);
  stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  stream << "<trace version=\"1.0\">\n";
  stream << "<data>\n";
  stream << "<image type=\"tif\">\n";
  stream << "<url>" << document()->getStack()->source().firstUrl().c_str()
         << "</url>\n";
  stream << "<resolution>" << "<x>" << xResolution() << "</x>"
      << "<y>" << yResolution() << "</y>" << "<z>" << zResolution() << "</z>"
      << "</resolution>\n";
  stream << "</image>\n";
  stream << "</data>\n";
  if (!(output.isEmpty()) || !(prefix.isEmpty())) {
    stream << "<output>\n";
    if (!output.isEmpty()) {
      stream << "<workdir>" << output << "</workdir>\n";
    }
    if (!prefix.isEmpty()) {
      stream << "<tube>" << prefix << "</tube>\n";
    }
    stream << "</output>\n";
  }
  stream << "</trace>\n";
  file.close();
}

void ZStackFrame::setView(ZStackView *view)
{
  setWidget(view);
}

void ZStackFrame::closeEvent(QCloseEvent *event)
{
  if (m_doc->isSavingRequired()) {
    int ans =  QMessageBox::question(
          this, tr("Confirm"), tr("There might be unsaved changes. Close anyway?"),
          QMessageBox::Cancel | QMessageBox::Ok, QMessageBox::Ok);
    if (ans == QMessageBox::Ok) {
      m_isClosing = true;
      event->accept();
      qDebug() << "emit closed(this)";
      emit closed(this);
    } else {
      event->ignore();
    }
  } else {
    m_isClosing = true;
    event->accept();
    qDebug() << "emit closed(this)";
    emit closed(this);
  }
}

void ZStackFrame::resizeEvent(QResizeEvent *event)
{
  QMdiSubWindow::resizeEvent(event);
  qDebug() << "emit infoChanged()";
  emit infoChanged();
}

void ZStackFrame::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("text/uri-list")) {
    event->acceptProposedAction();
  }
}

void ZStackFrame::dropEvent(QDropEvent *event)
{
  QList<QUrl> urls = event->mimeData()->urls();
  //Filter out tiff files
  QList<QUrl> imageUrls;
  QList<QUrl> nonImageUrls;
  foreach (QUrl url, urls) {
    if (ZFileType::isImageFile(url.path().toStdString())) {
      imageUrls.append(url);
    } else {
      nonImageUrls.append(url);
    }
  }
  if (!imageUrls.isEmpty()) {
    MainWindow *mainWindow = getMainWindow();
    if (mainWindow != NULL) {
      QStringList fileList;
      foreach (QUrl url, imageUrls) {
        fileList.append(url.path());
      }
      mainWindow->openFile(fileList);
    }
  }
  if (!nonImageUrls.isEmpty()) {
    load(nonImageUrls);
    if (NeutubeConfig::getInstance().getApplication() == "Biocytin") {
      open3DWindow(Z3DWindow::INIT_EXCLUDE_VOLUME);
    }
  }
}

void ZStackFrame::setViewInfo(const QString &info)
{
  view()->setInfo(info);
}

void ZStackFrame::setViewInfo()
{
  view()->setInfo();
}

QString ZStackFrame::briefInfo() const
{
  return m_statusInfo;
}

QString ZStackFrame::info() const
{
  if ((document()) && view() != NULL) {
    QString info = document()->getStack()->sourcePath().c_str();
    info +=
      QString("\n %1 x %2 => %3 x %4").arg(document()->getStack()->width()).
      arg(document()->getStack()->height()).
      arg(view()->imageWidget()->screenSize().width()).
      arg(view()->imageWidget()->screenSize().height());
    info += QString("\n") + m_statusInfo;
    return info;
  } else {
    return QString("");
  }
}

void ZStackFrame::showSetting()
{
  if (m_settingDlg->exec() == QDialog::Accepted) {
    synchronizeDocument();
  }
}

void ZStackFrame::showManageObjsDialog()
{
  if (m_manageObjsDlg) {
    m_manageObjsDlg->raise();
    m_manageObjsDlg->show();
  } else {
    m_manageObjsDlg = new QDialog(this);
    m_manageObjsDlg->setWindowTitle("Objects");
    m_manageObjsDlg->setSizeGripEnabled(true);
    ZObjsManagerWidget* omw = new ZObjsManagerWidget(m_doc.get(), NULL);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(omw);
    m_manageObjsDlg->setLayout(layout);
    m_manageObjsDlg->show();
  }
}

double ZStackFrame::xResolution()
{
  return m_settingDlg->getXScale();
}

double ZStackFrame::yResolution()
{
  return m_settingDlg->getYScale();
}

double ZStackFrame::zResolution()
{
  return m_settingDlg->getZScale();
}

void ZStackFrame::addDecoration(ZStackObject *obj)
{
  presenter()->addDecoration(obj, true);
}

void ZStackFrame::clearDecoration()
{
  presenter()->removeAllDecoration();

  updateView();
}

void ZStackFrame::setBc(double greyScale, double greyOffset, int channel)
{
  presenter()->setStackBc(greyScale, greyOffset, channel);
  document()->setStackBc(greyScale, greyOffset, channel);
}

void ZStackFrame::synchronizeSetting()
{
  ZResolution res = m_doc.get()->getResolution();
  m_settingDlg->setScale(res.voxelSizeX(), res.voxelSizeY(), res.voxelSizeZ());
  m_settingDlg->setBackground(document()->getStackBackground());
}

void ZStackFrame::synchronizeDocument()
{
  document()->setResolution(m_settingDlg->getXScale(), m_settingDlg->getYScale(), m_settingDlg->getZScale(), 'p');
  if (hasProject()) {
    document()->setWorkdir(m_traceProject->workspacePath().toLocal8Bit().constData());
  }
  updateTraceConfig();
  m_doc->setStackBackground(m_settingDlg->getBackground());
}

void ZStackFrame::setLocsegChainInfo(ZLocsegChain *chain, QString prefix, QString suffix)
{
  m_statusInfo += prefix;
  if (chain != NULL) {
    if (chain->heldNode() < 0) {
      m_statusInfo = QString("Chain %1 %2; %3 segments;")
                     .arg(chain->id())
                     .arg(chain->getSource().c_str())
                     .arg(chain->length());
    } else {
      m_statusInfo =  QString("Chain %1 [%2] %3").arg(chain->id()).
                      arg(chain->heldNode()).arg(chain->getSource().c_str());
    }
  }
  m_statusInfo += suffix;
  updateInfo();
}

void ZStackFrame::changeWindowTitle(bool clean)
{
  QString title = windowTitle();
  if (clean) {
    if (title.endsWith(" *")) {
      title.resize(title.size()-2);
      setWindowTitle(title);
    }
  } else {
    if (!title.endsWith(" *")) {
      title += " *";
      setWindowTitle(title);
    }
  }
}

void ZStackFrame::keyPressEvent(QKeyEvent *event)
{
  if (m_presenter != NULL) {
    if (!m_presenter->processKeyPressEvent(event)) {
      emit keyEventEmitted(event);
    }
  }
}

void ZStackFrame::updateInfo()
{
  qDebug() << "emit infoChanged()";
  emit infoChanged();
}

ZCurve ZStackFrame::curveToPlot(PlotSettings *settings, int option) const
{
  switch (option) {
  case 0:
    if (settings != NULL) {
      settings->minY = 0.0;
      settings->maxY = 500.0;
      settings->adjust();
    }
    return document()->locsegProfileCurve(STACK_FIT_DOT);
  case 1:
    if (settings != NULL) {
      settings->minY = 0.0;
      settings->maxY = 1.0;
      settings->adjust();
    }
    return document()->locsegProfileCurve(STACK_FIT_CORRCOEF);
  case 2:
    if (settings != NULL) {
      settings->minY = 0.0;
      settings->maxY = 255.0;
      settings->adjust();
    }
    return document()->locsegProfileCurve(STACK_FIT_MEAN_SIGNAL);
  case 3:
    if (settings != NULL) {
      settings->minY = 0.0;
      settings->maxY = 255.0;
      settings->adjust();
    }
    return document()->locsegProfileCurve(STACK_FIT_OUTER_SIGNAL);
  default:
    if (settings != NULL) {
      settings->minY = 0.0;
      settings->maxY = 1.0;
      settings->adjust();
    }
    return document()->locsegProfileCurve(STACK_FIT_CORRCOEF);
  }
}

QStringList ZStackFrame::toStringList() const
{
  QStringList list;
  list += view()->toStringList();
  list += presenter()->toStringList();
  list += document()->toStringList();
  return list;
}

void ZStackFrame::updateView()
{
  m_view->redraw(ZStackView::UPDATE_QUEUED);
}

void ZStackFrame::undo()
{
  m_doc->undoStack()->undo();
}

void ZStackFrame::redo()
{
  m_doc->undoStack()->redo();
}

void ZStackFrame::pushUndoCommand(QUndoCommand *command)
{
  m_doc->undoStack()->push(command);
}

void ZStackFrame::pushBinarizeCommand()
{
  m_doc->executeBinarizeCommand(view()->getIntensityThreshold());
}

void ZStackFrame::pushBwsolidCommand()
{
  m_doc->executeBwsolidCommand();
}

void ZStackFrame::pushEnhanceLineCommand()
{
  m_doc->executeEnhanceLineCommand();
}

void ZStackFrame::executeSwcRescaleCommand(const ZRescaleSwcSetting &setting)
{
  document()->executeSwcRescaleCommand(setting);
}

void ZStackFrame::executeAutoTraceCommand(int traceLevel, bool doResample, int c)
{
  document()->executeAutoTraceCommand(traceLevel, doResample, c);
}

void ZStackFrame::executeAutoTraceAxonCommand()
{
  document()->executeAutoTraceAxonCommand();
}

void ZStackFrame::executeWatershedCommand()
{
  document()->executeWatershedCommand();
}

void ZStackFrame::executeAddObjectCommand(ZStackObject *obj)
{
  document()->executeAddObjectCommand(obj);
}

double ZStackFrame::displayGreyMin(int c) const
{
  if (presenter()->greyScale(c) == 0.0) {
    return 0.0;
  }
  return -presenter()->greyOffset(c) / presenter()->greyScale(c);
}

double ZStackFrame::displayGreyMax(int c) const
{
  if (presenter()->greyScale(c) == 0.0) {
    return 0.0;
  }
  return (255.0 - presenter()->greyOffset(c)) / presenter()->greyScale(c);
}

void ZStackFrame::saveProjectAs(const QString &path)
{
  if (m_traceProject == NULL) {
    m_traceProject = new ZTraceProject(this);
  }
  document()->setWorkdir(path);
  m_traceProject->saveAs(path);
  m_doc->undoStack()->setClean();
}

void ZStackFrame::saveProject()
{
  if (m_traceProject == NULL) {
    m_traceProject = new ZTraceProject(this);
  }
  view()->progressBar()->setRange(0, 100);
  view()->progressBar()->show();
  QApplication::processEvents();
  m_traceProject->save();
  view()->progressBar()->setValue(view()->progressBar()->maximum());
  QApplication::processEvents();
  view()->progressBar()->hide();
  view()->setInfo("Project saved.");
  m_doc->undoStack()->setClean();
}

void ZStackFrame::importSwcAsReference(const QStringList &pathList)
{
  if (m_traceProject == NULL) {
    m_traceProject = new ZTraceProject(this);
  }
  document()->importSwc(pathList, ZStackDoc::APPEND_OBJECT);
  foreach (QString path, pathList) {
    m_traceProject->addDecoration(path, "swc");
  }
}

bool ZStackFrame::isReadyToSave() const
{
  if (!hasProject()) {
    return false;
  }
  if (m_traceProject->workspacePath().isEmpty()) {
    return false;
  }
  return true;
}

void ZStackFrame::exportPuncta(const QString &filePath)
{
  document()->exportPuncta(filePath.toStdString().c_str());
}

void ZStackFrame::exportTube(const QString &filePath)
{
  document()->exportBinary(filePath.toStdString().c_str());
}

void ZStackFrame::exportChainFileList(const QString &filePath)
{
  document()->exportChainFileList(filePath.toStdString().c_str());
}

ZStack* ZStackFrame::getObjectMask()
{
  return view()->getObjectMask(1);
}

ZStack* ZStackFrame::getObjectMask(NeuTube::EColor color)
{
  return view()->getObjectMask(color, 1);
}

ZStack* ZStackFrame::getStrokeMask()
{
  return view()->getStrokeMask(1);
}

ZStack* ZStackFrame::getStrokeMask(NeuTube::EColor color)
{
  return view()->getStrokeMask(color);
}

void ZStackFrame::exportObjectMask(const QString &filePath)
{
  view()->exportObjectMask(filePath.toStdString());
}

void ZStackFrame::exportObjectMask(
    NeuTube::EColor color, const QString &filePath)
{
  view()->exportObjectMask(color, filePath.toStdString());
}

void ZStackFrame::saveStack(const QString &filePath)
{
  if (document()->hasStackData()) {
    document()->getStack()->save(filePath.toStdString());
    document()->setStackSource(filePath.toStdString().c_str());
  } else {
    QList<ZSparseObject*> objList = document()->getSparseObjectList();
    ZObject3dScanArray objArray;
    foreach (ZSparseObject *obj, objList) {
      objArray.push_back(*dynamic_cast<ZObject3dScan*>(obj));
    }
    ZStack *stack = objArray.toStackObject();
    if (stack != NULL) {
      stack->save(filePath.toStdString().c_str());
      delete stack;
    }
  }
}

void ZStackFrame::displayActiveDecoration(bool enabled)
{
  m_view->displayActiveDecoration(enabled);
}

QRect ZStackFrame::getViewGeometry() const
{
  QRect rect = view()->geometry();
  rect.moveTo(view()->mapToGlobal(rect.topLeft()));

  return rect;
}

ZInteractiveContext::ViewMode ZStackFrame::getViewMode() const
{
  return presenter()->interactiveContext().viewMode();
}

void ZStackFrame::setViewMode(ZInteractiveContext::ViewMode mode)
{
  presenter()->setViewMode(mode);
}

void ZStackFrame::setObjectDisplayStyle(ZStackObject::EDisplayStyle style)
{
  presenter()->setObjectStyle(style);
}

void ZStackFrame::setViewPortCenter(int x, int y, int z)
{
  view()->setViewPortCenter(x, y, z, NeuTube::AXIS_NORMAL);
}

void ZStackFrame::viewRoi(int x, int y, int z, int radius)
{
  ZGeometry::shiftSliceAxis(x, y, z, view()->getSliceAxis());
  ZStackViewLocator locator;
  locator.setCanvasSize(view()->imageWidget()->canvasSize().width(), view()->imageWidget()->canvasSize().height());
  QRect viewPort = locator.getLandmarkViewPort(x, y, radius);
  presenter()->setZoomRatio(locator.getZoomRatio(viewPort.width(), viewPort.height()));
  view()->setViewPortCenter(x, y, z, NeuTube::AXIS_SHIFTED);
}

void ZStackFrame::hideObject()
{
  presenter()->setObjectVisible(false);
}

void ZStackFrame::showObject()
{
  presenter()->setObjectVisible(true);
}

// zstackframe.cpp
// gui/zstackframe.cpp
Z3DWindow* ZStackFrame::open3DWindow(Z3DWindow::EInitMode mode)
{
  // pick the right doc (projection → parent doc)
  ZSharedPointer<ZStackDoc> doc = document();
  if (doc->getTag() == NeuTube::Document::BIOCYTIN_PROJECTION) {
    doc = doc->getParentDoc();
  }

  // Reuse existing 3D window if present
  if (Z3DWindow *existing = doc->getParent3DWindow()) {
    existing->show();
    existing->raise();
    return existing;
  }

  // Show progress while we spin up GL
  if (auto* mw = getMainWindow()) mw->startProgress("Opening 3D View …", 0);

  // Create the window; actual GL init will occur in finishInitGL()
  Z3DWindow *win = Z3DWindow::Make(doc, /*parent*/nullptr, mode);
  if (!win) {                // extremely unlikely now; ctor would have had to throw/abort
    if (auto* mw = getMainWindow()) mw->endProgress();
    QMessageBox::critical(this, tr("3D View"), tr("Could not create 3D window."));
    return nullptr;
  }

  // Keep title in sync with this frame
  QString title = windowTitle();
  if (title.endsWith(" *")) title.chop(2);
  win->setWindowTitle(title);

  // Close the 3D window when this frame closes (ownership semantics unchanged)
  connect(this, SIGNAL(closed(ZStackFrame*)), win, SLOT(close()));

  // When GL reports back, stop the spinner and, on failure, inform the user.
  connect(win, &Z3DWindow::initialized, this,
          [this](bool ok, const QString& err){
            if (auto* mw = getMainWindow()) mw->endProgress();
            if (!ok && !err.isEmpty()) {
              QMessageBox::critical(this, tr("OpenGL Initialization"), err);
            }
          });

  // IMPORTANT: show to let Qt create the context and fire glReady() → finishInitGL()
  win->show();
  win->raise();
  return win;
}

void ZStackFrame::load(const QList<QUrl> &urls)
{
  m_doc->loadFileList(urls);
}

void ZStackFrame::load(const QStringList &fileList)
{
  m_doc->loadFileList(fileList);
}

void ZStackFrame::load(const QString &filePath)
{
  m_doc->loadFile(filePath);
}

void ZStackFrame::load(const std::string &filePath)
{
  m_doc->loadFile(filePath.c_str());
}

QAction* ZStackFrame::getBodySplitAction()
{
  QAction *action = NULL;
  if (getMainWindow() != NULL) {
    action = getMainWindow()->getBodySplitAction();
  }
  return action;
}

MainWindow* ZStackFrame::getMainWindow()
{
  MainWindow *mainwin = NULL;
  QObject *parentObject = parent();
  if (parentObject != NULL) {
    parentObject = parentObject->parent();
    if (parentObject != NULL) {
      parentObject = parentObject->parent();
    }
    mainwin = qobject_cast<MainWindow*>(parentObject);
  }

  return mainwin;
}

ZStackFrame* ZStackFrame::spinoffStackSelection(const vector<int> &selected)
{
  if (document()->hasStackData()) {
    int channelNumber = document()->getStack()->channelNumber();
    std::vector<std::vector<double> > selectedColor = ZDoubleVector::reshape(selected, channelNumber);
    return spinoffStackSelection(selectedColor);
  }
  return NULL;
}

ZStackFrame* ZStackFrame::spinoffStackSelection(
    const vector<vector<double> > &selected)
{
  ZStackFrame *frame = NULL;
  if (m_doc->hasStackData()) {
    frame = Make(NULL);
    ZStack *substack = m_doc->getStack()->createSubstack(selected);
    frame->document()->loadStack(substack);
    frame->view()->reset();
  }
  return frame;
}

void ZStackFrame::invertStack()
{
  if (m_doc->hasStackData()) {
    m_doc->invert();
  }
}

void ZStackFrame::yFlipStack()
{
  if (m_doc->hasStackData()) {
    m_doc->flipY();
  }
}

void ZStackFrame::subtractBackground()
{
  if (m_doc->hasStackData()) {
    m_doc->subtractBackground();
  }
}

void ZStackFrame::subtractBackgroundAdaptive()
{
  if (m_doc->hasStackData()) {
    m_doc->subtractBackgroundAdaptive();
  }
}

void ZStackFrame::detachParentFrame()
{
  if (m_parentFrame != NULL) {
    m_parentFrame->removeChildFrame(this);
    m_parentFrame = NULL;
  }
}

void ZStackFrame::removeChildFrame(ZStackFrame *frame)
{
  if (m_childFrameList.removeOne(frame)) {
    frame->m_parentFrame = NULL;
  }
}

void ZStackFrame::removeAllChildFrame()
{
  foreach (ZStackFrame *childFrame, m_childFrameList) {
    childFrame->m_parentFrame = NULL;
  }
  m_childFrameList.clear();
}

void ZStackFrame::closeAllChildFrame()
{
  foreach (ZStackFrame *childFrame, m_childFrameList) {
    childFrame->close();
  }
  m_childFrameList.clear();
}

void ZStackFrame::setParentFrame(ZStackFrame *frame)
{
  detachParentFrame();
  m_parentFrame = frame;
  if (m_parentFrame != NULL) {
    m_parentFrame->m_childFrameList.append(this);
  }
}

void ZStackFrame::findLoopInStack()
{
  document()->findLoop();
}

void ZStackFrame::bwthin()
{
  document()->bwthin();
}

void ZStackFrame::importSeedMask(const QString &filePath)
{
  document()->importSeedMask(filePath);
}

void ZStackFrame::importMask(const QString &filePath)
{
  ZStack *stack = NULL;
  if (ZFileType::fileType(filePath.toStdString()) == ZFileType::PNG_FILE) {
    QImage image;
    image.load(filePath);
    stack = new ZStack(GREY, image.width(), image.height(), 1, 1);
    size_t offset = 0;
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        QRgb rgb = image.pixel(x, y);
        if (qRed(rgb) == 255 && qGreen(rgb) == 255 && qBlue(rgb) == 255) {
          stack->array8()[offset] = 0;
        } else {
          if (image.hasAlphaChannel()) {
            stack->array8()[offset] = qAlpha(rgb);
          } else {
            stack->array8()[offset] = qRed(rgb);
          }
        }
        ++offset;
      }
    }
  } else {
    ZStackFile stackFile;
    stackFile.import(filePath.toStdString());
    stack = stackFile.readStack();
  }
  if (stack != NULL) {
    if (stack->channelNumber() == 1 && stack->kind() == GREY) {
      ZObject3dScan *obj = ZObject3dFactory::MakeObject3dScan(*stack, NULL);
      obj->setSource(stack->sourcePath());
      obj->setColor(QColor(255, 0, 0, 128));
      if (!obj->isEmpty()) {
        obj->translate(document()->getStackOffset());
        executeAddObjectCommand(obj);
      } else {
        delete obj;
        report("Loading mask failed", "Cannot convert the image into mask", NeuTube::MSG_ERROR);
      }
    } else {
      report("Loading mask failed", "Must be single 8-bit image", NeuTube::MSG_ERROR);
    }
    delete stack;
  }
}

void ZStackFrame::importSwc(const QString &filePath)
{
  load(filePath);
}

void ZStackFrame::importSobj(const QStringList &fileList)
{
  ZStackFile stackFile;
  vector<string> fileArray;
  foreach(QString file, fileList) {
    fileArray.push_back(file.toStdString());
  }
  stackFile.importSobjList(fileArray);
  ZStack *stack = stackFile.readStack();
  loadStack(stack);
}

void ZStackFrame::importPointList(const QString &filePath)
{
  QList<ZPunctum*> puncta = ZPunctumIO::load(filePath);
  if (!puncta.isEmpty()) {
    document()->beginObjectModifiedMode(ZStackDoc::OBJECT_MODIFIED_CACHE);
    foreach (ZPunctum* punctum, puncta) {
      document()->addObject(punctum);
    }
    document()->endObjectModifiedMode();
    document()->notifyObjectModified();
  }
}

void ZStackFrame::autoBcAdjust()
{
  document()->startProgress();
  for (int i = 0; i < document()->getStack()->channelNumber(); ++i) {
    document()->advanceProgress(0.1);
    double lower, upper;
    ZStackStatistics::getGreyMapHint(*document()->getStack(), i, &lower, &upper);
    document()->advanceProgress(0.2);
    double scale = 1.0;
    double offset = 0.0;
    if (upper > lower) {
      scale = 255.0 / (upper - lower);
    }
    offset = -scale * lower;
    setBc(scale, offset, i);
  }
  document()->endProgress();
  updateView();
}

void ZStackFrame::loadRoi(bool isExclusive)
{
  if (!document()->stackSourcePath().empty()) {
    ZString sourcePath = document()->stackSourcePath();
    ZString suffix = ZBiocytinFileNameParser::getSuffix(ZBiocytinFileNameParser::ROI);
    sourcePath = sourcePath.dirPath() + ZString::FileSeparator + ZBiocytinFileNameParser::getCoreName(sourcePath);
    QFileInfo fileInfo((sourcePath + suffix + ".tif").c_str());
    if (!fileInfo.exists()) {
      fileInfo.setFile((sourcePath + static_cast<const ZString&>(suffix).lower() + ".tif").c_str());
    } else if (!fileInfo.exists()) {
      fileInfo.setFile((sourcePath + static_cast<const ZString&>(suffix).upper() + ".tif").c_str());
    }
    if (fileInfo.exists()) {
      loadRoi(fileInfo.absoluteFilePath(), isExclusive);
    }
  }
}

void ZStackFrame::loadRoi(const QString &filePath, bool isExclusive)
{
  ZStackFile stackFile;
  stackFile.import(filePath.toStdString());
  ZStack *stack = stackFile.readStack();
  if (stack->kind() != GREY) {
    std::cerr << "Ignore invalid ROI" <<std::endl;
    delete stack;
    stack = NULL;
  }
  if (stack != NULL) {
    Stack *mask = C_Stack::clone(stack->c_stack());
    ZObject3dScan *obj = new ZObject3dScan;
    Stack_Binarize(mask);
    Stack_Not(mask, mask);
    obj->loadStack(mask);
    obj->duplicateSlice(document()->getStack()->depth());
    obj->setColor(16, 16, 16, 64);
    obj->setTarget(ZStackObject::TARGET_OBJECT_CANVAS);
    if (isExclusive) {
      clearDecoration();
    }
    obj->addVisualEffect(NeuTube::Display::SparseObject::VE_FORCE_SOLID);
    addDecoration(obj);
    updateView();
    C_Stack::kill(mask);
    delete stack;
  }
}

void ZStackFrame::zoomToSelectedSwcNodes()
{
  if (document()->hasSelectedSwcNode()) {
    std::set<Swc_Tree_Node*> nodeSet = document()->getSelectedSwcNodeSet();
    ZCuboid cuboid = SwcTreeNode::boundBox(nodeSet);
    ZPoint center = cuboid.center();
    //check which stack the selected points belong to.
    //If needed, load the corresponding stack.
    if (getTileManager() != NULL) {
      ZTileInfo tile = getTileManager()->getSelectedTileItem()->getTileInfo();
      QRect bound= QRect(tile.getOffset().x(),tile.getOffset().y(),tile.getWidth(),tile.getHeight());
      if (!bound.contains(center.x(),center.y())) {
        QList<QGraphicsItem*> itemList = getTileManager()->items();
        foreach (QGraphicsItem *item, itemList) {
          ZTileGraphicsItem *zitem = dynamic_cast<ZTileGraphicsItem*>(item);
          if (zitem != NULL) {
            //check whether this item contains the selected points.
            tile = zitem->getTileInfo();
            bound = QRect(tile.getOffset().x(),tile.getOffset().y(),tile.getWidth(),tile.getHeight());
            if (bound.contains(center.x(),center.y())) {
              getTileManager()->selectItem(zitem);
              break;
            }
          }
        }
      }
    }
    int cx, cy, cz;
    cx = iround(center.x());
    cy = iround(center.y());
    cz = iround(center.z());
    int radius = iround(std::max(cuboid.width(), cuboid.height()) / 2.0);
    viewRoi(cx, cy, cz, radius);
  }
}

void ZStackFrame::notifyUser(const QString &message)
{
  if (!message.isEmpty()) {
    m_statusInfo = message;
    emit infoChanged();
  }
}

void ZStackFrame::locateSwcNodeIn3DView()
{
  if (document()->hasSelectedSwcNode()) {
    Z3DWindow *window = document()->getParent3DWindow();
    if (!window) {
      window = open3DWindow();
    }
    window->zoomToSelectedSwcNodes();
    window->raise();
  }
}

void ZStackFrame::runSeededWatershed()
{
  emit splitStarted();
  document()->runSeededWatershed();
}

void ZStackFrame::makeSwcProjection(ZStackDoc *doc)
{
    if (doc == NULL) return;
    QList<ZSwcTree*> swclist= doc->getSwcList();
    foreach (ZSwcTree* swc, swclist) {
        ZSwcTree *swcClone = swc->clone();
        std::vector<Swc_Tree_Node*> nodes = swcClone->getSwcTreeNodeArray();
        foreach (Swc_Tree_Node *nd, nodes) {
            SwcTreeNode::setZ(nd, 0);
        }
        this->presenter()->addDecoration(swcClone);
    }
}

ZStackObject::EDisplayStyle ZStackFrame::getObjectStyle() const
{
  return m_presenter->objectStyle();
}

void ZStackFrame::setObjectStyle(ZStackObject::EDisplayStyle style)
{
  m_presenter->setObjectStyle(style);
}

void ZStackFrame::createMainWindowActions()
{
  m_presenter->createMainWindowActions();
}

void ZStackFrame::notifyViewChanged(const ZStackViewParam &param)
{
  emit viewChanged(param);
}

void ZStackFrame::setView(const ZStackViewParam &param)
{
  view()->setView(param);
}

void ZStackFrame::customizeWidget()
{
  if (!m_isWidgetReady) {
    view()->customizeWidget();
    m_isWidgetReady = true;
  }
}

void ZStackFrame::MessageProcessor::processMessage(
    ZMessage *message, QWidget *host) const
{
  switch (message->getType()) {
  case ZMessage::TYPE_3D_VIS:
  {
    ZStackFrame *frame = qobject_cast<ZStackFrame*>(host);
    if (frame != NULL) {
      if (frame->document()->getTag() == NeuTube::Document::BIOCYTIN_STACK ||
          frame->document()->getTag() == NeuTube::Document::BIOCYTIN_PROJECTION) {
        frame->open3DWindow(Z3DWindow::INIT_EXCLUDE_VOLUME);
      } else {
        frame->open3DWindow();
      }
    }
    message->deactivate();
  }
    break;
  default:
    break;
  }
}
