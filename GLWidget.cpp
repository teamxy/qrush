#include "GLWidget.h"
#include <QTimer>
#include <iostream>
#include <QMouseEvent>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
}

void GLWidget::paintEvent(QPaintEvent *event) {
    std::cout << "paintEvent ... " << std::endl;
    //painter.restore();
    //painter.end();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if(isPressed){

        int x = event->x();
        int y = event->y();
        std::cout << "mouse drag: (" << event->x() << ", " << event->y() << ")" << std::endl;


        painter.setPen(Qt::red);
        painter.drawEllipse(QRectF(x, y, 20, 20));
        //painter.fillRect(50, 50, 100, 100, QBrush(QColor(64, 32, 64)));
        //painter.save();
        this->repaint();

    }
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    isPressed = true;
    std::cout << "mouse press: (" << event->x() << ", " << event->y() << ")" << std::endl;

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
}


void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    isPressed = false;
    std::cout << "mouse release: (" << event->x() << ", " << event->y() << ")" << std::endl;
    painter.end();
}
