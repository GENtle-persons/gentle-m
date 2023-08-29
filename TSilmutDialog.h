/** \file
	\brief Contains the TSilmutDialog class, and its helper class TSilmutItem
*/
#ifndef _TSILMUTDIALOG_H_
#define _TSILMUTDIALOG_H_

#include "main.h"
#include <wx/spinctrl.h>

#define M_SILMUT 0
#define M_WHATCUTS 1

class TRestrictionEnzyme ;
class TPrimerDesign ;

/**	\brief Helper class for TSilmutDialog
*/
class TSilmutItem
    {
    public :
    wxString mut ; ///< Mutated sequence
    int changes , cuts ;
    TRestrictionEnzyme *e ; ///< Pointer to the restriction enzyme
    wxArrayInt fragments ;
    } ;

/** \brief Silent mutation dialog
*/
class TSilmutDialog : public wxDialog
    {
    public :
    TSilmutDialog ( wxWindow *parent , const wxString &s ,
                    int _mode = M_SILMUT ,
                    int _mut_pos = -1 , int _mut_dir = 1 ) ; ///< Constructor
    ~TSilmutDialog () ; ///< Destructor

    virtual void initme ( TVector *vec , int _from , int _to ) ; ///< Initialization
    virtual void calc () ; ///< Calculate new cuts through mutation
    virtual void calc_acr () ; ///< Calculate cuts that go away through mutation
    virtual void showit () ; ///< Show results
    virtual void OnSpin ( wxSpinEvent &event ) ; ///< Spin control (changes) event handler
    virtual void OnSpin2 ( wxCommandEvent &event ) ; ///< Spin control (max) event handler
    virtual void OnChoose ( wxCommandEvent &event ) ; ///< Choice event handler
    virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    virtual void OnACR ( wxCommandEvent &ev ) ; ///< Allow cut removal event handler
    virtual void OnLbDoubleClick ( wxCommandEvent &ev ) ; ///< Listbox double-click event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual wxString getSequence () ; ///< Returns the sequence
    virtual TRestrictionEnzyme *getEnzyme () ; ///< Returns a pointer to the enzyme

    private :
    wxString getAAresult ( wxString dna ) ; ///< Returns resulting amino acid sequence
    int selID () ; ///< Returns selected ID

    // Variables
    TVector *v ;
    int from , to ;
    vector <TSilmutItem> vs ;
    wxListBox *lb ;
    wxCheckBox *allow_cut_removal ;
    TPrimerDesign *pd ;
    wxStaticText *status ;
    wxSpinCtrl *lim_xhg , *lim_max ;
    wxChoice *egr , *mut ;
    wxArrayInt lb_retro ;
    int last_selection ;
    int mode ;
    int mut_pos , mut_dir ;
    bool running ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
