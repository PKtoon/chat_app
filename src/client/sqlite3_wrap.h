#ifndef SQLITE3_WRAP_H
#define SQLITE3_WRAP_H

#include <string>
#include <functional>

#include "../../external/sqlite3/sqlite3.h"

class FunObj
{
    std::function<int(int,char**,char**)> func;
public:
    FunObj(std::function<int(int,char**,char**)> fun):func{fun} {}
    inline int operator()(int numOfColumns, char **columnData, char **columnName)
    {
        return func(numOfColumns, columnData, columnName);
    }
};

inline int dbCallback(void* arg, int numOfColumns, char **columnData, char **columnName)
{
    auto func = static_cast<FunObj*>(arg);
    
    return (*func)(numOfColumns,columnData,columnName);
}

class SQLite3DB
{
    sqlite3 *db;
    std::string name;
    std::string error;
    int ec = 0;

public:
    SQLite3DB(std::string n) : name{n}
    {
        ec = sqlite3_open(name.c_str(),&db);
        if(ec)
            error = "unable to open database";
    }

    ~SQLite3DB()
    {
        sqlite3_close(db);
    }

    sqlite3* get() { return db; }
    std::string getError() const { return error; }
    int getErrorCode() const { return ec; }

    inline bool queryExec(std::string query, int (*callback) (void* arg,int numOfColumns, char **columnData, char **columnName) = nullptr,void* callbackArg=nullptr);
    
    inline bool queryExec(std::string query,std::function<int(int,char**,char**)> func);
};

bool SQLite3DB::queryExec(std::string query, int (*callback) (void* arg,int numOfColumns, char **columnData, char **columnName),void* callbackArg)
{
    char* err = nullptr;
    
    ec = sqlite3_exec(db,query.c_str(),callback,callbackArg,&err);
    
    if(ec != SQLITE_OK)
    {
        if(err)
        {
            error = std::string(err);
            sqlite3_free(err);
        }
        return false;
    }
    return true;
}

bool SQLite3DB::queryExec(std::string query,std::function<int(int,char**,char**)> func)
{
    FunObj fun{func};
    
    return queryExec(query,dbCallback,&fun);
}

#endif // SQLITE3_WRAP_H
