#ifndef SEARCHDATETIME_H
#define SEARCHDATETIME_H

#include <boost/date_time/posix_time/posix_time.hpp>
namespace bdt = boost::date_time ;

class searchDateTime
{
    public:
        /** Default constructor */
        searchDateTime();
        /** Default destructor */
        virtual ~searchDateTime();

    protected:
        /** [2002-Jan-01 01:25:10.000000001/ 2002-Jan-31 01:25:10.123456789]  **/
        /** bdt::time_period m_tp(ptime,ptime); */
    private:
};

#endif // SEARCHDATETIME_H
