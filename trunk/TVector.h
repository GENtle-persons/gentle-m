#ifndef _TVECTOR_H_
#define _TVECTOR_H_

#include "main.h"
#include "TUndo.h"

class MyChild ;
class TRestrictionEnzyme ;
class TRestrictionCut ;
class TGenBank ;
class TVectorTree ;
class PlasmidCanvas ;
class TVectorEditor ;
class ChildBase ;
class SeqAA ;
class TEnzymeRules ;

#define VIT_TYPES      9//number of possible types

#define VIT_GENE       1
#define VIT_CDS        2
#define VIT_REP_ORI    3
#define VIT_PROMOTER   4
#define VIT_TERMINATOR 5
#define VIT_MISC       6
#define VIT_PROT_BIND  7
#define VIT_ORI_T      8

class TAAProp
    {
    public :
    TAAProp () ;
    virtual void set_cf ( int pa , int pb , int pt , float f0 , float f1 , float f2 , float f3 ) ;
    virtual void set_data ( float _mw , float _pi , wxString _tla ) ;
    virtual void set_hp ( float _hp_kd , float _hp_hw ) ;
    wxString tla ;
    float mw , pi ;
    float cf_f[4] ;
    int cf_pa , cf_pb , cf_pt ;
    float hp_kd , hp_hw ;
    vector <float> data ; // For temporary data only
    } ;

class TORF
    {
    public :
    TORF () { rf = 0 ; } ;
    TORF ( int _f , int _t , int _r )
        {
        from = _f ;
        to = _t ;
        rf = _r ;
        } ;
    
    int from , to ;
    int rf ;
    
    float dist1 , dist2 ;
    float deg1 , deg2 ;
    } ;
    
class Tdna2aa
    {
    public :
    Tdna2aa () {} ;
    Tdna2aa ( char _aa , int i1 , int i2 , int i3 ) ;
    char aa ; // Single-letter AA code
    int dna[3] ; // The three DNA positions that make up the codon
    } ;

class TVectorItem
    {
    public :
    TVectorItem () ;
    TVectorItem ( wxString sn , wxString n , int f , int t , char ty ) ;
    
    virtual wxBrush *getBrush () ;
    virtual wxColour getFontColor () ;
    virtual void setColor ( wxColour col ) ;
    virtual bool isVisible () ;
    virtual void setVisible ( bool b ) ;
    virtual int getRF () ;
    virtual void setRF ( int x ) ;
    virtual int getOffset () ;
    virtual void setOffset ( int o = -1 ) ; // -1 = no offset
    virtual void setType ( wxString s ) ;
    virtual int getOffsetAt ( int i ) ;
    
    virtual void doRemove ( int f , int t , int l ) ;
    virtual wxString implodeParams () ;
    virtual void explodeParams ( wxString _s ) ;
    virtual wxTreeItemId getTreeID () { return treeid ; } ;
    virtual void setTreeID ( wxTreeItemId newid ) { treeid = newid ; }
    virtual int getDirection () { return direction ; }
    virtual void setDirection ( int newdir ) { direction = newdir ; }
    virtual char getType () { return type ; }
    virtual void setType ( char newtype ) { type = newtype ; }
    virtual int getMem () ;

    // Parameters
    virtual wxString getParam ( wxString p , wxString def = "" ) ;
    virtual wxArrayString getParamKeys () ;
    virtual void setParam ( wxString p , wxString v ) ;
    virtual void setParam ( wxString p , int v ) ;

    // dna2aa stuff
    virtual void translate ( TVector *v , SeqAA *aa , vector <Tdna2aa> &dna2aa ) ;
    virtual void getArrangedAA ( TVector *v , wxString &s , int disp , SeqAA *aa = NULL ) ;
    virtual wxString getAminoAcidSequence () ;
    virtual void setLastVector ( TVector *v ) ;
    
    // Variables
    wxString desc , name ;
    int from , to ;
    
    private:
    virtual void initParams () ;
    
    signed char direction ; // 1=clockwise, -1=counter-clockwise
    char type ; // VIT_XXX
    wxTreeItemId treeid ;
    wxArrayString pname , pvalue ;
    vector <Tdna2aa> dna2aa_item ;
    TVector *lastVector ;

    // Visual information
    friend class TVectorEditor ;
    friend class MyChild ;
    friend class PlasmidCanvas ;
    int r1 , r2 ;
    int r3 , r4 ; // Only used in linar display mode
    float a1 , a2 ;
    } ;

class TVector
    {
    public :
    TVector ( ChildBase *win = NULL ) ;  
    virtual ~TVector () ;
    virtual void init () ;
    virtual void clear () ;
    
    // Restriction enzymes
    virtual void recalculateCuts () ;
    virtual void getCuts ( TRestrictionEnzyme *e , vector <TRestrictionCut> &ret , 
    						bool clear_vector = true , int max = 10000000 ) ;
    virtual bool reduceToFragment ( TRestrictionCut left , TRestrictionCut right ) ;
    virtual void doRestriction () ;
    virtual void sortRestrictionSites () ;
    virtual int countCuts ( wxString enzyme ) ;
    
    // Nucleotide access/conversion
    virtual bool basematch ( char b1 , char b2 ) ; // b1 in IUPAC, b2 in SIUPAC
    virtual void setIUPAC ( char b , char *s , char *pac = NULL ) ;
    virtual char getNucleotide ( int pos , bool complement = false ) ;
    virtual void setNucleotide ( int pos , char t ) ;
    virtual char getComplement ( char c ) ;
    
    // Vector/sequence access
    virtual void ligate_right ( TVector &v , bool inverted = false ) ;
    virtual void closeCircle () ;
    virtual void turn ( int off ) ;
    
    virtual TVector *getAAvector ( int from , int to , int dir = 1 ) ;
    virtual void doAction () ;
    virtual void doRemove ( int from , int to , bool update = true , bool enableUndo = true ) ;
    virtual void insert_char ( char x , int pos , bool overwrite = false ) ;
    
    virtual float getAAmw ( char aa ) ;
    virtual float getAApi ( char aa ) ;
    virtual wxString dna2aa ( wxString codon , int translation_table = -1 ) ;
    virtual void setAction ( wxString _action , int _action_value = 0 ) ;
    virtual void setDatabase ( wxString s ) { database = s ; }
    virtual wxString getDatabase () { return database ; }
    virtual TVector *newFromMark ( int from , int to ) ;
    
    virtual void setChanged ( bool c = true ) ;
    virtual bool isChanged () { return changed ; }
    
    virtual void ClearORFs () ;
    virtual void addORFs ( int off ) ;

    virtual void removeBlanksFromSequence () ;
    virtual void removeBlanksFromVector () ;
    
    virtual wxString getParams () ;
    virtual void setParams ( const wxString &s ) ;
    virtual void setWindow ( ChildBase *c ) ;
    virtual void setCircular ( bool c = true ) ;
    virtual bool isCircular () ;
    virtual bool isLinear () ;
    virtual wxString one2three ( int a ) ;
    virtual char three2one ( wxString s ) ;
    virtual void setStickyEnd ( bool left , bool upper , wxString s ) ;
    virtual wxString getStickyEnd ( bool left , bool upper ) ;
    virtual bool hasStickyEnds () ;
    virtual void callUpdateUndoMenu () ;
    virtual void setFromVector ( TVector &v ) ;
    virtual void doRemoveNucleotide ( int x ) ;
    virtual int getItemLength ( int a ) ;
    virtual TVector *backtranslate () ;

    virtual TAAProp getAAprop ( char a ) ;
    virtual int find_item ( wxString s ) ;
    virtual bool isEnzymeHidden ( wxString s ) ;

    virtual wxString getSubstring ( int mf , int mt ) ;
    virtual wxString transformSequence ( bool inverse , bool reverse ) ;
    virtual wxString getSequence () ;
    virtual wxString *getSequencePointer () ; // Only used by SequenceCanvas::OnCharHook
    virtual char getSequenceChar ( int x ) ;
    virtual void setSequence ( wxString ns ) ;
    virtual void addToSequence ( wxString x ) ;
    virtual void alterSequence ( int pos , char c ) ;
    virtual int getSequenceLength() ;
    virtual void eraseSequence ( int from , int len ) ;
    virtual void removeAlignmentArtifacts ( char what = '-' ) ;
    
    virtual wxString getDescription () ;
    virtual void setDescription ( wxString s ) ;
    virtual void addDescription ( wxString s ) ;
    virtual wxString getName () ;
    virtual void setName ( wxString s ) ;
    virtual void addName ( wxString s ) ;
    virtual void setGenomeMode ( bool gm = true ) ;
    virtual bool getGenomeMode () ;
    virtual int getMem () ;
    virtual void getVectorCuts ( TVector *v ) ;
    
    // Variables
    int type ;
    bool recalcvisual ;
    vector <TVectorItem> items ;
    vector <TORF> worf ;
    vector <TRestrictionCut> rc ;
    wxArrayTRestrictionEnzyme re ;

    wxArrayString hiddenEnzymes , proteases , cocktail ;
    TUndo undo ;
    
    private :
    virtual wxString invert ( wxString s ) ;
    virtual wxString vary_base ( char b ) ;
    virtual void makeAA2DNA () ;
    virtual wxString mergeCodons ( wxString c1 , wxString c2 ) ;
    virtual void setCodonTable ( int table , wxString sequence ) ;

    wxString sequence ;    
    wxString _lu , _ll , _ru , _rl ; // Sticky ends
    wxString name , desc ;
    bool circular , changed , genomeMode ;
    ChildBase *window ;
    int turned , action_value ;
    wxString params , database , action , aa ;
    wxString AA2DNA[256] ;
    TEnzymeRules *enzyme_rules ;
    
    static char IUPAC[256] , SIUPAC[256] , COMPLEMENT[256] ;
    static char ACGT[256] ;
    static vector <TAAProp> aaprop ;
    static wxArrayString codon_tables ;
    } ;
    
#endif

