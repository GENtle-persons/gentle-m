// This file includes import (and later, export) filters for
// - GenBank
// - Sequencing data

#ifndef _GENBANK_H_
#define _GENBANK_H_

#include "main.h"

class TVector ;
class TVectorItem ;
class TVectorTree ;

class TGenBankFeature
    {
    public :
    TGenBankFeature () {} ;
    TGenBankFeature ( wxString _n , wxString _v ) { name = _n ; value = _v ; }
    wxString name , value ;
    wxArrayString qname , qvalue ;
    } ;

class TGenBankKey
    {
    public :
    TGenBankKey () {} ;
    TGenBankKey ( wxString _k , wxString _v ) { key = _k ; value = _v ; }
    wxString key , value ;
    vector <TGenBankFeature> features ;
    } ;

class TGenBank
    {
    public :
    TGenBank () ;
    virtual void load ( wxString s ) ;
    virtual void paste ( wxString s ) ;
    virtual void remap ( TVector *v ) ;
    virtual void doExport ( TVector *v , wxArrayString &ex ) ;
    virtual wxString trim ( wxString s ) ;
    virtual wxString trimQuotes ( wxString s ) ;
    virtual wxString quote ( wxString pre , wxString q ) ;
    virtual void wrapit ( wxArrayString &ex , wxString init , wxString data , int limit = 80 ) ;
    virtual wxString expand ( wxString init , int to , wxString with = " " ) ;

    bool success ;
    
    private :
    friend class TAlignment ;
    friend class MyFrame ;
    vector <wxArrayString> vs_l ;
    vector <wxArrayInt> vi_l ;
    wxArrayString vs ;
    wxArrayInt vi ;
    vector <TGenBankKey> keys ;
    bool perm[256] ;
    wxString params , title , description ;
    
    void parseLines () ;
    void addItem ( TVector *v , wxArrayString &va ) ;
    void iterateItem ( TVector *v , TVectorItem &i , wxString l , int tag = 0 ) ;
    bool isValidSequence ( char a ) ;
    } ;

#endif
