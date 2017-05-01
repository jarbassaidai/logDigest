
#include <stdlib.h>
#include <string>
#include <iostream>
#include <regex>
#include <pqxx/pqxx>

#include "../structuredLogger/structuredLogger.h"
#include "ruleset.h"

/// \version $Revision: $
/// \date $Date: $
/// \file ruleset.cpp
/// \class rulset

///\fn ruleset::ruleset
///\arg none
///<<  constructor no args
///<< connect to rule table  to fill in the fields
///

ruleset::ruleset()
{
    //ctor
}
///\fn ruleset::ruleset
///\arg pqxx::connection *
/// <<< used or creating an already connected db
ruleset::ruleset(pqxx::connection *dbc)
{
    mp_dbc = dbc;
    m_doNotDisconnect = true;
    std::string table("ruleset");
    retriveTableMetadata(table);
}
///\fn ruleset::ruleset
///\arg std::string connection
///<< dbname=[databaseName] user=[database user] password=[md5 encypted password] hostadd=[ip or localhost] port=[5432]
///<<  constructor string for database connection
///<< connect to rule table  to fill in the fields
///

ruleset::ruleset(std::string &dbconnection) : genericTable(dbconnection)
{
    // file in field and type string vectors
    std::string table("ruleset");
    retriveTableMetadata(table);
}

pqxx::result ruleset::getRuleset(std::string &ruleset, std::vector<std::string> &rv)
{

    std::string query( "SELECT rule.id, rule.regex FROM rule INNER JOIN rule_set ON rule.rule_set=rule_set.id and rule_set.name ~* '.*");
    query.append (ruleset) ;
    query.append (".*'");

    pqxx::result r = execQuery(nullptr,query);
        // go through the results
        // vector<string> of rules (regex)
        //

        for (pqxx::result::const_iterator row = r.begin();
        row != r.end(); row++)
        {
            //bool fname = true;    // toggle whether it is the field or the field=-type
            for (pqxx::result::tuple::const_iterator fields = row->begin();
            fields != row->end();
            ++fields)
            {
               rv.push_back(fields->c_str());
               elog()->debugL(debug,fields.c_str(),__FUNCTION__ );
            }


        }

        return r;
}

ruleset::~ruleset()
{
    //dtor
}
