/** \file
	\brief Contains the TPrimerDesign module class
*/
#ifndef _PRIMER_DSEGIN_H_
#define _PRIMER_DSEGIN_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TPrimerDialog ;
class TPrimerDesign ;
class TPrimer ;

/**	\brief The virtual PCR/primer design module
*/
class TPrimerDesign : public ChildBase
    {
    public :
    TPrimerDesign(wxWindow *parent, 
                    wxString title,
                    TVector *_vec,
                    vector <TPrimer> pl ,
                    int _mut = -1 ) ; ///< Constructor
    ~TPrimerDesign () ; ///< Destructor
    
    void initme () ; ///< Initialize
    virtual wxString getName () ; ///< Returns the name of the vector
    virtual void AddPrimer ( wxString s , wxString pname = _T("") ) ; ///< Add a primer to the module
    virtual void showSequence () ; ///< Refresh the sequence map
    virtual void updateResultSequence () ; ///< Generates the resulting DNA and amino acid sequences from the template sequence and the primers
    virtual void updatePrimersFromSequence () ; ///< Updates the primers from the primer sequences in the map (after editing)
    virtual void updatePrimerStats () ; ///< Update primer detail display
    virtual void doShowPrimer ( int i ) ; ///< Mark and scroll to a primer in the sequence map

    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnEditMode(wxCommandEvent& event);
    
    virtual void OnSelectPrimer ( wxListEvent& event);
    virtual void OnActivatePrimer ( wxListEvent& event);
    virtual void OnPrint ( wxCommandEvent &ev ) ;

    virtual void OnExportPrimer ( wxCommandEvent &ev ) ;
    virtual void OnImportPrimer ( wxCommandEvent &ev ) ;
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){};

    virtual void OnEditPrimer ( wxCommandEvent &ev ) ;
    virtual void OnDeletePrimer ( wxCommandEvent &ev ) ;
    virtual void OnToggleFeatures ( wxCommandEvent &ev ) ;
    virtual void OnSpin(wxSpinEvent& event);
    virtual void OnSpinText(wxCommandEvent& event);
    
    virtual void OnAA_all(wxCommandEvent& event);
    virtual void OnAA_three(wxCommandEvent& event);
    virtual void OnAA_one(wxCommandEvent& event);
    virtual void OnAA_three_1(wxCommandEvent& event);
    virtual void OnAA_three_2(wxCommandEvent& event);
    virtual void OnAA_three_3(wxCommandEvent& event);
    virtual void OnAA_three_M1(wxCommandEvent& event);
    virtual void OnAA_three_M2(wxCommandEvent& event);
    virtual void OnAA_three_M3(wxCommandEvent& event);
    virtual void OnAA_known(wxCommandEvent& event);
    virtual void OnAA_setit(int mode);
    virtual void OnInsertRestrictionSiteLeft ( wxCommandEvent &ev ) ;
    virtual void OnInsertRestrictionSiteRight ( wxCommandEvent &ev ) ;
    
    virtual void OnSilmut ( wxCommandEvent& event) ;
    virtual void OnHorizontal ( wxCommandEvent& event ) ;
    virtual void OnPaste (wxCommandEvent& WXUNUSED(event)) ;
    
    // Variables
    SequenceCanvas *sc ;
    int from ;
    vector <TPrimer> primer ;
    TVector *vc , *w ;
    
    int aa_state , aa_disp ;
    int lastPrimerActivated ;
    int show_features ;
    int mut ;
    
    private :
    virtual void guessOptNuc () ;
    virtual void calculateResultSequence () ;
    wxSpinCtrl *spin ;
    wxTextCtrl *stat ;
    wxBoxSizer *v0 ;
    wxListCtrl *lc ;
    bool spinTextEnabeled ;
    bool updating ;
    
    DECLARE_EVENT_TABLE()
    } ;



#endif

