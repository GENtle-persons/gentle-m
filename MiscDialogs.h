#ifndef _MISCDIALOGS_H_
#define _MISCDIALOGS_H_

#include "main.h"
#include "TLigationDialog.h"
#include "TVectorEditor.h"
#include "ManageDatabase.h"
#include "TextImportDialog.h"
#include "TPrimerDialog.h"
#include <wx/spinctrl.h>

class TStorage ;
class MyFrame ;
class MyChild ;
class TRestrictionEnzyme ;
class ChildBase ;
class TPrimer ;
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
    TSilmutDialog ( wxWindow *parent , const wxString &s ) ;
    virtual void initme ( TVector *vec , int _from , int _to ) ;
    virtual void calc () ;
    virtual void showit () ;
    virtual void OnSpin ( wxSpinEvent &event ) ;
    virtual void OnChoose ( wxCommandEvent &event ) ;
    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnLbDoubleClick ( wxCommandEvent &ev ) ;
    virtual string getSequence () ;
    virtual TRestrictionEnzyme *getEnzyme () ;
    
    private :
    string getAAresult ( string dna ) ;
    
    // Variables
    TVector *v ;
    int from , to ;
    vector <TSilmutItem> vs ;
    wxListBox *lb ;
    TPrimerDesign *pd ;
    wxSpinCtrl *lim_xhg , *lim_max ;
    wxChoice *egr ;

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
                *checkUpdate ;
    
    MyFrame *f ;

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

