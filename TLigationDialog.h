/** \file
    \brief Contains the TLigationDialog class
*/
#ifndef _TLIGATIONDIALOG_H_
#define _TLIGATIONDIALOG_H_

#include "main.h"
#include <vector>
#include <wx/checklst.h>

using namespace std ;

class TVector ;
class MyFrame ;
class TStorage ;

/** \brief The ligation dialog class
*/
class TLigationDialog : public wxDialog
    {
    public :
    TLigationDialog(wxWindow *parent, const wxString& title ) ; ///< Constructor
    ~TLigationDialog () ; ///< Destructor

    virtual void init () ; ///< Initialize
    virtual void generateTargets () ; ///< Generate the target list
    virtual void curseTargets ( vector <bool> &vc , vector <bool> &used , wxArrayInt &vi ) ; ///< Recurse through targets
    virtual bool doMatch ( int l , int r , bool invertSecond = false ) ; ///< Do these match?
    virtual wxString getVIName ( const wxArrayInt &vi ) const ; ///< Generate a name
    virtual void addVTname ( const wxString& name , wxArrayInt &vi , const bool circular ) ; ///< Add this one

    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual void OnToggleSources ( wxCommandEvent &ev ) ; ///< Toggle source list event handler
    virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler

    wxCheckListBox *l_sources , *l_targets ;

    wxArrayTVector vv ;
    vector <TVector> ligates ;
    wxArrayString vt ;
    vector <bool> orientation ;
    bool doLigate ;

    private :
    virtual wxString invert ( const wxString& s ) const ; ///< Invert the string

    wxTextCtrl *message ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
