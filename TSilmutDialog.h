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
    string mut ;
    int changes , cuts ;
    TRestrictionEnzyme *e ;
    vector <int> fragments ;
    } ;

class TSilmutDialog : public wxDialog
    {
    public :
    TSilmutDialog ( wxWindow *parent , const wxString &s , int _mode = M_SILMUT ) ;
    virtual void initme ( TVector *vec , int _from , int _to ) ;
    virtual void calc () ;
    virtual void showit () ;
    virtual void OnSpin ( wxSpinEvent &event ) ;
    virtual void OnChoose ( wxCommandEvent &event ) ;
    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnLbDoubleClick ( wxCommandEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;
    virtual string getSequence () ;
    virtual TRestrictionEnzyme *getEnzyme () ;
    
    private :
    string getAAresult ( string dna ) ;
    int selID () ;
    
    // Variables
    TVector *v ;
    int from , to ;
    vector <TSilmutItem> vs ;
    wxListBox *lb ;
    TPrimerDesign *pd ;
    wxSpinCtrl *lim_xhg , *lim_max ;
    wxChoice *egr ;
    vector <int> lb_retro ;
    int last_selection ;
    int mode ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
