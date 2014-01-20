#ifndef _QRUSH_BRUSH_H_
#define _QRUSH_BRUSH_H_

#include <QImage>
#include <QPainter>
#include <v8.h>

class Brush {
  public:
    Brush(QImage* image, std::string file);
    // TODO implement onClick, onRelease
    void onDrag(int x, int y);

  private:
    char* script;
};

#endif /* end of include guard */
