#include "TGraph.h"
#include <wx/dcbuffer.h>

wxColour TGraphDisplay::prettyColor = wxColour ( 0x99 , 0xCC , 0xFF ) ;

BEGIN_EVENT_TABLE(TGraph, MyChildBase)
//    EVT_MENU(SEQ_PRINT, TCalculator::OnSeqPrint)
//    EVT_MENU(MDI_PRINT_REPORT,TCalculator::OnPrintPreview)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_COMMAND_SCROLL(GRAPH_ZOOM_X, TGraph::OnZoomX)
    EVT_COMMAND_SCROLL(GRAPH_ZOOM_Y, TGraph::OnZoomY)

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


BEGIN_EVENT_TABLE(TGraphDisplay, wxPanel)
    EVT_PAINT(TGraphDisplay::OnPaint)
    EVT_MOUSE_EVENTS(TGraphDisplay::OnEvent)
    EVT_MENU(GRAPH_SWAP_SIDE,TGraphDisplay::OnSwapSides)
END_EVENT_TABLE()

TGraph::TGraph(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
	{
    def = "GRAPH" ;
    vec = NULL ;
    nb = NULL ;
	}
 
TGraph::~TGraph()
	{
	if ( nb )
		{
  		delete nb ;
  		nb = NULL ;
		}    
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
    gd->SetupDummy () ;
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
    
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , "Zoom (X)" ) ) ;
    toolBar->AddControl ( zoom_x ) ;
    toolBar->AddSeparator() ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , "Zoom (Y)" ) ) ;
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
	}

wxString TGraph::getName ()
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

//******************************************************** TGraphData

TGraphData::TGraphData ( TGraphDisplay *d )
	{
 	display = d ;
 	sx = sy = NULL ;
 	pointStyle = "rect" ;
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
	int lx , ly ;
	wxRect inner ;
	drawn.clear () ;
	dc.GetClippingBox ( &inner.x , &inner.y , &inner.width , &inner.height ) ;
	dc.SetPen ( *MYPEN ( col ) ) ;
	dc.SetBrush ( *MYBRUSH ( col ) ) ;
	for ( a = 0 ; a < dx.size() ; a++ )
		{
  		x = sx->GetRealCoord ( dx[a] , inner ) ;
  		y = sy->GetRealCoord ( dy[a] , inner ) ;
  		if ( a != 0 ) dc.DrawLine ( lx , ly , x , y ) ;
  		
  		if ( inner.Inside ( wxPoint ( x , y ) ) )
  			drawn.push_back ( wxPoint ( x , y ) ) ;
  		
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
	if ( symbol == "rect" ) dc.DrawRectangle ( x-size*2/3 , y-size*2/3 , size*4/3+1 , size*4/3+1 ) ;
	else if ( symbol == "circle" ) dc.DrawCircle ( x , y , size ) ;
	else if ( symbol == "triangle" )
		{
  		wxPoint points[3] ;
  		int ns = size ;
  		points[0] = wxPoint ( x , y - ns ) ;
  		points[1] = wxPoint ( x - ns , y + ns ) ;
  		points[2] = wxPoint ( x + ns , y + ns ) ;
  		dc.DrawPolygon ( 3 , points ) ;
		}    
	}    
    
//******************************************************** TGraphScale
 
TGraphScale::TGraphScale ( float _min , float _max , bool _horizontal , bool _left , wxString _name , wxString _unit , 
				wxColour _col , wxString _type )
	{
 	min = _min ;
    max = _max ;
 	top = max ;
 	bottom = min ;
 	horizontal = _horizontal ;
 	left = _left ;
 	col = *wxBLACK ;
 	name = _name ;
 	unit = _unit ;
 	type = _type ;
 	col = _col ;
 	outline = wxRect ( -1 , -1 , 0 , 0 ) ;
 	selected = false ;
 	show_mark = false ;
	}
	
int TGraphScale::GetWidth ()
	{
 	return horizontal ? 30 : 50 ;
	}
	
wxRect TGraphScale::CalcInternalRect ( wxRect &r )
	{
	wxRect ir = r ;
	int w = GetWidth() ;
	if ( horizontal )
		{
   		if ( left ) // Top
   			{
		    r.y += w ;
   			}
      	else // Bottom
       		{
   		    ir.y += ir.height - w ;
         	}        
      	ir.height = w ;
      	r.height -= w ;
		}
 	else
  		{
   		if ( left ) // Left
   			{
		    r.x += w ;
   			}
      	else // Right
       		{
   		    ir.x += ir.width - w ;
         	}        
      	ir.width = w ;
      	r.width -= w ;
    	}        
   	return ir ;
	}    

void TGraphScale::drawit ( wxDC &dc , wxRect &r , wxRect &inner )
	{
   	wxRect ir = CalcInternalRect ( r ) ;
   	int border = 2 ;
   	
   	if ( horizontal )
   		{
	    if ( ir.x < inner.x ) ir.x = inner.x ;
	    while ( ir.GetRight() > inner.GetRight() ) ir.width-- ;
	    outline = ir ;
	    ir.height -= border*2 ;
	    ir.y += border ;
   		}
    else
    	{
	    if ( ir.y < inner.y ) ir.y = inner.y ;
	    while ( ir.GetBottom() > inner.GetBottom() ) ir.height-- ;
	    outline = ir ;
	    ir.width -= border*2 ;
	    ir.x += border ;
     	}        
   	
   	// Selected? Draw nice background!
   	if ( selected )
   		{
	    dc.SetPen ( *MYPEN ( TGraphDisplay::prettyColor ) ) ;
	    dc.SetBrush ( *MYBRUSH ( TGraphDisplay::prettyColor ) ) ;
	    dc.DrawRectangle ( outline ) ;
	    dc.SetBrush ( *wxTRANSPARENT_BRUSH ) ;
   		}   
   	
   	// Calculate major and minor units
   	float factor = 1 ;
   	while ( factor <= max ) factor *= 10 ;
   	float f2 = top - bottom ;
   	while ( factor >= f2 ) factor /= 10 ;
   	
   	float major = factor ;
   	float minor = major / 5 ;
   	
   	// Draw scale
   	dc.SetFont ( *MYFONT ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ) ;
   	dc.SetTextForeground ( col ) ;
   	dc.SetPen ( *MYPEN ( col ) ) ;
   	
   	float p = 0 ;
   	while ( p > bottom ) p -= minor ;
   	while ( p < bottom ) p += minor ;
   	if ( p != bottom ) p -= minor ;
   	while ( p <= top )
   		{
	    DrawMark ( dc , p , ir , wxString::Format ( "%f" , p ) , ( p - ((int)(p/major))*major ) == 0 ) ;
	    p += minor ;
   		}    
	
	// Draw "backline" and name/unit
	wxString text = name ;
	if ( !unit.IsEmpty() ) text += " [" + unit + "]" ;
   	int tw , th ;
   	dc.GetTextExtent ( text , &tw , &th ) ;
	if ( horizontal )
		{
  		if ( left )
    		{
          	dc.DrawLine ( ir.x , ir.y , ir.GetRight() , ir.y ) ;
          	dc.DrawText ( text , ir.x , ir.y ) ;
          	}   	
  		else
    		{
     		dc.DrawLine ( ir.x , ir.GetBottom() , ir.GetRight() , ir.GetBottom() ) ;
          	dc.DrawText ( text , ir.x , ir.GetBottom() - th ) ;
     		}  		
  		if ( show_mark )
  			{
	    	int c = GetRealCoord ( GetVirtualCoordinate ( mark , inner ) , inner ) ;
	    	dc.SetPen ( *wxRED_PEN ) ;
	    	dc.DrawLine ( c , ir.GetTop() , c , ir.GetBottom() ) ;
  			}    
		}		
	else
		{
		if ( left )
  			{
         	dc.DrawLine ( ir.x , ir.y , ir.x , ir.GetBottom() ) ;
         	dc.DrawRotatedText ( text , ir.x + 2 , ir.GetBottom() , 90 ) ;
         	}   	
		else
  			{
    		dc.DrawLine ( ir.GetRight() , ir.y , ir.GetRight() , ir.GetBottom() ) ;
         	dc.DrawRotatedText ( text , ir.GetRight()-th , ir.GetBottom() , 90 ) ;
      		}
  		if ( show_mark )
  			{
	    	int c = GetRealCoord ( GetVirtualCoordinate ( mark , inner ) , inner ) ;
	    	dc.SetPen ( *wxRED_PEN ) ;
	    	dc.DrawLine ( ir.GetLeft() , c , ir.GetRight() , c ) ;
  			}    
   		}
	last_inner = inner ;
	show_mark = false ;
	}    

void TGraphScale::DrawMark ( wxDC &dc , float p , wxRect &ir , wxString text , bool big )
	{
	int x , y , z ;
 	if ( horizontal )
 		{
    	x = GetRealCoord ( p , ir ) ;
    	y = ir.y + ir.height / 2 ;
    	z = big ? 0 : ir.height / 4 ;
    	if ( x < ir.x || x > ir.GetRight() ) return ;
 		}
 	else
  		{
    	x = ir.x + ir.width / 2 ;
    	y = GetRealCoord ( p , ir ) ;
    	z = big ? 0 : ir.width / 4 ;
    	if ( y < ir.y || y > ir.GetBottom() ) return ;
    	}        
   	while ( text.Last() == '0' ) text = text.Left ( text.length() - 1 ) ;
   	while ( text.Last() == '.' ) text = text.Left ( text.length() - 1 ) ;
   	
   	int tw , th ;
   	dc.GetTextExtent ( text , &tw , &th ) ;
   	if ( horizontal )
   		{
	    if ( left ) dc.DrawLine ( x , y-z , x , ir.y ) ;
	    else dc.DrawLine ( x , y+z , x , ir.GetBottom() ) ;
	    x -= tw / 2 ;
	    y = left ? ir.GetBottom() - th : ir.y ;
   		}
    else
    	{
	    if ( left ) dc.DrawLine ( ir.x , y , x-z , y ) ;
	    else dc.DrawLine ( x+z , y , ir.GetRight() , y ) ;
	    y -= th / 2 ;
	    x = left ? ir.GetRight() - tw : ir.x ;
     	}        

   	if ( big ) dc.DrawText ( text , x , y ) ;
	}
    
int TGraphScale::GetRealCoord ( float f , wxRect &inner )
	{
	float m2 = horizontal ? inner.width : inner.height ; // The pixel "width"
	float f2 = top - bottom ; // The internal "width"
	float ret = m2 * ( f - bottom ) / f2 ;
	if ( horizontal ) ret += (float ) inner.x ;
	else ret = (float) ( inner.GetBottom() ) - ret ;
 	return (int) ret ;
	}    

float TGraphScale::GetVirtualCoordinate ( int i , wxRect &inner )
	{
 	i -= horizontal ? inner.x : inner.y ;
 	float i2 = i ;
	float m2 = horizontal ? inner.width : inner.height ;
	float f2 = top - bottom ; // The internal "width"
	f2 = f2 * i2 / m2 ;
	if ( horizontal ) f2 += bottom ;
	else f2 = top - f2 ;
	return f2 ;
 	}    	

void TGraphScale::Drag ( int delta )
	{
	float df = GetVirtualCoordinate ( delta , last_inner ) ;
	df -= GetVirtualCoordinate ( 0 , last_inner ) ;
	top += df ;
	bottom += df ;
	if ( top > max )
 		{
   		bottom -= top - max ;
        top = max ;
        }    
    if ( bottom < min )
    	{
	    top += min - bottom ;
	    bottom = min ;
    	}    
	}    
    
//******************************************************** TGraphDisplay    
    
TGraphDisplay::TGraphDisplay ( wxWindow *parent , int id )
	: wxPanel ( parent , id )
	{
	init () ;
	}
	
void TGraphDisplay::init ()
	{
	while ( data.size() ) data.pop_back () ;
	while ( scales.size() ) scales.pop_back () ;
	old_scale = NULL ;
	old_data = NULL ;
	zx = zy = 100 ;
	}    
	
void TGraphDisplay::SetZoom ( int _zx , int _zy )
	{
	int a ;
	for ( a = 0 ; a < scales.size() ; a++ )
		{
  		float z = scales[a]->horizontal ? _zx : _zy ;
  		float m = ( scales[a]->top + scales[a]->bottom ) / 2 ;
  		float h = scales[a]->max - scales[a]->min ;
  		float p = 10.0 * z + 90 ; // 100 - 1000
  		h = h * 100.0 / p ;
  		scales[a]->top = m + h/2 ;
  		scales[a]->bottom = m - h/2 ;
  		scales[a]->Drag ( 0 ) ;
		}
	zx = _zx ;
	zy = _zy ;
    wxClientDC dc ( this ) ;
        {
        wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
        drawit ( dc2 ) ;
        }    
	}    

stringField TGraphDisplay::readCSVfile ( wxString filename )
	{
	wxTextFile file ( filename ) ;
	file.Open () ;
	wxString s ;
	stringField sf ;
	for ( s = file.GetFirstLine(); !file.Eof(); s = file.GetNextLine() )
		{
  		wxArrayString as ;
  		explode ( "," , s , as ) ;
  		sf.push_back ( as ) ;
		}    
	return sf ;
	}    
	
void TGraphDisplay::setupPhotometerGraph ( const stringField &sf )
	{
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , txt("t_wavelength") , "nm" , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , txt("t_intensity") , "au" , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	
 	wxArrayString colors , styles ;
 	colors.Add ( "RED" ) ;
 	colors.Add ( "GREEN" ) ;
 	colors.Add ( "BLUE" ) ;
 	colors.Add ( "MAGENTA" ) ;
 	styles.Add ( "rect" ) ;
 	styles.Add ( "circle" ) ;
 	styles.Add ( "triangle" ) ;
 	int cnt_col = 0 , cnt_sty = 0 ;
	
	int a , b ;
	for ( a = 0 ; a+2 < sf[0].GetCount() ; a += 2 )
		{
  		TGraphData *ng = new TGraphData ( this ) ;
  		ng->name = sf[0][a] ;
  		ng->SetScales ( sx , sy ) ;
  		ng->pointStyle = styles[cnt_sty] ;
  		ng->col = wxTheColourDatabase->Find ( colors[cnt_col] ) ;
  		if ( ++cnt_col >= colors.GetCount() )
  			{
	    	cnt_col = 0 ;
	    	if ( ++cnt_sty >= styles.GetCount() )
	    		cnt_sty = 0 ;
         	}
  		for ( b = 2 ; b < sf.size() && ( sf[b].GetCount() > a + 1 && sf[b][0] != "" ) ; b++ )
  			{
	    	double x , y ;
	    	sf[b][a].ToDouble ( &x ) ;
	    	sf[b][a+1].ToDouble ( &y ) ;
	    	ng->Add ( (float) x , (float) y ) ;
  			}    
  		data.push_back ( ng ) ;
		}    
	}    

void TGraphDisplay::SetupDummy ()
	{
/*
 	scales.push_back ( new TGraphScale ( 0 , 0 , true , false , "x" , "min" , *wxBLUE ) ) ;
 	scales.push_back ( new TGraphScale ( 0 , 0 , false , true , "y" , "cm" , *wxRED ) ) ;
 	data.push_back ( new TGraphData ( this ) ) ;
 	data[0]->SetScales ( scales[0] , scales[1] ) ;
*/


	stringField sf = readCSVfile ( "C:\\Dokumente und Einstellungen\\DSP\\Desktop\\RTS Kristin.csv" ) ;
	setupPhotometerGraph ( sf ) ;


/*
 	wxString file = "C:\\Dokumente und Einstellungen\\DSP\\Desktop\\HADI\\G6_" ;
 	wxString file1 = file + "1.DAT" ;
 	wxFile f1 ( file1 , wxFile::read ) ;
 	long l = f1.Length() ;
 	wxMessageBox ( wxString::Format ( "%d" , l ) ) ;
 	
 	unsigned char *c = new unsigned char ( l + 5 ) ;
 	f1.Read ( c , l ) ;
 	f1.Close () ;
 	
 	int a ;
 	for ( a = 0 ; a+4 < l ; a++ )
 		{
   		unsigned long ul = c[a++] ;
   		ul <<= 8 ; ul += c[a++] ;
   		ul <<= 8 ; ul += c[a++] ;
//   		ul <<= 8 ; ul += c[a] ;
   		data[0]->Add ( (float) a , (float) ul ) ;
 		}

    delete c ;
*/	
/* 	int a ;
 	for ( a = 0 ; a < 1000 ; a++ )
 		data[0]->Add ( a , a % 100 ) ;
*/

	AutoScale () ;
	}

void TGraphDisplay::AutoScale ()
	{
 	int a ;
 	for ( a = 0 ; a < scales.size() ; a++ )
 		{
   		scales[a]->min = scales[a]->max = 0 ;
 		}    
 	for ( a = 0 ; a < data.size() ; a++ )
 		{
   		if ( data[a]->dx.size() == 0 ) continue ;
   		data[a]->sx->min = data[a]->sx->max = data[a]->dx[0] ;
   		data[a]->sy->min = data[a]->sy->max = data[a]->dy[0] ;
 		}    
 	for ( a = 0 ; a < data.size() ; a++ )
 		{
   		data[a]->AutoScale () ;
 		}    
/*	for ( a = 0 ; a < scales.size() ; a++ )
		{
  		int sw = scales[a]->max - scales[a]->min ;
  		sw = sw / 20 ;
  		scales[a]->max += sw ;
  		if ( scales[a]->min != 0 ) scales[a]->min -= sw ;
  		scales[a]->top = scales[a]->max ;
  		scales[a]->bottom = scales[a]->min ;
		}    */
 	}

void TGraphDisplay::drawit ( wxDC &dc , int mode )
	{
 	dc.Clear () ;
 	int a , b ;
 	wxRect outer ( 0 , 0 , 0 , 0 ) ;
 	dc.GetSize ( &outer.width , &outer.height ) ;
 	int border = 5 ;
 	outer.x += border * 2 ;
 	outer.y += border ;
 	outer.width -= border * 4 ;
 	outer.height -= border * 2 ;
 	
 	// Setting inner rectangle
 	inner = outer ;
 	for ( a = 0 ; a < scales.size() ; a++ )
   		scales[a]->CalcInternalRect ( inner ) ;
 		
	dc.SetPen ( *wxBLACK_PEN ) ;
//	dc.DrawRectangle ( outer ) ;
	dc.DrawRectangle ( inner ) ;
	
//	dc.DestroyClippingRegion () ;
//	dc.SetClippingRegion ( outer ) ;
	wxRect o2 = outer ;
	for ( a = 0 ; a < scales.size() ; a++ )
		{
  		scales[a]->drawit ( dc , o2 , inner ) ;
		}    
	
	dc.DestroyClippingRegion () ;
	
	if ( mode & GRAPH_DRAW_MAP )
		{
		dc.SetClippingRegion ( inner ) ;
    	for ( a = 0 ; a < data.size() ; a++ )
    		{
      		data[a]->drawit ( dc ) ;
    		}    
    	showLegend ( dc ) ;
    	dc.DestroyClippingRegion () ;
     	}   	
     	
 	if ( mode & GRAPH_DRAW_MINI )
 		showMiniature ( dc ) ;
	}
	
void TGraphDisplay::showMiniature ( wxDC &dc )
	{
	dc.SetPen ( *wxBLACK_PEN ) ;
	dc.SetBrush ( *wxWHITE_BRUSH ) ;
	
	int border = 20 ;
	wxRect r = inner ;
	r.x += border ;
	r.y += border ;
	r.width = 100 ;
	r.height = r.width * inner.height / inner.width ;
	dc.DrawRectangle ( r ) ;
	
	int a ;
	bool has_x = false , has_y = false ;
	for ( a = 0 ; a < scales.size() ; a++ )
		{
  		if ( scales[a]->horizontal && !has_x )
  			{
	    	has_x = true ;
	    	r.x += ((float)r.width) * ( scales[a]->bottom - scales[a]->min ) / scales[a]->GetTotalWidth() ;
	    	r.width = ((float)r.width) * scales[a]->GetVisibleWidth() / scales[a]->GetTotalWidth() ;
  			}
      	if ( !scales[a]->horizontal && !has_y )
      		{
  		    has_y = true ;
	    	r.y += r.height - ( ((float)r.height) * ( scales[a]->bottom - scales[a]->min ) / scales[a]->GetTotalWidth() ) ;
	    	r.height = ((float)r.height) * scales[a]->GetVisibleWidth() / scales[a]->GetTotalWidth() ;
	    	r.y -= r.height ;
      		}    
		}    
	dc.SetBrush ( *MYBRUSH ( prettyColor ) ) ;
	dc.DrawRectangle ( r ) ;
	}    
	
void TGraphDisplay::showLegend ( wxDC &dc )
	{
	dc.SetPen ( *wxBLACK_PEN ) ;
	dc.SetBrush ( *wxWHITE_BRUSH ) ;
	
	int w = 0 , h = 4 , symw = 50 ;
	int border = 20 ;
	
	int a ;
	int tw , th ;
	for ( a = 0 ; a < data.size() ; a++ )
		{
  		dc.GetTextExtent ( data[a]->name , &tw , &th ) ;
  		h += th + 2 ;
  		w = tw > w ? tw : w ;
		}    
	w += 4 + symw ;
	
	lr.x = inner.GetRight() - w - border ;
	lr.y = inner.y + border ;
	lr.width = w ;
	lr.height = h ;
	
	dc.DrawRectangle ( lr ) ;
	
	for ( a = 0 ; a < data.size() ; a++ )
		{
 		if ( data[a]->selected )
 			{
    		dc.SetBrush ( *MYBRUSH ( TGraphDisplay::prettyColor ) ) ;
    		dc.SetPen ( *MYPEN ( TGraphDisplay::prettyColor ) ) ;
    		dc.DrawRectangle ( lr.x + 2 , 
      						   lr.y + 2 + ( 2 + th ) * a ,
      						   w - 4 , th ) ;
 			}    
  		dc.SetTextForeground( data[a]->col ) ;
		dc.DrawText ( data[a]->name , lr.x + 2 + symw , lr.y + 2 + ( 2 + th ) * a ) ;
		dc.SetPen ( *MYPEN ( data[a]->col ) ) ;
		dc.SetBrush ( *MYBRUSH ( data[a]->col ) ) ;
		dc.DrawLine ( lr.x + 2 + symw/10 ,
					  lr.y + 2 + ( 2 + th ) * a + th/2 ,
					  lr.x + 2 + symw*9/10 ,
					  lr.y + 2 + ( 2 + th ) * a + th/2 ) ;
        data[a]->DrawSymbol ( dc , 
        					  data[a]->pointStyle , 
        					  lr.x + 2 + symw/2 ,
        					  lr.y + 2 + ( 2 + th ) * a + th/2 ) ;
		}		
	}    

void TGraphDisplay::OnPaint(wxPaintEvent& event)
	{
	wxPaintDC dc(this);
        {
        wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
        drawit ( dc2 ) ;
        }    
    }

void TGraphDisplay::OnEvent(wxMouseEvent& event)
    {
    wxPoint pt(event.GetPosition());
    
    // Find out where the mouse is
    int a , b ;
    bool doRefresh = event.Leaving() ;
    TGraphScale *new_scale = NULL ;
    TGraphData *new_data = NULL ;
    
    for ( a = 0 ; a < scales.size() ; a++ )
    	{
    	scales[a]->selected = false ;
    	if ( scales[a]->outline.Inside ( pt ) ) new_scale = scales[a] ;
     	}
   	if ( new_scale ) new_scale->selected = true ;
     	
   	if ( inner.Inside ( pt ) )
   		{
	    int best = 100 ;
	    for ( a = 0 ; a < data.size() ; a++ )
	    	{
 	    	data[a]->selected = false ;
 	    	for ( b = 0 ; b < data[a]->drawn.size() ; b++ )
 	    		{
  		    	wxPoint z = wxPoint ( pt.x - data[a]->drawn[b].x , pt.y - data[a]->drawn[b].y ) ;
  		    	int d = (z.x*z.x) + (z.y*z.y) ;
  		    	if ( d < 4*4 && d < best )
  		    		{
  		    		new_data = data[a] ;
  		    		best = d ;
  		    		break ;
  		    		}  		
 	    		}    
	    	}    
    	if ( new_data ) new_data->selected = true ;
   		}    
   		
    // Dragging?
    bool showMiniature = false ;
    if ( zx*zy != 10000 && event.Dragging() && !event.RightIsDown() && !event.MiddleIsDown() ) // NEEDS ANOTHER CONDITION
    	{
	    showMiniature = true ;
	    int dx = mouse_pos.x - pt.x ;
	    int dy = mouse_pos.y - pt.y ;
	    doRefresh = true ;
	    for ( a = 0 ; a < scales.size() ; a++ )
	    	{
 	    	if ( scales[a]->horizontal ) scales[a]->Drag ( dx ) ;
 	    	else scales[a]->Drag ( dy ) ;
	    	}    
    	}    
    	
   	// Mouse Wheel?
   	if ( zx*zy != 10000 && event.GetWheelRotation() != 0 )
   		{
	    showMiniature = true ;
	    int dx = 0 ;
	    int dy = event.GetWheelRotation() > 0 ? -10 : 10 ;
	    if ( event.ShiftDown() ) { int dz = dx ; dx = dy ; dy = dz ; }
	    doRefresh = true ;
	    for ( a = 0 ; a < scales.size() ; a++ )
	    	{
 	    	if ( scales[a]->horizontal ) scales[a]->Drag ( dx ) ;
 	    	else scales[a]->Drag ( dy ) ;
	    	}    
   		}    

    // Red marker lines
    if ( inner.Inside ( pt ) )
    	{
       	for ( a = 0 ; a < scales.size() ; a++ )
        	{
            scales[a]->show_mark = true ; 
            scales[a]->mark = scales[a]->horizontal ? pt.x : pt.y ;
            }    
        }
   	
   	// Redraw
    old_scale = new_scale ;
    old_data = new_data ;
    wxClientDC dc ( this ) ;
        {
        wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
        int mode = GRAPH_DRAW_SCALES | GRAPH_DRAW_MAP ;
        if ( showMiniature ) mode |= GRAPH_DRAW_MINI ;
//        if ( doRefresh || new_scale != old_scale || old_data != new_data )
            drawit ( dc2 , mode ) ;
//        else
//            drawit ( dc2 , GRAPH_DRAW_SCALES ) ;
        }    

    // Context menu?
    if ( event.RightDown() )
    	{
        wxMenu *cm = new wxMenu ;
	    if ( new_scale )
	    	{
	    	cm->Append ( GRAPH_SWAP_SIDE , txt("m_graph_swap_side") ) ;
	    	}   
        PopupMenu ( cm , pt ) ;
        delete cm ;    
    	}   
    	
   	mouse_pos = pt ; 
    }    


void TGraphDisplay::OnSwapSides(wxCommandEvent &event)
	{
	if ( !old_scale ) return ;
	old_scale->left = !old_scale->left ;
	wxPaintEvent ev ;
	OnPaint ( ev ) ;
	}

