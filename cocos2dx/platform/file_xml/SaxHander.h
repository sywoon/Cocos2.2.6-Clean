#pragma once

#include "ccMacros.h"
#include "tinyxml2.h"


NS_CC_BEGIN

class SAXParser;
class XmlSaxHander : public tinyxml2::XMLVisitor
{
public:
	XmlSaxHander();

	virtual bool visitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* firstAttribute);
	virtual bool visitExit(const tinyxml2::XMLElement& element);
	virtual bool visit(const tinyxml2::XMLText& text);
	virtual bool visit(const tinyxml2::XMLUnknown&) { return true; }

	void		 setSAXParserImp(SAXParser* parser);

private:
	SAXParser*	_saxParserImp;
};




NS_CC_END