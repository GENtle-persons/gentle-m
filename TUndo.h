#ifndef _UNDO_H_
#define _UNDO_H_

#include <wx/wx.h>
#include <vector>

using namespace std ;

class TVector ;

class TUndo
    {
    public :
    TUndo () ;
    virtual void setbase ( TVector *_base ) ;
    virtual void start ( wxString _msg = "" ) ;
    virtual void stop () ;
    virtual void abort () ;
    virtual void pop () ;
    virtual bool canUndo () ;
    virtual wxString getLastMessage () ;
    virtual void remember ( wxString _msg ) ;
    virtual void clear () ;
    
    virtual TUndo & operator = ( TUndo &u ) ;
    
    private :
    vector <TVector*> mem ;
    wxArrayString msg ;
    int cnt ;
    TVector *base ;
    } ;


#endif
