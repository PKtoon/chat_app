#include "server.h"
#include "user.h"

void User::intro()
{
    inDataSize=20;
    inData.resize(inDataSize);
    boost::asio::async_read(socket,boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t)
    {
        std::string temp{inData.begin(),inData.end()};
        std::istringstream is {temp};
        is>>name;
        std::cout<<name<<" connected"<<std::endl;
        readHeader();
    });
}

void User::readHeader()
{
    inHeader.resize(headerLength);
    boost::asio::async_read(socket,boost::asio::buffer(inHeader),[this](const boost::system::error_code& error, std::size_t)
    {
        if (error)
        {
            readHeader();
        }
        else
        {
            std::string temp{inHeader.begin(),inHeader.end()};
            if(temp=="ping")
            {
                alive=true;
                readHeader();
            }
            else
            {
                std::istringstream is{temp};
                is>>std::hex>>inDataSize;
                readBody();
            }
        }
    });
}

void User::readBody()
{
    inData.resize(inDataSize);
    boost::asio::async_read(socket,boost::asio::buffer(inData),[this](const boost::system::error_code& error, std::size_t)
    {
        if (error)
        {
            readHeader();
        }
        else
        {
            std::string temp{inData.begin(),inData.end()};
            writer(Stream(temp));
            readHeader();
        }
    });
}

void User::writer(Stream st)
{
    User* u;
    if(s->searchUser(st.getReceiver()))
    {
        u = s->getUser(st.getReceiver());
    }
    else
    {
        std::string msg = st.getReceiver()+" not found";
        st = Stream("server", name, msg);
        u=this;
    }

    std::string serialized{st.getSerialized()};
    int length = serialized.size();
    std::ostringstream os;
    os<<std::setw(headerLength)<<std::hex<<length;
    std::string header {os.str()};
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(header));
    buffers.push_back(boost::asio::buffer(serialized));

    boost::asio::async_write(u->socket,buffers,[](const boost::system::error_code& error, std::size_t)
    {
    });
}

void User::pingMe()
{
    std::ostringstream os;
    os<<std::setw(headerLength)<<"ping";
    boost::asio::async_write(socket,boost::asio::buffer(os.str()),[](const boost::system::error_code& error, std::size_t){});
    alive = false;
}
