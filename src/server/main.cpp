#include <iostream>

#include "server.hpp"

int main(int argc, char* argv[])
{
    if (argc!=2)
    {
        std::cerr<<"Usage: server port"<<std::endl;
        exit(1);
    }
    Server s(std::atoi(argv[1]));    
    return 0;
}
