#include <cstring>
#include <node.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>
#include <string>
#include <v8.h>

#include "Goo.h"
#include "GooDoc.h"
#include "GooNode.h"

#include <iostream>

v8::Persistent<v8::Function> Goo::docConstructor;
v8::Persistent<v8::Function> Goo::nodeConstructor;
v8::Persistent<v8::FunctionTemplate> Goo::docConstructorTemplate;
v8::Persistent<v8::FunctionTemplate> Goo::nodeConstructorTemplate;

void Goo::GC() {
	while(!v8::V8::IdleNotification()) {};
}

void Goo::Init() {
	v8::HandleScope scope;

	uint16_t defSize = sizeof(MethodDef);

	MethodDef docMethodDefs[] = {
		{ CloneNodeForDocument,	(char *) "cloneNode"			},
		{ CreateElement,		(char *) "createElement"		},
		{ CreateTextNode,		(char *) "createTextNode"		},
		{ DocumentElement,		(char *) "documentElement"		},
		{ DocXml,				(char *) "xml"					}
	};
	docConstructorTemplate = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(GooDoc::NewGooDocument));
	docConstructor = Goo::BuildConstructor(docConstructorTemplate, (char *) "GooDoc", docMethodDefs, sizeof(docMethodDefs) / defSize);

	MethodDef nodeMethodDefs[] = {
		{ AppendChild,			(char *) "appendChild"			},
		{ CloneNode,			(char *) "cloneNode"			},
		{ ChildNodes,			(char *) "childNodes"			},
		{ GetAttribute,			(char *) "getAttribute"			},
		{ HasAttribute,			(char *) "hasAttribute"			},
		{ InsertBefore,			(char *) "insertBefore"			},
		{ NextSibling,			(char *) "nextSibling"			},
		{ NodeName,				(char *) "nodeName"				},
		{ NodeType,				(char *) "nodeType"				},
		{ NodeValue,			(char *) "nodeValue"			},
		{ NodeXml,				(char *) "xml"					},
		{ OwnerDocument,		(char *) "ownerDocument"		},
		{ ParentNode,			(char *) "parentNode"			},
		{ PreviousSibling,		(char *) "previousSibling"		},
		{ RemoveChild,			(char *) "removeChild"			},
		{ SelectNodes,			(char *) "selectNodes"			},
		{ SelectSingleNode,		(char *) "selectSingleNode"		},
		{ SetAttribute,			(char *) "setAttribute"			}
	};
	nodeConstructorTemplate = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(GooNode::NewGooNode));
	nodeConstructor = Goo::BuildConstructor(nodeConstructorTemplate, (char *) "GooNode", nodeMethodDefs, sizeof(nodeMethodDefs) / defSize);

	xmlGenericErrorFunc errorHandler = (xmlGenericErrorFunc)Goo::HandleGenericErrorDefaultFunc;
	initGenericErrorDefaultFunc(&errorHandler);
}

std::string Goo::lastError_;

#define TMP_BUF_SIZE 4096
void Goo::HandleGenericErrorDefaultFunc(void* ctx, const char* message, ...) {
	char buffer[TMP_BUF_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
	vsnprintf(buffer, TMP_BUF_SIZE, message, arg_ptr);
	va_end(arg_ptr);

	Goo::lastError_.append(buffer);
}

v8::Handle<v8::Value> Goo::DisposeModule(const v8::Arguments& args) {
	v8::HandleScope scope;

	xmlCleanupParser();
	docConstructor.Dispose();
	nodeConstructor.Dispose();

	return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> Goo::BuildGooDocInstance(v8::Handle<v8::Value> xml) {
	v8::HandleScope scope;

	if(!xml->IsString()) return scope.Close(Goo::ThrowTypeError((char*)"Please pass a single string of XML to parse"));
	if(xml->ToString()->Utf8Length() == 0) return scope.Close(Goo::ThrowRangeError((char*)"The supplied XML is invalid: empty string"));

	v8::Handle<v8::Value> argv[] = { };
	v8::Handle<v8::Value> ret = docConstructor->NewInstance(0, argv);
	GooDoc* gooDoc = node::ObjectWrap::Unwrap<GooDoc>(ret->ToObject());
	Goo::lastError_.clear();

	bool success = gooDoc->Initialise(*v8::String::Utf8Value(xml));

	if(!success) {
		ret = v8::Undefined();
		if(!Goo::LibXmlErrorThrown())
			v8::ThrowException(v8::Exception::RangeError(v8::String::New("Parsing of the supplied XML failed")));
	}

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::BuildGooNodeInstance(GooDoc* gDoc, xmlNodePtr node) {
	v8::HandleScope scope;

	// check if we already have an instance for this node
	v8::Handle<v8::Value> instance = v8::Undefined();
	if(gDoc) instance = gDoc->Lookup(node);

	if(instance->IsUndefined()) {
		// build a new node for this object
		v8::Local<v8::Value> argv[] = {};
		instance = nodeConstructor->NewInstance(0, argv);

		// unwrap and initialise the new node
		GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(instance->ToObject());
		gooNode->Initialise(node, gDoc);
	}

	return scope.Close(instance);
}

v8::Persistent<v8::Function> Goo::BuildConstructor(const v8::Handle<v8::FunctionTemplate> constructorTemplate, char* className, const struct MethodDef * methodDefs, const int methodCount) {
	v8::HandleScope scope;

	constructorTemplate->SetClassName(v8::String::NewSymbol(className));
	constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	// methods are only allowed to be invoked against the constructed type of object
	v8::Local<v8::Signature> signature = v8::Signature::New(constructorTemplate);
	// add all methods
	for(int i = 0; i < methodCount; i++) {
		constructorTemplate->PrototypeTemplate()->Set(
			v8::String::NewSymbol(methodDefs[i].symbolName),
			v8::FunctionTemplate::New(methodDefs[i].callback, v8::Handle<v8::Value>(), signature)->GetFunction()
		);
	}

	return v8::Persistent<v8::Function>::New(constructorTemplate->GetFunction());
}



v8::Handle<v8::Value> Goo::AppendChild(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsObject() || !Goo::nodeConstructorTemplate->HasInstance(args[0]->ToObject()))
		return scope.Close(Goo::ThrowRangeError((char*) "Please supply a valid node to append"));

	// unwrap the nodes
	GooNode* gooNodeParent = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	GooNode* gooNodeChild = node::ObjectWrap::Unwrap<GooNode>(args[0]->ToObject());
	GooDoc* gooNodeParentDoc = gooNodeParent->GetOwner();
	GooDoc* gooNodeChildDoc = gooNodeChild->GetOwner();

	xmlNodePtr nodeParent = gooNodeParent->GetNode();
	xmlNodePtr nodeChild = gooNodeChild->GetNode();

	if(nodeParent->type != XML_ELEMENT_NODE)			return scope.Close(Goo::ThrowRangeError((char*) "A child can only be appended to an element"));
	if(gooNodeChildDoc != gooNodeParentDoc)				return scope.Close(Goo::ThrowRangeError((char*) "A child can only be appended to a parent which is part of the same document"));
	if(!gooNodeParentDoc)								return scope.Close(Goo::ThrowRangeError((char*) "A child can not be appended to a parent which is not part of a document"));
	if(Goo::IsSameOrAncestor(nodeChild, nodeParent))	return scope.Close(Goo::ThrowRangeError((char*) "A child can not be appended to itself or an ancestor"));

	v8::Handle<v8::Value> ret;

	xmlNodePtr added = xmlAddChild(nodeParent, nodeChild);
	if(gooNodeChild->GetNode() == added) {
		// node was not merged. Update the gooNode with pointer to its new owner
		assert(gooNodeParentDoc);
		gooNodeChild->SetOwner(gooNodeParentDoc);
		// lookup the gooNode for the added xmlNodePtr (this verifies that we don't somehow have two gooNodes referencing the same xmlNodePtr)
		ret = Goo::NodeOrUndefined(gooNodeParentDoc, added);
		assert(ret == gooNodeChild->handle_);
	} else {
		// appended node was merged with another (and then destroyed)
		gooNodeChild->SetIsDead(true);
		ret = Goo::NodeOrUndefined(gooNodeParentDoc, added);
	}

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::ChildNodes(const v8::Arguments& args) {
	v8::HandleScope scope;

	// create the array to return
	v8::Handle<v8::Array> ret = v8::Array::New();

	// unwrap the node
	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	xmlNodePtr parentNode = gooNode->GetNode();
	if(parentNode->type != XML_ELEMENT_NODE)	return scope.Close(Goo::ThrowRangeError((char*) "childNodes can only be invoked on an element"));
	// get the child nodes to wrap and the document;
	xmlNodePtr childNode = parentNode->children;
	GooDoc* doc = gooNode->GetOwner();

	// for each child node, create a gooNode instance and add to the array
	uint32_t childCount = 0;
	while(childNode) {
		v8::Handle<v8::Value> instance = Goo::BuildGooNodeInstance(doc, childNode);
		ret->Set(childCount, instance);
		childCount++;
		childNode = childNode->next;
	}

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::CloneNode(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsBoolean()) return scope.Close(Goo::ThrowTypeError((char*) "cloneNode expects true or false as its only parameter"));

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	xmlNodePtr nodeToClone = gooNode->GetNode();
	v8::Local<v8::Boolean> isDeep(args[0]->ToBoolean());
	xmlNodePtr node = xmlDocCopyNode(nodeToClone, gooNode->GetOwner()->GetDoc(), (int)(isDeep->Value()));
	v8::Handle<v8::Value> ret = Goo::BuildGooNodeInstance(gooNode->GetOwner(), node);

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::CloneNodeForDocument(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsObject() || !nodeConstructorTemplate->HasInstance(args[0]->ToObject()))
		return scope.Close(Goo::ThrowTypeError((char*) "cloneNode expects a (foreign) node to clone"));

	// unwrap
	GooDoc* gooDocCloner = node::ObjectWrap::Unwrap<GooDoc>(args.Holder());
	GooNode* gooNodeToClone = node::ObjectWrap::Unwrap<GooNode>(args[0]->ToObject());

	xmlDocPtr docCloner = gooDocCloner->GetDoc();
	xmlNodePtr nodeToClone = gooNodeToClone->GetNode();

	if(nodeToClone->doc == docCloner)
		return scope.Close(Goo::ThrowRangeError((char *) "to clone a node within the same document, you must call cloneNode on the node itself"));

	xmlNodePtr node = xmlDocCopyNode(nodeToClone, docCloner, 1);
	v8::Handle<v8::Value> ret = Goo::BuildGooNodeInstance(gooDocCloner, node);

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::CreateElement(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsString()) return scope.Close(Goo::ThrowTypeError((char*)"Please supply a single string as the element's node name"));
	if(args[0]->ToString()->Utf8Length() == 0) return scope.Close(Goo::ThrowRangeError((char*)"Invalid name specified"));

	v8::Handle<v8::Value> ret = v8::Undefined();

	GooDoc* gooDoc = node::ObjectWrap::Unwrap<GooDoc>(args.Holder());
	v8::String::Utf8Value elementName(args[0]);

	if(!Goo::ValidateTagName(*elementName)) return scope.Close(Goo::ThrowRangeError((char*)"Invalid name specified"));

	Goo::lastError_.clear();
	xmlNodePtr node = xmlNewDocNode(gooDoc->GetDoc(), NULL, BAD_CAST *elementName, NULL);
	if(!Goo::LibXmlErrorThrown())
		ret = Goo::BuildGooNodeInstance(gooDoc, node);

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::CreateTextNode(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsString()) return scope.Close(Goo::ThrowTypeError((char*) "Please supply a single string of text to create a node for"));

	GooDoc* gooDoc = node::ObjectWrap::Unwrap<GooDoc>(args.Holder());
	v8::String::Utf8Value textValue(args[0]);
	xmlNodePtr node = xmlNewDocText(gooDoc->GetDoc(), BAD_CAST *textValue);

	return scope.Close(Goo::BuildGooNodeInstance(gooDoc, node));
}

v8::Handle<v8::Value> Goo::DocumentElement(const v8::Arguments& args) {
 	v8::HandleScope scope;
 	// unwrap the document
 	GooDoc* gDoc = node::ObjectWrap::Unwrap<GooDoc>(args.Holder());
 	// get the document element
 	xmlNodePtr docElement = xmlDocGetRootElement(gDoc->GetDoc());
 	// node instance to return
 	v8::Handle<v8::Value> ret = Goo::BuildGooNodeInstance(gDoc, docElement);

 	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::DocXml(const v8::Arguments& args) {
	v8::HandleScope scope;

	// unwrap the GooDoc
	GooDoc* gDoc = node::ObjectWrap::Unwrap<GooDoc>(args.Holder());
	// get the document and document element

	xmlDocPtr doc = gDoc->GetDoc();
	xmlNodePtr docElement = xmlDocGetRootElement(doc);

	return scope.Close(Goo::Xml(doc, docElement));
}

v8::Handle<v8::Value> Goo::EvaluateXPath(const v8::Arguments& args, v8::Handle<v8::Value> (*valueExtractor)(GooDoc*, xmlNodeSet*)) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsString())
		return scope.Close(Goo::ThrowTypeError((char*)"Please supply a valid xpath expression"));

	v8::Handle<v8::Value> ret = v8::Undefined();

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	GooDoc* gooDoc = gooNode->GetOwner();

	if(gooDoc) {
		xmlXPathContextPtr xpathContext = xmlXPathNewContext(gooDoc->GetDoc());
		xpathContext->node = gooNode->GetNode();
		v8::String::Utf8Value xpathExpression(args[0]);
		xmlXPathObjectPtr xpathResult = xmlXPathEval(BAD_CAST *xpathExpression, xpathContext);

		ret = valueExtractor(gooDoc, (xpathResult ? xpathResult->nodesetval : NULL));

		xmlXPathFreeObject(xpathResult);
		xmlXPathFreeContext(xpathContext);

	} else {
		ret = v8::ThrowException(v8::Exception::Error(v8::String::New("XPath evaluation is not supported on detached nodes")));
	}

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::GetAttribute(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length() < 1 || !args[0]->IsString() || args[0]->ToString()->Length() < 1)
		return scope.Close(ThrowTypeError((char*) "Please specify the attribute name"));

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	xmlNodePtr parentNode = gooNode->GetNode();
	if(parentNode->type!=XML_ELEMENT_NODE) return scope.Close(Goo::ThrowRangeError((char*) "getAttribute can only be invoked on an element"));

	v8::String::Utf8Value attributeName(args[0]);
	xmlChar* attributeXmlContent = xmlGetProp(gooNode->GetNode(), BAD_CAST *attributeName);

	v8::Handle<v8::Value> ret = v8::Undefined();
	if(attributeXmlContent!=NULL) {
		ret = v8::String::New((char*) attributeXmlContent);
		xmlFree(attributeXmlContent);
	}

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::HasAttribute(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length() < 1 || !args[0]->IsString() || args[0]->ToString()->Length() < 1)
		return scope.Close(ThrowTypeError((char*) "Please specify the attribute name"));

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	xmlNodePtr parentNode = gooNode->GetNode();
	if(parentNode->type!=XML_ELEMENT_NODE) return scope.Close(Goo::ThrowRangeError((char*) "hasAttribute can only be invoked on an element"));

	v8::String::Utf8Value attributeName(args[0]);
	xmlChar* attributeXmlContent = xmlGetProp(parentNode, BAD_CAST *attributeName);

	v8::Handle<v8::Value> ret = v8::Boolean::New(false);
	if(attributeXmlContent!=NULL) {
		ret = v8::Boolean::New(true);
		xmlFree(attributeXmlContent);
	}

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::InsertBefore(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsObject() || !Goo::nodeConstructorTemplate->HasInstance(args[0]->ToObject()))
		return scope.Close(Goo::ThrowRangeError((char*) "Please supply a valid node to insert"));

	// unwrap the nodes
	GooNode* gooNodeToInsert = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	GooNode* gooSiblingNode = node::ObjectWrap::Unwrap<GooNode>(args[0]->ToObject());
	GooDoc* gooNodeToInsertDoc = gooNodeToInsert->GetOwner();
	GooDoc* gooSiblingNodeDoc = gooSiblingNode->GetOwner();

	xmlNodePtr nodeToInsert = gooNodeToInsert->GetNode();
	xmlNodePtr nodeSibling = gooSiblingNode->GetNode();

	if(Goo::IsDocumentElement(nodeSibling))					return scope.Close(Goo::ThrowRangeError((char*) "A node cannot be inserted before a document element"));
	if(gooNodeToInsertDoc != gooSiblingNodeDoc)				return scope.Close(Goo::ThrowRangeError((char*) "A node can only be inserted before another node owned by the same document"));
	if(!gooSiblingNodeDoc)									return scope.Close(Goo::ThrowRangeError((char*) "A node can not be inserted before a node which is not part of a document"));
	if(!nodeSibling->parent)								return scope.Close(Goo::ThrowRangeError((char*) "A node can not be inserted before a node which has no parent"));
	if(Goo::IsSameOrAncestor(nodeSibling, nodeToInsert))	return scope.Close(Goo::ThrowRangeError((char*) "A node can not be inserted before itself or an ancestor"));
	if(Goo::IsSameOrAncestor(nodeToInsert, nodeSibling))	return scope.Close(Goo::ThrowRangeError((char*) "A node can not be inserted before itself or a descendant"));

	v8::Handle<v8::Value> ret;

	xmlNodePtr added = xmlAddPrevSibling(nodeSibling, nodeToInsert);
	if(gooNodeToInsert->GetNode() == added) {
		// node was not merged. Update the gooNode with pointer to its new owner
		assert(gooSiblingNodeDoc);
		gooNodeToInsert->SetOwner(gooSiblingNodeDoc);
		// lookup the gooNode for the added xmlNodePtr (this verifies that we don't somehow have two gooNodes referencing the same xmlNodePtr)
		ret = Goo::NodeOrUndefined(gooSiblingNodeDoc, added);
		assert(ret == gooNodeToInsert->handle_);
	} else {
		// appended node was merged with another (and then destroyed)
		gooNodeToInsert->SetIsDead(true);
		ret = Goo::NodeOrUndefined(gooSiblingNodeDoc, added);
	}

	return scope.Close(ret);
}

bool Goo::IsDocumentElement(xmlNodePtr candidate) {
	return (candidate) && (candidate->doc) && (candidate == candidate->doc->children);
}

bool Goo::IsSameOrAncestor(xmlNodePtr candidate, xmlNodePtr toMatch) {
	while(toMatch && toMatch!=candidate) toMatch = toMatch->parent;
	return toMatch == candidate;
}

bool Goo::LibXmlErrorThrown() {
	if(Goo::lastError_.empty()) return false;
	v8::ThrowException(v8::Exception::RangeError(v8::String::New(Goo::lastError_.c_str())));
	Goo::lastError_.clear();
	return true;
}

v8::Handle<v8::Value> Goo::NextSibling(const v8::Arguments& args) {
	v8::HandleScope scope;

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());

	xmlNodePtr node = gooNode->GetNode();
	if(Goo::IsDocumentElement(node)) return scope.Close(Goo::ThrowTypeError((char*) "nextSibling can not be called on the document element"));
	// to return
	xmlNodePtr sibling = node->next;

	return scope.Close(NodeOrUndefined(gooNode->GetOwner(), sibling));
}

v8::Handle<v8::Value> Goo::NodeName(const v8::Arguments& args) {
	v8::HandleScope scope;

	// unwrap the node
	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());

	return scope.Close(v8::String::New((char*)(gooNode->GetNode()->name)));
}

v8::Handle<v8::Value> Goo::NodeOrUndefined(GooDoc* owner, xmlNodePtr node) {
	v8::HandleScope scope;

	if(node==NULL)
		return v8::Undefined();
	else
		return Goo::BuildGooNodeInstance(owner, node);
}

v8::Handle<v8::Value> Goo::NodeType(const v8::Arguments& args) {
	v8::HandleScope scope;

	// unwrap the node
	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());

	return scope.Close(v8::Number::New(gooNode->GetNode()->type));
}

v8::Handle<v8::Value> Goo::NodeValue(const v8::Arguments& args) {
	v8::HandleScope scope;

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	char* nodeContent = (char*) xmlNodeGetContent(gooNode->GetNode());
	v8::Local<v8::String> content = v8::String::New(nodeContent);
	xmlFree(nodeContent);

	return scope.Close(content);
}

v8::Handle<v8::Value> Goo::NodeXml(const v8::Arguments& args) {
 	v8::HandleScope scope;
 	// unwrap the Node
 	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
 	// get the node and it's document
 	xmlNodePtr node = gooNode->GetNode();
 	xmlDocPtr doc = node->doc;
	// close the scope and return
	return scope.Close(Goo::Xml(doc, node));
}

v8::Handle<v8::Value> Goo::OwnerDocument(const v8::Arguments& args) {
	v8::HandleScope scope;
	v8::Handle<v8::Value> ret = v8::Undefined();

	// unwrap the node
	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	// get the doc
	GooDoc* doc = gooNode->GetOwner();

	if(doc) ret = doc->handle_;

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::ParentNode(const v8::Arguments& args) {
	v8::HandleScope scope;

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
 	GooDoc* gooDoc = gooNode->GetOwner();

 	// node instance to return
 	xmlNodePtr node = gooNode->GetNode();

 	v8::Handle<v8::Value> ret = v8::Undefined();

 	if(node->parent) {
		if (gooDoc && (node->parent == (xmlNode*)gooDoc->GetDoc())) {
			ret = gooDoc->handle_;
		} else {
			ret = Goo::BuildGooNodeInstance(gooDoc, node->parent);
		}
	}

 	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::PreviousSibling(const v8::Arguments& args) {
	v8::HandleScope scope;

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());

	xmlNodePtr node = gooNode->GetNode();
	if(Goo::IsDocumentElement(node)) return scope.Close(Goo::ThrowTypeError((char*) "previousSibling can not be called on the document element"));
	// to return
	xmlNodePtr sibling = gooNode->GetNode()->prev;

	return scope.Close(NodeOrUndefined(gooNode->GetOwner(), sibling));
}

v8::Handle<v8::Value> Goo::RemoveChild(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=1 || !args[0]->IsObject() || !Goo::nodeConstructorTemplate->HasInstance(args[0]->ToObject()))
		return scope.Close(Goo::ThrowRangeError((char*) "Please supply a valid node to remove"));

	GooNode* gooNodeParent = node::ObjectWrap::Unwrap<GooNode>(args.Holder());
	GooNode* gooNodeChild = node::ObjectWrap::Unwrap<GooNode>(args[0]->ToObject());
	xmlNodePtr parentNode = gooNodeParent->GetNode();
	xmlNodePtr childNode = gooNodeChild->GetNode();

	if(parentNode->type!=XML_ELEMENT_NODE) return scope.Close(Goo::ThrowRangeError((char*) "getAttribute can only be invoked on an element"));

	v8::Handle<v8::Value> ret;

	if(parentNode != childNode->parent) {
		ret = v8::ThrowException(v8::Exception::Error(v8::String::New("A child can only be removed from its parent")));
	} else {
		xmlUnlinkNode(childNode);
		ret = v8::Undefined();
	}

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::SelectNodes(const v8::Arguments& args) {
	return Goo::EvaluateXPath(args, &Goo::SelectNodesValueExtractor);
}

v8::Handle<v8::Value> Goo::SelectNodesValueExtractor(GooDoc* gooDoc, xmlNodeSet* nodeSet) {
	v8::HandleScope scope;
	v8::Handle<v8::Array> ret = v8::Array::New();

	if(nodeSet)
		for(int i = 0; i < nodeSet->nodeNr; i++)
			ret->Set(i, BuildGooNodeInstance(gooDoc, nodeSet->nodeTab[i]));

	return scope.Close(ret);
}

v8::Handle<v8::Value> Goo::SelectSingleNode(const v8::Arguments& args) {
	return Goo::EvaluateXPath(args, &Goo::SelectSingleNodeValueExtractor);
}

v8::Handle<v8::Value> Goo::SelectSingleNodeValueExtractor(GooDoc* gooDoc, xmlNodeSet* nodeSet) {
	if(nodeSet && nodeSet->nodeTab && nodeSet->nodeTab[0])
		return BuildGooNodeInstance(gooDoc, nodeSet->nodeTab[0]);
	else
		return v8::Undefined();
}

v8::Handle<v8::Value> Goo::SetAttribute(const v8::Arguments& args) {
	v8::HandleScope scope;

	if(args.Length()!=2) return scope.Close(Goo::ThrowTypeError((char*) "Please specify the attribute name and value to set as two string parameters to setAttribute"));
	if(!args[0]->IsString() || args[0]->ToString()->Length() < 1) return scope.Close(Goo::ThrowTypeError((char*) "Please specify a string as the attribute name"));
	if(!args[1]->IsString()) return scope.Close(Goo::ThrowTypeError((char*) "Please specify a string as the value to set"));

	v8::Handle<v8::Value> ret;

	GooNode* gooNode = node::ObjectWrap::Unwrap<GooNode>(args.Holder());

	if(gooNode->GetNode()->type == XML_ELEMENT_NODE) {
		v8::String::Utf8Value attributeName(args[0]);
		v8::String::Utf8Value attributeValue(args[1]);
		xmlNodePtr node = gooNode->GetNode();

		xmlAttrPtr attr = xmlHasProp(node, BAD_CAST *attributeName);
		if(attr == NULL) {
			xmlNewProp(node, BAD_CAST *attributeName, BAD_CAST *attributeValue);
		} else {
			xmlSetProp(node, BAD_CAST *attributeName, BAD_CAST *attributeValue);
		}

		ret = Goo::GetAttribute(args);
	} else {
		ret = v8::ThrowException(v8::Exception::Error(v8::String::New("Set attribute can only be invoked on an element node")));
	}

	return scope.Close(ret);
}

v8::Handle<v8::Primitive> Goo::ThrowRangeError(char* message) {
	v8::ThrowException(v8::Exception::RangeError(v8::String::New(message)));
	return v8::Undefined();
}

v8::Handle<v8::Primitive> Goo::ThrowTypeError(char* message) {
	v8::ThrowException(v8::Exception::TypeError(v8::String::New(message)));
	return v8::Undefined();
}

bool Goo::ValidateTagName(const char* tagName) {
	const char* charCheck = tagName;
	while(*charCheck) {
		if(strchr("<>", *charCheck)) return false;
		charCheck++;
	}
	return true;
}

v8::Handle<v8::Value> Goo::Xml(const xmlDocPtr doc, const xmlNodePtr node) {
	v8::HandleScope scope;

	v8::Handle<v8::Value> ret;

	// create a buffer
	xmlBuffer* buffer = xmlBufferCreate();

	try {

		// dump to buffer
		xmlNodeDump(buffer, doc, node, 0, 0);
		// convert to return value
		ret = v8::Handle<v8::Value>(v8::String::New( (char*) buffer->content ));
		// deallocate the buffer
		xmlBufferFree(buffer);

	} catch(...) {

		// deallocate the buffer and rethrow
		xmlBufferFree(buffer);
		throw;

	}

	return scope.Close(ret);
}