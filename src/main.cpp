#include <iostream>

#include "CPU/CPU.h"

int main(int, char**)
{
    std::shared_ptr<CPU> cpu = std::make_shared<CPU>();
    cpu->init();
    
    std::cout << "Hello World!";
    return 0;
}
