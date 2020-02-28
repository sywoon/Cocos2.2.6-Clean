#include "Node.h"
#include "cocoa/String.h"
#include "cocoa/Array.h"
#include "cocoa/extend/PointExtend.h"
#include "cocoa/extend/AffineExtend.h"
#include "kazmath/GL/matrix.h"
#include "platform/PlatformBase.h"
#include "shaders/GLProgram.h"



NS_CC_BEGIN

static int s_globalOrderOfArrival = 1;


CCNode* CCNode::create(void)
{
	CCNode* pRet = new CCNode();
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



CCNode::CCNode(void)
	: m_bRunning(false)
	, m_bVisible(true)
	, m_nTag(kCCNodeTagInvalid)
	, m_nZOrder(0)
	, m_uOrderOfArrival(0)
	, m_fVertexZ(0.0f)

	, m_pChildren(NULL)
	, m_pParent(NULL)
	, m_bReorderChildDirty(false)
	
	, m_obPosition(PointZero)
	, m_obContentSize(SizeZero)

	, m_fScaleX(1.0f)
	, m_fScaleY(1.0f)
	, m_fRotationX(0.0f)
	, m_fRotationY(0.0f)
	, m_fSkewX(0.0f)
	, m_fSkewY(0.0f)

	, m_obAnchorPointInPoints(PointZero)
	, m_obAnchorPoint(PointZero)
	, m_bIgnoreAnchorPointForPosition(false)

	, m_bTransformDirty(false)
	, m_bInverseDirty(false)
	, m_bAdditionalTransformDirty(false)

	, m_pShaderProgram(NULL)
{

}

CCNode::~CCNode(void)
{
	CC_SAFE_RELEASE(m_pShaderProgram);

	if (m_pChildren && m_pChildren->count() > 0)
	{
		Object* child;
		ARRAY_FOREACH(m_pChildren, child)
		{
			CCNode* pChild = (CCNode*)child;
			if (pChild)
			{
				pChild->m_pParent = NULL;
			}
		}
	}
	CC_SAFE_RELEASE(m_pChildren);

	m_pParent = NULL;
}

bool CCNode::init()
{
	return true;
}

const char* CCNode::description()
{
	return String::createWithFormat("<CCNode | Tag = %d>", m_nTag)->getCString();
}

bool CCNode::isRunning()
{
	return m_bRunning;
}


void CCNode::onEnter()
{
	//fix setTouchEnabled not take effect when called the function in onEnter in JSBinding.
	m_bRunning = true;

	//Judge the running state for prevent called onEnter method more than once,it's possible that this function called by addChild  
	if (m_pChildren && m_pChildren->count() > 0)
	{
		Object* child;
		CCNode* node;
		ARRAY_FOREACH(m_pChildren, child)
		{
			node = (CCNode*)child;
			if (!node->isRunning())
			{
				node->onEnter();
			}
		}
	}
}

void CCNode::onEnterTransitionDidFinish()
{
	arrayMakeObjectsPerformSelector(m_pChildren, onEnterTransitionDidFinish, CCNode*);
}

void CCNode::onExitTransitionDidStart()
{
	arrayMakeObjectsPerformSelector(m_pChildren, onExitTransitionDidStart, CCNode*);
}

void CCNode::onExit()
{
	m_bRunning = false;

	arrayMakeObjectsPerformSelector(m_pChildren, onExit, CCNode*);
}

void CCNode::cleanup()
{
	arrayMakeObjectsPerformSelector(m_pChildren, cleanup, CCNode*);
}



bool CCNode::isVisible()
{
	return m_bVisible;
}

void CCNode::setVisible(bool var)
{
	m_bVisible = var;
}

void CCNode::setZOrder(int z)
{
	m_nZOrder = z;
	if (m_pParent)
	{
		m_pParent->reorderChild(this, z);
	}
}

int CCNode::getZOrder()
{
	return m_nZOrder;
}

int CCNode::getTag() const
{
	return m_nTag;
}

void CCNode::setTag(int var)
{
	m_nTag = var;
}



//////////////////////////////
// parent and children
//

CCNode* CCNode::getParent()
{
	return m_pParent;
}

void CCNode::setParent(CCNode* var)
{
	m_pParent = var;
}

Array* CCNode::getChildren()
{
	return m_pChildren;
}

unsigned int CCNode::getChildrenCount(void) const
{
	return m_pChildren ? m_pChildren->count() : 0;
}

void CCNode::addChild(CCNode* child)
{
	CCAssert(child != NULL, "Argument must be non-nil");
	this->addChild(child, child->m_nZOrder, child->m_nTag);
}

void CCNode::addChild(CCNode* child, int zOrder)
{
	CCAssert(child != NULL, "Argument must be non-nil");
	this->addChild(child, zOrder, child->m_nTag);
}


void CCNode::addChild(CCNode* child, int zOrder, int tag)
{
	CCAssert(child != NULL, "Argument must be non-nil");
	CCAssert(child->m_pParent == NULL, "child already added. It can't be added again");

	if (!m_pChildren)
	{
		this->childrenAlloc();
	}

	this->insertChild(child, zOrder);

	child->m_nTag = tag;

	child->setParent(this);
	child->setOrderOfArrival(s_globalOrderOfArrival++);

	if (m_bRunning)
	{
		child->onEnter();
		child->onEnterTransitionDidFinish();
	}
}

CCNode* CCNode::getChildByTag(int aTag)
{
	CCAssert(aTag != kCCNodeTagInvalid, "Invalid tag");

	if (m_pChildren && m_pChildren->count() > 0)
	{
		Object* child;
		ARRAY_FOREACH(m_pChildren, child)
		{
			CCNode* pNode = (CCNode*)child;
			if (pNode && pNode->m_nTag == aTag)
				return pNode;
		}
	}
	return NULL;
}


void CCNode::removeChild(CCNode* child)
{
	this->removeChild(child, true);
}

void CCNode::removeChild(CCNode* child, bool cleanup)
{
	if (m_pChildren == NULL)
	{
		return;
	}

	if (m_pChildren->containsObject(child))
	{
		this->detachChild(child, cleanup);
	}
}

void CCNode::removeChildByTag(int tag)
{
	this->removeChildByTag(tag, true);
}

void CCNode::removeChildByTag(int tag, bool cleanup)
{
	CCAssert(tag != kCCNodeTagInvalid, "Invalid tag");

	CCNode* child = this->getChildByTag(tag);

	if (child == NULL)
	{
		CCLOG("cocos2d: removeChildByTag(tag = %d): child not found!", tag);
	}
	else
	{
		this->removeChild(child, cleanup);
	}
}

void CCNode::removeFromParent()
{
	this->removeFromParentAndCleanup(true);
}

void CCNode::removeFromParentAndCleanup(bool cleanup)
{
	if (m_pParent != NULL)
	{
		m_pParent->removeChild(this, cleanup);
	}
}

void CCNode::removeAllChildren()
{
	this->removeAllChildrenWithCleanup(true);
}

void CCNode::removeAllChildrenWithCleanup(bool cleanup)
{
	// not using detachChild improves speed here
	if (m_pChildren && m_pChildren->count() > 0)
	{
		Object* child;
		ARRAY_FOREACH(m_pChildren, child)
		{
			CCNode* pNode = (CCNode*)child;
			if (pNode)
			{
				// IMPORTANT:
				//  -1st do onExit
				//  -2nd cleanup
				if (m_bRunning)
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

		m_pChildren->removeAllObjects();
	}

}


void CCNode::sortAllChildren()
{
	if (m_bReorderChildDirty)
	{
		int i, j, length = m_pChildren->data->num;
		CCNode** x = (CCNode**)m_pChildren->data->arr;
		CCNode* tempItem;

		// insertion sort
		for (i = 1; i < length; i++)
		{
			tempItem = x[i];
			j = i - 1;

			//continue moving element downwards while zOrder is smaller or when zOrder is the same but mutatedIndex is smaller
			while (j >= 0 && (tempItem->m_nZOrder < x[j]->m_nZOrder || (tempItem->m_nZOrder == x[j]->m_nZOrder && tempItem->m_uOrderOfArrival < x[j]->m_uOrderOfArrival)))
			{
				x[j + 1] = x[j];
				j = j - 1;
			}
			x[j + 1] = tempItem;
		}

		//don't need to check children recursively, that's done in visit of each child

		m_bReorderChildDirty = false;
	}
}

unsigned int CCNode::getOrderOfArrival()
{
	return m_uOrderOfArrival;
}

void CCNode::setOrderOfArrival(unsigned int uOrderOfArrival)
{
	m_uOrderOfArrival = uOrderOfArrival;
}

void CCNode::reorderChild(CCNode* child, int zOrder)
{
	CCAssert(child != NULL, "Child must be non-nil");
	m_bReorderChildDirty = true;
	child->setOrderOfArrival(s_globalOrderOfArrival++);
	child->setZOrder(zOrder);
}

void CCNode::childrenAlloc(void)
{
	m_pChildren = Array::createWithCapacity(4);
	m_pChildren->retain();
}

void CCNode::insertChild(CCNode* child, int z)
{
	m_bReorderChildDirty = true;
	ccArrayAppendObjectWithResize(m_pChildren->data, child);
	child->setZOrder(z);
}

void CCNode::detachChild(CCNode* child, bool doCleanup)
{
	// IMPORTANT:
	//  -1st do onExit
	//  -2nd cleanup
	if (m_bRunning)
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

	m_pChildren->removeObject(child);
}
//
//////////////////////////////




////////////////////////////////
// scale
//
float CCNode::getScale(void)
{
	CCAssert(m_fScaleX == m_fScaleY, "CCNode#scale. ScaleX != ScaleY. Don't know which one to return");
	return m_fScaleX;
}

void CCNode::setScale(float scale)
{
	m_fScaleX = m_fScaleY = scale;
	m_bTransformDirty = m_bInverseDirty = true;
}

void CCNode::setScale(float fScaleX, float fScaleY)
{
	m_fScaleX = fScaleX;
	m_fScaleY = fScaleY;
	m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getScaleX()
{
	return m_fScaleX;
}

void CCNode::setScaleX(float newScaleX)
{
	m_fScaleX = newScaleX;
	m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getScaleY()
{
	return m_fScaleY;
}

void CCNode::setScaleY(float newScaleY)
{
	m_fScaleY = newScaleY;
	m_bTransformDirty = m_bInverseDirty = true;
}


////////////////////////////////
// rotation
//

float CCNode::getRotation()
{
	CCAssert(m_fRotationX == m_fRotationY, "CCNode#rotation. RotationX != RotationY. Don't know which one to return");
	return m_fRotationX;
}

void CCNode::setRotation(float newRotation)
{
	m_fRotationX = m_fRotationY = newRotation;
	m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getRotationX()
{
	return m_fRotationX;
}

void CCNode::setRotationX(float fRotationX)
{
	m_fRotationX = fRotationX;
	m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getRotationY()
{
	return m_fRotationY;
}

void CCNode::setRotationY(float fRotationY)
{
	m_fRotationY = fRotationY;
	m_bTransformDirty = m_bInverseDirty = true;
}


////////////////////////////////
// skew 切变(扭曲)
//


float CCNode::getSkewX()
{
	return m_fSkewX;
}

void CCNode::setSkewX(float newSkewX)
{
	m_fSkewX = newSkewX;
	m_bTransformDirty = m_bInverseDirty = true;
}

float CCNode::getSkewY()
{
	return m_fSkewY;
}

void CCNode::setSkewY(float newSkewY)
{
	m_fSkewY = newSkewY;

	m_bTransformDirty = m_bInverseDirty = true;
}




////////////////////////////////
// position
//

const Point& CCNode::getPosition()
{
	return m_obPosition;
}

void CCNode::setPosition(const Point& newPosition)
{
	m_obPosition = newPosition;
	m_bTransformDirty = m_bInverseDirty = true;
}

void CCNode::getPosition(float* x, float* y)
{
	*x = m_obPosition.x;
	*y = m_obPosition.y;
}

void CCNode::setPosition(float x, float y)
{
	setPosition(ccp(x, y));
}

float CCNode::getPositionX(void)
{
	return m_obPosition.x;
}

float CCNode::getPositionY(void)
{
	return  m_obPosition.y;
}

void CCNode::setPositionX(float x)
{
	setPosition(ccp(x, m_obPosition.y));
}

void CCNode::setPositionY(float y)
{
	setPosition(ccp(m_obPosition.x, y));
}


////////////////////////////////
// content
//
const Size& CCNode::getContentSize() const
{
	return m_obContentSize;
}

void CCNode::setContentSize(const Size& size)
{
	if (!size.equals(m_obContentSize))
	{
		m_obContentSize = size;

		m_obAnchorPointInPoints = ccp(m_obContentSize.width * m_obAnchorPoint.x, m_obContentSize.height * m_obAnchorPoint.y);
		m_bTransformDirty = m_bInverseDirty = true;
	}
}


////////////////////////////////
// anchor point  [0,0] - [1,1]
//
void CCNode::setAnchorPoint(const Point& point)
{
	if (!point.equals(m_obAnchorPoint))
	{
		m_obAnchorPoint = point;
		m_obAnchorPointInPoints = ccp(m_obContentSize.width * m_obAnchorPoint.x, m_obContentSize.height * m_obAnchorPoint.y);
		m_bTransformDirty = m_bInverseDirty = true;
	}
}

const Point& CCNode::getAnchorPointInPoints()
{
	return m_obAnchorPointInPoints;
}

const Point& CCNode::getAnchorPoint()
{
	return m_obAnchorPoint;
}

bool CCNode::isIgnoreAnchorPointForPosition()
{
	return m_bIgnoreAnchorPointForPosition;
}
/// isRelativeAnchorPoint setter
void CCNode::ignoreAnchorPointForPosition(bool newValue)
{
	if (newValue != m_bIgnoreAnchorPointForPosition)
	{
		m_bIgnoreAnchorPointForPosition = newValue;
		m_bTransformDirty = m_bInverseDirty = true;
	}
}


//////////////////////////////
// render
//

float CCNode::getVertexZ()
{
	return m_fVertexZ;
}

void CCNode::setVertexZ(float var)
{
	m_fVertexZ = var;
}

GLProgram* CCNode::getShaderProgram()
{
	return m_pShaderProgram;
}

void CCNode::setShaderProgram(GLProgram* pShaderProgram)
{
	CC_SAFE_RETAIN(pShaderProgram);
	CC_SAFE_RELEASE(m_pShaderProgram);
	m_pShaderProgram = pShaderProgram;
}

void CCNode::visit()
{
	if (!m_bVisible)
		return;

	kmGLPushMatrix();

	this->transform();

	CCNode* pNode = NULL;
	unsigned int i = 0;

	if (m_pChildren && m_pChildren->count() > 0)
	{
		sortAllChildren();
		// draw children zOrder < 0
		ccArray* arrayData = m_pChildren->data;
		for (; i < arrayData->num; i++)
		{
			pNode = (CCNode*)arrayData->arr[i];

			if (pNode && pNode->m_nZOrder < 0)
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
			pNode = (CCNode*)arrayData->arr[i];
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
	m_uOrderOfArrival = 0;

	kmGLPopMatrix();
}

void CCNode::draw()
{
	//CCAssert(0, "sub class use");
	// Only use- this function to draw your stuff.
	// DON'T draw your stuff outside this method
}


void CCNode::transform()
{
	kmMat4 transfrom4x4;

	// Convert 3x3 into 4x4 matrix
	AffineTransform tmpAffine = this->nodeToParentTransform();
	CGAffineToGL(&tmpAffine, transfrom4x4.mat);

	// Update Z vertex manually
	transfrom4x4.mat[14] = m_fVertexZ;

	kmGLMultMatrix(&transfrom4x4);
}

void CCNode::transformAncestors()
{
	if (m_pParent != NULL)
	{
		m_pParent->transformAncestors();
		m_pParent->transform();
	}
}

// 提供给子类来实现
void CCNode::updateTransform()
{
	// Recursively iterate over children
	arrayMakeObjectsPerformSelector(m_pChildren, updateTransform, CCNode*);
}


AffineTransform CCNode::nodeToParentTransform(void)
{
	if (!m_bTransformDirty)
	{
		return m_sTransform;
	}

	// Translate values
	float x = m_obPosition.x;
	float y = m_obPosition.y;

	if (m_bIgnoreAnchorPointForPosition)
	{
		x += m_obAnchorPointInPoints.x;
		y += m_obAnchorPointInPoints.y;
	}

	// Rotation values
	// Change rotation code to handle X and Y
	// If we skew with the exact same value for both x and y then we're simply just rotating
	float cx = 1, sx = 0, cy = 1, sy = 0;
	if (m_fRotationX || m_fRotationY)
	{
		float radiansX = -CC_DEGREES_TO_RADIANS(m_fRotationX);
		float radiansY = -CC_DEGREES_TO_RADIANS(m_fRotationY);
		cx = cosf(radiansX);
		sx = sinf(radiansX);
		cy = cosf(radiansY);
		sy = sinf(radiansY);
	}

	bool needsSkewMatrix = (m_fSkewX || m_fSkewY);


	// optimization:
	// inline anchor point calculation if skew is not needed
	// Adjusted transform calculation for rotational skew
	if (!needsSkewMatrix && !m_obAnchorPointInPoints.equals(PointZero))
	{
		x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
		y += sy * -m_obAnchorPointInPoints.x * m_fScaleX + cx * -m_obAnchorPointInPoints.y * m_fScaleY;
	}


	// Build Transform Matrix
	// Adjusted transform calculation for rotational skew
	m_sTransform = AffineTransformMake(cy * m_fScaleX, sy * m_fScaleX,
		-sx * m_fScaleY, cx * m_fScaleY,
		x, y);

	// If skew is needed, apply skew and then anchor point
	if (needsSkewMatrix)
	{
		AffineTransform skewMatrix = AffineTransformMake(1.0f, tanf(CC_DEGREES_TO_RADIANS(m_fSkewY)),
			tanf(CC_DEGREES_TO_RADIANS(m_fSkewX)), 1.0f,
			0.0f, 0.0f);
		m_sTransform = AffineTransformConcat(skewMatrix, m_sTransform);

		// adjust anchor point
		if (!m_obAnchorPointInPoints.equals(PointZero))
		{
			m_sTransform = AffineTransformTranslate(m_sTransform, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
		}
	}

	if (m_bAdditionalTransformDirty)
	{
		m_sTransform = AffineTransformConcat(m_sTransform, m_sAdditionalTransform);
		m_bAdditionalTransformDirty = false;
	}

	m_bTransformDirty = false;

	return m_sTransform;
}

AffineTransform CCNode::parentToNodeTransform(void)
{
	if (m_bInverseDirty) {
		m_sInverse = AffineTransformInvert(this->nodeToParentTransform());
		m_bInverseDirty = false;
	}

	return m_sInverse;
}

AffineTransform CCNode::nodeToWorldTransform()
{
	AffineTransform t = this->nodeToParentTransform();

	for (CCNode* p = m_pParent; p != NULL; p = p->getParent())
		t = AffineTransformConcat(t, p->nodeToParentTransform());

	return t;
}

AffineTransform CCNode::worldToNodeTransform(void)
{
	return AffineTransformInvert(this->nodeToWorldTransform());
}



void CCNode::setAdditionalTransform(const AffineTransform& additionalTransform)
{
	m_sAdditionalTransform = additionalTransform;
	m_bTransformDirty = true;
	m_bAdditionalTransformDirty = true;
}




////////////////////////////////
//
//



NS_CC_END