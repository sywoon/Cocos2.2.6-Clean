#pragma once
#include "Object.h"
#include "data_support/ccCArray.h"


#define ARRAY_FOREACH(__array__, __object__)                                                                         \
    if ((__array__) && (__array__)->data->num > 0)                                                                     \
    for(Object** __arr__ = (__array__)->data->arr, **__end__ = (__array__)->data->arr + (__array__)->data->num-1;    \
    __arr__ <= __end__ && (((__object__) = *__arr__) != NULL/* || true*/);                                             \
    __arr__++)

#define ARRAY_FOREACH_REVERSE(__array__, __object__)                                                                  \
    if ((__array__) && (__array__)->data->num > 0)                                                                      \
    for(Object** __arr__ = (__array__)->data->arr + (__array__)->data->num-1, **__end__ = (__array__)->data->arr;     \
    __arr__ >= __end__ && (((__object__) = *__arr__) != NULL/* || true*/);                                              \
    __arr__--)

//检查存储的对象是否都是指定类型
#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
#define ARRAY_VERIFY_TYPE(__array__, __type__)                                                                 \
    do {                                                                                                         \
        if ((__array__) && (__array__)->data->num > 0)                                                           \
            for(Object** __arr__ = (__array__)->data->arr,                                                     \
                **__end__ = (__array__)->data->arr + (__array__)->data->num-1; __arr__ <= __end__; __arr__++)    \
                CCAssert(dynamic_cast<__type__>(*__arr__), "element type is wrong!");                            \
    } while(false)
#else
#define ARRAY_VERIFY_TYPE(__array__, __type__) void(0)
#endif


//遍历对象 强制转换为指定类型 并调用指定函数名  注意没参数
#define arrayMakeObjectsPerformSelector(pArray, func, elementType)    \
do {                                                                  \
    if(pArray && pArray->count() > 0)                                 \
    {                                                                 \
        Object* child;                                              \
        CCARRAY_FOREACH(pArray, child)                                \
        {                                                             \
            elementType pNode = (elementType) child;                  \
            if(pNode)                                                 \
            {                                                         \
                pNode->func();                                        \
            }                                                         \
        }                                                             \
    }                                                                 \
}                                                                     \
while(false)

//同上 调用函数里 多了一个给定Object*参数
#define arrayMakeObjectsPerformSelectorWithObject(pArray, func, pObject, elementType)   \
do {                                                                  \
    if(pArray && pArray->count() > 0)                                 \
    {                                                                 \
        Object* child = NULL;                                       \
        CCARRAY_FOREACH(pArray, child)                                \
        {                                                             \
            elementType pNode = (elementType) child;                  \
            if(pNode)                                                 \
            {                                                         \
                pNode->func(pObject);                                 \
            }                                                         \
        }                                                             \
    }                                                                 \
}                                                                     \
while(false)




NS_CC_BEGIN


class CC_DLL Array : public Object
{
public:
	~Array();

	static Array* create();
	static Array* createWithCapacity(unsigned int capacity);
	static Array* create(Object* pObject, ...);
	static Array* createWithObject(Object* pObject);
	static Array* createWithArray(Array* otherArray);


public:  //允许非create方式使用 自己控制释放
	Array();
	Array(unsigned int capacity);
	bool init();
	bool initWithObject(Object* pObject);
	bool initWithObjects(Object* pObject, ...);
	bool initWithCapacity(unsigned int capacity);
	bool initWithArray(Array* otherArray);


public:
	unsigned int count() const;
	unsigned int capacity() const;
	unsigned int indexOfObject(Object* object) const;
	Object*		 objectAtIndex(unsigned int index);
	Object*		 lastObject();
	Object*		 randomObject();
	bool		 containsObject(Object* object) const;
	bool		 isEqualToArray(Array* pOtherArray);

	void addObject(Object* object);
	void addObjectsFromArray(Array* otherArray);
	void insertObject(Object* object, unsigned int index);

	void removeLastObject(bool bReleaseObj = true);
	void removeObject(Object* object, bool bReleaseObj = true);
	void removeObjectAtIndex(unsigned int index, bool bReleaseObj = true);
	void removeObjectsInArray(Array* otherArray);
	void removeAllObjects();
	void fastRemoveObject(Object* object);
	void fastRemoveObjectAtIndex(unsigned int index);


	/** Swap two elements */
	void exchangeObject(Object* object1, Object* object2);
	/** Swap two elements with certain indexes */
	void exchangeObjectAtIndex(unsigned int index1, unsigned int index2);
	void replaceObjectAtIndex(unsigned int uIndex, Object* pObject, bool bReleaseObject = true);
	void reverseObjects();
	/* Shrinks the array so the memory footprint corresponds with the number of items */
	void reduceMemoryFootprint();


protected:
	virtual Object* cloneSelf();

public:
	ccArray* data;
};

NS_CC_END