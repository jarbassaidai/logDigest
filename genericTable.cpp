#include <stdlib.h>
#include <string>
#include <iostream>
#include <regex>
#include <pqxx/except.hxx>
#include <pqxx/pqxx>

#include "../structuredLogger/structuredLogger.h"
#include "genericTable.h"
/// \version $Revision: $
/// \date $Date: $
/// \file genericTable.cpp
/// \class genericTable

///\fn genericTable::genericTable
///\arg none
///\brief  constructor no args
genericTable::genericTable()
{
    mp_dbc = nullptr;
    mp_wdbc = nullptr;
    m_doNotDisconnect = false;
    mp_readCusorHandler = nullptr;
    m_elogName = "genericTable";
}
//
// connectionstring dbname=LefthandOSKnowledge user=postgres password=xxxx hostaddr=localhost port=5432
///\fn genericTable::genericTable
///\arg string
///\brief  constructor with a database connection string of the format
///\brief <dbname=[databaseName] user=[database user] password=[md5 encypted password] hostadd=[ip or localhost] port=[5432]
/// \brief any thing in [] should be replaced by the actual values
 genericTable::genericTable(std::string &dbConnectionString) {
    try
    {
        mp_wdbc = nullptr;
        m_doNotDisconnect=false;
        mp_readCusorHandler = nullptr;


        mp_dbc = new pqxx::connection(dbConnectionString);
       connected();
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);
        mp_dbc = nullptr;

    }
    catch(const std::exception &e)
    {
        dumpException(e);
        mp_dbc = nullptr;
    }

 }
///\fn genericTable::genericTable
///\arg std::vector<std::string>
///\arg std::string dbconnection ,
///\arg std::string table
///<< vector of strings that define the field names of the table that you want to construct or work on
///<< constructor with a database connection string of the format
///<< dbname=[databaseName] user=[database user] password=[md5 encypted password] hostadd=[ip or localhost] port=[5432]
///<< any thing in [] should be replaced by the actual values
///<< table name that the fields go with

genericTable::genericTable(std::vector<std::string> &fieldNames,std::string &dbConnectionString,std::string &table)
{
    //ctor
    m_fields = fieldNames;
    m_tableName = table;
    m_doNotDisconnect = false;
    mp_wdbc = nullptr;
    mp_readCusorHandler = nullptr;


    try
    {
        mp_dbc = new pqxx::connection(dbConnectionString);
    }
   catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e);
    }


}
///\fn genericTable::genericTable
///\arg pqxx::connection *dbc
///\brief  constructor for existing database connection
genericTable::genericTable(pqxx::connection *dbc)
{
    mp_dbc = nullptr;
    mp_wdbc = dbc;
    m_doNotDisconnect = true;
    mp_readCusorHandler = nullptr;
    connected(true);

}
///\fn genericTable::genericTable
///\arg std::string connectionString
///\brief  drop any existing database connections and connect to the passed in argument

bool genericTable::connectDatabase(std::string & connectionString)
{
    bool v=true;
    try
    {
        if (mp_wdbc != mp_dbc)
        {
            if (mp_wdbc->is_open()) {
                mp_wdbc->disconnect();
                delete mp_wdbc;
                mp_wdbc=nullptr;
            }
        }
        if (connected() ) {
           mp_dbc->disconnect();
           delete mp_dbc;
        }

        mp_wdbc = nullptr;
        m_doNotDisconnect=false;
        mp_readCusorHandler = nullptr;
        mp_dbc = new pqxx::connection(connectionString);
        connected();
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);
        mp_dbc = nullptr;
        v=false;

    }
    catch(const std::exception &e)
    {
        dumpException(e);
        mp_dbc = nullptr;
        v=false;
    }
    return v;
}

genericTable::~genericTable()
{
    //dtor
    if (m_doNotDisconnect)
    {
        return;
    }
    else {
        if (mp_dbc == mp_wdbc) {
            mp_wdbc = nullptr;
        }
        if (mp_dbc != nullptr) {

            if (mp_dbc->is_open())
            {
                mp_dbc->disconnect();
            }

            delete mp_dbc;
            mp_dbc = nullptr;
        }
        if (mp_wdbc != nullptr ){
            mp_wdbc->disconnect();
            delete mp_wdbc;
            mp_wdbc = nullptr;
        }

        /// clean up storage
        ///
        freeActualData();
        m_fields.clear();
        m_fieldData.clear();
        m_fieldDataTypes.clear();

    }
}
///\fn freeActualData
///\arg none
///\return void
///\brief free m_actualData by type
void genericTable::freeActualData()
{
    std::string valueType;
    for (size_t pos = 0; pos < m_fieldDataTypes.size(); pos++)
    {
        valueType = m_fieldDataTypes[pos];
        if (valueType.compare("integer") == 0 || valueType.compare("bigint") ==0
                || valueType.compare("long") == 0 )
        {
            if (mp_actualData[pos] != nullptr)
                delete (int64_t *) mp_actualData[pos];
        }
        else if (valueType.compare("date") == 0 || valueType.compare("date interval") == 0
                 || valueType.compare("text") == 0 || valueType.compare("character varying") == 0)
        {
            if (mp_actualData[pos] != nullptr)
                free (mp_actualData[pos]);
        }
        else if (valueType.find("ARRAY") != std::string::npos)
        {
            // todo what to do with ARRAY
            // find subtype and fill the string with the correct syntax
            if (mp_actualData[pos] != nullptr)
                free (mp_actualData[pos]);

        }
        else if (valueType.compare("USER-DEFINED") == 0)
        {
            // todo what to do with USER-DEFINED
            if (mp_actualData[pos] != nullptr)
                free (mp_actualData[pos]);

        }
        else
        {
            // log unknown data types

            t_spSL elog = structuredLogger::getInstance(m_elogName.c_str());
            elog->debugL(debug,valueType.c_str());

        }
    } /// end for field data types
}

///\fn genericTable::createTable
///\arg std::string tableName
///\arg std::vector<std::string> fieldNames
///\arg std::vector<std::string> fieldTypes
///\brief Name of the table to create
///\brief vector of strings with the field names
///\brief vector of strings with the matching postgres data types for the fields


bool genericTable::createTable(std::string &tableName,std::vector<std::string> &fieldNames,std::vector<std::string> &fieldTypes) {
    bool r = true;

    std::string query = "create Table \"" + tableName + "\" (";
    std::vector<std::string>::iterator ifn = fieldNames.begin();
    std::vector<std::string>::iterator ifd = fieldTypes.begin();

    for (ifd,ifn; ifn != fieldNames.end(); ifn++, ifd++) {
        query += " \"" + *ifn + "\" " + *ifd + "," ;
    }
    query.erase(query.size() -1,1);
    query += ");";

    try {
        pqxx::result x = nontransQuery(nullptr,query);
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);
        r  = false;
    }
    catch(const std::exception &e)
    {
        dumpException(e);
        r = false;
    }

    return r;



}
///\fn genericTable::retriveTableMetadata
///\arg std::string tableName
///\brief retrived data is placed in the instances fields and data types string vectors
//
// all tables all columns data type
///      dbname         table
//select table_catalog, table_name,column_name, data_type from information_schema.columns where table_schema='public';
//  and table_name= 'volume'  specific table
//
void genericTable::retriveTableMetadata(std::string &tableName)
{

    std::string query("SELECT  column_name, data_type FROM information_schema.columns WHERE table_schema='public' AND table_name='");
    query.append(tableName);
    query.append("'");
    pqxx::result r;

    try
    {

        pqxx::connection  *wdbc =  mp_dbc;
        connected(true);
        pqxx::work txn (*wdbc);
        r = txn.exec(query);
        // go through the results
        // build query and set sting vectors
        //
        for (pqxx::result::const_iterator row = r.begin();
        row != r.end(); row++)
        {
            bool fname = true;    // toggle whether it is the field name or the field type
            for (pqxx::result::tuple::const_iterator field = row->begin();
            field != row->end();
            ++field)
            {
                if (fname)
                {
                    m_fields.push_back(field.c_str());
                    fname = false;
                }
                else  {
                    m_fieldDataTypes.push_back(field.c_str());
                    mp_actualData.push_back((void *) nullptr);  // place saver nullptr
                    fname = true;
                }
                //elog()->debugL(debug,field->c_str() );

            }

        }
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e);
        // how to get a stack dump will need to use boost exceptions
        //scopes stack:\n"
        //<< *boost::get_error_info< logging::current_scope_info >(e);

        // option to rethrow
        //rethrow(e);
    }

}
///\fn genericTable::storeValue
///\arg pos integer array postion for vector collected from datatype fieldname [in]
///\arg void *data binary data to be put into objects data vector [in]
///\return void
///\pre table must be selected before calling
///\brief This may not be used in current form as not all data types are accounted for and postgresql will want everything as a char string
//
void genericTable::storeValue(int pos, void *data )
{

    if (m_fieldDataTypes.size() == 0){
        retriveTableMetadata(m_tableName);
        if (m_fieldDataTypes.size() == 0)
        {
            std::string ex="no field data types for storeValue to use";
            throw new pqxx::sql_error(ex);
        }else {
            initStoreValue();
        }
    }

    std::string valueType = m_fieldDataTypes[pos];
    std::string value;
    if (valueType.compare("integer") == 0 || valueType.compare("bigint") ==0 ){
        int64_t *i = (int64_t *) data;
        mp_actualData[pos] = new int64_t(*i);
        m_fieldData[pos] = std::to_string(*i);
    }else if (valueType.compare("long") == 0){
        int64_t *i = (int64_t *) data;
        mp_actualData[pos] = new int64_t(*i);
        m_fieldData[pos] = std::to_string(*i);

    }
    else if (valueType.compare("date") == 0)
    {
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]= sd.c_str();

    }
    else if (valueType.compare("date interval") == 0)
    {
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]=sd.c_str();

    }else if (valueType.compare("text") == 0){
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]=sd.c_str();

    }else if (valueType.compare("character varying") == 0){
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]=sd.c_str();

    }else if (valueType.find("ARRAY") != std::string::npos){
        // todo what to do with ARRAY
        // find subtype and fill the string with the correct syntax
        m_fieldData[pos] = array2string(pos,data);
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());

    }else if (valueType.compare("USER-DEFINED") == 0){
        // todo what to do with USER-DEFINED
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]=sd.c_str();

    }else {
        // log unknown data types

        t_spSL elog = structuredLogger::getInstance(m_elogName.c_str());
        elog->debugL(debug,valueType.c_str());

    }

}
///\fn initStoreValue
///\arg [in] none
///\arg [out] none
///\brief init m_fieldData to strings called from storeValue, protected method
void genericTable::initStoreValue() {
/**
    ** fill string vector with place holders
    **/
    std::string placeholder("place holder");
    for (size_t idx = 0;  idx < m_fieldDataTypes.size(); idx++) {
        m_fieldData.push_back(placeholder.c_str());
    }
}


///\fn genericTable::readFields
///\arg pqxx::conection * dbc
///\arg std::vector<std::string> fieldsToRead ,
///\arg std::string condtiion
///\brief read the specified fields from a table and rows selected by the condition supplied
///\brief could also be the result of a union ???
pqxx::result genericTable::readFields(pqxx::connection *dbc , std::vector<std::string> &fieldsToRead, std::string &condition)
{
    pqxx::result r;
    try
    {
        pqxx::connection  *wdbc = (dbc != NULL) ? dbc : mp_dbc;
        connected(true);
        pqxx::work txn (*wdbc);
        std::vector<std::string> columnsToRead = (! fieldsToRead.empty() ) ? fieldsToRead : m_fields;
        /** reads full table no conditions
        ** std::vector<std::string> R;
        **pqxx::tablereader trd (txn,m_tableName,[iterator-begin],[iterator-end]);
        **for (int n=0; (trd >> R); ++n)
        **    {
        **      // Keep the first row for later consistency check
        **      if (n == 0) First = R;
        **
        **      cout << n << ":\t";
        **      for (vector<string>::const_iterator i = R.begin(); i != R.end(); ++i)
        **            cout << *i << '\t';
        **        cout << endl;
        **        R.clear();
        **       }
        **
        **
        **/

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
        query.append( " FROM " + m_tableName );
        if (!condition.empty()) {
            query.append(" WHERE " + condition );
        }
        query.append(" ; ");
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
///\fn genericTable::execQuery
///\arg pqxx::conection * dbc
///\arg std::string query
///\brief execute the provided query
///\brief could also be the result of a union ???
pqxx::result genericTable::execQuery(pqxx::connection *dbc , std::string &query)
{
    pqxx::result r;
    try
    {
        mp_wdbc = (dbc != nullptr) ? dbc : mp_dbc;
        connected(true);
        pqxx::work txn (*mp_wdbc);

        r = txn.exec(query);
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e);
        // option to rethrow
        //rethrow(e);
    }

    return r;
}
///\fn genericTable::readCusor
///\arg pqxx::conection * dbc
///\arg std::vector<std::string> fieldsToRead ,
///\arg std::string condtiion
///\arg long records per cusor
///\brief read the specified fields from a table and rows selected by the condition supplied
///\brief could also be the result of a union ???
///\brief for reads of extremly large return sets  (how much memory do you have ???)
///
pqxx::result genericTable::readCusor(pqxx::connection *dbc ,
    std::vector<std::string> &fieldsToRead,
    std::string &condition, void * instancePtr,
    long cursorSize)
{
    pqxx::result r;
    size_t countRecords=0;

try {

        pqxx::work W(*dbc);
        pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned>
                cursor(W, condition, "mycursor", true);
        /* Assume you know total number of records returned */
        /* todo detrmine the total number of records */
        /* see getRecordCount */
        /**
        * Need to set what the number of rows to send to the callback function.
        **/
        for (size_t idx = 0; idx < countRecords; idx += cursorSize) {
            /* Fetch 100,000 records at a time */
            r = cursor.retrieve(idx, idx + cursorSize);
            if (mp_readCusorHandler != nullptr) {
                    mp_readCusorHandler(r);
                    ///(*mp_readCusorHandler)(r);
            }else { /** THIS NEEDS TO BE LOOKED AT **/
                for (size_t rownum = 0; rownum <  r.size(); rownum++) {
                    const pqxx::result::tuple row = r[rownum];
                        if (mp_readCusorHandler != nullptr) {
                            (*mp_readCusorHandlerTuple)(row);
                        }
                }
            }
                //used mapping of data type to tuple  for converting to binary data types
                //vid1 = row[0].as<int>();
                //vid2 = row[1].as<int>();
                //vid3 = row[2].as<int>();

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
///\fn genericTable::write
///\arg pqxx::connection/ null_ptr [in]
///\arg std::vector<std::string> fieldNamesToWrite [in]
///\arg std::vector<std::string> fieldData [in]
///\arg std::string condition
///\return long primary key
///\pre table must be selected before calling if a condtion is not supplied when calling
///\brief inserts provided fields into the already selected table returning the primarykey for the row

long genericTable::write(pqxx::connection *dbc ,
                        std::vector<std::string> &fieldNamesToWrite,
                        std::vector<std::string> &fieldData,
                        std::string &condition)
{
    if(dbc == nullptr)
    {
        dbc = mp_dbc;
    }

    std::string insertStr("INSERT INTO " + m_tableName + " (");

    //
    // walk through vector to see what columns we are writing
    //
    std::vector<std::string>::iterator sit = fieldNamesToWrite.begin();
    std::vector<std::string>::iterator vit = fieldData.begin();
    std::string values(") VALUES (");
    pqxx::result r;
    //
    // this throws exception
    try
    {
        pqxx::work txn (*dbc);
        while (sit != fieldNamesToWrite.end() && vit != fieldData.end() )
        {
            insertStr.append( *sit);
            values.append(txn.quote(*vit));
            sit++;
            vit++;
            if (sit != fieldNamesToWrite.end() && vit != fieldData.end())
            {
                insertStr.append(", ");
                values.append(", ");
            }
        }
        values.erase(values.length() - 1,1) ;  // remove extra comma
        values.append( "\'  ) ");
        insertStr.append(values) ;

        if (! condition.empty()) {
              insertStr.append(condition) ;   // + condition ??  RETURNING primarykey
        }
        insertStr.append(" ;");

        r= txn.exec(insertStr.c_str());

        pqxx::tablewriter tw (txn,m_tableName,fieldNamesToWrite.begin(),fieldNamesToWrite.end());

        //tw.push_back(fieldData);
        // this throws exception
        tw.complete();
        txn.commit();
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e);

        // option to rethrow
        //rethrow(e);
    }


    long rv = (r.begin() != r.end() ) ? r.begin()["primarykey"].as<long>() : -1 ;
    return  rv;
}
///\fn genericTable::update
///\arg pqxx::connection/ null_ptr [in]
///\arg std::vector<std::string> fieldNamesToWrite [in]
///\arg std::vector<std::string> fieldData [in]
///\arg std::string condition
///\return long primary key
///\pre table must be selected before calling if a condtion is not supplied when calling
///\brief inserts provided fields into the already selected table returning the primarykey for the row

bool genericTable::update(pqxx::connection *dbc , std::vector<std::string> &fieldNamesToWrite, std::vector<std::string> &fieldData, std::string &condition)
{
    bool rval = true;
    if(dbc == nullptr)
    {
        dbc = mp_dbc;
    }
    std::string insertStr("UPDATE " + m_tableName + " SET ");

    //
    // walk through vector to see what columns we are writing
    //
    std::vector<std::string>::iterator sit = fieldNamesToWrite.begin();
    std::vector<std::string>::iterator vit = fieldData.begin();
    //std::string values(") VALUES (");
    pqxx::result r;
    //
    // this throws exception
    try
    {
        pqxx::work txn (*dbc);
        while (sit != fieldNamesToWrite.end() && vit != fieldData.end() )
        {
            insertStr.append( *sit);
            insertStr.append("=");
            insertStr.append(txn.quote(*vit));
            //values.append(txn.quote(*vit));
            sit++;
            vit++;
            // needs to be changed to set the values an run query not tablewriter
            // TODO ^^^^^^^^^^^^^^
            if (sit != fieldNamesToWrite.end() && vit != fieldData.end())
            {
                insertStr.append(", ");
                //values.append(", ");
            }
        }
        //insertStr.erase(insertStr.length() - 1,1) ;  // remove extra comma
        if (!condition.empty()) {
            insertStr.append(" WHERE " + condition);
        }

        insertStr.append( " ; "  );  // + condition ??
        //insertStr.append(values);
        r= txn.exec(insertStr.c_str());

        //pqxx::tablewriter tw (txn,m_tableName,fieldNamesToWrite.begin(),fieldNamesToWrite.end());

        //tw.push_back(fieldData);
        // this throws exception
        //tw.complete();
        txn.commit();
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e);
        //rethrow(e);
    }


    return  rval;
}
///\fn genericTable::deleteR
///\arg std::vector<std::string> condition [in]
///\return bool
///\pre table must be selected before calling
///\brief removes n number of records matching the condition supplied by caller
///
bool genericTable::deleteR(std::string &condition) {

    bool rval = true;
    std::string query("DELETE FROM ");
    query.append(m_tableName);
    query.append(" WHERE " + condition + " ;");
    pqxx::result r;
    try
    {
        connected(true);
        pqxx::work txn (*mp_dbc);
        r= txn.exec(query);
        txn.commit();
        if (!r.empty()){

          std::string msg =   r.begin()[0].as<std::string>();
          std::cerr << msg << std::endl;
        }
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



    return rval;
}
///\fn genericTable::getRecordCount
///\arg std:string condition [in]
///\return int
///
///\brief returns the number of records for the condition
/*
* count of records that match a condition from differnt tables:
*    select count(*) as cnt From rule r where EXISTS( SELECT 1 FROM rule_set rs WHERE r.rule_set= rs.id);
* count of records in a table
*    select count(*) FROM RULE_SET;
* count by some condition
*   select id, count(id) From rule_set group by  id having count(id) = 1;
*/
long genericTable::getRecordCount(std::string &condition)
{
    long count=0;
    pqxx::result r;
    try
    {

        r = execQuery(nullptr,condition);
        count =r.begin()[0].as<long>();
        // get number of records to see if we want to open a cusor
        //SELECT week, count(*) FROM games WHERE week = (SELECT max(week) FROM games) GROUP BY week;
        //std::iostringstream  q;
        //q << "SELECT count(*) as rec_ct FROM " << m_tableName << " tbl WHERE EXISTS (SELECT 1 FROM tablex where " << condition ;
        //q << "SELECT [field] count(*) FROM " << m_tableName << " = (
        // select  field, count(filed) from table where field meets condition
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


    return count;
}
///\fn writeStored()
///\arg  none
///\brief take the values that have been accumlated through storeValue and write/update them to
///
long genericTable::writeStored(pqxx::connection *dbc,std::vector<std::string> &fields )
{
    // create a vector string of the datafields since they may be holes
    //
    std::vector<std::string> data;
    std::vector<std::string>::iterator it = m_fieldData.begin();
    std::string c("place holder");
    for (it; it != m_fieldData.end(); it++) {
        if (c.compare(*it) != 0) {
            data.push_back(*it);
        }
    }
    std::string blank;
    long rval = write(dbc,fields,data,blank);
    /**
    *   empty m_fieldData and m_p_actualdata
    **/
    emptydata();
    return rval;

}
void genericTable::emptydata() {
    while (m_fieldData.size() != 0)
        m_fieldData.pop_back();
    while (mp_actualData.size() !=0){
        mp_actualData.pop_back();
    }
}

///\fn dumpException
///\arg std::exception
///\brief standard call to send message to std::err and logging to errorL
///\brief always called from a catch exception
void genericTable::dumpException(const std::exception &e,std::string extra)
{
        std::cerr << e.what()  << std::endl;
        std::string msg (e.what());
        if (! extra.empty() ) {
            msg += extra;
        }
        t_spSL elog = structuredLogger::getInstance(m_elogName.c_str());
        elog->errorL(critical,(const char *)msg.c_str());
        // option to rethrow
        //rethrow(e);
}
///\fn dumpPaxxException
///\arg std::exception
///\brief standard call to send message to std::err and logging to errorL
///\brief always called from a catch exception
///\brief must be caught before standard exception since it is a supper class
void genericTable::dumpPqxxException(const pqxx::sql_error &e)
{
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "Query was: " << e.query() << std::endl;
        std::string msg (e.what());
        msg += "\n" + e.query() ;
        t_spSL elog = structuredLogger::getInstance(m_elogName.c_str());
        elog->errorL(critical,(const char *)msg.c_str() );
}
///\fn array2string
///\arg void *
///\brief convert an array of data discovered datatype to a string
///\brief not implemented yet
std::string genericTable::array2string(int pos, void *data)
{
std::string valueType = m_fieldDataTypes[pos];
    std::string value;
    if (valueType.find("integer") != std::string::npos || valueType.find("bigint") !=std::string::npos ){
        int64_t *i = (int64_t *) data;
        mp_actualData[pos] = new int64_t(*i);
        m_fieldData[pos] = std::to_string(*i);
    }else if (valueType.find("long") != std::string::npos){
        int64_t *i = (int64_t *) data;
        mp_actualData[pos] = new int64_t(*i);
        m_fieldData[pos] = std::to_string(*i);

    }
    else if (valueType.find("date") != std::string::npos)
    {
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]= sd;

    }
    else if (valueType.find("date interval") != std::string::npos)
    {
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]=sd;

    }else if (valueType.find("text") !=std::string::npos){
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]=sd;

    }else if (valueType.find("character varying") != std::string::npos){
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());
        m_fieldData[pos]=sd;

    }/*  ** avoid infinate loop
    else if (valueType.find("ARRAY") != std::string::npos){
        // todo what to do with ARRAY
        // find subtype and fill the string with the correct syntax
        // how deep do we go ??
        m_fieldData[pos] = array2string(pos,data);
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());

    } */
    else if (valueType.find("USER-DEFINED") != std::string::npos){
        // todo what to do with USER-DEFINED
        std::string sd((const char *)data);
        mp_actualData[pos] = calloc(1,sd.size() + 1);
        memccpy(mp_actualData[pos],sd.c_str(),1,sd.size());

    }else {
        // log unknown data types
        t_spSL elog =structuredLogger::getInstance(m_elogName.c_str());
        elog->debugL(debug,valueType.c_str());

    }
 return std::string("{{not},{implemented},{yet}}");
}
///\fn nontransQuery
///\arg pqxx::connection *dbc
///\arg std::string & query
///\brief  Non-transactional querys like create database and table;

pqxx::result genericTable::nontransQuery(pqxx::connection *dbc,std::string &query)
{

    pqxx::result r;
    try {
        if (dbc == nullptr){
            dbc = mp_dbc;
        }
        pqxx::nontransaction N(*dbc);
        r =N.exec(query);
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e,std::string());

        // option to rethrow
        //rethrow(e);
    }
    return r;
}

bool genericTable::connected(bool throwE)
{
    try
    {
        if (mp_dbc == nullptr){
           if (throwE){
                throw new pqxx::sql_error("Not conencted");
            }else{
                m_connected = false;
            }
        }else if (!mp_dbc->is_open()){
            t_spSL elog = structuredLogger::getInstance(m_elogName.c_str());
            elog->errorL(warning,"no longer connected");
            m_connected = false;
            if (throwE){
                throw new pqxx::sql_error("Not conencted");
            }
        }else {
            m_connected = true;
        }
    }
    catch (const pqxx::sql_error &e)
    {
        dumpPqxxException(e);

    }
    catch(const std::exception &e)
    {
        dumpException(e,std::string());
        t_spSL elog =structuredLogger::getInstance(m_elogName.c_str());
        elog->stack_trace("crash and burn on checking for valid pqxx::connection");
        // option to rethrow
        //rethrow(e);
    }

return m_connected;
}
