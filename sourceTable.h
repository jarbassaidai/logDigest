#ifndef SOURCETABLE_H
#define SOURCETABLE_H

#include "genericTable.h"


class sourceTable : public genericTable
{
    public:
        /** Default constructor */
        sourceTable();
        sourceTable(std::string & db_connection);
        sourceTable(pqxx::connection *dbc);
        /** Default destructor */
        virtual ~sourceTable();
        void fillrecord(std::string &source,
                        long collect_id,
                        std::string &date_range,
                        std::string &start_time,
                        std::string &end_time);
        bool store();
    protected:
    enum class stenum {id=0,spec,collection,range,host,startT,endT};
    void init();

    private:
};

#endif // SOURCETABLE_H
