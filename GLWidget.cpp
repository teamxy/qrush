#include "GLWidget.h"
#include <QTimer>
#include <iostream>
#include <QMouseEvent>

GLWidget::GLWidget(QWidget *parent)
  : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {

    // leave widget parts in the north-west and only repaint new stuff
    setAttribute(Qt::WA_StaticContents);

    setCursor(QCursor(Qt::CrossCursor));
    brush = new Brush(&image, "");
  }

void GLWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.drawImage(0, 0, image);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  if(isPressed){
    brush->onDrag(event->x(), event->y());
    update();
  }
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
