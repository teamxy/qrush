#include "Brush.h"
#include <iostream>
#include <functional>

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

  QPainter painter(image);
  // TODO let the user set the color via JS
  painter.setPen(Qt::red);
  painter.drawPoint(x->Value(), y->Value());
}

// TODO: implement lots of more QPainter mappings

void Brush::onDrag(int x, int y){
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
void Brush::setImage(QImage* _image){
  image = _image;
}

Brush::Brush(char* script){

  // Get the default Isolate created at startup.
  Isolate* isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // define new global object
  Handle<ObjectTemplate> global = ObjectTemplate::New();

  // define point draw fun
  Local<FunctionTemplate> dpcFun = FunctionTemplate::New(isolate, DrawPointCallback);
  global->Set(isolate, "drawPoint", dpcFun);

  // Create a new context.
  Handle<Context> context = Context::New(isolate, NULL, global);

  // Persist context globally
  _context.Reset(isolate, context);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::NewFromUtf8(isolate, script);

  // Compile the source code.
  Handle<Script> s = Script::Compile(source);

  // Run the script
  s->Run();

  // retrieve the ondrag function from the global object
  Handle<Object> glob = context->Global();
  Handle<Value> value = glob->Get(String::NewFromUtf8(isolate, "onDrag"));

  // persist the ondrag function globally
  if (value->IsFunction()) {
    dragFun.Reset(isolate, Handle<Function>::Cast(value));
  }

}
