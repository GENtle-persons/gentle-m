#include "TUndo.h"

TUndo::TUndo ( TVector *_base )
    {
    cnt = 0 ;
    base = _base ;
    }
    
// Sets the vector that holds the TUndo item
void TUndo::setbase ( TVector *_base )
    {
    base = _base ;
    }
    
// Remembers the "before" state
// To avoid having multiple "undos" for a (for the user) single action,
// only the first one is stored, the other in-between steps
// are ignored. Each undo-worthy function calls "start" and "stop"/"abort".
void TUndo::start ( wxString _msg )
    {
    cnt++ ;
    if ( cnt > 1 ) return ;
    TVector *v = new TVector ;
    *v = *base ;
    mem.push_back ( v ) ;
    msg.push_back ( _msg ) ;
//    if ( base ) base->callUpdateUndoMenu() ;
    }

void TUndo::stop ()
    {
    cnt-- ;
    if ( cnt > 0 ) return ;
    if ( base ) base->setChanged() ;
    if ( base ) base->callUpdateUndoMenu() ;
    }
    
// Same as stop, but if called from the "initial" caller,
// the undo process will be cancelled
void TUndo::abort ()
    {
    cnt-- ;
    if ( cnt > 0 ) return ;
    mem.pop_back () ;
    msg.pop_back () ;
    if ( base ) base->callUpdateUndoMenu() ;
    }
    
void TUndo::remember ( wxString _msg )
    {
    start ( _msg ) ;
    stop () ;
    }
    
void TUndo::clear ()
    {
    msg.clear() ;
    mem.clear() ;
    cnt = 0 ;
    if ( base ) base->setChanged ( false ) ;
    if ( base ) base->callUpdateUndoMenu() ;
    }

void TUndo::pop ()
    {
    TVector *v = mem[mem.size()-1] ;
    mem.pop_back () ;
    *base = *v ;
    v->undo = NULL ;
    delete v ;
    msg.pop_back () ;
    if ( canUndo() ) base->setChanged ( true ) ;
    else base->setChanged ( false ) ;
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
