#ifndef GENERICTABLE_H
#define GENERICTABLE_H

#include <pqxx/pqxx>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <structuredLogger.h>
#include <functional>

/// \author $Author: $
/// \version $Revision: $
/// \date $Date: $
/// \file genericTable.h
/// \class genericTable
///< genericTable will read the database (postgres) for the table description filling in
///< a vector<std::string> with the fieldnames and a second vector with the data types
///< a third vector of void * is then used to store the values for the fields
///< This allows you to use this class for any table with standard data types.



class genericTable
{
    public:
        genericTable();
        genericTable(std::string &dbConnectionString);
        genericTable(std::vector<std::string> &fieldNames,std::string &dbConnectionString,std::string &tablename);
        genericTable(pqxx::connection *dbc);
        virtual ~genericTable();
        bool createTable(std::string &tablename,std::vector<std::string> &fieldNames,std::vector<std::string> &fieldTypes);
        void retriveTableMetadata(std::string &tablename);
        virtual pqxx::result readFields(pqxx::connection *dbc,
                std::vector<std::string> &,
                std::string &condition);
        pqxx::result execQuery(pqxx::connection *dbc, std::string &query);
        long write(pqxx::connection *dbc , std::vector<std::string> &fieldNamesToWrite, std::vector<std::string> &fieldData,std::string &condtion);
        bool update(pqxx::connection *dbc , std::vector<std::string> &fieldNamesToWrite, std::vector<std::string> &fieldData, std::string &condition);
        inline t_spSL elog() { return structuredLogger::getInstance(m_elogName.c_str());}
        inline std::vector<std::string> & getFieldsVector() { return m_fields; }
        inline std::vector<std::string> & getFieldsDataVector() { return m_fieldDataTypes; }
        inline void selectTable(std::string &table) {m_tableName=table;}
        bool deleteR(std::string &condition);
        bool deleteTable(std::string &tablename);
        bool connected(bool throwE=false) ;
        bool connectDatabase(std::string & conectionString);

        virtual pqxx::result readCusor(pqxx::connection *dbc ,
                std::vector<std::string> &fieldsToRead,
                std::string &condition,
                void * = nullptr,
                long = 100000);
        pqxx::result nontransQuery(pqxx::connection *dbc,std::string &query);
        inline void setLog(std::string &elname) { m_elogName = elname; }

        typedef  std::function<void(const pqxx::result &)> t_cbReadCusor;

    protected:

        pqxx::connection *mp_dbc;
        pqxx::connection *mp_wdbc;  // passed in someone elses pointer
        std::vector<std::string> m_fields;
        std::vector<std::string> m_fieldDataTypes; // binary
        std::vector<std::string> m_fieldData;      // string
        std::vector<void *> mp_actualData;         // string, binary, int, long etc
        std::string m_tableName;
        bool m_connected;
        bool m_doNotDisconnect;
        void storeValue(int pos, void *data);
        virtual long writeStored(pqxx::connection *dbc,std::vector<std::string> &fields);
        long getRecordCount(std::string &condition);
        void dumpException(const std::exception &e,std::string extra=std::string());
        void dumpPqxxException(const pqxx::sql_error &e);
        std::string array2string(int,void *);
        void (*mp_readCusorHandlerTuple)(const pqxx::result::tuple &row) ;
        ///void (*mp_readCusorHandler) (const pqxx::result &rows);
        t_cbReadCusor mp_readCusorHandler;
        void emptydata();
        void initStoreValue();
        void freeActualData();

        std::string m_elogName;
    private:
};

#endif // GENERICTABLE_H
