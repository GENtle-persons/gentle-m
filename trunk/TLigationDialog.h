#ifndef _TLIGATIONDIALOG_H_
#define _TLIGATIONDIALOG_H_

#include "main.h"
#include <vector>
#include <string>
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
    void curseTargets ( vector <bool> &vc , vector <bool> &used , vector <int> &vi ) ;
    bool doMatch ( int l , int r , bool invertSecond = false ) ;
    string getVIName ( vector <int> &vi ) ;
    void addVTname ( string name , vector <int> &vi , bool circular ) ;

    void OnToggleSources ( wxCommandEvent &ev ) ;
    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;

    wxCheckListBox *l_sources , *l_targets ;

    vector <TVector*> vv ;
    vector <TVector> ligates ;
    vector <string> vt ;
    vector <bool> orientation ;
    bool doLigate ;
    
    private :
    string invert ( string s ) ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
