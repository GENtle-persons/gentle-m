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

class TDDR // Drag'n'Drop Rect
	{
	public :
	TDDR ( int _type = DDR_NONE ) ;
	~TDDR () ;
	void resizeForText ( wxDC &dc ) ;
	void draw ( wxDC &dc , wxPoint off = wxPoint ( 0 , 0 ) ) ;
	TDDR *findItem ( wxPoint p , wxPoint ori = wxPoint ( 0 , 0 ) , TDDR *match = NULL ) ;
	wxPoint getRealOffset () ;
	void clear_children () ;
	void do_highlight ( wxPoint p ) ;
	
	wxRect r ;
	int draggable ;
	vector <TDDR*> children ;
	TDDR *parent ;
	wxPen pen ;
	wxBrush brush ;
	wxString title ;
	int type ;
	bool dragging ;
	TVector *vector ;
	TVectorItem *item ;
	int highlight ;
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
	TCloningAssistant *ca ;
	TDDR *dragging , *highlight ;
	wxPoint drag_diff , last_dragged_point ;

    DECLARE_EVENT_TABLE()
	} ;

/**	\brief The cloning assistant module
*/
class TCloningAssistant : public ChildBase
    {
    public :
	TCloningAssistant(wxWindow *parent, const wxString& title) ; ///< Constructor
    virtual ~TCloningAssistant () ; ///< Destructor
    virtual wxString getName () ; ///< Returns the module name
    virtual void EnforceRefesh () ; ///< Refreshes the module display

    virtual void initme () ;
    virtual void Refresh (bool eraseBackground = true, const wxRect* rect = NULL) ;
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    virtual void OnDraw(wxDC& pdc) ;
    
    private :
	friend class TCloningAssistantPanel ;
	
	TDDR *new_from_vector ( TVector *v ) ;
	
	vector <TVector*> vectors ;
	TDDR *base , *vlist , *tlist ;
	TCloningAssistantPanel *panel ;

    DECLARE_EVENT_TABLE()
	} ;

#endif
