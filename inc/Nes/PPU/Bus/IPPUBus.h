#pragma once
#include <memory>
#include "IBus.h"
#include "IPPUBus.h"
#include "ICatridge.h"

class IPPUBus : public IBus
{

public:
    IPPUBus() = default;
    ~IPPUBus() = default;

    virtual void connectCatridge(std::shared_ptr<ICatridge> catridge) = 0;
};
