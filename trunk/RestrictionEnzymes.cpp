#include "RestrictionEnzymes.h"

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
    
string TRestrictionEnzyme::getEndUpperLeft ()
    {
    string r ;
    for ( int a = 0 ; a < cut ; a++ )
        r += sequence[a] ;
    return r ;
    }
    
string TRestrictionEnzyme::getEndLowerLeft ()
    {
    string r , s = invertSequence () ;
    for ( int a = 0 ; a < cut+overlap ; a++ )
        r += s[a] ;
    return r ;
    }
    
string TRestrictionEnzyme::getEndUpperRight ()
    {
    string r ;
    for ( int a = cut ; a < sequence.length() ; a++ )
        r += sequence[a] ;
    return r ;
    }
    
string TRestrictionEnzyme::getEndLowerRight ()
    {
    string r , s = invertSequence () ;
    for ( int a = cut+overlap ; a < s.length() ; a++ )
        r += s[a] ;
    return r ;
    }
    
string TRestrictionEnzyme::invertSequence ()
    {
    TVector v ;
    v.sequence = sequence ;
    return v.transformSequence ( true , false ) ;
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
    
string TRestrictionCut::getNameAndPosition ()
    {
    char t[1000] ;
    sprintf ( t , "%s:%d" , e->name.c_str() , pos ) ;
    return t ;
    }

bool TRestrictionCut::isHidden ( TVector *v )
    {
    return v->isEnzymeHidden ( e->name ) ;
    }
        
//------------------------------------------------------------------------------

TProtease::TProtease ( string _name , string m , string _note )
    {
    name = _name ;
    str_match = m ;
    string s ;
    int a ;
    for ( a = 0 ; a < m.length() ; a++ )
        {
        if ( m[a] == ',' || m[a] == ' ' || m[a] == '|' )
           {
           if ( s != "" ) match.push_back ( s ) ;
           s = "" ;
           if ( m[a] == '|' ) cut = match.size() - 1 ;
           }
        else s += m[a] ;
        }
    if ( s != "" ) match.push_back ( s ) ;
    use = true ;
    note = _note ;
    }



bool TProtease::does_match ( string s )
    {
    if ( s.length() != len() ) return false ;
    int a , b ;
    for ( a = 0 ; a < len() ; a++ )
        {
        bool yes = true , found = false ;
        string m = match[a] ;
        for ( b = 0 ; b < m.length() && !found ; b++ )
           {
           if ( m[b] == '!' ) yes = !yes ;
           else if ( !yes && s[a] == m[b] ) return false ;
           else if ( yes && ( s[a] == m[b] || m[b] == '*' ) ) found = true ;
           }
        if ( !found && yes ) return false ;
        }
    return true ;
    }
    
