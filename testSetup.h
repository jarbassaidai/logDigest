#ifndef TESTSETUP_H
#define TESTSETUP_H

#include <boost/test/unit_test.hpp>
#include "structuredLogger.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <csignal>
#include "msgFormatVerify.h"
//#include "stackdumptest.h"
#include "genericTable.h"

#include "stackTrace.h"
// define all the regular expressions for readability
#define DATEREG "(\\d+ \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d+)"
#define SEVREG "(<[^>]+>)"
#define MEDREG "(\\[[^\\]]+\\])"
#define THREADREG "(\\[[\\dabcdefx]+\\])"

// Make this the Unit test

//#define NORMREG DATEREG SEVEREG
//#define DEBUGREG NORMREG MEDREG THREADREG

//
//  BOOST_WARN_EQUAL, BOOST_CHECK_EQUAL, BOOST_REQUIRE_EQUAL
//
//    BOOST_<level>
//    BOOST_<level>_BITWISE_EQUAL
//    BOOST_<level>_CLOSE
//    BOOST_<level>_CLOSE_FRACTION
//    BOOST_<level>_EQUAL
//    BOOST_<level>_EQUAL_COLLECTION
//    BOOST_<level>_EXCEPTION
//    BOOST_<level>_GE
//    BOOST_<level>_GT
//    BOOST_<level>_LE
//    BOOST_<level>_LT
//    BOOST_<level>_MESSAGE
//    BOOST_<level>_NE
//    BOOST_<level>_NO_THROW
//    BOOST_<level>_PREDICATE
//    BOOST_<level>_SMALL
//    BOOST_<level>_THROW
//    BOOST_ERROR
//    BOOST_FAIL
//    BOOST_IS_DEFINED
//
////
#define ELOGNAME "pqsqlAccessUtest"
class testSetup :public genericTable {
public:
    testSetup();
    ~testSetup();
    static void signalCatch(int);

     inline std::string & connectionString() { return m_connectString; }
     genericTable * getinstance();
    inline t_spSL elog(){ return structuredLogger::getInstance(ELOGNAME); }
    void test_storeValue_writeStored() ;


 protected:
    genericTable *mp_gtab;
    std::string m_connectString;

};

class sigCatch {

public:
    sigCatch() {
        signal(SIGSEGV,&sigCatch::bang);
        }
    void static bang(int) {
        std::cout << stackTrace().str() ;
    }
};

#endif // TESTSETUP_H
