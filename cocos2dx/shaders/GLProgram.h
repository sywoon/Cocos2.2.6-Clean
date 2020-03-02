#pragma once
#include "ccMacros.h"
#include "cocoa/Object.h"
#include "GL/glew.h"
#include "cocoa/data_support/uthash.h"


NS_CC_BEGIN



enum {
	kCCVertexAttrib_Position,
	kCCVertexAttrib_Color,
	kCCVertexAttrib_TexCoords,

	kCCVertexAttrib_MAX,
};

// Attribute names
#define    kCCAttributeNamePosition        "a_position"
#define    kCCAttributeNameColor           "a_color"
#define    kCCAttributeNameTexCoord        "a_texCoord"


enum {
	kCCUniformPMatrix,
	kCCUniformMVMatrix,
	kCCUniformMVPMatrix,

	kCCUniformTime,
	kCCUniformSinTime,
	kCCUniformCosTime,

	kCCUniformRandom01,
	kCCUniformSampler,

	kCCUniform_MAX,
};

// uniform names  shaderÖÐµÄÃû³Æ
#define kCCUniformPMatrix_s				"CC_PMatrix"
#define kCCUniformMVMatrix_s			"CC_MVMatrix"
#define kCCUniformMVPMatrix_s			"CC_MVPMatrix"
#define kCCUniformTime_s				"CC_Time"
#define kCCUniformSinTime_s				"CC_SinTime"
#define kCCUniformCosTime_s				"CC_CosTime"
#define kCCUniformRandom01_s			"CC_Random01"
#define kCCUniformSampler_s				"CC_Texture0"
#define kCCUniformAlphaTestValue		"CC_alpha_value"


typedef void (*GLInfoFunction)(GLuint program, GLenum pname, GLint* params);
typedef void (*GLLogFunction) (GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);



class CC_DLL GLProgram : public Object
{
public:
    GLProgram();
    virtual ~GLProgram();

public:
    inline const GLuint     getProgram() { return _uProgram; }

    bool initWithVertexShaderByteArray(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray);
    bool initWithVertexShaderFilename(const char* vShaderFilename, const char* fShaderFilename);

    bool link();
    void use();

    void reset();

    void addAttribute(const char* attributeName, GLuint index);

    /** It will create 4 uniforms:
    - kCCUniformPMatrix
    - kCCUniformMVMatrix
    - kCCUniformMVPMatrix
    - kCCUniformSampler
     And it will bind "kCCUniformSampler" to 0
     */
    void updateUniforms();

    GLint getUniformLocationForName(const char* name);
    void setUniformLocationWith1i(GLint location, GLint i1);
    void setUniformLocationWith2i(GLint location, GLint i1, GLint i2);
    void setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3);
    void setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4);

    void setUniformLocationWith2iv(GLint location, GLint* ints, unsigned int numberOfArrays);
    void setUniformLocationWith3iv(GLint location, GLint* ints, unsigned int numberOfArrays);
    void setUniformLocationWith4iv(GLint location, GLint* ints, unsigned int numberOfArrays);

    void setUniformLocationWith1f(GLint location, GLfloat f1);
    void setUniformLocationWith2f(GLint location, GLfloat f1, GLfloat f2);
    void setUniformLocationWith3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3);
    void setUniformLocationWith4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);

    void setUniformLocationWith2fv(GLint location, GLfloat* floats, unsigned int numberOfArrays);
    void setUniformLocationWith3fv(GLint location, GLfloat* floats, unsigned int numberOfArrays);
    void setUniformLocationWith4fv(GLint location, GLfloat* floats, unsigned int numberOfArrays);
    void setUniformLocationWithMatrix4fv(GLint location, GLfloat* matrixArray, unsigned int numberOfMatrices);

    void setUniformsForBuiltins();


private:
    bool        compileShader(GLuint* shader, GLenum type, const GLchar* source);
    bool        updateUniformLocation(GLint location, GLvoid* data, unsigned int bytes);

    const char* programLog();
    const char* vertexShaderLog();
    const char* fragmentShaderLog();
    const char* logForOpenGLObject(GLuint object, GLInfoFunction infoFunc, GLLogFunction logFunc);


private:
    GLuint            _uProgram;
	GLuint            _uVertShader;
	GLuint            _uFragShader;
    GLint             _uUniforms[kCCUniform_MAX];

    bool              _bUsesTime;

    struct _hashUniformEntry* _pHashForUniforms;
};








NS_CC_END