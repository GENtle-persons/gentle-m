#ifndef _T_PRIMER_DIALOG_
#define _T_PRIMER_DIALOG_

#include "main.h"
#include <wx/spinctrl.h>

class TStorage ;
class MyFrame ;
class MyChild ;
class TRestrictionEnzyme ;
class ChildBase ;
class TPrimer ;
class TPrimerDesign ;

class TPrimerDialog : public wxDialog
    {
    public:
    TPrimerDialog(wxWindow *parent , const wxString& title ) ;
    ~TPrimerDialog() ;
    void initme ( TPrimer *_p , TPrimerDesign *_pd ) ;
    wxSpinCtrl *addSpinCtrl ( int id , wxString title , int x , int y , int def ) ;

    TPrimer *p , *cp ;

    private :
    void OnSearch ( wxCommandEvent &ev ) ;
    void OnReset ( wxCommandEvent &ev ) ;
    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    void OnActivatePrimer ( wxListEvent& event);
    void OnSpinChanged ( wxSpinEvent &ev ) ;
    void OnCharHook(wxKeyEvent& event) ;
    
    void updateList () ;
    void ShowCur () ;
    void ShowLC () ;

    private :
    TPrimerDesign *pd ;
    vector <TPrimer> pl ;
    
    wxSpinCtrl *l3 , *l5 , *r3 , *r5 , *lmin , *lmax , *tmin , *tmax ;
    wxTextCtrl *seq , *rep ;
    wxListCtrl *lc ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
