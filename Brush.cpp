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
  TryCatch tryCatch;

  Handle<Value> result = fun->Call(context->Global(), 2, args);

  // TODO: Catch runtime exceptions

  /*
  if(result.IsEmpty()) {

      //String::Utf8Value exception(tryCatch.Exception());

      // catch runtime errors
      std::cout << "runtime errrrrror:" << std::endl;

      compileError = true;
      //((MainWindow*) parent)->logError(getErrorMessage(isolate, &tryCatch));

      return;

  }*/
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

  // Create a new context.
  Handle<Context> context = Context::New(isolate, NULL, global);

  // Persist context globally
  _context.Reset(isolate, context);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> sourceHandle = String::NewFromUtf8(isolate, source.toStdString().data());

  // Compile the source code.
  Handle<String> scriptFileName = String::NewFromUtf8(isolate, name.toStdString().data());;
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
  Handle<Value> value = glob->Get(String::NewFromUtf8(isolate, "onDrag"));

  // persist the ondrag function globally
  if (value->IsFunction()) {
    dragFun.Reset(isolate, Handle<Function>::Cast(value));
  }
}
