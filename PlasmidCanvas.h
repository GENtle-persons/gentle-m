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
    
    // circular mode
    virtual int deg2x ( float deg , int r ) ;
    virtual int deg2y ( float deg , int r ) ;
    virtual float xy2deg ( float x , float y ) ;
    virtual float xy2r ( float x , float y ) ;
    virtual int findVectorObject ( float angle , float radius ) ;
    virtual int findRestrictionSite ( int x , int y ) ;
    virtual wxPoint makeLastRect ( int a , wxDC &dc ) ;
    virtual void arrangeRestrictionSite ( int q , wxDC &dc ) ;
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
    virtual bool isEnzymeVisible ( string s ) ;
    
    PlasmidCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDraw(wxDC& pdc);
    virtual void OnDrawLinear(wxDC& dc);
    virtual void OnDrawCircular(wxDC& dc);
    virtual void Refresh () ;
    virtual void print () ;
    virtual void setZoom ( int factor ) ;

    bool IsDirty() const { return m_dirty; }
    string getSelection () ;
    void SetMyToolTip ( string s , int mode ) ;
    void OnCopyImage ( wxCommandEvent &ev ) ;

    void OnEvent(wxMouseEvent& event);
    void OnEventLinear(wxMouseEvent& event);
    void OnEventCircular(wxMouseEvent& event);
    
    void RunPrimerEditor ( vector <TPrimer> &pl ) ;
    void OnPrimerForward ( wxCommandEvent &ev ) ;
    void OnPrimerBackward ( wxCommandEvent &ev ) ;
    void OnPrimerBoth ( wxCommandEvent &ev ) ;
    void OnPrimerMutation ( wxCommandEvent &ev ) ;
    
    void invokeVectorEditor ( string what = "" , int num = 0 , bool forceUpdate = false ) ;
    wxMenu *invokeItemPopup ( int item , wxPoint pt , bool doreturn = false ) ;
    wxMenu *invokeRsPopup ( int rs , wxPoint pt , bool doreturn = false ) ;
    wxMenu *invokeVectorPopup ( wxPoint pt , bool doreturn = false ) ;
    void invokeORFpopup ( int item , wxPoint pt ) ;

    // Popup menu handlers
    void itemMark ( wxCommandEvent &ev ) ;
    void itemMarkShow ( wxCommandEvent &ev ) ;
    void itemEdit ( wxCommandEvent &ev ) ;
    void itemAsNewSequence ( wxCommandEvent &ev ) ;
    void itemDelete ( wxCommandEvent &ev ) ;
    void itemShowHide ( wxCommandEvent &ev ) ;
    void itemBlastDNA ( wxCommandEvent &ev ) ;
    void itemBlastAA ( wxCommandEvent &ev ) ;
    void itemCopyDNA ( wxCommandEvent &ev ) ;
    void itemCopyAA ( wxCommandEvent &ev ) ;
    void itemAsNewAA ( wxCommandEvent &ev ) ;
    void rsInfo ( wxCommandEvent &ev ) ;
    void rsEdit ( wxCommandEvent &ev ) ;
    void rsDel ( wxCommandEvent &ev ) ;
    void rsShowHide ( wxCommandEvent &ev ) ;
    void rsMark ( wxCommandEvent &ev ) ;
    void rsMarkShow ( wxCommandEvent &ev ) ;
    void rsAdd2Cocktail ( wxCommandEvent &ev ) ;
    void rsAddAndCut ( wxCommandEvent &ev ) ;
    void rsCutWithCocktail ( wxCommandEvent &ev ) ;
    void vecEdit ( wxCommandEvent &ev ) ;
    void blastDNA ( wxCommandEvent &ev ) ;
    void blastAA ( wxCommandEvent &ev ) ;
    void orfCopyDNA ( wxCommandEvent &ev ) ;
    void orfCopyAA ( wxCommandEvent &ev ) ;
    void orfAsNewItem ( wxCommandEvent &ev ) ;
    void orfAsNewDNA ( wxCommandEvent &ev ) ;
    void orfAsNewAA ( wxCommandEvent &ev ) ;
    void orfBlastDNA ( wxCommandEvent &ev ) ;
    void orfBlastAA ( wxCommandEvent &ev ) ;
    void OnFillKlenow(wxCommandEvent& event);
    void OnRunPCR(wxCommandEvent& event);

    private :
    string getDNAorAA ( int from , int to , int dir , bool dna = true ) ;
    
    public :

    // Variables
    MyChild *p ;
    bool m_dirty , initialclick , printing , painting , hasBeenPainted ;
    int zoom , r , w , h ;
    int lastvectorobject , lastrestrictionsite , lastbp ;
    int mark_from , mark_to ;
    int mousediffx , mousediffy ;
    int context_last_item , context_last_rs , context_last_orf ;
    int lasttooltip ;
    wxToolTip *tt ;

    DECLARE_EVENT_TABLE()
    };


#endif

