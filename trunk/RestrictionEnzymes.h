#ifndef _RESTRICTIONENZYMES_H_
#define _RESTRICTIONENZYMES_H_

#include "main.h"

class TRestrictionEnzyme
    {
    public :
    bool differ ( TRestrictionEnzyme &e ) ;
    
    virtual string getEndUpperLeft () ;
    virtual string getEndLowerLeft () ;
    virtual string getEndUpperRight () ;
    virtual string getEndLowerRight () ;
    virtual string invertSequence () ;
    
    string name , sequence ;
    string location , note ;
    unsigned long dbid ;
    int cut , overlap ;
    } ;

class TRestrictionCut
    {
    public :
    TRestrictionCut ( int _pos , TRestrictionEnzyme *_e )
        { pos = _pos ; e = _e ; }
        
    virtual void linearUpdate ( int w , int h ) ;
    virtual string getNameAndPosition () ;
        
    int pos ;
    TRestrictionEnzyme *e ;
    
    // Visual information
    float angle , angle3 , r1 , r2 , r3 ; 
    wxRect lastrect ;
    wxPoint p , lp ;
    wxTreeItemId treeid ;
    } ;
    
class TProtease
    {
    public :
    TProtease ( string _name = "" , string m = "" , string _note = "" ) ;
    bool does_match ( string s ) ;
    inline int len() { return match.size() ; }
    string name ;
    vector <string> match ;
    int cut ; // After this, starting at 0
    bool use ;
    string note , str_match ;
    } ;

class TProteaseCut
    {
    public :
    TProtease *protease ;
    int cut ;
    bool left ;
    } ;
    

#endif

