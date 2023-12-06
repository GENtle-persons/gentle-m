/** \file
    \brief The TXMLfile class
*/
#ifndef _TXMLFILE_H_
#define _TXMLFILE_H_

#include "main.h"

class TVector ;
class TiXmlNode ;
class TVectorItem ;
class TiXmlDocument ;

/// This class is a wrapper around the tinyxml software, specialized for GenBankXML format
class TXMLfile
    {
    public :
    TXMLfile () ; ///< Constructor
    ~TXMLfile () {} ; ///< Dummy destructor

    void load ( const wxString& file ) ; ///< Load XML from file
    void parse ( const wxString& text ) ; ///< Pass XML as string
    bool success () const { return _success ; } ///< Errors during parsing?
    int countVectors () const ; ///< How many vectors were made from the XML?
    TVector *getVector ( const int a ) const ; ///< Returns pointer to one of the vectors

    private :
    void analyze ( TiXmlDocument &doc ) ; ///< Parse single/multiple sequences
    void readGBSeq ( TiXmlNode *base ) ; ///< The actual parser
    wxString t ( TiXmlText * const txt ) const ; ///< Safe text conversion
    wxString t ( const char * const txt ) const ; ///< Safe text conversion
    void readGBqualifiers ( TVectorItem &i , TiXmlNode *n ) ; ///< Part of the parser
    void readGBintervals ( vector <TVectorItem> &vi , const TVectorItem &i , TiXmlNode *n ) ; ///< Part of the parser
    wxString get_sub_desc ( TiXmlNode *base , int depth = 0 ) const ;
    bool _success ; ///< Internal success marker
    wxArrayTVector _v ; ///< The generated vectors
    } ;

#endif

