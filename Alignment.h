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
    
    void redoAlignments () ;
    wxColour findColors ( char c1 , char c2 , bool fg ) ;

    void OnSettings ( wxCommandEvent &ev ) ;
    void OnClose(wxCloseEvent& event) ;
    
    // Variables
    SequenceCanvas *sc ;
    wxPanel *up ;
    wxSplitterWindow *hs ;
    string gap ;
    
    vector <string> qAlign , qName ;
    int match , mismatch , gap_penalty ;
    vector <TVector*> qVec ;
    int algorithm ;
    string matrix ;
        
    DECLARE_EVENT_TABLE()
    } ;

class TAlignmentDialog : public wxDialog
    {
    public :
    TAlignmentDialog(wxWindow *parent, const wxString& title ) ;
    ~TAlignmentDialog();
    void init_what () ;
    void init_how () ;
    void init_disp () ;
    
    wxNotebook *nb ;
    wxPanel *pwhat , *phow , *pdisp ;
    wxListBox *cur , *all , *alg ;
    wxSpinCtrl *alg_match , *alg_mismatch , *alg_penalty ;
    wxChoice *alg_matrix ;
    
    int bo , th ;
    TAlignment *al ;
    
    virtual void OnCharHook(wxKeyEvent& event) ;
    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;
    void OnAdd ( wxCommandEvent &ev ) ;
    void OnDel ( wxCommandEvent &ev ) ;
    void OnUp ( wxCommandEvent &ev ) ;
    void OnDown ( wxCommandEvent &ev ) ;
    
    vector <TVector*> vav , vcv ;
    vector <string> van , vcn ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
