#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected:
    void paintEvent(QPaintEvent *event);

private:
    bool isPressed;
    QPainter painter;

};

#endif // GLWIDGET_H
