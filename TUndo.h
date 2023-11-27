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
    void setbase ( TVector *_base ) ; ///< Set the base TVector
    void start ( wxString _msg = _T("") ) ; ///< Start the undo record, with action name
    void stop () ; ///< End the undo record
    void abort () ; ///< Abort the undo record
    void pop () ; ///< Remove the last undo record
    bool canUndo () ; ///< Is there undo information available?
    wxString getLastMessage () ; ///< Returns the name of the last action with undo information
    void remember ( wxString _msg ) ; ///< Remember the current state and action name
    void clear () ; ///< Remove all undo information

    TUndo & operator = ( const TUndo &u ) ;

    private :
    wxArrayTVector mem ; ///< The old TVector objects
    wxArrayString msg ; ///< The action names
    int cnt ; ///< Internam counter
    TVector *base ; ///< Pointer to the base TVector object
    } ;


#endif
