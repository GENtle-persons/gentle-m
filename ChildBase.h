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

class ChildBase : public MyChildBase
    {
    public :
    ChildBase () ;
    wxString def ;
    TVector *vec ;
    wxTreeItemId inMainTree ;
    SequenceCanvas *cSequence;
    
    ChildBase(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style) ;
    ChildBase(wxWindow *parent, const wxString& title);
        
    virtual wxString getName () { return "" ; }
    virtual void showName ( wxString x = "" ) ;
    virtual void OnFocus(wxFocusEvent& event);
    virtual bool caniclose(wxCloseEvent& event);
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) {} ;
    virtual wxToolBar *CreateToolBar ( int i ) ;
    virtual void Maximize ( bool isit = true ) ;
    virtual void updateUndoMenu () ;
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){};
    virtual void OnPaste (wxCommandEvent& WXUNUSED(event)) {} ;
    virtual void OnExport (wxCommandEvent& WXUNUSED(event)) ;

    
    // Compatability functions
    virtual wxToolBar *CreateToolBar ( int i , int j , wxString s ) ;
    virtual void SetMenuBar ( wxMenuBar *menu_bar ) ;
    virtual wxMenuBar *GetMenuBar () ;
    virtual wxToolBar *GetToolBar () ;
    virtual void Activate () ;
    virtual void SetIcon ( wxIcon icon ) ;
    
    protected :
    friend class MyFrame ;
    wxMenuBar *menubar ;
    wxToolBar *toolbar ;
    
    virtual void doExport ( wxString filename , int filter ) ;
    virtual void exportVector ( TVector *vec , wxFile &out , int filter ) ;
    virtual void arrangedExport ( wxFile &out , wxString n , wxString s , int l ) ;
    virtual void updateToolbar () ;
    } ;

#endif
