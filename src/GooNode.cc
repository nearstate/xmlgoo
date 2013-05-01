#include <node.h>
#include <libxml/parser.h>
#include <libxml/xmlstring.h>
#include <v8.h>

#include "GooNode.h"

#include <iostream>

GooNode::GooNode() {
	this->node_ = NULL;
	this->owner_ = NULL;
	this->prev_ = NULL;
	this->next_ = NULL;
	this->isDead_ = false;
}

GooNode::~GooNode() {

	bool shouldFreeNode = (!this->isDead_) // already freed?
		&& (this->node_) // no node to free
		&& ((this->node_->doc == NULL) || (this->node_->parent == NULL)) // node which is not linked to a document tree (although it may be owned by a document)
		;

	if(shouldFreeNode) {
		xmlUnlinkNode(this->node_);
		xmlFreeNode(this->node_);
	}

	this->DetachSelf();

	this->node_ = NULL;
	this->owner_ = NULL;
	this->prev_ = NULL;
	this->next_ = NULL;
}

v8::Handle<v8::Value> GooNode::NewGooNode(const v8::Arguments& args) {
	v8::HandleScope scope;

 	(new GooNode())->Wrap(args.This());

 	return scope.Close(args.This());
}


// Behaviours

void GooNode::DetachSelf() {

	if(this->owner_) this->owner_->OnRemovingGooNode(this);

	// tell prev_ about new next_
	if(this->prev_ ) {
		if(this->next_) {
			this->prev_->SetNextSibling(this->next_);
		} else {
			this->prev_->SetNextSibling(NULL);
		}
	}
	// tell next_ about new prev_
	if(this->next_) {
		if(this->prev_) {
			this->next_->SetPrevSibling(this->prev_);
		} else {
			this->next_->SetPrevSibling(NULL);
		}
	}

	this->prev_ = NULL;
	this->next_ = NULL;
	this->owner_ = NULL;
}

GooNode* GooNode::FindLastGooNode() {
	v8::HandleScope scope;

	if( this->next_ ) {
		return this->next_->FindLastGooNode();
	}
	return this;
}

v8::Handle<v8::Value> GooNode::FindWrapper(xmlNodePtr node) {

	// am I the wrapper?
	if( this->node_ && this->node_ == node ) {
		return this->handle_;
	}
	// can I pass the request on?
	if( this->next_ ) return this->next_->FindWrapper(node);
	// else
	return v8::Undefined();
}

void GooNode::Initialise(xmlNodePtr node, GooDoc* owner) {
	v8::HandleScope scope;

	this->SetNode(node);
	this->SetOwner(owner);
}


// Accessors

bool GooNode::GetIsDead() {
	return this->isDead_;
}

void GooNode::SetIsDead(bool value) {
	this->isDead_ = value;
}

GooNode* GooNode::GetNextSibling() {
	return this->next_;
}

void GooNode::SetNextSibling(GooNode* nextSibling) {
	assert(this != nextSibling);
	this->next_ = nextSibling;
}

xmlNodePtr GooNode::GetNode() {
	return this->node_;
}

void GooNode::SetNode(xmlNodePtr value) {
	this->node_ = value;
	this->SetIsDead(false);
}

GooDoc* GooNode::GetOwner() {
	return this->owner_;
}

void GooNode::SetOwner(GooDoc* owner) {

	this->DetachSelf();

	if(owner) {

		this->owner_ = owner;

		GooNode* nodes = owner->GetGooNodes();
		// if my owner has nodes
		if( nodes )
			// find the last in the list of nodes and set it as my previous link
			this->prev_ = nodes->FindLastGooNode();
		else
			// otherwise - make myself the first node in the linked list
			this->owner_->SetGooNodes(this);

		// I should not already be in the document
		assert(this->prev_ != this);

		// if I have a previous node i am linked to, tell the previous node that I am its next link
		if( this->prev_ ) this->prev_->SetNextSibling(this);

	}
}

void GooNode::SetPrevSibling(GooNode* prevSibling) {
	assert(this != prevSibling);
	this->prev_ = prevSibling;
}
