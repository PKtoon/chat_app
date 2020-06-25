#include "./client.hpp"
#include <iostream>

void Client::connect(std::string host, std::string port, std::function<void (asio::error_code)> callBack)
{
    net.connect(host,port,[this,callBack](asio::error_code error)
    {
        if(error != asio::error::operation_aborted)
        {
            callBack(error);
        }
    });
}

void Client::disconnect()
{
    net.disconnect();
}

asio::ip::tcp::socket *Client::getSocket()
{
    return net.getSocket();
}

void Client::newSocket(asio::io_context &io)
{
    net.newConnection(io);
}

void Client::init(std::string name, std::string password,std::function<void (asio::error_code, Stream data)> callBack)
{
    name_ = name;
    Stream initPack;
    initPack.head = Header::INIT;
    initPack.sender = name;
    initPack.data1 = password;
    net.send(initPack,[this,callBack](asio::error_code error, std::size_t sent)
    {
        if(error)
        {
            if(error != asio::error::operation_aborted)
                callBack(error, Stream());
        }
        else
        {
            net.receive([this,callBack](Stream initAck, asio::error_code error, std::size_t read)
            {
                if (error)
                {
                    if(error != asio::error::operation_aborted)
                    {
                        std::cerr<<"Client::init()::net.receive(): "<<error.message()<<std::endl;
                        callBack(error, Stream());
                    }
                }
                else
                    callBack(error, initAck);
            }
            );
        }
    }
    );
}

void Client::reader(std::function<void (Stream data,asio::error_code,std::size_t)> callBack)
{
    net.receive([this,callBack](Stream data, asio::error_code error, std::size_t read)
    {
        if(error)
        {
//            if(error != asio::error::operation_aborted)
//                callBack(data,error,read);
        }
        else
        {
            processData(data);
        }
        if(error != asio::error::operation_aborted)
            callBack(data,error,read);
    }
    );
}

//same problem as described in server. one failing message will stall others. well here the recipient is only server, so if one message have network error all other messages will also.
void Client::writer()
{
    isWriting = true;
    if(!writeQueue.empty())
    {
        net.send(*writeQueue.begin(),[this](asio::error_code error, std::size_t sent)
        {
            if(error)
            {

            }
            else
            {
                writeQueue.pop_front();
            }
            if(error != asio::error::operation_aborted)
                writer();
        });
    }
    else
        isWriting = false;
}

void Client::queueMessage(Stream data)
{
    writeQueue.push_back(data);
    if(!isWriting)
        writer();
}

void Client::processData(Stream data)
{
    switch(data.head)
    {
        case Header::PING:
            ping();
            break;
        case Header::MESSAGE:
            processMessage(data);
            break;
        default:
            break;
    }
}

void Client::processMessage(Stream data)
{
    bool result;
    if(!getContact(data.sender,result))
    {
        std::cerr<<db.getError();
        return;
    }
    if(!result)
    {
        if(!insertContact(data.sender))
        {
            std::cerr<<db.getError();
            return;
        }
    }
    if(!insertMessage(data.sender,data.sender,data.data1))
    {
        std::cerr<<db.getError();
        return;
    }
}

void Client::ping()
{
    Stream ping;
    ping.head = Header::PING;
    queueMessage(ping);
}

std::string Client::getDBError()
{
    return db.getError();
}


//db functions

void Client::initDB()
{
    std::string query {"SELECT COUNT(name) FROM sqlite_master WHERE name=\"user\";"};
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
            {
                std::string colName{columnName[0]};
                std::string colData{columnData[0]};
                if(colName == "COUNT(name)" && colData == "0")
                {
                    if(!db.queryExec("CREATE TABLE user (name TEXT PRIMARY KEY);"))
                        std::cerr<<db.getError();
                }
                return 0;
            };
        if(!db.queryExec(query,func))
            std::cerr<<db.getError();
    }
    query ="SELECT COUNT(name) FROM sqlite_master WHERE name=\"contacts\";";
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName{columnName[0]};
            std::string colData{columnData[0]};
            if(colName == "COUNT(name)" && colData == "0")
            {
                if(!db.queryExec("CREATE TABLE contacts (name TEXT PRIMARY KEY);"))
                    std::cerr<<db.getError();
            }
                return 0;
        };
        if(!db.queryExec(query,func))
            std::cerr<<db.getError();
    }
    query ="SELECT COUNT(name) FROM sqlite_master WHERE name=\"messages\";";
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
            {
                std::string colName{columnName[0]};
                std::string colData{columnData[0]};
                if(colName == "COUNT(name)" && colData == "0")
                {
                    if(!db.queryExec("CREATE TABLE messages (id INTEGER PRIMARY KEY AUTOINCREMENT, contact TEXT, sender TEXT, message TEXT, time TEXT);"))
                        std::cerr<<db.getError();
                }
                    return 0;
            };
        if(!db.queryExec(query,func))
            std::cerr<<db.getError();
    }
}
bool Client::getContact(std::string name, bool& result)
{
    std::string query {"SELECT name FROM contacts WHERE name = \""+name+"\";"};

    auto func = [&name,&result](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName {columnName[0]};            //need to store in a string first as columnName[0]=="name" always fails
            std::string colData {columnData[0]};
            if(colName=="name" && colData == name)
                result = true;
            return 0;
        };

    return db.queryExec(query,func);
}
bool Client::getContactList(std::vector<std::string>& list)
{
    std::string query {"SELECT name FROM contacts;"};

    auto func = [&list](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName {columnName[0]};            //need to store in a string first as columnName[0]=="name" always fails
            if(colName=="name")
                list.push_back(std::string(columnData[0]));

            return 0;
        };

    return db.queryExec(query,func);
}

bool Client::getMessages(std::string contact, std::vector<std::pair<std::string,std::string>>& msg)
{
    std::string query = "SELECT sender,message FROM messages WHERE contact=\""+contact+"\";";

    auto func = [&msg](int numOfColumns,char **columnData, char **columnName)->int
    {
        msg.push_back(std::pair<std::string,std::string>(std::string(columnData[0]),std::string(columnData[1])));
        return 0;
    };

    return db.queryExec(query,func);
}

bool Client::insertContact(std::string name)
{
    std::string query{"INSERT INTO contacts VALUES (\""+name+"\");"};
    return db.queryExec(query);
}

bool Client::insertMessage(std::string contact, std::string sender, std::string msg)
{
    std::string query {"INSERT INTO messages (contact,sender,message,time) VALUES (\""+contact+"\", \""+sender+"\", \""+msg+"\",datetime(\"now\"));"};
    return db.queryExec(query);
}
