#ifndef _PRIMER_DSEGIN_H_
#define _PRIMER_DSEGIN_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TPrimerDialog ;
class TPrimerDesign ;
class TPrimer ;

typedef vector <int> tvi ;
typedef vector <char> tvc ;

class TPrimerDesign : public ChildBase
    {
    public :
    TPrimerDesign(wxMDIParentFrame *parent, 
                    wxString title,
                    TVector *_vec,
                    vector <TPrimer> pl ) ;
    ~TPrimerDesign () ;
    
    void initme ( MyApp *_app ) ;
    virtual string getName () ;
    void AddPrimer ( string s ) ;
    void showSequence () ;
    void updateResultSequence () ;
    void updatePrimersFromSequence () ;
    void updatePrimerStats () ;
    void doShowPrimer ( int i ) ;
/*    void showStat () ;
    
    void OnMarkAll(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnEditMode(wxCommandEvent& event);
    void OnEditName(wxCommandEvent& event);
    */
    void OnCopy(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event) ;
    void OnEditMode(wxCommandEvent& event);
    
    void OnSelectPrimer ( wxListEvent& event);
    void OnActivatePrimer ( wxListEvent& event);
    void OnPrint ( wxCommandEvent &ev ) ;

    void OnExportPrimer ( wxCommandEvent &ev ) ;
    void OnImportPrimer ( wxCommandEvent &ev ) ;

    void OnEditPrimer ( wxCommandEvent &ev ) ;
    void OnDeletePrimer ( wxCommandEvent &ev ) ;
    
    void OnAA_all(wxCommandEvent& event);
    void OnAA_three(wxCommandEvent& event);
    void OnAA_one(wxCommandEvent& event);
    void OnAA_three_1(wxCommandEvent& event);
    void OnAA_three_2(wxCommandEvent& event);
    void OnAA_three_3(wxCommandEvent& event);
    void OnAA_three_M1(wxCommandEvent& event);
    void OnAA_three_M2(wxCommandEvent& event);
    void OnAA_three_M3(wxCommandEvent& event);
    void OnAA_setit(int mode);
    
    // Variables
    SequenceCanvas *sc ;
    wxTextCtrl *stat ;
    wxPanel *up ;
    wxSplitterWindow *hs ;
    MyApp *app ;
    int from ;
    vector <TPrimer> primer ;
    TVector *vc , *w ;
    
    int aa_state , aa_disp ;
    int lastPrimerActivated ;
    wxListCtrl *lc ;
    
    DECLARE_EVENT_TABLE()
    } ;



#endif

