/** \file
	\brief Contains the ChildBase class
*/
#ifndef _CHILDBASE_H_
#define _CHILDBASE_H_

//#include "main.h"
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/docview.h>

using namespace std ;

typedef wxPanel MyChildBase ;
typedef wxFrame MyFrameType ;

class TVector ;
class MyFrame ;
class SequenceCanvas ;

/**	\class ChildBase
	\brief The base class for all modules
*/
class ChildBase : public MyChildBase
    {
    public :
    ChildBase () ; ///< Default constructor
    wxString def ; ///< The class name, as a wxString
    TVector *vec ; ///< Pointer to the base vector (may be NULL, depending on the module)
    wxTreeItemId inMainTree ; ///< The ID of this module in the main tree
    SequenceCanvas *cSequence; ///< Pointer to the sequence display (may be NULL, depending on the module)
    
    ChildBase(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style) ; ///< Constructor
    ChildBase(wxWindow *parent, const wxString& title); ///< Constructor
        
    virtual wxString getName () { return _T("") ; } ///< Returns the name of the module (e.g., the vector name)
    virtual void showName ( wxString x = _T("") ) ; ///< Shows the module name in the window title
    virtual void OnFocus(wxFocusEvent& event); ///< Event handler for focus event
    virtual bool caniclose(wxCloseEvent& event); ///< Checks for unsaved edits
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) {} ; ///< Event handler for save command
    virtual wxToolBar *CreateToolBar ( int i ) ; ///< Tool bar creation
    virtual void Maximize ( bool isit = true ) ; ///< Maximize the window
    virtual void updateUndoMenu () ; ///< Updates the "undo" menu entry
    virtual void OnClose(wxCloseEvent& event); ///< Close event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler
    virtual void OnPaste (wxCommandEvent& WXUNUSED(event)) {} ; ///< Paste event handler
    virtual void OnExport (wxCommandEvent& WXUNUSED(event)) ; ///< Export command event handler
    virtual void EnforceRefesh () {} ; ///< Refreshes the module display

    
    // Compatability functions
    virtual wxToolBar *CreateToolBar ( int i , int j , wxString s ) ; ///< Generates the tool bar
    virtual void SetMenuBar ( wxMenuBar *menu_bar ) ; ///< Sets the menu bar for the window
    virtual wxMenuBar *GetMenuBar () ; ///< Returns a pointer to the menu bar
    virtual wxToolBar *GetToolBar () ; ///< Returns a pointer to the tool bar
    virtual void Activate () ; ///< Activates the module (in foreground etc.)
    virtual void SetIcon ( wxIcon icon ) ; ///< Sets the module icon (unused)
    
    protected :
    friend class MyFrame ;
    wxMenuBar *menubar ; ///< Pointer to the window menu bar
    wxToolBar *toolbar ; ///< Pointer to the window too bar
    
    virtual void doExport ( wxString filename , int filter ) ; ///< Export data, depending on filename and export type
    virtual void exportVector ( TVector *vec , wxFile &out , int filter , wxString filename = _T("") ) ; ///< Exports a TVector
    virtual void arrangedExport ( wxFile &out , wxString n , wxString s , int l ) ;
    virtual void updateToolbar () ; ///< Sets the tool bar
    } ;

#endif
