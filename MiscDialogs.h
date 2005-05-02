/** \file
	\brief Contains several (minor) dialogs and their helper classes
*/
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

/**	\brief The dialog handling the TIPC settings
*/
class TIPCDialog : public wxDialog
	{
    public : 
    TIPCDialog(wxWindow *parent, const wxString& title , int _seqlen ) ; ///< Constructor
    virtual ~TIPCDialog () ; ///< Destructor
    
    void OnLimit ( wxCommandEvent &event ) ; ///< Limit update event handler
//    void OnOK ( wxCommandEvent &ev ) ;
//    void OnCancel ( wxCommandEvent &ev ) ;

    private :
    int seqlen ; ///< The sequence length
    wxTextCtrl *limit ; ///< Pointer to the text box containing the limit
    wxStaticText *est ; ///< Pointer to the time estimation display
    
    double estimate_time ( int f ) ; ///< Function estimating runtime
    
    DECLARE_EVENT_TABLE()
	} ;    

/*	\brief Apparently unused
*/
/*
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
*/

/** \brief A text control highlighting URLs
*/
class TURLtext : public wxTextCtrl
    {
    public :
    TURLtext ( wxWindow *parent , int id , wxString title ,
                    wxPoint pos , wxSize size , int style ) ; ///< Constructor
    virtual void OnURL(wxTextUrlEvent& event) ; ///< URL event handler
    DECLARE_EVENT_TABLE()
    } ;
/*
class TStandbyDialog : public wxDialog
    {
    public :
    TStandbyDialog ( wxWindow *parent , const wxString &s , const wxString &t = _T("") ) ;
    } ;
*/

/** \brief A dialog for multiple choices
*/
class TMyMultipleChoiceDialog : public wxDialog
    {
    public :
    TMyMultipleChoiceDialog () ; ///< Default constructor
    TMyMultipleChoiceDialog ( wxWindow *parent ,
                              const wxString &message ,
                              const wxString &caption ,
                              int n,
                              const wxString choices[] = NULL,
                              int options = wxOK | wxCANCEL | wxCENTRE ,
                              const wxPoint& pos = wxDefaultPosition ) ; ///< Constructor
    virtual ~TMyMultipleChoiceDialog () ; ///< Destructor
    virtual void Check ( int item, bool check = TRUE ) ; ///< Check an item
    virtual void CheckAll ( bool check = TRUE ) ; ///< Uncheck all items
    virtual bool IsChecked ( int item ) const ; ///< Is an item checked?
    virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    private :
    wxCheckListBox *clb ; ///< The checklist-box
    vector <bool> check ;

    DECLARE_EVENT_TABLE()
    } ;

/** \brief A dialog to display/edit enzyme properties
*/
class TEnzymeDialog : public wxDialog
    {
    public :
    TEnzymeDialog(wxWindow *parent, const wxString& title, const wxPoint& pos , 
                        const wxSize& size , const long style ) ; ///< Constructor
    void initme ( TRestrictionEnzyme *_e = NULL , bool ro = false ) ; ///< Initialization
    void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    void OnCharHook(wxKeyEvent& event) ; ///< Cancel button event handler
    
    // Variables
    bool readonly ; ///< Protect dialog from user changes
    TRestrictionEnzyme *e ; ///< The enzyme in question
    wxTextCtrl *eb_name , *eb_seq, *eb_loc , *eb_note , *eb_from , *eb_to ;

    DECLARE_EVENT_TABLE()
    } ;

/** \brief The dialog for settings when searching for sequencing primers
*/
class TSequencingPrimerDialog : public wxDialog
    {
    public :
    TSequencingPrimerDialog ( wxWindow *parent, const wxString& title ) ; ///< Constructor
    ~TSequencingPrimerDialog () ; ///< Destructor
    
    virtual void OnCharHook ( wxKeyEvent& event ) ; ///< Key event handler
    virtual void OnDB ( wxCommandEvent& event ) ; ///< Second database event handler

    virtual void getPrimerList ( wxArrayString &p_name , wxArrayString &p_seq ) ; ///< Retrieve a list of primers
    virtual bool matchToVector ( TVector *v , wxString name , wxString seq ) ; ///< Match the primers to the vector
    virtual bool getClear() { return cb_clear->GetValue() ; } ///< Clear existing sequencing primer items from vecotr?
    
    private :
    virtual int findBestMatch ( wxString &s , wxString seq , int &pos , int ml ) ; ///< Finds the best match for a primer in the vector
    virtual void addSequencingPrimer ( TVector *v , wxString name , wxString seq , 
    								int best_pos , int best_score , int dir ) ; ///< Add the primer as an item to the vector

    wxSpinCtrl *t_ma ; ///< Minimum number of aligning bases
    wxChoice *c_db , *c_pj ;
    wxCheckBox *cb_pj , *cb_clear , *cb_35 , *cb_53 ;
    wxArrayString db_names , db_files ;
    
    DECLARE_EVENT_TABLE()
    } ;

/** \brief Tiny dialog for transforming a DNA sequence
*/
class TransformSequenceDialog : public wxDialog
    {
    public :
    TransformSequenceDialog ( wxWindow *parent, const wxString& title ) ; ///< Constructor
    ~TransformSequenceDialog () ; ///< Destructor
    virtual void OnCharHook ( wxKeyEvent& event ) ; ///< Key event handler
    
    wxCheckBox *invert , *complement , *new_item ;

    DECLARE_EVENT_TABLE()
    } ;

#endif

