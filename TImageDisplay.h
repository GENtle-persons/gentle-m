#ifndef _TIMAGE_DISPLAY_H_
#define _TIMAGE_DISPLAY_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>
#include <wx/spinctrl.h>
#include <wx/dirctrl.h>
#include <wx/image.h>

class TIMGreader ;
class TMyImagePanel ;
class TMyFileBox ;

class TImageDisplay : public ChildBase
    {
    public :
    TImageDisplay(wxWindow *parent, const wxString& title) ;
    ~TImageDisplay () ;
    
    void initme () ;
    virtual wxString getName () ;

//    virtual void OnClose(wxCloseEvent& event) ;
    virtual void OnDir ( wxCommandEvent &event ) ;
    virtual void OnFile ( wxCommandEvent &event ) ;
    
    void ShowDir ( wxString s ) ;

    TIMGreader *r ;
    
    TMyImagePanel *right ;
    wxSplitterWindow *rl ;
    wxListBox *lb ;
    wxButton *bu ;
    
    DECLARE_EVENT_TABLE()
    } ;
    
class TMyImagePanel : public wxPanel
    {
    public :
    TMyImagePanel ( wxWindow *parent , int id = -1 ) ;
    ~TMyImagePanel () ;

    virtual void OnDraw(wxDC& pdc) ;
    virtual void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL) ;
    virtual void OnSize(wxSizeEvent &event) ;

    virtual void OnEvent(wxMouseEvent& event);
    virtual void OnSaveAsBitmap(wxCommandEvent &event);
    virtual void OnCopy(wxCommandEvent &event);
    virtual void OnPrint(wxCommandEvent &event);
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){};
    
    wxBitmap *bmp ;
    wxImage i ;
    wxString dir , file ;
    bool printing ;
    TImageDisplay *imdi ;

    DECLARE_EVENT_TABLE()
    } ;
    
#endif
