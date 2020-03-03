#include "DictMaker.h"
#include "cocoa/Array.h"
#include "cocoa/Dictionary.h"



NS_CC_BEGIN

DictMaker::DictMaker()
	: _eResultType(SAX_RESULT_NONE)
	, _tState(SAX_NONE)
	, _pRootArray(NULL)
	, _pArray(NULL)
	, _pRootDict(NULL)
	, _pCurDict(NULL)
{
}


DictMaker::~DictMaker()
{
}


Dictionary* DictMaker::dictionaryWithContentsOfFile(const char* pFileName)
{
	_eResultType = SAX_RESULT_DICT;
	SAXParser parser;

	if (false == parser.init("UTF-8"))
	{
		return NULL;
	}
	parser.setDelegator(this);

	parser.parse(pFileName);
	return _pRootDict;
}


Array* DictMaker::arrayWithContentsOfFile(const char* pFileName)
{
	_eResultType = SAX_RESULT_ARRAY;

	SAXParser parser;
	if (false == parser.init("UTF-8"))
		return NULL;

	parser.setDelegator(this);
	parser.parse(pFileName);

	return _pArray;
}


void DictMaker::startElement(void* ctx, const char* name, const char** atts)
{
	CC_UNUSED_PARAM(ctx);
	CC_UNUSED_PARAM(atts);
	std::string sName((char*)name);
	if (sName == "dict")
	{
		_pCurDict = new Dictionary();
		if (_eResultType == SAX_RESULT_DICT && _pRootDict == NULL)
		{
			// Because it will call m_pCurDict->release() later, so retain here.
			_pRootDict = _pCurDict;
			_pRootDict->retain();
		}
		_tState = SAX_DICT;

		kSAXState preState = SAX_NONE;
		if (!_tStateStack.empty())
		{
			preState = _tStateStack.top();
		}

		if (SAX_ARRAY == preState)
		{
			// add the dictionary into the array
			_pArray->addObject(_pCurDict);
		}
		else if (SAX_DICT == preState)
		{
			// add the dictionary into the pre dictionary
			CCAssert(!_tDictStack.empty(), "The state is wrong!");
			Dictionary* pPreDict = _tDictStack.top();
			pPreDict->setObject(_pCurDict, _sCurKey.c_str());
		}

		_pCurDict->release();

		// record the dict state
		_tStateStack.push(_tState);
		_tDictStack.push(_pCurDict);
	}
	else if (sName == "key")
	{
		_tState = SAX_KEY;
	}
	else if (sName == "integer")
	{
		_tState = SAX_INT;
	}
	else if (sName == "real")
	{
		_tState = SAX_REAL;
	}
	else if (sName == "string")
	{
		_tState = SAX_STRING;
	}
	else if (sName == "array")
	{
		_tState = SAX_ARRAY;
		_pArray = new Array();
		if (_eResultType == SAX_RESULT_ARRAY && _pRootArray == NULL)
		{
			_pRootArray = _pArray;
			_pRootArray->retain();
		}
		kSAXState preState = SAX_NONE;
		if (!_tStateStack.empty())
		{
			preState = _tStateStack.top();
		}

		if (preState == SAX_DICT)
		{
			_pCurDict->setObject(_pArray, _sCurKey.c_str());
		}
		else if (preState == SAX_ARRAY)
		{
			CCAssert(!_tArrayStack.empty(), "The state is wrong!");
			Array* pPreArray = _tArrayStack.top();
			pPreArray->addObject(_pArray);
		}
		_pArray->release();
		// record the array state
		_tStateStack.push(_tState);
		_tArrayStack.push(_pArray);
	}
	else
	{
		_tState = SAX_NONE;
	}
}

void DictMaker::endElement(void* ctx, const char* name)
{
	CC_UNUSED_PARAM(ctx);
	kSAXState curState = _tStateStack.empty() ? SAX_DICT : _tStateStack.top();
	std::string sName((char*)name);
	if (sName == "dict")
	{
		_tStateStack.pop();
		_tDictStack.pop();
		if (!_tDictStack.empty())
		{
			_pCurDict = _tDictStack.top();
		}
	}
	else if (sName == "array")
	{
		_tStateStack.pop();
		_tArrayStack.pop();
		if (!_tArrayStack.empty())
		{
			_pArray = _tArrayStack.top();
		}
	}
	else if (sName == "true")
	{
		String* str = new String("1");
		if (SAX_ARRAY == curState)
		{
			_pArray->addObject(str);
		}
		else if (SAX_DICT == curState)
		{
			_pCurDict->setObject(str, _sCurKey.c_str());
		}
		str->release();
	}
	else if (sName == "false")
	{
		String* str = new String("0");
		if (SAX_ARRAY == curState)
		{
			_pArray->addObject(str);
		}
		else if (SAX_DICT == curState)
		{
			_pCurDict->setObject(str, _sCurKey.c_str());
		}
		str->release();
	}
	else if (sName == "string" || sName == "integer" || sName == "real")
	{
		String* pStrValue = new String(_sCurValue);

		if (SAX_ARRAY == curState)
		{
			_pArray->addObject(pStrValue);
		}
		else if (SAX_DICT == curState)
		{
			_pCurDict->setObject(pStrValue, _sCurKey.c_str());
		}

		pStrValue->release();
		_sCurValue.clear();
	}

	_tState = SAX_NONE;
}

void DictMaker::textHandler(void* ctx, const char* ch, int len)
{
	CC_UNUSED_PARAM(ctx);
	if (_tState == SAX_NONE)
		return;

	kSAXState curState = _tStateStack.empty() ? SAX_DICT : _tStateStack.top();
	String* pText = new String(std::string((char*)ch, 0, len));

	switch (_tState)
	{
	case SAX_KEY:
		_sCurKey = pText->getCString();
		break;
	case SAX_INT:
	case SAX_REAL:
	case SAX_STRING:
	{
		if (curState == SAX_DICT)
		{
			CCAssert(!_sCurKey.empty(), "key not found : <integer/real>");
		}

		_sCurValue.append(pText->getCString());
	}
	break;
	default:
		break;
	}
	pText->release();
}




NS_CC_END