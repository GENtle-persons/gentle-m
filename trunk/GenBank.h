// This file includes import (and later, export) filters for
// - GenBank
// - Sequencing data

#ifndef _GENBANK_H_
#define _GENBANK_H_

#include "main.h"

class TVector ;
class TVectorItem ;
class TVectorTree ;
typedef vector <string> TVS ;
typedef vector <int> TVI ;

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
    void load ( wxString s ) ;
    void paste ( wxString s ) ;
//    void parseLines ( char *t , long l ) ;
//    void makeTree () ;
//    void showtree ( TVectorTree *z ) ;
    void remap ( TVector *v ) ;
    void doExport ( TVector *v , wxArrayString &ex ) ;
//    void scanItem ( TVector *v , int k , int f , int type , bool descfirst = false ) ;
//    void addItem ( TVector *v , TVectorItem *vi , wxString l , int depth = 0 ) ;
    wxString trim ( wxString s ) ;
    wxString trimQuotes ( wxString s ) ;
    wxString quote ( wxString pre , wxString q ) ;
//    wxArrayString explode ( char p , wxString s ) ;
    void wrapit ( wxArrayString &ex , wxString init , wxString data , int limit = 80 ) ;
    wxString expand ( wxString init , int to , wxString with = " " ) ;
    
    vector <wxArrayString> vs_l ;
    vector <TVI> vi_l ;
    vector <int> vi ;
    vector <TGenBankKey> keys ;
    wxArrayString vs ;
    bool perm[256] ;
    bool success ;
    wxString params , title , description ;
    
    private :
    void parseLines () ;
    void addItem ( TVector *v , wxArrayString &va ) ;
    void iterateItem ( TVector *v , TVectorItem &i , wxString l , int tag = 0 ) ;
    bool isValidSequence ( char a ) ;
    } ;

// **************************************************** TFasta

class TFasta
    {
    public :
    TFasta () {} ;
    void load ( wxString s ) ;
    void remap ( TVector *v ) ;
    
    wxArrayString l ;
    bool success ;
    } ;

#endif
