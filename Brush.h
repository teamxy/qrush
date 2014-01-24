#ifndef _QRUSH_BRUSH_H_
#define _QRUSH_BRUSH_H_

#include <QImage>
#include <QPainter>
#include <v8.h>
#include <string>

class Brush {
  public:
    Brush(char* script);
    // TODO implement onClick, onRelease
    void onDrag(int x, int y);
    void setImage(QImage* _image);

  private:
    v8::Persistent<v8::Function> dragFun;
    v8::Persistent<v8::Context> _context;
};

#endif /* end of include guard */
