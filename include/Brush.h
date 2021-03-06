#ifndef QRUSH_BRUSH_H_
#define QRUSH_BRUSH_H_

#include <QImage>
#include <QPainter>
#include <v8.h>

class Brush {
  public:
    Brush(QString script, QString name = "unnamed.js");
    void onDrag(int x, int y);
    void onClick(int x, int y);
    void onRelease(int x, int y);
    void setImage(QImage* _image, QImage* _previewImage);

  private:
    v8::Persistent<v8::Function> clickFun;
    v8::Persistent<v8::Function> dragFun;
    v8::Persistent<v8::Function> releaseFun;
    v8::Persistent<v8::Context> _context;
    bool compileError;
    void runV8Callback(int x, int y, v8::Persistent<v8::Function>& function);
    QWidget* window;
};

#endif /* end of include guard */
