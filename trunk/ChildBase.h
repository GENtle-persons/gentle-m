#ifndef _CHILDBASE_H_
#define _CHILDBASE_H_

#include <string.h>
#include <wx/wx.h>
#include <wx/treectrl.h>

using namespace std ;

class MyApp ;
class TVector ;

class ChildBase : public wxMDIChildFrame
    {
    public :
    ChildBase () { app = NULL ; vec = NULL ; }
    string def ;
    MyApp *app ;
    TVector *vec ;
    wxTreeItemId inMainTree ;
    
    ChildBase(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
        : wxMDIChildFrame(parent, -1, title, pos, size, style) {  }
    ChildBase(wxMDIParentFrame *parent, const wxString& title)
        : wxMDIChildFrame(parent, -1, title) {  }
        
    virtual string getName () { return "" ; }
    virtual void showName ( string x = "" ) ;
    virtual void OnFocus(wxFocusEvent& event);
    virtual bool caniclose(wxCloseEvent& event);
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) {} ;
    virtual wxToolBar *CreateToolBar ( int i ) ;
    virtual void Maximize ( bool isit = true ) ;
    virtual void updateUndoMenu () ;
    } ;

#endif
