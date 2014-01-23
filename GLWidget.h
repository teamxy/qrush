#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <string>
#include "Brush.h"

class GLWidget : public QGLWidget {
  Q_OBJECT

  public:
    GLWidget(QWidget *parent);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

  protected:
    void paintEvent(QPaintEvent *event);
    void drawLineTo(const QPoint &endPoint);
    void resizeEvent(QResizeEvent *event);
    void resizeImage(QImage *image, const QSize &newSize);

  private:
    bool isPressed;
    QPoint lastPoint;
    QImage image;
    Brush* brush;

};

#endif // GLWIDGET_H
