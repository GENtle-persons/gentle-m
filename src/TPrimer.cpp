/** \file
    \brief The TPrimer class
*/
#include "TPrimer.h"

//********************************************************************** TPrimer

TPrimer::TPrimer ( const int _from , const int _to , const bool _upper )
    {
    from = _from ;
    to = _to ;
    upper = _upper ;
    annealingVector = NULL ;
    }

float TPrimer::getTm ( const int type ) const
    {
    switch ( type ) {
        case TM_STANDARD : return tm ; break ;
        case TM_SALT : return tm_salt ; break ;
        case TM_GC : return tm_gc ; break ;
        }
    return 0 ;
    }

wxString TPrimer::getName () const { return name ; }
void TPrimer::setName ( const wxString& nn ) { name = nn ; }
float TPrimer::getEvaluation () const { return evaluation ; }
float TPrimer::getGCcontents () const { return pgc ; }

void TPrimer::getSequenceFromVector ( const TVector * const v , const bool from3 )
    {
    int a ;
    sequence = _T("") ;
    bool invert = !upper ;
    if ( from3 ) invert = !invert ;
    for ( a = from ; a <= to ; a++ )
        sequence += v->getNucleotide ( a-1 , invert ) ;
    }

void TPrimer::makeStats () /* not const */
    {
    for ( int a = 0 ; a < 256 ; a++ ) contents[a] = 0 ;
    for ( int a = 0 ; a < sequence.length() ; a++ )
       contents[sequence.GetChar(a)]++ ;
    int gc = contents['C'] + contents['G'] ;
//  int at = contents['A'] + contents['T'] ;
    pgc = (float) gc * 100.0 ;
    pgc /= (float) sequence.length() ;
    tm = evaluateTm () ;
    evaluation = 0 ;
    }

wxString TPrimer::report () const
    {
    wxString r ;
    int l = sequence.length() ;
    TVector v ;

    r += _T("5'-") + get53sequence() + _T("-3'\n") ;

    r += wxString::Format ( _T("deltaH=%2.1fKcal/mol; deltaS=%2.1fcal/(K*mol)\n") , H , S ) ;
    r += wxString::Format ( _T("%d bp; %2.1f %%GC\n") , l , pgc ) ;
    r += wxString::Format ( txt("t_melting_temperatures") , tm , tm_salt , tm_gc ) ;

    // Self annealing
    r += wxString::Format ( _T("Highest self-annealing score : %d\n") , annScore ) ;
    r += ann1 + _T("\n") + annm + _T("\n") + ann2 + _T("\n\n") ;

    return r ;
    }

void TPrimer::evaluate ( const float tm_opt ) /* not const */
    {
    int gc = 1 ;
    wxString s = getAnnealingSequence() ;

    // Start value
    evaluation = 500 ; // not cont

    // Difference from optimum temperature
    evaluation -= ( tm - tm_opt ) * ( tm - tm_opt ) ;

    // End-GC
    if ( upper )
        {
        for ( int a = 0 ; a < s.length() ; a++ )
            {
            if ( s.GetChar(a) == 'G' || s.GetChar(a) == 'C' ) gc *= 2 ;
            else gc = 1 ;
            }
        }
    else
        {
        for ( int a = s.length()-1 ; a >= 0 ; a-- )
            {
            if ( s.GetChar(a) == 'G' || s.GetChar(a) == 'C' ) gc *= 2 ;
            else gc = 1 ;
            }
        }
    if ( gc > 1 ) evaluation += gc ;

    // Self-Annealing
    evaluateSelfAnnealing () ;
    evaluation -= annScore / 10 ;
    }

float TPrimer::evaluateTm ( const double& conc_nm , const double& Na_mm ) /* not const */
    {
/*
    double tx = -21.6 ; // C
    double T0 = -237.15 ; // C
    double R = 1.987 ; // Molar gas constant
    double y = 50.0 / 1000000000.0 ;
*/
    double ret ;

    wxString s = getAnnealingSequence() ;

    // Salt
    tm_salt = tm_gc = 0 ;
    int base[256] ;
    for ( int a = 0 ; a < 256 ; a++ ) base[a] = 0 ;
    for ( int a = 0 ; a < s.size() ; a++ ) base[s.GetChar(a)]++ ;

    double CG = base['C'] + base['G'] ;
    double AT = base['A'] + base['T'] ;
    double ATCG = AT + CG ;

    if ( ATCG > 0 && AT > 0 )
        {
        tm_salt = 100.5 + ( 41.0 * CG / ATCG ) - ( 820.0 / ATCG ) + 16.6 * log ( Na_mm / 1000.0 ) / log ( 10.0 ) ;
        tm_gc = 64 + ( CG + 16.4 ) / ( AT ) ;
        }

    OligoCount () ;

    deltaHValmin = DeltaH(false); // not const
    deltaGValmin = DeltaG(false);
    deltaSValmin = DeltaS(false);
    deltaHValmax = deltaHValmin;
    deltaGValmax = deltaGValmin;
    deltaSValmax = deltaSValmin;

    H = deltaHValmin ;
    S = deltaSValmin ;

    ret = NeighbourTM ( false , 50 , 50 ) ;

    return ret ;
    }

void TPrimer::invertSequence() /* not const */
    {
    wxString t ;
    for ( int a = 0 ; a < sequence.length() ; a++ ) t = sequence.GetChar(a) + t ;
    sequence = t ;
    }

wxString TPrimer::get53sequence () const
    {
    if ( upper ) return sequence ;
    wxString t ;
    for ( int a = 0 ; a < sequence.length() ; a++ ) t = sequence.GetChar(a) + t ;
    return t ;
    }

wxString TPrimer::get35sequence () const
    {
    wxString s , t = get53sequence () ;
    for ( int a = 0 ; a < t.length() ; a++ ) s = t.GetChar(a) + s ;
    return s ;
    }

double TPrimer::NeighbourTM ( const bool max , const double& pconc , const double& saltconc ) const
    {
    double theReturn = 0 ;
    double RlogK = 0 ;
    wxString s = getAnnealingSequence() ;
    if ( s.length() > 7) {
        //
        double K = 1/(pconc/1000000000.0);  // Convert from nanomoles to moles
        double R = 1.987;  //cal/(mole*K);
        double RlnK = R*log(K);
        RlogK =RlnK ;
        // Helix initiation Free Energy of 3.4 kcal (Sugimoto et al, 1996)
        // symmetry function: if symmetrical, subtract another 0.4
        if (!max) {
            theReturn = 1000*((deltaHValmin-3.4)/(deltaSValmin+RlnK));
            theReturn += -272.9;
            theReturn += 7.21*log (saltconc/1000.0);
//          theReturn = Math.round(theReturn);
        } else {
            theReturn = 1000*((deltaHValmax-3.4)/(deltaSValmax+RlnK));
            theReturn += -272.9;
            theReturn += 7.21*log (saltconc/1000.0);
//          theReturn = Math.round(theReturn);
        }
    } else {
//        RlogK ="";
    }
    return theReturn;
    }

double TPrimer::CountNeighbors ( const wxString& s ) const
    {
    double ret = 0 ;
    const wxString seq = getAnnealingSequence() ;
    for ( int a = 0 ; a+s.length() < seq.length() ; a++ )
        {
        if ( seq.substr ( a , s.length() ) == s ) ret += 1 ;
        }
    return ret ;
    }

void TPrimer::OligoCount () /* not const */
    {
    const wxString seq = getAnnealingSequence() ;

    // count Nearest Neighbors
    aaCount = CountNeighbors(_T("AA"))+CountNeighbors(_T("TT")); /* not const */
    atCount = CountNeighbors(_T("AT"));
    taCount = CountNeighbors(_T("TA"));
    caCount = CountNeighbors(_T("CA"))+CountNeighbors(_T("TG"));
    gtCount = CountNeighbors(_T("GT"))+CountNeighbors(_T("AC"));
    ctCount = CountNeighbors(_T("CT"))+CountNeighbors(_T("AG"));
    gaCount = CountNeighbors(_T("GA"))+CountNeighbors(_T("TC"));
    cgCount = CountNeighbors(_T("CG"));
    gcCount = CountNeighbors(_T("GC"));
    ggCount = CountNeighbors(_T("GG"))+CountNeighbors(_T("CC"));

    for ( int j = 0 ; j < 3 ; j++ )
        {
        IUpairVals_min[j] = IUpairVals_max[j] = 0 ;
        }

    for ( int i = 1 ; i < seq.length() ; i++ ) //first base can not be IUpacbase
        {
        wxString base0 = seq.GetChar(i-1) ;
        wxString base = seq.GetChar(i) ;
        double *temp ;

        temp=CalcIUpair(base0, base, i, false );
        for( int j=0; j<3; j++) IUpairVals_min[j]+=temp[j];
        delete temp ;

        temp=CalcIUpair(base0, base, i, true);
        for( int j=0; j<3; j++) IUpairVals_max[j]+=temp[j];
        delete temp ;
        }
    }

bool TPrimer::IsBase ( const wxString& theBase ) const
    {
    if ((theBase == _T("A")) || (theBase == _T("G")) || (theBase == _T("C")) || (theBase == _T("T")))
            {
            return true;
            }
    return false;
    }

bool TPrimer::IsIUpacBase ( const wxString& theBase ) const
    {
    if ((theBase == _T("M")) ||
        (theBase == _T("R")) ||
        (theBase == _T("W")) ||
        (theBase == _T("S")) ||
        (theBase == _T("Y")) ||
        (theBase == _T("K")) ||
        (theBase == _T("V")) ||
        (theBase == _T("H")) ||
        (theBase == _T("D")) ||
        (theBase == _T("B")) ||
        (theBase == _T("N")))
        {
        return true;
        }
    return false;
    }

double *TPrimer::CalcIUpair ( wxString base0 , wxString base , int i , const bool max ) /* not const */
    {
    double *reValue = new double[3] ;
    double temp1[3] , temp2[3] ;
    wxString IUpacBase, pair1 , pair2 , base2 ;
    wxString seq = getAnnealingSequence() ;

    // Init
    for ( int a = 0 ; a < 3 ; a++ )
       reValue[a] = temp1[a] = temp2[a] = 0 ;
    if ( i+1 < seq.length() ) base2 += seq.GetChar(i+1) ;


    // JavaScript dump
    if(IsIUpacBase(base0))    //if previous base is IUpacBase, do nothing
    {    return reValue;    }

    if(IsIUpacBase(base) )
    {
        if(base==_T("M")){IUpacBase=_T("AC");}
        else if(base==_T("R")){IUpacBase=_T("AG");}
        else if(base==_T("W")){IUpacBase=_T("AT");}
        else if(base==_T("S")){IUpacBase=_T("CG");}
        else if(base==_T("Y")){IUpacBase=_T("CT");}
        else if(base==_T("K")){IUpacBase=_T("GT");}
        else if(base==_T("V")){IUpacBase=_T("ACG");}
        else if(base==_T("H")){IUpacBase=_T("ACT");}
        else if(base==_T("D")){IUpacBase=_T("AGT");}
        else if(base==_T("B")){IUpacBase=_T("CGT");}
        else if(base==_T("N")){IUpacBase=_T("ACGT");}

        int j=0;
//        while(IUpacBase.charAt(j)!="")
        while ( j < IUpacBase.length() )
        {
//          base=IUpacBase.charAt(j);
            base=IUpacBase.GetChar(j);

            pair1=base0+base;

            if(pair1==_T("AA")){temp1[0]= 1.2 ;temp1[1]=8.0; temp1[2]=21.9 ;}
            else if(pair1==_T("AT")){temp1[0]= 0.9 ;temp1[1]=5.6; temp1[2]=15.2  ;}
            else if(pair1==_T("TA")){temp1[0]=0.9  ;temp1[1]=6.6; temp1[2]= 18.4 ;}
            else if(pair1==_T("CA")){temp1[0]=1.7  ;temp1[1]=8.2; temp1[2]=21.0  ;}
            else if(pair1==_T("GT")){temp1[0]= 1.5 ;temp1[1]=9.4; temp1[2]=25.5  ;}
            else if(pair1==_T("CT")){temp1[0]= 1.5 ;temp1[1]=6.6; temp1[2]=16.4  ;}
            else if(pair1==_T("GA")){temp1[0]=1.5  ;temp1[1]=8.8; temp1[2]=23.5  ;}
            else if(pair1==_T("CG")){temp1[0]= 2.8 ;temp1[1]=11.8; temp1[2]=29.0  ;}
            else if(pair1==_T("GC")){temp1[0]=2.3  ;temp1[1]=10.5; temp1[2]=26.4  ;}
            else if(pair1==_T("GG")){temp1[0]=2.1  ;temp1[1]=10.9; temp1[2]=28.4  ;}

            if(base2.IsEmpty())
                {
                for(int k=0; k<2; k++)
                    {
                    temp2[k]=0.0;
                    }

                }
            else if(!IsIUpacBase(base2))
                {
                pair2=base+base2;
                if(pair2==_T("AA")){temp2[0]= 1.2 ;temp2[1]=8.0; temp2[2]=21.9 ;}
                else if(pair2==_T("AT")){temp2[0]= 0.9 ;temp2[1]=5.6; temp2[2]=15.2  ;}
                else if(pair2==_T("TA")){temp2[0]=0.9  ;temp2[1]=6.6; temp2[2]= 18.4 ;}
                else if(pair2==_T("CA")){temp2[0]=1.7  ;temp2[1]=8.2; temp2[2]=21.0  ;}
                else if(pair2==_T("GT")){temp2[0]= 1.5 ;temp2[1]=9.4; temp2[2]=25.5  ;}
                else if(pair2==_T("CT")){temp2[0]= 1.5 ;temp2[1]=6.6; temp2[2]=16.4  ;}
                else if(pair2==_T("GA")){temp2[0]=1.5  ;temp2[1]=8.8; temp2[2]=23.5  ;}
                else if(pair2==_T("CG")){temp2[0]= 2.8 ;temp2[1]=11.8; temp2[2]=29.0  ;}
                else if(pair2==_T("GC")){temp2[0]=2.3  ;temp2[1]=10.5; temp2[2]=26.4  ;}
                else if(pair2==_T("GG")){temp2[0]=2.1  ;temp2[1]=10.9; temp2[2]=28.4  ;}
                }
            else if(IsIUpacBase(base2))
                {
                base0=base;
                base=base2;
                i++;
                double *tempp = CalcIUpair(base0,base,i,max);
                for ( int a = 0 ; a < 3 ; a++ ) temp2[a] = tempp[a] ;
                delete tempp ;
                i--;
                }

            for(int k=0;k<3;k++)
                {
                if(j==0)
                    {
                    reValue[k]=temp1[k]+temp2[k];
                    }
                    else
                        {
                        if ((max)&&(reValue[k]<temp1[k]+temp2[k]))
                            {
                            reValue[k]=temp1[k]+temp2[k];
                            }
                        else if((!max)&&(reValue[k]>temp1[k]+temp2[k]))
                            {
                            reValue[k]=temp1[k]+temp2[k];
                            }
                        }
                    }
                j++;
            }
        }
    return reValue;
    }

double TPrimer::DeltaG ( const bool max ) const
{
    wxString seq = getAnnealingSequence() ;
    if (seq.length() > 7) {
        double val= -5.0;
        // Helix initiation Free Energy of 5 kcal.
        // symmetry function: if symmetrical, subtract another 0.4
        val+=1.2*aaCount;
        val+=0.9*atCount;
        val+=0.9*taCount;
        val+=1.7*caCount;
        val+=1.5*gtCount;
        val+=1.5*ctCount;
        val+=1.5*gaCount;
        val+=2.8*cgCount;
        val+=2.3*gcCount;
        val+=2.1*ggCount;
        if(!max){
            val+=IUpairVals_min[0];
        }else{
            val+=IUpairVals_max[0];
        }
        return val ;
    }
    return 0;
}

double TPrimer::DeltaH ( const bool max ) const
{
    wxString seq = getAnnealingSequence() ;
    if (seq.length() > 7) {
        double val= 0.0;
        val+=8.0*aaCount;
        val+=5.6*atCount;
        val+=6.6*taCount;
        val+=8.2*caCount;
        val+=9.4*gtCount;
        val+=6.6*ctCount;
        val+=8.8*gaCount;
        val+=11.8*cgCount;
        val+=10.5*gcCount;
        val+=10.9*ggCount;
        if(!max){
            val+=IUpairVals_min[1];
        }else{
            val+=IUpairVals_max[1];
        }
        return val ;
    }
    return 0;
}

double TPrimer::DeltaS ( const bool max ) const
{
    wxString seq = getAnnealingSequence() ;
    if (seq.length() > 7) {
        double val=0;

        val+=21.9*aaCount;
        val+=15.2*atCount;
        val+=18.4*taCount;
        val+=21.0*caCount;
        val+=25.5*gtCount;
        val+=16.4*ctCount;
        val+=23.5*gaCount;
        val+=29.0*cgCount;
        val+=26.4*gcCount;
        val+=28.4*ggCount;
        if(!max){
            val+=IUpairVals_min[2];
        }else{
            val+=IUpairVals_max[2];
        }
        return val;
    }
    return 0;
}


// ***

void TPrimer::evaluateSelfAnnealing () /* not const */
    {
    wxString s = sequence ;
    wxString t ;
    int l = s.length() ;
    annScore = 0 ; // not const
    ann1 = _T("") ;
    ann2 = _T("") ;
    annm = _T("") ;
    for ( int a = 0 ; a < l ; a++ ) t = s.GetChar(a) + t ;
    for ( int a = -(l-1) ; a < l ; a++ )
        {
        int lm = 0 ;
        wxString ann_tmp ;
        for ( int b = 0 ; b < l ; b++ )
            {
            if ( a+b >= 0 && a+b < l )
                {
                char c = '|' ;
                if ( s.GetChar(a+b) == 'A' && t.GetChar(b) == 'T' ) lm += 2 ;
                else if ( s.GetChar(a+b) == 'T' && t.GetChar(b) == 'A' ) lm += 2 ;
                else if ( s.GetChar(a+b) == 'G' && t.GetChar(b) == 'C' ) lm += 4 ;
                else if ( s.GetChar(a+b) == 'C' && t.GetChar(b) == 'G' ) lm += 4 ;
                else c = ' ' ;
                ann_tmp += c ;
                }
            }
        if ( lm > annScore )
            {
            annScore = lm ;
            ann1 = s ;
            ann2 = t ;
            annm = ann_tmp ;
            if ( a < 0 )
                {
                for ( int b = a ; b < 0 ; b++ ) ann1 = _T(" ") + ann1 ;
                }
            if ( a > 0 )
                {
                for ( int b = a ; b < 0 ; b++ ) ann2 = _T(" ") + ann2 ;
                }
            for ( int b = 0 ; b < abs(a) ; b++ ) annm = _T(" ") + annm ;
            }
        }
    }

int TPrimer::checkFit ( const TVector * const v , const bool justCount ) const
    {
    int count = 0 ;
    int match = 0 ;
    int match3 = 0 ;
    wxString s = get53sequence() ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        char c ;
        if ( upper ) c = v->getNucleotide ( a + from - 1 ) ;
        else c = v->getNucleotide ( to - a - 1 , true ) ;
        if ( c == s.GetChar(a) )
            {
            match++ ;
            match3 += 2 ;
            count++ ;
            }
        else match3 = count = 0 ;
        }
    if ( justCount ) return count ;
    return match + match3 ;
    }

wxString TPrimer::getAnnealingSequence() const
    {
    if ( !annealingVector ) return get53sequence () ;
    const wxString s = get53sequence () ;
    return s.substr ( s.length() - checkFit ( annealingVector , true ) ) ;
    }

bool TPrimer::overlap ( const TPrimer &op ) const
    {
    if ( op.from <= to && op.to >= from ) return true ;
    return false ;
    }

