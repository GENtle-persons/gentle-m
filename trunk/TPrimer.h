#ifndef _TPRIMER_H_
#define _TPRIMER_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TPrimerDialog ;
class TPrimerDesign ;

typedef vector <int> tvi ;
typedef vector <char> tvc ;


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
    void getSequenceFromVector ( TVector *v ) ;
    void makeStats () ;
    void evaluate ( float tm_opt = 0 ) ;
    string report () ;
    string get53sequence () ;
    string get35sequence () ;
    int checkFit ( TVector *v , bool justCount = false ) ;

    private :
    friend class TPrimerDesign ;
    void evaluateSelfAnnealing () ;
    float evaluateTm ( double conc_nm = 50 , double Na_mm = 50 ) ;

    void OligoCount () ;
    double NeighbourTM ( bool max , double pconc , double saltconc ) ;
    bool IsBase ( string theBase ) ;
    bool IsIUpacBase ( string theBase ) ;
    double *CalcIUpair ( string base0 , string base , int i , bool max ) ;
    double DeltaG ( bool max ) ;
    double DeltaH ( bool max ) ;
    double DeltaS ( bool max ) ;
    double CountNeighbors ( string s ) ;
    void invertSequence() ;
    string getAnnealingSequence() ;
    
    public :
    // Variables
    int from , to ;
    string sequence ;
    bool upper ;
    int contents[256] ;
    float tm , tm_salt , tm_gc ;
    float pgc ;
    float evaluation ;
    TVector *annealingVector ;
    
    private :
    string ann1 , ann2 , annm ;
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

