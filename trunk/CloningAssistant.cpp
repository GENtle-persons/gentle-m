#include "CloningAssistant.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(TCloningAssistantPanel, wxPanel)
    EVT_MOUSE_EVENTS(TCloningAssistantPanel::OnEvent)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TCloningAssistant, MyChildBase)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)

    // Dummies
    EVT_MENU(MDI_COPY,ChildBase::OnDummy)
    EVT_MENU(MDI_FILE_SAVE,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_FEATURES,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_UNDO,ChildBase::OnDummy)
    EVT_MENU(MDI_CUT,ChildBase::OnDummy)
    EVT_MENU(MDI_PASTE,ChildBase::OnDummy)
    EVT_MENU(MDI_EDIT_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_EXPORT,ChildBase::OnDummy)
    EVT_MENU(MDI_MARK_ALL,ChildBase::OnDummy)
    EVT_MENU(MDI_FIND,ChildBase::OnDummy)
    EVT_MENU(AA_NONE,TABIviewer::OnDummy)
    EVT_MENU(AA_KNOWN, TABIviewer::OnDummy)
    EVT_MENU(AA_ALL, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE, TABIviewer::OnDummy)
    EVT_MENU(AA_ONE, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_3, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M3, TABIviewer::OnDummy)
END_EVENT_TABLE()

TCloningAssistant::TCloningAssistant(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
	{
	def = _T("CloningAssistant") ;
	
	dragging = NULL ;
	base = new TDDR ;
	vlist = new TDDR ;
	
	base->r = wxRect ( 0 , 0 , 200 , 200 ) ;
	vlist->r = wxRect ( 0 , 0 , 100 , 100 ) ;
	
	base->children.push_back ( vlist ) ;
	}

TCloningAssistant::~TCloningAssistant ()
	{
	}

void TCloningAssistant::initme ()
	{
    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    SetMenuBar(menu_bar);
    
    panel = new TCloningAssistantPanel ( this ) ;

	// Toolbar
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    toolBar->Reparent ( this ) ;
    toolbar = toolBar ;
    myapp()->frame->InitToolBar(toolBar);
	myapp()->frame->addTool ( toolBar , MDI_TEXT_IMPORT ) ;
	myapp()->frame->addTool ( toolBar , MDI_FILE_OPEN ) ;
    myapp()->frame->addDefaultTools ( toolBar ) ;
    toolBar->Realize() ;
	
    myapp()->frame->setChild ( this ) ;
    Maximize () ;
    Activate () ;
    Refresh () ;
	}

wxString TCloningAssistant::getName ()
	{
	return txt("t_cloning_assistant") ;
	}

void TCloningAssistant::Refresh (bool eraseBackground , const wxRect* rect )
	{
	panel->Refresh () ;
	}

void TCloningAssistant::OnPaint(wxPaintEvent& event)
	{
	panel->Refresh () ;
	}

void TCloningAssistant::OnDraw(wxDC& pdc)
	{
	panel->OnDraw ( pdc ) ;
	}
//______________________________________________________________________________

TCloningAssistantPanel::TCloningAssistantPanel ( TCloningAssistant *parent )
	: wxPanel ( (wxWindow*) parent , -1 )
	{
	ca = parent ;
//	Maximize () ;
	}

void TCloningAssistantPanel::OnDraw(wxDC& pdc)
	{
	wxMessageBox ( "!" ) ;
	ca->base->draw ( pdc ) ;
	pdc.SetPen ( *wxBLACK_PEN ) ;
	pdc.DrawLine ( 0 , 0 , 1000 , 1000 ) ;
	}

void TCloningAssistantPanel::OnPaint(wxPaintEvent& event)
	{
	wxPaintDC dc(this);
//	wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
	OnDraw ( dc ) ;
    }

void TCloningAssistantPanel::Refresh (bool eraseBackground , const wxRect* rect )
	{
	wxClientDC dc ( this ) ;
	OnDraw ( dc ) ;
//	wxPaintEvent ev ;
//	OnPaint ( ev ) ;	
	}

void TCloningAssistantPanel::OnEvent(wxMouseEvent& event)
	{
	wxPaintEvent ev ;
	OnPaint ( ev ) ;	
	}

//______________________________________________________________________________

TDDR::TDDR ()
	{
	draggable = DDR_NONE ;
	pen = *wxBLACK_PEN ;
	brush = *wxTRANSPARENT_BRUSH ;
	}

TDDR::~TDDR ()
	{
	for ( int a = 0 ; a < children.size() ; a++ )
		delete children[a] ;
	}

void TDDR::draw ( wxDC &dc , wxPoint off )
	{
	dc.SetPen ( *wxBLACK_PEN ) ;
	dc.SetBrush ( *wxWHITE_BRUSH ) ;
	dc.DrawLine ( 0 , 0 , 100 , 100 ) ;
	dc.DrawRectangle ( r.GetLeft() + off.x , r.GetTop() + off.y , 
						r.GetRight() + off.x , r.GetBottom() + off.y ) ;
	wxPoint p = wxPoint ( r.GetLeft() + off.x , r.GetTop() + off.y ) ;
	for ( int a = 0 ; a < children.size() ; a++ )
		children[a]->draw ( dc , p ) ;
	}
