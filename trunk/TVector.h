#ifndef _TVECTOR_H_
#define _TVECTOR_H_

#include "main.h"

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
    void init () ;
    void recalculateCuts () ;
    vector <TRestrictionCut> getCuts ( TRestrictionEnzyme *e ) ;
    
    bool basematch ( char b1 , char b2 ) ; // b1 in IUPAC, b2 in SIUPAC
    void setIUPAC ( char b , char *s , char *pac = NULL ) ;
    string transformSequence ( bool inverse , bool reverse ) ;
    char getNucleotide ( int pos , bool complement = false ) ;
    char getComplement ( char c ) ;
    void ligate_right ( TVector &v ) ;
    void closeCircle () ;
    
    TVector *getAAvector ( int from , int to , int dir = 1 ) ;
    bool reduceToFragment ( TRestrictionCut left , TRestrictionCut right ) ;
    void doRestriction () ;
    void doAction () ;
    void doRemove ( int from , int to , bool update = true ) ;
    int countCuts ( string enzyme ) ;
    
    string getSubstring ( int mf , int mt ) ;
    void insert_char ( char x , int pos , bool overwrite = false ) ;
    
    float getAAmw ( char aa ) ;
    float getAApi ( char aa ) ;
    string dna2aa ( string codon ) ;
    void turn ( int off ) ;
    void setAction ( string _action , int _action_value = 0 ) ;
    void setDatabase ( string s ) { database = s ; }
    string getDatabase () { return database ; }
    
    void setChanged ( bool c = true ) ;
    bool isChanged () { return changed ; }
    
    void ClearORFs () ;
    void addORFs ( int off ) ;

    void removeBlanksFromSequence () ;
    
    string getParams () ;
    void setParams ( string s ) ;
    void setWindow ( ChildBase *c ) ;
    void setCircular ( bool c = true ) ;
    bool isCircular () ;
    bool isLinear () ;
    string one2three ( int a ) ;
    void setStickyEnd ( bool left , bool upper , string s ) ;
    string getStickyEnd ( bool left , bool upper ) ;
    bool hasStickyEnds () ;
    
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
    
    private :
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

