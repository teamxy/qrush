#include "Brush.h"

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
  painter.setPen(Qt::red);
  painter.drawPoint(x->Value(), y->Value());
}

// TODO: implement lots of more QPainter mappings

void Brush::onDrag(int x, int y){
  // Get the default Isolate created at startup.
  Isolate* isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // define new global object
  Handle<ObjectTemplate> global = ObjectTemplate::New();

  // define point draw fun
  Local<FunctionTemplate> fun = FunctionTemplate::New(isolate, DrawPointCallback);

  // add log function
  global->Set(String::NewFromUtf8(isolate, "drawPoint"), fun);

  // set x and y
  global->Set(String::NewFromUtf8(isolate, "x"), Number::New(isolate, x));
  global->Set(String::NewFromUtf8(isolate, "y"), Number::New(isolate, y));

  // Create a new context.
  Handle<Context> context = Context::New(isolate, NULL, global);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::NewFromUtf8(isolate, script);

  // Compile the source code.
  Handle<Script> script = Script::Compile(source);

  // Run the script to get the result.
  Handle<Value> result = script->Run();
}

Brush::Brush(QImage* _image, std::string file){
  image = _image;

  // TODO: get script from file
  script = "drawPoint(x, y);";
}
