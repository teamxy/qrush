#ifndef QRUSH_GLWIDGET_H_
#define QRUSH_GLWIDGET_H_

#include <QGLWidget>
#include <deque>
#include "Brush.h"

class GLWidget : public QGLWidget {
  Q_OBJECT

  public:
    GLWidget(QWidget *parent);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void setBrush(std::shared_ptr<Brush> brush);
    void undo();
    void redo();

  protected:
    void paintEvent(QPaintEvent *event);
    void drawLineTo(const QPoint &endPoint);
    void resizeEvent(QResizeEvent *event);
    void resizeImage(QImage *image, const QSize &newSize);

  private:
    bool isPressed;
    QPoint lastPoint;
    QImage image;
    QImage preview;
    std::shared_ptr<Brush> brush;

    std::deque<QImage> history_undo;
    std::deque<QImage> history_redo;

    const int maxHistoryLength = 10;
};

#endif // QRUSH_GLWIDGET_H_
