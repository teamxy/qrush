#include <iostream>

#include <QApplication>

#include "../include/Brush.h"
#include "../include/MainWindow.h"

using namespace v8;

// needs to be static for now :/
// TODO improve this
QImage* image;

/**
 * Turns a Javascript value of type Number, Object or Array
 * into an int representing the numeric color value
 * to be used by the painter.
 *
 * e.g.:
 * 0xFF0010
 * [255, 0, 16]
 * { r : 255, g : 0, b : 16}
 *
 */
static long ColorToInt(Value* value){
  Isolate* iso = Isolate::GetCurrent();

  if (value->IsNumber()) {
    return Integer::Cast(value)->IntegerValue();
  }

  int r = 0, g = 0, b = 0;

  if (value->IsArray()) {
    Array* arr = Array::Cast(value);
    r = Integer::Cast(*(arr->Get(0)))->IntegerValue();
    g = Integer::Cast(*(arr->Get(1)))->IntegerValue();
    b = Integer::Cast(*(arr->Get(2)))->IntegerValue();
  }else if (value->IsObject()) {
    Object* obj = Object::Cast(value);
    r = Integer::Cast(*(obj->Get(String::NewFromUtf8(iso, "r"))))->IntegerValue();
    g = Integer::Cast(*(obj->Get(String::NewFromUtf8(iso, "g"))))->IntegerValue();
    b = Integer::Cast(*(obj->Get(String::NewFromUtf8(iso, "b"))))->IntegerValue();
  }

  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

// Draw a simple point
static void DrawPointCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 2) return;
  HandleScope scope(args.GetIsolate());
  Integer* x = Integer::Cast(*args[0]);
  Integer* y = Integer::Cast(*args[1]);
  int color = ColorToInt(*args[2]);

  QPainter painter(image);

  painter.setPen(QColor(color));
  painter.drawPoint(x->Value(), y->Value());
}

static void DrawLineCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 4) return;
  HandleScope scope(args.GetIsolate());
  Integer* x1 = Integer::Cast(*args[0]);
  Integer* y1 = Integer::Cast(*args[1]);
  Integer* x2 = Integer::Cast(*args[2]);
  Integer* y2 = Integer::Cast(*args[3]);

  int color = ColorToInt(*args[4]);

  QPainter painter(image);

  painter.setPen(QColor(color));
  painter.drawLine(x1->IntegerValue(), y1->IntegerValue(), x2->IntegerValue(), y2->IntegerValue());
}

static void DrawRectCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 4) return;
  HandleScope scope(args.GetIsolate());
  Integer* x1 = Integer::Cast(*args[0]);
  Integer* y1 = Integer::Cast(*args[1]);
  Integer* x2 = Integer::Cast(*args[2]);
  Integer* y2 = Integer::Cast(*args[3]);

  int color = ColorToInt(*args[4]);

  Value* fill = Boolean::Cast(*args[5]);

  QPainter painter(image);

  painter.setPen(QColor(color));

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

  int color = ColorToInt(*args[4]);

  Value* fill = Boolean::Cast(*args[5]);

  QPainter painter(image);

  painter.setPen(QColor(color));

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

  int color = ColorToInt(*args[3]);

  Value* fill = Boolean::Cast(*args[4]);

  QPainter painter(image);

  painter.setPen(QColor(color));

  if(fill->BooleanValue())
    painter.setBrush(QBrush(color));

  painter.drawEllipse(QPoint(
      x->IntegerValue(),
      y->IntegerValue()),
      r->IntegerValue(),
      r->IntegerValue());
}

static void LogCallback(const FunctionCallbackInfo<Value>& args) {
  HandleScope scope(args.GetIsolate());

  String::Utf8Value msg(args[0]->ToString());

  MainWindow* window = (MainWindow*) QApplication::activeWindow();
  window->log(*msg);
}

// TODO make rgb values easier (wrap in object or array)
static void GetDataCallback(const FunctionCallbackInfo<Value>& args) {

  // Get the default Isolate created at startup.
  Isolate* iso = args.GetIsolate();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(iso);

  int width = image->width();
  int height = image->height();
  Handle<Array> pixels = Array::New(iso, width * height);

  // parse image data into js array
  // TODO improve this, maybe get all pixels at once from image
  for (int w = 0; w < width; w++) {
    for (int h = 0; h < height; h++) {
      // this is like the worst thing you can do if you want a fast app
      int color = image->pixel(w, h);
      pixels->Set(w * height + h, Integer::New(iso, color));
    }
  }

  // return the pixel values
  args.GetReturnValue().Set(pixels);
}

static void GetWidthCallback(const FunctionCallbackInfo<Value>& args) {

  // Get the default Isolate created at startup.
  Isolate* iso = args.GetIsolate();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(iso);

  Handle<Value> width = Number::New(iso, image->width());
  args.GetReturnValue().Set(width);
}

static void GetHeightCallback(const FunctionCallbackInfo<Value>& args) {

  // Get the default Isolate created at startup.
  Isolate* iso = args.GetIsolate();

  // Create a stack-allocated handle scope.
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

  // Get the default Isolate created at startup.
  Isolate* iso = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(iso);

  // Create a new local context.
  Local<Context> context = Local<Context>::New(iso, _context);

  // Create a new context scope
  Context::Scope context_scope(context);

  // put arguments into an array
  Handle<Value> args[2];
  args[0] = Number::New(iso, x);
  args[1] = Number::New(iso, y);

  Local<Function> fun = Local<Function>::New(iso, function);

  TryCatch tryCatch;

  Handle<Value> result = fun->Call(context->Global(), 2, args);

  // catch runtime errors
  if(tryCatch.HasCaught()) {

    // somehow tryCatch.Exception() causes a seg fault...

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

// TODO: make this static?
void Brush::setImage(QImage* _image) {
  image = _image;
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
