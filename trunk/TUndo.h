#ifndef _UNDO_H_
#define _UNDO_H_

#include "main.h"

class TVector ;

class TUndo
    {
    public :
    TUndo ( TVector *_base = NULL ) ;
    virtual void setbase ( TVector *_base ) ;
    virtual void start ( wxString _msg = "" ) ;
    virtual void stop () ;
    virtual void pop () ;
    virtual bool canUndo () ;
    virtual wxString getLastMessage () ;
    
    private :
    vector <TVector*> mem ;
    vector <wxString> msg ;
    int cnt ;
    TVector *base ;
    } ;


#endif
