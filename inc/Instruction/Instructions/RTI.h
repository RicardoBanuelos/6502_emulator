#pragma 

#include "Instruction/Instruction.h"

class RTI : public Instruction
{
    
public:
    RTI(std::shared_ptr<ICPU> icpu);

    ~RTI();

    void run() override;
};

