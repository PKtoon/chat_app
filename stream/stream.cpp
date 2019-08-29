#include "stream.h"
#include <boost/asio.hpp>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

std::string Stream::getSerialized()
{
    std::ostringstream archiveStream;
    boost::archive::text_oarchive archive(archiveStream);
    archive<<*this;
    return archiveStream.str();
}

void Stream::getUnSerialized(std::string& inData)
{
    std::istringstream archiveStream(inData);
    boost::archive::text_iarchive archive(archiveStream);
    archive>>*this;
}
