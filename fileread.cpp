
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "../structuredLogger/structuredLogger.h"
#include "fileread.h"
///\file fileread.h
///
///\fn fileread::fileread(char *name)
///< constructor. allocates memory, opens the file. memory is freed when the distructor is called
///<
///\param  char *  name of file to read
///
fileread::fileread( std::string & name)
{
    m_filespec= name;
    m_ifs.open (name,std::ifstream::in);
    m_curpos = m_prepos = 0;
    mp_linebuf = ((char *) calloc(1,maxLineSize + 32));
    m_sourceid = -1;
    //ctor
}
///\fn fileread::~fileread
///\brief closed file frees memory

fileread::~fileread()
{
    //dtor
    m_ifs.close();
    elog().reset();
    if (mp_linebuf != nullptr)
        free(mp_linebuf);

}
///\fn fileread::readsource()
///\brief reads a single line of the file into the line buffer
///< use lastLineRead to get const char * buffer pointer
///< returns true if read succeeded
///<
bool fileread::readsource()
{
    bool r = true;
    if (! m_ifs.eof() )
    {
        m_prepos = m_ifs.tellg();
        m_ifs.getline(mp_linebuf,maxLineSize);
        if (! m_ifs.good())
        {
            r = false;
            elog()->errorL(error,"m_ifs.getline(mp_linebuf,maxLineSize) failed",
                           __FUNCTION__);

        }
        else
        {
            m_curpos = m_ifs.tellg();
        }
    }
    return r;
}

