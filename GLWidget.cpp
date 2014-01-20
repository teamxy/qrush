#include "GLWidget.h"
#include <QTimer>
#include <iostream>
#include <QMouseEvent>

GLWidget::GLWidget(QWidget *parent)
  : QGLWidget(QGLFormat(QGL::SampleBuffers), parent){
    // leave widget parts in the north-west and only repaint new stuff
    setAttribute(Qt::WA_StaticContents);
  }

void GLWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.drawImage(0, 0, image);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  if(isPressed){
    //int x = event->x();
    //int y = event->y();
    //std::cout << "mouse drag: (" << event->x() << ", " << event->y() << ")" << std::endl;

    // TODO implement v8 drawing here
    drawLineTo(event->pos());
    update();
  }
}

// Temporary testing function
void GLWidget::drawLineTo(const QPoint &endPoint){
  QPainter painter(&image);
  painter.setPen(Qt::red);
  painter.drawLine(lastPoint, endPoint);
  lastPoint = endPoint;
}

void GLWidget::resizeImage(QImage *image, const QSize &newSize) {
  if (image->size() == newSize)
    return;

  QImage newImage(newSize, QImage::Format_RGB32);
  newImage.fill(qRgb(255, 255, 255));
  QPainter painter(&newImage);
  painter.drawImage(QPoint(0, 0), *image);
  *image = newImage;
}

void GLWidget::resizeEvent(QResizeEvent *event) {
  if (width() > image.width() || height() > image.height()) {
    int newWidth = qMax(width() + 128, image.width());
    int newHeight = qMax(height() + 128, image.height());
    resizeImage(&image, QSize(newWidth, newHeight));
    update();
  }
  QWidget::resizeEvent(event);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  isPressed = true;
}


void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  isPressed = false;
}
