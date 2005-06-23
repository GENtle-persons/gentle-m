/** \file
	\brief The TUndo class
*/
#ifndef _UNDO_H_
#define _UNDO_H_

#include <wx/wx.h>

using namespace std ;

class TVector ;

#ifndef _wxArrayTVector
#define _wxArrayTVector
WX_DEFINE_ARRAY(TVector *, wxArrayTVector);
#endif

/// This class stores TVector objects for the undo function
class TUndo
    {
    public :
    TUndo () ; ///< Constructor
	~TUndo () {} ; ///< Destructor (empty)
    virtual void setbase ( TVector *_base ) ; ///< Set the base TVector
    virtual void start ( wxString _msg = _T("") ) ; ///< Start the undo record, with action name
    virtual void stop () ; ///< End the undo record
    virtual void abort () ; ///< Abort the undo record
    virtual void pop () ; ///< Remove the last undo record
    virtual bool canUndo () ; ///< Is there undo information available?
    virtual wxString getLastMessage () ; ///< Returns the name of the last action with undo information
    virtual void remember ( wxString _msg ) ; ///< Remember the current state and action name
    virtual void clear () ; ///< Remove all undo information
    
    virtual TUndo & operator = ( TUndo &u ) ;
    
    private :
    wxArrayTVector mem ; ///< The old TVector objects
    wxArrayString msg ; ///< The action names
    int cnt ; ///< Internam counter
    TVector *base ; ///< Pointer to the base TVector object
    } ;


#endif
