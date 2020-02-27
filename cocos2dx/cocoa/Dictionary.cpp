#include "Dictionary.h"
#include "Integer.h"
#include "String.h"
#include "platform/PlatformBase.h"


using namespace std;


NS_CC_BEGIN

// -----------------------------------------------------------------------
// DictElement

DictElement::DictElement(const char* pszKey, Object* pObject)
{
	CCAssert(pszKey && strlen(pszKey) > 0, "Invalid key value.");
	_iKey = 0;
	const char* pStart = pszKey;

	int len = strlen(pszKey);
	if (len > MAX_KEY_LEN)
	{
		char* pEnd = (char*)& pszKey[len - 1];
		pStart = pEnd - (MAX_KEY_LEN - 1);
	}

	strcpy(_szKey, pStart);

	_pObject = pObject;
	memset(&hh, 0, sizeof(hh));
}

DictElement::DictElement(intptr_t iKey, Object * pObject)
{
	_szKey[0] = '\0';
	_iKey = iKey;
	_pObject = pObject;
	memset(&hh, 0, sizeof(hh));
}

DictElement::~DictElement()
{

}





// -----------------------------------------------------------------------
// Dictionary

Dictionary::Dictionary()
	: _pElements(NULL)
	, _eDictType(kCCDictUnknown)
{

}

Dictionary::~Dictionary()
{
	removeAllObjects();
}


void Dictionary::setObject(Object* pObject, const std::string& key)
{
	CCAssert(key.length() > 0 && pObject != NULL, "Invalid Argument!");
	if (_eDictType == kCCDictUnknown)
	{
		_eDictType = kCCDictStr;
	}

	CCAssert(_eDictType == kCCDictStr, "this dictionary doesn't use string as key.");

	DictElement* pElement = NULL;
	HASH_FIND_STR(_pElements, key.c_str(), pElement);
	if (pElement == NULL)
	{
		setObjectUnSafe(pObject, key);
	}
	else if (pElement->_pObject != pObject)
	{
		Object* pTmpObj = pElement->_pObject;
		pTmpObj->retain();
		removeObjectForElememt(pElement);
		setObjectUnSafe(pObject, key);
		pTmpObj->release();
	}
}

void Dictionary::setObject(Object* pObject, intptr_t key)
{
	CCAssert(pObject != NULL, "Invalid Argument!");
	if (_eDictType == kCCDictUnknown)
	{
		_eDictType = kCCDictInt;
	}

	CCAssert(_eDictType == kCCDictInt, "this dictionary doesn't use integer as key.");

	DictElement* pElement = NULL;
	HASH_FIND_PTR(_pElements, &key, pElement);
	if (pElement == NULL)
	{
		setObjectUnSafe(pObject, key);
	}
	else if (pElement->_pObject != pObject)
	{
		Object* pTmpObj = pElement->_pObject;
		pTmpObj->retain();
		removeObjectForElememt(pElement);
		setObjectUnSafe(pObject, key);
		pTmpObj->release();
	}
}


void Dictionary::removeObjectForKey(const std::string& key)
{
	if (_eDictType == kCCDictUnknown)
		return;

	CCAssert(_eDictType == kCCDictStr, "this dictionary doesn't use string as its key");
	CCAssert(key.length() > 0, "Invalid Argument!");

	DictElement* pElement = NULL;
	HASH_FIND_STR(_pElements, key.c_str(), pElement);
	removeObjectForElememt(pElement);
}

void Dictionary::removeObjectForKey(intptr_t key)
{
	if (_eDictType == kCCDictUnknown)
	{
		return;
	}

	CCAssert(_eDictType == kCCDictInt, "this dictionary doesn't use integer as its key");
	DictElement* pElement = NULL;
	HASH_FIND_PTR(_pElements, &key, pElement);
	removeObjectForElememt(pElement);
}

void Dictionary::removeObjectsForKeys(Array* pKeyArray)
{
	Object* pObj = NULL;
	ARRAY_FOREACH(pKeyArray, pObj)
	{
		String* pStr = (String*)pObj;
		removeObjectForKey(pStr->getCString());
	}
}

void Dictionary::removeObjectForElememt(DictElement* pElement)
{
	if (pElement != NULL)
	{
		HASH_DEL(_pElements, pElement);
		pElement->_pObject->release();
		CC_SAFE_DELETE(pElement);
	}
}

void Dictionary::removeAllObjects()
{
	// 方式1
	//DictElement* pElement, * tmp;
	//HASH_ITER(hh, _pElements, pElement, tmp)
	//{
	//	HASH_DEL(_pElements, pElement);
	//	pElement->_pObject->release();
	//	CC_SAFE_DELETE(pElement);
	//}

	//方式2 简化版 功能相同
	DictElement* pElement;
	HASH_ITER_EX(_pElements, pElement)
	{
		HASH_DEL(_pElements, pElement);
		pElement->_pObject->release();
		CC_SAFE_DELETE(pElement);
	}
}


Object* Dictionary::objectForKey(const std::string& key)
{
	if (_eDictType == kCCDictUnknown) return NULL;
	CCAssert(_eDictType == kCCDictStr, "this dictionary does not use string as key.");

	Object * pRetObject = NULL;
	DictElement * pElement = NULL;
	HASH_FIND_STR(_pElements, key.c_str(), pElement);
	if (pElement != NULL)
	{
		pRetObject = pElement->_pObject;
	}
	return pRetObject;
}

Object* Dictionary::objectForKey(intptr_t key)
{
	// if dictionary wasn't initialized, return NULL directly.
	if (_eDictType == kCCDictUnknown) return NULL;
	// Dictionary only supports one kind of key, string or integer.
	// This method uses integer as key, therefore we should make sure that the key type of this Dictionary is integer.
	CCAssert(_eDictType == kCCDictInt, "this dictionary does not use integer as key.");

	Object * pRetObject = NULL;
	DictElement * pElement = NULL;
	HASH_FIND_PTR(_pElements, &key, pElement);
	if (pElement != NULL)
	{
		pRetObject = pElement->_pObject;
	}
	return pRetObject;
}

const String* Dictionary::valueForKey(const std::string& key)
{
	String* pStr = dynamic_cast<String*>(objectForKey(key));
	if (pStr == NULL)
	{
		pStr = String::create("");
	}
	return pStr;
}

const String* Dictionary::valueForKey(intptr_t key)
{
	String* pStr = dynamic_cast<String*>(objectForKey(key));
	if (pStr == NULL)
	{
		pStr = String::create("");
	}
	return pStr;
}



unsigned int Dictionary::count()
{
	return HASH_COUNT(_pElements);
}

Array* Dictionary::allKeys()
{
	int iKeyCount = this->count();
	if (iKeyCount <= 0) return NULL;

	Array * pArray = Array::createWithCapacity(iKeyCount);

	DictElement * pElement, *tmp;
	if (_eDictType == kCCDictStr)
	{
		HASH_ITER(hh, _pElements, pElement, tmp)
		{
			String* pOneKey = new String(pElement->_szKey);
			pArray->addObject(pOneKey);
			CC_SAFE_RELEASE(pOneKey);
		}
	}
	else if (_eDictType == kCCDictInt)
	{
		HASH_ITER(hh, _pElements, pElement, tmp)
		{
			Integer* pOneKey = new Integer((int)(pElement->_iKey));
			pArray->addObject(pOneKey);
			CC_SAFE_RELEASE(pOneKey);
		}
	}

	return pArray;
}

Array* Dictionary::allKeysForObject(Object* object)
{
	int iKeyCount = this->count();
	if (iKeyCount <= 0) return NULL;
	Array * pArray = Array::create();

	DictElement * pElement, *tmp;

	if (_eDictType == kCCDictStr)
	{
		HASH_ITER(hh, _pElements, pElement, tmp)
		{
			if (object == pElement->_pObject)
			{
				String* pOneKey = new String(pElement->_szKey);
				pArray->addObject(pOneKey);
				CC_SAFE_RELEASE(pOneKey);
			}
		}
	}
	else if (_eDictType == kCCDictInt)
	{
		HASH_ITER(hh, _pElements, pElement, tmp)
		{
			if (object == pElement->_pObject)
			{
				Integer* pOneKey = new Integer(pElement->_iKey);
				pArray->addObject(pOneKey);
				CC_SAFE_RELEASE(pOneKey);
			}
		}
	}
	return pArray;
}


Object* Dictionary::randomObject()
{
	if (_eDictType == kCCDictUnknown)
	{
		return NULL;
	}

	Object* key = allKeys()->randomObject();

	if (_eDictType == kCCDictInt)
	{
		return objectForKey(((Integer*)key)->getValue());
	}
	else if (_eDictType == kCCDictStr)
	{
		return objectForKey(((String*)key)->getCString());
	}
	else
	{
		return NULL;
	}
}

Dictionary* Dictionary::create()
{
	Dictionary* pRet = new Dictionary();
	if (pRet != NULL)
	{
		pRet->autorelease();
	}
	return pRet;
}

Dictionary* Dictionary::createWithDictionary(Dictionary * srcDict)
{
	Dictionary* pNewDict = (Dictionary*)srcDict->clone();
	pNewDict->autorelease();
	return pNewDict;
}


void Dictionary::setObjectUnSafe(Object* pObject, const std::string& key)
{
	pObject->retain();
	DictElement* pElement = new DictElement(key.c_str(), pObject);
	HASH_ADD_STR(_pElements, _szKey, pElement);
}

void Dictionary::setObjectUnSafe(Object* pObject, const intptr_t key)
{
	pObject->retain();
	DictElement* pElement = new DictElement(key, pObject);
	HASH_ADD_PTR(_pElements, _iKey, pElement);
}


Object* Dictionary::cloneSelf()
{
	Dictionary* pNewDict = new Dictionary();

	DictElement* pElement = NULL;
	Object* pTmpObj = NULL;

	if (_eDictType == kCCDictInt)
	{
		DICT_FOREACH(this, pElement)
		{
			pTmpObj = pElement->getObject()->clone();
			pNewDict->setObject(pTmpObj, pElement->getIntKey());
			pTmpObj->release();
		}
	}
	else if (_eDictType == kCCDictStr)
	{
		DICT_FOREACH(this, pElement)
		{
			pTmpObj = pElement->getObject()->clone();
			pNewDict->setObject(pTmpObj, pElement->getStrKey());
			pTmpObj->release();
		}
	}

	return pNewDict;
}



NS_CC_END
