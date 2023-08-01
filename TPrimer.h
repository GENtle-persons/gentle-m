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
    TPrimer ( int _from = 0 , int _to = 0 , bool _upper = true ) ; ///< Constructor
    ~TPrimer () {} ; ///< Dummy Destructor
    void getSequenceFromVector ( TVector *v , bool from3 = false ) ; ///< Reads primer sequence from vectors
    void makeStats () ; ///< Generates key values about the primer
    void evaluate ( float tm_opt = 0 ) ; ///< Evaluates primer
    wxString report () ; ///< Generates a human-readable report
    wxString get53sequence () ; ///< Returns the 5'->3' primer sequence
    wxString get35sequence () ; ///< Returns the 3'->5' primer sequence
    int checkFit ( TVector *v , bool justCount = false ) ; ///< Tries to fit the primer to a sequence
    bool overlap ( TPrimer &op ) ; ///< Does this primer overlap with another?

    float getTm ( int type = TM_STANDARD ) ; ///< Get melting temperature
    float getEvaluation () ; ///< Get quality evaluation (for annealing)
    float getGCcontents () ; ///< Get GC contents
    
    wxString getName () ; ///< Returns the name of the primer, if any was given
    void setName ( wxString nn ) ; ///< Sets the name of the primer
    
    
    // Variables
    /// The beginning of the primer in a sequence
    int from , to ; ///< The end of a primer in the sequence
    wxString sequence ; ///< The primer sequence
    bool upper ; ///< Upper (5'->3') or lower (3'->5') primer?
    TVector *annealingVector ; ///< The vector to anneal to
    
    private :
    void evaluateSelfAnnealing () ; ///< Check for self-annealing
    float evaluateTm ( double conc_nm = 50 , double Na_mm = 50 ) ; ///< Calculate melting temperature, salt-adjusted

    void OligoCount () ; ///< Nearest neighbour helper method
    double NeighbourTM ( bool max , double pconc , double saltconc ) ; ///< Calculate melting temperature, nearest neighbour
    bool IsBase ( wxString theBase ) ; ///< Nearest neighbour helper method
    bool IsIUpacBase ( wxString theBase ) ; ///< Nearest neighbour helper method
    double *CalcIUpair ( wxString base0 , wxString base , int i , bool max ) ; ///< Nearest neighbour helper method
    double DeltaG ( bool max ) ; ///< Nearest neighbour helper method
    double DeltaH ( bool max ) ; ///< Nearest neighbour helper method
    double DeltaS ( bool max ) ; ///< Nearest neighbour helper method
    double CountNeighbors ( wxString s ) ; ///< Nearest neighbour helper method
    
    void invertSequence() ; ///< Inverts the sequence
    wxString getAnnealingSequence() ; ///< Returns annealing sequence
    
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

