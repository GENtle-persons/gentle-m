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

#define VIT_TYPES      7 //number of possible types

#define VIT_GENE       1
#define VIT_CDS        2
#define VIT_REP_ORI    3
#define VIT_PROMOTER   4
#define VIT_TERMINATOR 5
#define VIT_MISC       6

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

class TVectorItem
    {
    public :
    TVectorItem () ;
    TVectorItem ( wxString sn , wxString n , int f , int t , char ty ) ;
    wxBrush *getBrush () ;
    wxColour getFontColor () ;
    void setColor ( wxColour col ) ;
    bool isVisible () ;
    void setVisible ( bool b ) ;
    int getRF () ;
    void setRF ( int x ) ;
    int getOffset () ;
    void setOffset ( int o = -1 ) ; // -1 = no offset
    void dummyInfo ( string s , int l ) ;  // For testing only
    
    // Variables
    wxString desc , name ;
    int from , to ;
    signed char direction ; // 1=clockwise, -1=counter-clockwise
    char type ; // VIT_XXX
    
    virtual void doRemove ( int f , int t , int l ) ;
    virtual string implodeParams () ;
    virtual void explodeParams ( string s ) ;
    virtual wxTreeItemId getTreeID () { return treeid ; } ;
    virtual string getParam ( string p , string def = "" ) ;

    protected:
    friend class TGenBank ;
    friend class TItemEditDialog ;
    friend class PlasmidCanvas ;
    virtual void setParam ( string p , string v ) ;
    virtual void setParam ( string p , int v ) ;
    virtual vector <string> getParamKeys () ;

    friend class TVectorTree ;
    wxTreeItemId treeid ;

    // Visual information
    friend class PlasmidCanvas ;
    friend class TVectorEditor ;
    friend class MyChild ;
    int r1 , r2 ;
    int r3 , r4 ; // Only used in linar display mode
    float a1 , a2 ;
    
    private :
    virtual void initParams () ;
    wxBrush brush ;
    vector <string> pname , pvalue ;
    } ;

class TVector
    {
    public :
    TVector ( ChildBase *win = NULL ) ;  
    virtual ~TVector () ;
    virtual void init () ;
    virtual void recalculateCuts () ;
    virtual vector <TRestrictionCut> getCuts ( TRestrictionEnzyme *e ) ;
    
    virtual bool basematch ( char b1 , char b2 ) ; // b1 in IUPAC, b2 in SIUPAC
    virtual void setIUPAC ( char b , char *s , char *pac = NULL ) ;
    virtual string transformSequence ( bool inverse , bool reverse ) ;
    virtual char getNucleotide ( int pos , bool complement = false ) ;
    virtual char getComplement ( char c ) ;
    virtual void ligate_right ( TVector &v , bool inverted = false ) ;
    virtual void closeCircle () ;
    
    virtual TVector *getAAvector ( int from , int to , int dir = 1 ) ;
    virtual bool reduceToFragment ( TRestrictionCut left , TRestrictionCut right ) ;
    virtual void doRestriction () ;
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
    
    virtual void setChanged ( bool c = true ) ;
    virtual bool isChanged () { return changed ; }
    
    virtual void ClearORFs () ;
    virtual void addORFs ( int off ) ;

    virtual void removeBlanksFromSequence () ;
    
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
    string _one2three[256] ;
    bool circular ;
    ChildBase *window ;
    string params ;
    float aa_mw[256] , aa_pi[256] ;
    char IUPAC[256] , SIUPAC[256] ;
    char ACGT[256] ;
    int turned ;
    string action ;
    int action_value ;
    string aa , database ;
    bool changed ;
    } ;
    
#endif

