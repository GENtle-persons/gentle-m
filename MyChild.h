#ifndef _MYCHILD_H_
#define _MYCHILD_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/clipbrd.h>
#include <wx/splitter.h>

class PlasmidCanvas ;
class SequenceCanvas ;
class TVector ;
class TVectorTree ;
class TMainTree ;
class TMainTreeItem ;
class MyChild ;
class TURLtext ;

class MySplitter : public wxSplitterWindow
    {
    public :
    MySplitter ( wxWindow *win , int id , MyChild *child ) ;
    void OnChanged ( wxSplitterEvent &ev ) ;
    MyChild *c ;

    DECLARE_EVENT_TABLE()
    } ;

class MyChild: public ChildBase
{
public:
    TVectorTree *treeBox ;
    TURLtext *propBox ;
    PlasmidCanvas *cPlasmid;
    SequenceCanvas *cSequence;
    
    MySplitter *sw , *swu , *swl ;
    int sp1 , sp2 ;
    int aa_state , aa_disp , orf_mode ;
    bool showORFs , viewMode ;
    wxChoice *zoom_cb ;
    int aa_offset ;
    
    void initme () ;
    void initPanels () ;
    virtual string getName () ;
    void updateSequenceCanvas ( bool remember = false ) ;
    string doExtractAA ( bool coding = true ) ;
            
    
    MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    ~MyChild();

    void OnActivate(wxActivateEvent& event);

    void OnAA_none(wxCommandEvent& event);
    void OnAA_known(wxCommandEvent& event);
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
    
    void OnMarkAll(wxCommandEvent& event);
    void OnTransformSequence(wxCommandEvent& event);
    void OnExtractAA(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) ;
    void OnQuit(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnViewMode(wxCommandEvent& event);
    void OnEditMode(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnCircularLinear(wxCommandEvent& event);
    void OnLigation(wxCommandEvent& event);
    void OnCopyToNew(wxCommandEvent& event);
    void OnAsNewFeature(wxCommandEvent& event);
    void OnPrintImage(wxCommandEvent& event);
    void OnRestriction(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnORFs(wxCommandEvent& event);
    void OnEditORFs(wxCommandEvent& event);
    void OnSeqPrint(wxCommandEvent& event);
    void OnPrintReport(wxCommandEvent& event);
    void OnToggleFeatures(wxCommandEvent& event);
    void OnToggleRestriction(wxCommandEvent& event);
    void OnRunPCR(wxCommandEvent& event);

    void OnZoom ( wxCommandEvent &ev ) ;

    void OnUpdateRefresh(wxUpdateUIEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif

