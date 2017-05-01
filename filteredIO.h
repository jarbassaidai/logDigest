#ifndef FILTEREDIO_H
#define FILTEREDIO_H

#include <stdlib.h>
#include <string>
#include <iostream>
#include "fileread.h"
#include <boost/regex.hpp>
#include <fstream>


/// \author $Author: $
/// \version $Revision: $
/// \date $Date: $
/// \file filteredIO.h
/// \class matchLine
///< matchline a class with a long file pointer for starting postion and the matching line
///<

class matchline {
    public:
        matchline(long fpos, std::string & line) { m_pos = fpos; m_line = line; }
        long  pos() { return m_pos; }
        std::string & line() { return m_line; }

    protected:
        long m_pos;
        std::string m_line;
        std::ofstream m_ofs;
};

static const long MAX_BEFORE_AFTER_LINES = 200;

/// \author $Author: $
/// \version $Revision: $
/// \date $Date: $
/// \file filteredIO.h
/// \class filteredIO
///< filteredIO
///< a class to read a source file apply multiple regular expression filters  passing back
///< the starting position and the line or lines that match
///

class filteredIO : public fileread
{
    public:
        /** Default constructor */
        filteredIO(std::string &fname,
                    std::vector<std::string> &searchVector,
                    long linesBeforeMatch=0,
                    long linesAfterMatch=0);
        void setOutFile(std::string & outFile);
        void executeToOutFile();
        std::vector<matchline*> & getMatches(long maxMatches=100);
        std::vector<matchline*> & getMatchPlus(long linesB , long linesA);

        /** Default destructor */
        virtual ~filteredIO();

    protected:

        std::vector<std::string> m_rules; /** regex strings **/
        std::vector<matchline *> m_lineMatches;
        std::string m_outfile;
        /** used to grab x number of before lines and x number of after lines  **/
        std::vector<long> m_linePostions;
        std::ofstream m_ofs;
        long m_beforeL;
        long m_afterL;
    private:
};

#endif // FILTEREDIO_H
