#ifndef _TSILMUTDIALOG_H_
#define _TSILMUTDIALOG_H_

#include "main.h"
#include <wx/spinctrl.h>

#define M_SILMUT 0
#define M_WHATCUTS 1

class TRestrictionEnzyme ;
class TPrimerDesign ;

class TSilmutItem
    {
    public :
    wxString mut ;
    int changes , cuts ;
    TRestrictionEnzyme *e ;
    wxArrayInt fragments ;
    } ;

class TSilmutDialog : public wxDialog
    {
    public :
    TSilmutDialog ( wxWindow *parent , const wxString &s , 
                    int _mode = M_SILMUT , 
                    int _mut_pos = -1 , int _mut_dir = 1 ) ;
    virtual void initme ( TVector *vec , int _from , int _to ) ;
    virtual void calc () ;
    virtual void showit () ;
    virtual void OnSpin ( wxSpinEvent &event ) ;
    virtual void OnSpin2 ( wxCommandEvent &event ) ;
    virtual void OnChoose ( wxCommandEvent &event ) ;
    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnLbDoubleClick ( wxCommandEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;
    virtual wxString getSequence () ;
    virtual TRestrictionEnzyme *getEnzyme () ;
    
    private :
    wxString getAAresult ( wxString dna ) ;
    int selID () ;
    
    // Variables
    TVector *v ;
    int from , to ;
    vector <TSilmutItem> vs ;
    wxListBox *lb ;
    TPrimerDesign *pd ;
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
