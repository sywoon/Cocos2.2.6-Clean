#include "Node.h"
#include "cocoa/String.h"
#include "cocoa/Array.h"
#include "cocoa/extend/PointExtend.h"
#include "cocoa/extend/AffineExtend.h"
#include "kazmath/GL/matrix.h"
#include "platform/PlatformBase.h"
#include "shaders/GLProgram.h"
#include "touch/Touch.h"



NS_CC_BEGIN

static int s_globalOrderOfArrival = 1;


Node* Node::create(void)
{
	Node* pRet = new Node();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}



Node::Node(void)
	: _bRunning(false)
	, _bVisible(true)
	, _nTag(kCCNodeTagInvalid)
	, _nZOrder(0)
	, _uOrderOfArrival(0)
	, _fVertexZ(0.0f)

	, _pChildren(NULL)
	, _pParent(NULL)
	, _bReorderChildDirty(false)
	
	, _obPosition(PointZero)
	, _obContentSize(SizeZero)

	, _fScaleX(1.0f)
	, _fScaleY(1.0f)
	, _fRotationX(0.0f)
	, _fRotationY(0.0f)
	, _fSkewX(0.0f)
	, _fSkewY(0.0f)

	, _obAnchorPointInPoints(PointZero)
	, _obAnchorPoint(PointZero)
	, _bIgnoreAnchorPointForPosition(false)

	, _bTransformDirty(false)
	, _bInverseDirty(false)
	, _bAdditionalTransformDirty(false)

	, _pShaderProgram(NULL)
{

}

Node::~Node(void)
{
	CC_SAFE_RELEASE(_pShaderProgram);

	if (_pChildren && _pChildren->count() > 0)
	{
		Object* child;
		ARRAY_FOREACH(_pChildren, child)
		{
			Node* pChild = (Node*)child;
			if (pChild)
			{
				pChild->_pParent = NULL;
			}
		}
	}
	CC_SAFE_RELEASE(_pChildren);

	_pParent = NULL;
}

bool Node::init()
{
	return true;
}

const char* Node::description()
{
	return String::createWithFormat("<CCNode | Tag = %d>", _nTag)->getCString();
}

bool Node::isRunning()
{
	return _bRunning;
}


void Node::onEnter()
{
	//fix setTouchEnabled not take effect when called the function in onEnter in JSBinding.
	_bRunning = true;

	//Judge the running state for prevent called onEnter method more than once,it's possible that this function called by addChild  
	if (_pChildren && _pChildren->count() > 0)
	{
		Object* child;
		Node* node;
		ARRAY_FOREACH(_pChildren, child)
		{
			node = (Node*)child;
			if (!node->isRunning())
			{
				node->onEnter();
			}
		}
	}
}

void Node::onEnterTransitionDidFinish()
{
	arrayMakeObjectsPerformSelector(_pChildren, onEnterTransitionDidFinish, Node*);
}

void Node::onExitTransitionDidStart()
{
	arrayMakeObjectsPerformSelector(_pChildren, onExitTransitionDidStart, Node*);
}

void Node::onExit()
{
	_bRunning = false;

	arrayMakeObjectsPerformSelector(_pChildren, onExit, Node*);
}

void Node::cleanup()
{
	arrayMakeObjectsPerformSelector(_pChildren, cleanup, Node*);
}



bool Node::isVisible()
{
	return _bVisible;
}

void Node::setVisible(bool var)
{
	_bVisible = var;
}

void Node::setZOrder(int z)
{
	_nZOrder = z;
	if (_pParent)
	{
		_pParent->reorderChild(this, z);
	}
}

int Node::getZOrder()
{
	return _nZOrder;
}

int Node::getTag() const
{
	return _nTag;
}

void Node::setTag(int var)
{
	_nTag = var;
}



//////////////////////////////
// parent and children
//

Node* Node::getParent()
{
	return _pParent;
}

void Node::setParent(Node* var)
{
	_pParent = var;
}

Array* Node::getChildren()
{
	return _pChildren;
}

unsigned int Node::getChildrenCount(void) const
{
	return _pChildren ? _pChildren->count() : 0;
}

void Node::addChild(Node* child)
{
	CCAssert(child != NULL, "Argument must be non-nil");
	this->addChild(child, child->_nZOrder, child->_nTag);
}

void Node::addChild(Node* child, int zOrder)
{
	CCAssert(child != NULL, "Argument must be non-nil");
	this->addChild(child, zOrder, child->_nTag);
}


void Node::addChild(Node* child, int zOrder, int tag)
{
	CCAssert(child != NULL, "Argument must be non-nil");
	CCAssert(child->_pParent == NULL, "child already added. It can't be added again");

	if (!_pChildren)
	{
		this->childrenAlloc();
	}

	this->insertChild(child, zOrder);

	child->_nTag = tag;

	child->setParent(this);
	child->setOrderOfArrival(s_globalOrderOfArrival++);

	if (_bRunning)
	{
		child->onEnter();
		child->onEnterTransitionDidFinish();
	}
}

Node* Node::getChildByTag(int aTag)
{
	CCAssert(aTag != kCCNodeTagInvalid, "Invalid tag");

	if (_pChildren && _pChildren->count() > 0)
	{
		Object* child;
		ARRAY_FOREACH(_pChildren, child)
		{
			Node* pNode = (Node*)child;
			if (pNode && pNode->_nTag == aTag)
				return pNode;
		}
	}
	return NULL;
}


void Node::removeChild(Node* child)
{
	this->removeChild(child, true);
}

void Node::removeChild(Node* child, bool cleanup)
{
	if (_pChildren == NULL)
	{
		return;
	}

	if (_pChildren->containsObject(child))
	{
		this->detachChild(child, cleanup);
	}
}

void Node::removeChildByTag(int tag)
{
	this->removeChildByTag(tag, true);
}

void Node::removeChildByTag(int tag, bool cleanup)
{
	CCAssert(tag != kCCNodeTagInvalid, "Invalid tag");

	Node* child = this->getChildByTag(tag);

	if (child == NULL)
	{
		CCLOG("cocos2d: removeChildByTag(tag = %d): child not found!", tag);
	}
	else
	{
		this->removeChild(child, cleanup);
	}
}

void Node::removeFromParent()
{
	this->removeFromParentAndCleanup(true);
}

void Node::removeFromParentAndCleanup(bool cleanup)
{
	if (_pParent != NULL)
	{
		_pParent->removeChild(this, cleanup);
	}
}

void Node::removeAllChildren()
{
	this->removeAllChildrenWithCleanup(true);
}

void Node::removeAllChildrenWithCleanup(bool cleanup)
{
	// not using detachChild improves speed here
	if (_pChildren && _pChildren->count() > 0)
	{
		Object* child;
		ARRAY_FOREACH(_pChildren, child)
		{
			Node* pNode = (Node*)child;
			if (pNode)
			{
				// IMPORTANT:
				//  -1st do onExit
				//  -2nd cleanup
				if (_bRunning)
				{
					pNode->onExitTransitionDidStart();
					pNode->onExit();
				}

				if (cleanup)
				{
					pNode->cleanup();
				}
				// set parent nil at the end
				pNode->setParent(NULL);
			}
		}

		_pChildren->removeAllObjects();
	}

}


void Node::sortAllChildren()
{
	if (_bReorderChildDirty)
	{
		int i, j, length = _pChildren->data->num;
		Node** x = (Node**)_pChildren->data->arr;
		Node* tempItem;

		// insertion sort
		for (i = 1; i < length; i++)
		{
			tempItem = x[i];
			j = i - 1;

			//continue moving element downwards while zOrder is smaller or when zOrder is the same but mutatedIndex is smaller
			while (j >= 0 && (tempItem->_nZOrder < x[j]->_nZOrder || (tempItem->_nZOrder == x[j]->_nZOrder && tempItem->_uOrderOfArrival < x[j]->_uOrderOfArrival)))
			{
				x[j + 1] = x[j];
				j = j - 1;
			}
			x[j + 1] = tempItem;
		}

		//don't need to check children recursively, that's done in visit of each child

		_bReorderChildDirty = false;
	}
}

unsigned int Node::getOrderOfArrival()
{
	return _uOrderOfArrival;
}

void Node::setOrderOfArrival(unsigned int uOrderOfArrival)
{
	_uOrderOfArrival = uOrderOfArrival;
}

void Node::reorderChild(Node* child, int zOrder)
{
	CCAssert(child != NULL, "Child must be non-nil");
	_bReorderChildDirty = true;
	child->setOrderOfArrival(s_globalOrderOfArrival++);
	child->setZOrder(zOrder);
}

void Node::childrenAlloc(void)
{
	_pChildren = Array::createWithCapacity(4);
	_pChildren->retain();
}

void Node::insertChild(Node* child, int z)
{
	_bReorderChildDirty = true;
	ccArrayAppendObjectWithResize(_pChildren->data, child);
	child->setZOrder(z);
}

void Node::detachChild(Node* child, bool doCleanup)
{
	// IMPORTANT:
	//  -1st do onExit
	//  -2nd cleanup
	if (_bRunning)
	{
		child->onExitTransitionDidStart();
		child->onExit();
	}

	// If you don't do cleanup, the child's actions will not get removed and the
	// its scheduledSelectors_ dict will not get released!
	if (doCleanup)
	{
		child->cleanup();
	}

	child->setParent(NULL);

	_pChildren->removeObject(child);
}
//
//////////////////////////////




////////////////////////////////
// scale
//
float Node::getScale(void)
{
	CCAssert(_fScaleX == _fScaleY, "CCNode#scale. ScaleX != ScaleY. Don't know which one to return");
	return _fScaleX;
}

void Node::setScale(float scale)
{
	_fScaleX = _fScaleY = scale;
	_bTransformDirty = _bInverseDirty = true;
}

void Node::setScale(float fScaleX, float fScaleY)
{
	_fScaleX = fScaleX;
	_fScaleY = fScaleY;
	_bTransformDirty = _bInverseDirty = true;
}

float Node::getScaleX()
{
	return _fScaleX;
}

void Node::setScaleX(float newScaleX)
{
	_fScaleX = newScaleX;
	_bTransformDirty = _bInverseDirty = true;
}

float Node::getScaleY()
{
	return _fScaleY;
}

void Node::setScaleY(float newScaleY)
{
	_fScaleY = newScaleY;
	_bTransformDirty = _bInverseDirty = true;
}


////////////////////////////////
// rotation
//

float Node::getRotation()
{
	CCAssert(_fRotationX == _fRotationY, "CCNode#rotation. RotationX != RotationY. Don't know which one to return");
	return _fRotationX;
}

void Node::setRotation(float newRotation)
{
	_fRotationX = _fRotationY = newRotation;
	_bTransformDirty = _bInverseDirty = true;
}

float Node::getRotationX()
{
	return _fRotationX;
}

void Node::setRotationX(float fRotationX)
{
	_fRotationX = fRotationX;
	_bTransformDirty = _bInverseDirty = true;
}

float Node::getRotationY()
{
	return _fRotationY;
}

void Node::setRotationY(float fRotationY)
{
	_fRotationY = fRotationY;
	_bTransformDirty = _bInverseDirty = true;
}


////////////////////////////////
// skew 切变(扭曲)
//


float Node::getSkewX()
{
	return _fSkewX;
}

void Node::setSkewX(float newSkewX)
{
	_fSkewX = newSkewX;
	_bTransformDirty = _bInverseDirty = true;
}

float Node::getSkewY()
{
	return _fSkewY;
}

void Node::setSkewY(float newSkewY)
{
	_fSkewY = newSkewY;

	_bTransformDirty = _bInverseDirty = true;
}




////////////////////////////////
// position
//

const Point& Node::getPosition()
{
	return _obPosition;
}

void Node::setPosition(const Point& newPosition)
{
	_obPosition = newPosition;
	_bTransformDirty = _bInverseDirty = true;
}

void Node::getPosition(float* x, float* y)
{
	*x = _obPosition.x;
	*y = _obPosition.y;
}

void Node::setPosition(float x, float y)
{
	setPosition(ccp(x, y));
}

float Node::getPositionX(void)
{
	return _obPosition.x;
}

float Node::getPositionY(void)
{
	return  _obPosition.y;
}

void Node::setPositionX(float x)
{
	setPosition(ccp(x, _obPosition.y));
}

void Node::setPositionY(float y)
{
	setPosition(ccp(_obPosition.x, y));
}


////////////////////////////////
// content
//
const Size& Node::getContentSize() const
{
	return _obContentSize;
}

void Node::setContentSize(const Size& size)
{
	if (!size.equals(_obContentSize))
	{
		_obContentSize = size;

		_obAnchorPointInPoints = ccp(_obContentSize.width * _obAnchorPoint.x, _obContentSize.height * _obAnchorPoint.y);
		_bTransformDirty = _bInverseDirty = true;
	}
}


////////////////////////////////
// anchor point  [0,0] - [1,1]
//
void Node::setAnchorPoint(const Point& point)
{
	if (!point.equals(_obAnchorPoint))
	{
		_obAnchorPoint = point;
		_obAnchorPointInPoints = ccp(_obContentSize.width * _obAnchorPoint.x, _obContentSize.height * _obAnchorPoint.y);
		_bTransformDirty = _bInverseDirty = true;
	}
}

const Point& Node::getAnchorPointInPoints()
{
	return _obAnchorPointInPoints;
}

const Point& Node::getAnchorPoint()
{
	return _obAnchorPoint;
}

bool Node::isIgnoreAnchorPointForPosition()
{
	return _bIgnoreAnchorPointForPosition;
}
/// isRelativeAnchorPoint setter
void Node::ignoreAnchorPointForPosition(bool newValue)
{
	if (newValue != _bIgnoreAnchorPointForPosition)
	{
		_bIgnoreAnchorPointForPosition = newValue;
		_bTransformDirty = _bInverseDirty = true;
	}
}


//////////////////////////////
// render
//

float Node::getVertexZ()
{
	return _fVertexZ;
}

void Node::setVertexZ(float var)
{
	_fVertexZ = var;
}

GLProgram* Node::getShaderProgram()
{
	return _pShaderProgram;
}

void Node::setShaderProgram(GLProgram* pShaderProgram)
{
	CC_SAFE_RETAIN(pShaderProgram);
	CC_SAFE_RELEASE(_pShaderProgram);
	_pShaderProgram = pShaderProgram;
}

void Node::visit()
{
	if (!_bVisible)
		return;

	kmGLPushMatrix();

	this->transform();

	Node* pNode = NULL;
	unsigned int i = 0;

	if (_pChildren && _pChildren->count() > 0)
	{
		sortAllChildren();
		// draw children zOrder < 0
		ccArray* arrayData = _pChildren->data;
		for (; i < arrayData->num; i++)
		{
			pNode = (Node*)arrayData->arr[i];

			if (pNode && pNode->_nZOrder < 0)
			{
				pNode->visit();
			}
			else
			{
				break;
			}
		}

		// self draw
		this->draw();

		for (; i < arrayData->num; i++)
		{
			pNode = (Node*)arrayData->arr[i];
			if (pNode)
			{
				pNode->visit();
			}
		}
	}
	else
	{
		this->draw();
	}

	// 个人理解：已经添加进入队列且排过序的 该值已经无用
	// 插入排序是稳定排序  可以加快排序速度？ 
	// 理应放入sortAllChildren 调用子节点重置为0 为了简化放入了每个节点的visit中
	_uOrderOfArrival = 0;

	kmGLPopMatrix();
}

void Node::draw()
{
	//CCAssert(0, "sub class use");
	// Only use- this function to draw your stuff.
	// DON'T draw your stuff outside this method
}


void Node::transform()
{
	kmMat4 transfrom4x4;

	// Convert 3x3 into 4x4 matrix
	AffineTransform tmpAffine = this->nodeToParentTransform();
	CGAffineToGL(&tmpAffine, transfrom4x4.mat);

	// Update Z vertex manually
	transfrom4x4.mat[14] = _fVertexZ;

	kmGLMultMatrix(&transfrom4x4);
}

void Node::transformAncestors()
{
	if (_pParent != NULL)
	{
		_pParent->transformAncestors();
		_pParent->transform();
	}
}

// 提供给子类来实现
void Node::updateTransform()
{
	// Recursively iterate over children
	arrayMakeObjectsPerformSelector(_pChildren, updateTransform, Node*);
}


AffineTransform Node::nodeToParentTransform(void)
{
	if (!_bTransformDirty)
	{
		return _sTransform;
	}

	// Translate values
	float x = _obPosition.x;
	float y = _obPosition.y;

	if (_bIgnoreAnchorPointForPosition)
	{
		x += _obAnchorPointInPoints.x;
		y += _obAnchorPointInPoints.y;
	}

	// Rotation values
	// Change rotation code to handle X and Y
	// If we skew with the exact same value for both x and y then we're simply just rotating
	float cx = 1, sx = 0, cy = 1, sy = 0;
	if (_fRotationX || _fRotationY)
	{
		float radiansX = -CC_DEGREES_TO_RADIANS(_fRotationX);
		float radiansY = -CC_DEGREES_TO_RADIANS(_fRotationY);
		cx = cosf(radiansX);
		sx = sinf(radiansX);
		cy = cosf(radiansY);
		sy = sinf(radiansY);
	}

	bool needsSkewMatrix = (_fSkewX || _fSkewY);


	// optimization:
	// inline anchor point calculation if skew is not needed
	// Adjusted transform calculation for rotational skew
	if (!needsSkewMatrix && !_obAnchorPointInPoints.equals(PointZero))
	{
		x += cy * -_obAnchorPointInPoints.x * _fScaleX + -sx * -_obAnchorPointInPoints.y * _fScaleY;
		y += sy * -_obAnchorPointInPoints.x * _fScaleX + cx * -_obAnchorPointInPoints.y * _fScaleY;
	}


	// Build Transform Matrix
	// Adjusted transform calculation for rotational skew
	_sTransform = AffineTransformMake(cy * _fScaleX, sy * _fScaleX,
		-sx * _fScaleY, cx * _fScaleY,
		x, y);

	// If skew is needed, apply skew and then anchor point
	if (needsSkewMatrix)
	{
		AffineTransform skewMatrix = AffineTransformMake(1.0f, tanf(CC_DEGREES_TO_RADIANS(_fSkewY)),
			tanf(CC_DEGREES_TO_RADIANS(_fSkewX)), 1.0f,
			0.0f, 0.0f);
		_sTransform = AffineTransformConcat(skewMatrix, _sTransform);

		// adjust anchor point
		if (!_obAnchorPointInPoints.equals(PointZero))
		{
			_sTransform = AffineTransformTranslate(_sTransform, -_obAnchorPointInPoints.x, -_obAnchorPointInPoints.y);
		}
	}

	if (_bAdditionalTransformDirty)
	{
		_sTransform = AffineTransformConcat(_sTransform, _sAdditionalTransform);
		_bAdditionalTransformDirty = false;
	}

	_bTransformDirty = false;

	return _sTransform;
}

AffineTransform Node::parentToNodeTransform(void)
{
	if (_bInverseDirty) {
		_sInverse = AffineTransformInvert(this->nodeToParentTransform());
		_bInverseDirty = false;
	}

	return _sInverse;
}

AffineTransform Node::nodeToWorldTransform()
{
	AffineTransform t = this->nodeToParentTransform();

	for (Node* p = _pParent; p != NULL; p = p->getParent())
		t = AffineTransformConcat(t, p->nodeToParentTransform());

	return t;
}

AffineTransform Node::worldToNodeTransform(void)
{
	return AffineTransformInvert(this->nodeToWorldTransform());
}



void Node::setAdditionalTransform(const AffineTransform& additionalTransform)
{
	_sAdditionalTransform = additionalTransform;
	_bTransformDirty = true;
	_bAdditionalTransformDirty = true;
}



Point Node::convertToNodeSpace(const Point& worldPoint)
{
	Point ret = PointApplyAffineTransform(worldPoint, worldToNodeTransform());
	return ret;
}

Point Node::convertToWorldSpace(const Point& nodePoint)
{
	Point ret = PointApplyAffineTransform(nodePoint, nodeToWorldTransform());
	return ret;
}

Point Node::convertToNodeSpaceAR(const Point& worldPoint)
{
	Point nodePoint = convertToNodeSpace(worldPoint);
	return ccpSub(nodePoint, _obAnchorPointInPoints);
}

Point Node::convertToWorldSpaceAR(const Point& nodePoint)
{
	Point pt = ccpAdd(nodePoint, _obAnchorPointInPoints);
	return convertToWorldSpace(pt);
}

// convenience methods which take a CCTouch instead of Point
Point Node::convertTouchToNodeSpace(Touch* touch)
{
	Point point = touch->getLocation();
	return this->convertToNodeSpace(point);
}
Point Node::convertTouchToNodeSpaceAR(Touch* touch)
{
	Point point = touch->getLocation();
	return this->convertToNodeSpaceAR(point);
}





////////////////////////////////
//
//



NS_CC_END