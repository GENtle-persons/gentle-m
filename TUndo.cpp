#include "TUndo.h"

TUndo::TUndo ( TVector *_base )
    {
    cnt = 0 ;
    base = _base ;
    }
    
void TUndo::setbase ( TVector *_base )
    {
    base = _base ;
    }
    
void TUndo::start ( wxString _msg )
    {
    cnt++ ;
    if ( cnt > 1 ) return ;
    TVector *v = new TVector ;
    *v = *base ;
    mem.push_back ( v ) ;
    msg.push_back ( _msg ) ;
    base->callUpdateUndoMenu() ;
    }

void TUndo::stop ()
    {
    cnt-- ;
    if ( cnt > 0 ) return ;
    }

void TUndo::pop ()
    {
    TVector *v = mem[mem.size()-1] ;
    mem.pop_back () ;
    *base = *v ;
    v->undo = NULL ;
    delete v ;
    msg.pop_back () ;
    }
    
bool TUndo::canUndo ()
    {
    return ( mem.size() > 0 ) ;
    }
    
wxString TUndo::getLastMessage ()
    {
    if ( !canUndo() ) return "" ;
    return msg[msg.size()-1] ;
    }
