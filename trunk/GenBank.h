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
    TGenBankFeature ( string _n , string _v ) { name = _n ; value = _v ; }
    string name , value ;
    vector <string> qname , qvalue ;
    } ;

class TGenBankKey
    {
    public :
    TGenBankKey () {} ;
    TGenBankKey ( string _k , string _v ) { key = _k ; value = _v ; }
    string key , value ;
    vector <TGenBankFeature> features ;
    } ;

class TGenBank
    {
    public :
    TGenBank () ;
    void load ( string s ) ;
    void paste ( string s ) ;
    void parseLines ( char *t , long l ) ;
    void makeTree () ;
    void showtree ( TVectorTree *z ) ;
    void remap ( TVector *v ) ;
    void doExport ( TVector *v , vector <string> &ex ) ;
    void scanItem ( TVector *v , int k , int f , int type , bool descfirst = false ) ;
    void addItem ( TVector *v , TVectorItem *vi , string l , int depth = 0 ) ;
    string trim ( string s ) ;
    string trimQuotes ( string s ) ;
    string quote ( string pre , string q ) ;
    vector <string> explode ( char p , string s ) ;
    void wrapit ( vector <string> &ex , string init , string data , int limit = 80 ) ;
    string expand ( string init , int to , string with = " " ) ;
    
    vector <TVS> vs_l ;
    vector <TVI> vi_l ;
    vector <string> vs ;
    vector <int> vi ;
    vector <TGenBankKey> keys ;
    bool perm[256] ;
    string params ;
    bool success ;
    string title , description ;
    } ;

// **************************************************** TFasta

class TFasta
    {
    public :
    TFasta () {} ;
    void load ( string s ) ;
    void remap ( TVector *v ) ;
    
    vector <string> l ;
    bool success ;
    } ;

#endif
