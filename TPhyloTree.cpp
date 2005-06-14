#include "TPhyloTree.h"

BEGIN_EVENT_TABLE(TPhyloTree, MyChildBase)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_SIZE(ChildBase::OnSize)
    

    // Dummies
    EVT_MENU(MDI_TOGGLE_FEATURES,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_UNDO,ChildBase::OnDummy)
    EVT_MENU(MDI_CUT,ChildBase::OnDummy)
    EVT_MENU(MDI_COPY,ChildBase::OnDummy)
    EVT_MENU(MDI_PASTE,ChildBase::OnDummy)
    EVT_MENU(MDI_EDIT_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_EXPORT,ChildBase::OnDummy)
    EVT_MENU(MDI_MARK_ALL,ChildBase::OnDummy)
    EVT_MENU(MDI_FILE_SAVE,ChildBase::OnDummy)
    EVT_MENU(MDI_FIND,ChildBase::OnDummy)
    EVT_MENU(AA_NONE,ChildBase::OnDummy)
    EVT_MENU(AA_KNOWN, ChildBase::OnDummy)
    EVT_MENU(AA_ALL, ChildBase::OnDummy)
    EVT_MENU(AA_THREE, ChildBase::OnDummy)
    EVT_MENU(AA_ONE, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_1, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_2, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_3, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M1, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M2, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M3, ChildBase::OnDummy)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TPhyloTreeBox, wxControl)
    EVT_PAINT(TPhyloTreeBox::OnPaint)

    EVT_MOUSE_EVENTS(TPhyloTreeBox::OnEvent)
    EVT_MENU(IV_MENU_SAVE_AS_BITMAP, TPhyloTreeBox::OnSaveAsBitmap)
    EVT_MENU(IV_MENU_COPY, TPhyloTreeBox::OnCopy)
    EVT_MENU(IV_MENU_PRINT, TPhyloTreeBox::OnPrint)
END_EVENT_TABLE()


TPhyloTree::TPhyloTree (wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
	{
	def = _T("PHYLOTREE") ;
	vec = NULL ;
	tree = NULL ;
	}
	
void TPhyloTree::initme ()
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

	// Tooblar
	wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
	myapp()->frame->InitToolBar(toolBar);
	toolBar->AddTool( MDI_TEXT_IMPORT , 
	          myapp()->frame->bitmaps[0] ,
	          txt("m_new_sequence") ) ;
	toolBar->AddTool( MDI_FILE_OPEN, 
	      myapp()->frame->bitmaps[1] ,
	      txt("m_open") , txt("m_opentxt") );
	toolBar->AddTool( MDI_FILE_SAVE, 
	          myapp()->frame->bitmaps[2] ,
	          txt("m_store_in_db") , 
	          txt("m_txt_store_in_db"));
	toolBar->AddTool( SEQ_PRINT, 
	          myapp()->frame->bitmaps[16],
	          txt("m_print_sequence") ) ;
	toolBar->AddSeparator() ;
	toolBar->AddTool( MDI_COPY, myapp()->frame->bitmaps[5] ) ;
	toolBar->AddSeparator() ;
	myapp()->frame->addDefaultTools ( toolBar ) ;
	toolBar->Realize() ;
	toolbar = toolBar ;

	// Phylo-Box
	box = new TPhyloTreeBox ( this , PHYLOBOX ) ;

	wxBoxSizer *vs = new wxBoxSizer ( wxVERTICAL ) ;
	vs->Add ( toolBar , 0 , wxBOTTOM , 5 ) ;
	vs->Add ( box , 1 , wxEXPAND , 0 ) ;

	myapp()->frame->setChild ( this ) ;
	this->SetSizer ( vs ) ;
	vs->Fit ( this ) ;    
	this->Show () ;
	}

wxString TPhyloTree::getName()
	{
	return _T("Phylogenetic tree") ;
	}
	
void TPhyloTree::setNewickTree ( wxString s )
	{
	tree = new TPTree ;
	tree->scanNewick ( s ) ;
	tree->setWeight ( 0 ) ;
	}
	
void TPhyloTree::setRealNames ( TAlignment *ali )
	{
	int a , b ;
	vector <TPTree*> vt ;
	tree->getAll ( vt ) ;
	
	for ( a = 0 ; a < vt.size() ; a++ )
		{
		wxString vn = vt[a]->getName().Trim().Trim(true) ;
		if ( vn == "" ) continue ;
		for ( b = 0 ; b < ali->lines.size() ; b++ )
			{
			if ( ali->lines[b].phylip_id == vn )
				{
				vt[a]->setName ( ali->lines[b].name ) ;
				break ;
				}
			}
		}	
	}


void TPhyloTree::setModeStrange ()
	{
	wxClientDC dc ( box ) ;
	wxRect r = box->GetClientSize () ;

	int md = tree->getMaxDepth() ;
	int a , b ;
	for ( a = 0 ; a < md ; a++ )
		{
		vector <TPTree*> vt ;
		tree->filterDepth ( a+1 , vt ) ;
		for ( b = 0 ; b < vt.size() ; b++ )
			{
			int tw = -5 , th = -5 ;
			if ( vt[b]->isLeaf() ) dc.GetTextExtent ( vt[b]->getName() , &tw , &th ) ;
			wxRect r2 (	r.GetLeft() + r.GetWidth() * a / md ,
							r.GetTop() + r.GetHeight() * b / vt.size() ,
							tw + 5 ,
							th + 5 ) ;
			vt[b]->rect = r2 ;
			}
		}

	mode = "STRANGE" ;
	dc.DrawRectangle ( r ) ;
	tree->drawRecursive ( dc , mode ) ;
	}

void TPhyloTree::setModeDrawgram ()
	{
	wxClientDC dc ( box ) ;
	wxRect r ( 0 , 0 , 0 , 0 ) ;
	dc.GetSize ( &r.width , &r.height ) ;

	int md = tree->getMaxDepth() ;
	int mw = tree->getMaxWeight() ;
	int mc = tree->countLeafs () ;
	int border = 5 ;
	
	vector <TPTree*> vt ;
	tree->getAll ( vt ) ;

	int a , b , maxw = 0 ;
	for ( int n = 0 ; n < 2 ; n++ )
		{
		for ( a = b = 0 ; a < vt.size() ; a++ )
			{
			if ( !vt[a]->isLeaf() ) continue ;
			int tw , th ;
			dc.GetTextExtent ( vt[a]->getName() , &tw , &th ) ;
			wxRect r2 ( r.GetLeft() + vt[a]->getCurrentWeight() * ( r.GetWidth() - n * maxw ) / mw ,
							r.GetTop() + r.GetHeight() * b / mc ,
							tw + border ,
							th + border ) ;
			vt[a]->rect = r2 ;
			if ( r2.GetWidth() > maxw ) maxw = r2.GetWidth() ;
			b++ ;	
			}
		}

	tree->averageY () ;
	for ( a = 0 ; a < vt.size() ; a++ )
		{
		if ( vt[a]->isLeaf() ) continue ;
		vt[a]->rect.SetHeight ( 0 ) ;
		vt[a]->rect.SetWidth ( 0 ) ;
		vt[a]->rect.SetX ( r.GetLeft() + vt[a]->getCurrentWeight() * ( r.GetWidth() - maxw ) / mw ) ;
		}

	mode = "DRAWGRAM" ;
	dc.DrawRectangle ( r ) ;
	tree->drawRecursive ( dc , mode ) ;
	}



//******************************************************************************

TPhyloTreeBox::TPhyloTreeBox ( wxWindow *parent , int id )
    : wxControl ( parent , id )
	{
	_parent = (TPhyloTree*) parent ;
	}

void TPhyloTreeBox::OnDraw(wxDC& dc)
	{
	if ( !_parent->tree ) return ;
	wxRect r ( 0 , 0 , 0 , 0 ) ;
	dc.GetSize ( &r.width , &r.height ) ;
	dc.DrawRectangle ( r ) ;
	_parent->tree->drawRecursive ( dc , _parent->mode ) ;
	}

void TPhyloTreeBox::OnPaint(wxPaintEvent& event)
	{
	wxPaintDC dc(this);
	OnDraw ( dc ) ;
	}
	
void TPhyloTreeBox::OnEvent(wxMouseEvent& event)
	{
	}

void TPhyloTreeBox::OnSaveAsBitmap(wxCommandEvent &event)
	{
	}

void TPhyloTreeBox::OnCopy(wxCommandEvent &event)
	{
	}

void TPhyloTreeBox::OnPrint(wxCommandEvent &event)
	{
	}

//******************************************************************************

TPTree::TPTree ()
	{
	weight = 1 ;
	parent = NULL ;
	leafNumber = 0 ;
	}

TPTree::~TPTree ()
	{
	int a ;
	for ( a = 0 ; a < children.size() ; a++ )
		delete children[a] ;
	}

wxString TPTree::scanNewick ( wxString s )
	{
	if ( s.Left ( 1 ) == "(" )
		{
		s = s.Mid ( 1 ) ;
		while ( s.Left ( 1 ) != ")" )
			{
			TPTree *t = new TPTree ;
			s = t->scanNewick ( s ) ;
			t->parent = this ;
			children.push_back ( t ) ;
			}
		s = s.Mid ( 1 ) ;
		if ( s.Left ( 1 ) == ":" )
			{
			s = s.Mid ( 1 ) ;
			wxString w ;
			while ( s.Left ( 1 ) != ")" && s.Left ( 1 ) != "," )
				{
				w += s.Left ( 1 ) ;
				s = s.Mid ( 1 ) ;
				}
			w.ToDouble ( &weight ) ;
			}
		if ( s.Left ( 1 ) == "," ) s = s.Mid ( 1 ) ;
		return s ;
		}
	else
		{
		wxString n , w ;
		while ( !s.IsEmpty() && s.Left(1) != "," && s.Left(1) != ")" )
			{
			n += s.Left ( 1 ) ;
			s = s.Mid ( 1 ) ;
			}
		w = n.AfterFirst ( ':' ) ;
		n = n.BeforeFirst ( ':' ) ;
		if ( w.IsEmpty() ) w = "1" ;
		name = n.Trim().Trim(false) ;
		w.ToDouble ( &weight ) ;
		if ( s.Left ( 1 ) == "," ) s = s.Mid ( 1 ) ;
		return s ;
		}
	}

double TPTree::getMaxWeight ()
	{
	double x = 0 ;
	for ( int a = 0 ; a < children.size() ; a++ )
		{
		double y = children[a]->getMaxWeight() ;
		if ( y > x ) x = y ;
		}
	return x + weight ;
	}

double TPTree::getCurrentWeight ()
	{
	if ( parent ) return weight + parent->getCurrentWeight() ;
	else return weight ;
	}

int TPTree::getMaxDepth ()
	{
	int r = getCurrentDepth() ;
	for ( int a = 0 ; a < children.size() ; a++ )
		{
		int x = children[a]->getMaxDepth() ;
		if ( x > r ) r = x ;
		}	
	return r ;
	}

int TPTree::getCurrentDepth ()
	{
	return parent ? parent->getCurrentDepth() + 1 : 1 ;
	}

int TPTree::countLeafs ()
	{
	if ( isLeaf () ) return 1 ;
	int cnt = 0 ;
	for ( int a = 0 ; a < children.size() ; a++ )
		cnt += children[a]->countLeafs() ;
	return cnt ;
	}

int TPTree::numberLeafs ( int i )
	{
	if ( isLeaf () )
		{
		leafNumber = ++i ;
		return i ;
		}
	
	// Not a leaf
	leafNumber = 0 ;
	int cnt = 0 ;
	for ( int a = 0 ; a < children.size() ; a++ )
		i = children[a]->numberLeafs ( i ) ;
	return i ;
	}

void TPTree::drawRecursive ( wxDC &dc , wxString mode )
	{	
	for ( int a = 0 ; a < children.size() ; a++ )
		{
		wxPoint p1 ( rect.GetRight() , ( rect.GetTop() + rect.GetBottom() ) / 2 ) ;
		wxPoint p2 ( children[a]->rect.GetLeft() , ( children[a]->rect.GetTop() + children[a]->rect.GetBottom() ) / 2 ) ;
		if ( mode.Find ( "[DIRECTLINE]" ) != -1 ) dc.DrawLine ( p1 , p2 ) ;
		else
			{
			wxPoint p1a ( p1.x * 11 / 10 , p1.y ) ;
			wxPoint p1b ( p1.x * 11 / 10 , ( p1.y + p2.y ) / 2 ) ;
			wxPoint p1c ( p2.x * 9 / 10 , ( p1.y + p2.y ) / 2 ) ;
			wxPoint p1d ( p2.x * 9 / 10 , p2.y ) ;
			dc.DrawLine ( p1 , p1a ) ;
			dc.DrawLine ( p1a , p1b ) ;
			dc.DrawLine ( p1b , p1c ) ;
			dc.DrawLine ( p1c , p1d ) ;
			dc.DrawLine ( p1d , p2 ) ;
			}
		
		wxString s = wxString::Format ( _T("%1.5f") , children[a]->getWeight() ) ;
		dedigitize ( s ) ;
		dc.DrawText ( s , ( p1.x + p2.x ) / 2 , ( p1.y + p2.y ) / 2 ) ;
		
		children[a]->drawRecursive ( dc , mode ) ;
		}

	if ( isLeaf() ) dc.DrawRectangle ( rect ) ;
	dc.DrawText ( name , rect.GetLeft() + 2 , rect.GetTop() + 2 ) ;
	}

void TPTree::filterDepth ( int depth , vector <TPTree*> &vt )
	{
	if ( getCurrentDepth() == depth )
		{
		vt.push_back ( this ) ;
//		return ;
		}
	
	for ( int a = 0 ; a < children.size() ; a++ )
		children[a]->filterDepth ( depth , vt ) ;
	}

void TPTree::getAll ( vector <TPTree*> &vt )
	{
	vt.push_back ( this ) ;
	for ( int a = 0 ; a < children.size() ; a++ )
		children[a]->getAll ( vt ) ;
	}

void TPTree::averageY ()
	{
	if ( isLeaf() ) return ;
	int a ;
	for ( a = 0 ; a < children.size() ; a++ ) children[a]->averageY () ;
	
	int y = 0 ;
	for ( a = 0 ; a < children.size() ; a++ )
		y += ( children[a]->rect.GetTop() + children[a]->rect.GetBottom() ) / 2 ;
	y /= children.size() ;
	rect.SetY ( y ) ;
	rect.SetHeight ( 20 ) ;
	}

void TPTree::dedigitize ( wxString &s )
	{
	if ( s.Find ( '.' ) == -1 ) return ; // Integer
	while ( s.Right ( 1 ) == '0' ) s = s.Mid ( 0 , s.length() - 1 ) ;
	if ( s.Right ( 1 ) == '.' ) s = s.Mid ( 0 , s.length() - 1 ) ;
	}
