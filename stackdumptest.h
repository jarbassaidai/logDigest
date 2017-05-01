#ifndef STACKDUMPTEST_H
#define STACKDUMPTEST_H

#include "structuredLogger.h"

class stackdumptest
{
    public:
        /** Default constructor */
        stackdumptest(structuredLogger &elog,long ncalls );
        /** Default destructor */
        virtual ~stackdumptest();
        void levelN(long l=1) ;
    protected:



    private:
    structuredLogger *myelog;
    long level;

    void callmyself() ;

};

#endif // STACKDUMPTEST_H
