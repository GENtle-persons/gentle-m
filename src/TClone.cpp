// TClone.cpp: implementation of the TClone class.
//
//////////////////////////////////////////////////////////////////////

#include "TClone.h"

int TClone::cmp ( const wxString &s1 , const wxString &s2 ) const
    {
    return s1.CmpNoCase ( s2 ) ;
    }

int TClone_Gene::cmp ( const wxString &s1 , const wxString &s2 ) const
    {
    return s1.CmpNoCase ( s2 ) ;
    }


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void TClone::loadEnzymeList ( TStorage * const st , const wxString& filename )
    {
    wxPrintf( "D: TClone::loadEnzymeList - start (filename: %s)\n" , filename ) ;
    wxFile f ( filename , wxFile::read ) ;
    size_t l = f.Length() ;
    char *t = new char [l+15] ;
    if (! t)
        {
        wxPrintf( "E: TClone::loadEnzymeList: Could not allocate %lu bytes of memory.\n" , l ) ;
        abort() ;
        }
    f.Read ( t , l ) ;
    f.Close() ;

    vector <TRestrictionEnzyme> vr ;
    for ( char *d = t ; d < t+l ; )
        {
        TRestrictionEnzyme r ;
        wxString r_name , r_sequence ;
        for ( int a = 0 ; a < 8 ; a++ )
            {
            if ( *d )
                {
                r_name += *d ;
                }
            d++ ;
            }
        r.setName ( r_name ) ;
        for ( int a = 0 ; a < 16 ; a++ )
            {
            if ( *d >= '0' && *d <= 'z' )
                {
                r_sequence += *d ;
                }
            d++ ;
            }
        r.setSequence ( r_sequence ) ;

        r.setOverlap ( (int) *d++ ) ;
        if ( r.getOverlap() > 127 )
             r.setOverlap ( 255 - r.getOverlap() ) ;
        d++ ; // ??? Total length ???
        r.setCut ( (int) *d++ ) ;
        if ( r.getOverlap() < 0 ) r.setCut ( r.getCut() - r.getOverlap() ) ;

        if ( r_sequence.IsEmpty() )
            {
            wxPrintf( "D: TClone::loadEnzymeList - empty sequence, skipping enzyme '%s'\n" , r_name ) ;
            abort() ;
            }
        vr.push_back ( r ) ;
        }
    delete [] t ;

    for ( unsigned int i = 0 ; i < vr.size() ; i++ )
     {
       if ( !st->getRestrictionEnzyme(vr[i].getName()) )
           {

           if ( vr[i].getName().IsEmpty() )
               {
               wxPrintf( "D: TClone::loadEnzymeList - Not copying restriction enzyme with no name at position i=%u\n" , i ) ;
               continue ;
               }

           if ( vr[i].getSequence().IsEmpty() )
               {
               wxPrintf( "D: TClone::loadEnzymeList - Not copying restriction enzyme '%s' with no sequence at position i=%u\n" , vr[i].getName() , i ) ;
               abort() ;
               }

           TRestrictionEnzyme *r = new TRestrictionEnzyme ;

           *r = vr[i] ;

           if ( r->getSequence().IsEmpty() )
               {
               wxPrintf( "D: TClone::loadEnzymeList - Restriction enzyme '%s' lost sequence after copying at position i=%u\n" , vr[i].getName() , i ) ;
               abort() ;
               }

           st->addRestrictionEnzyme ( r ) ;
           }
       }
    wxPrintf( "D: TClone::loadEnzymeList - end (filename: %s)\n" , filename ) ;
    }

void TClone::remap ( TVector * const v ) const
    {
    wxPrintf( "D: TClone::remap (vector %s) - start\n" , v->getName() ) ;
    v->updateDisplay() ;

    v->setSequence ( sequence ) ;
    v->setName ( name ) ;
    v->setDescription ( description ) ;
    v->setCircular ( !isLinear ) ;

    // Genes'n'stuff
    for ( int a = 0 ; a < genes.size() ; a++ )
        {
        wxString t = genes[a].type ;
        int ty = VIT_MISC;
        if ( t == _T("GENE") ) ty = VIT_GENE ;
        long gf = genes[a].begin ;
        long gt = genes[a].end ;

        if ( genes[a].getCCW() ) { long gg = gf ; gf = gt ; gt = gg ; }

        wxString sname = genes[a].shortname ;
        if ( sname.IsEmpty() )
            {
            sname = genes[a].fullname ;
            }
        TVectorItem vi ( sname , genes[a].fullname , gf , gt , ty ) ;
        vi.setDirection ( genes[a].getCCW()?-1:1 );
        v->items.push_back ( vi ) ;
        }

    // Restriction enzymes
    wxArrayString vs ;
    for ( int a = 0 ; a < enzymes.size() ; a++ )
        {
        int b;
        for ( b = 0 ; b < vs.GetCount() && vs[b] != enzymes[a].name ; b++ ) ;
        if ( b == vs.GetCount() )
           vs.Add ( enzymes[a].name ) ;
        }
    for ( int a = 0 ; a < vs.GetCount() ; a++ )
        {
        wxString n = vs[a] ;
        TRestrictionEnzyme *e = LS->getRestrictionEnzyme( n ) ;
        if ( e->getSequence().IsEmpty() )
            {
            wxPrintf( "D: TClone::remap enzyme %s from LS->getRestrictionEnzyme(%s) has no sequence\n" , e->getName() , n ) ;
            abort() ; 
            }
        if ( e )
           v->re.Add ( e ) ;
        }
    v->recalculateCuts () ;
    wxPrintf( "D: TClone::remap (vector %s) - end\n" , v->getName() ) ;
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
    filename = _T("") ;
    name = _T("") ;
    sequence = _T("") ;
    description = _T("") ;
    size = -1 ;
    isLinear = false ;
    linear_e1 = linear_e2 = linear_s1 = linear_s2 = _T("") ;
    genes.clear () ;
    enzymes.clear () ;
    }

void TClone::parseLines ( wxArrayString &v , char * const t , const long l ) const
    {
    for ( char *c=t, *d=t ; c-t < l ; c++ )
        {
        if ( !*c ) *c = 1 ;
        if ( *c == 10 || *c == 13 )
            {
            *c = 0 ;
            v.Add ( wxString ( d , *wxConvCurrent ) ) ;
            c += 2 ;
            d = c ;
            c-- ;
            }
        }
    }

void TClone::separateNames ( wxString &s1 , wxString &s2 ) const
    {
    char * t = new char [ s1.length() + 5 ] ;
    strcpy ( t , s1.mb_str() ) ;

    char* c ;
    for (c = t ; *c && *c != 1 ; c++ ) ;
    if ( *c )
        {
        *c = 0 ;
        s1 = wxString ( c+1 , *wxConvCurrent ) ;
        s2 = wxString ( t , *wxConvCurrent ) ;
        }
    else
        {
        wxPrintf( "W: TClone::separateNames: Not performing actual separation (s1='%s').\n" , s1 ) ;
        }

    delete [] t ;
    }

void TClone::load ( const wxString& s )
    {
    wxPrintf( "D: TClone::load(%s) - start\n" , s ) ;
   
    wxFile f ( s , wxFile::read ) ;
    long l = f.Length() ;
    char *t = new char [l+15] ;
    f.Read ( t , l ) ;
    f.Close() ;

    wxArrayString v ;
    parseLines ( v , t , l ) ;
    delete [] t ;

    success = true ;
    if ( v.GetCount() < 3 || v[0].IsEmpty() || v[0].length() > 50 )
        {
        success = false ;
        return ;
        }

    cleanup () ;

    int n , cnt ;
    filename = s ;

    // Basic Data
    name = v[0] ;
    size = a2i ( v[1] ) ;
    cnt = 2 ;

    // Cutting Enzymes
    n = a2i ( v[cnt++] ) ;
    for ( int a = 0 ; a < n ; a++ )
        {
        enzymes.push_back ( TClone_Enzyme () ) ;
        enzymes[a].name = v[cnt++] ;
        enzymes[a].position = a2i ( v[cnt++] ) ;
        }

    // Genes
    n = a2i ( v[cnt++] ) ;
    for ( int a = 0 ; a < n ; a++ )
        {
        genes.push_back ( TClone_Gene () ) ;
        genes[a].fullname = v[cnt++] ;
        genes[a].begin = a2i ( v[cnt++] ) ;
        genes[a].end = a2i ( v[cnt++] ) ;
        genes[a].direction = v[cnt++] ;
        genes[a].five = v[cnt++] ;
        genes[a].type = _T("GENE") ;
        separateNames ( genes[a].fullname , genes[a].shortname ) ;
        }

    // Mark
    n = a2i ( v[cnt++] ) ;
    for ( int a = 0 ; a < n ; a++ )
        {
        genes.push_back ( TClone_Gene () ) ;
        genes[genes.size()-1].fullname = v[cnt++] ;
        genes[genes.size()-1].begin = a2i ( v[cnt++] ) ;
        genes[genes.size()-1].end = 0 ;
        genes[genes.size()-1].direction = v[cnt++] ;
        genes[genes.size()-1].five = _T("") ;
        genes[genes.size()-1].type = _T("MARK") ;
        separateNames ( genes[genes.size()-1].fullname , genes[genes.size()-1].shortname ) ;
        }

    // The Rest
    wxString st ;
    while ( cnt < v.GetCount() )
        {
        st = v[cnt++] ;
        if ( !cmp ( st , _T("sequence") ) )
            {
            sequence = v[cnt++] ;
            }
        else if ( !cmp ( st , _T("description") ) )
            {
            description = v[cnt++] ;
            }
        else if ( !cmp ( st , _T("linear") ) )
            {
            isLinear = true ;
            linear_e1 = v[cnt++] ;
            linear_e2 = v[cnt++] ;
            linear_s1 = v[cnt++] ;
            linear_s2 = v[cnt++] ;
            }
        }

//    sort ( enzymes.begin() , enzymes.end() ) ;
    if ( sequence.IsEmpty() ) success = false ;

    wxPrintf( "D: TClone::load(%s) - end\n" , s ) ;
    }

void TClone::save ( const wxString& s )
    {
    wxASSERT_MSG(false,"TClone::save not implemented");
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

bool TClone_Gene::getCCW() const
    {
    if ( !cmp ( direction , _T("L") ) ) return true ;
    else if ( !cmp ( direction , _T("CCW") ) ) return true ;
    return false ;
    }

void TClone_Gene::setCCW(const bool x)
    {
    if ( !cmp ( type , _T("MARK") ) && x ) direction = _T("L") ;
    else if ( !cmp ( type , _T("MARK") ) && !x ) direction = _T("R") ;
    else if ( !cmp ( type , _T("GENE") ) && x ) direction = _T("CCW") ;
    else if ( !cmp ( type , _T("GENE") ) && !x ) direction = _T("CW") ;
    }
/*
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
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        int b = genes[i].begin + a ;
        b %= sequence.length() ;
        sequence[b] = s[a] ;
        }
    }
*/

int TClone_Gene::getRealBegin() const
    {
    return getCCW()?end:begin ;
    }

int TClone_Gene::getRealEnd() const
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
    shortname = g.shortname ;
    type = g.type ;
    }
