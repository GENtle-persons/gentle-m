/** \file
	\brief Contains the TVirtualGel class, as well as the TGelLane and TMyGelControl helper classes
*/
#ifndef _TVIRTUAL_GEL_H_
#define _TVIRTUAL_GEL_H_

#include "main.h"
#include "ChildBase.h"

class TMyGelControl ;

/**	\class TGelLane
	\brief This class represents a lane on a virtual gel
*/
class TGelLane
	{
	public :
	virtual void clear () ; ///< Clears the lane data
	virtual void setMarker ( wxString _name ) ; ///< Sets a default marker
	virtual void add ( int size , int weight = 1 ) ; ///< Adds a weightenend band
	virtual void add ( int size , wxString title ) ; ///< Adds a labeled band
	virtual void add ( int size , int weight , wxString title ) ; ///< Adds a band with weight and label
	wxString name , type ;
	wxArrayInt vi , vw ;
	wxArrayString vs ;
	int unit_volume ;
    wxRect pos ; ///< The outer border of the lane
	} ;    

/**	\class TVirtualGel
	\brief The virtual gel module
*/
class TVirtualGel : public ChildBase
    {
    public :
    TVirtualGel(wxWindow *parent, const wxString& title) ; ///< Constructor
    
    void initme () ; ///< Initialization
    virtual wxString getName () ; ///< Returns the gel module name

    virtual void OnPercent ( wxCommandEvent &ev ) ; ///< Gel percent setting change event handler
    virtual void OnLabel ( wxCommandEvent &ev ) ; ///< Show labels event handler

    TMyGelControl *right ; ///< Pointer to the actual gel
    wxChoice *ch_percent ; ///< Gel concentration choice box
    wxCheckBox *cb_label ; ///< Show labels check box
    vector <TGelLane> lanes ; ///< The lanes on the gel
    wxString type ; ///< Gel type (unused yet)
    double percent ; ///< Gel concentration
    int cutoff , maxband ;
    
    DECLARE_EVENT_TABLE()
    } ;
    
/**	\class TMyGelControl
	\brief This class handles a single virtual gel for TVirtualGel, using TGelLane lanes
*/
class TMyGelControl : public wxControl
    {
    public :
    TMyGelControl ( wxWindow *parent , int id = -1 ) ; ///< Constructor

    virtual void OnDraw(wxDC& dc) ; ///< Draw event handler
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler

    virtual void OnEvent(wxMouseEvent& event); ///< Mouse event handler
    virtual void OnSaveAsBitmap(wxCommandEvent &event); ///< Save image handler
    virtual void OnCopy(wxCommandEvent &event); ///< Copy event handler
    virtual void OnPrint(wxCommandEvent &event); ///< Print event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler
    
    virtual int getLanePos ( int size , int height , float perc = 0 ) ;
    virtual double fix_percent ( int size , float perc = 0 ) ;
    
    virtual void drawBand ( wxDC &dc , TGelLane &lane , int band ) ; ///< Draws a gel band
    
    bool printing ; ///< Is the current output going to printer?
    TVirtualGel *vg ; ///< Pointer to the parent module

    DECLARE_EVENT_TABLE()
    } ;
    
#endif
