#include <stdlib.h>
#include <regex>

#include <string>
#include <map>
#include <boost/regex.hpp>
#include "../structuredLogger/structuredLogger.h"
#include "filetype.h"


filetype::filetype(std::string filespec)
{
    //ctor


    m_filespec = filespec;

    boost::regex csufix( ".*?\\.(tar\\.gz|tgz|tbz|rar|zip|7zip|tar|gz)$");

    std::string::const_iterator start, end;
    start = m_filespec.begin();
    end = m_filespec.end();
    boost::smatch compression;
    boost::match_flag_type flags = boost::match_default | boost::match_perl;
    if (regex_match(m_filespec,compression,csufix))
    {
        m_compressionType = compression[1];
    }
    else
    {
        while(regex_search(start, end, compression, csufix, flags))
        {
            m_compressionType = compression[1];
            flags |= boost::match_prev_avail;
            flags |= boost::match_not_bob;
            start++;
        }
    }

}

filetype::~filetype()
{
    //dtor
    elog().~structuredLogger();
}
