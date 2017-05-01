///#define BOOST_TEST_MODULE ruleTable
#include "testSetup.h"
#include "ruleTable.h"
#include "ruleset.h"
#include "fileread.h"

class rulesTest : public genericTable
{
public:
    rulesTest() {};
    ~rulesTest() {} ;

    void readCusorHandler( const pqxx::result &rows)
    {
        std::vector<std::string> fnames { "rule_name", "regex","rule_set","severity"};
        std::vector<std::string> data {"test_rule","(warning|error)","5","warning"};

        long idx =0;
        for (pqxx::result::const_iterator rit = rows.begin(); rit != rows.end(); rit++,idx++)
        {
            pqxx::result::tuple row = *rit;
            try
            {
                ///debug
                ///std::cout <<  " " << row["rule_name"].c_str()<< std::endl;


                BOOST_CHECK_EQUAL(data[0].compare(row["rule_name"].c_str()),0);
                BOOST_CHECK_EQUAL(data[1].compare(row["regex"].c_str()),0);
                BOOST_CHECK((row["rule_set"].as<long>()) <= 20000);
                BOOST_CHECK_EQUAL(data[3].compare(row["severity"].c_str()),0);
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << " " << __LINE__ << std::endl;
            }
        }
    }
};

BOOST_AUTO_TEST_SUITE(s_PgSql_RulesTable)

BOOST_AUTO_TEST_CASE(rt_create)
{
    testSetup su;
    ruleTable rt(su.connectionString());
    std::vector<std::string> fnames { "id","rule_name", "regex","rule_set","severity"};
    std::vector<std::string> dataTypes {"serial","character varying","character varying","character varying","character varying"};
    std::string condition;
    std::string table="rule";
    try {
        rt.createTable(table,fnames,dataTypes);

    }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        su.elog()->errorL(critical,msg.c_str());
    }


}
BOOST_AUTO_TEST_CASE(rt_readCursor)
{
    testSetup su;
    ruleTable rt(su.connectionString());
     std::string ctable(
    "CREATE TABLE \"rule\" \
    ( \
    \"id\" serial, \
    \"rule_name\" character varying, \
    \"regex\" character varying, \
    \"rule_set\" integer, \
    \"severity\" character varying, \
    CONSTRAINT ridpk PRIMARY KEY (id) \
    );" );

    std::vector<std::string> fnames { "rule_name", "regex","rule_set","severity"};
    std::vector<std::string> data {"test_rule","(warning|error)","5","warning"};
    std::string condition;
    try {
        pqxx::result r = rt.nontransQuery(nullptr,ctable);
        /** write 20000 records to see how we handle ths **/
        su.elog()->tokenL(notification,"start of 20000 record write");
        for (int  i =0 ;i < 20000; i++){
            data[2] = std::to_string(i);
            rt.write(nullptr,fnames,data,condition);
        }
        ///see if record count is at least 20000
        std::vector<std::string> cfields ={"count(*)"};
        r = rt.readFields(nullptr,cfields,condition);
        if (r.empty()){
            BOOST_CHECK_MESSAGE(error,"FAILED TO COLLECT NUMBER OF COLUMNS");
        } else {
            BOOST_CHECK_GE (r.at(0).at(0).as<long>(),20000);
        }
        rulesTest * rTest = new rulesTest();
        rt.setCusorCallBack(std::bind(&rulesTest::readCusorHandler,rTest,std::placeholders::_1));
        su.elog()->tokenL(notification,"end of 20000 record write start of record read");
        /**
        * reading 10000 records
        */
        r=rt.readCusor(nullptr,fnames,condition);
        su.elog()->tokenL(notification,"end of 20000 record read/process");
        /// size is zero since cursor was emptied
        BOOST_CHECK_EQUAL(r.size(), 0);
        delete rTest;
    }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        su.elog()->errorL(critical,msg.c_str());
    }

}
/**
*
*/


BOOST_AUTO_TEST_CASE(rt_getRecordCount)
{
    testSetup su;
    ruleTable rt(su.connectionString());
     std::string ctable(
    "CREATE TABLE \"rule\" \
    ( \
    \"id\" serial, \
    \"rule_name\" character varying, \
    \"regex\" character varying, \
    \"rule_set\" integer, \
    \"severity\" character varying, \
    CONSTRAINT ridpk PRIMARY KEY (id) \
    );" );

    std::vector<std::string> fnames { "rule_name", "regex","rule_set","severity"};
    std::vector<std::string> data {"test_rule","(warning|error)","5","warning"};
    std::string condition;
    try {
        pqxx::result r = rt.nontransQuery(nullptr,ctable);
        /** write 20000 records to see how we handle ths **/
        su.elog()->tokenL(notification,"start of 20000 record write");
        for (int  i =0 ;i < 20000; i++){
            data[2] = std::to_string(i);
            rt.write(nullptr,fnames,data,condition);
        }
        std::string blank;
        BOOST_CHECK_EQUAL(rt.getRecordCount(blank),20000);
     }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        su.elog()->errorL(critical,msg.c_str());
    }

}
BOOST_AUTO_TEST_SUITE_END()
