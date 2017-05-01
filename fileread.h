#ifndef FILEREAD_H
#define FILEREAD_H

#include <structuredLogger.h>
#include <iostream>
#include <fstream>

/// \author $Author: $
/// \version $Revision: $
/// \date $Date: $
/// \file fileread.h
/// \class fileread
///< fileread
///< allows for reading a file a line at a time lim position is saved so it can be retrieved
///< errors are logged to logDigest.log
///< todo: read 20 lines at a time

class fileread
{
    public:
        fileread() { mp_linebuf=NULL; m_curpos=0; m_sourceid = -1; m_prepos =0; }
        fileread( std::string &);
        virtual ~fileread();
        bool readsource();
        static const int maxLineSize = 4096;
        static const int maxLinesRead = 10;
        typedef enum { store,manager,alerts,kernel,message,cron,httpd,postgres} filetype_t;
        inline long curpos() { return m_curpos;}
        inline long prepos() { return m_prepos; }

        inline t_spSL elog() { return structuredLogger::getInstance("populateDB");}
        inline long source(long id= (long) NULL) {return m_sourceid; }
        std::string inline filespec() {return m_filespec; }
    protected:
     std::ifstream m_ifs;
      char * mp_linebuf;
     long m_curpos;
     long m_prepos;
     long m_sourceid;
     std::string m_filespec;

     char *  lastLineRead() { return mp_linebuf;}
    private:
};

#endif // FILEREAD_H
