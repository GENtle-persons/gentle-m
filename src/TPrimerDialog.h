/** \file
    \brief Contains the TPrimerDialog class
*/
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

/** \brief The dialog to optimize primers
*/
class TPrimerDialog : public wxDialog
    {
    public:
    TPrimerDialog(wxWindow *parent , const wxString& title ) ; ///< Constructor
    ~TPrimerDialog() ; ///< Destructor
    void initme ( TPrimer * const _p , const TPrimerDesign * const _pd ) ; ///< Initialize
    wxSpinCtrl *addSpinCtrl ( const int id , const wxString& title , const int x , const int y , const int def ) ; ///< Create a new wxSpinCtrl (this dialog has so many of them...)

    TPrimer *p , *cp ;

    private :
    void OnSearch ( wxCommandEvent &ev ) ; ///< "Search for primers button" event handler
    void OnReset ( wxCommandEvent &ev ) ; ///< "Reset button" event handler
    virtual void OnOK ( wxCommandEvent &ev ) ; ///< "OK button" event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< "Cancel button" event handler
    void OnActivatePrimer ( wxListEvent& event); ///< Primer activation event handler
    void OnSpinChanged ( wxSpinEvent &ev ) ; ///< "A spin control has changed" event handler
    void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    void updateList () ; ///< Update the potential primer list
    void ShowCur () ; ///< Refreshes the currently selected primer and its properties
    void ShowLC () ; ///< Refreshes the potential primer list

    private :
    const TPrimerDesign *pd ;
    vector <TPrimer> pl ; ///< The list of potential primers

    wxSpinCtrl *l3 , *l5 , *r3 , *r5 , *lmin , *lmax , *tmin , *tmax ;
    wxTextCtrl *seq , *rep ;
    wxListCtrl *lc ; ///< The list display of potential primers

    DECLARE_EVENT_TABLE()
    } ;

#endif
