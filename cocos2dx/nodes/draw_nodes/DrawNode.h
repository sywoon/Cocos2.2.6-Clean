#pragma once

#include "ccMacros.h"
#include "nodes/base_nodes/Node.h"
#include "cocoa/Geometry.h"


NS_CC_BEGIN


class CC_DLL DrawNode : public Node
{
public:
	static DrawNode* create();
	virtual ~DrawNode();


protected:
	DrawNode();
	virtual bool	init();

public:
	virtual void	draw();

	void			setBlendFunc(const ccBlendFunc& blendFunc);
	ccBlendFunc		getBlendFunc() const;

	void			drawDot(const Point& pos, float radius, const ccColor4F& color);
	void			drawSegment(const Point& from, const Point& to, float radius, const ccColor4F& color);
	void			drawPolygon(Point* verts, unsigned int count, const ccColor4F& fillColor, float borderWidth, const ccColor4F& borderColor);

private:
	void			ensureCapacity(unsigned int count);
	void			render();

protected:
	GLuint			_uVao;
	GLuint			_uVbo;

	unsigned int    _uBufferCapacity;
	GLsizei         _nBufferCount;
	ccV2F_C4B_T2F*  _pBuffer;

	ccBlendFunc     _sBlendFunc;

	bool            _bDirty;
};





NS_CC_END
