#include <iostream>

#include "server.hpp"

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cerr<<"Usage: server [PORT] [POSTGRESQL URI]\n";
        exit(1);
    }
#ifndef NDEBUG
    std::cerr<<"This is debug build! Only for testing purpose!\n";
#endif
    Server s(std::atoi(argv[1]), argv[2]);
    s.run();
    return 0;
}
