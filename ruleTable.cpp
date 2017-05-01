
#include <stdlib.h>
#include <string>
#include <iostream>
#include <regex>
#include <pqxx/pqxx>


#include <structuredLogger.h>

#include "ruleTable.h"
/// \version $Revision: $
/// \date $Date: $
/// \file genericTable.cpp
/// \class genericTable

///\fn ruleTable::ruleTable
///\arg none
///<<  constructor no args
///<< connect to rule table  to fill in the fields
///
/**
**  fields
**      id  big serial
**      rule_name var char
**      regex  var char
**      rule_set foreign key
**      severity  enum notification warn error fatal debug  (db type)
**/

ruleTable::ruleTable() :  genericTable()
{
   m_tableName = "rule"; //ctor
   setCusorCallBack(std::bind(&ruleTable::readCusorHandler,this,std::placeholders::_1));
}
///\fn ruleTable::ruleTable
///\arg std::string connection
///<< dbname=[databaseName] user=[database user] password=[md5 encypted password] hostadd=[ip or localhost] port=[5432]
///<<  constructor string for database connection
///<< connect to rule table  to fill in the fields
///

ruleTable::ruleTable(std::string &dbconnection) : genericTable(dbconnection)
{
    // file in field and type string vectors
    std::string tableName("rule");
    m_tableName = "rule";
    retriveTableMetadata(tableName);
    mp_readCusorHandler = std::bind(&ruleTable::readCusorHandler,this,std::placeholders::_1);
}

ruleTable::~ruleTable()
{
    //dtor
}
///\fn readCusorHandler
///\arg void * point to the instance
///\arg const pqxx::result::tuple &row
///\brief static funtion in the class ruleTable to handle the call back when walking through
///\brief all the rules
///
void ruleTable::readCusorHandler( const pqxx::result &row)
//void ruleTable::readCusorHandler(void *classInstPtr, const pqxx::result::tuple &row)
{

/** debug
     for (pqxx::result::tuple::iterator it=row.begin();
        it != row.end() ; it++)
            std::cout << it.c_str() << " ";
        std::cout << std::endl;
*/
}

///\fn ruleTable::readCusor
///\arg pqxx::conection * dbc
///\arg std::vector<std::string> fieldsToRead ,
///\arg std::condtion
///\brief read the specified fields from a table and rows selected by the condition supplied
///\brief could also be the result of a union ???
///\brief for reads of extremly large return sets  (how much memory do you have ???)
///\brief virtual
///
pqxx::result ruleTable::readCusor(pqxx::connection *dbc ,
                                    std::vector<std::string> &fieldsToRead,
                                    std::string &condition,
                                    void * instancePtr,
                                    long cursorSize)
{
    pqxx::result r;
    // get record count before we begin
    size_t countRecords=getRecordCount(condition);

    if(dbc == nullptr)
    {
        dbc = mp_dbc;
    }
    std::string sqlquery= "SELECT ";
    for (std::vector<std::string>::iterator it =fieldsToRead.begin() ;
        it != fieldsToRead.end(); it++)
            sqlquery.append(*it + ",");
    sqlquery.erase(sqlquery.length() - 1,1) ;  // remove extra comma
    sqlquery.append(" FROM " + m_tableName + " ");
    if (!condition.empty()) {
        sqlquery.append(condition);
    }
    sqlquery.append(";");

    try {
        pqxx::work W(*dbc);
        pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned>
                cursor(W, sqlquery, "mycursor", true);
        /* Assume you know total number of records returned */
        /* todo detrmine the total number of records */
        /* see getRecordCount */
        /** Walk through all the records/rows one at a time.
        *   This could be changed to the number of rows the handler wants
        **/
        for (size_t idx = 0; idx < countRecords; ) {

            pqxx::result r = cursor.retrieve(idx,cursorSize);
            /**
            * pass just tuple
            for (size_t rownum = 0; rownum <  r.size(); rownum++) {
                const pqxx::result::tuple row = r[rownum];
                if (mp_readCusorHandler != nullptr) {
                    (*mp_readCusorHandler)(instancePtr,row);
                }
                //used mapping of data type to tuple  for converting to binary data types
                //vid1 = row[0].as<int>();
                //vid2 = row[1].as<int>();
                //vid3 = row[2].as<int>();

            }
            **/
            /**
            * pass set of rows
            */
            if (mp_readCusorHandler != nullptr && r.size() > 0 ) {
                    ///(*mp_readCusorHandler)(instancePtr, r);
                    mp_readCusorHandler(r);
                }
            idx += cursorSize;
        }
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch (const std::exception &e) {
        dumpException(e);
    }

    return r;
}

///\fn ruleTable::readFields
///\arg pqxx::conection * dbc
///\arg std::vector<std::string> fieldsToRead ,
///\arg std::string condtiion
///\brief read the specified fields from a table and rows selected by the condition supplied
///\brief could also be the result of a union ???
///\brief  virtual
pqxx::result ruleTable::readFields(pqxx::connection *dbc ,
                                    std::vector<std::string> &fieldsToRead,
                                    std::string &condition)
{
    pqxx::result r;
    try
    {
        pqxx::connection  *wdbc = (dbc != nullptr) ? dbc : mp_dbc;
        pqxx::work txn (*wdbc);
        std::vector<std::string> columnsToRead = (! fieldsToRead.empty() ) ? fieldsToRead : m_fields;
        // reads full table no conditions
        //pqxx::tablereader trd (txn,m_tableName,columnsToRead.begin(),columnsToRead.end());

        // or
        std::string query("SELECT ");
        //
        // walk thru vector to see what we want
        //
        std::vector<std::string>::iterator sit = columnsToRead.begin();
        while (sit != columnsToRead.end() )
        {
            query.append(*sit);
            sit++;
            if (sit != columnsToRead.end())
            {
                query.append(" , ");
            }
        }
        if ( ! condition.empty())
            query.append( " FROM " + m_tableName + " WHERE " + condition );
        else
            query.append( " FROM " + m_tableName);
        /* todo   get the number of records */

        r = txn.exec(query);
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e,condition);
        // option to rethrow
        //rethrow(e);
    }

    return r;
}
///\fn ruleTable::getRecordCount
///\arg [in] std::string condition
///\return [out] long
///\brief get the record count  using a default if no condition is set
///
long  ruleTable::getRecordCount(std::string & condition)
{
    std::string defaultSql ("SELECT count(*) from " + m_tableName + " ");
    if (!condition.empty() ) {
        defaultSql.append("WHERE ");
        defaultSql.append(condition );
    }
    defaultSql.append(" ;");
    return genericTable::getRecordCount(defaultSql);
}

