#ifndef _TXMLFILE_H_
#define _TXMLFILE_H_

#include "main.h"

class TVector ;
class TiXmlNode ;
class TVectorItem ;
class TiXmlDocument ;

class TXMLfile
    {
    public :
    TXMLfile () ;
    virtual void load ( string file ) ;
    virtual void parse ( string text ) ;
    virtual bool success () { return _success ; }
    virtual int countVectors () ;
    virtual TVector *getVector ( int a ) ;
    
    private :
    virtual void analyze ( const TiXmlDocument &doc ) ;
    virtual void readGBSeq ( TiXmlNode *base ) ;
    virtual string t ( TiXmlText *txt ) ;
    virtual string t ( const char *txt ) ;
    virtual void readGBqualifiers ( TVectorItem &i , TiXmlNode *n ) ;
    virtual void readGBintervals ( vector <TVectorItem> &vi , const TVectorItem &i , TiXmlNode *n ) ;
    bool _success ;
    vector <TVector*> _v ;
    } ;
    

#endif

