#pragma once
#include "SaxParser.h"
#include <stack>


NS_CC_BEGIN


class Dictionary;
class Array;

class DictMaker : public SAXDelegator
{
public:
	typedef enum
	{
		SAX_NONE = 0,
		SAX_KEY,
		SAX_DICT,
		SAX_INT,
		SAX_REAL,
		SAX_STRING,
		SAX_ARRAY
	} kSAXState;

	typedef enum
	{
		SAX_RESULT_NONE = 0,
		SAX_RESULT_DICT,
		SAX_RESULT_ARRAY
	} kSAXResult;


public:
	DictMaker();
	~DictMaker();

public:
	Dictionary*		dictionaryWithContentsOfFile(const char* pFileName);
	Array*			arrayWithContentsOfFile(const char* pFileName);

public:
	virtual void	startElement(void* ctx, const char* name, const char** atts);
	virtual void	endElement(void* ctx, const char* name);
	virtual void	textHandler(void* ctx, const char* s, int len);


public:
	Array*		_pRootArray;
	Array*		_pArray;

	kSAXResult _eResultType;
	kSAXState	_tState;
	
	Dictionary* _pRootDict;
	Dictionary* _pCurDict;
	std::stack<Dictionary*> _tDictStack;

	std::string _sCurKey;   ///< parsed key
	std::string _sCurValue; // parsed value
	
	std::stack<Array*>		_tArrayStack;
	std::stack<kSAXState>   _tStateStack;
};



NS_CC_END