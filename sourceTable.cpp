#include <stdlib.h>
#include <string>
#include <pqxx/pqxx>
#include "../structuredLogger/structuredLogger.h"
#include "sourceTable.h"

/**
** id big-serial
** source_spec var char
** collection_id int
** date_range  date_range
**SELECT  '[2012-03-28, 2012-04-02]'::daterange;
** host_id     foreign key
** starting_time time
** ending_time   time

**/
/// \version $Revision: $
/// \date $Date: $
/// \file sourceTable.cpp
/// \class sourceTable

///\fn sourceTable
///\arg none

///\arg pqxx::conection * dbc
///\arg std::string query
///\brief execute the provided query
///\brief could also be the result of a union ???
sourceTable::sourceTable()
{
    //ctor
}

sourceTable::sourceTable(pqxx::connection *dbc) : genericTable(dbc)
{
    std::string t("source");
    retriveTableMetadata(t);
}
sourceTable::sourceTable(std::string & db_connection  ) : genericTable(db_connection)
{
    std::string t("source");
    retriveTableMetadata(t);
}

sourceTable::~sourceTable()
{
    //dtor
}
///\fn init
///\arg none
///\brief load field names
void sourceTable::init()
{
    std::string t("source");
    retriveTableMetadata(t);


}
///\fn fillrecord
///\arg std::string source spec
///\arg int collection_id
///\arg std::string date_range
///\arg std::string start_time
///\arg std::string end_time
///\brief load data string vector, but do not save to table as we may not have all values
void sourceTable::fillrecord(std::string &src,
                            long collect_id,
                            std::string &date_range,
                            std::string &start_time,
                            std::string &end_time)
{
    if (! src.empty()) {
        storeValue((int)stenum::spec,(void *)src.c_str());
    }
    if (collect_id != 0) {
        storeValue((int)stenum::collection,(void*) &collect_id);
    }
    if (! start_time.empty()){
        storeValue((int)stenum::startT,(void*)start_time.c_str());
    }
    if (! date_range.empty()){
        storeValue((int)stenum::range,(void*)date_range.c_str());
    }
    if (! end_time.empty()) {
        storeValue((int)stenum::endT,(void*)end_time.c_str());
    }

}

bool sourceTable::store()
{

    std::string b;
    std::vector<std::string> fields{ m_fields[(int)stenum::spec],
                                     m_fields[(int)stenum::collection],
                                     m_fields[(int)stenum::range],
                                     m_fields[(int)stenum::startT],
                                     m_fields[(int)stenum::endT]
                                    };

    long prkey=genericTable::writeStored(mp_dbc,fields);
    storeValue((int)stenum::id,(void*)&prkey);
    return true;
}

