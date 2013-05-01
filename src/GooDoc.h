#ifndef DOC_H
#define DOC_H

#include <libxml/parser.h>
#include <node.h>
#include <v8.h>

#include "GooNode.h"

class GooNode;

class GooDoc : public node::ObjectWrap {

	public:
		~GooDoc();
		GooDoc();

		bool Initialise(char* xml);
		v8::Handle<v8::Value> Lookup(xmlNodePtr node);
		void OnRemovingGooNode(GooNode* toRemove);

		xmlDocPtr GetDoc();
		GooNode* GetGooNodes();
		void SetGooNodes(GooNode* gooNode);


		static v8::Handle<v8::Value> NewGooDocument(const v8::Arguments& args);
		static void WeakCallbackWithDependents(v8::Persistent<v8::Value> handle, void* gDoc);

	private:
		xmlDocPtr doc_;
		GooNode* gooNodes_;

};

#endif