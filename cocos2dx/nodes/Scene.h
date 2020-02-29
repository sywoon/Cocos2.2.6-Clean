#pragma once
#include "ccMacros.h"
#include "nodes/base_nodes/Node.h"


NS_CC_BEGIN


class CC_DLL Scene : public Node
{
public:
    virtual ~Scene();
    bool init();

    static Scene* create(void);

protected:
    Scene();
};





NS_CC_END


