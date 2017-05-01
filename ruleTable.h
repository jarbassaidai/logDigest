#ifndef RULETABLE_H
#define RULETABLE_H

#include "genericTable.h"
#include <functional>

/// \author $Author: $
/// \version $Revision: $
/// \date $Date: $
/// \file genericTable.h
/// \class ruleTable
///<< rule table has specialized searches of a ruleset so that all the rules within the rulesset can be found
///<<
///<< genericTable will read the database (postgres) for the table description filling in
///<< a vector<std::string> with the fieldnames and a second vector with the data types
///<< a third vector of void * is then used to store the values for the fields
///<< This allows you to use this class for any table with standard data types.

class ruleTable ;
///typedef  void (*t_cbReadCusor)(const pqxx::result &) ;

class ruleTable : public genericTable
{
    public:
        /** Default constructor */
        ruleTable();
        ruleTable(std::string &dbconnection)  ;
        virtual pqxx::result readFields(pqxx::connection *dbc,
                std::vector<std::string> &,
                std::string &condition);
        virtual pqxx::result readCusor(pqxx::connection *dbc ,
                std::vector<std::string> &fieldsToRead,
                std::string &condition,
                void * = nullptr,
                long cursorSize = 10000);
                void setCusorCallBack(t_cbReadCusor callback)  { mp_readCusorHandler = callback; }
        /** Default destructor */
        virtual ~ruleTable();

        //static void readCusorHandler( void * ClassInstPtr, const pqxx::result &row);
        void readCusorHandler(const pqxx::result &row);
        virtual long getRecordCount(std::string & condition);
    protected:

    private:
};

#endif // RULETABLE_H
