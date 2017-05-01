#include "testSetup.h"


testSetup::testSetup()
{
   signal(SIGSEGV,&testSetup::signalCatch);

   t_spSL elogSU = structuredLogger::getInstance(ELOGNAME,(t_loggingStyle) (allInfoFile| tokenMsg | sepErrorFile | sepDebugFile));

   m_connectString ="user=postgres host=192.168.81.194 password=m0r3p0w3r";
       genericTable *mp_gtab = new genericTable(m_connectString);
    std::string n1 = ELOGNAME;
    mp_gtab->setLog(n1);
    BOOST_REQUIRE_EQUAL(mp_gtab->connected(),true);
    std::string dropt ("drop table IF EXISTS \"logDigestUnitTest10\" ;");
    std::string cdb( " \
    CREATE DATABASE \"logDigestUnitTest10\" \
    WITH OWNER = postgres    \
       ENCODING = \'UTF8\'  \
       TABLESPACE = pg_default \
       LC_COLLATE = \'en_US.UTF-8\' \
       LC_CTYPE = \'en_US.UTF-8\' \
       CONNECTION LIMIT = -1; \
        ");
    std::string ctable(
    "CREATE TABLE \"hardware_id\" \
    ( \
    \"hwid\" serial, \
    \"make\" character varying, \
    \"revision\" character varying, \
    \"model\" character varying, \
    \"serialnum\" character varying, \
    \"date\" time without time zone, \
    \"description\" character varying, \
    \"ownerid\" integer, \
    CONSTRAINT hwidpk PRIMARY KEY (hwid) \
    );" );

    try{
        pqxx::result r = mp_gtab->nontransQuery(nullptr,dropt);
        r = mp_gtab->nontransQuery(nullptr,cdb);
        delete mp_gtab;
        mp_gtab = nullptr;
        m_connectString += " dbname=logDigestUnitTest10";
        mp_gtab = new genericTable(m_connectString);
        r = mp_gtab->nontransQuery(nullptr,ctable);
        BOOST_CHECK_EQUAL(0,r.size());

    }
    catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        elogSU->errorL(critical,msg.c_str());
    }
    catch (std::exception &e) {
        elogSU->errorL(critical,e.what());
        elogSU->stack_trace("crash & burn");
    }


        delete mp_gtab;
        mp_gtab = nullptr;




    BOOST_TEST_MESSAGE("setup genericTable");
}

testSetup::~testSetup()
{

    m_connectString ="user=postgres host=192.168.81.194 password=m0r3p0w3r";
    mp_gtab = new genericTable(m_connectString);
    std::string n=ELOGNAME;
    mp_gtab->setLog(n);
    BOOST_REQUIRE_EQUAL(mp_gtab->connected(),true);

    std::string ddb("DROP DATABASE \"logDigestUnitTest10\" ; ");
    try {
        pqxx::result r = mp_gtab->nontransQuery(nullptr,ddb);
        BOOST_TEST_MESSAGE("teardown genericTable");
    }
    catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        elog()->errorL(critical,msg.c_str());
    }


        delete mp_gtab;
        mp_gtab = nullptr;
        elog().reset();

}



 genericTable * testSetup::getinstance()
{

    return  mp_gtab;
}

void testSetup::signalCatch(int sigNum) {
    ///t_spSL elog = structuredLogger::getInstance(ELOGNAME,(t_loggingStyle) (allInfoFile| tokenMsg | sepErrorFile | sepDebugFile));
    ///elog->stack_trace("crash & burn");
    ///elog.reset();
    std::cout << stackTrace().str() ;
}
/**
** to test the protected funtions storeValue writeStored setup needs to
** be a supper class
*/

void testSetup::test_storeValue_writeStored() {

    connectDatabase(m_connectString);
    std::string n1 = ELOGNAME;
    setLog(n1);
    std::string tablename="hardware_id";
    selectTable(tablename);
    storeValue(1,(void*) "chevy");        // make
    storeValue(4,(void*) "2oaiueroiwuer234");// serial number
    std::vector<std::string> fieldsToWrite = { "make", "serialnum"};
    pqxx::result r;
    try {
        writeStored(nullptr,fieldsToWrite);
        std::string query = "select make,serialnum from hardware_id; ";
        r = execQuery(nullptr,query);

    }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        elog()->errorL(critical,msg.c_str());
    }
    std::map<std::string,std::string> valuesToCompare {{"make","chevy"},{"serialnum","2oaiueroiwuer234"}};

   for (pqxx::result::const_iterator row = r.begin();
        row != r.end(); row++)
    {
          for (pqxx::result::tuple::const_iterator fields = row->begin();
            fields != row->end();
            fields++)
            {
                std::string cv = valuesToCompare[fields->name()];
                std::string sv = fields->c_str();
                //std::cout << cv << " " << sv << std::endl;
                BOOST_CHECK_EQUAL(cv.compare(sv),0);

            }
    }


    mp_dbc->disconnect();
    mp_wdbc = nullptr;
    delete mp_dbc;
    mp_dbc=nullptr;
    elog().reset();
}
