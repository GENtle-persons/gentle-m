#include "TPrimer.h"

//********************************************************************** TPrimer

float TPrimer::getTm ( int type )
    {
    switch ( type ) {
        case TM_STANDARD : return tm ; break ;
        case TM_SALT : return tm_salt ; break ;
        case TM_GC : return tm_gc ; break ;
        }
    return 0 ;
    }
    
float TPrimer::getEvaluation () { return evaluation ; }
float TPrimer::getGCcontents () { return pgc ; }

void TPrimer::getSequenceFromVector ( TVector *v , bool from3 )
    {
    int a ;
    sequence = "" ;
    bool invert = !upper ;
    if ( from3 ) invert = !invert ;
    for ( a = from ; a <= to ; a++ )
        sequence += v->getNucleotide ( a-1 , invert ) ;
    }
    
void TPrimer::makeStats ()
    {
    int a ;
    for ( a = 0 ; a < 256 ; a++ ) contents[a] = 0 ;
    for ( a = 0 ; a < sequence.length() ; a++ )
       contents[sequence[a]]++ ;
    int gc = contents['C'] + contents['G'] ;
    int at = sequence.length() - gc ;
    pgc = gc * 100 / sequence.length() ;
    tm = evaluateTm () ;
    evaluation = 0 ;
    }
    
string TPrimer::report ()
    {
    string r ;
    int a , l = sequence.length() ;
    TVector v ;
    char u[100] ;

    r += "5'-" + get53sequence() + "-3'\n" ;
//    for ( a = 0 ; a < r.length() ; a++ ) if ( r[a] < 'A' || r[a] > 'Z' ) r[a] = '!' ;
    sprintf ( u , "deltaH=%2.1fKcal/mol; deltaS=%2.1fcal/(K*mol)\n" , H , S ) ; r += u ;
    sprintf ( u , "%d bp; %2.1f %%GC\n" , l , pgc ) ;         r += u ;
    sprintf ( u , txt("t_melting_temperatures") , tm , tm_salt , tm_gc ) ;     r += u ;
   
    // Self annealing
    sprintf ( u , "Highest self-annealing score : %d\n" , annScore ) ; r += u ;
    r += ann1 + "\n" + annm + "\n" + ann2 + "\n\n" ;

    return r ;
    }
    
void TPrimer::evaluate ( float tm_opt )
    {
    int a ;
    int gc = 1 ;
    string s = getAnnealingSequence() ;
    
    // Start value
    evaluation = 500 ;
    
    // Difference from optimum temperature
    evaluation -= ( tm - tm_opt ) * ( tm - tm_opt ) ;
    
    // End-GC
    if ( upper )
        {
        for ( a = 0 ; a < s.length() ; a++ )
           {
           if ( s[a] == 'G' || s[a] == 'C' ) gc *= 2 ;
           else gc = 1 ;
           }
        }
    else
        {
        for ( a = s.length()-1 ; a >= 0 ; a-- )
           {
           if ( s[a] == 'G' || s[a] == 'C' ) gc *= 2 ;
           else gc = 1 ;
           }
        }
    if ( gc > 1 ) evaluation += gc ;
    
    // Self-Annealing
    evaluateSelfAnnealing () ;
    evaluation -= annScore / 10 ;
    }
    
float TPrimer::evaluateTm ( double conc_nm , double Na_mm )
    {
    double tx = -21.6 ; // °C
    double T0 = -237.15 ; // °C
    double R = 1.987 ; // Molar gas constant
    double y = 50.0 / 1000000000.0 ;
    double ret ;

    string s = getAnnealingSequence() ;

    // Salt
    tm_salt = tm_gc = 0 ;
    int a , base[256] ;
    for ( a = 0 ; a < 256 ; a++ ) base[a] = 0 ;
    for ( a = 0 ; a < s.size() ; a++ ) base[s[a]]++ ;
    
    double CG = base['C'] + base['G'] ;
    double AT = base['A'] + base['T'] ;
    double ATCG = AT + CG ;
    
    if ( ATCG > 0 && AT > 0 )
       {
       tm_salt = 100.5 + ( 41.0 * CG / ATCG ) - ( 820.0 / ATCG ) + 16.6 * log ( Na_mm / 1000.0 ) / log ( 10.0 ) ;
       tm_gc = 64 + ( CG + 16.4 ) / ( AT ) ;
       }
    
    OligoCount () ;
    
	deltaHValmin = DeltaH(false);
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
    
void TPrimer::invertSequence()
    {
    string t ;
    for ( int a = 0 ; a < sequence.length() ; a++ ) t = sequence[a] + t ;
    sequence = t ;
    }
    
string TPrimer::get53sequence ()
    {
    if ( upper ) return sequence ;
    string t ;
    for ( int a = 0 ; a < sequence.length() ; a++ ) t = sequence[a] + t ;
    return t ;
    }
    
string TPrimer::get35sequence ()
    {
    string s , t = get53sequence () ;
    for ( int a = 0 ; a < t.length() ; a++ ) s = t[a] + s ;
    return s ;
    }
    
double TPrimer::NeighbourTM ( bool max , double pconc , double saltconc )
    {
	double theReturn = 0 ;
	double RlogK = 0 ;
	string s = getAnnealingSequence() ;
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
//			theReturn = Math.round(theReturn);
		} else {
			theReturn = 1000*((deltaHValmax-3.4)/(deltaSValmax+RlnK));
			theReturn += -272.9;
			theReturn += 7.21*log (saltconc/1000.0);
//			theReturn = Math.round(theReturn);
		}
	} else {
//		RlogK ="";
	}
	return theReturn;    
    }
    
double TPrimer::CountNeighbors ( string s )
    {
    int a ;
    double ret = 0 ;
    string seq = getAnnealingSequence() ;
    for ( a = 0 ; a+s.length() < seq.length() ; a++ )
        {
        if ( seq.substr ( a , s.length() ) == s ) ret += 1 ;
        }
    return ret ;
    }
    
void TPrimer::OligoCount ()
    {
    int i , j ;
    string seq = getAnnealingSequence() ;

	// count Nearest Neighbors
	aaCount = CountNeighbors("AA")+CountNeighbors("TT");
	atCount = CountNeighbors("AT");
	taCount = CountNeighbors("TA");
	caCount = CountNeighbors("CA")+CountNeighbors("TG");
	gtCount = CountNeighbors("GT")+CountNeighbors("AC");
	ctCount = CountNeighbors("CT")+CountNeighbors("AG");
	gaCount = CountNeighbors("GA")+CountNeighbors("TC");
	cgCount = CountNeighbors("CG");
	gcCount = CountNeighbors("GC");
	ggCount = CountNeighbors("GG")+CountNeighbors("CC");

    for ( j = 0 ; j < 3 ; j++ ) IUpairVals_min[j] = IUpairVals_max[j] = 0 ;
    for ( i = 1 ; i < seq.length() ; i++ ) //first base can not be IUpacbase
        {
        string base , base0 ;
        base0 += seq[i-1] ;
        base += seq[i] ;
        double *temp ;
	
		temp=CalcIUpair(base0, base, i, false );
		for( j=0; j<3; j++) IUpairVals_min[j]+=temp[j];
		delete temp ;

		temp=CalcIUpair(base0, base, i, true);
		for( j=0; j<3; j++) IUpairVals_max[j]+=temp[j];
		delete temp ;
		}
    }
    
bool TPrimer::IsBase ( string theBase )
    {
	if ((theBase == "A") ||
		(theBase == "G") ||
		(theBase == "C") ||
		(theBase == "T")) {
			return true;
	}
	return false;
    }
    
bool TPrimer::IsIUpacBase ( string theBase )
    {
	if ((theBase == "M") ||
		(theBase == "R") ||
		(theBase == "W") ||
		(theBase == "S") ||
		(theBase == "Y") ||
		(theBase == "K") ||
		(theBase == "V") ||
		(theBase == "H") ||
		(theBase == "D") ||
		(theBase == "B") ||
		(theBase == "N")) 
	{
			return true;
	}
	return false;
    }
    
double *TPrimer::CalcIUpair ( string base0 , string base , int i , bool max )
    {
    double *reValue = new double[3] ;
    double temp1[3] , temp2[3] ;
    string IUpacBase, pair1 , pair2 , base2 ;
    string seq = getAnnealingSequence() ;
    
    // Init
    int a , k ;
    for ( a = 0 ; a < 3 ; a++ )
       reValue[a] = temp1[a] = temp2[a] = 0 ;
    if ( i+1 < seq.length() ) base2 += seq[i+1] ;
    

    // JavaScript dump
	if(IsIUpacBase(base0))	//if previous base is IUpacBase, do nothing
	{	return reValue;	}
	
	if(IsIUpacBase(base) )
	{
		if(base=="M"){IUpacBase="AC";}
		else if(base=="R"){IUpacBase="AG";}
		else if(base=="W"){IUpacBase="AT";}
		else if(base=="S"){IUpacBase="CG";}
		else if(base=="Y"){IUpacBase="CT";}
		else if(base=="K"){IUpacBase="GT";}
		else if(base=="V"){IUpacBase="ACG";}
		else if(base=="H"){IUpacBase="ACT";}
		else if(base=="D"){IUpacBase="AGT";}
		else if(base=="B"){IUpacBase="CGT";}
		else if(base=="N"){IUpacBase="ACGT";}
		
		int j=0;
//		while(IUpacBase.charAt(j)!="")
        while ( j < IUpacBase.length() )
		{
//			base=IUpacBase.charAt(j);
			base=IUpacBase[j];

			pair1=base0+base;

			if(pair1=="AA"){temp1[0]= 1.2 ;temp1[1]=8.0; temp1[2]=21.9 ;}
			else if(pair1=="AT"){temp1[0]= 0.9 ;temp1[1]=5.6; temp1[2]=15.2  ;}
			else if(pair1=="TA"){temp1[0]=0.9  ;temp1[1]=6.6; temp1[2]= 18.4 ;}
			else if(pair1=="CA"){temp1[0]=1.7  ;temp1[1]=8.2; temp1[2]=21.0  ;}
			else if(pair1=="GT"){temp1[0]= 1.5 ;temp1[1]=9.4; temp1[2]=25.5  ;}
			else if(pair1=="CT"){temp1[0]= 1.5 ;temp1[1]=6.6; temp1[2]=16.4  ;}
			else if(pair1=="GA"){temp1[0]=1.5  ;temp1[1]=8.8; temp1[2]=23.5  ;}
			else if(pair1=="CG"){temp1[0]= 2.8 ;temp1[1]=11.8; temp1[2]=29.0  ;}
			else if(pair1=="GC"){temp1[0]=2.3  ;temp1[1]=10.5; temp1[2]=26.4  ;}
			else if(pair1=="GG"){temp1[0]=2.1  ;temp1[1]=10.9; temp1[2]=28.4  ;}
			
			if(base2==""){
				for(k=0; k<2; k++)
				{	temp2[k]=0.0;	}
			
			}else if(!IsIUpacBase(base2)){
				pair2=base+base2;
				if(pair2=="AA"){temp2[0]= 1.2 ;temp2[1]=8.0; temp2[2]=21.9 ;}
				else if(pair2=="AT"){temp2[0]= 0.9 ;temp2[1]=5.6; temp2[2]=15.2  ;}
				else if(pair2=="TA"){temp2[0]=0.9  ;temp2[1]=6.6; temp2[2]= 18.4 ;}
				else if(pair2=="CA"){temp2[0]=1.7  ;temp2[1]=8.2; temp2[2]=21.0  ;}
				else if(pair2=="GT"){temp2[0]= 1.5 ;temp2[1]=9.4; temp2[2]=25.5  ;}
				else if(pair2=="CT"){temp2[0]= 1.5 ;temp2[1]=6.6; temp2[2]=16.4  ;}
				else if(pair2=="GA"){temp2[0]=1.5  ;temp2[1]=8.8; temp2[2]=23.5  ;}
				else if(pair2=="CG"){temp2[0]= 2.8 ;temp2[1]=11.8; temp2[2]=29.0  ;}
				else if(pair2=="GC"){temp2[0]=2.3  ;temp2[1]=10.5; temp2[2]=26.4  ;}
				else if(pair2=="GG"){temp2[0]=2.1  ;temp2[1]=10.9; temp2[2]=28.4  ;}
			}else if(IsIUpacBase(base2)){
				base0=base;
                base=base2;
                i++; 
                double *tempp = CalcIUpair(base0,base,i,max);
                for ( a = 0 ; a < 3 ; a++ ) temp2[a] = tempp[a] ;
                delete tempp ;
				i--;
			}
			
			for(k=0;k<3;k++)
			{
				if(j==0){
					reValue[k]=temp1[k]+temp2[k];
				}else{
					if ((max)&&(reValue[k]<temp1[k]+temp2[k]))
					{	reValue[k]=temp1[k]+temp2[k];	
					}else if((!max)&&(reValue[k]>temp1[k]+temp2[k]))
					{	reValue[k]=temp1[k]+temp2[k]; 	
					}
				}
			}
			j++;
		}
	}
	return reValue;
    
    
    }

double TPrimer::DeltaG ( bool max )
{
    string seq = getAnnealingSequence() ;
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

double TPrimer::DeltaH ( bool max )
{
    string seq = getAnnealingSequence() ;
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

double TPrimer::DeltaS ( bool max )
{
    string seq = getAnnealingSequence() ;
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

void TPrimer::evaluateSelfAnnealing ()
    {
    string s = sequence ;
    string t ;
    int a , b , l = s.length() ;
    annScore = 0 ;
    ann1 = "" ;
    ann2 = "" ;
    annm = "" ;
    for ( a = 0 ; a < l ; a++ ) t = s[a] + t ;
    for ( a = -(l-1) ; a < l ; a++ )
        {
        int lm = 0 ;
        string ann_tmp ;
        for ( b = 0 ; b < l ; b++ )
           {
           if ( a+b >= 0 && a+b < l )
              {
              char c = '|' ;
              if ( s[a+b] == 'A' && t[b] == 'T' ) lm += 2 ;
              else if ( s[a+b] == 'T' && t[b] == 'A' ) lm += 2 ;
              else if ( s[a+b] == 'G' && t[b] == 'C' ) lm += 4 ;
              else if ( s[a+b] == 'C' && t[b] == 'G' ) lm += 4 ;
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
              for ( b = a ; b < 0 ; b++ ) ann1 = " " + ann1 ;
           if ( a > 0 )
              for ( b = a ; b < 0 ; b++ ) ann2 = " " + ann2 ;
           for ( b = 0 ; b < abs(a) ; b++ ) annm = " " + annm ;
           }
        }
    }
    
int TPrimer::checkFit ( TVector *v , bool justCount )
    {
    int a ;
    int count = 0 ;
    int match = 0 ;
    int match3 = 0 ;
    string s = get53sequence() ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        char c ;
        if ( upper ) c = v->getNucleotide ( a + from - 1 ) ;
        else c = v->getNucleotide ( to - a - 1 , true ) ;
        if ( c == s[a] )
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
    
string TPrimer::getAnnealingSequence()
    {
    if ( !annealingVector ) return get53sequence () ;
    string s = get53sequence () ;
    return s.substr ( s.length() - checkFit ( annealingVector , true ) ) ;
    }
    
bool TPrimer::overlap ( TPrimer &op )
    {
    if ( op.from <= to && op.to >= from ) return true ;
    return false ;
    }
    
