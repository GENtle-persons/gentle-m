#ifndef _TPRIMER_H_
#define _TPRIMER_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TPrimerDialog ;
class TPrimerDesign ;

#define TM_STANDARD 0
#define TM_SALT 1
#define TM_GC 2

class TPrimer
    {
    public :
    TPrimer ( int _from = 0 , int _to = 0 , bool _upper = true )
        {
        from = _from ;
        to = _to ;
        upper = _upper ;
        annealingVector = NULL ;
        }
    void getSequenceFromVector ( TVector *v , bool from3 = false ) ;
    void makeStats () ;
    void evaluate ( float tm_opt = 0 ) ;
    wxString report () ;
    wxString get53sequence () ;
    wxString get35sequence () ;
    int checkFit ( TVector *v , bool justCount = false ) ;
    bool overlap ( TPrimer &op ) ;

    float getTm ( int type = TM_STANDARD ) ;
    float getEvaluation () ;
    float getGCcontents () ;
    
    // Variables
    int from , to ;
    wxString sequence ;
    bool upper ;
    TVector *annealingVector ;
    
    private :
    void evaluateSelfAnnealing () ;
    float evaluateTm ( double conc_nm = 50 , double Na_mm = 50 ) ;

    void OligoCount () ;
    double NeighbourTM ( bool max , double pconc , double saltconc ) ;
    bool IsBase ( wxString theBase ) ;
    bool IsIUpacBase ( wxString theBase ) ;
    double *CalcIUpair ( wxString base0 , wxString base , int i , bool max ) ;
    double DeltaG ( bool max ) ;
    double DeltaH ( bool max ) ;
    double DeltaS ( bool max ) ;
    double CountNeighbors ( wxString s ) ;
    void invertSequence() ;
    wxString getAnnealingSequence() ;
    
    // Variables
    int contents[256] ;
    float pgc ;
    float evaluation ;
    float tm , tm_salt , tm_gc ;
    wxString ann1 , ann2 , annm ;
    int annScore ;
    double S , H ;

    double IUpairVals_min[3] , IUpairVals_max[3] ;
    double deltaHValmin , deltaHValmax ;
    double deltaSValmin , deltaSValmax ;
    double deltaGValmin , deltaGValmax ;

    double aaCount, atCount , taCount , caCount , gtCount ;
    double ctCount , gaCount , cgCount , gcCount , ggCount ;
    } ;

#endif

