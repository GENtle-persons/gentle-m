#ifndef _RESTRICTIONENZYMES_H_
#define _RESTRICTIONENZYMES_H_

#include "main.h"

class TRestrictionEnzyme
    {
    public :
    bool differ ( TRestrictionEnzyme &e ) ;
    
    virtual wxString getEndUpperLeft () ;
    virtual wxString getEndLowerLeft () ;
    virtual wxString getEndUpperRight () ;
    virtual wxString getEndLowerRight () ;
    virtual wxString invertSequence () ;
    
    wxString name , sequence ;
    wxString location , note ;
    unsigned long dbid ;
    int cut , overlap ;
    } ;

class TRestrictionCut
    {
    public :
    TRestrictionCut ( int _pos , TRestrictionEnzyme *_e )
        { pos = _pos ; e = _e ; }
        
    virtual void linearUpdate ( int w , int h ) ;
    virtual wxString getNameAndPosition () ;
    virtual bool isHidden ( TVector *v ) ;
        
    int pos ;
    TRestrictionEnzyme *e ;
    
    // Visual information
    float angle , angle3 , r1 , r2 , r3 ; 
    wxRect lastrect ;
    wxPoint p , lp ;
    wxTreeItemId treeid ;
    } ;

bool operator < ( const TRestrictionCut &c1 , const TRestrictionCut &c2 ) ;
bool operator == ( const TRestrictionCut &c1 , const TRestrictionCut &c2 ) ;

    
class TProtease
    {
    public :
    TProtease ( wxString _name = "" , wxString m = "" , wxString _note = "" ) ;
    bool does_match ( wxString s ) ;
    inline int len() { return match.GetCount() ; }
    wxString name ;
    wxArrayString match ;
    int cut ; // After this, starting at 0
    bool use ;
    wxString note , str_match ;
    } ;

class TProteaseCut
    {
    public :
    TProtease *protease ;
    int cut ;
    bool left ;
    } ;
    

#endif

