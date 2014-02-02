#include <iostream>

#include <QApplication>
#include <QColor>

#include "../include/Brush.h"
#include "../include/MainWindow.h"

using namespace v8;

// needs to be static for now :/
// TODO improve this
QImage* image;
QImage* previewImage;
QImage* currentImage;
Persistent<Array> _imageDataArray;
Persistent<Array> _imageDataObject;
Persistent<Array> _imageDataNumber;

static int RGBAToInt(int a, int r, int g, int b){
  return ((a) << 24) + ((r) << 16) + ((g) << 8) + (b);
}

/**
 * Turns a Javascript value of type Number, Object or Array
 * into an int representing the numeric color value
 * to be used by the painter.
 * e.g.:
 * 0xFF0010FF, [255, 0, 16, 255], {r:255,g:0,b:16, a:128}
 */
static QColor valueToQColor(Value* value){
  Isolate* iso = Isolate::GetCurrent();

  if (value->IsNumber()) {
    int iColor = Integer::Cast(value)->Int32Value();
    return QColor(qRed(iColor), qGreen(iColor), qBlue(iColor), qAlpha(iColor));
  }

  int r = 0, g = 0, b = 0, a = 0;

  if (value->IsArray()) {
    Array* arr = Array::Cast(value);
    r = Integer::Cast(*(arr->Get(0)))->IntegerValue();
    g = Integer::Cast(*(arr->Get(1)))->IntegerValue();
    b = Integer::Cast(*(arr->Get(2)))->IntegerValue();
    a = Integer::Cast(*(arr->Get(3)))->IntegerValue();
  }else if (value->IsObject()) {
    Object* obj = Object::Cast(value);
    r = Integer::Cast(*(obj->Get(String::NewFromUtf8(iso, "r"))))->IntegerValue();
    g = Integer::Cast(*(obj->Get(String::NewFromUtf8(iso, "g"))))->IntegerValue();
    b = Integer::Cast(*(obj->Get(String::NewFromUtf8(iso, "b"))))->IntegerValue();
    a = Integer::Cast(*(obj->Get(String::NewFromUtf8(iso, "a"))))->IntegerValue();
  }

  return QColor(r,g,b,a);
}

// Draw a simple point
static void DrawPointCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 2) return;
  HandleScope scope(args.GetIsolate());
  Integer* x = Integer::Cast(*args[0]);
  Integer* y = Integer::Cast(*args[1]);

  QColor color = valueToQColor(*args[2]);

  QPainter painter(currentImage);

  painter.setPen(color);
  painter.drawPoint(x->Value(), y->Value());
}

static void DrawLineCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 4) return;
  HandleScope scope(args.GetIsolate());
  Integer* x1 = Integer::Cast(*args[0]);
  Integer* y1 = Integer::Cast(*args[1]);
  Integer* x2 = Integer::Cast(*args[2]);
  Integer* y2 = Integer::Cast(*args[3]);

  QColor color = valueToQColor(*args[4]);

  QPainter painter(currentImage);

  painter.setPen(color);
  painter.drawLine(x1->IntegerValue(), y1->IntegerValue(), x2->IntegerValue(), y2->IntegerValue());
}

static void DrawRectCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 4) return;
  HandleScope scope(args.GetIsolate());
  Integer* x1 = Integer::Cast(*args[0]);
  Integer* y1 = Integer::Cast(*args[1]);
  Integer* x2 = Integer::Cast(*args[2]);
  Integer* y2 = Integer::Cast(*args[3]);

  QColor color = valueToQColor(*args[4]);

  Value* fill = Boolean::Cast(*args[5]);

  QPainter painter(currentImage);

  painter.setPen(color);

  if(fill->BooleanValue()) painter.setBrush(QBrush(color));

  painter.drawRect(QRect(
        QPoint(x1->IntegerValue(), y1->IntegerValue()),
        QPoint(x2->IntegerValue(), y2->IntegerValue()))
      );
}

static void DrawEllipseCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 4) return;
  HandleScope scope(args.GetIsolate());
  Integer* x1 = Integer::Cast(*args[0]);
  Integer* y1 = Integer::Cast(*args[1]);
  Integer* x2 = Integer::Cast(*args[2]);
  Integer* y2 = Integer::Cast(*args[3]);

  QColor color = valueToQColor(*args[4]);

  Value* fill = Boolean::Cast(*args[5]);

  QPainter painter(currentImage);

  painter.setPen(color);

  if(fill->BooleanValue())
    painter.setBrush(QBrush(color));

  painter.drawEllipse(
      QRect(
        QPoint(x1->IntegerValue(), y1->IntegerValue()),
        QPoint(x2->IntegerValue(), y2->IntegerValue()))
      );
}

static void DrawCircleCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 3) return;
  HandleScope scope(args.GetIsolate());
  Integer* x = Integer::Cast(*args[0]);
  Integer* y = Integer::Cast(*args[1]);
  Integer* r = Integer::Cast(*args[2]);

  QColor color = valueToQColor(*args[3]);

  Value* fill = Boolean::Cast(*args[4]);

  QPainter painter(currentImage);

  painter.setPen(color);

  if(fill->BooleanValue())
    painter.setBrush(QBrush(color));

  painter.drawEllipse(QPoint(
      x->IntegerValue(),
      y->IntegerValue()),
      r->IntegerValue(),
      r->IntegerValue());
}

static void LogCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 1) return;
  HandleScope scope(args.GetIsolate());

  String::Utf8Value msg(args[0]->ToString());

  MainWindow* window = (MainWindow*) QApplication::activeWindow();
  window->log(*msg);
}

static void FillCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 1) return;
  HandleScope scope(args.GetIsolate());

  QColor color = valueToQColor(*args[0]);

  currentImage->fill(color);
}

static void SetPreviewCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 1) return;
  HandleScope scope(args.GetIsolate());

  Value* active = Boolean::Cast(*args[0]);

  MainWindow* window = (MainWindow*) QApplication::activeWindow();
  window->log(QString("Preview active: ").append(active->BooleanValue() ? "true" : "false"));

  if(active->BooleanValue())
      currentImage = previewImage;
  else {
      currentImage = image;
      previewImage->fill(0);
  }
}

static void refreshImageNumbers(){
  Isolate* iso = Isolate::GetCurrent();
  HandleScope handle_scope(iso);

  // get image dimensions
  int width = image->width();
  int height = image->height();

  // create a new pixel array
  Handle<Array> pixels = Array::New(iso, width * height);

  // get pixel data
  QRgb* pixel = (QRgb*) image->constBits();

  // parse image data into js array
  for (int w = 0; w < width; w++) {
    for (int h = 0; h < height; h++) {
      // get color value
      QRgb color = pixel[w * height + h];
      int alpha = qAlpha(color);
      int red = qRed(color);
      int green = qGreen(color);
      int blue = qBlue(color);

      pixels->Set(w * height + h, Integer::New(iso, RGBAToInt(red, green, blue, alpha)));
    }
  }

  _imageDataNumber.Reset(iso, pixels);
}

static void refreshImageArrays(){
  Isolate* iso = Isolate::GetCurrent();
  HandleScope handle_scope(iso);

  // get image dimensions
  int width = image->width();
  int height = image->height();

  // create a new pixel array
  Handle<Array> pixels = Array::New(iso, width * height);

  // get pixel data
  QRgb* pixel = (QRgb*) image->constBits();

  // parse image data into js array
  for (int w = 0; w < width; w++) {
    for (int h = 0; h < height; h++) {
      // get color value
      QRgb color = pixel[w * height + h];
      int red = qRed(color);
      int green = qGreen(color);
      int blue = qBlue(color);

      // create new rgb array
      Handle<Array> arr = Array::New(iso);
      arr->Set(0, Integer::New(iso, red));
      arr->Set(1, Integer::New(iso, green));
      arr->Set(2, Integer::New(iso, blue));

      // put in array
      pixels->Set(w * height + h, arr);
    }
  }

  _imageDataArray.Reset(iso, pixels);
}

static void refreshImageObjects(){
  Isolate* iso = Isolate::GetCurrent();
  HandleScope handle_scope(iso);

  // get image dimensions
  int width = image->width();
  int height = image->height();

  // create a new pixel array
  Handle<Array> pixels = Array::New(iso, width * height);

  // get pixel data
  QRgb* pixel = (QRgb*) image->constBits();

  // create strings once and not in the loop
  Handle<String> r = String::NewFromUtf8(iso, "r");
  Handle<String> g = String::NewFromUtf8(iso, "g");
  Handle<String> b = String::NewFromUtf8(iso, "b");

  // parse image data into js array
  for (int w = 0; w < width; w++) {
    for (int h = 0; h < height; h++) {
      // get color value
      QRgb color = pixel[w * height + h];
      int red = qRed(color);
      int green = qGreen(color);
      int blue = qBlue(color);

      // create new rgb object
      Handle<Object> obj = Object::New(iso);
      obj->Set(r, Integer::New(iso, red));
      obj->Set(g, Integer::New(iso, green));
      obj->Set(b, Integer::New(iso, blue));

      // put in array
      pixels->Set(w * height + h, obj);
    }
  }

  _imageDataObject.Reset(iso, pixels);
}

static void GetDataCallback(const FunctionCallbackInfo<Value>& args) {
  // v8 boilerplate
  Isolate* iso = args.GetIsolate();
  HandleScope handle_scope(iso);

  // does the user want fresh data?
  bool refresh = Boolean::Cast(*args[0])->BooleanValue();

  std::string type = *String::Utf8Value(args[1]);

  if(type == "array"){
    if(refresh){ refreshImageArrays(); }
    // return the pixel values
    args.GetReturnValue().Set(_imageDataArray);
    return;
  }

  if(type == "object"){
    if(refresh){ refreshImageObjects(); }
    // return the pixel values
    args.GetReturnValue().Set(_imageDataObject);
    return;
  }

  if(refresh){ refreshImageNumbers(); }
  args.GetReturnValue().Set(_imageDataNumber);
  return;

}

static void GetWidthCallback(const FunctionCallbackInfo<Value>& args) {
  Isolate* iso = args.GetIsolate();
  HandleScope handle_scope(iso);

  Handle<Value> width = Number::New(iso, image->width());
  args.GetReturnValue().Set(width);
}

static void GetHeightCallback(const FunctionCallbackInfo<Value>& args) {
  Isolate* iso = args.GetIsolate();
  HandleScope handle_scope(iso);

  Handle<Value> height = Number::New(iso, image->height());
  args.GetReturnValue().Set(height);
}

QString getErrorMessage(Isolate* iso, TryCatch* tryCatch) {
  QString errorStr;

  HandleScope handleScope(iso);
  String::Utf8Value exception(tryCatch->Exception());
  Handle<Message> message = tryCatch->Message();

  if (message.IsEmpty()) {
    errorStr = *exception;
  } else {
    String::Utf8Value filename(message->GetScriptResourceName());
    int linenum = message->GetLineNumber();

    errorStr.append(*filename);
    errorStr.append(": ");
    errorStr.append(linenum);
    errorStr.append(*exception);
    errorStr.append("\n");

    // print line of source code.
    errorStr.append(*String::Utf8Value(message->GetSourceLine()));
    errorStr.append("\n");

    // fill padding with empty chars to point to the error pos with ^
    int start = message->GetStartColumn();
    for (int i = 0; i < start; i++) {
      errorStr.append(" ");
    }

    int end = message->GetEndColumn();
    for (int i = start; i < end; i++) {
      errorStr.append("^");
    }

    errorStr.append("\n");

    String::Utf8Value stackTrace(tryCatch->StackTrace());
    if (stackTrace.length() > 0) {
      errorStr.append(*stackTrace);
    }
  }

  return errorStr;
}

void Brush::runV8Callback(int x, int y, Persistent<Function>& function){

  // if the script is not working or the function was not implemented
  if(compileError || function.IsEmpty()){
    return;
  }

  // setup isolate and context
  Isolate* iso = Isolate::GetCurrent();
  HandleScope handle_scope(iso);
  Local<Context> context = Local<Context>::New(iso, _context);
  Context::Scope context_scope(context);

  // put arguments into an array
  Handle<Value> args[2];
  args[0] = Number::New(iso, x);
  args[1] = Number::New(iso, y);

  // convert to local function
  Local<Function> fun = Local<Function>::New(iso, function);

  TryCatch tryCatch;

  // call function
  Handle<Value> result = fun->Call(context->Global(), 2, args);

  // catch runtime errors
  if(tryCatch.HasCaught()) {

    // TODO somehow tryCatch.Exception() causes a seg fault...

    //String::Utf8Value exception(tryCatch.Exception());

    String::Utf8Value stackTrace(tryCatch.StackTrace());

    compileError = true;
    ((MainWindow*) parent)->logError(*stackTrace);
  }
}

void Brush::onClick(int x, int y){
  runV8Callback(x, y, clickFun);
}

void Brush::onDrag(int x, int y) {
  runV8Callback(x, y, dragFun);
}

void Brush::onRelease(int x, int y){
  runV8Callback(x, y, releaseFun);
}

void Brush::setImage(QImage* _image, QImage* _previewImage) {
  // setup isolate and context
  // this is necessary even though we don't use them!
  Isolate* iso = Isolate::GetCurrent();
  HandleScope handle_scope(iso);
  Local<Context> context = Local<Context>::New(iso, _context);
  Context::Scope context_scope(context);

  image = _image;
  previewImage = _previewImage;
  currentImage = _image;
  refreshImageNumbers();
  refreshImageObjects();
  refreshImageArrays();
}

Brush::Brush(QObject* parent, QString source, QString name) : compileError(false), parent(parent) {

  // Get the default Isolate created at startup.
  Isolate* iso = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(iso);

  // define new global object
  Handle<ObjectTemplate> global = ObjectTemplate::New();

  // define global functions

  // drawing functions
  global->Set(iso, "point", FunctionTemplate::New(iso, DrawPointCallback));
  global->Set(iso, "line", FunctionTemplate::New(iso, DrawLineCallback));
  global->Set(iso, "rect", FunctionTemplate::New(iso, DrawRectCallback));
  global->Set(iso, "ellipse", FunctionTemplate::New(iso, DrawEllipseCallback));
  global->Set(iso, "circle", FunctionTemplate::New(iso, DrawCircleCallback));

  // utility functions
  global->Set(iso, "log", FunctionTemplate::New(iso, LogCallback));
  global->Set(iso, "getColorData", FunctionTemplate::New(iso, GetDataCallback));
  global->Set(iso, "getCanvasWidth", FunctionTemplate::New(iso, GetWidthCallback));
  global->Set(iso, "getCanvasHeight", FunctionTemplate::New(iso, GetHeightCallback));
  global->Set(iso, "setPreview", FunctionTemplate::New(iso, SetPreviewCallback));
  global->Set(iso, "fill", FunctionTemplate::New(iso, FillCallback));

  // Create a new context.
  Handle<Context> context = Context::New(iso, NULL, global);

  // Persist context globally
  _context.Reset(iso, context);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> sourceHandle = String::NewFromUtf8(iso, source.toStdString().data());

  // Compile the source code.
  Handle<String> scriptFileName = String::NewFromUtf8(iso, name.toStdString().data());
  TryCatch tryCatch;
  Handle<Script> script = Script::Compile(sourceHandle, scriptFileName);

  if(script.IsEmpty()) {

    // catch compilation errors
    compileError = true;
    ((MainWindow*) parent)->logError(getErrorMessage(iso, &tryCatch));
    return;
  }

  Handle<Value> result = script->Run();
  if(result.IsEmpty()) {

    // catch runtime errors
    compileError = true;
    ((MainWindow*) parent)->logError(getErrorMessage(iso, &tryCatch));
    return;
  }

  // retrieve the handler functions from the global object
  Handle<Object> glob = context->Global();
  Handle<Value> onClickVal = glob->Get(String::NewFromUtf8(iso, "onClick"));
  Handle<Value> onDragVal = glob->Get(String::NewFromUtf8(iso, "onDrag"));
  Handle<Value> onReleaseVal = glob->Get(String::NewFromUtf8(iso, "onRelease"));

  // persist the functions globally
  if (onClickVal->IsFunction()) {
    clickFun.Reset(iso, Handle<Function>::Cast(onClickVal));
  }

  if (onDragVal->IsFunction()) {
    dragFun.Reset(iso, Handle<Function>::Cast(onDragVal));
  }

  if (onReleaseVal->IsFunction()) {
    releaseFun.Reset(iso, Handle<Function>::Cast(onReleaseVal));
  }
}
