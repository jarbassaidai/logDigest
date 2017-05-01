#define BOOST_TEST_DYN_LINK
// either Suits or Fitxtures
#define BOOST_TEST_MODULE PgsqlAccess
//#define BOOST_TEST_MODULE Fixtures

#include "testSetup.h"


extern "C"
{

BOOST_AUTO_TEST_SUITE(PgsqlAccess)
/***
* genericTableTest
* rulesTest
***/

/**
BOOST_AUTO_TEST_CASE(filteredIO_test)
{


}
**/
/**
BOOST_AUTO_TEST_CASE(gt_regex_from_db)
{
    std::string db("dbname=ScanningEngine user=postgres password=m0r3p0w3r hostaddr=192.168.81.194 port=5432");
    ruleset rs(db);
    BOOST_CHECK(rs.connected());
    std::vector<std::string> rules;
    std::string logname("KernelLog");
    rs.getRuleset(logname, rules);
    std::string fn("/home/jar/projects/populateDB/test.data/kernel.10.0.108.20.error");
   fileread fr(fn);
    BOOST_CHECK_EQUAL(fr.prepos(),fr.curpos());

    while (fr.readsource()) {
        const char * b = fr.lastLineRead();
        std::string tb(b);
        BOOST_CHECK_EQUAL(tb.empty(),0);
        BOOST_CHECK_GT(fr.curpos(),fr.prepos());
        BOOST_CHECK_EQUAL(fr.curpos() - tb.size() ,fr.prepos()+1 );
        //
        // now compare the line against all the regex expressions we have in our string vector
        //
        boost::cmatch found;
        for (std::vector<std::string>::iterator it = rules.begin(); it != rules.end(); it++){
            std::string regexs = *it ;
            boost::regex expr(regexs.c_str(),boost::regex::perl|boost::match_partial);
            if (boost::regex_match(b,found,expr )) {
                std::cout << found[0].first << ":" << found[0].second   << std::endl;
                std::cout << found[1].first << ":" << found[1].second   << std::endl;
                std::cout << found[2].first << ":" << found[2].second   << std::endl;
            }
        }// end of rules for file
    } // end read file
}
**/
BOOST_AUTO_TEST_SUITE_END()


}

