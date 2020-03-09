#pragma once


#include "ccMacros.h"
#include "cocoa/Object.h"


NS_CC_BEGIN



class CC_DLL TextureETC : public Object
{
public:
	TextureETC();
	virtual ~TextureETC();


public:
	bool initWithFile(const char* file);

	unsigned int getName() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:
	bool loadTexture(const char* file);


private:
	GLuint _name;
	unsigned int _width;
	unsigned int _height;
};



NS_CC_END
