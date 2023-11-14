#include "TVector.h"

#define MAXCUTS_PER_1K 30

// ***************************************************************************************
// TVector
// ***************************************************************************************

char TVector::ACGT[256] ;
char TVector::IUPAC[256] ;
char TVector::SIUPAC[256] ;
char TVector::COMPLEMENT[256] ;
vector <TAAProp> TVector::aaprop ;
wxArrayString TVector::codon_tables ;
wxArrayString TVector::codon_table_names ;

void TVector::setWindow ( ChildBase * const c ) { window = c ; }
void TVector::setCircular ( const bool c ) { circular = c ; }
bool TVector::isCircular () const { return circular ; }
bool TVector::isLinear () const { return !circular ; }
bool TVector::hasStickyEnds () const { return (_lu+_ll+_ru+_rl!=_T("")) ; }
float TVector::getAAmw ( char aa ) const { return aaprop[aa].mw ; }
float TVector::getAApi ( char aa ) const { return aaprop[aa].pi ; }
char TVector::getComplement ( const char c ) const { return COMPLEMENT[c] ; }
TAAProp TVector::getAAprop ( const char a ) const { return aaprop[a] ; }
void TVector::setGenomeMode ( const bool gm ) { genomeMode = gm ; }
bool TVector::getGenomeMode () const { return genomeMode ; }
wxString *TVector::getSequencePointer () { return &sequence ; }
TORF *TVector::getORF ( const int a ) { return &worf[a] ; }
int TVector::countORFs () { return worf.size() ; }
void TVector::updateDisplay ( bool update ) { recalcvisual = update ; }
bool TVector::displayUpdate () { return recalcvisual ; }
void TVector::setType ( const int newtype ) { type = newtype ; }
int TVector::getType () const { return type ; }
int TVector::getMethylationSiteIndex ( const int pos ) const { return methyl.Index ( pos ) ; }
int TVector::getMethylationSite ( const int index ) const { return methyl[index] ; }
int TVector::countMethylationSites () const { return methyl.GetCount() ; }
TEnzymeRules *TVector::getEnzymeRules () const { return enzyme_rules ; }
void TVector::setEnzymeRules ( TEnzymeRules * const er ) { enzyme_rules = er ; }
int TVector::countCodonTables () const { return codon_table_names.size() ; }
wxString TVector::getCodonTableName ( const int x ) const { return codon_table_names[x] ; }
void TVector::resetTurn () { turned = 0 ; }


void TVector::prepareFeatureEdit ( const int _pos , const bool overwrite )
{
    int mode = myapp()->frame->editFeatureMode ;
        int pos = _pos;

    if ( mode == 0 ) return ; // Keep as is

    int a ;
    for ( a = 0 ; a < items.size() ; a++ )
        {
        int from = items[a].from ;
        int to = items[a].to ;
        if ( to < from )
              {
            if ( pos > to && pos+overwrite <= from ) continue ; // Change doesn't concern this feature
             }
        else
            {
            if ( pos+overwrite <= from || pos > to ) continue ; // Change doesn't concern this feature
            }

        if ( mode == 1 ) // Change name
            {
               if ( items[a].name.Right ( 1 ) != _T("*") )
                   {
                   items[a].name += _T("*") ;
                   updateDisplay () ;
                   }
            }
        else if ( mode == 2 ) // Cut off feature
              {
            if ( to < from )
                {
                 to += sequence.length() ;
                 if ( pos <= from ) pos += sequence.length() ;
                }

            if ( items[a].getDirection() < 0 ) // <---
                {
                from = pos + overwrite ;
                }
              else // --->
                   {
                   to = pos - 1 ;
                   if ( to == sequence.length() && from < to ) to-- ; // Bug fix
                 }
              while ( from <= 0 ) from += sequence.length() ;
              while ( to <= 0 ) to += sequence.length() ;
              while ( from > sequence.length() ) from -= sequence.length() ;
              while ( to > sequence.length() ) to -= sequence.length() ;
              items[a].from = from ;
              items[a].to = to ;
               if ( items[a].name.Right ( 1 ) != _T("*") ) items[a].name += _T("*") ;
              updateDisplay () ;
             }
        }
    }

void TVector::getItemsAtPosition ( const int pos , wxArrayInt &vi , const bool limit ) const
    {
    vi.clear () ;
    int a ;
    for ( a = 0 ; ( vi.size() == 0 || !limit ) && a < items.size() ; a++ )
        {
        if ( items[a].from <= items[a].to ) // "Normal"
            {
            if ( pos < items[a].from-1 || pos > items[a].to-1 ) continue ;
            }
        else // Goes round the "0" point
            {
            if ( pos > items[a].to-1 && pos < items[a].from-1 ) continue ;
            }
        vi.Add ( a ) ;
        }
    }

bool TVector::hasItemsAtPosition ( const int pos ) const
    {
    wxArrayInt vi ;
    getItemsAtPosition ( pos , vi , true ) ;
    return vi.size() > 0 ;
    }

wxString TVector::getStrand53 () const
    {
    return _lu + sequence + _ru ;
    }
     
wxString TVector::getStrand35 () const
    {
    wxString t1 , t2 , t3 ;
    int a ;
    for ( a = 0 ; a < _rl.length() ; a++ ) t1 = _rl.GetChar(a) + t1 ;
    for ( a = 0 ; a < sequence.length() ; a++ ) t2 = ((wxChar) getComplement ( sequence.GetChar(a) )) + t2 ;
    for ( a = 0 ; a < _ll.length() ; a++ ) t3 = _ll.GetChar(a) + t3 ;
    return t1 + t2 + t3 ;
    }
     
int TVector::showGC () const // Return 0 for "no", otherwise number of blocks
    {
    if ( type == TYPE_AMINO_ACIDS ) return 0 ;
    int r = getSequenceLength() / 5 ;
    return getEnzymeRule()->showgc ? ( r > 400 ? 400 : r ) : 0 ;
    }

wxString TVector::getParams () const
    {
    wxString params ;
    for ( int a = 0 ; a < paramk.GetCount() ; a++ )
    {
        if ( paramk[a].StartsWith ( _T("toomanycuts") ) ) continue ; // No need to save this
        params += _T("#") + paramk[a] + _T("\n") + paramv[a] + _T("\n") ;
    }
    return params ;
    }
    
wxString TVector::getParam ( const wxString& key ) const
    {
    int a ;
    for ( a = 0 ; a < paramk.GetCount() && paramk[a] != key ; a++ ) ;
    if ( a == paramk.GetCount() ) return _T("") ;
    else return paramv[a] ;
    }
     
void TVector::setParam ( const wxString& key , const wxString& value ) /* not const */
    {
    int a ;
    for ( a = 0 ; a < paramk.GetCount() && paramk[a] != key ; a++ ) ;
    if ( a == paramk.GetCount() )
        {
         paramk.Add ( key ) ;
         paramv.Add ( value ) ;
        }
    else paramv[a] = value ;
    evaluate_key_value ( key , value ) ;
    }

void TVector::setParams ( wxString t ) /* not const */
    {
    paramk.Clear () ;
    paramv.Clear () ;
    if ( t.IsEmpty() || t.GetChar(0) != '#' ) t = _T("#genbank\n") + t ; // Backwards compatability
    wxArrayString vs ;
    explode ( _T("\n") , t , vs ) ;
    int a ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( vs[a].Left ( 1 ) == _T("#") )
            {
            paramk.Add ( vs[a].Mid ( 1 ) ) ;
            paramv.Add ( _T("") ) ;
            }
        else paramv[paramv.GetCount()-1] += vs[a] + _T("\n") ;
        }
    for ( a = 0 ; a < paramk.GetCount() ; a++ )
        evaluate_key_value ( paramk[a] , paramv[a] ) ;
    }
    
void TVector::evaluate_key_value ( const wxString& key , const wxString& value )
    {
    if ( key == _T("enzymerule") )
        {
        if ( !enzyme_rules ) enzyme_rules = new TEnzymeRules ;
        enzyme_rules->from_string ( value ) ;
        recalculateCuts () ;
        recalcvisual = true ;
        }
    }

void TVector::methylationSites ( wxArrayInt &vi , const int what )
    {
    vi.Clear () ;
    int a ;
    if ( what & DAM_METHYLATION )
        {
        for ( a = 0 ; a < sequence.length() ; a++ )
            {
               if ( getNucleotide ( a ) == 'G' &&
                    getNucleotide ( a + 1 ) == 'A' &&
                    getNucleotide ( a + 2 ) == 'T' &&
                    getNucleotide ( a + 3 ) == 'C' )
                vi.Add ( a + 1 ) ;
            }
        }
    if ( what & DCM_METHYLATION )
        {
        for ( a = 0 ; a < sequence.length() ; a++ )
            {
               if ( getNucleotide ( a ) == 'C' &&
                    getNucleotide ( a + 1 ) == 'C' &&
                    getNucleotide ( a + 3 ) == 'G' &&
                    getNucleotide ( a + 4 ) == 'G' &&
                 basematch ( getNucleotide ( a + 2 ) , 'W' ) )
                vi.Add ( a + 1 ) ;
            }
        }
    }

TVector *TVector::newFromMark ( const int from , const int to )  const
    {
//  char t[1000] ;
    TVector *nv = new TVector ;
    nv->setFromVector ( *this ) ;
    if ( from == 1 && to == sequence.length() ) return nv ; // The whole vector
    if ( to > nv->sequence.length() ) // Marking includes "0" point
        {
        nv->doRemove ( to+1-nv->sequence.length() , from-1 , false ) ;
        }
    else
        {
        nv->doRemove ( to+1 , nv->sequence.length() , false ) ;
        nv->doRemove ( 1 , from-1 , false ) ;
        }

    if ( to > sequence.length() )
            nv->turn ( sequence.length() - from + 1 ) ;
    if ( !nv->getDescription().IsEmpty() )
        nv->addDescription ( _T("\n") ) ;
    wxString t = wxString::Format ( txt("t_cropped_fragment") , nv->getName().c_str() , from , to ) ;
    nv->addDescription ( t ) ;
    nv->addName ( _T("*") ) ;
    nv->setChanged () ;
    nv->setCircular ( false ) ;
    nv->recalculateCuts() ;
    nv->recalcvisual = true ;
    return nv ;
    }
    
void TVector::setNucleotide ( const int _pos , const char t )
    {
    int sl = sequence.length() ;
    if ( isLinear() && ( _pos < 0 || _pos >= sl ) ) return ;

    int pos=_pos;
    while ( pos < 0 ) pos += sl ;
    while ( pos >= sequence.length() ) pos -= sl ;
//    insert_char ( t , pos+1 , true ) ;
    sequence.SetChar(pos,t);
    }

bool TVector::basematch ( const char b1 , const char b2 ) const // b1 in IUPAC, b2 in SIUPAC
   {
   return b1 == b2 || ( ( IUPAC[b1] & SIUPAC[b2] ) > 0 ) ;
   }
   
wxString TVector::vary_base ( const char b ) const
    {
    wxString ret ;
    if ( basematch ( 'A' , b ) ) ret += 'A' ;
    if ( basematch ( 'C' , b ) ) ret += 'C' ;
    if ( basematch ( 'G' , b ) ) ret += 'G' ;
    if ( basematch ( 'T' , b ) ) ret += 'T' ;
    return ret ;
    }

const wxString TVector::one2three ( const int a ) const
    {
    if ( a < 0 || a > 255 ) return _T("") ;
    return aaprop[a].tla ;
    }
    
void TVector::setStickyEnd ( const bool left , const bool upper , const wxString& s )
    {
    if ( left && upper ) _lu = s ;
    else if ( left && !upper ) _ll = s ;
    else if ( !left && upper ) _ru = s ;
    else if ( !left && !upper ) _rl = s ;
    }

wxString TVector::getStickyEnd ( bool left , bool upper )
    {
    if ( left && upper ) return _lu ;
    else if ( left && !upper ) return _ll ;
    else if ( !left && upper ) return _ru ;
    else /*if ( !left && !upper ) */ return _rl ;
    }


TVector::TVector ( ChildBase *win )
    {
    enzyme_rules = NULL ;
    changed = true ;
    turned = 0 ;
    type = TYPE_VECTOR ;
    genomeMode = false ;
    circular = false ;
    window = win ;
    undo.setbase ( this ) ;
    init () ;
    }

TVector::~TVector ()
    {
    undo.setbase ( NULL ) ;
    undo.clear () ;
    }
    
void TVector::setCodonTable ( const int table , const wxString& sequence , const wxString& name )
    {
    while ( codon_tables.GetCount() <= table ) codon_tables.Add ( _T("") ) ;
    while ( codon_table_names.GetCount() <= table ) codon_table_names.Add ( _T("") ) ;
    codon_tables[table] = sequence ;
    codon_table_names[table] = name ;
    }

void TVector::init ()
    {
    // Setting DNA => AA matrix
    // Can be adapted for other organisms

//    aa = "FFLLSSSSYY||CC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG" ;

    // Static initialization
    if ( aaprop.size() > 0 )
        {
        aa = codon_tables[1] ;
        return ;
        }

    int a ;

    // See http://www.ncbi.nlm.nih.gov/Taxonomy/Utils/wprintgc.cgi?mode=c
    setCodonTable (  1 , _T("FFLLSSSSYY||CC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Standard") ) ;
    setCodonTable (  2 , _T("FFLLSSSSYY||CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSS||VVVVAAAADDEEGGGG") , _T("Vertebrate mitochondrial") ) ;
    setCodonTable (  3 , _T("FFLLSSSSYY||CCWWTTTTPPPPHHQQRRRRIIMMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Yeast mitochondrial") ) ;
    setCodonTable (  4 , _T("FFLLSSSSYY||CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Mold,Protozoan,Coelenterate Mitochondrial,Myco-/Spiroplasma") ) ;
    setCodonTable (  5 , _T("FFLLSSSSYY||CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSSSSVVVVAAAADDEEGGGG") , _T("Invertebrate mitochondrial") ) ;
    setCodonTable (  6 , _T("FFLLSSSSYYQQCC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Ciliate, Dasycladacean and Hexamita nuclear") ) ;
    setCodonTable (  9 , _T("FFLLSSSSYY||CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNNKSSSSVVVVAAAADDEEGGGG") , _T("Echinoderm mitochondrial") ) ;
    setCodonTable ( 10 , _T("FFLLSSSSYY||CCCWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Euplotid nuclear") ) ;
    setCodonTable ( 11 , _T("FFLLSSSSYY||CC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Bacterial") ) ;
    setCodonTable ( 12 , _T("FFLLSSSSYY||CC|WLLLSPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Alternative Yeast nuclear") ) ;
    setCodonTable ( 13 , _T("FFLLSSSSYY||CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSSGGVVVVAAAADDEEGGGG") , _T("Ascidian mitochondrial") ) ;
    setCodonTable ( 14 , _T("FFLLSSSSYYY|CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNNKSSSSVVVVAAAADDEEGGGG") , _T("Flatworm mitochondrial") ) ;
    setCodonTable ( 15 , _T("FFLLSSSSYY|QCC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Blepharisma nuclear") ) ;
    setCodonTable ( 16 , _T("FFLLSSSSYY|LCC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Chlorophycean mitochondrial") ) ;
    setCodonTable ( 21 , _T("FFLLSSSSYY||CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNNKSSSSVVVVAAAADDEEGGGG") , _T("Trematode mitochondrial") ) ;
    setCodonTable ( 22 , _T("FFLLSS|SYY|LCC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Scenedesmus obliquus mitochondrial") ) ;
    setCodonTable ( 23 , _T("FF|LSSSSYY||CC|WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG") , _T("Thraustochytrium mitochondrial") ) ;
    for ( a = 0 ; a < codon_tables.GetCount() ; a++ ) // Setting all empty tables to standard one
        {
        if ( codon_tables[a] == _T("") )
              {
            codon_tables[a] = codon_tables[1] ;
            codon_table_names[a] = codon_table_names[1] ;
            }
        }
    aa = codon_tables[1] ;

    // IUPAC DNA
    for ( a = 0 ; a < 256 ; a++ ) IUPAC[a] = 0 ;
    setIUPAC ( 'A' , _T("A") ) ;
    setIUPAC ( 'C' , _T("C") ) ;
    setIUPAC ( 'T' , _T("T") ) ;
    setIUPAC ( 'G' , _T("G") ) ;
    for ( a = 0 ; a < 256 ; a++ ) SIUPAC[a] = IUPAC[a] ;
    setIUPAC ( 'R' , _T("AG") , SIUPAC ) ;
    setIUPAC ( 'Y' , _T("TC") , SIUPAC ) ;
    setIUPAC ( 'M' , _T("AC") , SIUPAC ) ;
    setIUPAC ( 'K' , _T("GT") , SIUPAC ) ;
    setIUPAC ( 'S' , _T("GC") , SIUPAC ) ;
    setIUPAC ( 'W' , _T("AT") , SIUPAC ) ;
    setIUPAC ( 'H' , _T("ACT") , SIUPAC ) ;
    setIUPAC ( 'B' , _T("GTC") , SIUPAC ) ;
    setIUPAC ( 'V' , _T("GCA") , SIUPAC ) ;
    setIUPAC ( 'D' , _T("GAT") , SIUPAC ) ;
    setIUPAC ( 'N' , _T("AGCT") , SIUPAC ) ;

    for ( a = 0 ; a < 256 ; a++ ) ACGT[a] = ' ' ;
    ACGT['t'] = ACGT['T'] = 0 ;
    ACGT['u'] = ACGT['U'] = 0 ;
    ACGT['c'] = ACGT['C'] = 1 ;
    ACGT['a'] = ACGT['A'] = 2 ;
    ACGT['g'] = ACGT['G'] = 3 ;

    // DNA complement
    for ( a = 0 ; a < 256 ; a++ ) COMPLEMENT[a] = ' ' ;
    COMPLEMENT['A'] = 'T' ;
    COMPLEMENT['T'] = 'A' ;
    COMPLEMENT['U'] = 'A' ;
    COMPLEMENT['C'] = 'G' ;
    COMPLEMENT['G'] = 'C' ;
    COMPLEMENT['M'] = 'K' ;
    COMPLEMENT['R'] = 'Y' ;
    COMPLEMENT['W'] = 'W' ;
    COMPLEMENT['S'] = 'S' ;
    COMPLEMENT['Y'] = 'R' ;
    COMPLEMENT['K'] = 'M' ;
    COMPLEMENT['V'] = 'B' ;
    COMPLEMENT['H'] = 'D' ;
    COMPLEMENT['D'] = 'H' ;
    COMPLEMENT['B'] = 'V' ;
    COMPLEMENT['X'] = 'N' ;
    COMPLEMENT['N'] = 'N' ;


    // Filling amino acid properties structure
    while ( aaprop.size() < 256 ) aaprop.push_back ( TAAProp() ) ;

    // MW, pI, three-letter-acronym
    aaprop['|'].set_data (   0      ,  0    , _T("STP")) ;
    aaprop['A'].set_data (  71.0788 ,  6.01 , _T("Ala")) ;
    aaprop['C'].set_data ( 103.1388 ,  5.07 , _T("Cys")) ;
    aaprop['D'].set_data ( 115.0886 ,  2.77 , _T("Asp")) ;
    aaprop['E'].set_data ( 129.1155 ,  3.22 , _T("Glu")) ;
    aaprop['F'].set_data ( 147.1766 ,  5.48 , _T("Phe")) ;
    aaprop['G'].set_data (  57.0519 ,  5.97 , _T("Gly")) ;
    aaprop['H'].set_data ( 137.1411 ,  7.59 , _T("His")) ;
    aaprop['I'].set_data ( 113.1594 ,  6.02 , _T("Ile")) ;
    aaprop['K'].set_data ( 128.1741 ,  9.74 , _T("Lys")) ;
    aaprop['L'].set_data ( 113.1594 ,  5.98 , _T("Leu")) ;
    aaprop['M'].set_data ( 131.1926 ,  5.74 , _T("Met")) ;
    aaprop['N'].set_data ( 114.1038 ,  5.41 , _T("Asn")) ;
    aaprop['P'].set_data (  97.1167 ,  6.48 , _T("Pro")) ;
    aaprop['Q'].set_data ( 128.1307 ,  5.65 , _T("Gln")) ;
    aaprop['R'].set_data ( 156.1875 , 10.76 , _T("Arg")) ;
    aaprop['S'].set_data (  87.0782 ,  5.68 , _T("Ser")) ;
    aaprop['T'].set_data ( 101.1051 ,  5.87 , _T("Thr")) ;
    aaprop['V'].set_data (  99.1326 ,  5.97 , _T("Val")) ;
    aaprop['W'].set_data ( 186.2132 ,  5.89 , _T("Trp")) ;
    aaprop['Y'].set_data ( 163.1760 ,  5.66 , _T("Tyr")) ;

    // Atoms ( carbon , hydrogen , nitrogen , oxygen , sulfur )
    aaprop['|'].set_atoms (  0 ,  0 , 0 , 0 , 0 ) ;
    aaprop['A'].set_atoms (  3 ,  5 , 1 , 1 , 0 ) ;
    aaprop['C'].set_atoms (  3 ,  5 , 1 , 1 , 1 ) ;
    aaprop['D'].set_atoms (  4 ,  5 , 1 , 3 , 0 ) ;
    aaprop['E'].set_atoms (  5 ,  7 , 1 , 3 , 0 ) ;
    aaprop['F'].set_atoms (  9 ,  9 , 1 , 1 , 0 ) ;
    aaprop['G'].set_atoms (  2 ,  3 , 1 , 1 , 0 ) ;
    aaprop['H'].set_atoms (  6 ,  7 , 3 , 1 , 0 ) ;
    aaprop['I'].set_atoms (  6 , 11 , 1 , 1 , 0 ) ;
    aaprop['K'].set_atoms (  6 , 12 , 2 , 1 , 0 ) ;
    aaprop['L'].set_atoms (  6 , 11 , 1 , 1 , 0 ) ;
    aaprop['M'].set_atoms (  5 ,  9 , 1 , 1 , 1 ) ;
    aaprop['N'].set_atoms (  4 ,  6 , 2 , 2 , 0 ) ;
    aaprop['P'].set_atoms (  5 ,  7 , 1 , 1 , 0 ) ;
    aaprop['Q'].set_atoms (  5 ,  8 , 2 , 2 , 0 ) ;
    aaprop['R'].set_atoms (  6 , 12 , 4 , 1 , 0 ) ;
    aaprop['S'].set_atoms (  3 ,  5 , 1 , 2 , 0 ) ;
    aaprop['T'].set_atoms (  4 ,  7 , 1 , 2 , 0 ) ;
    aaprop['V'].set_atoms (  5 ,  9 , 1 , 1 , 0 ) ;
    aaprop['W'].set_atoms ( 11 , 10 , 2 , 1 , 0 ) ;
    aaprop['Y'].set_atoms (  9 ,  9 , 1 , 2 , 0 ) ;

    // Half-life (in minutes; negative values mean "greater than"; 0=unknown)
    aaprop['|'].set_halflife ( 0 , 0 , 0 ) ;
    aaprop['A'].set_halflife ( 264 , -1200 , -600 ) ;
    aaprop['C'].set_halflife ( 72 , -1200 , -600 ) ;
    aaprop['D'].set_halflife ( 660 , 3 , -600 ) ;
    aaprop['E'].set_halflife ( 60 , 30 , -600 ) ;
    aaprop['F'].set_halflife ( 66 , 3 , 2 ) ;
    aaprop['G'].set_halflife ( 1800 , -1200 , -600 ) ;
    aaprop['H'].set_halflife ( 210 , 10 , -600 ) ;
    aaprop['I'].set_halflife ( 1200 , 30 , -600 ) ;
    aaprop['K'].set_halflife ( 78 , 3 , 2 ) ;
    aaprop['L'].set_halflife ( 330 , 3 , 2 ) ;
    aaprop['M'].set_halflife ( 1800 , -1200 , -600 ) ;
    aaprop['N'].set_halflife ( 84 , 3 , -600 ) ;
    aaprop['P'].set_halflife ( -1200 , -1200 , 0 ) ;
    aaprop['Q'].set_halflife ( 48 , 10 , -600 ) ;
    aaprop['R'].set_halflife ( 60 , 2 , 2 ) ;
    aaprop['S'].set_halflife ( 1140 , -1200 , -600 ) ;
    aaprop['T'].set_halflife ( 432 , -1200 , -600 ) ;
    aaprop['V'].set_halflife ( 6000 , -1200 , -600 ) ;
    aaprop['W'].set_halflife ( 168 , 3 , 2 ) ;
    aaprop['Y'].set_halflife ( 1680 , 10 , 2 ) ;

    // Chou-Fasman algorithm data
    aaprop['A'].set_cf ( 142 ,  83 ,  66 , 0.06  , 0.076 , 0.035 , 0.058 ) ;
    aaprop['C'].set_cf (  70 , 119 , 119 , 0.149 , 0.05  , 0.117 , 0.128 ) ;
    aaprop['D'].set_cf ( 101 ,  54 , 146 , 0.147 , 0.11  , 0.179 , 0.081 ) ;
    aaprop['E'].set_cf ( 151 ,  37 ,  74 , 0.056 , 0.06  , 0.077 , 0.064 ) ;
    aaprop['F'].set_cf ( 113 , 138 ,  60 , 0.059 , 0.041 , 0.065 , 0.065 ) ;
    aaprop['G'].set_cf (  57 ,  75 , 156 , 0.102 , 0.085 , 0.19  , 0.152 ) ;
    aaprop['H'].set_cf ( 100 ,  87 ,  95 , 0.14  , 0.047 , 0.093 , 0.054 ) ;
    aaprop['I'].set_cf ( 108 , 160 ,  47 , 0.043 , 0.034 , 0.013 , 0.056 ) ;
    aaprop['K'].set_cf ( 114 ,  74 , 101 , 0.055 , 0.115 , 0.072 , 0.095 ) ;
    aaprop['L'].set_cf ( 121 , 130 ,  59 , 0.061 , 0.025 , 0.036 , 0.07  ) ;
    aaprop['M'].set_cf ( 145 , 105 ,  60 , 0.068 , 0.082 , 0.014 , 0.055 ) ;
    aaprop['N'].set_cf (  67 ,  89 , 156 , 0.161 , 0.083 , 0.191 , 0.091 ) ;
    aaprop['P'].set_cf (  57 ,  55 , 152 , 0.102 , 0.301 , 0.034 , 0.068 ) ;
    aaprop['Q'].set_cf ( 111 , 110 ,  98 , 0.074 , 0.098 , 0.037 , 0.098 ) ;
    aaprop['R'].set_cf (  98 ,  93 ,  95 , 0.07  , 0.106 , 0.099 , 0.085 ) ;
    aaprop['S'].set_cf (  77 ,  75 , 143 , 0.12  , 0.139 , 0.125 , 0.106 ) ;
    aaprop['T'].set_cf (  83 , 119 ,  96 , 0.086 , 0.108 , 0.065 , 0.079 ) ;
    aaprop['V'].set_cf ( 106 , 170 ,  50 , 0.062 , 0.048 , 0.028 , 0.053 ) ;
    aaprop['W'].set_cf ( 108 , 137 ,  96 , 0.077 , 0.013 , 0.064 , 0.167 ) ;
    aaprop['Y'].set_cf (  69 , 147 , 114 , 0.082 , 0.065 , 0.114 , 0.125 ) ;

    // Hydrophobicity (Kyte-Doolittle, Hopp-Woods)
    aaprop['A'].set_hp (  1.8 , -0.5 ) ;
    aaprop['C'].set_hp (  2.5 , -1.0 ) ;
    aaprop['D'].set_hp ( -3.5 ,  3.0 ) ;
    aaprop['E'].set_hp ( -3.5 ,  3.0 ) ;
    aaprop['F'].set_hp (  2.8 , -2.5 ) ;
    aaprop['G'].set_hp ( -0.4 ,  0.0 ) ;
    aaprop['H'].set_hp ( -3.2 , -0.5 ) ;
    aaprop['I'].set_hp (  4.5 , -1.8 ) ;
    aaprop['K'].set_hp ( -3.9 ,  3.0 ) ;
    aaprop['L'].set_hp (  3.8 , -1.8 ) ;
    aaprop['M'].set_hp (  1.9 , -1.3 ) ;
    aaprop['N'].set_hp ( -3.5 ,  0.2 ) ;
    aaprop['P'].set_hp ( -1.6 ,  0.0 ) ;
    aaprop['Q'].set_hp ( -3.5 ,  0.2 ) ;
    aaprop['R'].set_hp ( -4.5 ,  3.0 ) ;
    aaprop['S'].set_hp ( -0.8 ,  0.3 ) ;
    aaprop['T'].set_hp ( -0.7 , -0.4 ) ;
    aaprop['V'].set_hp (  4.2 , -1.5 ) ;
    aaprop['W'].set_hp ( -0.9 , -3.4 ) ;
    aaprop['Y'].set_hp ( -1.3 , -2.3 ) ;

    for ( a = 'a' ; a <= 'z' ; a++ ) aaprop[a] = aaprop[a-'a'+'A'] ;
    }
    
void TVector::makeAA2DNA ( const wxString& mode ) // not const
    {
    int a , b , c ;
    wxString iu = _T("ACGT") ;
    for ( a = 0 ; a < 256 ; a++ ) AA2DNA[a] = _T("") ;

    if ( mode == _T("") ) // Default, abstract code
        {
        for ( a = 0 ; a < iu.length() ; a++ )
           {
           for ( b = 0 ; b < iu.length() ; b++ )
              {
              for ( c = 0 ; c < iu.length() ; c++ )
                 {
                 wxString codon ;
                 codon += iu.GetChar(a) ;
                 codon += iu.GetChar(b) ;
                 codon += iu.GetChar(c) ;
                 unsigned char z = dna2aa ( codon ) . GetChar ( 0 ) ;
                 AA2DNA[z] = mergeCodons ( codon , AA2DNA[z] ) ;
                 }
              }
           }
        }
    else // Species-specific
        {
        wxString aas = _T("ACDEFGHIKLMNPQRSTVWY") ;

        wxHashString cc , cc_spec ;
        myapp()->init_txt ( _T("Default") , _T("codon_catalog.csv") , &cc , 2 ) ;
        myapp()->init_txt ( mode , _T("codon_catalog.csv") , &cc_spec , 2 ) ;

        for ( a = 0 ; a < aas.length() ; a++ )
            {
             unsigned char c = aas.GetChar(a) ;
             wxString cs = wxString ( aas.GetChar(a) ) ;
             if ( cc_spec.count(cs) > 0 ) AA2DNA[c] = cc_spec[cs] ;
             else AA2DNA[c] = cc[cs] ;
            }
        }
    for ( a = 0 ; a < 256 ; a++ )
        if ( AA2DNA[a].IsEmpty() )
           AA2DNA[a] = _T("NNN") ;
    }

wxString TVector::mergeCodons ( wxString c1 , wxString c2 )
    {
    if ( c1 == _T("") ) c1 = _T("   ") ;
    if ( c2 == _T("") ) c2 = _T("   ") ;
    int a , b ;
    wxString ret ;
    for ( a = 0 ; a < 3 ; a++ )
        {
        char a1 = c1.GetChar ( a ) ;
        char a2 = c2.GetChar ( a ) ;
        char r = 'N' ;
        int u = SIUPAC[a1] | SIUPAC[a2] ;
        for ( b = 0 ; b < 256 ; b++ )
           if ( SIUPAC[b] == u )
              r = b ;
        ret += r ;
        }
    return ret ;
    }

void TVector::removeBlanksFromSequence ()
    {
    sequence.Replace ( _T(" ") , _T("") , true ) ;
    }

void TVector::removeBlanksFromVector ()
    {
    int a ;
    for ( a = 0 ; a < sequence.size() ; a++ )
        {
        if ( sequence.GetChar(a) == ' ' )
           {
           doRemoveNucleotide ( a ) ;
           a-- ;
           }
        }
    }

void TVector::doRemoveNucleotide ( int x )
    {
    if ( sequence.IsEmpty() )
        {
        items.clear () ;
        return ;
        }
    if ( x >= sequence.length() ) return ;
    if ( x < 0 ) return ;

    int a ;
    if ( isCircular () )
        {
        for ( a = 0 ; a < items.size() ; a++ )
           {
           if ( items[a].from > items[a].to ) items[a].to += sequence.length() ;
           }
        }

    for ( a = 0 ; a < items.size() ; a++ )
        {
        if ( items[a].from <= x+1 && items[a].to >= x+1 )
           {
           items[a].to-- ;
           if ( items[a].to < items[a].from )
              {
              items.erase ( items.begin()+a ) ;
              a-- ;
              }
           }
        else if ( items[a].from > x+1 )
           {
           items[a].from-- ;
           items[a].to-- ;
           }
        }

    sequence.erase ( x , 1 ) ;

    if ( isCircular () )
        {
        for ( a = 0 ; a < items.size() ; a++ )
           {
           if ( items[a].to > sequence.length() ) items[a].to -= sequence.length() ;
           }
        }
    }

void TVector::insert_char ( char x , int pos , bool overwrite )
    {
    wxString dummy ;
    dummy = (wxChar) x ;
    if ( !overwrite || sequence.GetChar(pos-1) != x ) prepareFeatureEdit ( pos , overwrite ) ;
    if ( overwrite && pos < sequence.length() )
       {
       myass ( pos-1 >= 0 && pos-1 < sequence.length() , "TVector::insert_char_1" ) ;
       sequence.SetChar(pos-1,x) ;
       return ;
       }
    sequence.insert(pos-1 , dummy ) ;

    int a ;
    for ( a = 0 ; a < items.size() ; a++ )
        {
        if ( items[a].from >= pos ) items[a].from++ ;
        if ( items[a].to >= pos ) items[a].to++ ;
        }
    }

void TVector::setIUPAC ( char b , wxString s , char *pac )
    {
    int x = 0 ;
    for ( int y = 0 ; y < s.Length() ; y++ )
        {
          char c = s.GetChar(y) ;
        if ( c == 'A' || c == 'a' ) x += IUPAC_A ;
        if ( c == 'C' || c == 'c' ) x += IUPAC_C ;
        if ( c == 'G' || c == 'g' ) x += IUPAC_G ;
        if ( c == 'T' || c == 't' ) x += IUPAC_T ;
        }
    if ( !pac ) pac = IUPAC ;
    pac[(unsigned char)b] = x ;
    }


void TVector::getCuts ( TRestrictionEnzyme *e , vector <TRestrictionCut> &ret ,
                        bool clear_vector , int max )
    {
    int b , c ;
    if ( clear_vector ) ret.clear () ;
    if ( ret.empty() ) ret.reserve ( 5 ) ; // Speedup, little waste
    wxString rs = e->getSequence() ;
    wxString t = sequence ;
    if ( circular ) t += sequence ;
    unsigned int sequence_length = sequence.length() ;
    unsigned int t_length = t.length() ;
    unsigned int rs_length = rs.length() ;

    for ( b = 0 ; b < sequence_length ; b++ )
        {
        for ( c = 0 ; b+c < t_length && c < rs_length &&
                        basematch ( t.GetChar(b+c) , rs.GetChar(c) ) ; c++ ) ;
        if ( c == rs_length )
           {
           int thecut = (b+e->getCut()) % sequence_length ;
           if ( thecut >= 0 && thecut < sequence_length )
                 {
              ret.push_back ( TRestrictionCut ( thecut , e ) ) ;
              if ( ret.size() > max ) return ;
              }
           }
        }

    if ( e->isPalindromic() ) return ;

    // Complement & invert for non-palindromic sequences
    TVector dummy_vector ;
    dummy_vector.sequence = rs ;
    rs = dummy_vector.transformSequence ( true , true ) ;

    for ( b = 0 ; b < sequence_length ; b++ )
        {
        for ( c = 0 ; b+c < t_length && c < rs_length &&
                        basematch ( t.GetChar(b+c) , rs.GetChar(c) ) ; c++ ) ;
        if ( c == rs_length )
           {
           int thecut = (b+e->getCut()) % sequence_length ;
           if ( thecut >= 0 && thecut < sequence_length )
                 {
              ret.push_back ( TRestrictionCut ( thecut , e , false ) ) ;
              if ( ret.size() > max ) return ;
              }
           }
        }

    }

void TVector::recalculateCuts ()
    {
    rc.clear () ;
    if ( type == TYPE_AMINO_ACIDS ) return ;
    if ( sequence == _T("") ) return ;
    bool truncate = false ;
    if ( sequence.GetChar(sequence.length()-1) == ' ' )
        {
        eraseSequence ( getSequenceLength()-1 , 1 ) ;
        truncate = true ;
        }
    bool join = getVectorCuts ( this ) ;
    if ( action == _T("RESTRICTION") ) join = false ;
    if ( truncate ) sequence += _T(" ") ;

    // Sorting by position
    sort ( rc.begin() , rc.end() ) ;

    // Join doubles
    if ( join )
        {
        int a , b ;
        for ( a = 0 ; a < rc.size() ; a++ )
            {
            for ( b = a+1 ; b < rc.size() && rc[b].getPos() == rc[a].getPos() ; b++ )
                {
                if ( b == rc.size() ) continue ;
                if ( rc[b].getPos() != rc[a].getPos() ) continue ;
                if ( !rc[a].join ( &rc[b] ) ) continue ;
                rc.erase ( rc.begin() + b ) ;
                a-- ;
                break ;
                }
               }
        }

    int maxcuts = sequence.length() * MAXCUTS_PER_1K / 1000 ;
    if ( maxcuts < 50 ) maxcuts = 50 ; // Arbitary number
    if ( rc.size() > maxcuts )
    {
        setParam ( _T("toomanycuts") , _T("1") ) ;
        if ( getParam ( _T("toomanycuts_warning") ) == _T("") )
        {
            if ( !myapp()->frame->isLocked() )
            {
                wxMessageBox ( wxString::Format ( txt("t_too_many_cuts") , rc.size() , maxcuts ) ) ;
                setParam ( _T("toomanycuts_warning") , _T("1") ) ;
            }
        }
//        while ( rc.size() > maxcuts ) rc.pop_back () ;
    }

    // Methylation
    methylationSites ( methyl , getEnzymeRule()->methylation ) ;
    }

// Returns wether or not "equal" enzymes should be joined
bool TVector::getVectorCuts ( TVector *v )
    {
    TEnzymeRules *er = getEnzymeRule () ;
    er->getVectorCuts ( this ) ;
    return er->join_enzymes ;
    }

// Gets the enzyme rules to follow
TEnzymeRules *TVector::getEnzymeRule () const
    {
    TEnzymeRules *er ;
    if ( enzyme_rules ) // Vector settings
         {
         er = enzyme_rules ;
         if ( er->useit ) return er ;
         }

    if ( myapp()->frame->project.getEnzymeRules() ) // Project settings
        {
         er = myapp()->frame->project.getEnzymeRules() ;
         if ( er->useit ) return er ;
         }


    er = myapp()->frame->global_enzyme_rules ; // Global settings
    return er ;
    }

// ******** ACTION!

#define MIN(_i,_o) (((_i)<(_o)?(_i):(_o)))
#define MAX(_i,_o) (((_i)>(_o)?(_i):(_o)))

// This function reduces the sequence of the vector and the associated objects
// to the fragment between two cuts of restriction enzymes
bool TVector::reduceToFragment ( TRestrictionCut left , TRestrictionCut right )
    {
    int a , from , to ;
    wxString s ;

    from = left.getPos() ;
    to = right.getPos() ;
    if ( to <= from ) to += sequence.length() ;

    from -= left.getCut() ;
    to += right.getSequence().length() - right.getCut() - 1 ;

    int from2 = from + left.getSequence().length() ;
    int to2 = to - right.getSequence().length() + 1 ;

    for ( a = from ; a <= to ; a++ )
        s += getNucleotide ( a ) ;

    wxString mlu , mll , mru , mrl ;

    if ( left.getSequence().IsEmpty() ) // Blank enzyme
        {
        mlu = getStickyEnd ( true , true ) ;
        mll = getStickyEnd ( true , false ) ;
        }

    if ( right.getSequence().IsEmpty() ) // Blank enzyme
        {
        mru = getStickyEnd ( false , true ) ;
        mrl = getStickyEnd ( false , false ) ;
        }

     _lu = left.getEndUpperRight () ;
     _ll = left.getEndLowerRight () ;
    _ru = right.getEndUpperLeft () ;
    _rl = right.getEndLowerLeft () ;

    // How many overlapping nucleotides are on both strands on either side?
    int lo = MIN ( _lu.length() , _ll.length() ) ;
    int ro = MIN ( _ru.length() , _rl.length() ) ;

    // Cutting sticky ends
    TVector dv ;
    dv.setSequence ( s ) ;
    wxString is = dv.transformSequence ( true , false ) ; // Inverse sequence
    _lu =  s.substr ( left.getSequence().length() - _lu.length() , _lu.length() - lo ) ;
    _ll = is.substr ( left.getSequence().length() - _ll.length() , _ll.length() - lo ) ;
    _ru =  s.substr ( s.length() - right.getSequence().length() + ro , _ru.length() - ro ) ;
    _rl = is.substr ( s.length() - right.getSequence().length() + ro , _rl.length() - ro ) ;

    // Removing sticky ends from main sequence
    s = s.substr ( 0 , s.length() - right.getSequence().length() + ro ) ;
    s = s.substr ( left.getSequence().length() - lo ) ;

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

    // Fix old sticky ends, which otherwise get deleted
    if ( mlu + mll != _T("") )
        {
        setStickyEnd ( true , true , mlu ) ;
        setStickyEnd ( true , false , mll ) ;
        }
    if ( mru + mrl != _T("") )
        {
        setStickyEnd ( false , true , mru ) ;
        setStickyEnd ( false , false , mrl ) ;
        }

    return true ; // Success; actually, no "return false" yet.
    }
    
char TVector::getNucleotide ( const int _pos ,  const bool complement ) const
    {
    int sl = sequence.length() ;
    if ( isLinear() && ( _pos < 0 || _pos >= sl ) ) return ' ' ;
    int pos = _pos;
    while ( pos < 0 ) pos += sl ;
    while ( pos >= sl ) pos -= sl ;
    if ( complement ) return getComplement ( sequence.GetChar(pos) ) ;
    return sequence.GetChar(pos) ;
    }
    
wxString TVector::transformSequence ( const bool inverse , const bool reverse ) const
    {
    int a ;
    wxString r = sequence ;
    if ( inverse )
        {
        for ( a = 0 ; a < r.length() ; a++ )
           r.SetChar ( a , getComplement ( r.GetChar(a) ) ) ;
        }
    if ( reverse )
        {
        wxString s = r ;
        for ( a = 0 ; a < r.length() ; a++ )
           r.SetChar ( a , s.GetChar(s.length()-a-1) ) ;
        }
    return r ;
    }

void TVector::doRestriction ()
    {
    vector <TRestrictionCut> cl ;
    int a , b ;

    mylog ( "TVector::doRestriction" , "1" ) ;
    if ( cocktail.GetCount() == 0 ) return ;
    mylog ( "TVector::doRestriction" , "2" ) ;
    myass ( myapp() , "Oh no! No application defined!" ) ;
    myapp()->frame->lastCocktail = cocktail ;

    TRestrictionEnzyme blankEnzyme ;
    blankEnzyme.setCut ( 0 ) ;
    blankEnzyme.setSequence ( _T("") ) ;
    blankEnzyme.setOverlap ( 0 ) ;
    if ( !circular )
        cl.push_back ( TRestrictionCut ( 0 , &blankEnzyme ) ) ;

    mylog ( "TVector::doRestriction" , "3" ) ;
    for ( a = 0 ; a < cocktail.GetCount() ; a++ )
       {
       for ( b = 0 ; b < re.GetCount() && re[b]->getName() != cocktail[a] ; b++ ) ;
       if ( b == re.GetCount() )
          {
          recalcvisual = true ;
          re.Add ( myapp()->frame->LS->getRestrictionEnzyme ( cocktail[a] ) ) ;
          myass ( re[re.GetCount()-1] , "Oh no! Unknown enzyme!" ) ;
          }
       }
    mylog ( "TVector::doRestriction" , "4" ) ;
    recalculateCuts() ;
    mylog ( "TVector::doRestriction" , "5" ) ;

    // Collecting restriction cuts
    for ( a = 0 ; a < cocktail.GetCount() ; a++ )
        {
        for ( b = 0 ; b < rc.size() ; b++ )
            {
            if ( rc[b].e->getName() == cocktail[a] )
                {
                cl.push_back ( rc[b] ) ;
                }
            }
        }
    mylog ( "TVector::doRestriction" , "6" ) ;

    // Arranging
    for ( a = 1 ; a < cl.size() ; a++ )
        {
        if ( cl[a-1].getPos() > cl[a].getPos() )
           {
           TRestrictionCut ct = cl[a] ;
           cl[a] = cl[a-1] ;
           cl[a-1] = ct ;
           a = 0 ;
           }
        }
    mylog ( "TVector::doRestriction" , "7" ) ;

    if ( circular )
        cl.push_back ( cl[0] ) ;
    else
        cl.push_back ( TRestrictionCut ( sequence.length()-1 , &blankEnzyme ) ) ;

        myapp()->frame->lockDisplay ( true ) ;

    mylog ( "TVector::doRestriction" , "8" ) ;
    for ( a = 0 ; a+1 < cl.size() ; a++ )
        {
        wxString t1 = wxString::Format ( _T("%d") , cl[a].getPos() ) ;
        wxString t2 = wxString::Format ( _T("%d") , cl[a+1].getPos() ) ;
        TVector *nv = new TVector ;
        nv->setFromVector ( *this ) ;
        nv->reduceToFragment ( cl[a] , cl[a+1] ) ;
        nv->recalculateCuts () ;
        nv->recalcvisual = true ;
        if ( !nv->getDescription().IsEmpty() ) nv->addDescription ( _T("\n") ) ;
          wxString tx = wxString::Format ( txt("res_desc").c_str() , nv->getName().c_str() , cl[a].e->getName().c_str() , t1.c_str() ) ;
        nv->addDescription ( tx ) ;
        if ( cl[a].e->getName() == cl[a+1].e->getName() )
           {
           nv->addDescription ( _T(".") ) ;
           nv->addName ( _T(" (") + cl[a].e->getName() + _T(")") ) ;
           }
        else
           {
           tx = wxString::Format ( txt("res_desc2").c_str() , cl[a+1].e->getName().c_str() , t2.c_str() ) ;
           nv->addName ( _T(" (") + cl[a].e->getName() + _T("/") + cl[a+1].e->getName() +_T(")") ) ;
           }
        nv->cocktail.Clear() ; // Cleaning up cocktail
        if ( nv->sequence.length() >= action_value ) // "No fragments smaller than XX base pairs"
           myapp()->frame->newFromVector ( nv , TYPE_FRAGMENT ) ;
        else delete nv ;
        }
        myapp()->frame->lockDisplay ( false ) ;
    mylog ( "TVector::doRestriction" , "9" ) ;
    cocktail.Clear() ;
    mylog ( "TVector::doRestriction" , "10" ) ;
    updateDisplay ( true ) ;
    }

void TVector::doAction ()
    {
    if ( action == _T("RESTRICTION") ) doRestriction () ;
    action = _T("") ;
    recalculateCuts () ;
    return ;
    }
    
wxString TVector::invert ( const wxString& s ) const
    {
    wxString t ;
    for ( int a = 0 ; a < s.length() ; a++ ) t = s.GetChar(a) + t ;
    return t ;
    }

// This function can attach a linear vector at the "right" end.
// It does *NOT* test if the action is valid!
void TVector::ligate_right ( TVector &v , bool inverted )
    {
    if ( circular ) return ;
    if ( v.circular ) return ;
    int a , b ;

    // Is the ligand (to the right!) inverted?
    if ( inverted )
        {
        v.items.clear () ; // Inversion will delete all features (for now)!
        wxString ll = v._ll ;
        wxString lu = v._lu ;
        wxString rl = v._rl ;
        wxString ru = v._ru ;
        v._ll = invert ( ru ) ;
        v._lu = invert ( rl ) ;
        v._rl = invert ( lu ) ;
        v._ru = invert ( ll ) ;
        v.sequence = v.transformSequence ( true , true ) ;
        }

    // Merging sequence
    wxString ol = _ru + v._lu ;
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
           if ( items[b].name == i.name && items[b].getType() == i.getType() )
              {
              b = -1 ;
              i.name += _T("*") ;
              }
           }
        items.push_back ( i ) ;
        }

    // Merging restriction enzymes
    for ( a = 0 ; a < v.re.GetCount() ; a++ )
        {
        if ( wxNOT_FOUND == re.Index ( v.re[a] ) )
           re.Add ( v.re[a] ) ;
        }

    }

// This function can close the circle after ligation.
// It does *NOT* test if the action is valid!
void TVector::closeCircle ()
    {
    if ( circular ) return ;
    sequence += _ru ;
    sequence += _lu ;
    _ru = _lu = _rl = _ll = _T("") ;
    circular = true ;
    recalcvisual = true ;
    if ( turned != 0 ) turn ( -turned ) ;
    }

// This function is a plague
void TVector::doRemove ( int from , int to , bool update , bool enableUndo )
    {
    if ( from == to + 1 ) return ; // Nothing to remove
    int a ;
    int l = sequence.length() ;
    if ( enableUndo ) undo.start ( txt("u_del_seq") ) ;

    // Sequence
    to %= l ;
    if ( to >= from ) sequence.Remove ( from - 1 , to - from + 1 ) ;
    else sequence = sequence.Mid ( to , from - to - 1 ) ;

/*
    // Old code
    wxString s = sequence , t ;
    int rt = to ;
    if ( rt < from ) rt += l ;
    for ( a = from ; a <= rt ; a++ )
       {
       b = a ;
       if ( b > l ) b -= l ;
       t += s.GetChar(b-1) ;
       s.SetChar(b-1,' ') ;
       }
    sequence = _T("") ;
    for ( a = 0 ; a < l ; a++ )
       if ( s.GetChar(a) != ' ' ) sequence += s.GetChar(a) ;
*/


    // Items
    for ( a = 0 ; a < items.size() ; a++ )
       {
       items[a].doRemove ( from , to , l ) ;
       if ( items[a].from == -1 )
          {
          items.erase ( items.begin() + a ) ;
          a-- ;
          }
/*       else if ( items[a].to > sequence.length() )
          {
          if ( isLinear() ) items[a].to = sequence.length() ;
          else items[a].to -= sequence.length() ;
          }    */
       }

    // Finish
    if ( update )
        {
        if ( enableUndo ) undo.stop () ;
        recalculateCuts () ;
        recalcvisual = true ;
        }
    else if ( enableUndo ) undo.abort () ;
    }

wxString TVector::get_translation_table ( const int translation_table ) const
    {
    wxString ret ;
    if ( myapp()->frame->nonstandard_translation_table != -1 ) // Forced translation table
        ret = codon_tables[myapp()->frame->nonstandard_translation_table] ;
    else if ( translation_table == -1 ) ret = aa ;
    else ret = codon_tables[translation_table] ;
    return ret ;
    }

// Currently returns only the direct encoding (a single char) or 'X'
// Could return all possible AAs (see IUPAC) in the future
wxString TVector::dna2aa ( const wxString& codon , const int translation_table ) const
    {
    if ( codon.length() != 3 ) return _T("?") ;
    wxString r;
    const wxString aa = get_translation_table ( translation_table ) ;
    char c0 = codon.GetChar(0) ;
    char c1 = codon.GetChar(1) ;
    char c2 = codon.GetChar(2) ;
    if ( c0 == ' ' ) return _T("?") ;
    if ( c1 == ' ' ) return _T("?") ;
    if ( c2 == ' ' ) return _T("?") ;
    if ( ACGT[c0] != ' ' && ACGT[c1] != ' ' && ACGT[c2] != ' ' )
       {
       int i = ACGT[c0]*16 +
               ACGT[c1]*4 +
               ACGT[c2] ;
       if ( i > 63 ) r = _T("?") ;
       else r = aa.GetChar(i) ;
       }
    else
       {
       char u = ' ' ;
       wxString s0 = vary_base ( c0 ) ;
       wxString s1 = vary_base ( c1 ) ;
       wxString s2 = vary_base ( c2 ) ;
       for ( int a0 = 0 ; a0 < s0.length() ; a0++ )
        for ( int a1 = 0 ; a1 < s1.length() ; a1++ )
         for ( int a2 = 0 ; a2 < s2.length() ; a2++ )
          {
          char v ;
          int i = ACGT[s0.GetChar(a0)]*16 +
                  ACGT[s1.GetChar(a1)]*4 +
                  ACGT[s2.GetChar(a2)] ;
          if ( i > 63 ) v = '?' ;
          else v = aa.GetChar(i) ;
          if ( u != ' ' && u != v ) return _T("?") ;
          else if ( v == '?' ) return _T("?") ;
          else u = v ;
          }
       if ( u == ' ' ) r = _T("?") ;
       else r = (wxChar) u ;
       }
    return r ;
    }
    
void TVector::turn ( const int off )
    {
    // Allowing turn of linear fragments as a temporary measure due to a problem in getAAvector
    //if ( !circular ) return ;
    if ( off == 0 ) return ;
    int a ;

    // Items
    for ( a = 0 ; a < items.size() ; a++ )
        {
        items[a].from += off ;
        items[a].to += off ;

        while ( items[a].from < 1 ) items[a].from += sequence.length() ;
        while ( items[a].from > sequence.length() ) items[a].from -= sequence.length() ;

        while ( items[a].to < 1 ) items[a].to += sequence.length() ;
        while ( items[a].to > sequence.length() ) items[a].to -= sequence.length() ;
        }

    // Sequence
    if ( off > 0 )
        {
        const int off_diff = sequence.length() - off ;
        sequence = sequence.substr ( off_diff ) + sequence.substr ( 0 , off_diff ) ;
        }
    else
        {
        sequence = sequence.substr ( -off ) + sequence.substr ( 0 , -off ) ;
        }

    // Cleanup
    recalculateCuts () ;
    recalcvisual = true ;
    turned += off ;
    }
    
int TVector::countCuts ( const wxString& enzyme ) const
    {
    int a , count ;
    for ( a = count = 0 ; a < rc.size() ; a++ )
        if ( rc[a].e->getName() == enzyme )
           count++ ;
    return count ;
    }
        
void TVector::setAction ( const wxString& _action , const int _action_value )
    {
    action = _action ;
    action_value = _action_value ;
    }

wxString TVector::getSubstring ( int mf , int mt )
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
        wxString s ;
        s += sequence.substr ( 0 , mt-l+1 ) ;
        s += sequence.substr ( mf ) ;
        return s ;
        }
    }
    
void TVector::setChanged ( const bool c )
    {
    if ( c == changed ) return ;
    changed = c ;
    if ( window ) window->showName () ;
    }

void TVector::ClearORFs ()
    {
    worf.clear() ;
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
        wxString codon ;
        codon += getNucleotide ( a + dir * 0 , complement ) ;
        codon += getNucleotide ( a + dir * 1 , complement ) ;
        codon += getNucleotide ( a + dir * 2 , complement ) ;
        if ( codon == _T("ATG") )
           {
           int cnt = (sequence.length()+2)/3 ;
           int aa = 0 ;
           for ( b = a ; cnt > 0 ; b += dir * 3 )
              {
              cnt-- ;
              aa++ ;
              if ( b < 0 ) b = sequence.length() - b ;
              if ( b >= sequence.length() ) b -= sequence.length() ;
              codon = _T("") ;
              codon += getNucleotide ( b + dir * 0 , complement ) ;
              codon += getNucleotide ( b + dir * 1 , complement ) ;
              codon += getNucleotide ( b + dir * 2 , complement ) ;
              if ( codon == _T("TAA") ||
                   codon == _T("TAG") ||
                   codon == _T("TGA") )
                 {
                 if ( aa > myapp()->frame->orfLength )
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
    
TVector *TVector::getAAvector ( const int _from , const int _to , const int dir ) const
    {
    char UNIQUE = '#' ;
    // Creating new vector
    TVector *v = new TVector ;
    v->setFromVector ( *this ) ;
    v->setType ( TYPE_AMINO_ACIDS ) ;

    // FIXME: Should be const but is not, maybe be more restrictive with from<to ?
    int to = _to;
    int from = _from;

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
    mylog ( "TVector::getAAvector" , "1" ) ;
    v->sequence = aas.s ;
    for ( a = to+1 ; a < v->sequence.length() ; a++ ) v->sequence.SetChar(a,UNIQUE) ;
    for ( a = 0 ; a < v->sequence.length() ; a++ )
        if ( v->sequence.GetChar(a) == ' ' || v->sequence.GetChar(a) == '|' )
           v->sequence.SetChar(a,UNIQUE) ;


    mylog ( "TVector::getAAvector" , "2" ) ;
    // Cleaning restriction enzymes
    v->re.Clear() ;
    v->rc.clear() ;
    mylog ( "TVector::getAAvector" , "3" ) ;

//    wxStopWatch sw ;
//    sw.Start() ;
    // Removing non-AA sequence and features
    for ( a = 0 ; a < v->sequence.length() ; a++ )
        {
        if ( v->sequence.GetChar(a) == UNIQUE )
           {
           for ( b = a+1 ; b < v->sequence.length() && v->sequence.GetChar(b) == UNIQUE ; b++ ) ;
           v->doRemove ( a+1 , b , false , false ) ;
           a-- ;
           }
        }
    mylog ( "TVector::getAAvector" , "4" ) ;

//    sw.Pause() ;

    // Fixing features, if neccessary
    // PATCH AS PATCH CAN!!!
    if ( dir < 0 )
        {
        wxString t ;
        for ( a = 0 ; a < v->sequence.length() ; a++ ) t = v->sequence.GetChar(a) + t ;
        v->sequence = t ;
        for ( a = 0 ; a < v->items.size() ; a++ )
           {
           int f = v->sequence.length() - v->items[a].to + 1 ;
           v->items[a].to = v->sequence.length() - v->items[a].from + 1 ;
           v->items[a].from = f ;
           }
        }

    v->circular = false ;
//    wxMessageBox ( wxString::Format ( "%d ms" , sw.Time() ) ) ;
    mylog ( "TVector::getAAvector" , "5" ) ;
    return v ;
    }

void TVector::callUpdateUndoMenu ()
    {
    if ( !window ) return ;
    window->updateUndoMenu() ;
    }

void TVector::setFromVector ( /*const*/ TVector /*&*/ v ) /* not const */
    {
    *this = v ; // maybe needs a copy operator to allow const
    for ( int a = 0 ; a < items.size() ; a++ ) items[a].setLastVector ( this ) ;
    undo.clear () ;
    }

int TVector::getItemLength ( int a )
    {
    if ( items[a].to >= items[a].from ) return items[a].to - items[a].from + 1 ;
    return items[a].to + sequence.length() - items[a].from + 1 ;
    }

void TVector::clear ()
    {
    _lu = _ll = _ru = _rl = _T("") ;
    circular = false ;
    paramk.Clear () ;
    paramv.Clear () ;
    methyl.Clear () ;
    if ( enzyme_rules ) { delete enzyme_rules ; enzyme_rules = NULL ; }
    turned = 0 ;
    action = _T("") ;
    action_value = 0 ;
    aa = database = _T("") ;
    changed = false ;
    undo.clear() ;
    sequence = name = desc = _T("") ;
    type = TYPE_VECTOR ;
    recalcvisual = false ;
    items.clear () ;
    re.Clear () ;
    rc.clear () ;
    cocktail.Clear () ;
    worf.clear () ;
    hiddenEnzymes.Clear () ;
    proteases.Clear () ;
    }
    
int TVector::find_item ( const wxString& s ) const
    {
    int a ;
    for ( a = 0 ; a < items.size() ; a++ )
       if ( items[a].name == s )
          return a ;
    return -1 ; // Not found
    }

bool TVector::isEnzymeHidden ( const wxString& s ) const
    {
    for ( int a = 0 ; a < hiddenEnzymes.GetCount() ; a++ )
       if ( hiddenEnzymes[a] == s ) return true ;
    return false ;
    }

void TVector::hideEnzyme ( const wxString& s , const bool hideit )
    {
    bool isHidden = isEnzymeHidden ( s ) ;
    if ( hideit && !isHidden ) // Add enzymes to hidden enzyme list
         {
           hiddenEnzymes.Add ( s ) ;
           }
    else if ( !hideit && isHidden ) // Remove enzyme from hidden enzyme list
        {
          hiddenEnzymes.Remove ( s ) ;
        }
    }

void TVector::sortRestrictionSites ()
    {
    int a ;
    for ( a = 1 ; a < rc.size() ; a++ ) // Sort by pos
        {
        if ( rc[a].getPos() < rc[a-1].getPos() )
           {
           TRestrictionCut dummy = rc[a] ;
           rc[a] = rc[a-1] ;
           rc[a-1] = dummy ;
           a = 0 ;
           }
        }
    }
    
const wxString TVector::getDescription () const
    {
    return desc ;
    }    
    
void TVector::setDescription ( const wxString& s )
    {
    desc = s ;
    }
    
void TVector::addDescription ( const wxString& s )
    {
    desc += s ;
    }
    
const wxString TVector::getName () const
    {
    return name ;
    }    
    
void TVector::setName ( const wxString& s )
    {
    name = s ;
    }
    
void TVector::addName ( const wxString& s )
    {
    name += s ;
    }
    
wxString TVector::getSequence () const
    {
    return sequence ;
    }
    
void TVector::removeAlignmentArtifacts ( const char what )
    {
    wxString what2 ( (wxChar) what ) ;
    sequence.Replace ( what2 , _T("") ) ;
    }    
    
char TVector::getSequenceChar ( const int x ) const
    {
    return sequence.GetChar(x) ;
    }    
        
void TVector::setSequence ( const wxString& ns )
    {
    sequence.Alloc ( ns.length() + 10 ) ;
    sequence = ns ;
    }    
    
void TVector::addToSequence ( const wxString& x )
    {
    sequence.Append ( x ) ;
    }    
    
void TVector::alterSequence ( const int pos , const char c )
    {
    sequence.SetChar ( pos , c ) ;
    }    
    
int TVector::getSequenceLength() const
    {
    return sequence.length() ;
    }    
    
void TVector::eraseSequence ( const int from , const int len )
    {
    sequence.erase ( from , len ) ;
    }    
    
char TVector::three2one ( wxString s ) const
    {
    s.MakeUpper() ;
    for ( int a = 0 ; a < 255 ; a++ )
       {
           if ( aaprop[a].tla.Upper() == s )
              return a ;
       }
    return ' ' ;
    }

// "Back-translate" amino acid sequence to DNA
TVector *TVector::backtranslate ( const wxString& mode ) // not const becaues of makeAA2DNA
    {
    TVector *nv = new TVector ;
    nv->setFromVector ( *this ) ;
    nv->type = TYPE_VECTOR ;
    wxString ns ;
    int a ;
    makeAA2DNA ( mode ) ;
    for ( a = 0 ; a < sequence.length() ; a++ )
        {
        unsigned char c = sequence.GetChar ( a ) ;
        ns += AA2DNA[c] ;
        }
    nv->sequence = ns ;
    for ( a = 0 ; a < nv->items.size() ; a++ )
       {
       nv->items[a].from = ( nv->items[a].from - 1 ) * 3 + 1 ;
       nv->items[a].to = ( nv->items[a].to - 1 ) * 3 + 1 ;
       }
    return nv ;
    }

int TVector::getMem () const
    {
    int a , r = 0 ;
    for ( a = 0 ; a < items.size() ; a++ ) r += items[a].getMem() ;
    r += sizeof ( TVector ) ;
    r += sizeof ( TORF ) * worf.size () ;
    r += rc.size() * sizeof ( TRestrictionCut ) ;
    r += re.GetCount() * 4 ; // Pointer
    for ( a = 0 ; a < hiddenEnzymes.GetCount() ; a++ ) r += hiddenEnzymes[a].length() ;
    for ( a = 0 ; a < proteases.GetCount() ; a++ ) r += proteases[a].length() ;
    for ( a = 0 ; a < cocktail.GetCount() ; a++ ) r += cocktail[a].length() ;
    r += sequence.length() + name.length() + desc.length() ;
    // Some minor ones not listed
    return r ;
    }

void TVector::addRestrictionEnzyme ( TRestrictionEnzyme * const e )
    {
    for ( int a = 0 ; a < re.size() ; a++ )
        {
        if ( re[a] == e ) return ;
        }
    re.Add ( e ) ;
    }

// ***************************************************************************************
// TVectorItem
// ***************************************************************************************

TVectorItem::TVectorItem()
    {
    initParams () ;
    }

TVectorItem::TVectorItem ( const wxString& sn , const wxString& n , const int f , const int t , const char ty = VIT_MISC )
    {
    initParams () ;
    name = sn ;
    desc = n ;
    from = f ;
    to = t ;
    type = ty ;
    direction = 1 ;
    r3 = r4 = -1 ;
    }

wxBrush *TVectorItem::getBrush () const
    {
    if ( getParam ( _T("ISDEFAULTBRUSH") ) == _T("1") )
        {
        if ( type == VIT_GENE ) return (wxBrush*) wxBLUE_BRUSH ;
        else if ( type == VIT_CDS  ) return (wxBrush*) wxRED_BRUSH ;
        else if ( type == VIT_REP_ORI ) return (wxBrush*) wxBLACK_BRUSH ;
        else if ( type == VIT_PROMOTER ) return (wxBrush*) wxGREY_BRUSH ;
        else if ( type == VIT_TERMINATOR ) return (wxBrush*) wxMEDIUM_GREY_BRUSH ;
        else if ( type == VIT_MISC ) return (wxBrush*) wxBLACK_BRUSH ;
        return (wxBrush*) wxBLUE_BRUSH ;
        }
    else
        {
        int c1 = atoi ( getParam ( _T("COLOR_RED") ) .mb_str() ) ;
        int c2 = atoi ( getParam ( _T("COLOR_GREEN") ) .mb_str() ) ;
        int c3 = atoi ( getParam ( _T("COLOR_BLUE") ) .mb_str() ) ;
        return MYBRUSH ( wxColour ( c1 , c2 , c3 ) ) ;
        }
    }

wxColour TVectorItem::getFontColor () const
    {
    wxBrush *b = getBrush() ;
    myass ( b , "Brush is NULL!" ) ;
    wxColour c = b->GetColour() ;
    if ( b == wxTRANSPARENT_BRUSH || b == wxWHITE_BRUSH )
        c = wxColour ( 255 , 255 , 255 ) ;
    return c ;
    }
    
bool TVectorItem::isVisible () const
    {
    wxString s = getParam ( _T("ISVISIBLE") ) ;
    return ( s == _T("1") ) ;
    }
    
void TVectorItem::setVisible ( const bool b )
    {
    setParam ( _T("ISVISIBLE") , b ) ;
    }
    
void TVectorItem::setColor ( const wxColour& col )
    {
    setParam ( _T("ISDEFAULTBRUSH") , false ) ;
    setParam ( _T("COLOR_RED") , col.Red() ) ;
    setParam ( _T("COLOR_GREEN") , col.Green() ) ;
    setParam ( _T("COLOR_BLUE") , col.Blue() ) ;
    }

//********************* PARAMS

void TVectorItem::setParam ( const wxString& p , const wxString& v )
    {
    int a = pname.Index ( p ) ;
    if ( a == wxNOT_FOUND )
        {
        pname.Add ( p ) ;
        pvalue.Add ( v ) ;
        }
    else pvalue[a] = v ;
    }
    
void TVectorItem::setParam ( const wxString& p , const int v )
    {
    setParam ( p , wxString::Format ( _T("%d") , v ) ) ;
    }
    
const wxString TVectorItem::getParam ( const wxString& p , const wxString& def ) const
    {
    int a = pname.Index ( p ) ;
    return ( a == wxNOT_FOUND ) ? def : pvalue[a] ;
    }
    
wxArrayString TVectorItem::getParamKeys () const
    {
    return pname ;
    }

wxString TVectorItem::implodeParams ()
    {
    wxString s ;
    int a , b ;
    for ( a = 0 ; a < pname.GetCount() ; a++ )
        {
        wxString t = pvalue[a] ;
        for ( b = 0 ; b < t.length() ; b++ )
           if ( t.GetChar(b) == '\n' )
              t.SetChar ( b , 2 ) ;
        s += pname[a] ;
        s += _T("\n") + t + _T("\n") ;
        }
    return s ;
    }

void TVectorItem::explodeParams ( const wxString& _s )
    {
    wxString s = _s ;
    int a ;
    initParams () ;
    while ( !s.IsEmpty() )
        {
        for ( a = 0 ; s.GetChar(a) != '\n' ; a++ ) ;
        wxString n = s.substr ( 0 , a ) ;
        s = s.substr ( a + 1 ) ;
        for ( a = 0 ; s.GetChar(a) != '\n' ; a++ ) ;
        wxString v = s.substr ( 0 , a ) ;
        for ( a = 0 ; a < v.length() ; a++ )
           if ( v.GetChar(a) == 2 )
              v.SetChar ( a , '\n' ) ;
        s = s.substr ( a + 1 ) ;
        setParam ( n , v ) ;
        }
    if ( getParam ( _T("ISDEFAULTBRUSH") ) != _T("1") )
        {
        setColor ( wxColour ( atoi ( getParam(_T("COLOR_RED")).mb_str() ) ,
                              atoi ( getParam(_T("COLOR_GREEN")).mb_str() ) ,
                              atoi ( getParam(_T("COLOR_BLUE")).mb_str() ) ) ) ;
        }
    }

void TVectorItem::initParams ()
    {
    r1 = r2 = -1 ;
    pname.Clear() ;
    pvalue.Clear() ;
    setParam ( _T("ISDEFAULTBRUSH") , true ) ;
    setVisible ( true ) ;
    lastVector = NULL ;
    }

int TVectorItem::getRF () const
    {
    if ( type != VIT_CDS ) return 0 ;
    long l ;
    getParam ( _T("/codon_start") ).ToLong ( &l ) ;
    return (int) l ;
    }
    
void TVectorItem::setRF ( const int x )
    {
    if ( type != VIT_CDS ) return ;
    setParam ( _T("/codon_start") , x ) ;
    }
    
void TVectorItem::doRemove ( const int f , const int t , const int l )
    {
    if ( from == -1 ) return ;
    int rt = t ;
    int rto = to ;
    if ( t < f ) rt += l ;
    if ( to < from ) rto += l ;
    int a ;
    wxString s ( (wxChar) '_' , l * 3 + 1 ) ;
    for ( a = from ; a <= rto ; a++ ) s.SetChar ( a , 'X' ) ;
    for ( a = f ; a <= rt ; a++ ) s.SetChar ( a , ' ' ) ;

    from = -1 ;
    to = -1 ;
    int ff = 0 ;
    for ( a = 1 ; a < s.length() ; a++ )
       {
       if ( s.GetChar(a) == ' ' ) ff++ ;
       else if ( s.GetChar(a) == 'X' )
          {
          if ( from == -1 ) from = a - ff ;
          to++ ;
          }
       }
    to += from ;

    const int l_mod = l - rt - f + 1 ;
    if ( l_mod == 0 ) 
        {
        from = -1 ;
        return ;
        }
    while ( to > l_mod ) to -= l_mod ;
    while ( from > l_mod ) from -= l_mod ;

    }
    
int TVectorItem::getOffset () const
    {
    return atoi ( getParam ( _T("OFFSET") , _T("-1") ).mb_str() ) ;
    }
    
void TVectorItem::setOffset ( const int o )
    {
    setParam ( _T("OFFSET") , o ) ;
    }
    
void TVectorItem::setType ( const wxString& _s )
    {
    wxString s = _s.Upper() ;
    if ( s == _T("CDS") ) type = VIT_CDS ;
    else if ( s == _T("GENE") ) type = VIT_GENE ;
    else if ( s == _T("PROMOTER") ) type = VIT_PROMOTER ;
    else if ( s == _T("REP_ORIGIN") ) type = VIT_REP_ORI ;
    else if ( s == _T("TERMINATOR") ) type = VIT_TERMINATOR ;
    else if ( s == _T("MISC_FEATURE") ) type = VIT_MISC ;
    else if ( s == _T("PROTEIN_BIND") ) type = VIT_PROT_BIND ;
    else if ( s == _T("ORIT") ) type = VIT_ORI_T ;
    else if ( s == _T("PROTEIN") ) type = VIT_MISC ;
    else if ( s == _T("REGION") ) type = VIT_MISC ;
    if ( type != VIT_MISC && name == _T("") )
        {
        wxString t = wxString::Format ( _T("itemtype%d") , type ) ;
        t = txt ( t ) ;
        name = t ;
        desc = s + _T("\n") + desc ;
        }
    }


void TVectorItem::translate ( TVector * const v , SeqAA * const aa , vector <Tdna2aa> &dna2aa ) // not const
    {
    lastVector = v ; // -> not const
//   dna2aa.clear () ;
    if ( type != VIT_CDS ) return ;
    if ( getRF() == 0 ) return ;

    char c = ' ' ;
    bool complement , roundOnce = false ;
    int b , coff , rf = getRF () ;//, voff = getOffset() ;

    if ( direction == 1 )
        {
        b = from - 1 ;
        coff = 0 ;
        complement = false ;
        }
    else
        {
        b = to - 1 ;
        coff = -2 ;
        complement = true ;
        }
    b += direction * (rf-1) ;

    if ( to >= from ) dna2aa.reserve ( ( to - from + 1 ) / 3 + 5 ) ;
    else dna2aa.reserve ( ( to + v->getSequenceLength() - from + 1 ) / 3 + 5 ) ;

    long translation_table = -1 ;
    getParam( _T("/transl_table") , _T("-1") ).ToLong ( &translation_table ) ;

    wxString three ;
    while ( c != '|' && rf != 0 )
        {
        three.Empty() ;
        three += v->getNucleotide ( b + 0 * direction , complement ) ;
        three += v->getNucleotide ( b + 1 * direction , complement ) ;
        three += v->getNucleotide ( b + 2 * direction , complement ) ;
        c = v->dna2aa ( three , translation_table ) . GetChar ( 0 ) ;

       // SeqAA update
       if ( aa )
           {
           // Protease analysis
           if ( !aa->proteases.IsEmpty() )
               {
               aa->pa_w += c ;
               aa->pa_wa.Add ( b+coff ) ;
               aa->analyzeProteases () ;
               }

         // Offset?
/*        int pawl = dna2aa.size() ;
          if ( voff != -1 && ( b + coff ) % 10 == 0 )
              {
              aa->offsets.Alloc ( b+coff ) ;
              aa->offset_items.Alloc ( b+coff ) ;
              while ( aa->offsets.GetCount() <= b+coff ) aa->offsets.Add ( -1 ) ;
              while ( aa->offset_items.GetCount() <= b+coff ) aa->offset_items.Add ( NULL ) ;
              aa->offsets[b+coff] = voff + pawl - 1 ;
              aa->offset_items[b+coff] = this ;
              }
*/
           }

        // Output
        dna2aa.push_back ( Tdna2aa ( c , b+0+coff , b+1+coff , b+2+coff ) ) ;

        b += direction * 3 ;
        if ( !v->isCircular() && b+direction*3 < 0 ) rf = 0 ;
        if ( !v->isCircular() && b+direction*3 > v->getSequenceLength() ) rf = 0 ;
        if ( v->isCircular() && ( b < 0 || b >= v->getSequenceLength() ) )
            {
            if ( roundOnce ) rf = 0 ;
            else if ( b < 0 ) b += v->getSequenceLength() ;
            else b -= v->getSequenceLength() ;
            roundOnce = true ;
            }
        if ( /*can->getPD() &&*/ c == '?' )
            c = '|' ;
        }
    }

wxString TVectorItem::getAminoAcidSequence () // not const because of translate
    {
    wxString s ;
    vector <Tdna2aa> dna2aa ;
    translate ( lastVector , NULL , dna2aa ) ;
    for ( int a = 0 ; a < dna2aa.size() ; a++ )
        s += dna2aa[a].aa ;
    return s ;
    }

void TVectorItem::getArrangedAA ( TVector * const v , wxString &s , const int disp , SeqAA * const aa ) // not const
    {
    vector <Tdna2aa> dna2aa_temp , *dna2aa ;
    if ( v->getGenomeMode() ) dna2aa = &dna2aa_temp ;
    else
        {
        dna2aa_item.clear() ;
        dna2aa = &dna2aa_item ;
        }
    translate ( v , aa , *dna2aa ) ;
    for ( int a = 0 ; a < dna2aa->size() ; a++ )
        {
        if ( disp == AA_ONE ) s.SetChar((*dna2aa)[a].dna[0],(*dna2aa)[a].aa) ;
        else
            {
            wxString three = v->one2three((int)(*dna2aa)[a].aa) ;
            s.SetChar((*dna2aa)[a].dna[0], three.GetChar(0) ) ;
            s.SetChar((*dna2aa)[a].dna[1], three.GetChar(1) ) ;
            s.SetChar((*dna2aa)[a].dna[2], three.GetChar(2) ) ;
            }
        }
    }

int TVectorItem::getOffsetAt ( int i ) const
    {
//    myass ( lastVector , "TVectorItem::getOffsetAt" ) ;
    if ( !lastVector ) return -1 ;
    if ( lastVector && lastVector->getGenomeMode() ) return -1 ;
    if ( getOffset () == -1 ) return -1 ;
    for ( int a = 0 ; a < dna2aa_item.size() ; a++ )
        {
        if ( dna2aa_item[a].dna[0] == i )
            return a + getOffset() ;
        }
    return -1 ;
    }
    
int TVectorItem::getMem () const
    {
    int a , r = 0 ;
    for ( a = 0 ; a < pname.GetCount() ; a++ ) r += pname[a].length() + pvalue[a].length() ;
//    r += dna2aa.size() * sizeof ( Tdna2aa ) ;
    r += 7 * sizeof ( int ) ;
    r += desc.length() + name.length() ;
    r += 2 * sizeof ( float ) + sizeof ( wxTreeItemId ) ;
    r += sizeof ( TVectorItem ) ;
    return r ;
    }    
    
void TVectorItem::setLastVector ( TVector * const v )
    {
    lastVector = v ;
    }

// ******************************************************************* Tdna2aa

Tdna2aa::Tdna2aa ( const char _aa , const int i1 , const int i2 , const int i3 )
    {
    aa = _aa ;
    dna[0] = i1 ;
    dna[1] = i2 ;
    dna[2] = i3 ;
    }

// ******************************************************************* TAAProp

TAAProp::TAAProp ()
    {
    tla = _T("???") ;
    mw = pi = 0 ;
    cf_f[0] = cf_f[1] = cf_f[2] = cf_f[3] = 0 ;
    cf_pa = cf_pb = cf_pt = 0 ;
    hp_kd = hp_hw = 0 ;
    carbon = hydrogen = nitrogen = oxygen = sulfur = 0 ;
    hl_mammal = hl_yeast = hl_ecoli = 0 ;
    }
    
void TAAProp::set_cf ( const int pa , const int pb , const int pt , const float f0 , const float f1 , const float f2 , const float f3 )
    {
    cf_pa = pa ;
    cf_pb = pb ;
    cf_pt = pt ;
    cf_f[0] = f0 ;
    cf_f[1] = f1 ;
    cf_f[2] = f2 ;
    cf_f[3] = f3 ;
    }
    
void TAAProp::set_data ( const float _mw , const float _pi , const wxString& _tla )
    {
    mw = _mw ;
    pi = _pi ;
    tla = _tla ;
    }
    
void TAAProp::set_hp ( const float _hp_kd , const float _hp_hw )
    {
    hp_kd = _hp_kd ;
    hp_hw = _hp_hw ;
    }

void TAAProp::set_atoms ( const int C , const int H , const int N , const int O , const int S )
    {
    carbon = C ;
    hydrogen = H ;
    nitrogen = N ;
    oxygen = O ;
    sulfur = S ;
     }
         
void TAAProp::set_halflife ( const int mammal , const int yeast , const int ecoli )
    {
    hl_mammal = mammal ;
    hl_yeast = yeast ;
    hl_ecoli = ecoli ;
    }
     
wxString TAAProp::get_halflife_text ( int hl ) const
    {
    wxString ret ;
    if ( hl == 0 ) return txt("t_hl_unknown") ;
    if ( hl < 0 )
        {
        hl = -hl ;
        ret += _T(">") ;
        }
    int h = hl / 60 ;
    int m = hl % 60 ;
    if ( h == 1 ) ret += wxString::Format ( txt("t_hl_hour") , 1 ) ;
    else if ( h > 1 ) ret += wxString::Format ( txt("t_hl_hours") , h ) ;
    if ( h > 0 && m > 0 ) ret += _T(" ") ;
    if ( m > 0 ) ret += wxString::Format ( txt("t_hl_minutes") , m ) ;
    return ret ;
    }

// ***************************************************************************************
// TORF
// ***************************************************************************************

TORF::TORF ()
    {
    rf = 0 ;
    } ;


TORF::TORF ( const int _f , const int _t , const int _r )
    {
    set ( _f , _t , _r ) ;
    }

void TORF::set ( const int _f , const int _t , const int _r )
    {
    from = _f ;
    to = _t ;
    rf = _r ;
    } ;

wxString TORF::getText () const
    {
    return wxString::Format ( _T("%d-%d, %d") , from , to , rf ) ;
    }
