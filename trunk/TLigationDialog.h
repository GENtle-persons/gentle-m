#ifndef _TLIGATIONDIALOG_H_
#define _TLIGATIONDIALOG_H_

#include "main.h"
#include <vector>
#include <wx/checklst.h>

using namespace std ;

class TVector ;
class MyFrame ;
class TStorage ;

class TLigationDialog : public wxDialog
    {
    public :
    TLigationDialog(wxWindow *parent, const wxString& title ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;
    void init () ;
    void generateTargets () ;
    void curseTargets ( vector <bool> &vc , vector <bool> &used , wxArrayInt &vi ) ;
    bool doMatch ( int l , int r , bool invertSecond = false ) ;
    wxString getVIName ( wxArrayInt &vi ) ;
    void addVTname ( wxString name , wxArrayInt &vi , bool circular ) ;

    void OnToggleSources ( wxCommandEvent &ev ) ;
    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;

    wxCheckListBox *l_sources , *l_targets ;

    wxArrayTVector vv ;
    vector <TVector> ligates ;
    wxArrayString vt ;
    vector <bool> orientation ;
    bool doLigate ;
    
    private :
    wxString invert ( wxString s ) ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
