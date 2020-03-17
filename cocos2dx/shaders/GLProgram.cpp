#include "GLProgram.h"
#include "GLStateCache.h"
#include "platform/PlatformBase.h"
#include "cocoa/String.h"
#include "kazmath/GL/matrix.h"
#include "Director.h"


NS_CC_BEGIN


typedef struct _hashUniformEntry
{
	GLvoid* value;               // value
	unsigned int    location;    // Key
	UT_hash_handle  hh;          // hash entry
} tHashUniformEntry;



GLProgram::GLProgram()
	: _uProgram(0)
	, _uVertShader(0)
	, _uFragShader(0)
	, _pHashForUniforms(NULL)

	, _bUsesTime(false)
{
	memset(_uUniforms, 0, sizeof(_uUniforms));
}

GLProgram::~GLProgram()
{
	if (_uProgram)
	{
		ccGLDeleteProgram(_uProgram);
	}
}


bool GLProgram::initWithVertexShaderByteArray(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray)
{
	_uProgram = glCreateProgram();
	CHECK_GL_ERROR_DEBUG();

	_uVertShader = _uFragShader = 0;

	if (vShaderByteArray)
	{
		if (!compileShader(&_uVertShader, GL_VERTEX_SHADER, vShaderByteArray))
		{
			CCLOG("cocos2d: ERROR: Failed to compile vertex shader");
			return false;
		}
	}

	if (fShaderByteArray)
	{
		if (!compileShader(&_uFragShader, GL_FRAGMENT_SHADER, fShaderByteArray))
		{
			CCLOG("cocos2d: ERROR: Failed to compile fragment shader");
			return false;
		}
	}

	if (_uVertShader)
	{
		glAttachShader(_uProgram, _uVertShader);
	}
	CHECK_GL_ERROR_DEBUG();

	if (_uFragShader)
	{
		glAttachShader(_uProgram, _uFragShader);
	}
	_pHashForUniforms = NULL;

	CHECK_GL_ERROR_DEBUG();

	return true;
}

bool GLProgram::initWithVertexShaderFilename(const char* vShaderFilename, const char* fShaderFilename)
{
	//const GLchar* vertexSource = (GLchar*)String::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename(vShaderFilename).c_str())->getCString();
	//const GLchar* fragmentSource = (GLchar*)String::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename(fShaderFilename).c_str())->getCString();

	//return initWithVertexShaderByteArray(vertexSource, fragmentSource);
	return false;
}


bool GLProgram::link()
{
	CCAssert(_uProgram != 0, "Cannot link invalid program");

	GLint status = GL_TRUE;

	glLinkProgram(_uProgram);

	if (_uVertShader)
	{
		glDeleteShader(_uVertShader);
	}

	if (_uFragShader)
	{
		glDeleteShader(_uFragShader);
	}

	_uVertShader = _uFragShader = 0;
#if (defined(COCOS2D_DEBUG) && COCOS2D_DEBUG) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	glGetProgramiv(_uProgram, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		CCLOG("cocos2d: ERROR: Failed to link program: %i\n%s", _uProgram, programLog());
		ccGLDeleteProgram(_uProgram);
		_uProgram = 0;
	}
#endif

	return (status == GL_TRUE);
}

void GLProgram::use()
{
	ccGLUseProgram(_uProgram);
}


void GLProgram::reset()
{
	_uVertShader = _uFragShader = 0;
	memset(_uUniforms, 0, sizeof(_uUniforms));


	// it is already deallocated by android
	//ccGLDeleteProgram(m_uProgram);
	_uProgram = 0;


	tHashUniformEntry* current_element, * tmp;

	// Purge uniform hash
	HASH_ITER(hh, _pHashForUniforms, current_element, tmp)
	{
		HASH_DEL(_pHashForUniforms, current_element);
		free(current_element->value);
		free(current_element);
	}
	_pHashForUniforms = NULL;
}





bool GLProgram::compileShader(GLuint* shader, GLenum type, const GLchar* source)
{
	GLint status;

	if (!source)
	{
		return false;
	}

	const GLchar* sources[] = {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32 && CC_TARGET_PLATFORM != CC_PLATFORM_LINUX && CC_TARGET_PLATFORM != CC_PLATFORM_MAC)
#if CC_TARGET_PLATFORM == CC_PLATFORM_NACL
		"precision highp float;\n"
#else
		(type == GL_VERTEX_SHADER ? "precision highp float;\n" : "precision mediump float;\n"),
#endif
#endif
		"uniform mat4 CC_PMatrix;\n"
		"uniform mat4 CC_MVMatrix;\n"
		"uniform mat4 CC_MVPMatrix;\n"
		"uniform vec4 CC_Time;\n"
		"uniform vec4 CC_SinTime;\n"
		"uniform vec4 CC_CosTime;\n"
		"uniform vec4 CC_Random01;\n"
		"//CC INCLUDES END\n\n",
		source,
	};

	*shader = glCreateShader(type);
	glShaderSource(*shader, sizeof(sources) / sizeof(*sources), sources, NULL);
	glCompileShader(*shader);

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		GLsizei length;
		glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &length);
		GLchar* src = (GLchar*)malloc(sizeof(GLchar) * length);

		glGetShaderSource(*shader, length, NULL, src);
		CCLOG("cocos2d: ERROR: Failed to compile shader:\n%s", src);

		if (type == GL_VERTEX_SHADER)
		{
			CCLOG("cocos2d: %s", vertexShaderLog());
		}
		else
		{
			CCLOG("cocos2d: %s", fragmentShaderLog());
		}
		free(src);

		abort();
	}
	return (status == GL_TRUE);
}


const char* GLProgram::programLog()
{
	return this->logForOpenGLObject(_uProgram, (GLInfoFunction)&glGetProgramiv, (GLLogFunction)&glGetProgramInfoLog);
}

const char* GLProgram::vertexShaderLog()
{
	return this->logForOpenGLObject(_uVertShader, (GLInfoFunction)&glGetShaderiv, (GLLogFunction)&glGetShaderInfoLog);
}

const char* GLProgram::fragmentShaderLog()
{
	return this->logForOpenGLObject(_uFragShader, (GLInfoFunction)&glGetShaderiv, (GLLogFunction)&glGetShaderInfoLog);
}

const char* GLProgram::logForOpenGLObject(GLuint object, GLInfoFunction infoFunc, GLLogFunction logFunc)
{
	GLint logLength = 0, charsWritten = 0;

	infoFunc(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength < 1)
		return 0;

	char* logBytes = (char*)malloc(logLength);
	logFunc(object, logLength, &charsWritten, logBytes);

	String* log = String::create(logBytes);

	free(logBytes);
	return log->getCString();
}


void GLProgram::addAttribute(const char* attributeName, GLuint index)
{
	glBindAttribLocation(_uProgram, index, attributeName);
}


///////////////////////////////////
// uniform
//

bool GLProgram::updateUniformLocation(GLint location, GLvoid* data, unsigned int bytes)
{
	if (location < 0)
		return false;

	bool updated = true;
	tHashUniformEntry* element = NULL;
	HASH_FIND_INT(_pHashForUniforms, &location, element);

	if (!element)
	{
		element = (tHashUniformEntry*)malloc(sizeof(*element));

		// key
		element->location = location;

		// value
		element->value = malloc(bytes);
		memcpy(element->value, data, bytes);

		HASH_ADD_INT(_pHashForUniforms, location, element);
	}
	else
	{
		if (memcmp(element->value, data, bytes) == 0)
		{
			updated = false;
		}
		else
		{
			memcpy(element->value, data, bytes);
		}
	}

	return updated;
}

void GLProgram::updateUniforms()
{
	_uUniforms[kCCUniformPMatrix]   = glGetUniformLocation(_uProgram, kCCUniformPMatrix_s);
	_uUniforms[kCCUniformMVMatrix]  = glGetUniformLocation(_uProgram, kCCUniformMVMatrix_s);
	_uUniforms[kCCUniformMVPMatrix] = glGetUniformLocation(_uProgram, kCCUniformMVPMatrix_s);

	_uUniforms[kCCUniformTime]    = glGetUniformLocation(_uProgram, kCCUniformTime_s);
	_uUniforms[kCCUniformSinTime] = glGetUniformLocation(_uProgram, kCCUniformSinTime_s);
	_uUniforms[kCCUniformCosTime] = glGetUniformLocation(_uProgram, kCCUniformCosTime_s);

	_bUsesTime = (
		_uUniforms[kCCUniformTime] != -1 ||
		_uUniforms[kCCUniformSinTime] != -1 ||
		_uUniforms[kCCUniformCosTime] != -1
		);

	_uUniforms[kCCUniformRandom01] = glGetUniformLocation(_uProgram, kCCUniformRandom01_s);
	_uUniforms[kCCUniformSampler]  = glGetUniformLocation(_uProgram, kCCUniformSampler_s);

	this->use();

	// Since sample most probably won't change, set it to 0 now.
	this->setUniformLocationWith1i(_uUniforms[kCCUniformSampler], 0);
}

GLint GLProgram::getUniformLocationForName(const char* name)
{
	CCAssert(name != NULL, "Invalid uniform name");
	CCAssert(_uProgram != 0, "Invalid operation. Cannot get uniform location when program is not initialized");

	return glGetUniformLocation(_uProgram, name);
}


void GLProgram::setUniformLocationWith1i(GLint location, GLint i1)
{
	bool updated = updateUniformLocation(location, &i1, sizeof(i1) * 1);

	if (updated)
	{
		glUniform1i((GLint)location, i1);
	}
}

void GLProgram::setUniformLocationWith2i(GLint location, GLint i1, GLint i2)
{
	GLint ints[2] = { i1,i2 };
	bool updated = updateUniformLocation(location, ints, sizeof(ints));

	if (updated)
	{
		glUniform2i((GLint)location, i1, i2);
	}
}

void GLProgram::setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3)
{
	GLint ints[3] = { i1,i2,i3 };
	bool updated = updateUniformLocation(location, ints, sizeof(ints));

	if (updated)
	{
		glUniform3i((GLint)location, i1, i2, i3);
	}
}

void GLProgram::setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4)
{
	GLint ints[4] = { i1,i2,i3,i4 };
	bool updated = updateUniformLocation(location, ints, sizeof(ints));

	if (updated)
	{
		glUniform4i((GLint)location, i1, i2, i3, i4);
	}
}



void GLProgram::setUniformLocationWith2iv(GLint location, GLint* ints, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, ints, sizeof(int) * 2 * numberOfArrays);

	if (updated)
	{
		glUniform2iv((GLint)location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith3iv(GLint location, GLint* ints, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, ints, sizeof(int) * 3 * numberOfArrays);

	if (updated)
	{
		glUniform3iv((GLint)location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith4iv(GLint location, GLint* ints, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, ints, sizeof(int) * 4 * numberOfArrays);

	if (updated)
	{
		glUniform4iv((GLint)location, (GLsizei)numberOfArrays, ints);
	}
}


void GLProgram::setUniformLocationWith1f(GLint location, GLfloat f1)
{
	bool updated = updateUniformLocation(location, &f1, sizeof(f1) * 1);

	if (updated)
	{
		glUniform1f((GLint)location, f1);
	}
}

void GLProgram::setUniformLocationWith2f(GLint location, GLfloat f1, GLfloat f2)
{
	GLfloat floats[2] = { f1,f2 };
	bool updated = updateUniformLocation(location, floats, sizeof(floats));

	if (updated)
	{
		glUniform2f((GLint)location, f1, f2);
	}
}

void GLProgram::setUniformLocationWith3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3)
{
	GLfloat floats[3] = { f1,f2,f3 };
	bool updated = updateUniformLocation(location, floats, sizeof(floats));

	if (updated)
	{
		glUniform3f((GLint)location, f1, f2, f3);
	}
}

void GLProgram::setUniformLocationWith4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
	GLfloat floats[4] = { f1,f2,f3,f4 };
	bool updated = updateUniformLocation(location, floats, sizeof(floats));

	if (updated)
	{
		glUniform4f((GLint)location, f1, f2, f3, f4);
	}
}


void GLProgram::setUniformLocationWith2fv(GLint location, GLfloat* floats, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, floats, sizeof(float) * 2 * numberOfArrays);

	if (updated)
	{
		glUniform2fv((GLint)location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith3fv(GLint location, GLfloat* floats, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, floats, sizeof(float) * 3 * numberOfArrays);

	if (updated)
	{
		glUniform3fv((GLint)location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith4fv(GLint location, GLfloat* floats, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, floats, sizeof(float) * 4 * numberOfArrays);

	if (updated)
	{
		glUniform4fv((GLint)location, (GLsizei)numberOfArrays, floats);
	}
}


void GLProgram::setUniformLocationWithMatrix4fv(GLint location, GLfloat* matrixArray, unsigned int numberOfMatrices)
{
	bool updated = updateUniformLocation(location, matrixArray, sizeof(float) * 16 * numberOfMatrices);

	if (updated)
	{
		glUniformMatrix4fv((GLint)location, (GLsizei)numberOfMatrices, GL_FALSE, matrixArray);
	}
}


void GLProgram::setUniformsForBuiltins()
{
	kmMat4 matrixP;
	kmMat4 matrixMV;
	kmMat4 matrixMVP;

	kmGLGetMatrix(KM_GL_PROJECTION, &matrixP);
	kmGLGetMatrix(KM_GL_MODELVIEW, &matrixMV);

	// matrixMV * matrixP
	kmMat4Multiply(&matrixMVP, &matrixP, &matrixMV);

	setUniformLocationWithMatrix4fv(_uUniforms[kCCUniformPMatrix], matrixP.mat, 1);
	setUniformLocationWithMatrix4fv(_uUniforms[kCCUniformMVMatrix], matrixMV.mat, 1);
	setUniformLocationWithMatrix4fv(_uUniforms[kCCUniformMVPMatrix], matrixMVP.mat, 1);

	if (_bUsesTime)
	{
		Director* director = Director::sharedDirector();
		// This doesn't give the most accurate global time value.
		// Cocos2D doesn't store a high precision time value, so this will have to do.
		// Getting Mach time per frame per shader using time could be extremely expensive.
		float time = director->getTotalFrames() * director->getAnimationInterval();

		setUniformLocationWith4f(_uUniforms[kCCUniformTime], time / 10.0, time, time * 2, time * 4);
		setUniformLocationWith4f(_uUniforms[kCCUniformSinTime], time / 8.0, time / 4.0, time / 2.0, sinf(time));
		setUniformLocationWith4f(_uUniforms[kCCUniformCosTime], time / 8.0, time / 4.0, time / 2.0, cosf(time));
	}

	if (_uUniforms[kCCUniformRandom01] != -1)
	{
		setUniformLocationWith4f(_uUniforms[kCCUniformRandom01], CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1());
	}

	// 还有一个kCCUniformSampler  在需要的控件中处理 比如sprite
}





NS_CC_END