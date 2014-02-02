#include "../include/GLWidget.h"
#include <QTimer>
#include <iostream>
#include <QMouseEvent>

GLWidget::GLWidget(QWidget *parent)
  : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {

    // leave widget parts in the north-west and only repaint new stuff
    setAttribute(Qt::WA_StaticContents);

    setCursor(QCursor(Qt::CrossCursor));

    image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    preview.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    preview.fill(QColor(0,0,0,0));
    image.fill(0);
}

void GLWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);


  painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
  painter.drawImage(0, 0, image);

  painter.drawImage(0, 0, preview);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  if(!brush){ return; }
  if(isPressed){
    brush->onDrag(event->x(), event->y());
    update();
  }
}

void GLWidget::resizeImage(QImage *image, const QSize &newSize) {
  if (image->size() == newSize)
    return;

  QImage newImage(newSize, QImage::Format_ARGB32_Premultiplied);
  newImage.fill(QColor(0, 0, 0, 0));
  QPainter painter(&newImage);
  painter.drawImage(QPoint(0, 0), *image);
  *image = newImage;
}

void GLWidget::resizeEvent(QResizeEvent *event) {
  if (width() > image.width() || height() > image.height()) {
    int newWidth = qMax(width() + 128, image.width());
    int newHeight = qMax(height() + 128, image.height());

    resizeImage(&image, QSize(newWidth, newHeight));
    resizeImage(&preview, QSize(newWidth, newHeight));

    update();
  }
  QWidget::resizeEvent(event);
}

void GLWidget::setBrush(std::shared_ptr<Brush> brush){
  this->brush = brush;
  brush->setImage(&image, &preview);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  if(!brush){ return; }
  isPressed = true;
  brush->onClick(event->x(), event->y());
  update();
}


void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  if(!brush){ return; }
  isPressed = false;
  brush->onRelease(event->x(), event->y());
  update();
}
