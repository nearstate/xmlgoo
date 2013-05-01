#ifndef GOO_H
#define GOO_H

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string>
#include <v8.h>

#include "GooDoc.h"

struct MethodDef {
	v8::InvocationCallback callback;
	char* symbolName;
};

class Goo {

	public:
		static void GC();
		static void Init();
		static v8::Handle<v8::Value> DisposeModule(const v8::Arguments& args);
		static v8::Handle<v8::Value> BuildGooDocInstance(v8::Handle<v8::Value> xml);
		static v8::Handle<v8::Value> BuildGooNodeInstance(GooDoc* doc, xmlNodePtr node);

	private:
		static v8::Persistent<v8::Function> BuildConstructor(const v8::Handle<v8::FunctionTemplate> constructorTemplate, char* className, const struct MethodDef * methodDefs, const int methodCount);
		static v8::Persistent<v8::Function> docConstructor;
		static v8::Persistent<v8::Function> nodeConstructor;
		static v8::Persistent<v8::FunctionTemplate> docConstructorTemplate;
		static v8::Persistent<v8::FunctionTemplate> nodeConstructorTemplate;
		static v8::Handle<v8::Value> NewGooDocument(const v8::Arguments& args);
		static v8::Handle<v8::Value> NewGooNode(const v8::Arguments& args);

		static v8::Handle<v8::Value> AppendChild(const v8::Arguments& args);
		static v8::Handle<v8::Primitive> ThrowTypeError(char* message);
		static v8::Handle<v8::Primitive> ThrowRangeError(char* message);
		static v8::Handle<v8::Value> ChildNodes(const v8::Arguments& args);
		static v8::Handle<v8::Value> CloneNode(const v8::Arguments& args);
		static v8::Handle<v8::Value> CloneNodeForDocument(const v8::Arguments& args);
		static v8::Handle<v8::Value> CreateElement(const v8::Arguments& args);
		static v8::Handle<v8::Value> CreateTextNode(const v8::Arguments& args);
		static v8::Handle<v8::Value> DocumentElement(const v8::Arguments& args);
		static v8::Handle<v8::Value> DocXml(const v8::Arguments& args);
		static v8::Handle<v8::Value> EvaluateXPath(const v8::Arguments& args, v8::Handle<v8::Value> (*valueExtractor)(GooDoc*, xmlNodeSet*));
		static v8::Handle<v8::Value> GetAttributeNode(const v8::Arguments& args);
		static v8::Handle<v8::Value> GetAttribute(const v8::Arguments& args);
		static void HandleGenericErrorDefaultFunc(void* ctx, const char* message, ...);
		static v8::Handle<v8::Value> HasAttribute(const v8::Arguments& args);
		static v8::Handle<v8::Value> InsertBefore(const v8::Arguments& args);
		static bool IsDocumentElement(xmlNodePtr candidate);
		static bool IsSameOrAncestor(xmlNodePtr candidate, xmlNodePtr toMatch);
		static bool LibXmlErrorThrown();
		static v8::Handle<v8::Value> NextSibling(const v8::Arguments& args);
		static v8::Handle<v8::Value> NodeName(const v8::Arguments& args);
		static v8::Handle<v8::Value> NodeOrUndefined(GooDoc* owner, xmlNodePtr node);
		static v8::Handle<v8::Value> NodeType(const v8::Arguments& args);
		static v8::Handle<v8::Value> NodeValue(const v8::Arguments& args);
		static v8::Handle<v8::Value> NodeXml(const v8::Arguments& args);
		static v8::Handle<v8::Value> RemoveChild(const v8::Arguments& args);
		static v8::Handle<v8::Value> OwnerDocument(const v8::Arguments& args);
		static v8::Handle<v8::Value> ParentNode(const v8::Arguments& args);
		static v8::Handle<v8::Value> PreviousSibling(const v8::Arguments& args);
		static v8::Handle<v8::Value> SelectNodes(const v8::Arguments& args);
		static v8::Handle<v8::Value> SelectNodesValueExtractor(GooDoc* gooDoc, xmlNodeSet* nodeTab);
		static v8::Handle<v8::Value> SelectSingleNode(const v8::Arguments& args);
		static v8::Handle<v8::Value> SelectSingleNodeValueExtractor(GooDoc* gooDoc, xmlNodeSet* nodeTab);
		static v8::Handle<v8::Value> SetAttribute(const v8::Arguments& args);
		static bool ValidateTagName(const char* tagName);
		static v8::Handle<v8::Value> Xml(xmlDocPtr doc, xmlNodePtr node);

		static std::string lastError_;
};

#endif