#include "TGraph.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(TGraphDisplay, wxPanel)
    EVT_KEY_DOWN(TGraphDisplay::OnCharHook)
    EVT_KEY_UP(TGraphDisplay::OnCharHook)
    EVT_CHAR(TGraphDisplay::OnCharHook)
    EVT_PAINT(TGraphDisplay::OnPaint)
    EVT_MOUSE_EVENTS(TGraphDisplay::OnEvent)
    EVT_MENU(GRAPH_SWAP_SIDE,TGraphDisplay::OnSwapSides)
    EVT_MENU(GRAPH_COPY_AS_IMAGE,TGraphDisplay::OnCopyAsImage)
    EVT_MENU(GRAPH_SAVE_AS_IMAGE,TGraphDisplay::OnSaveAsImage)
END_EVENT_TABLE()

//******************************************************** TGraphDisplay    
    
TGraphDisplay::TGraphDisplay ( wxWindow *parent , int id )
	: wxPanel ( parent , id )
	{
 	colors.Add ( _T("BLUE") ) ;
 	colors.Add ( _T("RED") ) ;
 	colors.Add ( _T("GREEN") ) ;
 	colors.Add ( _T("MAGENTA") ) ;
 	styles.Add ( _T("rect") ) ;
 	styles.Add ( _T("circle") ) ;
 	styles.Add ( _T("triangle") ) ;
 	
 	scaleTypes.Add ( _T("linear") ) ;
 	
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
	file.Open ( *(myapp()->isoconv) ) ;
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
	    	s.Replace ( _T(",") , _T(".") ) ;
      		while ( s.First ( '\t' ) != -1 )
      			{
					wxString xyz = s.BeforeFirst ( '\t' ) ;
	    	    	as.push_back ( string ( xyz.mb_str() ) ) ;
   	 	    	s = s.AfterFirst ( '\t' ) ;
      			}    
	    		as.push_back ( string ( s.mb_str() ) ) ;
      		sf.push_back ( as ) ;
  			}
      	else
       		{
      		while ( s.First ( ',' ) != -1 )
      			{
    	    	as.push_back ( string (s.BeforeFirst ( ',' ).mb_str()) ) ;
    	    	s = s.AfterFirst ( ',' ) ;
      			}    
    			as.push_back ( string ( s.mb_str() ) ) ;
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

 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , _T("m/z") , _T("") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , _T("rel. Int.") , _T("") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	
 	TGraphData *ng = new TGraphData ( this ) ;
	ng->name = _T("IPC") ;
	ng->SetScales ( sx , sy ) ;
	ng->pointStyle = _T("none") ;
	ng->col = wxTheColourDatabase->Find ( _T("BLUE") ) ;
	data.push_back ( ng ) ;

 	int a ;
 	for ( a = 0 ; a < sf.size() && sf[a].size() > 1 && sf[a][0] != "" ; a++ )
 		{
    	double x , y ;
    	wxString s0 = wxString ( sf[a][0].c_str() , wxConvUTF8 ) ;
    	wxString s1 = wxString ( sf[a][1].c_str() , wxConvUTF8 ) ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	ng->Add ( (float) x , (float) y ) ;
 		}
	}
    
void TGraphDisplay::setupXYpair ( const stringField &sf )
	{
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , _T("X") , _T("") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , _T("Y") , _T("") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
	addNewGraph ( sf , txt("t_data") , sx , sy , 0 ) ; 	
 	} 	

void TGraphDisplay::setupPhotometerGraph ( const stringField &sf )
	{
	if ( sf.size() < 3 ) return ;
	if ( sf[0].size() < 2 ) return ;
	if ( sf[1].size() < 2 ) return ;
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , wxString ( sf[1][0].c_str() , wxConvUTF8 ) , _T("") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , wxString ( sf[1][1].c_str() , wxConvUTF8 ) , _T("") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	
 	TGraphData *ng = new TGraphData ( this ) ;
	ng->name = wxString ( sf[0][0].c_str() , wxConvUTF8 ) ;
	ng->SetScales ( sx , sy ) ;
	ng->pointStyle = _T("none") ;
	ng->col = wxTheColourDatabase->Find ( _T("BLUE") ) ;
	data.push_back ( ng ) ;

 	int a ;
 	for ( a = 2 ; a < sf.size() && sf[a].size() > 1 && sf[a][0] != "" ; a++ )
 		{
    	double x , y ;
    	wxString s0 ( sf[a][0].c_str() , wxConvUTF8 ) ;
    	wxString s1 ( sf[a][1].c_str() , wxConvUTF8 ) ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	ng->Add ( (float) x , (float) y ) ;
 		}    
 	} 	

void TGraphDisplay::setupFluorimeterGraph ( const stringField &sf )
	{
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , txt("t_wavelength") , _T("nm") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , txt("t_intensity") , _T("au") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	
 	int cnt_col = 0 , cnt_sty = 0 ;
	
	int a , b ;
	for ( a = 0 ; a+2 < sf[0].size() ; a += 2 )
		{
  		TGraphData *ng = new TGraphData ( this ) ;
  		ng->name = wxString ( sf[0][a].c_str() , wxConvUTF8 ) ;
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
        	wxString s0 ( sf[b][a+0].c_str() , wxConvUTF8 ) ;
        	wxString s1 ( sf[b][a+1].c_str() , wxConvUTF8 ) ;
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
	as.Add ( txt("t_graph_file_type_photometer") ) ;
	as.Add ( txt("t_graph_file_type_fluorimeter") ) ;
	as.Add ( txt("t_graph_file_type_xypair") ) ;
	as.Add ( txt("t_graph_file_type_bio") ) ;
	as.Add ( txt("t_graph_file_type_bio_csv") ) ;
	as.Add ( txt("t_graph_file_type_duf") ) ;

	wxString *vs = new wxString [ as.GetCount() ] ;
	for ( a = 0 ; a < as.GetCount() ; a++ )
		vs[a] = as[a] ;
	
	wxSingleChoiceDialog scd ( this , txt("t_graph_open_text") , txt("t_graph_open_title") , as.GetCount() , vs ) ;
	wxString guess = tryall ( filename ) ;
	if ( guess != _T("") )
		{
  		for ( a = 0 ; a < as.GetCount() ; a++ )
  			{
	    	if ( as[a] == txt(guess) )
	    		scd.SetSelection ( a ) ;
  			}    
		}
	else return false ;
	if ( wxID_OK != scd.ShowModal() ) return false ;
	
	wxString s = scd.GetStringSelection() ;
	if ( s == txt("t_graph_file_type_photometer") ) setupPhotometerGraph ( readTextfile ( filename ) ) ;
	if ( s == txt("t_graph_file_type_fluorimeter") ) setupFluorimeterGraph ( readTextfile ( filename ) ) ;
	if ( s == txt("t_graph_file_type_xypair") ) setupXYpair ( readTextfile ( filename ) ) ;
	if ( s == txt("t_graph_file_type_bio") ) setupBioFormat ( filename ) ;
	if ( s == txt("t_graph_file_type_bio_csv") ) setupBioCSVFormat ( readTextfile ( filename ) ) ;
	if ( s == txt("t_graph_file_type_duf") ) setupDUF ( filename ) ;
	
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
    
void TGraphDisplay::addRawData ( unsigned char *d , long l , wxString title )
	{
	stringField sf ;
	TVS b ;
	b.push_back ( "" ) ;
	b.push_back ( "" ) ;
	
	long a , sum = 0 , integrate = 10 , cnt = 0 ;
	for ( a = 2000+1 ; a+30 < l ; a += 4 )
		{
  		unsigned long x = 0 ;
  		x += (unsigned long) d[a+0] ; x <<= 8 ;
  		x += (unsigned long) d[a+1] ; x <<= 8 ;
//  		x += (unsigned long) d[a+2] ; x <<= 8 ;
//  		x += (unsigned long) d[a+3] ;
  		sum += x / integrate;
  		if ( cnt >= integrate )
  			{
  			b[0] = wxString::Format ( _T("%d") , a ) . mb_str() ;
  			b[1] = wxString::Format ( _T("%d") , sum ) . mb_str() ;
  			sf.push_back ( b ) ;
  			sum = 0 ;
  			cnt = 0 ;
   			} 	
        cnt++ ;		
		}    
	
	delete d ;
	addNewGraph ( sf , title , scales[0] , scales[1] , 0 ) ;
	}	

void TGraphDisplay::addRawData2 ( unsigned char *d , long l , wxString title )
	{
	stringField sf ;
	TVS b ;
	b.push_back ( "" ) ;
	b.push_back ( "" ) ;
	
	long a , sum = 0 , integrate = 10 , cnt = 0 ;
	for ( a = 2000+1 ; a+30 < l ; a += 4 )
		{
  		unsigned long x = 0 ;
  		x |= (unsigned long) d[a+0] ; x <<= 8 ;
  		x |= (unsigned long) d[a+1] ; x <<= 8 ;
  		x |= (unsigned long) d[a+2] ; x <<= 8 ;
  		x |= (unsigned long) d[a+3] ;
  		
  		signed long y = (signed long) x ;
  		y /= 10 ;
  		
//  		sum += x / integrate;
//  		if ( cnt >= integrate )
  			{
  			b[0] = wxString::Format ( _T("%d") , a ) . mb_str() ;
  			b[1] = wxString::Format ( _T("%l") , &y ) . mb_str() ;
  			sf.push_back ( b ) ;
  			sum = 0 ;
  			cnt = 0 ;
   			} 	
        cnt++ ;		
		}    
	
	addNewGraph ( sf , title , scales[0] , scales[1] , 0 ) ;
	}	

void TGraphDisplay::addNewGraph ( const stringField &sf , wxString title , TGraphScale *sx , TGraphScale*sy , int startrow )
	{
 	TGraphData *ng = new TGraphData ( this ) ;
	ng->name = title ;
	ng->SetScales ( sx , sy ) ;
	ng->pointStyle = _T("none") ;
	ng->col = wxTheColourDatabase->Find ( colors[data.size()%colors.size()] ) ;
	data.push_back ( ng ) ;

 	for ( int a = startrow ; a < sf.size() && sf[a].size() > 1 && sf[a][0] != "" ; a++ )
 		{
    	double x , y ;
    	wxString s0 ( sf[a][0].c_str() , wxConvUTF8 ) ;
    	wxString s1 ( sf[a][1].c_str() , wxConvUTF8 ) ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	ng->Add ( (float) x , (float) y ) ;
 		}    
	}
    
void TGraphDisplay::setupRawFPLC ( wxString filenamebase )
	{
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , _T("X") , _T("") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , _T("Y") , _T("") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;

	long l ;
	unsigned char *d ;

	d = readRawData ( filenamebase + _T("_1.DAT") , l ) ;
	addRawData ( d , l , _T("1") ) ;

	d = readRawData ( filenamebase + _T("_2.DAT") , l ) ;
	addRawData ( d , l , _T("2") ) ;
	}    

wxString TGraphDisplay::tryall ( wxString filename )
	{
	wxString r ;
	int best = 0 ;
	int a , cnt ;
	stringField sf = readTextfile ( filename ) ;
	init () ;
/*
	setupDUF ( filename ) ; // Not working yet
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = _T("t_graph_file_type_duf") ; best = cnt ; }
	init () ;
*/	
	setupPhotometerGraph ( sf ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = _T("t_graph_file_type_photometer") ; best = cnt ; }
	init () ;
	
	setupFluorimeterGraph ( sf ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = _T("t_graph_file_type_fluorimeter") ; best = cnt ; }
	init () ;
	
	setupXYpair ( sf ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = _T("t_graph_file_type_xypair") ; best = cnt ; }
	init () ;
	
	setupBioFormat ( filename ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = _T("t_graph_file_type_bio") ; best = cnt ; }
	init () ;
	
	setupBioCSVFormat ( sf ) ;
	for ( a = cnt = 0; a < data.size() ; a++ ) cnt += data[a]->dx.size() ;
	if ( cnt > best ) { r = _T("t_graph_file_type_bio_csv") ; best = cnt ; }
	init () ;
	
	if ( best < 5 ) r = _T("") ; // A graph with less points is useless
	
	return r ;
	}    

void TGraphDisplay::setupBioCSVFormat ( const stringField &sf )
	{
	int a ;
	for ( a = 0 ; a < sf.size() ; a++ )
		{
		if ( sf[a].size() != 3 ) continue ;
		if ( sf[a][0] != "Time" ) continue ;
		if ( sf[a][1] != "UV" ) continue ;
		if ( sf[a][2] != "Conductivity" ) continue ;
		break ;
		}
	
	if ( a >= sf.size() ) return ; // Wrong file type, no need to bother any more

 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , txt("t_biorad_csv_time") , _T("s") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , txt("t_biorad_csv_UV") , _T("") , *wxBLACK ) ;
  	TGraphScale *sz = new TGraphScale ( 0 , 0 , false , false , txt("t_biorad_csv_conductivity") , _T("mS/cm") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;
 	scales.push_back ( sz ) ;
 	
 	TGraphData *ng1 = new TGraphData ( this ) ;
	ng1->name = wxString ( "UV" , wxConvUTF8 ) ;
	ng1->SetScales ( sx , sy ) ;
	ng1->pointStyle = _T("none") ;
	ng1->col = wxTheColourDatabase->Find ( _T("BLUE") ) ;
	
 	TGraphData *ng2 = new TGraphData ( this ) ;
	ng2->name = wxString ( "Conductivity" , wxConvUTF8 ) ;
	ng2->SetScales ( sx , sz ) ;
	ng2->pointStyle = _T("none") ;
	ng2->col = wxTheColourDatabase->Find ( _T("RED") ) ;
	
	data.push_back ( ng1 ) ;
	data.push_back ( ng2 ) ;
/*
 	int a ;
 	for ( a = 2 ; a < sf.size() && sf[a].size() > 1 && sf[a][0] != "" ; a++ )
 		{
    	double x , y ;
    	wxString s0 ( sf[a][0].c_str() , wxConvUTF8 ) ;
    	wxString s1 ( sf[a][1].c_str() , wxConvUTF8 ) ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	ng->Add ( (float) x , (float) y ) ;
 		}    
*/
	
	for ( a++ ; a < sf.size() ; a++ )
		{
		if ( sf[a].size() != 3 ) break ;
    	double x , y , z ;
    	wxString s0 ( sf[a][0].c_str() , wxConvUTF8 ) ;
    	wxString s1 ( sf[a][1].c_str() , wxConvUTF8 ) ;
    	wxString s2 ( sf[a][2].c_str() , wxConvUTF8 ) ;
    	s0.ToDouble ( &x ) ;
    	s1.ToDouble ( &y ) ;
    	s2.ToDouble ( &z ) ;
    	ng1->Add ( (float) x , (float) y ) ;		
    	ng2->Add ( (float) x , (float) z ) ;		
		}
	}

void TGraphDisplay::setupBioFormat ( wxString filenamebase )
	{
	if ( filenamebase.Right(4).Upper() != _T(".BIO") ) return ;
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , _T("X") , _T("") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , _T("Y") , _T("") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;

	long l ;
	unsigned char *d ;

	d = readRawData ( filenamebase , l ) ;
	addRawData2 ( d , l , _T("1") ) ;
	delete d ;
	}

/*
 * Not used
 */
void TGraphDisplay::setupDUF ( wxString filenamebase )
	{
	if ( filenamebase.Right(4).Upper() != _T(".DUF") ) return ;
 	TGraphScale *sx = new TGraphScale ( 0 , 0 , true , false , _T("X") , _T("") , *wxBLACK ) ;
  	TGraphScale *sy = new TGraphScale ( 0 , 0 , false , true , _T("Y") , _T("") , *wxBLACK ) ;
 	scales.push_back ( sx ) ;
 	scales.push_back ( sy ) ;

	long l ;
	unsigned char *d ;

	d = readRawData ( filenamebase , l ) ;
	
	wxString text ;
	unsigned long pos = 40 ;
	unsigned long w ;
	do {
		w = d[pos] ;
		if ( w == 0 ) break ;
		pos += 4 ;
		wxString s = (char*) d+pos ;
		s = s.BeforeFirst ( ':' ) ;
		text += s ;
		pos += 10 + w ;
	} while ( w > 0 ) ;
	
	pos += 8*5*21 ;
	
	for ( int b = 0 ; b < 8 ; b++ )
		{
		double *x = (double*) (d+b+pos) ;
		wxMessageBox ( wxString::Format ( "%d : %f" , b , *x ) ) ;
		}

	// Add data
 	TGraphData *ng1 = new TGraphData ( this ) ;
	ng1->name = wxString ( "1" , wxConvUTF8 ) ;
	ng1->SetScales ( sx , sy ) ;
	ng1->pointStyle = _T("none") ;
	ng1->col = wxTheColourDatabase->Find ( _T("BLUE") ) ;
	
 	TGraphData *ng2 = new TGraphData ( this ) ;
	ng2->name = wxString ( "2" , wxConvUTF8 ) ;
	ng2->SetScales ( sx , sy ) ;
	ng2->pointStyle = _T("none") ;
	ng2->col = wxTheColourDatabase->Find ( _T("GREEN") ) ;

 	TGraphData *ng3 = new TGraphData ( this ) ;
	ng3->name = wxString ( "3" , wxConvUTF8 ) ;
	ng3->SetScales ( sx , sy ) ;
	ng3->pointStyle = _T("none") ;
	ng3->col = wxTheColourDatabase->Find ( _T("BLUE") ) ;

	data.push_back ( ng1 ) ;
	data.push_back ( ng2 ) ;
	data.push_back ( ng3 ) ;

	for ( int a = 0 ; a < 20 ; a++ )
		{
		double *x = (double*) (d+pos) ;
		double *y1 = (double*) (d+pos+8*2) ;
		double *y2 = (double*) (d+pos+8*3) ;
		double *y3 = (double*) (d+pos+8*4) ;
    	ng1->Add ( (float) *x , (float) *y1 ) ;
    	ng2->Add ( (float) *x , (float) *y2 ) ;
    	ng3->Add ( (float) *x , (float) *y3 ) ;
		pos += 8 * 5 ;
		}
	
	delete d ;
	}

void TGraphDisplay::SetupDummy ()
	{
//	setupRawFPLC ( _T("C:\\Dokumente und Einstellungen\\DSP\\Desktop\\NORBERT\\G23") ) ;
	setupBioFormat ( _T("C:\\Dokumente und Einstellungen\\DSP\\Desktop\\Sterner FPLC\\e.BIO") ) ;
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
    for ( a = 0 ; !event.Dragging() && a < scales.size() ; a++ )
    	{
    	scales[a]->selected = false ;
    	if ( scales[a]->outline.Inside ( pt ) ) new_scale = scales[a] ;
     	}
   	if ( new_scale )
			{
			new_scale->selected = true ;
   		if ( event.LeftDClick() )
   			{
				TGraphDialog gd ( g , txt("t_graph_dialog") ) ;
				gd.ShowScale ( new_scale ) ;
				if ( wxID_OK == gd.ShowModal() ) UpdateDisplay () ;
				return ;
				}
			}
   	
   	// Inside the graph?
   	if ( inner.Inside ( pt ) )
   		{
	    // Status text with coordinates of mouse pointer
	    wxString c1 ;
	    for ( a = 0 ; a < scales.size() ; a++ )
	    	{
 	    	if ( !c1.IsEmpty() ) c1 += _T("; ") ;
 	    	c1 += scales[a]->name ;
 	    	c1 += _T(":") ;
	    	int v = scales[a]->horizontal ? pt.x : pt.y ;
 	    	c1 += wxString::Format ( _T("%f") , scales[a]->GetVirtualCoordinate ( v , inner ) ) ;
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

   		if ( event.LeftDClick() )
   			{
				TGraphDialog gd ( g , txt("t_graph_dialog") ) ;
				if ( new_data ) gd.ShowData ( new_data ) ;
				if ( wxID_OK == gd.ShowModal() ) UpdateDisplay () ;
				return ;
				}

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
   	else if ( inner.Inside ( pt ) && event.LeftUp() && draggingRect.x != -1  )
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
   	else if ( !event.ControlDown() )
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
    	else
    		{
		    cm->Append ( GRAPH_COPY_AS_IMAGE , txt("m_graph_copy_as_image") ) ;
		    cm->Append ( GRAPH_SAVE_AS_IMAGE , txt("m_graph_save_as_image") ) ;
    		}    
        PopupMenu ( cm , pt ) ;
        delete cm ;    
    	}   
    	
   	mouse_pos = pt ; 
    }    

void TGraphDisplay::OnCharHook ( wxKeyEvent& event )
	{
 	if ( event.ControlDown() ) SetCursor ( *wxCROSS_CURSOR ) ;
// 	else if ( event.ShiftDown() ) 
 	else SetCursor(wxCursor(wxCURSOR_HAND)) ;
	}    

void TGraphDisplay::OnSwapSides(wxCommandEvent &event)
	{
	if ( !old_scale ) return ;
	old_scale->left = !old_scale->left ;
	wxPaintEvent ev ;
	OnPaint ( ev ) ;
	}
	
void TGraphDisplay::DrawIntoBitmap ( wxBitmap &bmp )
	{
	int w , h ;
	g->GetClientSize ( &w , &h ) ;
	wxBitmap bmp2 ( w , h , 24 ) ;
	wxMemoryDC memdc ;
	memdc.SelectObject ( bmp2 ) ;
	drawit ( memdc ) ;
	bmp = bmp2 ;
	}    

void TGraphDisplay::OnCopyAsImage(wxCommandEvent &event)
	{
    if (wxTheClipboard->Open())
      {
      wxBitmap bmp ;
      DrawIntoBitmap ( bmp ) ;
      wxTheClipboard->SetData( new wxBitmapDataObject ( bmp ) );
      wxTheClipboard->Close();
      }
	}
	
void TGraphDisplay::OnSaveAsImage(wxCommandEvent &event)
	{
 	wxString filename = _T("Graph") ;
 	wxBitmap bmp ;
 	DrawIntoBitmap ( bmp ) ;
	myapp()->frame->saveImage ( &bmp , filename ) ;
	}
	
void TGraphDisplay::UpdateDisplay ()
	{
    wxClientDC dc ( this ) ;
	wxBufferedDC dc2 ( &dc , dc.GetSize() ) ;
	drawit ( dc2 ) ;
	}
    
bool TGraphDisplay::IsSetupComplete()
	{
	return setupCompleted && scales.size()>0 && data.size()>0 ;
	}
    
