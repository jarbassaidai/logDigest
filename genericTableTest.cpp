///#define BOOST_TEST_MODULE GenericTable
#include "testSetup.h"

BOOST_AUTO_TEST_SUITE(s_PgSql_GenericTable)


BOOST_AUTO_TEST_CASE(genericTable_basic)
{
    boost::filesystem::path::codecvt() ;
    //boost::filesystem::path::imbue(std::locale("C"));
    sigCatch sc;
    testSetup s;

}

BOOST_AUTO_TEST_CASE(genericTable_Test)
{
 testSetup su;

   genericTable gt(su.connectionString());
   std::string n1 = ELOGNAME;
    gt.setLog(n1);
    BOOST_TEST_MESSAGE("construction genericTable");



}

BOOST_AUTO_TEST_CASE(genericTable_retriveTableMetadata)
{
testSetup su;
   genericTable gt(su.connectionString());
   std::string n1 = ELOGNAME;
    gt.setLog(n1);

    // do stuff

    std::string table("hardware_id");
    gt.retriveTableMetadata(table);
    std::vector<std::string> &fields = gt.getFieldsVector();
    BOOST_CHECK_EQUAL(fields.size(),8);
    BOOST_CHECK_EQUAL(fields[0].compare("hwid"),0);
    BOOST_CHECK_EQUAL(fields[1].compare("make"),0);
    BOOST_CHECK_EQUAL(fields[2].compare("revision"),0);
    BOOST_CHECK_EQUAL(fields[3].compare("model"),0);
    BOOST_CHECK_EQUAL(fields[4].compare("serialnum"),0);
    BOOST_CHECK_EQUAL(fields[5].compare("date"),0);
    BOOST_CHECK_EQUAL(fields[6].compare("description"),0);
    BOOST_CHECK_EQUAL(fields[7].compare("ownerid"),0);
    //BOOST_CHECK_EQUAL(fields[4].compare("endoflife"),0);

}

BOOST_AUTO_TEST_CASE(gt_w_fields)
{
    testSetup su;
    std::string tname ="hardware_id";
    std::vector<std::string> fnames{
    "hwid","make","revision", "model","serialnum","date","description","ownerid"
    };
    genericTable gt(fnames,su.connectionString(),tname);

    try{
        gt.retriveTableMetadata(tname);
        std::vector<std::string> &fields = gt.getFieldsVector();

    }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
    }
    catch (std::exception &e) {
        su.elog()->errorL(critical,e.what());
        su.elog()->stack_trace("crash & burn");
    }
}

BOOST_AUTO_TEST_CASE(gt_create_t_w_fields)
{
    testSetup su;
    std::string tname ="hardware_id2";
    std::vector<std::string> fnames{
    "hwid","make","revision", "model","serialnum","date","description","ownerid"
    };
    std::vector<std::string> fdataTypes { "serial","character varying", "character varying",
    "character varying", "character varying","time without time zone", "character varying",
    "integer"};
    genericTable gt(fnames,su.connectionString(),tname);

    try{
        gt.createTable(tname,fnames,fdataTypes);

    }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        su.elog()->errorL(critical,msg.c_str());
    }


}
/**
** writing  postions one and three character varying
** these are protected calls code is in testSetup and testSetup is a superclass of genericTable
**/

BOOST_AUTO_TEST_CASE(gt_storeValue)
{
    testSetup su;
    su.test_storeValue_writeStored();


}

BOOST_AUTO_TEST_CASE(gt_write_read)
{
    testSetup su;
    std::string tname ="hardware_id2";
    std::vector<std::string> fnames{
    "hwid","make","revision", "model","serialnum","date","description","ownerid"
    };
    std::vector<std::string> fdataTypes { "serial","character varying", "character varying",
    "character varying", "character varying","time without time zone", "character varying",
    "integer"};
    genericTable gt(fnames,su.connectionString(),tname);
    std::vector<std::string> wfnames { "make","revision", "model","serialnum","date"};
    std::vector<std::string> data {"ford","v10","shellby","x123","19:30:45.23"};
    std::map<std::string,std::string> mapData  {{"make","ford"},{"revision","v10"},
    {"model","shellby"},{"serialnum","x123"},{"date","19:30:45.23"}};

    std::string blank;
    try{
        gt.createTable(tname,fnames,fdataTypes);
        gt.selectTable(tname);
        gt.write(nullptr,wfnames,data,blank);
        blank="model ~ \'shellby\' ";
        pqxx::result r =gt.readFields(nullptr,fnames,blank);
        for (pqxx::result::iterator ir = r.begin(); ir != r.end() ; ir++) {
        /**    std::cout << ir["hwid"].as<std::string>() << " "
            << ir["make"].as<std::string>() << " "
            << ir["revision"].as<std::string>() << " "
            << ir["model"].as<std::string>() << " "
            << ir["serialnum"].as<std::string>() << " "
            << ir["date"].as<std::string>() << " " << std::endl;
            std::cout << "does >shellby< equal >" << ir["model"].as<std::string>()
            << "< ?" << std::endl;
        **/

            for ( pqxx::result::tuple::const_iterator field = ir->begin();
                    field != ir->end();
                    ++field) {
                  std::string original = mapData[field->name()];
                  std::string recordField = field->c_str();
                  if (!original.empty())
                    BOOST_CHECK_EQUAL(original.compare(recordField),0);
                  //std::cout << ir[*i].as<std::string>() << " ";
            }

            std::cout << std::endl;

        }



    }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        su.elog()->errorL(critical,msg.c_str());
    }

}

BOOST_AUTO_TEST_CASE(gt_update)
{
    testSetup su;
    std::string tname ="hardware_id2";
    std::vector<std::string> fnames{
    "hwid","make","revision", "model","serialnum","date","description","ownerid"
    };
    std::vector<std::string> fdataTypes { "serial","character varying", "character varying",
    "character varying", "character varying","time without time zone", "character varying",
    "integer"};
    genericTable gt(fnames,su.connectionString(),tname);
    std::vector<std::string> wfnames { "make","revision", "model","serialnum","date"};
    std::vector<std::string> data {"ford","v10","shellby","x123","19:30:45.23"};
      std::map<std::string,std::string> mapData  {{"make","ford"},{"revision","v10"},
    {"model","shellby"},{"serialnum","x123"},{"date","19:30:45.23"}};

    std::string blank;
    try{
        gt.createTable(tname,fnames,fdataTypes);
        gt.selectTable(tname);
        gt.write(nullptr,wfnames,data,blank);
        blank="model ~ \'shellby\' ";
        pqxx::result r =gt.readFields(nullptr,fnames,blank);
        for (pqxx::result::iterator ir = r.begin(); ir != r.end() ; ir++) {
        /**    std::cout << ir["hwid"].as<std::string>() << " "
            << ir["make"].as<std::string>() << " "
            << ir["revision"].as<std::string>() << " "
            << ir["model"].as<std::string>() << " "
            << ir["serialnum"].as<std::string>() << " "
            << ir["date"].as<std::string>() << " " << std::endl;
            std::cout << "does >shellby< equal >" << ir["model"].as<std::string>()
            << "< ?" << std::endl;
        **/

            for ( pqxx::result::tuple::const_iterator field = ir->begin();
                    field != ir->end();
                    ++field)  {
                  std::string original = mapData[field->name()];
                  std::string recordField = field->c_str();
                  if (!original.empty())
                    BOOST_CHECK_EQUAL(original.compare(recordField),0);
                  //std::cout << ir[*i].as<std::string>() << " ";
            }

            std::cout << std::endl;

        }
        std::vector<std::string> upFields{"description","ownerid"};
        std::vector<std::string> updata{"Hotest car around in the usa","123169"};
        BOOST_CHECK_EQUAL(gt.update(nullptr,upFields,updata,blank),true);
        for (std::vector<std::string>::iterator ui = upFields.begin();
            ui != upFields.end(); ui++)
            wfnames.push_back(*ui);

        for(std::vector<std::string>::iterator udi = updata.begin();
            udi != updata.end(); udi++)
            data.push_back(*udi);

        r =gt.readFields(nullptr,wfnames,blank);
        for (pqxx::result::iterator ir = r.begin(); ir != r.end() ; ir++ ) {

            for ( pqxx::result::tuple::const_iterator field = ir->begin();
                    field != ir->end();
                    ++field) {
                  std::string original = mapData[field->name()];
                  std::string recordField = field->c_str();
                  if (!original.empty())
                    BOOST_CHECK_EQUAL(original.compare(recordField),0);

            }
            int orig = ir[6].as<int>();
            BOOST_CHECK_EQUAL(orig,123169);
            std::cout << std::endl;
        }


    }catch (const pqxx::sql_error &e)
    {
        std::string msg = e.query() + e.what();
        su.elog()->errorL(critical,msg.c_str());
    }

}

BOOST_AUTO_TEST_SUITE_END()
