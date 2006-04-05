#ifndef __CLONING_ASSISTANT_H_
#define __CLONING_ASSISTANT_H_

#include "main.h"

#include "ChildBase.h"

class TVector ;

enum {
	DDR_NONE = 0 ,
	DDR_AS_TARGET ,
	DDR_AS_SEQUENCE ,
} ;

class TCloningAssistant ;

class TDDR // Drag'n'Drop Rect
	{
	public :
	TDDR () ;
	~TDDR () ;
	void draw ( wxDC &dc , wxPoint off = wxPoint ( 0 , 0 ) ) ;
	
	wxRect r ;
	int draggable ;
	vector <TDDR*> children ;
	TDDR *root ;
	wxPen pen ;
	wxBrush brush ;
	wxString title ;
	} ;

class TCloningAssistantPanel : wxPanel
	{
	public :
	TCloningAssistantPanel ( TCloningAssistant *parent ) ;

    virtual void Refresh (bool eraseBackground = true, const wxRect* rect = NULL) ;
    virtual void OnDraw(wxDC& pdc) ;
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    virtual void OnEvent(wxMouseEvent& event) ; ///< Mouse event handler
    	
	private :
	TCloningAssistant *ca ;

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

    virtual void initme () ;
    virtual void Refresh (bool eraseBackground = true, const wxRect* rect = NULL) ;
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    virtual void OnDraw(wxDC& pdc) ;
    
    private :
	friend class TCloningAssistantPanel ;
	vector <TVector*> vectors ;
	TDDR *base , *dragging , *vlist ;
	TCloningAssistantPanel *panel ;

    DECLARE_EVENT_TABLE()
	} ;

#endif
