#ifndef _ALIGNMENT_H_
#define _ALIGNMENT_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>
#include <wx/spinctrl.h>

#define BACK_LEFT 1
#define BACK_UP 2
#define BACK_LU 4

#define ALG_CW 0
#define ALG_SW 1
#define ALG_NW 2

typedef vector <int> tvi ;
typedef vector <char> tvc ;

class SeqAA ;

class TAlignLine // pun intended
    {
    public :
    TAlignLine () ;
    virtual ~TAlignLine () ;
    virtual void ResetSequence () ;
    virtual ChildBase *FindOrigin () ;
    virtual void showFeatures () ;
    virtual void hideFeatures () ;
    virtual TVector *getFeatures () ;
    virtual bool hasFeatures () ;
    
    string name , s ;
    TVector *v ;
    bool isIdentity ;
    private :
    TVector *features ;
    } ;

class TAlignment : public ChildBase
    {
    public :
    TAlignment(MyFrame *parent, const wxString& title) ;
    ~TAlignment () ;
    
    void initme () ;
    virtual string getName () ;

    int NeedlemanWunsch ( string &s1 , string &s2 )  ;
    int SmithWaterman ( string &s1 , string &s2 )  ;
    int MatrixAlignment ( string &s1 , string &s2 , bool local )  ;
    void MatrixBacktrack ( vector <tvc> &back , 
                                    string s1 , string s2 , 
                                    string &t1 , string &t2 ,
                                    int i , int j ) ;
    
    void redoAlignments ( bool doRecalc = true ) ;
    void recalcAlignments () ;
    wxColour findColors ( char c1 , char c2 , bool fg ) ;

    void OnSettings ( wxCommandEvent &ev ) ;
    void OnClose(wxCloseEvent& event) ;
    void invokeOriginal ( int id , int pos ) ;

    void OnMenuBold ( wxCommandEvent &ev ) ;
    void OnMenuMono ( wxCommandEvent &ev ) ;
    void OnMenuNorm ( wxCommandEvent &ev ) ;
    void OnMenuInvs ( wxCommandEvent &ev ) ;
    void OnMenuSoa ( wxCommandEvent &ev ) ;
    void OnMenuSoaI ( wxCommandEvent &ev ) ;
    void OnMenuSiml ( wxCommandEvent &ev ) ;
    void OnMenuSeq ( wxCommandEvent &ev ) ;
    void OnMenuFeat ( wxCommandEvent &ev ) ;
    void OnMenuRNA ( wxCommandEvent &ev ) ;
    void OnMenuCons ( wxCommandEvent &ev ) ;
    void OnMenuIdent ( wxCommandEvent &ev ) ;
    
    void OnHorizontal ( wxCommandEvent& event ) ;
    
    void callMiddleMouseButton ( int id , int pos ) ;
    void MoveUpDown ( int what , int where ) ;
    
    // Variables
    SequenceCanvas *sc ;
    wxPanel *up ;
    wxSplitterWindow *hs ;
    string gap ;
    
    vector <TAlignLine> lines ;

    int match , mismatch , gap_penalty ;
    int algorithm ;
    string matrix ;
    bool bold , mono ;
    
    private :
    wxListBox *mmb ;
    SeqAA *aaa ;
    void myInsert ( int line , int pos , char what ) ;
    void myDelete ( int line , int pos ) ;
    void updateSequence () ;
    void generateConsensusSequence ( bool addit = true ) ;
    void fixMenus ( int i ) ;
    
    public :
    string consensusSequence ;
    vector <wxColour> colDNA , colAA , *colCur ;
    bool cons , invs ;
    bool showIdentity ;
        
    DECLARE_EVENT_TABLE()
    } ;



#endif
