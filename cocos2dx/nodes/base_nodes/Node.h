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
class Touch;
class CC_DLL Node : public Object
{
public:
    Node(void);
    virtual ~Node(void);

    static Node*      create(void);

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
    virtual void        setParent(Node* parent);
    virtual Node*       getParent();

    virtual Array*      getChildren();
    virtual unsigned int getChildrenCount(void) const;

    virtual void        addChild(Node* child);
    virtual void        addChild(Node* child, int zOrder);
    virtual void        addChild(Node* child, int zOrder, int tag);

    virtual Node*       getChildByTag(int tag);

    virtual void        removeChild(Node* child);
    virtual void        removeChild(Node* child, bool cleanup);
    virtual void        removeChildByTag(int tag);
    virtual void        removeChildByTag(int tag, bool cleanup);

    virtual void        removeFromParent();
    virtual void        removeFromParentAndCleanup(bool cleanup);

    virtual void        removeAllChildren();
    virtual void        removeAllChildrenWithCleanup(bool cleanup);

    virtual void        sortAllChildren();

    virtual void        setOrderOfArrival(unsigned int uOrderOfArrival);
    virtual unsigned int getOrderOfArrival();

    virtual void        reorderChild(Node* child, int zOrder);


private:
    void                childrenAlloc(void);
    void                insertChild(Node* child, int z);
    void                detachChild(Node* child, bool doCleanup);

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


    Point   convertToNodeSpace(const Point& worldPoint);
    Point   convertToWorldSpace(const Point& nodePoint);
    Point   convertToNodeSpaceAR(const Point& worldPoint);  //����ê�� ������Ϊ����
    Point   convertToWorldSpaceAR(const Point& nodePoint);
    Point   convertTouchToNodeSpace(Touch* touch);
    Point   convertTouchToNodeSpaceAR(Touch* touch);


protected:
    virtual bool        init();


protected:
    bool            _bRunning;  //onEnter���������
    bool            _bVisible;
    int             _nTag;
    int             _nZOrder;
    unsigned int    _uOrderOfArrival;  //��zorder��ͬʱ ��������� һ��Ϊ��ӵ�˳��
    float           _fVertexZ;     //OpenGL real Z vertex

	Array*          _pChildren;
	Node*           _pParent;
    bool            _bReorderChildDirty;

    Point           _obPosition;
    Size            _obContentSize;

	float           _fScaleX;
	float           _fScaleY;
	float           _fRotationX; 
	float           _fRotationY;
	float           _fSkewX;
	float           _fSkewY;
    
	Point           _obAnchorPointInPoints;    //(100,100) �����ʵ�ʵ�λ��
	Point           _obAnchorPoint;            //(0.5, 0.5)
    bool            _bIgnoreAnchorPointForPosition;

	bool            _bTransformDirty;  //node to parent
	bool            _bInverseDirty;    //parent to node
    bool            _bAdditionalTransformDirty;

    AffineTransform _sTransform;
	AffineTransform _sAdditionalTransform;
	AffineTransform _sInverse;

    GLProgram*      _pShaderProgram;      //< OpenGL shader
};




NS_CC_END
