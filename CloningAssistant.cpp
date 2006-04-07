#include "CloningAssistant.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(TCloningAssistantPanel, wxScrolledWindow)
    EVT_MOUSE_EVENTS(TCloningAssistantPanel::OnEvent)
    EVT_SIZE(TCloningAssistantPanel::OnSize)
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

wxPen TDDR::ThickRedPen ;

TCloningAssistant::TCloningAssistant(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
	{
	TDDR::ThickRedPen = wxPen ( *wxRED , 5 ) ;
	def = _T("CloningAssistant") ;
	
	base = new TDDR ;
	vlist = new TDDR ;
	tlist = new TDDR ;
	
	base->r = wxRect ( 0 , 0 , 200 , 200 ) ;
	vlist->r = wxRect ( 10 , 10 , 100 , 100 ) ;
	base->brush = *wxWHITE_BRUSH ;
	
	base->children.push_back ( vlist ) ;
	base->children.push_back ( tlist ) ;

	for ( int a = 0 ; a < 5 ; a++ )
		{
		vectors.push_back ( new TVector ) ;
		tlist->children.push_back ( new_from_vector ( vectors[a] ) ) ;
		}
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
    
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    panel = new TCloningAssistantPanel ( this , this ) ;
	panel->arrange () ;
    v0->Add ( panel , 1 , wxEXPAND ) ;

	// Toolbar
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    toolBar->Reparent ( this ) ;
    toolbar = toolBar ;
    myapp()->frame->InitToolBar(toolBar);
	myapp()->frame->addTool ( toolBar , MDI_TEXT_IMPORT ) ;
	myapp()->frame->addTool ( toolBar , MDI_FILE_OPEN ) ;
    myapp()->frame->addDefaultTools ( toolBar ) ;
    toolBar->Realize() ;
	
	SetSizer ( v0 ) ;
    myapp()->frame->setChild ( this ) ;
    Maximize () ;
	panel->arrange () ;
	
    Activate () ;
	v0->Layout () ;
//	EnforceRefesh () ;
//	Refresh () ;
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

void TCloningAssistant::EnforceRefesh ()
	{
	panel->arrange () ;
	panel->Refresh () ;
	}

TDDR *TCloningAssistant::new_from_vector ( TVector *v )
	{
	TDDR *n = new TDDR ( DDR_AS_SEQUENCE ) ;
	n->title = v->getName() ;
	n->parent = tlist ;
	int a ;
	for ( a = 0 ; a < v->items.size() ; a++ )
		{
		TDDR *m = new TDDR ( DDR_AS_ITEM ) ;
		m->title = v->items[a].name ;
		m->r = wxRect ( 0 , 0 , 10 , 15 ) ;
		m->parent = n ;
		n->children.push_back ( m ) ;
		}
	return n ;
	}

//______________________________________________________________________________

TCloningAssistantPanel::TCloningAssistantPanel ( TCloningAssistant *_ca , wxWindow *parent )
	: wxScrolledWindow ( (wxWindow*) parent , -1 )
	{
	dragging = NULL ;
	highlight = NULL ;
	ca = _ca ;
	}

void TCloningAssistantPanel::OnDraw(wxDC& pdc)
	{
	pdc.Clear () ;
	ca->base->draw ( pdc ) ;
	}

void TCloningAssistantPanel::OnPaint(wxPaintEvent& event)
	{
	wxPaintDC dc(this);
	if ( dragging )
		{
		wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
		OnDraw ( dc ) ;
		}
	else
		{
		OnDraw ( dc ) ;
		}
    }

void TCloningAssistantPanel::Refresh (bool eraseBackground , const wxRect* rect )
	{
	wxClientDC dc ( this ) ;
	wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
	OnDraw ( dc2 ) ;
//	wxPaintEvent ev ;
//	OnPaint ( ev ) ;	
	}

void TCloningAssistantPanel::OnEvent(wxMouseEvent& event)
	{
	wxPaintEvent ev ;
	OnPaint ( ev ) ;	
    wxPoint pt(event.GetPosition());
	
	TDDR *over = NULL ;
	if ( !dragging ) over = ca->base->findItem ( pt ) ;
	
	if ( event.LeftDown() )
		{
		if ( dragging ) dragging->dragging = false ;
		dragging = over ;
		if ( over && over->draggable != DDR_NONE )
			{
			drag_diff = pt - over->getRealOffset() ;
			dragging->dragging = true ;
			}
		}
	else if ( dragging && event.LeftUp() )
		{
		if ( highlight )
			{
			do_drop ( dragging , highlight ) ;
			if ( highlight )
				{
				highlight->highlight = DDR_HIGHLIGHT_NONE ;
				highlight = NULL ;
				}
			dragging->dragging = false ;
			dragging = NULL ;
			Refresh () ;
			return ;
			}
		int max = 25 ;
		for ( int a = 0 ; a <= max ; a++ )
			{
			wxClientDC dc ( this ) ;
			wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
			wxPoint p = dragging->getRealOffset() ;
			p.x -= dragging->r.x ;
			p.y -= dragging->r.y ;
			p.x = ( p.x * a + last_dragged_point.x * ( max - a ) ) / max ;
			p.y = ( p.y * a + last_dragged_point.y * ( max - a ) ) / max ;
			OnDraw ( dc2 ) ;
			dragging->draw ( dc2 , p ) ;
			}
		dragging->dragging = false ;
		dragging = NULL ;
		return ;
		}
	
	if ( dragging && event.LeftIsDown() )
		{
		over = ca->base->findItem ( pt , wxPoint ( 0 , 0 ) , dragging ) ;
		if ( highlight ) highlight->highlight = DDR_HIGHLIGHT_NONE ;
		if ( !over || dragging == highlight ) highlight = NULL ;
		else
			{
			highlight = over ;
			highlight->do_highlight ( pt ) ;
			if ( highlight->highlight == DDR_HIGHLIGHT_NONE )
				highlight = NULL ;
			}

		last_dragged_point = wxPoint ( -dragging->r.x , -dragging->r.y ) ;
		last_dragged_point += pt ;
		last_dragged_point -= drag_diff ;
		wxClientDC dc ( this ) ;
		wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
		OnDraw ( dc2 ) ;
		dragging->draw ( dc2 , last_dragged_point ) ;
		}
	else if ( over && over->draggable != DDR_NONE )
		{
		SetCursor(wxCursor(wxCURSOR_HAND)) ;
		}
	else
		{
		SetCursor(*wxSTANDARD_CURSOR) ;
		}
	
	}

void TCloningAssistantPanel::arrange ()
	{
	int a , b ;
	wxRect r ;
	wxClientDC dc ( this ) ;
	wxSize cs = dc.GetSize() ;
	
	// Base
	ca->base->r = wxRect ( wxPoint ( 0 , 0 ) , GetClientSize() ) ;
	
	// Object list
	r = wxRect ( 5 , 5 , 50 , 15 ) ;
	while ( ca->vlist->children.size() ) ca->vlist->children.pop_back () ;
	int maxw = 0 ;
	for ( a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
		{
		if ( myapp()->frame->children[a]->def != _T("dna") ) continue ;
		if ( !myapp()->frame->children[a]->vec ) continue ;
		TDDR *x = new TDDR ;
		x->title = myapp()->frame->children[a]->getName() ;
		x->r = r ;
		x->vector = myapp()->frame->children[a]->vec ;
		x->draggable = DDR_AS_SEQUENCE ;
		x->parent = ca->vlist ;
		ca->vlist->children.push_back ( x ) ;
		x->resizeForText ( dc ) ;
		r = x->r ;
		if ( r.width > maxw ) maxw = r.width ;
		r.SetY ( r.GetBottom() + 5 ) ;
		}
	for ( a = 0 ; a < ca->vlist->children.size() ; a++ )
		ca->vlist->children[a]->r.width = maxw ;
	ca->vlist->r = wxRect ( 5 , 5 , maxw + 10 , ca->base->r.GetHeight() - 10 ) ;
	
	// Targets
	ca->tlist->r = wxRect ( ca->vlist->r.GetRight() + 5 , 
							ca->vlist->r.GetTop() ,
							cs.GetWidth() - ca->vlist->r.GetRight() - 10 , 
							ca->vlist->r.GetHeight() ) ;
	for ( a = 0 ; a < ca->tlist->children.size() ; a++ )
		{
		TDDR *i = ca->tlist->children[a] ;
		i->r = wxRect (
			5 ,
			ca->tlist->r.GetHeight() * a / ca->tlist->children.size() + 5 ,
			ca->tlist->r.GetWidth() - 10 ,
			ca->tlist->r.GetHeight() / ca->tlist->children.size() - 10
			) ;
		
		int lastx = 5 ;
		for ( b = 0 ; b < i->children.size() ; b++ )
			{
			i->children[b]->resizeForText ( dc ) ;
			i->children[b]->r.x = lastx + 5 ;
			i->children[b]->r.y = 15 ;
			lastx += i->children[b]->r.width + 5 ;
			}
		}
	}

void TCloningAssistantPanel::OnSize (wxSizeEvent& event)
	{
	arrange () ;
	Refresh () ;
	}

void TCloningAssistantPanel::do_drop ( TDDR *source , TDDR *target )
	{
	int a ;
	if ( ( source->dragging & target->type & DDR_AS_SEQUENCE ) > 0 ) // Sequence
		{
		for ( a = 0 ; a < ca->tlist->children.size() ; a++ )
			{
			if ( ca->tlist->children[a] != target ) continue ;
			TVector *nv = new TVector ;
			nv->setFromVector ( *(source->vector) ) ;
			ca->vectors[a] = nv ;
			delete ca->tlist->children[a] ;
			ca->tlist->children[a] = ca->new_from_vector ( nv ) ;
			ca->tlist->children[a]->parent = ca->tlist ;
			break ;
			}
		highlight = NULL ;
		}
	}

//______________________________________________________________________________

TDDR::TDDR ( int _type )
	{
	draggable = DDR_NONE ;
	pen = *wxBLACK_PEN ;
	brush = *wxTRANSPARENT_BRUSH ;
	type = _type ;
	parent = NULL ;
	dragging = false ;
	vector = NULL ;
	item = NULL ;
	highlight = DDR_HIGHLIGHT_NONE ;
	}

TDDR::~TDDR ()
	{
	clear_children () ;
	}

void TDDR::clear_children ()
	{
	for ( int a = 0 ; a < children.size() ; a++ )
		delete children[a] ;
	children.clear () ;
	}

void TDDR::draw ( wxDC &dc , wxPoint off )
	{
	if ( highlight == DDR_HIGHLIGHT_AS ) dc.SetPen ( ThickRedPen ) ;
	else dc.SetPen ( pen ) ;
	dc.SetBrush ( brush ) ;
	dc.DrawRectangle ( r.GetLeft() + off.x , r.GetTop() + off.y , 
						r.GetWidth() , r.GetHeight() ) ;
	wxPoint p = wxPoint ( r.GetLeft() + off.x , r.GetTop() + off.y ) ;
	if ( !title.IsEmpty() ) dc.DrawText ( title , p.x + 2 , p.y + 2 ) ;
	for ( int a = 0 ; a < children.size() ; a++ )
		{
		if ( !children[a]->dragging ) children[a]->draw ( dc , p ) ;
		}
	
	if ( highlight == DDR_HIGHLIGHT_NONE ) return ;
	dc.SetPen ( ThickRedPen ) ;
	int x ;
	if ( highlight == DDR_HIGHLIGHT_LEFT ) x = r.GetLeft() + off.x - 2 ;
	else if ( highlight == DDR_HIGHLIGHT_RIGHT ) x = r.GetRight() + off.x + 2 ;
	else return ;
	dc.DrawLine ( x , r.GetTop() + off.y , x , r.GetBottom() + off.y ) ;
	dc.DrawLine ( x-5 , r.GetTop() + off.y-5, x , r.GetTop() + off.y ) ;
	dc.DrawLine ( x+5 , r.GetTop() + off.y-5 , x , r.GetTop() + off.y ) ;
	dc.DrawLine ( x-5 , r.GetBottom() + off.y+5, x , r.GetBottom() + off.y ) ;
	dc.DrawLine ( x+5 , r.GetBottom() + off.y+5 , x , r.GetBottom() + off.y ) ;
	}

void TDDR::resizeForText ( wxDC &dc )
	{
	long w , h ;
	dc.GetTextExtent ( title , &w , &h ) ;
	r.SetWidth ( w + 4 ) ;
	r.SetHeight ( h + 4 ) ;
	}

TDDR *TDDR::findItem ( wxPoint p , wxPoint ori , TDDR *match )
	{
	if ( dragging ) return NULL ;
	wxRect rr = r ;
	rr.x += ori.x ;
	rr.y += ori.y ;
	if ( !rr.Inside ( p ) ) return NULL ;
	wxPoint p2 ( rr.x , rr.y ) ;
	for ( int a = 0 ; a < children.size() ; a++ )
		{
		TDDR *ret = children[a]->findItem ( p , p2 , match ) ;
		if ( ret ) return ret ;
		}
	if ( match && ( type & match->draggable ) > 0 ) return this ;
	if ( match ) return NULL ;
	if ( draggable == DDR_NONE ) return NULL ;
	return this ;
	}

wxPoint TDDR::getRealOffset ()
	{
	wxPoint p ( 0 , 0 ) ;
	if ( parent ) p = parent->getRealOffset () ;
	p.x += r.x ;
	p.y += r.y ;
	return p ;
	}

void TDDR::do_highlight ( wxPoint p )
	{
	if ( ( type & DDR_AS_SEQUENCE ) > 0 )
		{
		highlight = DDR_HIGHLIGHT_AS ;
		return ;
		}
	wxPoint ro = getRealOffset () ;
	wxRect rl = wxRect ( ro.x , ro.y , r.GetWidth() / 10 , r.GetHeight() ) ;
	wxRect rr = wxRect ( ro.x + r.GetWidth() * 9 / 10 , ro.y , r.GetWidth() / 10 + 1 , r.GetHeight() ) ;
	if ( rl.Inside ( p ) ) highlight = DDR_HIGHLIGHT_LEFT ;
	else if ( rr.Inside ( p ) ) highlight = DDR_HIGHLIGHT_RIGHT ;
	else highlight = DDR_HIGHLIGHT_AS ;
	}
