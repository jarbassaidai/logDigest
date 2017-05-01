#include <structuredLogger.h>
#include "filteredIO.h"


filteredIO::filteredIO(std::string &fname,
                        std::vector<std::string> &searchVector,
                        long linesBeforeMatch,
                        long linesAfterMatch) : fileread(fname)
{
    m_rules = searchVector;
    m_beforeL = linesBeforeMatch;
    m_afterL = linesAfterMatch;

}

filteredIO::~filteredIO()
{
    //dtor
}
///\fn setOutFile
///\arg std::sting & output file specification
///\return void
///

void filteredIO::setOutFile(std::string & outFile) {
    m_outfile = outFile;

     m_ofs.open (m_outfile,std::ofstream::out);

}
///\fn executeReadtoOutFile
///\arg none
///\pre all regular expression filters should be loaded
///\pre outfile name specification set
///< reads

void filteredIO::executeToOutFile(){
    long linecnt =0 ;
    while (readsource()) {
        m_linePostions.push_back(prepos());

        char * b = lastLineRead();
        std::string tb(b);
        //
        // now compare the line against all the regex expressions we have in our string vector
        //
        boost::cmatch found;
        for (std::vector<std::string>::iterator it = m_rules.begin(); it != m_rules.end(); it++){
            std::string regexs = *it ;
            boost::regex expr(regexs.c_str(),boost::regex::perl|boost::match_partial);
            if (boost::regex_search(b,found,expr )) {
                m_lineMatches.push_back(new matchline(prepos(),tb));
                if (m_beforeL == 0 && m_afterL == 0) {
                    m_ofs << m_lineMatches[0]->pos() << " " << m_lineMatches[0]->line() ;
                }else if (m_beforeL !=0){

                    for (long x=(linecnt - m_beforeL > 0) ? linecnt - m_beforeL : 0  ;
                        x > linecnt ; x++ ) {
                            if (x == (linecnt - m_beforeL) )
                                m_ifs.seekg(m_linePostions[x]);
                            if (readsource() )
                                m_ofs << m_linePostions[x] << " " << lastLineRead();
                            else
                                break;
                    }  // end for picking up before lines
                    m_ofs << m_lineMatches[0]->pos() << " " << m_lineMatches[0]->line() ;
                }else if (m_afterL != 0) {
                    /** this skips matches for all afterL lines
                    ** if this is a problem an alternative implementation will need to be
                    ** done
                    */
                    m_ofs << m_lineMatches[0]->pos() << " " << m_lineMatches[0]->line() ;
                    for (int cnt= 0 ; cnt > m_afterL ; cnt++) {
                        if (readsource()) {
                            m_ofs << prepos() << " " << lastLineRead();
                            m_linePostions.push_back(prepos());
                            linecnt++;
                        }
                        linecnt--;  //decrement for the up comming increment
                    } // end for int cnt = 0

                }
            }
            linecnt++;
/**
** save for later use
                std::cout << found[0].first << ":" << found[0].second   << std::endl;
                std::cout << found[1].first << ":" << found[1].second   << std::endl;
                std::cout << found[2].first << ":" << found[2].second   << std::endl;
**/
        }// end of rules for file

    } // end while


}
///\fn getMatch
///\arg long maximum number of matches to collect at one time
///\return  std::vector<matchline> &
///\brief returns a single matchline  it could be empty
///\brief keeps postion so next call will pull next line

std::vector<matchline *> & filteredIO::getMatches(long maxMatches){
   while (readsource() && maxMatches > 0 ) {
        m_linePostions.push_back(prepos());

        char * b = lastLineRead();
        std::string tb(b);
       ///
        /// now compare the line against all the regex expressions we have in our string vector
        ///
        boost::cmatch found;
        bool somematch = false;
        for (std::vector<std::string>::iterator it = m_rules.begin(); it != m_rules.end(); it++){
            std::string regexs = *it ;
            boost::regex expr(regexs.c_str(),boost::regex::perl|boost::match_partial);
            if (boost::regex_search(b,found,expr )) {
                m_lineMatches.push_back(new matchline(prepos(),tb));
                somematch = true;
             }

        } // end for
        if (somematch)
            maxMatches --;
    } // end while file to read
    return m_lineMatches;
}
///\fn getMatchPlus
///\arg long lines before match may be 0
///\arg long lines after match  may be 0
/// < if both before and after lines are zero return vector will contain only a match or
/// < and empty string if no matches have been found
/// < if m_beforeL and/or m_afterL are set the values, passed in arguements
/// < override set values
/// < reject negative line counts and more than 200 lines before or after
std::vector<matchline *> & filteredIO::getMatchPlus(long linesB , long linesA){


    long linecnt = 0;
    bool earlyout = false;
    if (linesB < 0 || linesB > MAX_BEFORE_AFTER_LINES)
        earlyout = true;
    else if (linesA < 0 || linesA > MAX_BEFORE_AFTER_LINES)
        earlyout = true;

    while (readsource() && ! earlyout ) {
        m_linePostions.push_back(prepos());

        char * b = lastLineRead();
        std::string tb(b);
       //
        // now compare the line against all the regex expressions we have in our string vector
        //
        boost::cmatch found;
        bool somematch = false;
        for (std::vector<std::string>::iterator it = m_rules.begin(); it != m_rules.end(); it++){
            std::string regexs = *it ;
            boost::regex expr(regexs.c_str(),boost::regex::perl|boost::match_partial);
            if (boost::regex_search(b,found,expr )) {

                if (m_beforeL == 0 && m_afterL == 0) {
                    m_lineMatches.push_back(new matchline(prepos(),tb));
                    earlyout = true;
                    break;
                }else if (linesB !=0){
                    for (long x=((linecnt - linesB) > 0) ? (linecnt -linesB) : 0  ;
                        x > linecnt ; x++ ) {
                        if (x == (linecnt - linesB))
                            m_ifs.seekg(m_linePostions[x]);
                        if (readsource() ) {
                            std::string x (lastLineRead());
                            m_lineMatches.push_back(new matchline(prepos(),x));
                        }
                        else
                            break;
                    }  // end for picking up before lines

                }else if (m_afterL != 0) {
                    /** this skips matches for all afterL lines
                    ** if this is a problem an alternative implementation will need to be
                    ** done
                    */

                    for (int cnt= 0 ; cnt > m_afterL ; cnt++) {
                        if (readsource()) {
                            std::string x (lastLineRead());
                            m_lineMatches.push_back(new matchline(prepos(),x));
                           linecnt++;
                        }
                        linecnt--;  //decrement for the up comming increment
                    } // end for int cnt = 0

                }
            }  // end if regex match
            linecnt++;

        } // end for

    } // end while file to read
    return m_lineMatches;


}
