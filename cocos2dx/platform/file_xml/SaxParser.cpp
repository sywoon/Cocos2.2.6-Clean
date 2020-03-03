#include "SaxParser.h"
#include "SaxHander.h"
#include "FileUtils.h"


NS_CC_BEGIN


tinyxml2::XMLElement* SAXParser::generateElementForObject(cocos2d::Object* object, tinyxml2::XMLDocument* pDoc)
{
	if (String* str = dynamic_cast<String*>(object))
	{
		tinyxml2::XMLElement* node = pDoc->NewElement("string");
		tinyxml2::XMLText* content = pDoc->NewText(str->getCString());
		node->LinkEndChild(content);
		return node;
	}

	if (Array* array = dynamic_cast<Array*>(object))
		return generateElementForArray(array, pDoc);

	if (Dictionary* innerDict = dynamic_cast<Dictionary*>(object))
		return generateElementForDict(innerDict, pDoc);

	CCLOG("This type cannot appear in property list");
	return NULL;
}

tinyxml2::XMLElement* SAXParser::generateElementForArray(cocos2d::Array* array, tinyxml2::XMLDocument* pDoc)
{
	tinyxml2::XMLElement* rootNode = pDoc->NewElement("array");

	Object* object = NULL;
	ARRAY_FOREACH(array, object)
	{
		tinyxml2::XMLElement* element = generateElementForObject(object, pDoc);
		if (element)
			rootNode->LinkEndChild(element);
	}
	return rootNode;
}


tinyxml2::XMLElement* SAXParser::generateElementForDict(cocos2d::Dictionary* dict, tinyxml2::XMLDocument* pDoc)
{
	tinyxml2::XMLElement* rootNode = pDoc->NewElement("dict");

	DictElement* dictElement = NULL;
	DICT_FOREACH(dict, dictElement)
	{
		tinyxml2::XMLElement* tmpNode = pDoc->NewElement("key");
		rootNode->LinkEndChild(tmpNode);
		tinyxml2::XMLText* content = pDoc->NewText(dictElement->getStrKey());
		tmpNode->LinkEndChild(content);

		Object* object = dictElement->getObject();
		tinyxml2::XMLElement* element = generateElementForObject(object, pDoc);
		if (element)
			rootNode->LinkEndChild(element);
	}
	return rootNode;
}




SAXParser::SAXParser()
	: _pDelegator(NULL)
{
}

SAXParser::~SAXParser(void)
{
}

bool SAXParser::init(const char* pszEncoding)
{
	CC_UNUSED_PARAM(pszEncoding);
	return true;
}

bool SAXParser::parse(const char* pXMLData, unsigned int uDataLength)
{
	tinyxml2::XMLDocument tinyDoc;
	tinyDoc.Parse(pXMLData, uDataLength);

	XmlSaxHander printer;
	printer.setSAXParserImp(this);

	return tinyDoc.Accept(&printer);
}

bool SAXParser::parse(const char* pszFile)
{
	bool bRet = false;
	unsigned long size = 0;
	char* pBuffer = (char*)FileUtils::sharedFileUtils()->getFileData(pszFile, "rt", &size);
	if (pBuffer != NULL && size > 0)
	{
		bRet = parse(pBuffer, size);
	}
	CC_SAFE_DELETE_ARRAY(pBuffer);
	return bRet;
}

void SAXParser::setDelegator(SAXDelegator* pDelegator)
{
	_pDelegator = pDelegator;
}


void SAXParser::startElement(void* ctx, const CC_XML_CHAR* name, const CC_XML_CHAR** atts)
{
	((SAXParser*)(ctx))->_pDelegator->startElement(ctx, (char*)name, (const char**)atts);
}

void SAXParser::endElement(void* ctx, const CC_XML_CHAR* name)
{
	((SAXParser*)(ctx))->_pDelegator->endElement(ctx, (char*)name);
}
void SAXParser::textHandler(void* ctx, const CC_XML_CHAR* name, int len)
{
	((SAXParser*)(ctx))->_pDelegator->textHandler(ctx, (char*)name, len);
}



NS_CC_END
