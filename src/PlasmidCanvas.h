/** \file
    \brief Contains the PlasmidCanvas class
*/
#ifndef _PlasmidCanvas_H_
#define _PlasmidCanvas_H_

#include "main.h"
#include <wx/tooltip.h>
#include <wx/metafile.h>

#define STANDARDRADIUS 10000

#define MODE_CIRCULAR 1
#define MODE_LINEAR 2

#define TT_NONE 0
#define TT_RS 1
#define TT_ITEM 2
#define TT_ORF 3

class MyChild ;
class TVectorItem ;
class TPrimer ;

/**  \brief The class to draw the DNA map, as well as the mini-map in the amino acid module
*/
class PlasmidCanvas : public wxScrolledWindow
    {
    public:
    PlasmidCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size); ///< Constructor
    virtual ~PlasmidCanvas () ; ///< Destructor
    virtual void OnDraw(wxDC& pdc) ; ///< Draws the map
    virtual void Refresh () ; ///< Redraws the map
    virtual void print () ; ///< Prints the map
    virtual void setZoom ( const int factor ) ; ///< Sets the display zoom factor
    virtual wxString getSelection () const ; ///< Returns the current sequence selection
    virtual void RunPrimerEditor ( vector <TPrimer> &pl , int mut = -1 ) ; ///< Invokes the PCR/primer design module
    virtual void invokeVectorEditor ( const wxString& what = _T("") , const int num = 0 , const bool forceUpdate = false ) ; ///< Invokes the vector editor dialog
    virtual wxMenu *invokeItemPopup ( const int item , const wxPoint& pt , const bool doreturn = false ) ; ///< Generates the item/feature context menu
    virtual wxMenu *invokeRsPopup ( const int rs , const wxPoint& pt , const bool doreturn = false ) ; ///< Generates the restriction enzyme context menu
    virtual wxMenu *invokeVectorPopup ( const wxPoint& pt , const bool doreturn = false , int pos = -1 ) ; ///< Generates the vector context menu
    virtual void invokeORFpopup ( const int item , const wxPoint& pt ) ; ///< Generates the open reading frame context menu


    virtual void OnPrimerForward ( wxCommandEvent& ev ) ; ///< "Run PCR with primer forward" event handler
    virtual void OnPrimerBackward ( wxCommandEvent& ev ) ; ///< "Run PCR with primer backwards" event handler
    virtual void OnPrimerBoth ( wxCommandEvent& ev ) ; ///< "Run PCR with both primers" event handler
    virtual void OnPrimerMutation ( wxCommandEvent& ev ) ; ///< "Run PCR with mutation primers" event handler
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    virtual void OnSaveImage ( wxCommandEvent& ev ) ; ///< "Save as image" event handler

    virtual void OnStrandCopy35(wxCommandEvent& event); ///< "Copy 3'->5' strand" event handler
    virtual void OnStrandCopy53(wxCommandEvent& event); ///< "Copy 5'->3' strand" event handler
    virtual void OnStrandCopyBoth(wxCommandEvent& event); ///< "Copy both strands" event handler
    virtual void OnStrandNew35(wxCommandEvent& event); ///< "New DNA module from 3'->5' strand" event handler
    virtual void OnStrandNew53(wxCommandEvent& event); ///< "New DNA module from 5'->3' strand" event handler
    virtual void OnStrandNewBoth(wxCommandEvent& event); ///< "New DNA modules from both strands" event handler
    virtual void OnOpenFeature(wxCommandEvent& event); ///< "Open feature" event handler

    // Popup menu handlers
    virtual void OnCopyImage ( wxCommandEvent &ev ) ; ///< "Copy as image" event handler
    virtual void itemMark ( wxCommandEvent &ev ) ; ///< "Mark item" event handler
    virtual void itemMarkShow ( wxCommandEvent &ev ) ; ///< "Mark item and show in sequence" event handler
    virtual void itemEdit ( wxCommandEvent &ev ) ; ///< "Edit item" event handler
    virtual void itemAsNewSequence ( wxCommandEvent &ev ) ; ///< "Make a new sequence from item" event handler
    virtual void itemDelete ( wxCommandEvent &ev ) ; ///< "Delete item" event handler
    virtual void itemBlastDNA ( wxCommandEvent &ev ) ; ///< "BLAST item DNA sequence" event handler
    virtual void itemBlastAA ( wxCommandEvent &ev ) ; ///< "BLAST item amino acid sequence" event handler
    virtual void itemCopyDNA ( wxCommandEvent &ev ) ; ///< "Copy item DNA sequence" event handler
    virtual void itemCopyAA ( wxCommandEvent &ev ) ; ///< "Copy item amino acid sequence" event handler
    virtual void itemAsNewAA ( wxCommandEvent &ev ) ; ///< "Make new amino acid sequence from item" event handler
    virtual void rsInfo ( wxCommandEvent &ev ) ; ///< "Go to ReBase web page for this restriction enzyme" event handler
    virtual void rsEdit ( wxCommandEvent &ev ) ; ///< "Edit this restriction enzyme" event handler
    virtual void rsDel ( wxCommandEvent &ev ) ; ///< "Delete this restriction enzyme" event handler
    virtual void rsShowHide ( wxCommandEvent &ev ) ; ///< "Toggle restriction enzyme visibility" event handler
    virtual void rsHideLimit ( wxCommandEvent &ev ) ; ///< "Limit maximum number of cuts per restriction enzyme" event handler
    virtual void rsMark ( wxCommandEvent &ev ) ; ///< "Mark sequence" event handler
    virtual void rsMarkShow ( wxCommandEvent &ev ) ; ///< "Mark and show in sequence display" event handler
    virtual void rsAdd2Cocktail ( wxCommandEvent &ev ) ; ///< "Add to restriction enzyme cocktail" event handler
    virtual void rsAddAndCut ( wxCommandEvent &ev ) ; ///< "Add to restriction enzyme cocktail and cut with it" event handler
    virtual void rsCutWithCocktail ( wxCommandEvent &ev ) ; ///< "Cut with restriction enzyme cocktail" event handler
    virtual void orfCopyDNA ( wxCommandEvent &ev ) ; ///< "Copy open reading frame DNA sequence" event handler
    virtual void orfCopyAA ( wxCommandEvent &ev ) ; ///< "Copy open reading frame amino acid sequence" event handler
    virtual void orfAsNewItem ( wxCommandEvent &ev ) ; ///< "Open reading frame as new item" event handler
    virtual void orfAsNewDNA ( wxCommandEvent &ev ) ; ///< "Open reading frame as new DNA sequence" event handler
    virtual void orfAsNewAA ( wxCommandEvent &ev ) ; ///< "Open reading frame as new amino acid sequence" event handler
    virtual void orfBlastDNA ( wxCommandEvent &ev ) ; ///< "BLAST open reading frame DNA sequence" event handler
    virtual void orfBlastAA ( wxCommandEvent &ev ) ; ///< "BLAST open reading frame amino acid sequence" event handler
    virtual void OnFillKlenow(wxCommandEvent& event); ///< "Klenow-fill sticky ends" event handler
    virtual void OnWhatCuts(wxCommandEvent& event); ///< "List enzymes that cut here" event handler
    virtual void OnRunPCR(wxCommandEvent& event); ///< "Run PCR" event handler
    virtual void vecEdit ( wxCommandEvent &ev ) ; ///< "Edit vector" event handler
    virtual void blastDNA ( wxCommandEvent &ev ) ; ///< "BLAST DNA sequence" event handler
    virtual void blastAA ( wxCommandEvent &ev ) ; ///< "BLAST amino acid sequence" event handler
    virtual void itemShowHide ( wxCommandEvent &ev ) ; ///< "Toggle item visibility" event handler
    virtual void OnTurningPoint ( wxCommandEvent &ev ) ; ///< "Turning point (12 o'clock)" event handler

    virtual void setPrinting ( const bool _b ) ; ///< Sets the printing mode on/off
    virtual void setLastContextItem ( const long _l ) ; ///< Sets the last item the mouse was over

    virtual int getMarkFrom () const ; ///< Returns the first selected position, or -1
    virtual int getMarkTo () const ; ///< Returns the last selected position, or -1
    virtual void getMark ( int &i1 , int &i2 ) const ; ///< Sets i1 and i2 to the first/last selected positions, or to -1
    virtual void setMark ( const int i1 , const int i2 ) ; ///< Sets the selection
    virtual void setRootChild ( MyChild * const _p ) ; ///< Sets the MyChild parent
    virtual int getZoom () const ; ///< Returns the current zoom factor

    private :
    // Event handlers
    virtual void OnDrawLinear(wxDC& dc) ; ///< Draw a linear sequence
    virtual void OnDrawCircular(wxDC& dc) ; ///< Draw a circular sequence
    virtual void OnEvent(wxMouseEvent& event); ///< Mouse event handler

    // circular mode
    virtual int deg2x ( const float& deg , const int& r ) const ; ///< Converts degree/radius to x coordinate
    virtual int deg2y ( const float& deg , const int& r ) const ; ///< Converts degree/radius to y coordinate
    virtual float xy2deg ( const float& x , const float& y ) const ; ///< Converts x/y coordinate to degrees
    virtual float xy2r ( const float& x , const float& y ) const ; ///< Converts x/y coordinate to radius
    virtual int findVectorObjectCircular ( const float& angle , const float& radius ) const ; ///< Returns the number of an item/feature at the given position, or -1
    virtual int findRestrictionSite ( const int x , const int y ) const ; ///< Returns the number of the restriction site at the given position, or -1
    virtual wxPoint makeLastRect ( int a , wxDC &dc ) const ; ///< Some internal function...
    virtual void arrangeRestrictionSitesCircular ( wxDC &dc ) const ; ///< Arranges the restriction site labels
    virtual bool optimizeCircularRestrictionSites ( const int a , wxDC &dc ) const ; ///< Rearranges the restriction site labels
    virtual int findORFcircular ( const float& angle , const float& radius ) const ; ///< Returns the number of the open reading frame at the given position, or -1
    virtual void drawCircularORFs ( wxDC &dc ) const ; ///< Draws open reading frames in circular mode

    // linear mode
    virtual int findVectorObjectLinear ( const wxPoint& pp ) const ; ///< Returns the number of an item/feature at the given position, or -1
    virtual void drawLinearItem ( wxDC &dc , const int r1 , const int r2 , const float a1 , const float a2 , TVectorItem *i ) const ; ///< Draws an item/feature in linear mode
    virtual void arrangeRestrictionSitesLinear ( const int a , wxDC &dc ) const ; ///< Arranges the restriction site labels
    virtual bool intersectsLine ( const wxRect &a , const wxPoint& p ) const ; ///< Does the point lie within the rectangle, except for the top part?
    virtual void drawLinearORFs ( wxDC &dc ) const ; ///< Draws open reading frames in linear mode
    virtual int findORFlinear ( const int x , const int y ) const ; ///< Returns the number of the open reading frame at the given position, or -1

    // both modes
    virtual bool intersects ( const wxRect &a , const wxRect &b ) const ; ///< Does the point lie within the rectangle?
    virtual bool pointinrect ( const int x , const int y , const wxRect &a ) const ; ///< Does the point lie within the rectangle?
    virtual bool isEnzymeVisible ( const wxString& s ) const ; ///< Is the enzyme visible (or was it set to "hidden")?
    virtual void updateLinkedItems ( TVector * const vec , const int in ) ; ///< Update items that belong together (like exons)

    // Misc internal methods
    virtual void SetMyToolTip ( const wxString& s , const int mode ) ; ///< Sets the tool tip
    virtual wxString getDNAorAA ( const int from , const int to , const int dir , const bool dna = true ) const ; ///< Returns the given DNA or amino acid sequence
    virtual int circular_pos ( const float& angle ) const ; ///< Converts an angle to a DNA position in circular mode
    virtual void push_rc_left ( const int a , wxDC &dc ) const ; ///< Internally "push" a restriction site label to the "left"
    virtual void push_rc_right ( const int a , wxDC &dc ) const ; ///< Internally "push" a restriction site label to the "right"
    virtual void recalc_rc ( const int a , wxDC &dc ) const ; ///< Recalculates the position of the restriction cuts and labels
    virtual void makeGCcolor ( const int percent , wxColour &col ) const ; ///< Sets the color appropriate for the percentage of the GC contents
    virtual void showGClegend ( wxDC &dc ) const ; ///< Draws the GC contents legend

    // Variables
    int context_last_rs , lasttooltip , context_last_orf ;
    int lastvectorobject , lastrestrictionsite , lastbp ;
    int mousediffx , mousediffy , zoom , last_rightclick_base ;
    long context_last_item ;
    int r , w , h ;
    bool initialclick , painting , hasBeenPainted , printing ;
    bool captured ;
    wxToolTip *tt ; ///< The current/last tooptip
    MyChild *p ; ///< The MyChild structure containing this PlasmidCanvas

    DECLARE_EVENT_TABLE()
    };

#endif

