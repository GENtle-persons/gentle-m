#include "TVector.h"

// ***************************************************************************************
// TVector
// ***************************************************************************************

string TVector::getParams () { return params ; }
void TVector::setParams ( string s ) { params = s ; }
void TVector::setWindow ( ChildBase *c ) { window = c ; }
void TVector::setCircular ( bool c ) { circular = c ; }
bool TVector::isCircular () { return circular ; }
bool TVector::isLinear () { return !circular ; }
bool TVector::hasStickyEnds () { return (_lu+_ll+_ru+_rl!="") ; }

string TVector::one2three ( int a )
    {
    if ( a < 0 || a > 255 ) return "" ;
    return _one2three[a] ;
    }
    
void TVector::setStickyEnd ( bool left , bool upper , string s )
    {
    if ( left && upper ) _lu = s ;
    else if ( left && !upper ) _ll = s ;
    else if ( !left && upper ) _ru = s ;
    else if ( !left && !upper ) _rl = s ;
    }

string TVector::getStickyEnd ( bool left , bool upper )
    {
    if ( left && upper ) return _lu ;
    else if ( left && !upper ) return _ll ;
    else if ( !left && upper ) return _ru ;
    else if ( !left && !upper ) return _rl ;
    }



TVector::TVector ( ChildBase *win )
    {
    changed = true ;
    turned = 0 ;
    type = TYPE_VECTOR ;
    circular = false ;
    window = win ;
    undo = NULL ;
    init () ;
    }
    
TVector::~TVector ()
    {
    if ( undo ) delete undo ;
    }
        
void TVector::init ()
    {
    if ( undo ) delete undo ;
    undo = new TUndo ( this ) ;
    undo->setbase ( this ) ;
    
    int a ;
    for ( a = 0 ; a < 256 ; a++ ) IUPAC[a] = 0 ;
    setIUPAC ( 'A' , "A" ) ;
    setIUPAC ( 'C' , "C" ) ;
    setIUPAC ( 'T' , "T" ) ;
    setIUPAC ( 'G' , "G" ) ;
    for ( a = 0 ; a < 256 ; a++ ) SIUPAC[a] = IUPAC[a] ;
    setIUPAC ( 'R' , "AG" , SIUPAC ) ;
    setIUPAC ( 'Y' , "TC" , SIUPAC ) ;
    setIUPAC ( 'M' , "AC" , SIUPAC ) ;
    setIUPAC ( 'K' , "GT" , SIUPAC ) ;
    setIUPAC ( 'S' , "GC" , SIUPAC ) ;
    setIUPAC ( 'W' , "AT" , SIUPAC ) ;
    setIUPAC ( 'H' , "ACT" , SIUPAC ) ;
    setIUPAC ( 'B' , "GTC" , SIUPAC ) ;
    setIUPAC ( 'V' , "GCA" , SIUPAC ) ;
    setIUPAC ( 'D' , "GAT" , SIUPAC ) ;
    setIUPAC ( 'N' , "AGCT" , SIUPAC ) ;
    
    for ( a = 0 ; a < 256 ; a++ ) ACGT[a] = ' ' ;
    ACGT['t'] = ACGT['T'] = 0 ;
    ACGT['u'] = ACGT['U'] = 0 ;
    ACGT['c'] = ACGT['C'] = 1 ;
    ACGT['a'] = ACGT['A'] = 2 ;
    ACGT['g'] = ACGT['G'] = 3 ;
    
    aa = "FFLLSSSSYY||CC|W"
         "LLLLPPPPHHQQRRRR"
         "IIIMTTTTNNKKSSRR"
         "VVVVAAAADDEEGGGG" ;
         
    for ( a = 0 ; a < 256 ; a++ ) _one2three[a] = "???" ;
    _one2three['A'] = "Ala" ;
    _one2three['R'] = "Arg" ;
    _one2three['N'] = "Asn" ;
    _one2three['D'] = "Asp" ;
    _one2three['G'] = "Gly" ;
    _one2three['V'] = "Val" ;
    _one2three['L'] = "Leu" ;
    _one2three['I'] = "Ile" ;
    _one2three['P'] = "Pro" ;
    _one2three['F'] = "Phe" ;
    _one2three['Y'] = "Tyr" ;
    _one2three['W'] = "Trp" ;
    _one2three['C'] = "Cys" ;
    _one2three['M'] = "Met" ;
    _one2three['T'] = "Thr" ;
    _one2three['S'] = "Ser" ;
    _one2three['K'] = "Lys" ;
    _one2three['H'] = "His" ;
    _one2three['E'] = "Glu" ; 
    _one2three['Q'] = "Gln" ;
    _one2three['|'] = "STP" ;
    
    // Amino acid values
    for ( a = 0 ; a < 256 ; a++ ) aa_mw[a] = 0 ;
    aa_mw['A'] =  71.0788 ;
    aa_mw['R'] = 156.1875 ;
    aa_mw['N'] = 114.1038 ;
    aa_mw['D'] = 115.0886 ;
    aa_mw['C'] = 103.1388 ;
    aa_mw['E'] = 129.1155 ;
    aa_mw['Q'] = 128.1307 ;
    aa_mw['G'] =  57.0519 ;
    aa_mw['H'] = 137.1411 ;
    aa_mw['I'] = 113.1594 ;
    aa_mw['L'] = 113.1594 ;
    aa_mw['K'] = 128.1741 ;
    aa_mw['M'] = 131.1926 ;
    aa_mw['F'] = 147.1766 ;
    aa_mw['P'] =  97.1167 ;
    aa_mw['S'] =  87.0782 ;
    aa_mw['T'] = 101.1051 ;
    aa_mw['W'] = 186.2132 ;
    aa_mw['Y'] = 163.1760 ;
    aa_mw['V'] =  99.1326 ;    

    for ( a = 0 ; a < 256 ; a++ ) aa_pi[a] = 0 ;
    aa_pi['A'] =  6.01 ;
    aa_pi['R'] = 10.76 ;
    aa_pi['N'] =  5.41 ;
    aa_pi['D'] =  2.77 ;
    aa_pi['C'] =  5.07 ;
    aa_pi['E'] =  3.22 ;
    aa_pi['Q'] =  5.65 ;
    aa_pi['G'] =  5.97 ;
    aa_pi['H'] =  7.59 ;
    aa_pi['I'] =  6.02 ;
    aa_pi['L'] =  5.98 ;
    aa_pi['K'] =  9.74 ;
    aa_pi['M'] =  5.74 ;
    aa_pi['F'] =  5.48 ;
    aa_pi['P'] =  6.48 ;
    aa_pi['S'] =  5.68 ;
    aa_pi['T'] =  5.87 ;
    aa_pi['W'] =  5.89 ;
    aa_pi['Y'] =  5.66 ;
    aa_pi['V'] =  5.97 ;
    }

void TVector::removeBlanksFromSequence ()
    {
    string s ;
    int a ;
    for ( a = 0 ; a < sequence.length() ; a++ )
        if ( sequence[a] != ' ' )
           s += sequence[a] ;
    sequence = s ;
    }

bool TVector::basematch ( char b1 , char b2 ) // b1 in IUPAC, b2 in SIUPAC
   {
   return ( IUPAC[b1] & SIUPAC[b2] ) > 0 ;
   }

void TVector::insert_char ( char x , int pos , bool overwrite )
    {
    string dummy ;
    dummy = (char) x ;
    if ( overwrite )
       {
       sequence[pos-1] = x ;
       return ;
       }
    sequence.insert ( pos-1 , dummy ) ;
    int a ;
    for ( a = 0 ; a < items.size() ; a++ )
        {
        if ( items[a].from >= pos ) items[a].from++ ;
        if ( items[a].to >= pos ) items[a].to++ ;
        }
    }

float TVector::getAAmw ( char aa )
    {
    return aa_mw[aa] ;
    }
    
float TVector::getAApi ( char aa )
    {
    return aa_pi[aa] ;
    }
    
void TVector::setIUPAC ( char b , char *s , char *pac )
    {
    int x = 0 ;
    for ( unsigned char *c = (unsigned char*) s ; *c ; c++ )
        {
        if ( *c == 'A' || *c == 'a' ) x += IUPAC_A ;
        if ( *c == 'C' || *c == 'c' ) x += IUPAC_C ;
        if ( *c == 'G' || *c == 'g' ) x += IUPAC_G ;
        if ( *c == 'T' || *c == 't' ) x += IUPAC_T ;
        }
    if ( !pac ) pac = IUPAC ;
    pac[(unsigned char)b] = x ;
    }
    
    
vector <TRestrictionCut> TVector::getCuts ( TRestrictionEnzyme *e )
    {
    int b , c ;
    vector <TRestrictionCut> ret ;
    string rs = e->sequence ;
    string t = sequence ;
    if ( circular ) t += sequence ;
    for ( b = 0 ; b < sequence.length() ; b++ )
        {
        for ( c = 0 ; b+c < t.length() && c < rs.length() && basematch ( t[b+c] , rs[c] ) ; c++ ) ;
        if ( c == rs.length() )
           {
           int thecut = (b+e->cut)%sequence.length() ;
           if ( thecut >= 0 && thecut < sequence.length() )
              ret.push_back ( TRestrictionCut ( thecut , e ) ) ;
           }
        }
    return ret ;
    }
    
void TVector::recalculateCuts ()
    {
    int a , b , c ;
    while ( rc.size() ) rc.pop_back () ;
    for ( a = 0 ; a < re.size() ; a++ )
       {
       vector <TRestrictionCut> x = getCuts ( re[a] ) ;
       for ( c = 0 ; c < x.size() ; c++ )
           rc.push_back ( x[c] ) ;
       }
    
    // Sorting by position; might be unnecessary
    a = 0 ;
    while ( a+1 < rc.size() )
        {
        if ( rc[a].pos > rc[a+1].pos )
           {
           TRestrictionCut z = rc[a] ;
           rc[a] = rc[a+1] ;
           rc[a+1] = z ;
           a = 0 ;
           }
        else a++ ;
        }
    }
    
// ******** ACTION!

#define MIN(_i,_o) (((_i)<(_o)?(_i):(_o)))
#define MAX(_i,_o) (((_i)>(_o)?(_i):(_o)))

// This function reduces the sequence of the vector and the associated objects
// to the fragment between two cuts of restriction enzymes
bool TVector::reduceToFragment ( TRestrictionCut left , TRestrictionCut right )
    {
    int a , from , to ;
    string s ;

    from = left.pos ;
    to = right.pos ;
    if ( to <= from ) to += sequence.length() ;

    from -= left.e->cut ;
    to += right.e->sequence.length() - right.e->cut - 1 ;

    int from2 = from + left.e->sequence.length() ;
    int to2 = to - right.e->sequence.length() + 1 ;
    
    for ( a = from ; a <= to ; a++ )
        s += getNucleotide ( a ) ;
    
    _lu = left.e->getEndUpperRight () ;
    _ll = left.e->getEndLowerRight () ;
    _ru = right.e->getEndUpperLeft () ;
    _rl = right.e->getEndLowerLeft () ;
    
    // How many overlapping nucleotides are on both strands on either side?
    int lo = MIN ( _lu.length() , _ll.length() ) ;
    int ro = MIN ( _ru.length() , _rl.length() ) ;
    
    // Cutting sticky ends
    TVector dv ;
    dv.sequence = s ;
    string is = dv.transformSequence ( true , false ) ; // Inverse sequence
    _lu =  s.substr ( left.e->sequence.length() - _lu.length() , _lu.length() - lo ) ;
    _ll = is.substr ( left.e->sequence.length() - _ll.length() , _ll.length() - lo ) ;
    _ru =  s.substr ( s.length() - right.e->sequence.length() + ro , _ru.length() - ro ) ;
    _rl = is.substr ( s.length() - right.e->sequence.length() + ro , _rl.length() - ro ) ;

    // Removing sticky ends from main sequence
    s = s.substr ( 0 , s.length() - right.e->sequence.length() + ro ) ;
    s = s.substr ( left.e->sequence.length() - lo ) ;
    
    from2 -= lo ;
    to2 += ro ;

    // Taking care of items
    if ( to2+1 > sequence.length() )
        {
        int rto = to2+1-sequence.length() ;
        doRemove ( rto , from2 , false ) ;
        turn ( -rto+1 ) ;
        }
    else
        {
        doRemove ( to2 + 1 , sequence.length() , false ) ;
        doRemove ( 1 , from2 , false ) ;
        }
    circular = false ;

    return true ; // Success; actually, no "return false" yet.
    }
    
char TVector::getNucleotide ( int pos , bool complement )
    {
    int sl = sequence.length() ;
    while ( pos < 0 ) pos += sl ;
    while ( pos > sl ) pos -= sl ;
    if ( complement ) return getComplement ( sequence[pos] ) ;
    return sequence[pos] ;
    }
    
char TVector::getComplement ( char c )
    {
    if ( c == 'A' ) return 'T' ;
    if ( c == 'T' ) return 'A' ;
    if ( c == 'G' ) return 'C' ;
    if ( c == 'C' ) return 'G' ;
    return ' ' ;
    }
    
string TVector::transformSequence ( bool inverse , bool reverse )
    {
    int a ;
    string r = sequence ;
    if ( inverse )
        {
        for ( a = 0 ; a < r.length() ; a++ )
           r[a] = getComplement ( r[a] ) ;
        }
    if ( reverse )
        {
        string s = r ;
        for ( a = 0 ; a < r.length() ; a++ )
           r[a] = s[s.length()-a-1] ;
        }
    return r ;
    }

void TVector::doRestriction ()
    {
    vector <int> rCut ;
    vector <string> rLU , rLL , rRU , rRL , rEnzyme , rSequence ;
    vector <TRestrictionCut> cl ;
    int a , b , c ;
    
    if ( cocktail.size() == 0 ) return ;
    wxASSERT_MSG ( myapp() , "Oh no! No application defined!" ) ;
    myapp()->frame->lastCocktail = cocktail ;
    
    TRestrictionEnzyme blankEnzyme ;
    blankEnzyme.cut = 0 ;
    blankEnzyme.sequence = "" ;
    blankEnzyme.overlap = 0 ;
    if ( !circular )
        cl.push_back ( TRestrictionCut ( 0 , &blankEnzyme ) ) ;

    bool doRecalc = false ;
    for ( a = 0 ; a < cocktail.size() ; a++ )
       {
       for ( b = 0 ; b < re.size() && re[b]->name != cocktail[a] ; b++ ) ;
       if ( b == re.size() )
          {
          doRecalc = true ;
          recalcvisual = true ;
          re.push_back ( myapp()->frame->LS->getRestrictionEnzyme ( cocktail[a] ) ) ;
          wxASSERT_MSG ( re[re.size()-1] , "Oh no! Unknown enzyme!" ) ;
          }
       }
    if ( doRecalc ) recalculateCuts() ;

    
    for ( a = 0 ; a < cocktail.size() ; a++ )
        {
        for ( b = 0 ; b < rc.size() ; b++ )
           if ( rc[b].e->name == cocktail[a] )
              cl.push_back ( rc[b] ) ;
        }

    // Arranging
    for ( a = 1 ; a < cl.size() ; a++ )
        {
        if ( cl[a-1].pos > cl[a].pos )
           {
           TRestrictionCut ct = cl[a] ;
           cl[a] = cl[a-1] ;
           cl[a-1] = ct ;
           a = 0 ;
           }
        }
    
    if ( circular )
        cl.push_back ( cl[0] ) ;
    else
        cl.push_back ( TRestrictionCut ( sequence.length()-1 , &blankEnzyme ) ) ;
        
    for ( a = 0 ; a+1 < cl.size() ; a++ )
        {
        char t1[100] , t2[100] ;
        sprintf ( t1 , "%d" , cl[a].pos ) ;
        sprintf ( t2 , "%d" , cl[a+1].pos ) ;
        TVector *nv = new TVector ;
        *nv = *this ;
        nv->reduceToFragment ( cl[a] , cl[a+1] ) ;
//        for ( int w = 0 ; w < nv->items.size() ; w++ ) nv->items[w].dummyInfo ( "lacZ" , nv->sequence.length() ) ;
        nv->recalculateCuts () ;
        nv->recalcvisual = true ;
        if ( nv->desc != "" ) nv->desc += "\n" ;
        char tx[1000] ;
        sprintf ( tx , txt("res_desc") , nv->name.c_str() , cl[a].e->name.c_str() , t1 ) ;
        nv->desc += tx ;
        if ( cl[a].e->name == cl[a+1].e->name )
           {
           nv->desc += "." ;
           nv->name += " (" + cl[a].e->name + ")" ;
           }
        else
           {
           sprintf ( tx , txt("res_desc2") , cl[a+1].e->name.c_str() , t2 ) ;
           nv->name += " (" + cl[a].e->name + "/" + cl[a+1].e->name + ")" ;
           }
        while ( nv->cocktail.size() ) nv->cocktail.pop_back () ; // Cleaning up cocktail
        if ( nv->sequence.length() >= action_value ) // "No fragments smaller than XX base pairs"
           window->app->frame->newFromVector ( nv , TYPE_FRAGMENT ) ;        
        else delete nv ;
        }
    while ( cocktail.size() ) cocktail.pop_back () ;
    }
        
void TVector::doAction ()
    {
    if ( action == "RESTRICTION" ) doRestriction () ;
    action = "" ;
    return ;
    }
    
string TVector::invert ( string s )
    {
    string t ;
    for ( int a = 0 ; a < s.length() ; a++ ) t = s[a] + t ;
    return t ;
    }
    
// This function can attach a linear vector at the "right" end.
// It does *NOT* test if the action is valid!
void TVector::ligate_right ( TVector v , bool inverted )
    {
    if ( circular ) return ;
    if ( v.circular ) return ;
    int a , b ;
    
    // Is the ligand (to the right!) inverted?
    if ( inverted )
        {
        v.items.clear () ; // Inversion will delete all features!
        string ll = v._ll ;
        string lu = v._lu ;
        string rl = v._rl ;
        string ru = v._ru ;
        v._ll = invert ( ru ) ;
        v._lu = invert ( rl ) ;
        v._rl = invert ( lu ) ;
        v._ru = invert ( ll ) ;
        v.sequence = v.transformSequence ( true , true ) ;
        }
    
    // Merging sequence
    string ol = _ru + v._lu ;
    sequence += ol ;
    sequence += v.sequence ;
    _ru = v._ru ;
    _rl = v._rl ;
    if ( turned == 0 ) turned = v.turned ;
    
    // Merging items
    for ( a = 0 ; a < v.items.size() ; a++ )
        {
        TVectorItem i = v.items[a] ;
        i.from += sequence.length() - v.sequence.length() ;
        i.to += sequence.length() - v.sequence.length() ;
        for ( b = 0 ; b < items.size() ; b++ )
           {
           if ( items[b].name == i.name && items[b].type == i.type )
              {
              b = -1 ;
              i.name += "*" ;
              }
           }
        items.push_back ( i ) ;
        }
    
    // Merging restriction enzymes
    vector <TRestrictionEnzyme*> e ;
    for ( a = 0 ; a < re.size() ; a++ ) e.push_back ( re[a] ) ;
    for ( a = 0 ; a < v.re.size() ; a++ ) e.push_back ( v.re[a] ) ;
    sort ( e.begin() , e.end() ) ;
    while ( re.size() ) re.pop_back () ;
    for ( a = 0 ; a < e.size() ; a++ )
        {
        if ( a == 0 || e[a] != e[a-1] )
           re.push_back ( e[a] ) ;
        }
    }
    
// This function can close the circle after ligation.
// It does *NOT* test if the action is valid!
void TVector::closeCircle ()
    {
    if ( circular ) return ;
    sequence += _ru ;
    sequence += _lu ;
    _ru = _lu = _rl = _ll = "" ;
    circular = true ;
    recalcvisual = true ;
    if ( turned != 0 ) turn ( -turned ) ;
    }

// This function is a plague
void TVector::doRemove ( int from , int to , bool update )
    {
    int a , b ;
    int l = sequence.length() ;
    
    // Sequence
    string s = sequence , t ;
    int rt = to ;
    if ( rt < from ) rt += l ;
    for ( a = from ; a <= rt ; a++ )
       {
       b = a ;
       if ( b > l ) b -= l ;
       t += s[b-1] ;
       s[b-1] = ' ' ;
       }
    sequence = "" ;
    for ( a = 0 ; a < l ; a++ )
       if ( s[a] != ' ' ) sequence += s[a] ;
       
    // Items
    for ( a = 0 ; a < items.size() ; a++ )
       {
       items[a].doRemove ( from , to , l ) ;
       if ( items[a].from == -1 )
          {
          items.erase ( items.begin() + a ) ;
          a-- ;
          }
       }

    // Finish
    setChanged () ;
    if ( update )
        {
        recalculateCuts () ;
        recalcvisual = true ;
        }
    }
    
// Currently returns only the direct encoding (a single char) or 'X'
// Could return all possible AAs (see IUPAC) in the future
string TVector::dna2aa ( string codon )
    {
    if ( codon.length() != 3 ) return "?" ;
    string r ;
    if ( codon[0] == ' ' ) return "?" ;
    if ( codon[1] == ' ' ) return "?" ;
    if ( codon[2] == ' ' ) return "?" ;
    int i = ACGT[codon[0]]*16 +
            ACGT[codon[1]]*4 +
            ACGT[codon[2]] ;
    if ( i > 63 ) r = "?" ;
    else r = aa[i] ;
    return r ;
    }
    
void TVector::turn ( int off )
    {
    if ( !circular || off == 0 ) return ;
    int a ;
    for ( a = 0 ; a < items.size() ; a++ )
        {
        items[a].from += off ;
        items[a].to += off ;

        while ( items[a].from < 1 ) items[a].from += sequence.length() ;
        while ( items[a].from > sequence.length() ) items[a].from -= sequence.length() ;

        while ( items[a].to < 1 ) items[a].to += sequence.length() ;
        while ( items[a].to > sequence.length() ) items[a].to -= sequence.length() ;
        }
    if ( off > 0 )
        {
        off = sequence.length() - off ;
        sequence = sequence.substr ( off ) + sequence.substr ( 0 , off ) ;
        }
    else
        {
        sequence = sequence.substr ( -off ) + sequence.substr ( 0 , -off ) ;
        }
    recalculateCuts () ;
    recalcvisual = true ;
    turned += off ;
    }
    
int TVector::countCuts ( string enzyme )
    {
    int a , count ;
    for ( a = count = 0 ; a < rc.size() ; a++ )
        if ( rc[a].e->name == enzyme )
           count++ ;
    return count ;
    }
        
void TVector::setAction ( string _action , int _action_value )
    {
    action = _action ;
    action_value = _action_value ;
    }
        
string TVector::getSubstring ( int mf , int mt )
    {
    int l = sequence.length() ;
    mf-- ;
    mt-- ;
    if ( mt <= l )
        {
        return sequence.substr ( mf , mt-mf+1 ) ;
        }
    else
        {
        string s ;
        s += sequence.substr ( 0 , mt-l+1 ) ;
        s += sequence.substr ( mf ) ;
        return s ;
        }
    }
    
void TVector::setChanged ( bool c )
    {
    if ( c == changed ) return ;
    changed = c ;
    if ( window ) window->showName () ;
    }
    
void TVector::ClearORFs ()
    {
    while ( worf.size() ) worf.pop_back () ;
    }
    
void TVector::addORFs ( int off )
    {
    int a , b ;
    int dir ;
    bool complement = false ;
    if ( off > 0 )
        {
        dir = 1 ;
        b = off - 1 ;
        }
    else
        {
        dir = -1 ;
        for ( b = 0 ; b <= sequence.length() ; b += 3 ) ;
        b -= 3 ;
        b += off + 1 ;
        complement = true ;
        }
    for ( a = b ; a+dir*3 > 0 && a+dir*3 < sequence.length() ; a += dir * 3 )
        {
        string codon ;
        codon += getNucleotide ( a + dir * 0 , complement ) ;
        codon += getNucleotide ( a + dir * 1 , complement ) ;
        codon += getNucleotide ( a + dir * 2 , complement ) ;
        if ( codon == "ATG" )
           {
           int cnt = (sequence.length()+2)/3 ;
           int aa = 0 ;
           for ( b = a ; cnt > 0 ; b += dir * 3 )
              {
              cnt-- ;
              aa++ ;
              if ( b < 0 ) b = sequence.length() - b ;
              if ( b >= sequence.length() ) b -= sequence.length() ;
              codon = "" ;
              codon += getNucleotide ( b + dir * 0 , complement ) ;
              codon += getNucleotide ( b + dir * 1 , complement ) ;
              codon += getNucleotide ( b + dir * 2 , complement ) ;
              if ( codon == "TAA" ||
                   codon == "TAG" ||
                   codon == "TGA" )
                 {
                 if ( aa > 100 )
                    {
                    int from = a ;
                    int to = b + dir * 2 ;
                    if ( from < to || dir == 1 )
                       worf.push_back ( TORF ( from , to , off ) ) ;
                    else
                       worf.push_back ( TORF ( to , from , off ) ) ;
                    }
                 cnt = 0 ;
                 }
              }
           }
        }
    }
    
TVector *TVector::getAAvector ( int from , int to , int dir )
    {
    char UNIQUE = '#' ;
    // Creating new vector
    TVector *v = new TVector ;
    *v = *this ;
    v->type = TYPE_AMINO_ACIDS ;

    if ( to < from ) to += v->sequence.length() ;
    v->turn ( -from+1 ) ;
    to -= from + 1 ;
    from = 1 ;
    
    int a , b ;

    // Break features, if necessary
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        if ( v->items[a].from > v->items[a].to )
           {
           TVectorItem i = v->items[a] ;
           i.from = 1 ;
           v->items[a].to = v->sequence.length() ;
           v->items.push_back ( i ) ;
           }
        }
    v->circular = false ;
    
    SeqAA aas ( NULL ) ;
    aas.disp = AA_ONE ;
    if ( dir > 0 ) aas.mode = AA_THREE_1 ;
    else if ( dir < 0 )
        {
        if ( ( to - 1 ) % 3 == 0 ) aas.mode = AA_THREE_M1 ;
        if ( ( to - 1 ) % 3 == 1 ) aas.mode = AA_THREE_M2 ;
        if ( ( to - 1 ) % 3 == 2 ) aas.mode = AA_THREE_M3 ;
        }
    else aas.mode = AA_KNOWN ;
    aas.initFromTVector ( v ) ;
    v->sequence = aas.s ;
    for ( a = to+1 ; a < v->sequence.length() ; a++ ) v->sequence[a] = UNIQUE ;
    for ( a = 0 ; a < v->sequence.length() ; a++ )
        if ( v->sequence[a] == ' ' || v->sequence[a] == '|' )
           v->sequence[a] = UNIQUE ;
    
    
    // Cleaning restriction enzymes
    v->re.clear() ;
    v->rc.clear() ;

    // Removing non-AA sequence and features
    for ( a = 0 ; a < v->sequence.length() ; a++ )
        {
        if ( v->sequence[a] == UNIQUE )
           {
           for ( b = a+1 ; b < v->sequence.length() && v->sequence[b] == UNIQUE ; b++ ) ;
           v->doRemove ( a+1 , b , false ) ;
           a-- ;
           }
        }
        

    // Fixing features, if neccessary
    // PATCH AS PATCH CAN!!!
    if ( dir < 0 )
        {
        string t ;
        for ( a = 0 ; a < v->sequence.length() ; a++ ) t = v->sequence[a] + t ;
        v->sequence = t ;
        for ( a = 0 ; a < v->items.size() ; a++ )
           {
           int f = v->sequence.length() - v->items[a].to + 1 ;
           v->items[a].to = v->sequence.length() - v->items[a].from + 1 ;
           v->items[a].from = f ;
           }
        }
    v->items.clear() ;

    v->circular = false ;
    return v ;
    }
    
void TVector::callUpdateUndoMenu ()
    {
    if ( !window ) return ;
    window->updateUndoMenu() ;
    }

    
// ***************************************************************************************
// TVectorItem
// ***************************************************************************************

TVectorItem::TVectorItem()
    {
    brush = *wxBLUE_BRUSH;
    initParams () ;
    }

TVectorItem::TVectorItem ( wxString sn , wxString n , int f , int t , char ty = VIT_MISC )
    {
    brush = *wxBLUE_BRUSH;
    initParams () ;
    name = sn ;
    desc = n ;
    from = f ;
    to = t ;
    type = ty ;
    direction = 1 ;
    r3 = r4 = -1 ;
    }

wxBrush *TVectorItem::getBrush ()
    {
    if ( getParam ( "ISDEFAULTBRUSH" ) == "1" )
        {
        if ( type == VIT_GENE ) brush = *wxBLUE_BRUSH ;
        if ( type == VIT_CDS  ) brush = *wxRED_BRUSH ;
        if ( type == VIT_REP_ORI ) brush = *wxBLACK_BRUSH ;
        if ( type == VIT_PROMOTER ) brush = *wxGREY_BRUSH ;
        if ( type == VIT_TERMINATOR ) brush = *wxMEDIUM_GREY_BRUSH ;
        if ( type == VIT_MISC ) brush = *wxBLACK_BRUSH ;
        }
    return &brush ;
    }

wxColour TVectorItem::getFontColor ()
    {
    wxBrush *b = getBrush() ;
    wxColour c = b->GetColour() ;
    if ( b == wxTRANSPARENT_BRUSH || b == wxWHITE_BRUSH )
        c = wxColour ( 255 , 255 , 255 ) ;
    return c ;
    }
    
bool TVectorItem::isVisible ()
    {
    string s = getParam ( "ISVISIBLE" ) ;
    return ( s == "1" ) ;
    }
    
void TVectorItem::setVisible ( bool b )
    {
    setParam ( "ISVISIBLE" , b ) ;
    }
    
void TVectorItem::setColor ( wxColour col )
    {
    setParam ( "ISDEFAULTBRUSH" , false ) ;
    setParam ( "COLOR_RED" , col.Red() ) ;
    setParam ( "COLOR_GREEN" , col.Green() ) ;
    setParam ( "COLOR_BLUE" , col.Blue() ) ;
    brush.SetColour ( col ) ;
    }
    
//********************* PARAMS

void TVectorItem::setParam ( string p , string v )
    {
    int a ;
    for ( a = 0 ; a < pname.size() && p != pname[a] ; a++ ) ;
    if ( a == pname.size() )
        {
        pname.push_back ( p ) ;
        pvalue.push_back ( v ) ;
        }
    else pvalue[a] = v ;
    }
    
void TVectorItem::setParam ( string p , int v )
    {
    char u[100] ;
    sprintf ( u , "%d" , v ) ;
    setParam ( p , string ( u ) ) ;
    }
    
string TVectorItem::getParam ( string p , string def )
    {
    int a ;
    for ( a = 0 ; a < pname.size() && p != pname[a] ; a++ ) ;
    if ( a == pname.size() ) return def ;
    return pvalue[a] ;
    }
    
vector <string> TVectorItem::getParamKeys ()
    {
    return pname ;
    }

string TVectorItem::implodeParams ()
    {
    string s ;
    int a , b ;
    for ( a = 0 ; a < pname.size() ; a++ )
        {
        string t = pvalue[a] ;
        for ( b = 0 ; b < t.length() ; b++ )
           if ( t[b] == '\n' )
              t[b] = 2 ;
        s += pname[a] + "\n" ;
        s += t + "\n" ;
        }
    return s ;
    }

void TVectorItem::explodeParams ( string s )
    {
    int a ;
    initParams () ;
    while ( s != "" )
        {
        for ( a = 0 ; s[a] != '\n' ; a++ ) ;
        string n = s.substr ( 0 , a ) ;
        s = s.substr ( a + 1 ) ;
        for ( a = 0 ; s[a] != '\n' ; a++ ) ;
        string v = s.substr ( 0 , a ) ;
        for ( a = 0 ; a < v.length() ; a++ )
           if ( v[a] == 2 )
              v[a] = '\n' ;
        s = s.substr ( a + 1 ) ;
        setParam ( n , v ) ;
        }
    if ( getParam ( "ISDEFAULTBRUSH" ) != "1" )
        {
        setColor ( wxColour ( atoi ( getParam("COLOR_RED").c_str() ) ,
                              atoi ( getParam("COLOR_GREEN").c_str() ) ,
                              atoi ( getParam("COLOR_BLUE").c_str() ) ) ) ;
        }
    }

void TVectorItem::initParams ()
    {
    r1 = r2 = -1 ;
    pname = vector <string> () ;
    pvalue = vector <string> () ;
    setParam ( "ISDEFAULTBRUSH" , true ) ;
    setVisible ( true ) ;
    }

int TVectorItem::getRF ()
    {
    if ( type != VIT_CDS ) return 0 ;
    return atoi ( (char*) getParam("/codon_start").c_str() ) ;
    }
    
void TVectorItem::setRF ( int x )
    {
    if ( type != VIT_CDS ) return ;
    setParam ( "/codon_start" , x ) ;
    }
    
void TVectorItem::doRemove ( int f , int t , int l )
    {
    int rt = t ;
    int rto = to ;
    if ( t < f ) rt += l ;
    if ( to < from ) rto += l ;
    int a ;
    string s = "_" ;
    while ( s.length() < l * 3 ) s += s ;
    for ( a = from ; a <= rto ; a++ ) s[a] = 'X' ;
    for ( a = f ; a <= rt ; a++ ) s[a] = ' ' ;
    from = -1 ;
    to = -1 ;
    int ff = 0 ;
    for ( a = 1 ; a < s.length() ; a++ )
       {
       if ( s[a] == ' ' ) ff++ ;
       else if ( s[a] == 'X' )
          {
          if ( from == -1 ) from = a - ff ;
          to++ ;
          }
       }
    to += from ;
    l -= rt - f + 1 ;
    while ( to > l ) to -= l ;
    while ( from > l ) from -= l ;
    }
    
void TVectorItem::dummyInfo ( string s , int l )
    {
    if ( s != name.c_str() ) return ;
    char u[100] ;
    int len = to - from + 1 ;
    if ( to < from ) len += l ;
    sprintf ( u , "%d-%d (%d)" , from , to , len ) ;
    wxMessageBox ( u , name.c_str() ) ;
    }
    
int TVectorItem::getOffset ()
    {
    return atoi ( getParam ( "OFFSET" , "-1" ).c_str() ) ;
    }
    
void TVectorItem::setOffset ( int o )
    {
    setParam ( "OFFSET" , o ) ;
    }
    
