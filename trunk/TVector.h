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
    virtual void set_data ( float _mw , float _pi , string _tla ) ;
    virtual void set_hp ( float _hp_kd , float _hp_hw ) ;
    string tla ;
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
    virtual void setType ( string s ) ;
    
    virtual void doRemove ( int f , int t , int l ) ;
    virtual string implodeParams () ;
    virtual void explodeParams ( string s ) ;
    virtual wxTreeItemId getTreeID () { return treeid ; } ;
    virtual string getParam ( string p , string def = "" ) ;
    virtual vector <string> getParamKeys () ;

    // dna2aa stuff
    virtual void translate ( TVector *v , SeqAA *aa = NULL ) ;
    virtual void getArrangedAA ( TVector *v , string &s , int disp ) ;
    
    virtual void dummyInfo ( string s , int l ) ;  // For testing only
    
    // Variables
    wxString desc , name ;
    int from , to ;
    signed char direction ; // 1=clockwise, -1=counter-clockwise
    char type ; // VIT_XXX
    
    protected:
    friend class TGenBank ;
    friend class TItemEditDialog ;
    friend class PlasmidCanvas ;
    friend class TXMLfile ;
    virtual void setParam ( string p , string v ) ;
    virtual void setParam ( string p , int v ) ;

    friend class TVectorTree ;
    wxTreeItemId treeid ;

    // Visual information
    friend class TVectorEditor ;
    friend class MyChild ;
    int r1 , r2 ;
    int r3 , r4 ; // Only used in linar display mode
    float a1 , a2 ;
    
    private :
    virtual void initParams () ;
    vector <string> pname , pvalue ;
    
    // dna2aa data
    vector <Tdna2aa> dna2aa ;
    } ;

class TVector
    {
    public :
    TVector ( ChildBase *win = NULL ) ;  
    virtual ~TVector () ;
    virtual void init () ;
    virtual void clear () ;
    virtual void recalculateCuts () ;
    virtual vector <TRestrictionCut> getCuts ( TRestrictionEnzyme *e ) ;
    
    virtual bool basematch ( char b1 , char b2 ) ; // b1 in IUPAC, b2 in SIUPAC
    virtual void setIUPAC ( char b , char *s , char *pac = NULL ) ;
    virtual string transformSequence ( bool inverse , bool reverse ) ;
    virtual char getNucleotide ( int pos , bool complement = false ) ;
    virtual void setNucleotide ( int pos , char t ) ;
    virtual char getComplement ( char c ) ;
    virtual void ligate_right ( TVector &v , bool inverted = false ) ;
    virtual void closeCircle () ;
    
    virtual TVector *getAAvector ( int from , int to , int dir = 1 ) ;
    virtual bool reduceToFragment ( TRestrictionCut left , TRestrictionCut right ) ;
    virtual void doRestriction () ;
    virtual void sortRestrictionSites () ;
    virtual void doAction () ;
    virtual void doRemove ( int from , int to , bool update = true ) ;
    virtual int countCuts ( string enzyme ) ;
    
    virtual string getSubstring ( int mf , int mt ) ;
    virtual void insert_char ( char x , int pos , bool overwrite = false ) ;
    
    virtual float getAAmw ( char aa ) ;
    virtual float getAApi ( char aa ) ;
    virtual string dna2aa ( string codon ) ;
    virtual void turn ( int off ) ;
    virtual void setAction ( string _action , int _action_value = 0 ) ;
    virtual void setDatabase ( string s ) { database = s ; }
    virtual string getDatabase () { return database ; }
    virtual TVector *newFromMark ( int from , int to ) ;
    
    virtual void setChanged ( bool c = true ) ;
    virtual bool isChanged () { return changed ; }
    
    virtual void ClearORFs () ;
    virtual void addORFs ( int off ) ;

    virtual void removeBlanksFromSequence () ;
    virtual void removeBlanksFromVector () ;
    
    virtual string getParams () ;
    virtual void setParams ( string s ) ;
    virtual void setWindow ( ChildBase *c ) ;
    virtual void setCircular ( bool c = true ) ;
    virtual bool isCircular () ;
    virtual bool isLinear () ;
    virtual string one2three ( int a ) ;
    virtual void setStickyEnd ( bool left , bool upper , string s ) ;
    virtual string getStickyEnd ( bool left , bool upper ) ;
    virtual bool hasStickyEnds () ;
    virtual void callUpdateUndoMenu () ;
    virtual void setFromVector ( TVector &v ) ;
    virtual void doRemoveNucleotide ( int x ) ;
    virtual int getItemLength ( int a ) ;

    virtual TAAProp getAAprop ( char a ) ;
    virtual int find_item ( string s ) ;
    virtual bool isEnzymeHidden ( string s ) ;
    
    // Variables
    string sequence ;
    string name , desc ;
    int type ;
    bool recalcvisual ;
    vector <TVectorItem> items ;
    vector <TRestrictionEnzyme*> re ;
    vector <TRestrictionCut> rc ;
    vector <string> cocktail ;
    vector <TORF> worf ;

    vector <string> hiddenEnzymes ;
    vector <string> proteases ;
    TUndo undo ;
    
    private :
    string invert ( string s ) ;
    
    string _lu , _ll , _ru , _rl ; // Sticky ends
    bool circular ;
    ChildBase *window ;
    string params ;
    int turned ;
    string action ;
    int action_value ;
    string aa , database ;
    bool changed ;
    
    static char IUPAC[256] , SIUPAC[256] , COMPLEMENT[256] ;
    static char ACGT[256] ;
    static vector <TAAProp> aaprop ;
    } ;
    
#endif
