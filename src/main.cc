#include <node.h>
#include <v8.h>

#include "Goo.h"

using namespace v8;

Handle<Value> Parse(const Arguments& args);

void init(Handle<Object> exports) {

	Goo::Init();

	exports->Set(String::NewSymbol("parse"), FunctionTemplate::New(Parse)->GetFunction());
	exports->Set(String::NewSymbol("_disposeModule"), FunctionTemplate::New(Goo::DisposeModule)->GetFunction());

}

NODE_MODULE(xmlgoo, init)

Handle<Value> Parse(const Arguments& args) {
	HandleScope scope;

	if(args.Length() != 1)
		return scope.Close(ThrowException(Exception::TypeError(String::New("Please pass a single string of XML to parse"))));
	else
		return scope.Close(Goo::BuildGooDocInstance(args[0]));
}