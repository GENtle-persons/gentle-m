#ifndef _CHILDBASE_H_
#define _CHILDBASE_H_

#include "main.h"
#include <string.h>
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/docview.h>

using namespace std ;

typedef wxMDIChildFrame MyChildBase ;
typedef wxMDIParentFrame MyFrameType ;

class TVector ;
class MyFrame ;

class ChildBase : public MyChildBase
    {
    public :
    ChildBase () { vec = NULL ; }
    string def ;
    TVector *vec ;
    wxTreeItemId inMainTree ;
    
    ChildBase(MyFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
        : MyChildBase((MyFrameType*)parent, -1, title, pos, size, style) {  }
    ChildBase(MyFrame *parent, const wxString& title)
        : MyChildBase((MyFrameType*)parent, -1, title) {  }
        
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
