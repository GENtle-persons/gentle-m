#ifndef _PRIMER_DSEGIN_H_
#define _PRIMER_DSEGIN_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TPrimerDialog ;
class TPrimerDesign ;
class TPrimer ;

class TPrimerDesign : public ChildBase
    {
    public :
    TPrimerDesign(wxWindow *parent, 
                    wxString title,
                    TVector *_vec,
                    vector <TPrimer> pl ,
                    int _mut = -1 ) ;
    ~TPrimerDesign () ;
    
    void initme () ;
    virtual wxString getName () ;
    virtual void AddPrimer ( wxString s ) ;
    virtual void showSequence () ;
    virtual void updateResultSequence () ;
    virtual void updatePrimersFromSequence () ;
    virtual void updatePrimerStats () ;
    virtual void doShowPrimer ( int i ) ;

    virtual void OnCopy(wxCommandEvent& event);
//    virtual void OnClose(wxCloseEvent& event) ;
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
    
    virtual void OnSilmut ( wxCommandEvent& event) ;
    virtual void OnHorizontal ( wxCommandEvent& event ) ;
    virtual void OnPaste (wxCommandEvent& WXUNUSED(event)) ;
    
    // Variables
    SequenceCanvas *sc ;
    wxTextCtrl *stat ;
    wxPanel *up ;
    wxSplitterWindow *hs ;
    int from ;
    vector <TPrimer> primer ;
    TVector *vc , *w ;
    
    int aa_state , aa_disp ;
    int lastPrimerActivated ;
    wxListCtrl *lc ;
    int show_features ;
    int mut ;
    
    private :
    virtual void guessOptNuc () ;
    virtual void calculateResultSequence () ;
    wxSpinCtrl *spin ;
    bool spinTextEnabeled ;
    
    DECLARE_EVENT_TABLE()
    } ;



#endif

