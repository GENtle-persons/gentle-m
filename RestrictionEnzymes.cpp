#include "RestrictionEnzymes.h"

bool operator < ( const TRestrictionCut &c1 , const TRestrictionCut &c2 )
    {
    return c1.pos > c2.pos ;
    }
    
bool operator == ( const TRestrictionCut &c1 , const TRestrictionCut &c2 )
    {
    return c1.pos == c2.pos ;
    }
    
bool TRestrictionEnzyme::differ ( TRestrictionEnzyme &e )
    {
    if ( name != e.name ) return true ;
    if ( note != e.note ) return true ;
    if ( sequence != e.sequence ) return true ;
    if ( location != e.location ) return true ;
    if ( cut != e.cut ) return true ;
    if ( overlap != e.overlap ) return true ;
    return false ;
    }
    
wxString TRestrictionEnzyme::getEndUpperLeft ()
    {
    wxString r ;
    for ( int a = 0 ; a < cut ; a++ )
        r += sequence.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::getEndLowerLeft ()
    {
    wxString r , s = invertSequence () ;
    for ( int a = 0 ; a < cut+overlap ; a++ )
        r += s.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::getEndUpperRight ()
    {
    wxString r ;
    for ( int a = cut ; a < sequence.length() ; a++ )
        r += sequence.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::getEndLowerRight ()
    {
    wxString r , s = invertSequence () ;
    for ( int a = cut+overlap ; a < s.length() ; a++ )
        r += s.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::invertSequence ()
    {
    TVector v ;
    v.setSequence ( sequence ) ;
    return v.transformSequence ( true , false ) . c_str() ;
    }
    


//------------------------------------------------------------------------------

void TRestrictionCut::linearUpdate ( int w , int h )
    {
    p.x = lp.x * w / STANDARDRADIUS + 2 ;
    p.y = lp.y * h / STANDARDRADIUS ;
    lastrect = wxRect ( p.x - lastrect.GetWidth() ,
                        p.y - lastrect.GetHeight() ,
                        lastrect.GetWidth() ,
                        lastrect.GetHeight() ) ;
    }
    
wxString TRestrictionCut::getNameAndPosition ()
    {
    return wxString::Format ( "%s:%d" , getDisplayName().c_str() , pos ) ;
    }

bool TRestrictionCut::isHidden ( TVector *v )
    {
    return v->isEnzymeHidden ( e->name ) ;
    }

wxString TRestrictionCut::getDisplayName ()
	{
	if ( display_name.IsEmpty() ) return e->name ;
	return display_name + ")" ;
	}    
	
bool TRestrictionCut::join ( TRestrictionCut *c )
	{
	if ( pos != c->pos ) return false ;
	if ( e->sequence != c->e->sequence ) return false ;
	if ( e->cut != c->e->cut ) return false ;
	if ( e->overlap != c->e->overlap ) return false ;
	if ( display_name.IsEmpty() )
		display_name = e->name + " (" + c->e->name ;
	else display_name += ", " + c->e->name ;
	return true ;
	}    
        
//------------------------------------------------------------------------------

TProtease::TProtease ( wxString _name , wxString m , wxString _note )
    {
    name = _name ;
    str_match = m ;
    wxString s ;
    int a ;
    for ( a = 0 ; a < m.length() ; a++ )
        {
        char ma = m.GetChar(a) ;
        if ( ma == ',' || ma == ' ' || ma == '|' )
           {
           if ( !s.IsEmpty() ) match.Add ( s ) ;
           s = "" ;
           if ( ma == '|' ) cut = match.GetCount() - 1 ;
           }
        else s += m.GetChar(a) ;
        }
    if ( !s.IsEmpty() ) match.Add ( s ) ;
    use = true ;
    note = _note ;
    }



bool TProtease::does_match ( wxString s )
    {
    if ( s.length() != len() ) return false ;
    int a , b ;
    for ( a = 0 ; a < len() ; a++ )
        {
        bool yes = true , found = false ;
        wxString m = match[a] ;
        for ( b = 0 ; b < m.length() && !found ; b++ )
           {
           if ( m.GetChar(b) == '!' ) yes = !yes ;
           else if ( !yes && s.GetChar(a) == m.GetChar(b) ) return false ;
           else if ( yes && ( s.GetChar(a) == m.GetChar(b) || m.GetChar(b) == '*' ) ) found = true ;
           }
        if ( !found && yes ) return false ;
        }
    return true ;
    }
    
