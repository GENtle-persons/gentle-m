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
    virtual ~TAlignment () ;
    
    virtual void initme () ;
    virtual string getName () ;

    virtual int NeedlemanWunsch ( string &s1 , string &s2 )  ;
    virtual int SmithWaterman ( string &s1 , string &s2 )  ;
    virtual int MatrixAlignment ( string &s1 , string &s2 , bool local )  ;
    virtual void MatrixBacktrack ( vector <tvc> &back , 
                                    string s1 , string s2 , 
                                    string &t1 , string &t2 ,
                                    int i , int j ) ;
    
    virtual void redoAlignments ( bool doRecalc = true ) ;
    virtual void recalcAlignments () ;
    virtual wxColour findColors ( char c1 , char c2 , bool fg ) ;

    virtual void OnSettings ( wxCommandEvent &ev ) ;
    virtual void OnClose(wxCloseEvent& event) ;
    virtual void OnSeqPrint(wxCommandEvent& event);
    virtual void OnFileSave ( wxCommandEvent &ev ) ;
    virtual void OnMenuBold ( wxCommandEvent &ev ) ;
    virtual void OnMenuMono ( wxCommandEvent &ev ) ;
    virtual void OnMenuNorm ( wxCommandEvent &ev ) ;
    virtual void OnMenuInvs ( wxCommandEvent &ev ) ;
    virtual void OnMenuSoa ( wxCommandEvent &ev ) ;
    virtual void OnMenuSoaI ( wxCommandEvent &ev ) ;
    virtual void OnMenuSiml ( wxCommandEvent &ev ) ;
    virtual void OnMenuSeq ( wxCommandEvent &ev ) ;
    virtual void OnMenuFeat ( wxCommandEvent &ev ) ;
    virtual void OnMenuRNA ( wxCommandEvent &ev ) ;
    virtual void OnMenuCons ( wxCommandEvent &ev ) ;
    virtual void OnMenuIdent ( wxCommandEvent &ev ) ;
    virtual void OnHorizontal ( wxCommandEvent& event ) ;
    
    virtual void invokeOriginal ( int id , int pos ) ;
    virtual void callMiddleMouseButton ( int id , int pos , wxString _mode = "" ) ;
    virtual void MoveUpDown ( int what , int where ) ;
    virtual void prealigned ( vector <string> &vs , vector <ChildBase*> &vc ) ;
    virtual void fromVector ( TVector *nv ) ;
    
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
    wxChoice *mmb ;
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
