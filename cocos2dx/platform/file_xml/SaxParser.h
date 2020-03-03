#pragma once
#include "ccMacros.h"
#include "tinyxml2.h"


NS_CC_BEGIN

typedef unsigned char CC_XML_CHAR;


class CC_DLL SAXDelegator
{
public:
	virtual void startElement(void* ctx, const char* name, const char** atts) = 0;
	virtual void endElement(void* ctx, const char* name) = 0;
	virtual void textHandler(void* ctx, const char* s, int len) = 0;
};


class CC_DLL SAXParser
{
public:
	SAXParser();
	~SAXParser(void);


public:
	bool		init(const char* pszEncoding);
	bool		parse(const char* pXMLData, unsigned int uDataLength);
	bool		parse(const char* pszFile);
	void		setDelegator(SAXDelegator* pDelegator);
	

public:
	static void startElement(void* ctx, const CC_XML_CHAR* name, const CC_XML_CHAR** atts);
	static void endElement(void* ctx, const CC_XML_CHAR* name);
	static void textHandler(void* ctx, const CC_XML_CHAR* name, int len);

	static tinyxml2::XMLElement* generateElementForObject(cocos2d::Object* object, tinyxml2::XMLDocument* pDoc);
	static tinyxml2::XMLElement* generateElementForDict(cocos2d::Dictionary* dict, tinyxml2::XMLDocument* pDoc);
	static tinyxml2::XMLElement* generateElementForArray(cocos2d::Array* array, tinyxml2::XMLDocument* pDoc);


private:
	SAXDelegator* _pDelegator;
};




NS_CC_END

