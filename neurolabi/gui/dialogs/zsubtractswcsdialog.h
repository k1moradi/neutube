#ifndef ZSUBTRACTSWCSDIALOG_H
#define ZSUBTRACTSWCSDIALOG_H

#include <QDialog>

class ZSelectFileWidget;
class QDialogButtonBox;
class QPushButton;
class ZSubtractSWCsDialog : public QDialog
{
  Q_OBJECT
public:
  explicit ZSubtractSWCsDialog(QWidget *parent = nullptr, Qt::WindowFlags f = {});
private:
  void init();
private slots:
  void subtractSWCs();
private:
  ZSelectFileWidget *m_inputSWCWidget;
  ZSelectFileWidget *m_subtractSWCsWidget;
  ZSelectFileWidget *m_outputSWCWidget;
  QPushButton *m_runButton;
  QPushButton *m_exitButton;
  QDialogButtonBox *m_buttonBox;
};

#endif // ZSUBTRACTSWCSDIALOG_H
