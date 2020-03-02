#pragma once

#include <string>
#include "Object.h"
#include "Array.h"
#include "ccMacros.h"
#include "data_support/uthash.h"
#include "platform/PlatformBase.h"




NS_CC_BEGIN

class Dictionary;


class CC_DLL DictElement
{
	#define   MAX_KEY_LEN   256

public:
	~DictElement();

	inline const char* getStrKey() const
	{
		CCAssert(_szKey[0] != '\0', "Should not call this function for integer dictionary");
		return _szKey;
	}

	inline intptr_t getIntKey() const
	{
		CCAssert(_szKey[0] == '\0', "Should not call this function for string dictionary");
		return _iKey;
	}

	inline Object* getObject() const { return _pObject; }


private:
	DictElement(const char* pszKey, Object* pObject);
	DictElement(intptr_t iKey, Object* pObject);


private:
	// char array is needed for HASH_ADD_STR in UT_HASH.
	// So it's a pain that all elements will allocate 256 bytes for this array.
	char		_szKey[MAX_KEY_LEN];
	intptr_t	_iKey;
	Object*		_pObject;


public:
	UT_hash_handle hh;      // makes this class hashable
	friend class Dictionary; // declare Dictionary as friend class
};



/** The macro for traversing dictionary
 *
 *  @note It's faster than getting all keys and traversing keys to get objects by objectForKey.
 *        It's also safe to remove elements while traversing.
 */
#define DICT_FOREACH(__dict__, __el__) \
    DictElement* pTmp##__dict__##__el__ = NULL; \
    if (__dict__) \
    HASH_ITER(hh, (__dict__)->_pElements, __el__, pTmp##__dict__##__el__)



class String;
class CC_DLL Dictionary : public Object
{
public:
	ACCEPT_VISITOR;

	//��һ�ε���setObject  ��ȷ�����ֵ�key����ʽ
	enum CCDictType
	{
		kCCDictUnknown = 0,
		kCCDictStr,
		kCCDictInt
	};

public:
	static Dictionary* create();
	static Dictionary* createWithDictionary(Dictionary* srcDict);

public:
	Dictionary();
	~Dictionary();

	// ������ ��ɾ�����ͷžɵ� ���µ��滻
	void		setObject(Object* pObject, const std::string& key);
	void		setObject(Object* pObject, intptr_t key);

	void		removeObjectForKey(const std::string& key);
	void		removeObjectForKey(intptr_t key);
	void		removeObjectsForKeys(Array* pKeyArray);

	// ����ɾ��Ԫ�صĵط� 
	// ��release DictElement �е�Object Ȼ��delete���DictElement
	void		removeObjectForElememt(DictElement* pElement);

	void		removeAllObjects();

	//    CCString* pStr = (CCString*)pDict->objectForKey("key1");
	//    CCString* pStr2 = dynamic_cast<CCString*>(pDict->objectForKey("key1"));
	//    if (pStr2 != NULL) {
	Object*		objectForKey(const std::string& key);
	Object*		objectForKey(intptr_t key);
	Object*		randomObject();

	// Ԫ�ر�����String*
	const String* valueForKey(const std::string& key);
	const String* valueForKey(intptr_t key);


	unsigned int count();

	//return  The array contains all keys of elements. It's an autorelease object yet.
	Array*		allKeys();

	// return   The array contains all keys for the specified object. It's an autorelease object yet.
	Array*		allKeysForObject(Object* object);


protected:
	virtual Object* cloneSelf();


private:
	// retain Object ������_pElements��
	void		setObjectUnSafe(Object* pObject, const std::string& key);
	void		setObjectUnSafe(Object* pObject, const intptr_t key);


public:
	DictElement* _pElements;

private:
	CCDictType _eDictType;
};



NS_CC_END