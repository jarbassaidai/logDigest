#ifndef RULESET_H
#define RULESET_H

#include "genericTable.h"


/// \author $Author: $
/// \version $Revision: $
/// \date $Date: $
/// \file genericTable.h
/// \class ruleTable
///<< rule table has specialized searches of a ruleset so that all the rules within the rulesset can be found
///<<
///<< genericTable will read the database (postgres) for the table description filling in
///<< a vector<std::string> with the fieldnames and a second vector with the data types
///<< a third vector of void * is then used to store the values for the fields
///<< This allows you to use this class for any table with standard data types.

class ruleset : public genericTable
{
    public:
        /** Default constructor */
        ruleset();
        ruleset(std::string &dbconnection);
        ruleset(pqxx::connection *dbc);

        pqxx::result getRuleset(std::string &setName, std::vector<std::string> &rv );

        /** Default destructor */
        virtual ~ruleset();
    protected:

    private:
};

#endif // RULETABLE_H
