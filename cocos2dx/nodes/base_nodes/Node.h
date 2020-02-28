#pragma once

#include "ccMacros.h"
#include "cocoa/Object.h"
#include "cocoa/Geometry.h"
#include "cocoa/AffineTransform.h"



NS_CC_BEGIN

enum {
	kCCNodeTagInvalid = -1,
};


class GLProgram;
class CC_DLL CCNode : public Object
{
public:
    CCNode(void);
    virtual ~CCNode(void);

    static CCNode*      create(void);

public:
    const char*         description(void);

public:
    virtual bool        isRunning();

    virtual void        onEnter();
    virtual void        onEnterTransitionDidFinish();

    virtual void        onExitTransitionDidStart();
    virtual void        onExit();

    // �ͷ�action��scheduler
    virtual void        cleanup(void);

public:
	virtual void        setVisible(bool visible);
	virtual bool        isVisible();

	virtual void        setZOrder(int zOrder);
	virtual int         getZOrder();

	virtual int         getTag() const;
	virtual void        setTag(int nTag);

public:
    virtual void        setParent(CCNode* parent);
    virtual CCNode*     getParent();

    virtual Array*      getChildren();
    virtual unsigned int getChildrenCount(void) const;

    virtual void        addChild(CCNode* child);
    virtual void        addChild(CCNode* child, int zOrder);
    virtual void        addChild(CCNode* child, int zOrder, int tag);

    virtual CCNode*     getChildByTag(int tag);

    virtual void        removeChild(CCNode* child);
    virtual void        removeChild(CCNode* child, bool cleanup);
    virtual void        removeChildByTag(int tag);
    virtual void        removeChildByTag(int tag, bool cleanup);

    virtual void        removeFromParent();
    virtual void        removeFromParentAndCleanup(bool cleanup);

    virtual void        removeAllChildren();
    virtual void        removeAllChildrenWithCleanup(bool cleanup);

    virtual void        sortAllChildren();

    virtual void        setOrderOfArrival(unsigned int uOrderOfArrival);
    virtual unsigned int getOrderOfArrival();

    virtual void        reorderChild(CCNode* child, int zOrder);


private:
    void                childrenAlloc(void);
    void                insertChild(CCNode* child, int z);
    void                detachChild(CCNode* child, bool doCleanup);

public:
    virtual void        setScaleX(float fScaleX);
    virtual float       getScaleX();
    virtual void        setScaleY(float fScaleY);
    virtual float       getScaleY();
    virtual void        setScale(float scale);
    virtual void        setScale(float fScaleX, float fScaleY);
    virtual float       getScale();

    // Positive values rotate node clockwise, and negative values for anti-clockwise.
    virtual void        setRotation(float fRotation);
    virtual float       getRotation();
    virtual void        setRotationX(float fRotaionX);
    virtual float       getRotationX();
    virtual void        setRotationY(float fRotationY);
    virtual float       getRotationY();

	virtual void        setSkewX(float fSkewX);
	virtual float       getSkewX();
	virtual void        setSkewY(float fSkewY);
	virtual float       getSkewY();

    virtual void        setPosition(const Point& position);
    virtual void        setPosition(float x, float y);
    virtual const Point& getPosition();
    virtual void        getPosition(float* x, float* y);

	virtual void        setPositionX(float x);
	virtual float       getPositionX(void);
	virtual void        setPositionY(float y);
	virtual float       getPositionY(void);

    virtual void        setContentSize(const Size& contentSize);
    virtual const Size& getContentSize() const;

    virtual void        setAnchorPoint(const Point& anchorPoint);
    virtual const Point& getAnchorPoint();
    virtual const Point& getAnchorPointInPoints();

    // Ĭ�Ͻ�ê������λ����ΪͼƬ��ԭ�� �ҵ����ڵ���
    // Ϊ��CCLayer CCScene������ʾ��������Ĭ��Ϊtrue�� ������ê��λ�� �����½���Ϊԭ��   
    // ֱ������anchorpoint ���Ǹ��ã�
    virtual void        ignoreAnchorPointForPosition(bool ignore);
    virtual bool        isIgnoreAnchorPointForPosition();


public:
	// opengl z �ŵ���Ⱦ���һ��
	virtual void        setVertexZ(float vertexZ);
	virtual float       getVertexZ();

    virtual GLProgram*  getShaderProgram();
    virtual void        setShaderProgram(GLProgram* pShaderProgram);

	virtual void        visit(void);
	virtual void        draw(void);

    //Performs OpenGL view-matrix transformation based on position, scale, rotation and other attributes.
    void                transform(void);

    /**  ��������transform
     * Performs OpenGL view-matrix transformation of it's ancestors.
     * Generally the ancestors are already transformed, but in certain cases (eg: attaching a FBO)
     * It's necessary to transform the ancestors again.
     */
    void                transformAncestors(void);
    virtual void        updateTransform(void);

    virtual AffineTransform nodeToParentTransform(void);
    virtual AffineTransform parentToNodeTransform(void);
    virtual AffineTransform nodeToWorldTransform(void);
    virtual AffineTransform worldToNodeTransform(void);

    // ����������������Ӿ��� �����ٷǸ��ӽṹ�� ģ������Ч��  BatchNode�õ�
	// note The additional transform will be concatenated at the end of nodeToParentTransform.
	// It could be used to simulate `parent-child` relationship between two nodes(e.g.one is in BatchNode, another isn't).
    void                setAdditionalTransform(const AffineTransform& additionalTransform);



protected:
    virtual bool        init();


protected:
    bool            m_bRunning;  //onEnter���������
    bool            m_bVisible;
    int             m_nTag;
    int             m_nZOrder;
    unsigned int    m_uOrderOfArrival;  //��zorder��ͬʱ ��������� һ��Ϊ��ӵ�˳��
    float           m_fVertexZ;     //OpenGL real Z vertex

	Array*          m_pChildren;
	CCNode*         m_pParent;
    bool            m_bReorderChildDirty;

    Point           m_obPosition;
    Size            m_obContentSize;

	float           m_fScaleX;
	float           m_fScaleY;
	float           m_fRotationX; 
	float           m_fRotationY;
	float           m_fSkewX;
	float           m_fSkewY;
    
	Point           m_obAnchorPointInPoints;    //(100,100) �����ʵ�ʵ�λ��
	Point           m_obAnchorPoint;            //(0.5, 0.5)
    bool            m_bIgnoreAnchorPointForPosition;

	bool            m_bTransformDirty;  //node to parent
	bool            m_bInverseDirty;    //parent to node
    bool            m_bAdditionalTransformDirty;

    AffineTransform m_sTransform;
	AffineTransform m_sAdditionalTransform;
	AffineTransform m_sInverse;

    GLProgram*      m_pShaderProgram;      //< OpenGL shader
};




NS_CC_END
