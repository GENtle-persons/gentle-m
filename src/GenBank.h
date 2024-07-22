/** \file
    \brief Contains the TGenBank class and its TGenBankFeature helper class
*/
#ifndef _GENBANK_H_
#define _GENBANK_H_

#include "main.h"
#include "TVector.h"

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
    ~TGenBank () ;
    void load ( const wxString& s ) ;
    void paste ( const wxString& s ) ;
    void remap ( TVector *v ) ;
    void remap ( TVector *v , const wxArrayString &vs , const wxArrayInt &vi ) ;
    void doExport ( const TVector * const sv , wxArrayString &ex ) const ;
    wxString quote ( const wxString& pre , wxString q ) const ;
    void wrapit ( wxArrayString &ex , const wxString& init , wxString data , const int limit = 80 ) const ;
    int count_blanks ( const wxString &s ) const ;

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

    void parseLines () ;
    void addItem ( TVector *v , wxArrayString &va ) ;
    void iterateItem ( TVector * const v , TVectorItem &i , wxString l , const int tag = 0 ) const ;

    static const char * const gb_item_type[VIT_TYPES] ;
    static bool perm[256] ; ///< Allowed chars
    static bool validseq[256] ; ///< Allowed sequence chars
    static bool isblank[256] ; ///< Blank chars
    static bool isblankorquote[256] ; ///< Blank or quote chars
    static bool isValidSequence ( const char& a ) ;
    static void itrim ( wxString &s ) ;
    static wxString expand ( const wxString& init , const int to , const wxString& with = _T(" ") ) ;
    static wxString trim ( const wxString&  s ) ;
    static wxString trimQuotes ( wxString s ) ;
    static void multitrim ( wxString &s , const bool quotes = true ) ;
    } ;

#endif
