#include "SaxHander.h"
#include "SaxParser.h"
#include <vector>


NS_CC_BEGIN


XmlSaxHander::XmlSaxHander()
	: _saxParserImp(0) 
{}

void XmlSaxHander::setSAXParserImp(SAXParser* parser)
{
	_saxParserImp = parser;
}


bool XmlSaxHander::visitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* firstAttribute)
{
	std::vector<const char*> attsVector;
	for (const tinyxml2::XMLAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next())
	{
		attsVector.push_back(attrib->Name());
		attsVector.push_back(attrib->Value());
	}

	// nullptr is used in c++11
	//attsVector.push_back(nullptr);
	attsVector.push_back(NULL);

	SAXParser::startElement(_saxParserImp, (const CC_XML_CHAR*)element.Value(), (const CC_XML_CHAR**)(&attsVector[0]));
	return true;
}

bool XmlSaxHander::visitExit(const tinyxml2::XMLElement& element)
{
	SAXParser::endElement(_saxParserImp, (const CC_XML_CHAR*)element.Value());
	return true;
}

bool XmlSaxHander::visit(const tinyxml2::XMLText& text)
{
	SAXParser::textHandler(_saxParserImp, (const CC_XML_CHAR*)text.Value(), strlen(text.Value()));
	return true;
}




NS_CC_END
