#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <boost/asio.hpp>
//#include <string>

inline void logIt(std::string name,std::string msg/*, std::ifstream& file*/)
{
    std::cerr<<name<<msg<<std::endl;
//    file<<msg;
}

inline void logError(std::string name,std::string msg, const boost::system::error_code error)
{
    std::ostringstream os;
    os<<error;
    std::cerr<<name<<msg<<os.str()<<std::endl;
}

inline void logRead(std::string name, std::string msg, unsigned long data)
{
    std::ostringstream os;
    os<<data;
    std::cerr<<name<<msg<<os.str()<<std::endl;
}

inline void logWrite(std::string name, std::string msg, std::string receiver, unsigned long data)
{
    std::ostringstream os;
    os<<data;
    std::cerr<<name<<msg<<receiver<<": "<<os.str()<<std::endl;
}

#endif // LOGGING_H
