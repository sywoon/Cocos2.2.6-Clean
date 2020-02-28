#pragma once

#include "ccMacros.h"
#include "GL/glew.h"


NS_CC_BEGIN

struct AffineTransform;

void CGAffineToGL(const AffineTransform* t, GLfloat* m);
void GLToCGAffine(const GLfloat* m, AffineTransform* t);


NS_CC_END