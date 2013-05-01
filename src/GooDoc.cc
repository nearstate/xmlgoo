#include <node.h>
#include <libxml/parser.h>
#include <libxml/xmlstring.h>
#include <map>
#include <v8.h>

#include "GooDoc.h"
#include "Goo.h"

#include <iostream>

GooDoc::GooDoc() {
	this->gooNodes_ = NULL;
}

GooDoc::~GooDoc() {
	v8::HandleScope scope;
	xmlFreeDoc(this->doc_);
	this->gooNodes_ = NULL;
}

bool GooDoc::Initialise(char* xml) {
	this->doc_ = xmlParseDoc(BAD_CAST xml);
	if(!(this->doc_)) return false;

	Goo::BuildGooNodeInstance(this, this->doc_->children);
	return true;
}

v8::Handle<v8::Value> GooDoc::NewGooDocument(const v8::Arguments& args) {
	v8::HandleScope scope;

	GooDoc* gDoc = new GooDoc();

	gDoc->Wrap(args.This());

	// disable normal garbage collection until all nodes are disposed of
	gDoc->Ref();
	gDoc->handle_.MakeWeak(gDoc, WeakCallbackWithDependents);

	gDoc = NULL;

	return scope.Close(args.This());
}

GooNode* GooDoc::GetGooNodes() {
	return this->gooNodes_;
}

void GooDoc::SetGooNodes(GooNode* gooNode) {
	this->gooNodes_ = gooNode;
}

xmlDocPtr GooDoc::GetDoc() {
	return this->doc_;
}

void GooDoc::OnRemovingGooNode(GooNode* toRemove) {
	if( this->gooNodes_ == toRemove )
		this->gooNodes_ = this->gooNodes_->GetNextSibling();
}

v8::Handle<v8::Value> GooDoc::Lookup(xmlNodePtr node) {
	if( !this->gooNodes_ )
		return v8::Undefined();
	return this->gooNodes_->FindWrapper(node);
}

void GooDoc::WeakCallbackWithDependents(v8::Persistent<v8::Value> handle, void* data) {

	GooDoc* gDoc = (GooDoc*) data;
	if( gDoc->GetGooNodes() )
		handle.MakeWeak(gDoc, WeakCallbackWithDependents);
	else
		gDoc->Unref(); // resume normal garbage collection
}