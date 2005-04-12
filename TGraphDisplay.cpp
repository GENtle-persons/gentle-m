#include "TGraph.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(TGraphDisplay, wxPanel)
    EVT_PAINT(TGraphDisplay::OnPaint)
    EVT_MOUSE_EVENTS(TGraphDisplay::OnEvent)
    EVT_MENU(GRAPH_SWAP_SIDE,TGraphDisplay::OnSwapSides)
    EVT_CHAR_HOOK(TGraphDisplay::OnCharHook)
END_EVENT_TABLE()

//******************************************************** TGraphDisplay    
    
TGraphDisplay::TGraphDisplay ( wxWindow *parent , int id )
	: wxPanel ( parent , id )
	{
	init () ;
	}
	
TGraphDisplay::~TGraphDisplay ()
	{/*
	int a ;
	for ( a = 0 ; a < data.size() ; a++ )
		delete data[a] ;
	data.clear () ;
	for ( a = 0 ; a < scales.size() ; a++ )
		delete scales[a] ;
	scales.clear () ;*/
	}    
	
void TGraphDisplay::init ()
	{
	while ( data.size() ) data.pop_back () ;
	while ( scales.size() ) scales.pop_back () ;
	old_scale = NULL ;
	old_data = NULL ;
	zx = zy = 100 ;
	draggingRect = wxRect ( -1 , -1 , -1 , -1 ) ;
	setupCompleted = false ;
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
	UpdateDisplay () ;
	}    

stringField TGraphDisplay::readTextfile ( wxString filename )
	{
	wxTextFile file ( filename ) ;
	file.Open () ;
	wxString s ;
	stringField sf ;
	if ( !file.IsOpened() ) return sf ;
	int a = 0 ;
	for ( s = file.GetFirstLine(); !file.Eof(); s = file.GetNextLine() )
		{
  		TVS as ;
  		s = s.Trim().Trim(false);
  		
  		if ( s.Find ( '\t' ) > -1 )
  			{
	    	s.Replace ( "," , "." ) ;
      		while ( s.First ( '\t' ) != -1 )
      			{
    	    	as.push_back ( s.BeforeFirst ( '\t' ).c_str() ) ;
    	    	s = s.AfterFirst ( '\t' ) ;
      			}    
    		as.push_back ( s.c_str() ) ;
      		sf.push_back ( as ) ;
  			}
      	else
       		{
      		while ( s.First ( ',' ) != -1 )
      			{
    	    	as.push_back ( s.BeforeFirst ( ',' ).c_str() ) ;
    	    	s = s.AfterFirst ( ',' ) ;
      			}    
    		as.push_back ( s.c_str() ) ;
      		sf.push_back ( as ) ;
      		}  		
		}    
	return sf ;
	}    
	
void TGraphDisplay::setupIPCfile ( wxString filename )
	{
	stringField sf = readTextfile ( filename ) ;
//	stringField sf = readTextfile ( "C:\\Dokumente und Einstellungen\\DSP\\Desktop\\0.1 IGF-VK + 0.4 PolyGluTyr 360nm.csv" ) ;
//	setupPhotometerGraph ( sf ) ;

 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , "m/z" , "" , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , "rel. Int." , "" , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	
 	TGraphData *ng = new TGraphData ( this ) ;
	ng->name = "IPC" ;
	ng->SetScales ( sx , sy ) ;
	ng->pointStyle = "none" ;
	ng->col = wxTheColourDatabase->Find ( "BLUE" ) ;
	data.push_back ( ng ) ;

 	int a ;
 	for ( a = 0 ; a < sf.size() && sf[a].size() > 1 && sf[a][0] != "" ; a++ )
 		{
    	double x , y ;
    	wxString s0 = sf[a][0].c_str() ;
    	wxString s1 = sf[a][1].c_str() ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	ng->Add ( (float) x , (float) y ) ;
 		}
	}
    
void TGraphDisplay::setupXYpair ( const stringField &sf )
	{
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , "X" , "" , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , "Y" , "" , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	
 	TGraphData *ng = new TGraphData ( this ) ;
	ng->name = txt("t_data") ;
	ng->SetScales ( sx , sy ) ;
	ng->pointStyle = "none" ;
	ng->col = wxTheColourDatabase->Find ( "BLUE" ) ;
	data.push_back ( ng ) ;

 	int a ;
 	for ( a = 0 ; a < sf.size() && sf[a].size() > 1 && sf[a][0] != "" ; a++ )
 		{
    	double x , y ;
    	wxString s0 = sf[a][0].c_str() ;
    	wxString s1 = sf[a][1].c_str() ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	ng->Add ( (float) x , (float) y ) ;
 		}    
 	} 	

void TGraphDisplay::setupPhotometerGraph ( const stringField &sf )
	{
	if ( sf.size() < 3 ) return ;
	if ( sf[0].size() < 2 ) return ;
	if ( sf[1].size() < 2 ) return ;
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , sf[1][0].c_str() , "" , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , sf[1][1].c_str() , "" , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	
 	TGraphData *ng = new TGraphData ( this ) ;
	ng->name = sf[0][0].c_str() ;
	ng->SetScales ( sx , sy ) ;
	ng->pointStyle = "none" ;
	ng->col = wxTheColourDatabase->Find ( "BLUE" ) ;
	data.push_back ( ng ) ;

 	int a ;
 	for ( a = 2 ; a < sf.size() && sf[a].size() > 1 && sf[a][0] != "" ; a++ )
 		{
    	double x , y ;
    	wxString s0 = sf[a][0].c_str() ;
    	wxString s1 = sf[a][1].c_str() ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	ng->Add ( (float) x , (float) y ) ;
 		}    
 	} 	

void TGraphDisplay::setupFluorimeterGraph ( const stringField &sf )
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
	for ( a = 0 ; a+2 < sf[0].size() ; a += 2 )
		{
  		TGraphData *ng = new TGraphData ( this ) ;
  		ng->name = sf[0][a].c_str() ;
  		ng->SetScales ( sx , sy ) ;
  		ng->pointStyle = styles[cnt_sty] ;
  		ng->col = wxTheColourDatabase->Find ( colors[cnt_col] ) ;
  		if ( ++cnt_col >= colors.GetCount() )
  			{
	    	cnt_col = 0 ;
	    	if ( ++cnt_sty >= styles.GetCount() )
	    		cnt_sty = 0 ;
         	}
  		for ( b = 2 ; b < sf.size() && ( sf[b].size() > a + 1 && sf[b][0] != "" ) ; b++ )
  			{
	    	double x , y ;
        	wxString s0 = sf[b][a+0].c_str() ;
        	wxString s1 = sf[b][a+1].c_str() ;
        	s0.ToDouble ( &x ) ;
        	s1.ToDouble ( &y ) ;
	    	ng->Add ( (float) x , (float) y ) ;
  			}    
  		data.push_back ( ng ) ;
		}    
	}    
	
bool TGraphDisplay::SetupFromFile ( wxString filename )
	{
	int a ;
	wxArrayString as ;
	as.Add ( txt ( "t_graph_file_type_photometer" ) ) ;
	as.Add ( txt ( "t_graph_file_type_fluorimeter" ) ) ;
	as.Add ( txt ( "t_graph_file_type_xypair" ) ) ;
	
	wxString *vs = new wxString [ as.GetCount() ] ;
	for ( a = 0 ; a < as.GetCount() ; a++ )
		vs[a] = as[a] ;
	
	wxSingleChoiceDialog scd ( this , "Msg" , "Cap" , as.GetCount() , vs ) ;
	wxString guess = tryall ( filename ) ;
	if ( guess != "" )
		{
  		for ( a = 0 ; a < as.GetCount() ; a++ )
  			{
	    	if ( as[a] == txt(guess) )
	    		scd.SetSelection ( a ) ;
  			}    
		}    
	if ( wxID_OK != scd.ShowModal() ) return false ;
	
	wxString s = scd.GetStringSelection() ;
	if ( s == txt ( "t_graph_file_type_photometer" ) ) setupPhotometerGraph ( readTextfile ( filename ) ) ;
	if ( s == txt ( "t_graph_file_type_fluorimeter" ) ) setupFluorimeterGraph ( readTextfile ( filename ) ) ;
	if ( s == txt ( "t_graph_file_type_xypair" ) ) setupXYpair ( readTextfile ( filename ) ) ;
	
	return true ;
	}

unsigned char *TGraphDisplay::readRawData ( wxString filename , long &l )
	{
 	wxFile f ( filename , wxFile::read ) ;
 	l = f.Length() ;
 	unsigned char *r = new unsigned char[f.Length()] ;
 	f.Read ( r , l ) ;
 	return r ;
	}
    
void TGraphDisplay::setupRawFPLC ( wxString filenamebase )
	{
	wxString f1 = filenamebase + "_1.DAT" ;
	
	long l1 ;
	unsigned char *d1 = readRawData ( f1 , l1 ) ;
	
	stringField sf ;
	TVS b ;
	b.push_back ( "1" ) ;
	b.push_back ( "2" ) ;
	b.push_back ( "" ) ;
	sf.push_back ( b ) ; // Dummy line
	sf.push_back ( b ) ; // Dummy line
	
	long a , sum = 0 , integrate = 10 , cnt = 0 ;
	for ( a = 2000+1 ; a+30 < l1 ; a += 4 )
		{
  		long x = 0 ;
  		x += (long) d1[a+0] ; x <<= 8 ;
  		x += (long) d1[a+1] ; x <<= 8 ;
  		x += (long) d1[a+2] ; x <<= 8 ;
  		x += (long) d1[a+3] ;
  		sum += x / integrate;
  		if ( cnt >= integrate )
  			{
  			b[0] = wxString::Format ( "%d" , a );
  			b[1] = wxString::Format ( "%d" , sum ) ;
  			sf.push_back ( b ) ;
  			sum = 0 ;
  			cnt = 0 ;
   			} 	
        cnt++ ;		
		}    
	
	delete d1 ;
	setupPhotometerGraph ( sf ) ;
	}    

wxString TGraphDisplay::tryall ( wxString filename )
	{
	wxString r ;
	int best = 0 ;
	int a , cnt ;
	stringField sf = readTextfile ( filename ) ;
	init () ;

	setupPhotometerGraph ( sf ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = "t_graph_file_type_photometer" ; best = cnt ; }
	init () ;
	
	setupFluorimeterGraph ( sf ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = "t_graph_file_type_fluorimeter" ; best = cnt ; }
	init () ;
	
	setupXYpair ( sf ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = "t_graph_file_type_xypair" ; best = cnt ; }
	init () ;
	
	return r ;
	}    

void TGraphDisplay::SetupDummy ()
	{
//	stringField sf = readTextfile ( "C:\\Dokumente und Einstellungen\\DSP\\Desktop\\RTS Kristin.csv" ) ;
//	setupFluorimeterGraph ( sf ) ;

	setupRawFPLC ( "C:\\Dokumente und Einstellungen\\DSP\\Desktop\\HADI\\G6" ) ;

//	stringField sf = readTextfile ( "C:\\Dokumente und Einstellungen\\DSP\\Desktop\\0.1 IGF-VK + 0.4 PolyGluTyr 360nm.csv" ) ;
//	setupPhotometerGraph ( sf ) ;


/*
 	scales.push_back ( new TGraphScale ( 0 , 0 , true , false , "x" , "min" , *wxBLUE ) ) ;
 	scales.push_back ( new TGraphScale ( 0 , 0 , false , true , "y" , "cm" , *wxRED ) ) ;
 	data.push_back ( new TGraphData ( this ) ) ;
 	data[0]->SetScales ( scales[0] , scales[1] ) ;

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

	AutoScale () ;
	}

void TGraphDisplay::AutoScale ()
	{
	if ( data.size() == 0 ) return ;
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
	setupCompleted = true ;
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
	if ( !IsSetupComplete() ) return ;
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
	dc.DrawRectangle ( inner ) ;
	
	wxRect o2 = outer ;
	for ( a = 0 ; a < scales.size() ; a++ )
  		scales[a]->drawit ( dc , o2 , inner ) ;
	
	dc.DestroyClippingRegion () ;
	
	if ( mode & GRAPH_DRAW_MAP )
		{
		dc.SetClippingRegion ( inner ) ;
		showDraggingRect ( dc ) ;
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
	
void TGraphDisplay::showDraggingRect ( wxDC &dc )
	{
	if ( draggingRect.x == -1 ) return ;
	dc.SetPen ( *wxBLACK_PEN ) ;
	dc.SetBrush ( *MYBRUSH ( prettyColor ) ) ;
	dc.DrawRectangle ( draggingRect ) ;
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

	if ( !IsSetupComplete() ) return ;
	
        {
        wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
        drawit ( dc2 ) ;
        }    
    }

void TGraphDisplay::OnEvent(wxMouseEvent& event)
    {
	if ( !IsSetupComplete() ) return ;
    wxPoint pt(event.GetPosition());
    
    // Find out where the mouse is
    int a , b ;
    bool doRefresh = event.Leaving() ;
    TGraphScale *new_scale = NULL ;
    TGraphData *new_data = NULL ;
    
    // Over a scale?
    for ( a = 0 ; a < scales.size() ; a++ )
    	{
    	scales[a]->selected = false ;
    	if ( scales[a]->outline.Inside ( pt ) ) new_scale = scales[a] ;
     	}
   	if ( new_scale ) new_scale->selected = true ;
   	
   	// Inside the graph?
   	if ( inner.Inside ( pt ) )
   		{
	    // Status text with coordinates of mouse pointer
	    wxString c1 ;
	    for ( a = 0 ; a < scales.size() ; a++ )
	    	{
 	    	if ( !c1.IsEmpty() ) c1 += "; " ;
 	    	c1 += scales[a]->name ;
 	    	c1 += ":" ;
	    	int v = scales[a]->horizontal ? pt.x : pt.y ;
 	    	c1 += wxString::Format ( "%f" , scales[a]->GetVirtualCoordinate ( v , inner ) ) ;
	    	}    
    	myapp()->frame->SetStatusText ( c1.c_str() , 0 ) ;
    	
    	
	    int best = 100 ;
	    for ( a = 0 ; a < data.size() ; a++ )
	    	{
 	    	data[a]->selected = false ;
 	    	int d = data[a]->Distance ( pt ) ;
 	    	if ( d < best )
 	    		{
  		    	new_data = data[a] ;
  		    	best = d ;
 	    		}    
    		if ( best >= 4 ) new_data = NULL ; // Not good enough!
	    	}    
    	if ( new_data ) new_data->selected = true ;
   		}    
   		
    // Dragging (CTRL)?
    bool showMiniature = false ;
    if ( inner.Inside ( pt ) && !event.ControlDown() && zx*zy != 10000 && 
    		event.Dragging() && !event.RightIsDown() && !event.MiddleIsDown() )
    	{
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
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

   	// End of dragging (box)?
   	else if ( inner.Inside ( pt ) && event.LeftUp() && draggingRect.x != -1 )
   		{
   		SetCursor(*wxSTANDARD_CURSOR) ;
	    int x1 = draggingRect.GetLeft() ;
	    int x2 = draggingRect.GetRight() ;
	    int y1 = draggingRect.GetTop() ;
	    int y2 = draggingRect.GetBottom() ;
	    if ( x1 > x2 ) { int temp = x1 ; x1 = x2 ; x2 = temp ; }
	    if ( y1 > y2 ) { int temp = y1 ; y1 = y2 ; y2 = temp ; }
	    int nx , ny ;
	    for ( a = 0 ; a < scales.size() ; a++ )
	    	{
 	    	float _top , _bottom ;
 	    	if ( scales[a]->horizontal )
 	    		{
  		    	_top = scales[a]->GetVirtualCoordinate ( x2 , inner ) ;
  		    	_bottom = scales[a]->GetVirtualCoordinate ( x1 , inner ) ;
  		    	nx = 100 * ( scales[a]->max - scales[a]->min ) / ( _top - _bottom ) ;
 	    		}
        	else    
 	    		{
  		    	_top = scales[a]->GetVirtualCoordinate ( y1 , inner ) ;
  		    	_bottom = scales[a]->GetVirtualCoordinate ( y2 , inner ) ;
  		    	ny = 100 * ( scales[a]->max - scales[a]->min ) / ( _top - _bottom ) ;
 	    		}
    		scales[a]->top = _top ;
    		scales[a]->bottom = _bottom ;
	    	}    
    	draggingRect.x = -1 ;
    	
    	zx = nx ;
    	zy = ny ;
    	g->zoom_x->SetValue ( zx ) ;
    	g->zoom_y->SetValue ( zy ) ;
    	g->zoom_linked->SetValue ( zx == zy ) ;
   		}

   	// Dragging (box)?
    else if ( inner.Inside ( pt ) && event.ControlDown() &&
    		event.Dragging() && !event.RightIsDown() && !event.MiddleIsDown() )
    	{
	    SetCursor ( *wxCROSS_CURSOR ) ;
	    if ( draggingRect.x == -1 )
	    	{
 	    	draggingRect.x = pt.x ;
 	    	draggingRect.y = pt.y ;
 	    	draggingRect.width = 0 ;
 	    	draggingRect.height = 0 ;
	    	}
	    else
     		{
 		    draggingRect.width = pt.x - draggingRect.x ;
 		    draggingRect.height = pt.y - draggingRect.y ;
 		    if ( event.ShiftDown() )
 		    	{
   	    		if ( draggingRect.width > draggingRect.height )
   	    			draggingRect.height = draggingRect.width ;
    			else draggingRect.width = draggingRect.height ;
 		    	}    
       		}
    	}    
   	else
    	{
     	draggingRect.x = -1 ;
     	if ( inner.Inside ( pt ) ) SetCursor(wxCursor(wxCURSOR_HAND)) ;
     	else SetCursor(*wxSTANDARD_CURSOR) ;
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
        drawit ( dc2 , mode ) ;
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

void TGraphDisplay::OnCharHook ( wxKeyEvent& event )
	{/*
 	if ( event.ControlDown() ) SetCursor ( *wxCROSS_CURSOR ) ;
// 	else if ( event.ShiftDown() ) 
 	else SetCursor(wxCursor(wxCURSOR_HAND)) ;
 	wxMessageBox ( "!" ) ;*/
	}    

void TGraphDisplay::OnSwapSides(wxCommandEvent &event)
	{
	if ( !old_scale ) return ;
	old_scale->left = !old_scale->left ;
	wxPaintEvent ev ;
	OnPaint ( ev ) ;
	}

void TGraphDisplay::UpdateDisplay ()
	{
    wxClientDC dc ( this ) ;
        {
        wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
        drawit ( dc2 ) ;
        }    
	}
    
bool TGraphDisplay::IsSetupComplete()
	{
	return setupCompleted && scales.size()>0 && data.size()>0 ;
	}
    
