// TClone.cpp: implementation of the TClone class.
//
//////////////////////////////////////////////////////////////////////

#include "TClone.h"

int TClone::cmp ( const wxString &s1 , const wxString &s2 )
    {
    return s1.CmpNoCase ( s2 ) ;
    }

int TClone_Gene::cmp ( const wxString &s1 , const wxString &s2 )
    {
    return s1.CmpNoCase ( s2 ) ;
    }


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void TClone::loadEnzymeList ( TStorage *st , wxString filename )
    {
	wxFile f ( filename , wxFile::read ) ;
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
    
    v->setSequence ( sequence ) ;
    v->setName ( name ) ;
    v->setDescription ( description ) ;
    v->setCircular ( !isLinear ) ;

    // Genes'n'stuff
    for ( a = 0 ; a < genes.size() ; a++ )
        {
        wxString t = genes[a].type ;
        int ty = VIT_MISC;
        if ( t == "GENE" ) ty = VIT_GENE ;
        long gf = genes[a].begin ;
        long gt = genes[a].end ;
        
        if ( genes[a].getCCW() ) { long gg = gf ; gf = gt ; gt = gg ; }
        
        wxString sname = genes[a].shortname ;
        if ( sname.IsEmpty() ) sname = genes[a].fullname ;
        TVectorItem vi ( sname ,
                         genes[a].fullname ,
                         gf ,
                         gt ,
                         ty ) ;
        vi.setDirection ( genes[a].getCCW()?-1:1 );
        v->items.push_back ( vi ) ;
        }
        
    // Restriction enzymes
    wxArrayString vs ;
    for ( a = 0 ; a < enzymes.size() ; a++ )
        {
        for ( b = 0 ; b < vs.GetCount() && vs[b] != enzymes[a].name ; b++ ) ;
        if ( b == vs.GetCount() )
           vs.Add ( enzymes[a].name ) ;
        }
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        TRestrictionEnzyme *e = LS->getRestrictionEnzyme(vs[a]) ;
        if ( e )
           v->re.Add ( e ) ;
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
	linear_e1 = linear_e2 = linear_s1 = linear_s2 = "" ;
	genes.clear () ;
	enzymes.clear () ;
}

void TClone::parseLines ( wxArrayString &v , char *t , long l ) 
{
	char *c , *d ;
	for ( c = d = t ; c-t < l ; c++ )
	{
		if ( !*c ) *c = 1 ;
		if ( *c == 10 || *c == 13 )
		{
			*c = 0 ;
			v.Add ( d ) ;
			c += 2 ;
			d = c ;
			c-- ;
		} 
	} 
}

void TClone::separateNames ( wxString &s1 , wxString &s2 )
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

void TClone::load ( wxString s ) 
{
	wxFile f ( s , wxFile::read ) ;
	long l = f.Length() ;
	char *t = new char [l+15] ;
	f.Read ( t , l ) ;
	f.Close() ;
	
	wxArrayString v ;
	parseLines ( v , t , l ) ;
	delete t ;

	int i ;
	success = true ;
	if ( v.GetCount() < 3 || v[0].IsEmpty() || v[0].length() > 50 )
	{
	    success = false ;
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
		enzymes.push_back ( TClone_Enzyme () ) ;
		enzymes[a].name = v[cnt++] ;
		enzymes[a].position = a2i ( v[cnt++] ) ;
	}

	// Genes
	n = a2i ( v[cnt++] ) ;
	for ( a = 0 ; a < n ; a++ )
	{
		genes.push_back ( TClone_Gene () ) ;
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
		genes.push_back ( TClone_Gene () ) ;
		genes[genes.size()-1].fullname = v[cnt++] ;
		genes[genes.size()-1].begin = a2i ( v[cnt++] ) ;
		genes[genes.size()-1].end = 0 ;
		genes[genes.size()-1].direction = v[cnt++] ;
		genes[genes.size()-1].five = "" ;
		genes[genes.size()-1].type = "MARK" ;
		separateNames ( genes[genes.size()-1].fullname , genes[genes.size()-1].shortname ) ;
	}

	// The Rest
	wxString st ;
	while ( cnt < v.GetCount() )
	{
		st = v[cnt++] ;
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
			linear_e1 = v[cnt++] ;
			linear_e2 = v[cnt++] ;
			linear_s1 = v[cnt++] ;
			linear_s2 = v[cnt++] ;
		}
	}
	
//	sort ( enzymes.begin() , enzymes.end() ) ;
    changed = false ;
    if ( sequence.IsEmpty() ) success = false ;
}

void TClone::save ( wxString s )
{
    /*
	if ( !changed ) return ;
	int a , b ;
	wxString end ;
	end += char ( 13 ) ;
	end += char ( 10 ) ;
	if ( s != "" ) filename = s ;
	wxFile out ( filename , wxFile::write ) ;
	out.Write ( name + end ) ;
	out.Write ( wxString::Format("%d",size) + end ) ;
	out.Write ( wxString::Format("%d",enzymes.size()) + end ) ;
	for ( a = 0 ; a < enzymes.size() ; a++ )
	{
	    // UNFINISHED BEYOND THIS POINT!
		out << enzymes[a].name << end ;
		out << enzymes[a].position << end ;
	}

	for ( a = b = 0 ; a < genes.size() ; a++ )
		if ( !cmp ( genes[a].type , "GENE" ) )
			b++ ;
	out << b << end ;
	for ( a = 0 ; a < genes.size() ; a++ )
	{
		if ( !cmp ( genes[a].type , "GENE" ) )
		{
			out << genes[a].shortname << (char)0 << genes[a].fullname << end ;
			out << genes[a].begin << end ;
			out << genes[a].end << end ;
			out << genes[a].direction << end ;
			out << genes[a].five << end ;
		}
	}
	for ( a = b = 0 ; a < genes.size() ; a++ )
		if ( !cmp ( genes[a].type , "MARK" ) )
			b++ ;
	out << b << end ;
	for ( a = 0 ; a < genes.size() ; a++ )
	{
		if ( !cmp ( genes[a].type , "MARK" ) )
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
		out << linear_e1 << end ;
		out << linear_e2 << end ;
		out << linear_s1 << end ;
		out << linear_s2 << end ;
	}
	if ( sequence != "" ) out << "sequence" << end << sequence << end ;
	if ( description != "" ) out << "description" << end << description << end ;
	changed = false ;
	*/
}

bool TClone_Gene::getCCW()
{
	if ( !cmp ( direction , "L" ) ) return true ;
	else if ( !cmp ( direction , "CCW" ) ) return true ;
	return false ;
}

void TClone_Gene::setCCW(bool x)
{
	if ( !cmp ( type , "MARK" ) && x ) direction = "L" ;
	else if ( !cmp ( type , "MARK" ) && !x ) direction = "R" ;
	else if ( !cmp ( type , "GENE" ) && x ) direction = "CCW" ;
	else if ( !cmp ( type , "GENE" ) && !x ) direction = "CW" ;
}

wxString TClone::getGeneSequence(int i)
{
	if ( sequence.IsEmpty() ) return "" ;
	wxString s ;
	for ( uint a = genes[i].begin ; a != genes[i].end ; a++ )
	{
		a %= sequence.length() ;
		s += sequence[a] ;
	}
	return s ;
}

void TClone::setGeneSequence(int i, wxString s) // VERY BASIC, NO LENGTH CHANGES ALLOWED!!!
{
	unsigned int a , b ;
	for ( a = 0 ; a < s.length() ; a++ )
	{
		b = genes[i].begin + a ;
		b %= sequence.length() ;
		sequence[b] = s[a] ;
	}
}


int TClone_Gene::getRealBegin()
{
	return getCCW()?end:begin ;
}

int TClone_Gene::getRealEnd()
{
	return !getCCW()?end:begin ;
}

TClone_Gene::TClone_Gene ( const TClone_Gene &g )
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
