/** \file
	\brief Contains the TRestrictionEnzyme, TRestrictionCut, TProtease, and TProteaseCut classes
*/
#include "RestrictionEnzymes.h"

bool operator < ( const TRestrictionCut &c1 , const TRestrictionCut &c2 )
    {
    return ( ((TRestrictionCut)c1).getPos() > ((TRestrictionCut)c2).getPos() ) || 
		( ((TRestrictionCut)c1).getPos() == ((TRestrictionCut)c2).getPos() && c1.e->getName() < c2.e->getName() ) ;
    }
    
bool operator == ( const TRestrictionCut &c1 , const TRestrictionCut &c2 )
    {
    return ((TRestrictionCut)c1).getPos() == ((TRestrictionCut)c2).getPos() && c1.e->getName() == c2.e->getName() ;
    }

bool operator < ( const TFragment &f1 , const TFragment &f2 )
    {
	 if ( f1.from < f2.from ) return true ;
	 if ( f1.from > f2.from ) return false ;
	 if ( f1.to < f2.to ) return true ;
	 return false ;
    }
    
bool operator == ( const TFragment &f1 , const TFragment &f2 )
    {
    return ( f1.from == f2.from ) && ( f1.to == f2.to ) ;
    }

/** \brief Compares two restriction enzymes
	\param e The enzyme to compare this one to
	
	If any of name, sequence, note, location, cut, or overlap do
	not match, FALSE is returned
*/
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
    
wxString TRestrictionEnzyme::getEndUpperLeft ( bool first_strand )
    {
    wxString r ;
    for ( int a = 0 ; a < getCut(first_strand) ; a++ )
        r += sequence.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::getEndLowerLeft ( bool first_strand )
    {
    wxString r , s = invertSequence () ;
    for ( int a = 0 ; a < getCut(first_strand)+getOverlap(first_strand) ; a++ )
        r += s.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::getEndUpperRight ( bool first_strand )
    {
    wxString r ;
    for ( int a = getCut(first_strand) ; a < sequence.length() ; a++ )
        r += sequence.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::getEndLowerRight ( bool first_strand )
    {
    wxString r , s = invertSequence () ;
    for ( int a = getCut(first_strand)+getOverlap(first_strand) ; a < s.length() ; a++ )
        r += s.GetChar(a) ;
    return r ;
    }
    
wxString TRestrictionEnzyme::invertSequence ()
    {
    TVector v ;
    v.setSequence ( sequence ) ;
    return v.transformSequence ( true , false ) . c_str() ;
    }
    
int TRestrictionEnzyme::getCut ( bool first_strand )
	{
	return first_strand ? cut : sequence.length() - cut - overlap ;
	}

int TRestrictionEnzyme::getOverlap ( bool first_strand )
	{
	return overlap ;
	}

void TRestrictionEnzyme::setCut ( int c ) { cut = c ; }
void TRestrictionEnzyme::setOverlap ( int o ) { overlap = o ; }
wxString TRestrictionEnzyme::getName () { return name ; }
void TRestrictionEnzyme::setName ( wxString _name ) { name = _name ; }
wxString TRestrictionEnzyme::getSequence () { return sequence ; }
bool TRestrictionEnzyme::isPalindromic () { return palindromic ; }

void TRestrictionEnzyme::setSequence ( wxString _seq )
	{
	sequence = _seq ;
    TVector v ;
    v.setSequence ( sequence ) ;
    _seq = v.transformSequence ( true , true ) . c_str() ;
	palindromic = _seq == sequence ;
	}


//------------------------------------------------------------------------------

/** \brief updates the cut position in linear display
	\param w Screen width
	\param h Screen height
*/
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
	 if ( !myapp()->frame->showEnzymePos ) return getDisplayName() ;
    return wxString::Format ( _T("%s %d") , getDisplayName().c_str() , pos ) ;
    }

bool TRestrictionCut::isHidden ( TVector *v )
    {
    return v->isEnzymeHidden ( e->getName() ) ;
    }

wxString TRestrictionCut::getDisplayName ()
	{
	if ( display_name.IsEmpty() ) return e->getName() ;
	return display_name ;
	}    

/** \brief Merges a cut with an isoenzyme for simplified display
	\param c The restriction cut to merge
*/
bool TRestrictionCut::join ( TRestrictionCut *c )
	{
	if ( pos != c->pos ) return false ;
	if ( e->getSequence() != c->e->getSequence() ) return false ;
	if ( e->getCut() != c->e->getCut() ) return false ;
	if ( e->getOverlap() != c->e->getOverlap() ) return false ;
	if ( display_name.IsEmpty() ) display_name = e->getName() ;
	display_name += _T(", ") + c->e->getName() ;
	return true ;
	}    
        
wxString TRestrictionCut::getEndUpperLeft () { return e->getEndUpperLeft ( first_strand ) ; }
wxString TRestrictionCut::getEndLowerLeft () { return e->getEndLowerLeft ( first_strand ) ; }
wxString TRestrictionCut::getEndUpperRight () { return e->getEndUpperRight ( first_strand ) ; }
wxString TRestrictionCut::getEndLowerRight () { return e->getEndLowerRight ( first_strand ) ; }
int TRestrictionCut::getCut () { return e->getCut ( first_strand ) ; }
int TRestrictionCut::getOverlap () { return e->getOverlap ( first_strand ) ; }
void TRestrictionCut::setPos ( int p ) { pos = p ; }
wxString TRestrictionCut::getSequence () { return e->getSequence () ; }

int TRestrictionCut::getPos ()
	{
	if ( first_strand ) return pos ;
	int a = pos ;
	a -= e->getCut() ;
	a += getCut() ;
//	a += e->getSequence().length() ;
//	a -= e->getCut(first_strand) ;
	return a ;
	}

int TRestrictionCut::getFrom ()
	{
	return pos - e->getCut() ;
	}

int TRestrictionCut::getTo ()
	{
	return pos - e->getCut() + e->getSequence().length() - 1 ;
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
           s = _T("") ;
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
    
