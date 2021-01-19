#include "./client.hpp"
#include <iostream>

void Client::connect(std::string host, std::string port, std::function<void (asio::error_code)> callBack)
{
    net.connect(host,port,[this,callBack](asio::error_code error)
    {
        if(!error)
            isConnected = true;
        callBack(error);
    });
}

void Client::disconnect()
{
// TODO:      do something like this with new client
    if(isConnected)
    if(net.getConnMan())
    {
        Stream data;
        data.head = Header::socket_close;
        net.send(data,[this](asio::error_code error, std::size_t)
        {
            if(error != asio::error::operation_aborted)
            {
                net.disconnect();
                isConnected = false;
            }
        });
    }
//    also try to give enough time to disconnect to actually deliver message
//     net.disconnect();
}

asio::ip::tcp::socket* Client::getSocket()
{
    return net.getSocket();
}

void Client::newSocket()
{
    disconnect();
    net.newConnection(io_);
}

void Client::userAuthInit(std::string name, std::string password, Header head)
{
    if(name.empty() || password.empty()){
        std::cerr<<"Name or password is empty\n";
        return;
    }
    name_ = name;
    Stream initPack;
    initPack.head = head;
    initPack.sender = name_;
    initPack.data1 = password;
    queueMessage(initPack);
}

void Client::reader(std::function<void (Stream data,asio::error_code,std::size_t)> callBack)
{
    net.receive([this,callBack](Stream data, asio::error_code error, std::size_t read)
    {
        if(!error)
            processData(data);
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
            if(!error)
                writeQueue.pop_front();
            writer();
        });
    }
    else
        isWriting = false;
}

Client::Error Client::queueMessage(Stream data)
{
    switch(data.head){
        case Header::message:
            if(!insertMessage(data.receiver,data.sender,data.data1))
                return Error::db_error;
            break;
        default:
            break;
    }

    writeQueue.push_back(data);
    if(!isWriting)
        writer();
    return Error::ok;
}

void Client::processData(Stream data)
{
    switch(data.head)
    {
        case Header::ping:
            ping();
            break;
        case Header::message:
            processMessage(data);
            break;
        case Header::signin|Header::ack:
        case Header::signup|Header::ack:
            initDB();
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
    ping.head = Header::ping;
    queueMessage(ping);
}

void Client::runIOContext()
{
    io_.restart();
#ifndef NDEBUG
    std::clog<<"IO_Context started"<<std::endl;
#endif
    io_.run();
#ifndef NDEBUG
    std::clog<<"IO_Context stopped"<<std::endl;
#endif
}

std::string Client::getDBError()
{
    return db.getError();
}


//db functions

void Client::initDB()
{
    std::string query {"SELECT COUNT(name) FROM sqlite_master WHERE name=\"users\";"};
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
            {
                std::string colName{columnName[0]};
                std::string colData{columnData[0]};
                if(colName == "COUNT(name)" && colData == "0")
                {
                    if(!db.queryExec("CREATE TABLE users (name TEXT PRIMARY KEY);"))
                        std::cerr<<db.getError();
                }
                return 0;
            };
        if(!db.queryExec(query,func))
            std::cerr<<db.getError();
    }
    query ="SELECT COUNT(name) FROM sqlite_master WHERE name=\"contacts_"+name_+"\";";
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
        {
            std::string colName{columnName[0]};
            std::string colData{columnData[0]};
            if(colName == "COUNT(name)" && colData == "0")
            {
                if(!db.queryExec("CREATE TABLE contacts_"+name_+" (name TEXT PRIMARY KEY);"))
                    std::cerr<<db.getError();
            }
                return 0;
        };
        if(!db.queryExec(query,func))
            std::cerr<<db.getError();
    }
    query ="SELECT COUNT(name) FROM sqlite_master WHERE name=\"messages_"+name_+"\";";
    {
        auto func = [this](int numOfColumns, char **columnData, char **columnName)->int
            {
                std::string colName{columnName[0]};
                std::string colData{columnData[0]};
                if(colName == "COUNT(name)" && colData == "0")
                {
                    if(!db.queryExec("CREATE TABLE messages_"+name_+" (id INTEGER PRIMARY KEY AUTOINCREMENT, contact TEXT, sender TEXT, message TEXT, time TEXT);"))
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
    std::string query {"SELECT name FROM contacts_"+name_+" WHERE name = \""+name+"\";"};

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
    std::string query {"SELECT name FROM contacts_"+name_+";"};

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
    std::string query = "SELECT sender,message FROM messages_"+name_+" WHERE contact=\""+contact+"\";";

    auto func = [&msg](int numOfColumns,char **columnData, char **columnName)->int
    {
        msg.push_back(std::pair<std::string,std::string>(std::string(columnData[0]),std::string(columnData[1])));
        return 0;
    };

    return db.queryExec(query,func);
}

bool Client::insertContact(std::string name)
{
    std::string query{"INSERT INTO contacts_"+name_+" VALUES (\""+name+"\");"};
    return db.queryExec(query);
}

bool Client::insertMessage(std::string contact, std::string sender, std::string msg)
{
    std::string query {"INSERT INTO messages_"+name_+" (contact,sender,message,time) VALUES (\""+contact+"\", \""+sender+"\", \""+msg+"\",datetime(\"now\"));"};
    return db.queryExec(query);
}
