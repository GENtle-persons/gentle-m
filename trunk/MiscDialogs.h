#ifndef _MISCDIALOGS_H_
#define _MISCDIALOGS_H_

#include "main.h"
#include "TLigationDialog.h"
#include "TVectorEditor.h"
#include "ManageDatabase.h"
#include "TextImportDialog.h"
#include "TPrimerDialog.h"
#include "TSilmutDialog.h"
#include <wx/spinctrl.h>

class TStorage ;
class MyFrame ;
class MyChild ;
class TRestrictionEnzyme ;
class ChildBase ;
class TPrimer ;
class TPrimerDesign ;
class TAlignment ;

class TMutationDialog : public wxDialog
    {
    TMutationDialog(wxWindow *parent, const wxString& title , string _codon ) ;
    virtual ~TMutationDialog () ;
    
    string codon , newcodon ;
    char aa , newaa ;
    TVector *v ;
    } ;

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
    
    vector <TVector*> vav , vcv ;
    vector <string> van , vcn ;

    DECLARE_EVENT_TABLE()
    } ;

class TURLtext : public wxTextCtrl
    {
    public :
    TURLtext ( wxWindow *parent , int id , wxString title ,
                    wxPoint pos , wxSize size , int style ) ;
    virtual void OnURL(wxTextUrlEvent& event) ;
    DECLARE_EVENT_TABLE()
    } ;

class TStandbyDialog : public wxDialog
    {
    public :
    TStandbyDialog ( wxWindow *parent , const wxString &s , const wxString &t = "" ) ;
    } ;
    
class TMyMultipleChoiceDialog : public wxDialog
    {
    public :
    TMyMultipleChoiceDialog () ;
    TMyMultipleChoiceDialog ( wxWindow *parent ,
                              const wxString &message ,
                              const wxString &caption ,
                              int n,
                              const wxString choices[] = NULL,
                              int options = wxOK | wxCANCEL | wxCENTRE ,
                              const wxPoint& pos = wxDefaultPosition ) ;
    virtual ~TMyMultipleChoiceDialog () ;
    virtual void Check ( int item, bool check = TRUE ) ;
    virtual void CheckAll ( bool check = TRUE ) ;
    virtual bool IsChecked ( int item ) const ;
    virtual void OnOK ( wxCommandEvent &ev ) ;
    private :
    wxCheckListBox *clb ;
    vector <bool> check ;

    DECLARE_EVENT_TABLE()
    } ;

class TEnzymeDialog : public wxDialog
    {
    public :
    TEnzymeDialog(wxWindow *parent, const wxString& title, const wxPoint& pos , 
                        const wxSize& size , const long style ) ;
    void initme ( TRestrictionEnzyme *_e = NULL , bool ro = false ) ;
    void OnOK ( wxCommandEvent &ev ) ;
    void OnCharHook(wxKeyEvent& event) ;
    
    // Variables
    bool readonly ;
    TRestrictionEnzyme *e ;
    wxTextCtrl *eb_name , *eb_seq, *eb_loc , *eb_note , *eb_from , *eb_to ;

    DECLARE_EVENT_TABLE()
    } ;

        
class ProgramOptionsDialog : public wxDialog
    {
    public :
    ProgramOptionsDialog(wxWindow *parent, const wxString& title ) ;

    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;
    void OnCharHook(wxKeyEvent& event) ;
    
    wxChoice *language ;
    wxCheckBox *enhancedDisplay , *vectorTitle , *vectorLength ,
                *loadLastProject , *useMetafile , *showSplashScreen ,
                *checkUpdate , *useInternalHelp ;
    
    DECLARE_EVENT_TABLE()
    } ;

class FindSequenceDialog : public wxDialog
    {
    public :
    FindSequenceDialog ( wxWindow *parent, const wxString& title ) ;
    void OnCharHook(wxKeyEvent& event) ;
    void OnSearch ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;

    string allowed_chars ;

    private :
    bool doesMatch ( char a , char b ) ;
    int subsearch ( const string &s , const string &sub , int start ) ;
    
    ChildBase *c ;
    wxTextCtrl *t ;
    int bo , fh ;
    int p , last ;

    DECLARE_EVENT_TABLE()
    } ;

class TransformSequenceDialog : public wxDialog
    {
    public :
    TransformSequenceDialog ( wxWindow *parent, const wxString& title ) ;
    ~TransformSequenceDialog () ;
    
    wxCheckBox *invert , *complement , *new_item ;

    DECLARE_EVENT_TABLE()
    } ;

#endif

