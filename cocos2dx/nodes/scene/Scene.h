#pragma once
#include "ccMacros.h"
#include "nodes/base_nodes/Node.h"


NS_CC_BEGIN


class CC_DLL Scene : public Node
{
public:
    Scene();
    virtual ~Scene();
    static Scene* create(void);

protected:
    bool    init();
};





NS_CC_END


