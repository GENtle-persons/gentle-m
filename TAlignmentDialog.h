/** \file
	\brief Contains the TAlignmentDialog class
*/
#ifndef _T_ALIGNMENT_DIALOG_
#define _T_ALIGNMENT_DIALOG_

#include "main.h"

class TAlignment ;

/** \brief The alignment settings dialog
*/
class TAlignmentDialog : public wxDialog
    {
    public :
    TAlignmentDialog(wxWindow *parent, const wxString& title ) ; ///< Constructor
    ~TAlignmentDialog(); ///< Destructor

    virtual void init_what () ; ///< Tab for sequences to align
    virtual void init_how () ; ///< Tab for algorithm settings
    virtual void init_disp () ; ///< Tab for display settings (unused)

    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    virtual void OnAdd ( wxCommandEvent &ev ) ; ///< "Add sequence to the list" event handler
    virtual void OnDel ( wxCommandEvent &ev ) ; ///< "Remove sequence from the list" event handler
    virtual void OnUp ( wxCommandEvent &ev ) ; ///< "Move sequence up in list" event handler
    virtual void OnDown ( wxCommandEvent &ev ) ; ///< "Move sequence down in list" event handler

    wxNotebook *nb ; ///< Pointer to the wxNotebook structure holding the tabs
    wxPanel *pwhat , *phow , *pdisp ;
    wxListBox *cur , *all , *alg ;
    wxSpinCtrl *alg_match , *alg_mismatch , *alg_penalty ;
    wxChoice *alg_matrix ;

    int bo , th ;
    TAlignment *al ; ///< Pointer to the calling TAlignment module

    wxArrayTVector vav , vcv ;
    wxArrayString van , vcn ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
