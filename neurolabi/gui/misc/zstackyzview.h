#ifndef ZSTACKYZVIEW_H
#define ZSTACKYZVIEW_H

#include "zstackview.h"

class ZStackYZView : public ZStackView
{
  Q_OBJECT
public:
  explicit ZStackYZView(ZStackFrame *parent = 0);
  explicit ZStackYZView(QWidget *parent = 0);
  virtual ~ZStackYZView();
  // Unhide base overload(s) and provide an exact override
  using ZStackView::resetCanvasWithStack;
  void resetCanvasWithStack(ZMultiscalePixmap& canvas, ZPainter* painter) override;
public:
  void paintStackBuffer();
  void paintSingleChannelStackSlice(ZStack *stack, int slice);
  template<typename T>
  void resetCanvasWithStack(T &canvas, ZPainter *painter);
  void updateImageCanvas();

signals:

public slots:

protected:
  int getDepth() const;
};

#endif // ZSTACKYZVIEW_H
