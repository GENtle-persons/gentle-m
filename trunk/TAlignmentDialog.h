#ifndef _T_ALIGNMENT_DIALOG_
#define _T_ALIGNMENT_DIALOG_

#include "main.h"

class TAlignment ;

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
    
    wxArrayTVector vav , vcv ;
    wxArrayString van , vcn ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
