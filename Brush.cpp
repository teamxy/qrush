#include <iostream>
#include <functional>

#include <QApplication>

#include "Brush.h"
#include "MainWindow.h"

using namespace v8;

// needs to be static for now :/
// TODO improve this
QImage* image;

// TODO: implement lots of more QPainter mappings

// Draw a simple point
static void DrawPointCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 2) return;
  HandleScope scope(args.GetIsolate());
  Integer* x = Integer::Cast(*args[0]);
  Integer* y = Integer::Cast(*args[1]);
  Integer* color = Integer::Cast(*args[2]);

  QPainter painter(image);

  painter.setPen(QColor(color->IntegerValue()));
  painter.drawPoint(x->Value(), y->Value());
}

static void DrawLineCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 2) return;
  HandleScope scope(args.GetIsolate());
  Integer* x1 = Integer::Cast(*args[0]);
  Integer* y1 = Integer::Cast(*args[1]);
  Integer* x2 = Integer::Cast(*args[2]);
  Integer* y2 = Integer::Cast(*args[3]);

  Integer* color = Integer::Cast(*args[4]);

  QPainter painter(image);

  painter.setPen(QColor(color->IntegerValue()));
  painter.drawLine(x1->IntegerValue(), y1->IntegerValue(), x2->IntegerValue(), y2->IntegerValue());
}

static void DrawRectCallback(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 2) return;
  HandleScope scope(args.GetIsolate());
  Integer* x1 = Integer::Cast(*args[0]);
  Integer* y1 = Integer::Cast(*args[1]);
  Integer* x2 = Integer::Cast(*args[2]);
  Integer* y2 = Integer::Cast(*args[3]);

  Integer* color = Integer::Cast(*args[4]);

  QPainter painter(image);

  painter.fillRect(QRect(QPoint(x1->IntegerValue(), y1->IntegerValue()), QPoint(x2->IntegerValue(), y2->IntegerValue())), QColor(color->IntegerValue()));
}

static void LogCallback(const FunctionCallbackInfo<Value>& args) {
  HandleScope scope(args.GetIsolate());

  String::Utf8Value msg(args[0]->ToString());

  MainWindow* window = (MainWindow*) QApplication::activeWindow();
  window->log(*msg);
}


QString getErrorMessage(Isolate* isolate, TryCatch* tryCatch) {
  QString errorStr;

  HandleScope handleScope(isolate);
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
  Isolate* isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // Create a new local context.
  Local<Context> context = Local<Context>::New(isolate, _context);

  Context::Scope context_scope(context);

  // parse image data into js array
  // TODO improve this, maybe get all pixels at once from image
  int width = image->width();
  int height = image->height();
  v8::Handle<v8::Array> pixels = v8::Array::New(isolate, width * height);
  for (int w = 0; w < width; w++) {
    for (int h = 0; h < height; h++) {
      // this is like the worst thing you can do if you want a fast app
      int color = image->pixel(w, h);
      pixels->Set(w * height + h, Integer::New(isolate, color));
    }
  }

  Handle<Value> args[5];
  args[0] = Number::New(isolate, x);
  args[1] = Number::New(isolate, y);
  args[2] = Number::New(isolate, width);
  args[3] = Number::New(isolate, height);
  args[4] = pixels;

  Local<Function> fun = Local<Function>::New(isolate, function);

  TryCatch tryCatch;

  Handle<Value> result = fun->Call(context->Global(), 5, args);

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
  Isolate* isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // define new global object
  Handle<ObjectTemplate> global = ObjectTemplate::New();

  // define point draw fun
  Local<FunctionTemplate> dpcFun = FunctionTemplate::New(isolate, DrawPointCallback);
  global->Set(isolate, "point", dpcFun);

  Local<FunctionTemplate> dlFun = FunctionTemplate::New(isolate, DrawLineCallback);
  global->Set(isolate, "line", dlFun);

  Local<FunctionTemplate> drFun = FunctionTemplate::New(isolate, DrawRectCallback);
  global->Set(isolate, "rect", drFun);

  Local<FunctionTemplate> logFun = FunctionTemplate::New(isolate, LogCallback);
  global->Set(isolate, "log", logFun);

  // Create a new context.
  Handle<Context> context = Context::New(isolate, NULL, global);

  // Persist context globally
  _context.Reset(isolate, context);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> sourceHandle = String::NewFromUtf8(isolate, source.toStdString().data());

  // Compile the source code.
  Handle<String> scriptFileName = String::NewFromUtf8(isolate, name.toStdString().data());
  TryCatch tryCatch;
  Handle<Script> script = Script::Compile(sourceHandle, scriptFileName);

  if(script.IsEmpty()) {

      // catch compilation errors

      compileError = true;
      ((MainWindow*) parent)->logError(getErrorMessage(isolate, &tryCatch));

      return;

  }

  Handle<Value> result = script->Run();
  if(result.IsEmpty()) {

      // catch runtime errors

      compileError = true;
      ((MainWindow*) parent)->logError(getErrorMessage(isolate, &tryCatch));

      return;

  }

  // retrieve the ondrag function from the global object
  Handle<Object> glob = context->Global();
  Handle<Value> onClickVal = glob->Get(String::NewFromUtf8(isolate, "onClick"));
  Handle<Value> onDragVal = glob->Get(String::NewFromUtf8(isolate, "onDrag"));
  Handle<Value> onReleaseVal = glob->Get(String::NewFromUtf8(isolate, "onRelease"));

  // persist the functions globally
  if (onClickVal->IsFunction()) {
    clickFun.Reset(isolate, Handle<Function>::Cast(onClickVal));
  }

  if (onDragVal->IsFunction()) {
    dragFun.Reset(isolate, Handle<Function>::Cast(onDragVal));
  }

  if (onReleaseVal->IsFunction()) {
    releaseFun.Reset(isolate, Handle<Function>::Cast(onReleaseVal));
  }
}
