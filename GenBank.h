/** \file
	\brief Contains the GenBank class and its TGenBankFeature helper class
*/
#ifndef _GENBANK_H_
#define _GENBANK_H_

#include "main.h"

class TVector ;
class TVectorItem ;
class TVectorTree ;

/// \brief Helper class for TGenBank
class TGenBankFeature
    {
    public :
    TGenBankFeature () {} ; ///< Empty constructor
    TGenBankFeature ( wxString _n , wxString _v ) { name = _n ; value = _v ; } ///< Constructor
    wxString name ; ///< Feature name
    wxString value ; ///< Feature (string) value
    } ;

/// \brief GenBank format import/export class
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

    bool success ; ///< Did we encounter any errors while parsing?
    
    private :
    friend class TAlignment ;
    friend class MyFrame ;
    vector <wxArrayString> vs_l ; ///< All the lines, grouped by sequences
    vector <wxArrayInt> vi_l ; ///< All the line indentations
    wxArrayString vs ; ///< Current sequence
    wxArrayInt vi ; ///< Current line indentations
    wxString params ; ///< ???
    wxString title ; ///< Sequence title
    wxString description ; ///< Sequence description
    bool perm[256] ; ///< Allowed chars
    bool validseq[256] ; ///< Allowed sequence chars
    bool isblank[256] ; ///< Blank chars
    bool isblankorquote[256] ; ///< Blank or quote chars
    
    void parseLines () ;
    void addItem ( TVector *v , wxArrayString &va ) ;
    void iterateItem ( TVector *v , TVectorItem &i , wxString l , int tag = 0 ) ;
    bool isValidSequence ( char a ) ;    
    } ;

#endif
