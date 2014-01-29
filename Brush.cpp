#include <iostream>
#include <functional>

#include <QApplication>

#include "Brush.h"
#include "MainWindow.h"

using namespace v8;

// needs to be static for now :/
// TODO improve this
QImage* image;

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

// TODO: implement lots of more QPainter mappings

void Brush::onDrag(int x, int y) {
  if(compileError)
      return;

  // Get the default Isolate created at startup.
  Isolate* isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // Create a new local context.
  Local<Context> context = Local<Context>::New(isolate, _context);

  Handle<Value> args[2];
  args[0] = Number::New(isolate, x);
  args[1] = Number::New(isolate, y);

  Local<Function> fun = Local<Function>::New(isolate, dragFun);

  fun->Call(context->Global(), 2, args);
}

// TODO: make this static?
void Brush::setImage(QImage* _image) {
  image = _image;
}

Brush::Brush(QObject* parent, QString source) : compileError(false) {

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

  // Create a new context.
  Handle<Context> context = Context::New(isolate, NULL, global);

  // Persist context globally
  _context.Reset(isolate, context);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> sourceHandle = String::NewFromUtf8(isolate, source.toStdString().data());

  // Compile the source code.
  v8::TryCatch try_catch;
  Handle<Script> script = Script::Compile(sourceHandle);

  if(script.IsEmpty()) {

      // errors during compilation

      compileError = true;

      v8::HandleScope handle_scope(isolate);
      v8::String::Utf8Value exception(try_catch.Exception());
      const char* exceptionStr = *exception;

      QString errorMessage;
      errorMessage.append("<font color='red'>");
      errorMessage.append(exceptionStr);
      errorMessage.append("</font>");
      ((MainWindow*) parent)->log(errorMessage);

      return;

  }

  std::cout << "compiled" << std::endl;
  // Run the script
  script->Run();

  // retrieve the ondrag function from the global object
  Handle<Object> glob = context->Global();
  Handle<Value> value = glob->Get(String::NewFromUtf8(isolate, "onDrag"));

  // persist the ondrag function globally
  if (value->IsFunction()) {
    dragFun.Reset(isolate, Handle<Function>::Cast(value));
  }
}
