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
bool TRestrictionEnzyme::differ ( const TRestrictionEnzyme &e ) const
    {
    if ( name != e.name ) return true ;
    if ( note != e.note ) return true ;
    if ( sequence != e.sequence ) return true ;
    if ( location != e.location ) return true ;
    if ( cut != e.cut ) return true ;
    if ( overlap != e.overlap ) return true ;
    return false ;
    }

wxString TRestrictionEnzyme::getEndUpperLeft ( const bool first_strand ) const
    {
    wxString r ;
    for ( int a = 0 ; a < getCut(first_strand) ; a++ )
        r += sequence.GetChar(a) ;
    return r ;
    }

wxString TRestrictionEnzyme::getEndLowerLeft ( const bool first_strand ) const
    {
    wxString r , s = invertSequence () ;
    for ( int a = 0 ; a < getCut(first_strand)+getOverlap(first_strand) ; a++ )
        r += s.GetChar(a) ;
    return r ;
    }

wxString TRestrictionEnzyme::getEndUpperRight ( const bool first_strand ) const
    {
    wxString r ;
    for ( int a = getCut(first_strand) ; a < sequence.length() ; a++ )
        r += sequence.GetChar(a) ;
    return r ;
    }

wxString TRestrictionEnzyme::getEndLowerRight ( const bool first_strand ) const
    {
    wxString r , s = invertSequence () ;
    for ( int a = getCut(first_strand)+getOverlap(first_strand) ; a < s.length() ; a++ )
        r += s.GetChar(a) ;
    return r ;
    }

wxString TRestrictionEnzyme::invertSequence () const
    {
    TVector v ;
    v.setSequence ( sequence ) ;
    return v.transformSequence ( true , false ) . c_str() ;
    }

int TRestrictionEnzyme::getCut ( const bool first_strand ) const
    {
    return first_strand ? cut : sequence.length() - cut - overlap ;
    }

int TRestrictionEnzyme::getOverlap ( const bool first_strand ) const
    {
    return overlap ;
    }

void TRestrictionEnzyme::setCut ( const int c ) { cut = c ; }
void TRestrictionEnzyme::setOverlap ( const int o ) { overlap = o ; }
wxString TRestrictionEnzyme::getName () const { return name ; }
void TRestrictionEnzyme::setName ( const wxString& _name ) { name = _name ; }
wxString TRestrictionEnzyme::getSequence () const { return sequence ; }
bool TRestrictionEnzyme::isPalindromic () const { return palindromic ; }

void TRestrictionEnzyme::setSequence ( const wxString& sequence )
    {
    TVector v ;
    v.setSequence ( sequence ) ;
    wxString tmpSeq = v.transformSequence ( true , true ) . c_str() ;
    palindromic = (tmpSeq == sequence) ;
    }


//------------------------------------------------------------------------------

/** \brief updates the cut position in linear display
    \param w Screen width
    \param h Screen height
*/
void TRestrictionCut::linearUpdate ( const int w , const int h )
    {
    p.x = lp.x * w / STANDARDRADIUS + 2 ;
    p.y = lp.y * h / STANDARDRADIUS ;
    lastrect = wxRect ( p.x - lastrect.GetWidth() ,
                        p.y - lastrect.GetHeight() ,
                        lastrect.GetWidth() ,
                        lastrect.GetHeight() ) ;
    }

wxString TRestrictionCut::getNameAndPosition () const
    {
    if ( !myapp()->frame->showEnzymePos ) return getDisplayName() ;
    return wxString::Format ( _T("%s %d") , getDisplayName().c_str() , pos ) ;
    }

bool TRestrictionCut::isHidden ( const TVector * const v ) const
    {
    return v->isEnzymeHidden ( e->getName() ) ;
    }

wxString TRestrictionCut::getDisplayName () const
    {
    if ( display_name.IsEmpty() ) return e->getName() ;
    return display_name ;
    }

/** \brief Merges a cut with an isoenzyme for simplified display
    \param c The restriction cut to merge
*/
bool TRestrictionCut::join ( TRestrictionCut * const c )
    {
    if ( pos != c->pos ) return false ;
    if ( e->getSequence() != c->e->getSequence() ) return false ;
    if ( e->getCut() != c->e->getCut() ) return false ;
    if ( e->getOverlap() != c->e->getOverlap() ) return false ;
    if ( display_name.IsEmpty() ) display_name = e->getName() ;
    display_name += _T(", ") + c->e->getName() ;
    return true ;
    }

wxString TRestrictionCut::getEndUpperLeft () const { return e->getEndUpperLeft ( first_strand ) ; }
wxString TRestrictionCut::getEndLowerLeft () const { return e->getEndLowerLeft ( first_strand ) ; }
wxString TRestrictionCut::getEndUpperRight () const { return e->getEndUpperRight ( first_strand ) ; }
wxString TRestrictionCut::getEndLowerRight () const { return e->getEndLowerRight ( first_strand ) ; }
int TRestrictionCut::getCut () const { return e->getCut ( first_strand ) ; }
int TRestrictionCut::getOverlap () const { return e->getOverlap ( first_strand ) ; }
void TRestrictionCut::setPos ( int p ) { pos = p ; }
wxString TRestrictionCut::getSequence () const { return e->getSequence () ; }

int TRestrictionCut::getPos () const
    {
    if ( first_strand ) return pos ;
    int a = pos ;
    a -= e->getCut() ;
    a += getCut() ;
//    a += e->getSequence().length() ;
//    a -= e->getCut(first_strand) ;
    return a ;
    }

int TRestrictionCut::getFrom () const
    {
    return pos - e->getCut() ;
    }

int TRestrictionCut::getTo () const
    {
    return pos - e->getCut() + e->getSequence().length() - 1 ;
    }


//------------------------------------------------------------------------------

TProtease::TProtease ( const wxString& _name , const wxString& m , const wxString& _note )
    {
    name = _name ;
    str_match = m ;
    wxString s ;
    for ( int a = 0 ; a < m.length() ; a++ )
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



bool TProtease::does_match ( const wxString& s ) const
    {
    if ( s.length() != len() ) return false ;
    for ( int a = 0 ; a < len() ; a++ )
        {
        bool yes = true , found = false ;
        wxString m = match[a] ;
        for ( int b = 0 ; b < m.length() && !found ; b++ )
           {
           if ( m.GetChar(b) == '!' ) yes = !yes ;
           else if ( !yes && s.GetChar(a) == m.GetChar(b) ) return false ;
           else if ( yes && ( s.GetChar(a) == m.GetChar(b) || m.GetChar(b) == '*' ) ) found = true ;
           }
        if ( !found && yes ) return false ;
        }
    return true ;
    }

