/** \file
    \brief The TPrimer class
*/
#ifndef _TPRIMER_H_
#define _TPRIMER_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TPrimerDialog ;
class TPrimerDesign ;

/** \def TM_STANDARD
    \brief Standard (nearest neighbour) method */
#define TM_STANDARD 0

/** \def TM_SALT
    \brief Salt-adjusted method */
#define TM_SALT 1

/** \def TM_GC
    \brief GC method */
#define TM_GC 2

/// This class holds and generates information about a primer
class TPrimer
    {
    public :
    TPrimer ( const int _from = 0 , const int _to = 0 , const bool _upper = true ) ; ///< Constructor
    ~TPrimer () {} ; ///< Dummy Destructor
    void getSequenceFromVector ( const TVector * const v , const bool from3 = false ) ; ///< Reads primer sequence from vectors
    void makeStats () ; ///< Generates key values about the primer
    void evaluate ( const float tm_opt = 0 ) ; ///< Evaluates primer
    wxString report () const ; ///< Generates a human-readable report
    wxString get53sequence () const ; ///< Returns the 5'->3' primer sequence
    wxString get35sequence () const ; ///< Returns the 3'->5' primer sequence
    int checkFit ( const TVector * const v , const bool justCount = false ) const ; ///< Tries to fit the primer to a sequence
    bool overlap ( const TPrimer &op ) const ; ///< Does this primer overlap with another?

    float getTm ( const int type = TM_STANDARD ) const ; ///< Get melting temperature
    float getEvaluation () const ; ///< Get quality evaluation (for annealing)
    float getGCcontents () const ; ///< Get GC contents

    wxString getName () const ; ///< Returns the name of the primer, if any was given
    void setName ( const wxString& nn ) ; ///< Sets the name of the primer

    // Variables
    /// The beginning of the primer in a sequence
    int from , to ; ///< The end of a primer in the sequence
    wxString sequence ; ///< The primer sequence
    bool upper ; ///< Upper (5'->3') or lower (3'->5') primer?
    TVector *annealingVector ; ///< The vector to anneal to

    private :
    void evaluateSelfAnnealing () ; ///< Check for self-annealing
    float evaluateTm ( const double& conc_nm = 50 , const double& Na_mm = 50 ) ; ///< Calculate melting temperature, salt-adjusted

    void OligoCount () ; ///< Nearest neighbour helper method
    double NeighbourTM ( const bool max , const double& pconc , const double& saltconc ) const ; ///< Calculate melting temperature, nearest neighbour
    bool IsBase ( const wxString& theBase ) const ; ///< Nearest neighbour helper method
    bool IsIUpacBase ( const wxString& theBase ) const ; ///< Nearest neighbour helper method
    double *CalcIUpair ( wxString base0 , wxString base , int i , const bool max ) ; ///< Nearest neighbour helper method
    double DeltaG ( const bool max ) const ; ///< Nearest neighbour helper method
    double DeltaH ( const bool max ) const ; ///< Nearest neighbour helper method
    double DeltaS ( const bool max ) const ; ///< Nearest neighbour helper method
    double CountNeighbors ( const wxString& s ) const ; ///< Nearest neighbour helper method

    void invertSequence() ; ///< Inverts the sequence
    wxString getAnnealingSequence() const ; ///< Returns annealing sequence

    // Variables
    wxString name ;
    int contents[256] ;
    float pgc ; ///< GC%
    float evaluation ; ///< The last quality evaluation
    float tm ; ///< Melting temperature, nearest neighbour method
    float tm_salt ; ///< Melting temperature, salt-adjusted
    float tm_gc ; ///< Melting temperature, GC method
    wxString ann1 , ann2 , annm ;
    int annScore ; ///< Annealing score
    double S , H ;

    double IUpairVals_min[3] , IUpairVals_max[3] ;
    double deltaHValmin , deltaHValmax ;
    double deltaSValmin , deltaSValmax ;
    double deltaGValmin , deltaGValmax ;

    double aaCount, atCount , taCount , caCount , gtCount ;
    double ctCount , gaCount , cgCount , gcCount , ggCount ;
    } ;

#endif

