/** \file
	\brief Contains the GenBank class and its TGenBankFeature helper class
*/
#ifndef _GENBANK_H_
#define _GENBANK_H_

#include "main.h"

class TVector ;
class TVectorItem ;
class TVectorTree ;

class TGenBankFeature
    {
    public :
    TGenBankFeature () {} ; ///< Empty constructor
    TGenBankFeature ( wxString _n , wxString _v ) { name = _n ; value = _v ; } ///< Constructor
    wxString name ; ///< Feature name
    wxString value ; ///< Feature (string) value
//    wxArrayString qname , qvalue ;
    } ;

class TGenBank
    {
    public :
    TGenBank () ;
    virtual ~TGenBank () ;
    virtual void load ( wxString s ) ;
    virtual void paste ( wxString s ) ;
    virtual void remap ( TVector *v ) ;
    virtual void remap ( TVector *v , const wxArrayString &vs , const wxArrayInt &vi ) ;
    virtual void doExport ( TVector *v , wxArrayString &ex ) ;
    virtual wxString trim ( wxString s ) ;
    virtual void itrim ( wxString &s ) ;
    virtual void multitrim ( wxString &s , bool quotes = true ) ;
    virtual wxString trimQuotes ( wxString s ) ;
    virtual wxString quote ( wxString pre , wxString q ) ;
    virtual void wrapit ( wxArrayString &ex , wxString init , wxString data , int limit = 80 ) ;
    virtual wxString expand ( wxString init , int to , wxString with = " " ) ;
    virtual int count_blanks ( wxString &s ) ;

    bool success ;
    
    private :
    friend class TAlignment ;
    friend class MyFrame ;
    vector <wxArrayString> vs_l ;
    vector <wxArrayInt> vi_l ;
    wxArrayString vs ;
    wxArrayInt vi ;
    bool perm[256] ;
    wxString params ;
    wxString title ;
    wxString description ;
    bool validseq[256] ;
    bool isblank[256] ;
    bool isblankorquote[256] ;
    
    void parseLines () ;
    void addItem ( TVector *v , wxArrayString &va ) ;
    void iterateItem ( TVector *v , TVectorItem &i , wxString l , int tag = 0 ) ;
    bool isValidSequence ( char a ) ;    
    } ;

#endif
