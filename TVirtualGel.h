#ifndef _TVIRTUAL_GEL_H_
#define _TVIRTUAL_GEL_H_

#include "main.h"
#include "ChildBase.h"

class TMyGelControl ;

class TGelLane
	{
	public :
	virtual void clear () ;
	virtual void setMarker ( wxString _name ) ;
	virtual void add ( int size , int weight = 1 ) ;
	virtual void add ( int size , wxString title ) ;
	virtual void add ( int size , int weight , wxString title ) ;
	wxString name , type ;
	wxArrayInt vi , vw ;
	wxArrayString vs ;
	int unit_volume ;
    wxRect pos ;    
	} ;    

class TVirtualGel : public ChildBase
    {
    public :
    TVirtualGel(wxWindow *parent, const wxString& title) ;
    
    void initme () ;
    virtual wxString getName () ;

//    virtual void OnDir ( wxCommandEvent &event ) ;
//    virtual void OnFile ( wxCommandEvent &event ) ;
    
//    void ShowDir ( wxString s ) ;
//    virtual void Refresh () ;    
//    virtual void OnSize(wxSizeEvent &event) ;

    TMyGelControl *right ;
/*    wxSplitterWindow *rl ;
    wxListBox *lb ;
    wxButton *bu ;*/

    vector <TGelLane> lanes ;
    wxString type ;
    float percent ;
    int cutoff , maxband ;
    
    DECLARE_EVENT_TABLE()
    } ;
    
class TMyGelControl : public wxControl
    {
    public :
    TMyGelControl ( wxWindow *parent , int id = -1 ) ;

    virtual void OnDraw(wxDC& dc) ;
    virtual void OnPaint(wxPaintEvent& event) ;

    virtual void OnEvent(wxMouseEvent& event);
    virtual void OnSaveAsBitmap(wxCommandEvent &event);
    virtual void OnCopy(wxCommandEvent &event);
    virtual void OnPrint(wxCommandEvent &event);
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){};
    
    virtual int getLanePos ( int size , int height , float perc = 0 ) ;
    virtual double fix_percent ( int size , float perc = 0 ) ;
    
    virtual void drawBand ( wxDC &dc , TGelLane &lane , int band ) ;
    
    bool printing ;
    TVirtualGel *vg ;

    DECLARE_EVENT_TABLE()
    } ;
    
#endif
