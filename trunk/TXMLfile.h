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
    virtual void load ( wxString file ) ; ///< Load XML from file
    virtual void parse ( wxString text ) ; ///< Pass XML as string
    virtual bool success () { return _success ; } ///< Errors during parsing?
    virtual int countVectors () ; ///< How many vectors were made from the XML?
    virtual TVector *getVector ( int a ) ; ///< Returns pointer to one of the vectors
    
    private :
    virtual void analyze ( const TiXmlDocument &doc ) ; ///< Parse single/multiple sequences
    virtual void readGBSeq ( TiXmlNode *base ) ; ///< The actual parser
    virtual wxString t ( TiXmlText *txt ) ; ///< Safe text conversion
    virtual wxString t ( const char *txt ) ; ///< Safe text conversion
    virtual void readGBqualifiers ( TVectorItem &i , TiXmlNode *n ) ; ///< Part of the parser
    virtual void readGBintervals ( vector <TVectorItem> &vi , const TVectorItem &i , TiXmlNode *n ) ; ///< Part of the parser
    bool _success ; ///< Internal success marker
    wxArrayTVector _v ; ///< The generated vectors
    } ;

#endif

