#include "propnet/include/basenet.hpp"

#include <iostream>

int main(void)
{
    std::cout << "Loading basenet\n";
    const propnet::BaseNet basenet {"scissors_paper_rock"};

    return 0;
}
