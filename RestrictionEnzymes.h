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
    TRestrictionEnzyme () { palindromic = false ; } ; ///< Empty constructor
    ~TRestrictionEnzyme () {} ; ///< Dummy destructor

    bool differ ( const TRestrictionEnzyme &e ) const ;
    
    wxString getEndUpperLeft ( const bool first_strand = true ) const ; ///< The "upper left" sequence after the cut
    wxString getEndLowerLeft ( const bool first_strand = true ) const ; ///< The "lower left" sequence after the cut
    wxString getEndUpperRight ( const bool first_strand = true ) const ; ///< The "upper right" sequence after the cut
    wxString getEndLowerRight ( const bool first_strand = true ) const ; ///< The "lower right" sequence after the cut
    
    wxString invertSequence () const ; ///< The recognition sequence, inverted
    
    int getCut ( const bool first_strand = true ) const ;
    void setCut ( const int c ) ;
    int getOverlap ( const bool first_strand = true ) const ;
    void setOverlap ( int o ) ;
    wxString getName () const ;
    void setName ( const wxString& _name ) ;
    wxString getSequence () const ;
    void setSequence ( const wxString& _seq ) ;
    bool isPalindromic () const ;

    /// The physical location of the vial containing the enzyme, if entered
    wxString location , note ; ///< A note about the enzyme, if entered
    unsigned long dbid ; ///< Database ID of the enzyme

  private :
    /// The enzyme name
    wxString name , sequence ; ///< The recognition sequence
    bool palindromic ;
    /// Where the cut occurs
    int cut , overlap ; ///< Length of the sticky ends (negative=<--)
} ;

/// \brief Stores a specific cut of a restriction enzyme
class TRestrictionCut
{
  public :
    TRestrictionCut ( int _pos , TRestrictionEnzyme *_e , bool _first_strand = true ) ///< Constructor
        { pos = _pos ; e = _e ; first_strand = _first_strand ; }
    ~TRestrictionCut () {} ; ///< Dummy destructor

    void linearUpdate ( int w , int h ) ;
    wxString getNameAndPosition () const ; ///< Returns the enzyme name and cut pusition
    bool isHidden ( const TVector * const v ) const ; ///< Is this enzyme in this TVector hidden?
    wxString getDisplayName () const ; ///< Returns the enzyme name (or the joined names)
    bool join ( TRestrictionCut *c ) ;

    wxString getEndUpperLeft () const ; ///< The "upper left" sequence after the cut
    wxString getEndLowerLeft () const ; ///< The "lower left" sequence after the cut
    wxString getEndUpperRight () const ; ///< The "upper right" sequence after the cut
    wxString getEndLowerRight () const ; ///< The "lower right" sequence after the cut
    int getCut () const ;
    int getOverlap () const ;
    int getPos () const ;
    void setPos ( int p ) ;
    int getFrom () const ;
    int getTo () const ;
    wxString getSequence () const ;

    TRestrictionEnzyme *e ; ///< Cutting enzyme
    wxString display_name ; ///< The name to display

    // Visual information
    float angle , angle3 , r1 , r2 , r3 ;
    wxRect lastrect ;
    wxPoint p , lp ;
    wxTreeItemId treeid ; ///< ID of the cut in the TVectorTree

    private:
    int pos ; ///< Position of the cut
    bool first_strand ;
} ;

bool operator < ( const TRestrictionCut &c1 , const TRestrictionCut &c2 ) ;
bool operator == ( const TRestrictionCut &c1 , const TRestrictionCut &c2 ) ;

/// \brief Class to store a protease
class TProtease
    {
    public :
    TProtease ( const wxString& _name = _T("") , const wxString& m = _T("") , const wxString& _note = _T("") ) ; ///< Constructor
    bool does_match ( const wxString& s ) const ; ///< Does this protease recognize this sequence?
    inline int len() const { return match.GetCount() ; } ///< Returns the number of different recognition sequences
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

