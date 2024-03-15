#ifndef __CLONING_ASSISTANT_H_
#define __CLONING_ASSISTANT_H_

#include "main.h"

#include "ChildBase.h"

class TVector ;

enum {
    DDR_NONE = 0 ,
    DDR_AS_SEQUENCE = 1 ,
    DDR_AS_ITEM = 2 ,
} ;

enum {
    DDR_HIGHLIGHT_NONE = 0 ,
    DDR_HIGHLIGHT_AS ,
    DDR_HIGHLIGHT_LEFT ,
    DDR_HIGHLIGHT_RIGHT
} ;

class TCloningAssistant ;
class TVector ;
class TVectorItem ;
class TCloningAssistantPanel ;

class TDDR_Timer : public wxTimer
    {
    public :
    virtual void Notify() ;
    TCloningAssistantPanel *cap ;
    int move_back , max ;
    } ;

class TDDR ;
typedef vector <TDDR*> VDDR ;

class TDDR // Drag'n'Drop Rect
    {
    public :
    TDDR ( const int _type = DDR_NONE ) ;
    ~TDDR () ;
    void resizeForText ( wxDC &dc ) ;
    void draw ( wxDC &dc , const wxPoint& off = wxPoint ( 0 , 0 ) ) ;
    TDDR *findItem ( const wxPoint& p , const wxPoint& ori = wxPoint ( 0 , 0 ) , TDDR *match = NULL ) ;
    wxPoint getRealOffset () const ;
    void clear_children () ;
    void do_highlight ( const wxPoint& p ) ;
    void duplicate_from ( const TDDR * const b ) ;
    void insert_new_child ( TDDR * const i , TDDR * const t , const bool before ) ;

    wxRect r ;
    int draggable ;
    VDDR children ;
    TDDR *parent , *original ;
    wxPen pen ;
    wxBrush brush ;
    wxString title ;
    int type ;
    bool dragging ;
    TVector *vector ;
    TVectorItem *item ;
    int highlight ;
    static wxPen ThickRedPen ;
    } ;

class TCloningAssistantPanel : public wxScrolledWindow
    {
    public :
    TCloningAssistantPanel ( TCloningAssistant *_ca , wxWindow *parent ) ;

    virtual void Refresh (bool eraseBackground = true, const wxRect* rect = NULL) ;
    virtual void OnDraw(wxDC& pdc) ;
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    virtual void OnEvent(wxMouseEvent& event) ; ///< Mouse event handler
    virtual void OnSize (wxSizeEvent& event) ;

    void arrange () ;

    private :
    friend class TDDR_Timer ;
    void do_drop ( TDDR *source , TDDR *target ) ;

    TDDR_Timer timer ;
    TCloningAssistant *ca ;
    TDDR *dragging , *highlight ;
    wxPoint drag_diff , last_dragged_point ;

    DECLARE_EVENT_TABLE()
    } ;

/** \brief The cloning assistant module
*/
class TCloningAssistant : public ChildBase
    {
    public :
    TCloningAssistant(wxWindow * const parent, const wxString& title) ; ///< Constructor
    virtual ~TCloningAssistant () ; ///< Destructor
    virtual wxString getName () const ; ///< Returns the module name
    virtual void EnforceRefesh () ; ///< Refreshes the module display

    virtual void initme () ;
    virtual void Refresh (const bool eraseBackground = true, const wxRect* rect = NULL) ;
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    virtual void OnDraw(wxDC& pdc) ;

    private :
    friend class TCloningAssistantPanel ;

    TDDR *new_from_vector ( TVector * const v , const int drag = DDR_NONE ) const ;

    vector <TVector*> vectors ;
    TDDR *base , *vlist , *tlist ;
    TCloningAssistantPanel *panel ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
