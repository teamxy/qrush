#ifndef _QRUSH_BRUSH_H_
#define _QRUSH_BRUSH_H_

#include <QImage>
#include <v8.h>

class Brush {
  public:
    Brush(QImage image, std::string file);

  private:
    QImage image;
    char* script;
};

#endif /* end of include guard */
