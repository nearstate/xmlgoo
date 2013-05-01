#ifndef NODE_H
#define NODE_H

#include <libxml/parser.h>
#include <v8.h>

#include "GooDoc.h"

class GooDoc;

class GooNode : public node::ObjectWrap {

	public:
		~GooNode();
		GooNode();

		void DetachSelf();
		GooNode* FindLastGooNode();
		v8::Handle<v8::Value> FindWrapper(xmlNodePtr node);
		void Initialise(xmlNodePtr node, GooDoc* owner);

		/*

			This property indicates that the node should not be used any more as it no longer wraps a valid pointer.

			This can happend e.g. when we do a CreateTextNode (creating a wrapper to return to JS), but then appending the
			text node to an element which already contains a text node, in which case libxml will append the text of the second
			node to the first node, and destroy the second node.

			(!)

		*/
		bool GetIsDead();
		void SetIsDead(bool value);

		GooNode* GetNextSibling();
		void SetNextSibling(GooNode* nextSibling);

		xmlNodePtr GetNode();
		void SetNode(xmlNodePtr value);

		GooDoc* GetOwner();
		void SetOwner(GooDoc* value);

		void SetPrevSibling(GooNode* prevSibling);

		static v8::Handle<v8::Value> NewGooNode(const v8::Arguments& args);

	private:
		xmlNodePtr node_;
		GooDoc* owner_;
		GooNode* prev_;
		GooNode* next_;
		bool isDead_;

};

#endif