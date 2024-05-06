/** \file
    \brief The TUndo class
*/
#include "TUndo.h"
#include "TVector.h"

//#define IS_UNDO_OFF return ;
#define IS_UNDO_OFF if ( myapp()->frame->dying ) return ;

TUndo::TUndo ()
    {
    cnt = 0 ;
    base = NULL ;
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
void TUndo::start ( const wxString& _msg )
    {
    IS_UNDO_OFF
    cnt++ ;
    if ( cnt > 1 ) return ;
    TVector *v = new TVector ;
    *v = *base ;
    mem.Add ( v ) ;
    msg.Add ( _msg ) ;
    }

void TUndo::stop ()
    {
    IS_UNDO_OFF
    cnt-- ;
    if ( cnt > 0 ) return ;
    if ( base ) base->setChanged() ;
    if ( base ) base->callUpdateUndoMenu() ;
    }

// Same as stop, but if called from the "initial" caller,
// the undo process will be cancelled
void TUndo::abort ()
    {
    IS_UNDO_OFF
    cnt-- ;
    if ( cnt > 0 ) return ;
    delete mem.Last() ;
    mem.RemoveAt ( mem.GetCount()-1 ) ;
    msg.RemoveAt ( msg.GetCount() - 1 ) ;
    if ( base ) base->callUpdateUndoMenu() ;
    }

void TUndo::remember ( const wxString& _msg )
    {
    IS_UNDO_OFF
    start ( _msg ) ;
    stop () ;
    }

void TUndo::clear ()
    {
    IS_UNDO_OFF
    for ( int a = 0 ; a < mem.GetCount() ; a++ )
        {
        mem[a]->undo.clear () ;
        delete mem[a] ;
        }
    msg.Clear() ;
    mem.Clear() ;
    cnt = 0 ;
    if ( base )
        {
        base->setChanged ( false ) ;
        base->callUpdateUndoMenu() ;
        }
    }

void TUndo::pop ()
    {
    IS_UNDO_OFF
    TVector *v = mem.Last() ;
    mem.RemoveAt ( mem.GetCount()-1 ) ;
    msg.RemoveAt ( msg.GetCount() - 1 ) ;

    wxArrayTVector _mem = mem ;
    wxArrayString _msg = msg;

    v->undo.mem.Clear() ;
    v->undo.msg.Clear() ;
    mem.Clear() ;
    msg.Clear() ;

    *base = *v ;
    delete v ;

    base->undo.mem = _mem ;
    base->undo.msg = _msg ;

    if ( canUndo() ) base->setChanged ( true ) ;
    else base->setChanged ( false ) ;
    }

bool TUndo::canUndo ()
    {
    return ( mem.GetCount() > 0 ) ;
    }

wxString TUndo::getLastMessage ()
    {
    if ( !canUndo() ) return _T("") ;
    return msg.Last() ;
    }

TUndo &TUndo::operator = ( const TUndo &u )
    {
    TVector *b = base ;
    base = NULL ;
    clear () ;
    base = b ;
    return *this ;
    }

