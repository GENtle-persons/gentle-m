#ifndef _MISCDIALOGS_H_
#define _MISCDIALOGS_H_

#include "main.h"
#include "TLigationDialog.h"
#include "TVectorEditor.h"
#include "ManageDatabase.h"
#include "TextImportDialog.h"
#include "TPrimerDialog.h"
#include "TSilmutDialog.h"
#include "ProgramOptionsDialog.h"
#include "FindSequenceDialog.h"
#include "TAlignmentDialog.h"
#include <wx/spinctrl.h>

class TStorage ;
class MyFrame ;
class MyChild ;
class TRestrictionEnzyme ;
class ChildBase ;
class TPrimer ;
class TPrimerDesign ;
class TAlignment ;

class TIPCDialog : public wxDialog
	{
    public : 
    TIPCDialog(wxWindow *parent, const wxString& title , int _seqlen ) ;
    virtual ~TIPCDialog () ;
    
    void OnLimit ( wxCommandEvent &event ) ;
//    void OnOK ( wxCommandEvent &ev ) ;
//    void OnCancel ( wxCommandEvent &ev ) ;

    private :
    int seqlen ;
    wxTextCtrl *limit ;
    wxStaticText *est ;
    
    double estimate_time ( int f ) ;
    
    DECLARE_EVENT_TABLE()
	} ;    

class TMutationDialog : public wxDialog
    {
    public : 
    TMutationDialog(wxWindow *parent, const wxString& title , wxString _codon ) ;
    virtual ~TMutationDialog () ;
    
    private :
    wxString codon , newcodon ;
    char aa , newaa ;
    TVector *v ;
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


class TSequencingPrimerDialog : public wxDialog
    {
    public :
    TSequencingPrimerDialog ( wxWindow *parent, const wxString& title ) ;
    ~TSequencingPrimerDialog () ;
    
    virtual void OnCharHook ( wxKeyEvent& event ) ;
    virtual void OnDB ( wxCommandEvent& event ) ;

    virtual void getPrimerList ( wxArrayString &p_name , wxArrayString &p_seq ) ;
    virtual bool matchToVector ( TVector *v , wxString name , wxString seq ) ;
    virtual bool getClear() { return cb_clear->GetValue() ; }
    
    private :
    virtual int findBestMatch ( wxString &s , wxString seq , int &pos , int ml ) ;
    virtual void addSequencingPrimer ( TVector *v , wxString name , wxString seq , 
    								int best_pos , int best_score , int dir ) ;

    wxSpinCtrl *t_ma ;
    wxChoice *c_db , *c_pj ;
    wxCheckBox *cb_pj , *cb_clear , *cb_35 , *cb_53 ;
    wxArrayString db_names , db_files ;
    
    DECLARE_EVENT_TABLE()
    } ;

class TransformSequenceDialog : public wxDialog
    {
    public :
    TransformSequenceDialog ( wxWindow *parent, const wxString& title ) ;
    ~TransformSequenceDialog () ;
    virtual void OnCharHook ( wxKeyEvent& event ) ;
    
    wxCheckBox *invert , *complement , *new_item ;

    DECLARE_EVENT_TABLE()
    } ;

#endif

