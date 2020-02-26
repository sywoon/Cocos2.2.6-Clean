#pragma once

#include "ccMacros.h"
#include "DataVisitor.h"


NS_CC_BEGIN

class CC_DLL Object
{
public:
	Object(void);
	virtual ~Object(void);

public:
	void			retain(void);
	void			release(void);
	Object*			autorelease(void);

	bool			isSingleReference(void) const;
	unsigned int	retainCount(void) const;

	Object*			clone(void);

public:
	virtual bool	isEqual(const Object* pObject);
	virtual void	acceptVisitor(DataVisitor& visitor);
	
protected:
	virtual Object* cloneSelf();


public:
	unsigned int        _uID;
	unsigned int        _uReference;
	unsigned int        _uAutoReleaseCount;
};


NS_CC_END