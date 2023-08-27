#include "TGraph.h"
#include <wx/dcbuffer.h>

wxColour TGraphDisplay::prettyColor = wxColour ( 0x99 , 0xCC , 0xFF ) ;

BEGIN_EVENT_TABLE(TGraph, MyChildBase)
    EVT_CHAR_HOOK(TGraph::OnCharHook)
//    EVT_MENU(SEQ_PRINT, TCalculator::OnSeqPrint)
//    EVT_MENU(MDI_PRINT_REPORT,TCalculator::OnPrintPreview)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_COMMAND_SCROLL(GRAPH_ZOOM_X, TGraph::OnZoomX)
    EVT_COMMAND_SCROLL(GRAPH_ZOOM_Y, TGraph::OnZoomY)
    EVT_MENU(MDI_COPY,TGraph::OnCopy)
    EVT_MENU(MDI_FILE_SAVE,TGraph::OnSave)

    // Dummies
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



TGraph::TGraph(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
	{
    def = _T("GRAPH") ;
    vec = NULL ;
    nb = NULL ;
    allow_save = allow_copy = true ;
	}
 
TGraph::~TGraph()
	{/*
	if ( nb )
		{
  		delete nb ;
  		nb = NULL ;
		} */
	}

void TGraph::OnCharHook ( wxKeyEvent& event )
	{
	wxMessageBox ( _T("!") ) ;
	if ( gd ) gd->OnCharHook ( event ) ;
	}    

void TGraph::initme ()
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

    nb = new wxNotebook ( this , -1 ) ;

    gd = new TGraphDisplay ( nb , -1 ) ;
    gd->g = this ;
//    gd->SetupDummy () ;
    nb->AddPage ( gd , txt("t_graph") ) ;

    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    toolBar->Reparent ( this ) ;
    toolbar = toolBar ;
    myapp()->frame->InitToolBar(toolBar);

    zoom_x = new wxSlider ( toolBar , GRAPH_ZOOM_X , 1 , 1 , 91 , 
                             wxDefaultPosition ,
                             wxDefaultSize ,
                             wxSL_HORIZONTAL ) ;
    zoom_y = new wxSlider ( toolBar , GRAPH_ZOOM_Y , 1 , 1 , 91 , 
                             wxDefaultPosition ,
                             wxDefaultSize ,
                             wxSL_HORIZONTAL ) ;
    zoom_linked = new wxCheckBox ( toolBar , GRAPH_ZOOM_LINKED , txt("t_zoom_linked") ) ;
    
	myapp()->frame->addTool ( toolBar , MDI_TEXT_IMPORT ) ;
	myapp()->frame->addTool ( toolBar , MDI_FILE_OPEN ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , _T("Zoom (X)") ) ) ;
    toolBar->AddControl ( zoom_x ) ;
    toolBar->AddSeparator() ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , _T("Zoom (Y)") ) ) ;
    toolBar->AddControl ( zoom_y ) ;
    toolBar->AddControl ( zoom_linked ) ;
    myapp()->frame->addDefaultTools ( toolBar ) ;
    toolBar->Realize() ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( nb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;
    
    zoom_linked->SetValue ( true ) ;

    myapp()->frame->setChild ( this ) ;
    Maximize () ;
    Activate () ;
    gd->SetFocus() ;
	}

const wxString TGraph::getName () const
	{
    return txt("t_graph") ;
	}

void TGraph::OnZoomX(wxScrollEvent& event)
	{
	if ( zoom_linked->GetValue() ) zoom_y->SetValue ( zoom_x->GetValue() ) ;
	gd->SetZoom ( zoom_x->GetValue() , zoom_y->GetValue() ) ;
	}    

void TGraph::OnZoomY(wxScrollEvent& event)
	{
	if ( zoom_linked->GetValue() ) zoom_x->SetValue ( zoom_y->GetValue() ) ;
	gd->SetZoom ( zoom_x->GetValue() , zoom_y->GetValue() ) ;
	}    

void TGraph::OnCopy(wxCommandEvent& event)
	{
    if ( gd ) gd->OnCopyAsImage ( event ) ;
    }

void TGraph::OnSave(wxCommandEvent& event)
	{
    if ( gd ) gd->OnSaveAsImage ( event ) ;
    }

//******************************************************** TGraphData

TGraphData::TGraphData ( TGraphDisplay *d )
	{
 	display = d ;
 	sx = sy = NULL ;
 	pointStyle = _T("rect") ;
 	col = *wxBLACK ;
 	selected = false ;
	}

void TGraphData::Add ( float x , float y )    
	{
	dx.push_back ( x ) ;
	dy.push_back ( y ) ;
	}    
	
void TGraphData::SetScales ( TGraphScale *_sx , TGraphScale *_sy )
	{
	sx = _sx ;
	sy = _sy ;
	}    

void TGraphData::AutoScale ()
	{
 	int a ;
 	for ( a = 0 ; a < dx.size() ; a++ )
 		{
   		if ( sx->min > dx[a] ) sx->min = dx[a] ;
   		if ( sx->max < dx[a] ) sx->max = dx[a] ;
   		if ( sy->min > dy[a] ) sy->min = dy[a] ;
   		if ( sy->max < dy[a] ) sy->max = dy[a] ;
 		}
	sx->top = sx->max ;
	sx->bottom = sx->min ;
	sy->top = sy->max ;
	sy->bottom = sy->min ;
	}
         
void TGraphData::drawit ( wxDC &dc )
	{
	int a ;
	int x , y ;
	int lx = 0 , ly = 0 ;
	wxRect inner ;
	drawn.clear () ;
	dc.GetClippingBox ( &inner.x , &inner.y , &inner.width , &inner.height ) ;
	dc.SetPen ( *MYPEN ( col ) ) ;
	dc.SetBrush ( *MYBRUSH ( col ) ) ;
	wxPoint last_drawn = wxPoint ( -1 , -1 ) ;
	for ( a = 0 ; a < dx.size() ; a++ )
		{
  		x = sx->GetRealCoord ( dx[a] , inner ) ;
  		y = sy->GetRealCoord ( dy[a] , inner ) ;
  		if ( a != 0 ) dc.DrawLine ( lx , ly , x , y ) ;
  		
  		if ( x >= inner.GetLeft() && x <= inner.GetRight() )
  			{
	    	wxPoint p ( x , y ) ;
	    	if ( p != last_drawn )
	    		{
   	   			drawn.push_back ( p ) ;
   	   			last_drawn = p ;
   	   			} 			
   			} 			
  		
  		if ( selected ) dc.SetPen ( *wxBLACK_PEN ) ;
  		DrawSymbol ( dc , pointStyle , x , y ) ;
  		if ( selected ) dc.SetPen ( *MYPEN ( col ) ) ;
  		
  		lx = x ;
    	ly = y ;
		}    
	dc.SetBrush ( *wxTRANSPARENT_BRUSH ) ;
	}    
	
void TGraphData::DrawSymbol ( wxDC &dc , wxString symbol , int x , int y , int size )
	{
	if ( symbol == _T("rect") ) dc.DrawRectangle ( x-size*2/3 , y-size*2/3 , size*4/3+1 , size*4/3+1 ) ;
	else if ( symbol == _T("circle") ) dc.DrawCircle ( x , y , size ) ;
	else if ( symbol == _T("triangle") )
		{
  		wxPoint points[3] ;
  		int ns = size ;
  		points[0] = wxPoint ( x , y - ns ) ;
  		points[1] = wxPoint ( x - ns , y + ns ) ;
  		points[2] = wxPoint ( x + ns , y + ns ) ;
  		dc.DrawPolygon ( 3 , points ) ;
		}    
	}    
    

unsigned long TGraphData::GetCount ()
	{
	return dx.size() ;
 	}

int TGraphData::Distance ( const wxPoint &pt )
	{
	int a , best = 999999999 ;
 	if ( drawn.size() < 2 ) return best ; // Not near at all
	wxPoint p1 = drawn[0] , p2 ;
	for ( a = 1 ; a < drawn.size() ; a++ )
		{
  		p2 = drawn[a] ;
  		
  		int f , min = 999999999 ;
  		float max = ( p2.x - p1.x ) * ( p2.x - p1.x ) + ( p2.y - p1.y ) * ( p2.y - p1.y ) ;
  		max = max > 0 ? sqrt ( max ) : 1 ;
  		for ( f = 0 ; f <= max ; f = f + 1 )
  			{
			float fx = p2.x - p1.x ;
			float fy = p2.y - p1.y ;
			fx = fx * f / max ;
			fy = fy * f / max ;
			fx = fx + p1.x ;
			fy = fy + p1.y ;
	    	fx = fx - ((float)pt.x) ;
	    	fy = fy - ((float)pt.y) ;
	    	fx = fx * fx ;
	    	fy = fy * fy ;
	    	if ( fx + fy < min ) min = fx + fy ;
  			}    
		min = sqrt ( (double) min ) ;
		if ( best > (int) min ) best = (int) min ;
  		
  		p1 = p2 ;
		}    
	return best ;
	}
    
