#ifndef _PlasmidCanvas_H_
#define _PlasmidCanvas_H_

#include "main.h"
#include <wx/tooltip.h>
#include <wx/metafile.h>

#define STANDARDRADIUS 10000

#define TT_NONE 0
#define TT_RS 1
#define TT_ITEM 2
#define TT_ORF 3

class MyChild ;
class TVectorItem ;
class TPrimer ;

class PlasmidCanvas : public wxScrolledWindow
    {
    public:
    PlasmidCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual ~PlasmidCanvas () ;
    virtual void OnDraw(wxDC& pdc);
    virtual void Refresh () ;
    virtual void print () ;
    virtual void setZoom ( int factor ) ;

    virtual wxString getSelection () ;
    virtual void OnCopyImage ( wxCommandEvent &ev ) ;

    virtual void RunPrimerEditor ( vector <TPrimer> &pl , int mut = -1 ) ;
    virtual void OnPrimerForward ( wxCommandEvent &ev ) ;
    virtual void OnPrimerBackward ( wxCommandEvent &ev ) ;
    virtual void OnPrimerBoth ( wxCommandEvent &ev ) ;
    virtual void OnPrimerMutation ( wxCommandEvent &ev ) ;
    
    virtual void invokeVectorEditor ( wxString what = "" , int num = 0 , bool forceUpdate = false ) ;
    virtual wxMenu *invokeItemPopup ( int item , wxPoint pt , bool doreturn = false ) ;
    virtual wxMenu *invokeRsPopup ( int rs , wxPoint pt , bool doreturn = false ) ;
    virtual wxMenu *invokeVectorPopup ( wxPoint pt , bool doreturn = false ) ;
    virtual void invokeORFpopup ( int item , wxPoint pt ) ;

    // Popup menu handlers
    virtual void itemMark ( wxCommandEvent &ev ) ;
    virtual void itemMarkShow ( wxCommandEvent &ev ) ;
    virtual void itemEdit ( wxCommandEvent &ev ) ;
    virtual void itemAsNewSequence ( wxCommandEvent &ev ) ;
    virtual void itemDelete ( wxCommandEvent &ev ) ;
    virtual void itemBlastDNA ( wxCommandEvent &ev ) ;
    virtual void itemBlastAA ( wxCommandEvent &ev ) ;
    virtual void itemCopyDNA ( wxCommandEvent &ev ) ;
    virtual void itemCopyAA ( wxCommandEvent &ev ) ;
    virtual void itemAsNewAA ( wxCommandEvent &ev ) ;
    virtual void rsInfo ( wxCommandEvent &ev ) ;
    virtual void rsEdit ( wxCommandEvent &ev ) ;
    virtual void rsDel ( wxCommandEvent &ev ) ;
    virtual void rsShowHide ( wxCommandEvent &ev ) ;
    virtual void rsHideLimit ( wxCommandEvent &ev ) ;
    virtual void rsMark ( wxCommandEvent &ev ) ;
    virtual void rsMarkShow ( wxCommandEvent &ev ) ;
    virtual void rsAdd2Cocktail ( wxCommandEvent &ev ) ;
    virtual void rsAddAndCut ( wxCommandEvent &ev ) ;
    virtual void rsCutWithCocktail ( wxCommandEvent &ev ) ;
    virtual void orfCopyDNA ( wxCommandEvent &ev ) ;
    virtual void orfCopyAA ( wxCommandEvent &ev ) ;
    virtual void orfAsNewItem ( wxCommandEvent &ev ) ;
    virtual void orfAsNewDNA ( wxCommandEvent &ev ) ;
    virtual void orfAsNewAA ( wxCommandEvent &ev ) ;
    virtual void orfBlastDNA ( wxCommandEvent &ev ) ;
    virtual void orfBlastAA ( wxCommandEvent &ev ) ;
    virtual void OnFillKlenow(wxCommandEvent& event);
    virtual void OnWhatCuts(wxCommandEvent& event);    
    virtual void OnRunPCR(wxCommandEvent& event);
    virtual void vecEdit ( wxCommandEvent &ev ) ;
    virtual void blastDNA ( wxCommandEvent &ev ) ;
    virtual void blastAA ( wxCommandEvent &ev ) ;
    virtual void itemShowHide ( wxCommandEvent &ev ) ;

    virtual void setPrinting ( bool _b ) ;
    virtual void setLastContextItem ( int _i ) ;
    
    virtual int getMarkFrom () ;
    virtual int getMarkTo () ;
    virtual void setMarkFrom ( int _i ) ;
    virtual void setMarkTo ( int _i ) ;
    virtual void getMark ( int &i1 , int &i2 ) ;
    virtual void setMark ( int i1 , int i2 ) ;
    virtual void setRootChild ( MyChild *_p ) ;
    virtual int getZoom () ;

    private :
    // Event handlers
    virtual void OnDrawLinear(wxDC& dc);
    virtual void OnDrawCircular(wxDC& dc);
    virtual void OnEvent(wxMouseEvent& event);
    virtual void OnEventLinear(wxMouseEvent& event);
    virtual void OnEventCircular(wxMouseEvent& event);

    // circular mode
    virtual int deg2x ( float deg , int r ) ;
    virtual int deg2y ( float deg , int r ) ;
    virtual float xy2deg ( float x , float y ) ;
    virtual float xy2r ( float x , float y ) ;
    virtual int findVectorObject ( float angle , float radius ) ;
    virtual int findRestrictionSite ( int x , int y ) ;
    virtual wxPoint makeLastRect ( int a , wxDC &dc ) ;
    virtual void arrangeRestrictionSitesCircular ( wxDC &dc ) ;
    virtual bool optimizeCircularRestrictionSites ( int a , wxDC &dc ) ;
    virtual int findORFcircular ( float angle , float radius ) ;
    virtual void drawCircularORFs ( wxDC &dc ) ;
    
    // linear mode
    virtual void drawLinearItem ( wxDC &dc , int r1 , int r2 , float a1 , float a2 , TVectorItem *i ) ;
    virtual void arrangeRestrictionSitesLinear ( int a , wxDC &dc ) ;
    virtual bool intersectsLine ( wxRect &a , wxPoint p ) ;
    virtual void drawLinearORFs ( wxDC &dc ) ;
    virtual int findORFlinear ( int x , int y ) ;
    
    // both modes
    virtual bool intersects ( wxRect &a , wxRect &b ) ;
    virtual bool pointinrect ( int x , int y , wxRect &a ) ;
    virtual bool isEnzymeVisible ( wxString s ) ;
    virtual void updateLinkedItems ( TVector *vec , int in ) ;
    
    // Misc internal methods
    virtual void SetMyToolTip ( wxString s , int mode ) ;
    virtual wxString getDNAorAA ( int from , int to , int dir , bool dna = true ) ;
    virtual int circular_pos ( float angle ) ;
    virtual void push_rc_left ( int a , wxDC &dc ) ;
    virtual void push_rc_right ( int a , wxDC &dc ) ;
    virtual void recalc_rc ( int a , wxDC &dc ) ;
    virtual void makeGCcolor ( int percent , wxColour &col ) ;
    virtual void showGClegend ( wxDC &dc ) ;
    
    // Variables
    int context_last_rs , lasttooltip , context_last_orf ;
    int lastvectorobject , lastrestrictionsite , lastbp ;
    int mousediffx , mousediffy , zoom , context_last_item ;
    int r , w , h ;
    bool initialclick , painting , hasBeenPainted , printing ;
    int mark_from , mark_to ;
    wxToolTip *tt ;
    MyChild *p ;
    
    DECLARE_EVENT_TABLE()
    };
    
//#include "CoolCanvas.h"


#endif

