// TClone.cpp: implementation of the TClone class.
//
//////////////////////////////////////////////////////////////////////

#include "TClone.h"

int TClone::cmp ( const string &s1 , const string &s2 )
    {
    wxString t1 ( s1.c_str() ) ;
    return t1.CmpNoCase ( s2.c_str() ) ;
    }

int TGene::strcmpi ( const string &s1 , const string &s2 )
    {
    wxString t1 ( s1.c_str() ) ;
    return t1.CmpNoCase ( s2.c_str() ) ;
    }


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void TClone::loadEnzymeList ( TStorage *st , string filename )
    {
	wxFile f ( filename.c_str() , wxFile::read ) ;
	long l = f.Length() ;
	char *t = new char [l+15] ;
	f.Read ( t , l ) ;
	f.Close() ;
	
	vector <TRestrictionEnzyme> vr ;
	char *d ;
	int i ;
	for ( d = t ; d < t+l ; )
	 {
	 int a ;
	 TRestrictionEnzyme r ;
	 for ( a = 0 ; a < 8 ; a++ )
	   if ( *d ) r.name += *d++ ;
	   else d++ ;
	 for ( a = 0 ; a < 16 ; a++ )
       {
	   if ( *d >= '0' && *d <= 'z' ) r.sequence += *d++ ;
	   else d++ ;
	   }
	   
     r.overlap = (int) *d++ ;
     if ( r.overlap > 127 )
          r.overlap = 255 - r.overlap ;
     d++ ; // ??? Total length ???
     r.cut = (int) *d++ ;
     if ( r.overlap < 0 ) r.cut -= r.overlap ;
     
     vr.push_back ( r ) ;
	 }
	delete t ;

	for ( i = 0 ; i < vr.size() ; i++ )
	 {
	   if ( !st->getRestrictionEnzyme(vr[i].name) )
	       {
	       TRestrictionEnzyme *r = new TRestrictionEnzyme ;
	       *r = vr[i] ;
	       st->addRestrictionEnzyme ( r ) ;
           }
	   }
    }

void TClone::remap ( TVector *v )
{
    int a , b ;
    v->recalcvisual = true ;
    
    v->sequence = sequence.c_str() ;
    v->name = name.c_str() ;
    v->desc = description.c_str() ;
    v->setCircular ( !isLinear ) ;

    // Genes'n'stuff
    for ( a = 0 ; a < genes.size() ; a++ )
        {
        wxString t = genes[a].type.c_str() ;
        int ty = VIT_MISC;
        if ( t == "GENE" ) ty = VIT_GENE ;
        long gf = genes[a].begin ;
        long gt = genes[a].end ;
        
        if ( genes[a].getCCW() ) { long gg = gf ; gf = gt ; gt = gg ; }
        
        wxString sname = genes[a].shortname.c_str() ;
        if ( sname == "" ) sname = genes[a].fullname.c_str() ;
        TVectorItem vi ( sname ,
                         genes[a].fullname.c_str() ,
                         gf ,
                         gt ,
                         ty ) ;
        vi.direction = genes[a].getCCW()?-1:1;
        v->items.push_back ( vi ) ;
        }
        
    // Restriction enzymes
    vector <string> vs ;
    for ( a = 0 ; a < enzymes.size() ; a++ )
        {
        for ( b = 0 ; b < vs.size() && vs[b] != enzymes[a].name ; b++ ) ;
        if ( b == vs.size() )
           vs.push_back ( enzymes[a].name ) ;
        }
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        TRestrictionEnzyme *e = LS->getRestrictionEnzyme(vs[a]) ;
        if ( e )
           v->re.push_back ( e ) ;
        }
    v->recalculateCuts () ;
}

TClone::TClone()
{
	cleanup () ;
}

TClone::~TClone()
{

}

void TClone::cleanup ()
{
	filename = "" ;
	name = "" ;
	sequence = "" ;
	description = "" ;
	size = -1 ;
	isLinear = false ;
	changed = false ;
	linear.e1 = linear.e2 = linear.s1 = linear.s2 = "" ;
	while ( genes.size() ) genes.pop_back () ;
	while ( enzymes.size() ) enzymes.pop_back () ;
}

void TClone::parseLines ( vector <string> &v , char *t , long l ) 
{
	char *c , *d ;
	for ( c = d = t ; c-t < l ; c++ )
	{
		if ( !*c ) *c = 1 ;
		if ( *c == 10 || *c == 13 )
		{
			*c = 0 ;
			v.push_back ( d ) ;
			c += 2 ;
			d = c ;
			c-- ;
		} 
	} 
}

void TClone::separateNames ( string &s1 , string &s2 )
{
	char *t , *c ;
	t = new char [ s1.length() + 5 ] ;
	strcpy ( t , s1.c_str() ) ;

	for ( c = t ; *c && *c != 1 ; c++ ) ;
	if ( *c )
	{
		*c = 0 ;
		s1 = c+1 ;
		s2 = t ;
	}

	delete t ;
}

void TClone::load ( string s ) 
{
	wxFile f ( s.c_str() , wxFile::read ) ;
	long l = f.Length() ;
	char *t = new char [l+15] ;
	f.Read ( t , l ) ;
	f.Close() ;
	
	vector <string> v ;
	parseLines ( v , t , l ) ;
	delete t ;

	int i ;
	success = true ;
	if ( v.size() < 3 || v[0] == "" || v[0].length() > 50 )
	{
	    success = false ;
//		wxMessageBox ( "Dis ain't a CLONE standard plasmid file!" , "File error" ) ;
		return ;
	}
	
	cleanup () ;

	int a , n , cnt ;
	filename = s ;
	
	// Basic Data
	name = v[0] ;
	size = a2i ( v[1] ) ;
	cnt = 2 ;

	// Cutting Enzymes
	n = a2i ( v[cnt++] ) ;
	for ( a = 0 ; a < n ; a++ )
	{
		enzymes.push_back ( TEnzyme () ) ;
		enzymes[a].name = v[cnt++] ;
		enzymes[a].position = a2i ( v[cnt++] ) ;
	}

	// Genes
	n = a2i ( v[cnt++] ) ;
	for ( a = 0 ; a < n ; a++ )
	{
		genes.push_back ( TGene () ) ;
		genes[a].fullname = v[cnt++] ;
		genes[a].begin = a2i ( v[cnt++] ) ;
		genes[a].end = a2i ( v[cnt++] ) ;
		genes[a].direction = v[cnt++] ;
		genes[a].five = v[cnt++] ;
		genes[a].type = "GENE" ;
		separateNames ( genes[a].fullname , genes[a].shortname ) ;
	}

	// Mark
	n = a2i ( v[cnt++] ) ;
	for ( a = 0 ; a < n ; a++ )
	{
		genes.push_back ( TGene () ) ;
		genes[genes.size()-1].fullname = v[cnt++] ;
		genes[genes.size()-1].begin = a2i ( v[cnt++] ) ;
		genes[genes.size()-1].end = 0 ;
		genes[genes.size()-1].direction = v[cnt++] ;
		genes[genes.size()-1].five = "" ;
		genes[genes.size()-1].type = "MARK" ;
		separateNames ( genes[genes.size()-1].fullname , genes[genes.size()-1].shortname ) ;
	}

	// The Rest
	string st ;
	while ( cnt < v.size() )
	{
		st = v[cnt++].c_str() ;
		if ( !cmp ( st , "sequence" ) )
		{
			sequence = v[cnt++] ;
		}
		else if ( !cmp ( st , "description" ) )
		{
			description = v[cnt++] ;
		}
		else if ( !cmp ( st , "linear" ) )
		{
			isLinear = true ;
			linear.e1 = v[cnt++] ;
			linear.e2 = v[cnt++] ;
			linear.s1 = v[cnt++] ;
			linear.s2 = v[cnt++] ;
		}
	}
	
//	sort ( enzymes.begin() , enzymes.end() ) ;
changed = false ;
}

void TClone::save ( string s )
{
	if ( !changed ) return ;
	int a , b ;
	string end ;
	end += char ( 13 ) ;
	end += char ( 10 ) ;
	if ( s != "" ) filename = s ;
	ofstream out ( filename.c_str() , ios::out | ios::binary ) ;
	out << name << end ;
	out << size << end ;
	out << enzymes.size() << end ;
	for ( a = 0 ; a < enzymes.size() ; a++ )
	{
		out << enzymes[a].name << end ;
		out << enzymes[a].position << end ;
	}

	for ( a = b = 0 ; a < genes.size() ; a++ )
		if ( !strcmpi ( genes[a].type.c_str() , "GENE" ) )
			b++ ;
	out << b << end ;
	for ( a = 0 ; a < genes.size() ; a++ )
	{
		if ( !strcmpi ( genes[a].type.c_str() , "GENE" ) )
		{
			out << genes[a].shortname << (char)0 << genes[a].fullname << end ;
			out << genes[a].begin << end ;
			out << genes[a].end << end ;
			out << genes[a].direction << end ;
			out << genes[a].five << end ;
		}
	}
	for ( a = b = 0 ; a < genes.size() ; a++ )
		if ( !strcmpi ( genes[a].type.c_str() , "MARK" ) )
			b++ ;
	out << b << end ;
	for ( a = 0 ; a < genes.size() ; a++ )
	{
		if ( !strcmpi ( genes[a].type.c_str() , "MARK" ) )
		{
			out << genes[a].shortname << (char)0 << genes[a].fullname << end ;
			out << genes[a].begin << end ;
			out << genes[a].end << end ;
			out << genes[a].direction << end ;
			out << genes[a].five << end ;
		}
	}
	if ( isLinear )
	{
		out << "linear" << end ;
		out << linear.e1 << end ;
		out << linear.e2 << end ;
		out << linear.s1 << end ;
		out << linear.s2 << end ;
	}
	if ( sequence != "" ) out << "sequence" << end << sequence << end ;
	if ( description != "" ) out << "description" << end << description << end ;
	changed = false ;
}

/*
void TClone::visualize ()
{
	int a , b ;
	string s , tab ;
	char t[1000] ;
	tab = " " ;
	tab[0] = 9 ;

	SendDlgItemMessage ( hwin , IDC_EDIT_NAME , WM_SETTEXT , 0 , (long) name.c_str() ) ;
	SendDlgItemMessage ( hwin , IDC_EDIT_FILENAME , WM_SETTEXT , 0 , (long) filename.c_str() ) ;
	SendDlgItemMessage ( hwin , IDC_EDIT_DESCRIPTION , WM_SETTEXT , 0 , (long) description.c_str() ) ;
	SendDlgItemMessage ( hwin , IDC_EDIT_SEQUENCE , WM_SETTEXT , 0 , (long) sequence.c_str() ) ;

	if ( sequence != "" ) size = sequence.length() ;
	itoa ( size , t , 10 ) ;
	SendDlgItemMessage ( hwin , IDC_EDIT_SIZE , WM_SETTEXT , 0 , (long) t ) ;

	SendDlgItemMessage ( hwin , IDC_LIST_ENZYMES , LB_RESETCONTENT , 0 , 0 ) ;
	for ( a = 0 ; a < enzymes.size() ; a++ )
	{
		strcpy ( t , enzymes[a].name.c_str() ) ;
		b = strlen ( t ) ;
		t[b] = 9 ;
		itoa ( enzymes[a].position , t+b+1 , 10 ) ;
		SendDlgItemMessage ( hwin , IDC_LIST_ENZYMES , LB_ADDSTRING , 0 , (long) t ) ;
	}

	SendDlgItemMessage ( hwin , IDC_LIST_GENES , LB_RESETCONTENT , 0 , 0 ) ;
	for ( a = 0 ; a < genes.size() ; a++ )
	{
		s = genes[a].shortname + tab ;
		itoa ( genes[a].begin , t , 10 ) ;
		s += t + tab ;
		itoa ( genes[a].end , t , 10 ) ;
		s += t + tab ;
		s += genes[a].direction + tab ;
		s += genes[a].five + tab ;
		s += genes[a].type + tab ;
		s += genes[a].fullname ;
		SendDlgItemMessage ( hwin , IDC_LIST_GENES , LB_ADDSTRING , 0 , (long) s.c_str() ) ;
	}

	if ( isLinear ) 
	{
		SendDlgItemMessage ( hwin , IDC_CHECK_LINEAR , BM_SETCHECK , BST_CHECKED , 0 ) ;
	}
	else 
	{
		SendDlgItemMessage ( hwin , IDC_CHECK_LINEAR , BM_SETCHECK , BST_UNCHECKED , 0 ) ;
	}
	SendDlgItemMessage ( hwin , IDC_EDIT_L1 , WM_SETTEXT , 0 , (long) linear.e1.c_str() ) ;
	SendDlgItemMessage ( hwin , IDC_EDIT_L2 , WM_SETTEXT , 0 , (long) linear.e2.c_str() ) ;
	SendDlgItemMessage ( hwin , IDC_EDIT_L3 , WM_SETTEXT , 0 , (long) linear.s1.c_str() ) ;
	SendDlgItemMessage ( hwin , IDC_EDIT_L4 , WM_SETTEXT , 0 , (long) linear.s2.c_str() ) ;


	showPlasmid ( *thisDlg->GetDC () , CLEAR_BACKGROUND ) ;

}

int TClone::cX ( float w , float r )
{
	double pi = 3.1415926535 ;
	return (int) ( (float) sin ( w * pi / 180 ) * r ) ;
}

int TClone::cY ( float w , float r )
{
	double pi = 3.1415926535 ;
	return (int) -( (float) cos ( w * pi / 180 ) * r ) ;
}

class TDummy 
{
public :
	TDummy (string a = "" , int b = 0 , int c = 0 ) { s = a ; x = b ; y = c ; } ;
	virtual bool intersect ( TDummy &d ) ;
	string s ;
	int x , y ;
	CSize ts ;
	float f , of ;
	bool b , selected ;
} ;

bool TDummy::intersect ( TDummy &d ) 
{
	if ( x <= d.x + d.ts.cx &&
		 y <= d.y + d.ts.cy &&
		 x + ts.cx >= d.x &&
		 y + ts.cy >= d.y )
		 return true ;
	return false ;
}

void TClone::showPlasmid ( CDC &dc , int opt )
{
	if ( filename == "" ) return ;
	bool portrait = false , printBW = false , printColor = false ;
	int ox , oy , ow , oh ;
	ox = 10 ;
	oy = 186 ;
	ow = 550 ;
	oh = 340 ;

	CSize si ;
	bool printing = ( opt & PRINTING ) > 0 ;
	if ( printing )
	{
		ox = oy = 0 ;
		ow = cs.cx ;
		oh = cs.cy ;
		if ( ow < oh )
		{
			ox += ow / 20 ;
			ow = ow * 9 / 10 ;
			oh = oh / 2 ;
			portrait = true ;
		}
		if ( dc.GetDeviceCaps ( NUMCOLORS ) == 2 ) printBW = true ;
		else printColor = true ;
	}

	RECT rt ;
	rt.top = oy ; rt.left = ox ; rt.right = ox+ow ; rt.bottom = oy+oh ;
	dc.SetBkColor ( GetSysColor ( COLOR_3DFACE ) ) ;


	bool oldLinear = isLinear ;
	if ( SendDlgItemMessage ( hwin , IDC_CHECK_LINEAR , BM_GETCHECK , 0 , 0 ) == BST_CHECKED ) isLinear = true ;
	else isLinear = false ;

	if ( ( opt & CLEAR_BACKGROUND ) || ( oldLinear != isLinear ) )
		dc.FillSolidRect ( &rt , GetSysColor ( COLOR_3DFACE ) ) ;
	CPen bp ( 0 , 1 , (COLORREF) 0 ) ;
	CPen rp ( 0 , 1 , RGB ( 200 , 0 , 0 ) ) ;
	CPen bluePen ( 0 , 2 , RGB ( 0 , 0 , 200 ) ) ;
	CPen greenPen ( 0 , 2 , RGB ( 0 , 200 , 0 ) ) ;
	CPen dgp ( 0 , 1 , RGB ( 100 , 100 , 100 ) ) ;
	dc.SelectObject ( bp ) ;
	if ( !printing ) dc.IntersectClipRect ( ox , oy , ox+ow , oy+oh ) ;

	if ( isLinear )
	{
		CFont fm , fl ;
		fm.CreatePointFont ( ow*10/68 , "Times New Roman" ) ;
		fl.CreatePointFont ( ow*10/45 , "Times New Roman" ) ;
		dc.SelectObject ( fl ) ;
		dc.SetTextAlign ( TA_LEFT ) ;
		dc.SetBkMode ( TRANSPARENT ) ;
		string s = name ;
		CSize ts = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;	
		dc.TextOut ( ox + ow/2 - ts.cx/2 , oy , s.c_str() ) ;

		char t[100] ;
		itoa ( size , t , 10 ) ;
		strcat ( t , " bp" ) ;
		s = t ;
		ts = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;	
		dc.TextOut ( ox + ow/2 - ts.cx/2 , oy + ts.cy , s.c_str() ) ;

		dc.SelectObject ( fm ) ;
		s = "12345679012345" ;
		ts = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;	
		int mw = ow * 8 / 10 ;
		int mh = ts.cy + 2 ;
		int mx = ox + ow / 20 ;
		int my = oy + oh / 2 ;
		vector <TDummy> ev ;
		int a , b ;
		
		for ( a = 0 ; a < enzymes.size() ; a++ )
		{
			s = enzymes[a].name ;
			ev.push_back ( TDummy ( s , mx + ( enzymes[a].position * mw / size ) + 3 , my - mh*10 ) ) ;
			ev[a].s = s + " (" ;
			itoa ( enzymes[a].position , t , 10 ) ;
			ev[a].s += t ;
			ev[a].s += ")" ;
			ev[a].ts = dc.GetOutputTextExtent ( ev[a].s.c_str() , ev[a].s.length() ) ;		
			ev[a].ts.cx += 2 ;
		}

		bool cont ;
		for ( a = 0 ; a < ev.size() ; a++ )
		{
			cont = false ;
			for ( b = 0 ; b < a ; b++ )
				if ( ev[a].intersect ( ev[b] ) )
					cont = true ;
			if ( cont ) 
			{
				ev[a].y += mh ;
				a-- ;
			}
		}

		for ( a = b = 0 ; a < ev.size() ; a++ )
			if ( ev[a].y > b )
				b = ev[a].y ;
		b = my - b - mh * 2 ;
		for ( a = 0 ; a < ev.size() ; a++ )
			ev[a].y += b ;

		dc.SelectObject ( bp ) ;
		dc.MoveTo ( mx , my ) ;
		dc.LineTo ( mx + mw , my ) ;
		for ( a = ev.size()-1 ; a >= 0 ; a-- )
		{
			dc.FillSolidRect ( ev[a].x , ev[a].y , ev[a].ts.cx , ev[a].ts.cy , GetSysColor ( COLOR_3DFACE ) ) ;
			dc.TextOut ( ev[a].x , ev[a].y , ev[a].s.c_str() ) ;
			dc.MoveTo ( mx + enzymes[a].position * mw / size , my ) ;
			dc.LineTo ( mx + enzymes[a].position * mw / size , ev[a].y + ts.cy / 2 ) ;
			dc.LineTo ( ev[a].x , ev[a].y + ts.cy / 2 ) ;
		}

		// Genes
		for ( a = 0 ; a < genes.size() ; a++ )
		{
			int x1 , x2 ;
			if ( genes[a].type == "MARK" )
			{
				x1 = genes[a].begin  * mw / size ;
				dc.MoveTo ( mx + x1 , my + mh ) ;
				dc.LineTo ( mx + x1 , my + mh * 3 / 2 ) ;
				s = genes[a].shortname ;
				if ( s == "" ) s = genes[a].fullname ;
				ts = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;	
				dc.TextOut ( mx + x1 - ts.cx / 2 , my + mh * 2 , s.c_str() ) ;
			}
			else // GENE
			{
				x1 = genes[a].begin  * mw / size ;
				x2 = genes[a].end * mw / size ;
				dc.MoveTo ( mx + x1 , my + mh ) ;
				dc.LineTo ( mx + x2 , my + mh ) ;
				dc.LineTo ( mx + x2 - ( x2 - x1 ) / 10 , my + mh * 3 / 2 ) ;
				dc.MoveTo ( mx + x2 , my + mh ) ;
				dc.LineTo ( mx + x2 - ( x2 - x1 ) / 10 , my + mh * 1 / 2 ) ;
				s = genes[a].shortname ;
				if ( s == "" ) s = genes[a].fullname ;
				ts = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;	
				dc.TextOut ( mx + ( x1 + x2 ) / 2 - ts.cx / 2 , my + mh * 2 , s.c_str() ) ;
			}
		}

	}
	else
	{
		int mx = ox + ow/2 ;
		int my = oy + oh/2 ;
		float r = oh * 7 / 10 ;
		if ( r > ow * 2 / 3 ) r = ow * 2 / 3 ;
		r /= 2 ;
		float r2 = r * 11 / 10 , r3 = r * 13 / 10 ;

		CFont fm , fl ;
		fm.CreatePointFont ( ow*10/68 , "Times New Roman" ) ;
		fl.CreatePointFont ( ow*10/45 , "Times New Roman" ) ;
		dc.SelectObject ( fl ) ;
		dc.SetTextAlign ( TA_LEFT ) ;
		dc.SetBkMode ( TRANSPARENT ) ;
		si = dc.GetOutputTextExtent ( name.c_str() , name.length() ) ;
		CBrush bkb ( GetSysColor ( COLOR_3DFACE ) ) ;
		CBrush wb ( RGB ( 255 , 255 , 255 ) ) ;
		if ( printing ) dc.SelectObject ( wb ) ;
		else dc.SelectObject ( bkb ) ;
		if ( printColor ) dc.SelectObject ( bluePen ) ;
		dc.Ellipse ( mx-r , my-r , mx+r , my+r ) ;

		if ( printColor ) dc.SetTextColor ( RGB ( 200 , 0 , 0 ) ) ;
		char st[1000] ;
		itoa ( size , st , 10 ) ;
		strcat ( st , " bp" ) ;
		dc.TextOut ( mx-si.cx/2 , my-si.cy/2 , name.c_str() ) ;
		si = dc.GetOutputTextExtent ( st , strlen ( st ) ) ;
		dc.TextOut ( mx-si.cx/2 , my+si.cy/2 , st ) ;
		dc.SelectObject ( fm ) ;
		dc.SetTextColor ( RGB ( 0 , 0 , 0 ) ) ;

		int a , b ;
		float f ;

		vector <TDummy> ev ;
		for ( a = 0 ; a < enzymes.size() ; a++ )
		{
			b = ev.size() ;
			f = enzymes[a].position * 360 ;
			f = f / size ;
			dc.MoveTo ( mx+cX(f,r) , my+cY(f,r) ) ;
			dc.LineTo ( mx+cX(f,r2) , my+cY(f,r2) ) ;
			ev.push_back ( TDummy ( "" , mx+cX(f,r3) , my+cY(f,r3) ) ) ;
			ev[b].f = f ;
			ev[b].of = f ;
			ev[b].selected = enzymes[a].selected ;

			string s = enzymes[a].name ;
			while ( a+1 < enzymes.size() && enzymes[a].position == enzymes[a+1].position )
			{
				s += ", " + enzymes[a+1].name ;
				if ( enzymes[a+1].selected ) ev[b].selected = true ;
				a++ ;
			}

			char t[100] ;
			itoa ( enzymes[a].position , t , 10 ) ;
			if ( ev[b].f < 180 ) s = s + " (" + string ( t ) + ")" ;
			else s = "(" + string ( t ) + ") " + s ;
			ev[b].s = s ;
			ev[b].ts = dc.GetOutputTextExtent ( ev[b].s.c_str() , ev[b].s.length() ) ;
			if ( ev[b].f > 180 ) ev[b].x -= ev[b].ts.cx ;
		}

		bool changed ;
		int cnt1 = 0 ;
		do {
			changed = false ;
			for ( a = 0 ; a < ev.size() ; a++ )
			{
				for ( b = 0 ; b < ev.size() ; b++ )
				{
					if ( a != b && ev[a].intersect ( ev[b] ) )
					{
						int c = b ;
						float af = 0.5 ;
						if ( ev[a].f < ev[b].f ) c = a ;

						ev[c].f -= af ;
						if ( ev[c].f < 0 ) ev[c].f += 360 ;
						ev[c].x = mx + cX ( ev[c].f , r3 ) ;
						ev[c].y = my + cY ( ev[c].f , r3 ) ;
						if ( ev[c].f > 180 ) ev[c].x -= ev[c].ts.cx ;

						if ( c == a ) c = b ; else c = a ;
						ev[c].f += af ;
						if ( ev[c].f > 360 ) ev[c].f -= 360 ;
						ev[c].x = mx + cX ( ev[c].f , r3 ) ;
						ev[c].y = my + cY ( ev[c].f , r3 ) ;
						if ( ev[c].f > 180 ) ev[c].x -= ev[c].ts.cx ;

						changed = true ;
					}
				}
			}
			if ( cnt1++ > 100 ) changed = false ; // Safety only
			if ( enzymes.size() > 200 ) changed = false ; // More safety
		} while ( changed ) ;


		if ( !printBW ) dc.SelectObject ( dgp ) ;
		if ( printColor ) dc.SelectObject ( bluePen ) ;
		for ( a = 0 ; a < ev.size() ; a++ )
		{
			f = ev[a].of ;
			dc.MoveTo ( mx+cX(f,r2) , my+cY(f,r2) ) ;
			int nx = ev[a].x , ny = ev[a].y + ev[a].ts.cy/2 ;
			if ( ev[a].f > 180 ) nx += ev[a].ts.cx ;
			dc.LineTo ( nx , ny ) ;
		}
		for ( a = 0 ; a < ev.size() ; a++ )
		{
			if ( ev[a].selected && !printBW ) dc.SetTextColor ( RGB ( 200 , 0 , 0 ) ) ;
			else dc.SetTextColor ( RGB ( 0 , 0 , 0 ) ) ;
			if ( printColor ) dc.SetTextColor ( RGB ( 0 , 0 , 200 ) ) ;
			dc.TextOut ( ev[a].x , ev[a].y , ev[a].s.c_str() ) ;
		}

		// Drawing genes / markers
		float r9 = r * 9 / 10 , r8 = r * 8 / 10 , r7 = r * 7 / 10 ;
		for ( a = 0 ; a < genes.size() ; a++ )
		{
			if ( genes[a].selected && !printBW )
			{
				dc.SelectObject ( rp ) ;
				dc.SetTextColor ( RGB ( 200 , 0 , 0 ) ) ;
			}
			else
			{
				dc.SelectObject ( bp ) ;
				dc.SetTextColor ( RGB ( 0 , 0 , 0 ) ) ;
			}
			if ( printColor ) 
			{
				dc.SelectObject ( greenPen ) ;
				dc.SetTextColor ( RGB ( 0 , 200 , 0 ) ) ;
			}
			if ( genes[a].type == "MARK" ) // MARK
			{
				string s = genes[a].shortname ;
				if ( s == "" ) s = genes[a].fullname ;
				CSize si = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;
				dc.MoveTo ( mx + cX ( 360 * genes[a].begin / size , r9 ) , my + cY ( 360 * genes[a].begin / size , r9 ) ) ;
				dc.LineTo ( mx + cX ( 360 * genes[a].begin / size , r8 ) , my + cY ( 360 * genes[a].begin / size , r8 ) ) ;
				dc.TextOut ( mx + cX ( 360 * genes[a].begin / size , r7 ) - si.cx / 2 , my + cY ( 360 * genes[a].begin / size , r7 ) , s.c_str() ) ;
			}
			else // GENE
			{
				float wb = 360 * genes[a].begin / size ;
				float we = 360 * genes[a].end / size ;

				if ( !genes[a].getCCW() && we < wb ) we += 360 ;
				if ( genes[a].getCCW() && wb < we ) wb += 360 ;
				float wd = ( we - wb ) / 100 ;
				float ww = 10 * wd ;

				dc.MoveTo ( mx + cX ( we - ww , r8 ) , my + cY ( we - ww , r8 ) ) ;
				dc.LineTo ( mx + cX ( we - ww , (r7+r8)/2 ) , my + cY ( we - ww , (r7+r8)/2 ) ) ;
				dc.LineTo ( mx + cX ( we , (r8+r9)/2 ) , my + cY ( we , (r8+r9)/2 ) ) ;
				dc.LineTo ( mx + cX ( we - ww , (r+r9)/2 ) , my + cY ( we - ww , (r+r9)/2 ) ) ;
				dc.LineTo ( mx + cX ( we - ww , r9 ) , my + cY ( we - ww , r9 ) ) ;
				for ( b = 10 ; b < 100 ; b++ )
					dc.LineTo ( mx + cX ( we - wd * b , r9 ) , my + cY ( we - wd * b , r9 ) ) ;
				dc.LineTo ( mx + cX ( wb , r9 ) , my + cY ( wb , r9 ) ) ;
				dc.LineTo ( mx + cX ( wb , r8 ) , my + cY ( wb , r8 ) ) ;
				for ( b = 0 ; b < 90 ; b++ )
					dc.LineTo ( mx + cX ( wb + wd * b , r8 ) , my + cY ( wb + wd * b , r8 ) ) ;
				dc.LineTo ( mx + cX ( we - ww , r8 ) , my + cY ( we - ww , r8 ) ) ;

				string s = genes[a].shortname ;
				if ( s == "" ) s = genes[a].fullname ;
				CSize si = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;
				dc.TextOut ( mx - si.cx / 2 + cX ( we - wd * 50 , r7 ) , my + cY ( we - wd * 50 , r7 ) , s.c_str() ) ;
			}
		}
	}

	if ( portrait ) // Additional information on a portrait A4 page
	{
		dc.SelectObject ( bp ) ;
		dc.SetTextColor ( RGB ( 0 , 0 , 0 ) ) ;
		ox = 0 ;
		ow = cs.cx ;
		oh = cs.cy / 2 ;
		oy = oh ;
		dc.MoveTo ( ox , oy ) ;
		dc.LineTo ( ox + ow , oy ) ;
		dc.MoveTo ( ox + ow/2 , oy ) ;
		dc.LineTo ( ox + ow/2 , oy + oh ) ;
		CFont fm , fl ;
		CSize si , so ;
		string s = "gI" ; // Dummy
		char t[100] ;
		int a ;
		fl.CreatePointFont ( ow*10/45 , "Times New Roman" ) ;
		fm.CreatePointFont ( ow*10/68 , "Times New Roman" ) ;
		dc.SelectObject ( fl ) ;
		dc.SetTextAlign ( TA_LEFT ) ;
		dc.SetBkMode ( TRANSPARENT ) ;
		dc.TextOut ( ox , oy , "Enzymes" ) ;
		dc.TextOut ( ox + ow / 2 + ow / 32 , oy , "Genes" ) ;
		dc.SelectObject ( fm ) ;

		{
			CTime time ;
			time = time.GetCurrentTime () ;
			int day = time.GetDay () ;
			int month = time.GetMonth () ;
			int year = time.GetYear () ;
			sprintf ( t , "%2d.%2d.%4d" , day , month , year ) ;
		}

		s = t ;
		so = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;
		dc.TextOut ( ox + ow - so.cx , 0 , s.c_str() ) ;
		s = filename ;
		so = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;
		dc.TextOut ( ox + ow - so.cx , oy + oh - so.cy , s.c_str() ) ;

		// Enzymes
		si = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;
		si.cy += 2 ;
		int nx = ox + 1 ;
		int ny = oy + si.cy * 3 ;
		for ( a = 0 ; a < enzymes.size() ; a++ ) 
		{
			itoa ( enzymes[a].position , t , 10 ) ;
			so = dc.GetOutputTextExtent ( t , strlen ( t ) ) ;
			dc.Rectangle ( nx - 1 , ny - 2 , nx + ow / 8 , ny + si.cy - 1 ) ;
			dc.TextOut ( nx , ny , enzymes[a].name.c_str() ) ;
			dc.TextOut ( nx + ow / 8 - so.cx , ny , t ) ;
			ny += si.cy ;
			if ( ny + si.cy >= oy + oh )
			{
				ny = oy + si.cy * 3 ;
				nx += ow / 8 + ow / 20 ;
			}
		}

		// Genes
		int nz = ow / 16 ;
		nx = ox + ow / 2 + nz / 2 ;
		ny = oy + si.cy * 3 ;
		for ( a = 0 ; a < genes.size() ; a++ )
		{
			s = genes[a].fullname ;
			if ( s == "" ) s = genes[a].shortname ;

			so = dc.GetOutputTextExtent ( s.c_str() , s.length() ) ;
			dc.Rectangle ( nx - 2 , ny - 2 , ox + ow , ny + si.cy - 1 ) ;

			dc.TextOut ( nx , ny , s.c_str() ) ;
			dc.TextOut ( nx + nz*5/2 , ny , genes[a].direction.c_str() ) ;
			itoa ( genes[a].begin , t , 10 ) ;
			so = dc.GetOutputTextExtent ( t , strlen ( t ) ) ;
			dc.TextOut ( nx + nz*4 - so.cx , ny , t ) ;
			itoa ( genes[a].end , t , 10 ) ;
			so = dc.GetOutputTextExtent ( t , strlen ( t ) ) ;
			dc.TextOut ( nx + nz*5 - so.cx , ny , t ) ;
			dc.TextOut ( nx + nz*11/2 , ny , genes[a].type.c_str() ) ;
			dc.TextOut ( nx + nz*7 , ny , genes[a].five.c_str() ) ;
			ny += si.cy ;
		}
	}
}

void TClone::ligate(TClone &c)
{
	int a ;
	if ( linear.s2 == c.linear.s1 )
	{
		int osl = sequence.length() ;
		sequence += c.sequence ;
		linear.e2 = c.linear.e2 ;
		linear.s2 = c.linear.s2 ;
		if ( linear.s1 == c.linear.s2 )
		{
			isLinear = false ;
		}
		for ( a = 0 ; a < c.enzymes.size() ; a++ )
		{
			c.enzymes[a].position += osl ;
			enzymes.push_back ( c.enzymes[a] ) ;
		}
		updateCurrentEnzymes () ;
		update () ;
	}
	else if ( linear.s1 == c.linear.s2 )
	{
		sequence = c.sequence + sequence ;
		linear.e1 = c.linear.e1 ;
		linear.s1 = c.linear.s1 ;
		updateCurrentEnzymes () ;
		update () ;
	}
}

void TClone::update()
{
	if ( sequence != "" ) size = sequence.length() ;
	if ( !isLinear )
	{
		linear.s1 = "" ;
		linear.s2 = "" ;
		linear.e1 = "" ;
		linear.e2 = "" ;
	}
}

void TClone::updateCurrentEnzymes()
{
	int a , b ;
	bool cont ;
	vector <bool> vb ;
	while ( vb.size() < silmut->enzymes.size() ) vb.push_back ( false ) ;
	for ( a = 0 ; a < enzymes.size() ; a++ )
	{
		cont = true ;
		for ( b = 0 ; cont && b < silmut->enzymes.size() ; b++ )
			if ( !strcmpi ( silmut->enzymes[b].name.c_str() , enzymes[a].name.c_str() ) )
				vb[b] = cont = true ;
	}
	while ( enzymes.size() ) enzymes.pop_back () ;
	for ( a = 0 ; a < vb.size() ; a++ )
	{
		if ( vb[a] )
		{
			vector <int> cuts ;
			getRestrictionSites ( a , cuts ) ;
			for ( b = 0 ; b < cuts.size() ; b++ )
			{
				enzymes.push_back ( TEnzyme () ) ;
				enzymes[enzymes.size()-1].name = silmut->enzymes[a].name ;
				enzymes[enzymes.size()-1].position = cuts[b] ;
			}
		}
	}
	sort ( enzymes.begin() , enzymes.end() ) ;
}

void TClone::getRestrictionSites( int sel , vector <int> &cuts )
{
	while ( cuts.size() ) cuts.pop_back() ;

	int overlap = 10 ;
	int a , c ;
	string s = silmut->upstr ( sequence ) , t ;
	if ( s.length() < overlap ) return ;
	for ( a = 0 ; a < overlap ; a++ ) s += s[a] ;

	for ( a = 0 ; a < sequence.length() ; a++ )
	{
		char v = sequence[a] ;
		if ( silmut->enzymes[sel].cutFrom != -1 )
		{
			t = silmut->enzymes[sel].sequence ;
			for ( c = 0 ; c < t.length() && silmut->matchSingleNA ( s[a+c] , t[c] ) ; c++ ) ;
			if ( c == t.length () )
				cuts.push_back ( a + silmut->enzymes[sel].cutFrom ) ;
		}
	}
	
	for ( a = 0 ; a < cuts.size() ; a++ )
		cuts[a] %= sequence.length() ;
}
*/


bool TGene::getCCW()
{
	if ( !strcmpi ( direction.c_str() , "L" ) ) return true ;
	else if ( !strcmpi ( direction.c_str() , "CCW" ) ) return true ;
	return false ;
}

void TGene::setCCW(bool x)
{
	if ( !strcmpi ( type.c_str() , "MARK" ) && x ) direction = "L" ;
	else if ( !strcmpi ( type.c_str() , "MARK" ) && !x ) direction = "R" ;
	else if ( !strcmpi ( type.c_str() , "GENE" ) && x ) direction = "CCW" ;
	else if ( !strcmpi ( type.c_str() , "GENE" ) && !x ) direction = "CW" ;
}

string TClone::getGeneSequence(int i)
{
	if ( sequence == "" ) return "" ;
	string s ;
	for ( int a = genes[i].begin ; a != genes[i].end ; a++ )
	{
		a %= sequence.length() ;
		s += sequence[a] ;
	}
	return s ;
}

void TClone::setGeneSequence(int i, string s) // VERY BASIC, NO LENGTH CHANGES ALLOWED!!!
{
	int a , b ;
	for ( a = 0 ; a < s.length() ; a++ )
	{
		b = genes[i].begin + a ;
		b %= sequence.length() ;
		sequence[b] = s[a] ;
	}
}


int TGene::getRealBegin()
{
	return getCCW()?end:begin ;
}

int TGene::getRealEnd()
{
	return !getCCW()?end:begin ;
}

TGene::TGene ( const TGene &g )
{
	begin = g.begin ;
	end = g.end ;
	direction = g.direction ;
	five = g.five ;
	fullname = g.fullname ;
	selected = g.selected ;
	shortname = g.shortname ;
	type = g.type ;
}
