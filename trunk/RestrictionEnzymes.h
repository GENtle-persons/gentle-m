/** \file
	\brief Contains the TRestrictionEnzyme, TRestrictionCut, TProtease, and TProteaseCut classes
*/
#ifndef _RESTRICTIONENZYMES_H_
#define _RESTRICTIONENZYMES_H_

#include "main.h"

/// \brief Stores a restriction enzyme
class TRestrictionEnzyme
    {
    public :
	TRestrictionEnzyme () {} ; ///< Empty constructor
	~TRestrictionEnzyme () {} ; ///< Dummy destructor
	
    bool differ ( TRestrictionEnzyme &e ) ;
    
    wxString getEndUpperLeft () ; ///< The "upper left" sequence after the cut
    wxString getEndLowerLeft () ; ///< The "lower left" sequence after the cut
    wxString getEndUpperRight () ; ///< The "upper right" sequence after the cut
    wxString getEndLowerRight () ; ///< The "lower right" sequence after the cut
    wxString invertSequence () ; ///< The recognition sequence, inverted
    
    /// The enzyme name
    wxString name , sequence ; ///< The recognition sequence
    
    /// The physical location of the vial containing the enzyme, if entered
    wxString location , note ; ///< A note about the enzyme, if entered
    unsigned long dbid ; ///< Database ID of the enzyme
    
    /// Where the cut occurs
    int cut , overlap ; ///< Length of the sticky ends (negative=<--)
    } ;

/// \brief Stores a specific cut of a restriction enzyme
class TRestrictionCut
    {
    public :
    TRestrictionCut ( int _pos , TRestrictionEnzyme *_e ) ///< Constructor
        { pos = _pos ; e = _e ; }
	~TRestrictionCut () {} ; ///< Dummy destructor
        
    void linearUpdate ( int w , int h ) ;
    wxString getNameAndPosition () ; ///< Returns the enzyme name and cut pusition
    bool isHidden ( TVector *v ) ; ///< Is this enzyme in this TVector hidden?
    wxString getDisplayName () ; ///< Returns the enzyme name (or the joined names)
    bool join ( TRestrictionCut *c ) ;
        
    int pos ; ///< Position of the cut
    TRestrictionEnzyme *e ; ///< Cutting enzyme
    wxString display_name ; ///< The name to display
    
    // Visual information
    float angle , angle3 , r1 , r2 , r3 ; 
    wxRect lastrect ;
    wxPoint p , lp ;
    wxTreeItemId treeid ; ///< ID of the cut in the TVectorTree
    } ;

bool operator < ( const TRestrictionCut &c1 , const TRestrictionCut &c2 ) ;
bool operator == ( const TRestrictionCut &c1 , const TRestrictionCut &c2 ) ;

/// \brief Class to store a protease
class TProtease
    {
    public :
    TProtease ( wxString _name = _T("") , wxString m = _T("") , wxString _note = _T("") ) ; ///< Constructor
    bool does_match ( wxString s ) ; ///< Does this protease recognize this sequence?
    inline int len() { return match.GetCount() ; } ///< Returns the number of different recognition sequences
    wxString name ; ///< The name of the protease
    wxArrayString match ; ///< The different recognition sequences
    int cut ; ///< Where the cut occurs, starting at 0
    bool use ; ///< Use this protease?
    
    /// A note about the protease
    wxString note , str_match ;
    } ;

/// \brief Class to store a protease cut
class TProteaseCut
    {
    public :
    TProtease *protease ; ///< Pointer to the protease
    int cut ; ///< Position of the cut
    bool left ; ///< Left or right from the given position?
    } ;
    

#endif

