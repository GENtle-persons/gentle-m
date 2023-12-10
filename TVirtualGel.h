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
    TGelLane () ; ///< Constructor
    virtual ~TGelLane () {} ///< Dummy destructor

    virtual void clear () ; ///< Clears the lane data
    virtual void setMarker ( const wxString& _name ) ; ///< Sets a default marker
    virtual void add ( const int size , int weight = 1 ) ; ///< Adds a weightenend band
    virtual void add ( const int size , const wxString& title ) ; ///< Adds a labeled band
    virtual void add ( const int size , const int weight , const wxString& title ) ; ///< Adds a band with weight and label
    wxString name , type ;
    wxArrayInt vi , vw ;
    wxArrayString vs ;
    double unit_volume ;
    wxRect pos ; ///< The outer border of the lane
    } ;

/**	\class TVirtualGel
	\brief The virtual gel module
*/
class TVirtualGel : public ChildBase
    {
  public :
    TVirtualGel(wxWindow *parent, const wxString& title) ; ///< Constructor

    virtual void initme () ; ///< Initialization
    virtual wxString getName () const ; ///< Returns the gel module name

    virtual void OnPercent ( wxCommandEvent &ev ) ; ///< Gel percent setting change event handler
    virtual void OnLabel ( wxCommandEvent &ev ) ; ///< Show labels event handler
    virtual void OnMarker ( wxCommandEvent &ev ) ; ///< Show labels event handler
    virtual void OnPrint ( wxCommandEvent &ev ) ; ///< Print event handler

    TMyGelControl *right ; ///< Pointer to the actual gel
    wxChoice *ch_percent , *ch_marker ; ///< Gel concentration choice box
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

    virtual int getLanePos ( const int size , const int height , const float perc = 0 ) ;
    virtual double fix_percent ( const int size , const float perc = 0 ) ;

    virtual void drawBand ( wxDC &dc , TGelLane &lane , int band ) ; ///< Draws a gel band

    bool printing ; ///< Is the current output going to printer?
    TVirtualGel *vg ; ///< Pointer to the parent module

    DECLARE_EVENT_TABLE()
    } ;


class TRestrictionIdentifier : public TVirtualGel
    {
    public :
    TRestrictionIdentifier(wxWindow *parent, const wxString& title) ; ///< Constructor
    virtual void initme () ; ///< Initialization

    virtual void OnDNAListChange(wxCommandEvent &event);
    virtual void OnEnzymeListChange(wxCommandEvent &event);
    virtual wxString getName () const ; ///< Returns the gel module name
    virtual void otherChildrenChanged () ;

    private:
    void listDNA () ;
    void recalcEnzymes() ;
    void addRestrictionLane ( const wxString& en , const TVector * const v ) ;
    wxArrayInt getRestrictionFragments ( const wxString& en , const TVector * const v ) const ;
    bool check4separation ( const wxString& en ) const ;
    bool arrays_are_separate ( const wxArrayInt &a1 , const wxArrayInt &a2 ) const ;
    bool arrays_differ ( const wxArrayInt &a1 , const wxArrayInt &a2 , const int bp , const int percent ) const ;

    wxCheckListBox *dna_list ;
    wxListBox *enzymes_list ;
    wxChoice *bp_list , *percent_list , *group_list ;
    vector <MyChild*> dna_items ;
    bool running ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
