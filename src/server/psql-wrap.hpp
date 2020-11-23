#ifndef PSQLWRAP_HPP
#define PSQLWRAP_HPP

#include <pqxx/pqxx>

namespace pk
{

class PSQLdb
{
    pqxx::connection connection;
    
public:
    PSQLdb(std::string param) : connection{param} {}
    
    pqxx::result exec(std::string query)
    {
        pqxx::work work{connection};
        return work.exec(query);
    }
    
    pqxx::result execCommit(std::string query)
    {
        pqxx::work work{connection};
        pqxx::result r = work.exec(query);
        work.commit();
        return r;
    }
};

}
    

#endif
