#ifndef _PDB_H_
#define _PDB_H_

#include "main.h"

class TVector ;

class tseqres
    {
    public :
    char chain ;
    wxString sequence ;
    TVector *v ;
    } ;    

class TPDB
    {
    public :
    TPDB () ;
    virtual void load ( wxString s ) ;
    virtual void paste ( wxString s ) ;
    virtual void remap () ;

    bool success ;
    vector <tseqres> seqres ;
    
    private :
    virtual void check4success () ;
    virtual int seq ( char c ) ;
    wxArrayString vs ;
    wxString _name , _desc ;
    } ;
    
#endif    
