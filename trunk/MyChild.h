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
    
    virtual void initme () ;
    virtual void initPanels () ;
    virtual string getName () ;
    virtual void updateSequenceCanvas ( bool remember = false ) ;
    virtual string doExtractAA ( bool coding = true ) ;
            
    
    MyChild(MyFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    ~MyChild();

    virtual void OnActivate(wxActivateEvent& event);

    virtual void OnAA_none(wxCommandEvent& event);
    virtual void OnAA_known(wxCommandEvent& event);
    virtual void OnAA_all(wxCommandEvent& event);
    virtual void OnAA_three(wxCommandEvent& event);
    virtual void OnAA_one(wxCommandEvent& event);
    virtual void OnAA_three_1(wxCommandEvent& event);
    virtual void OnAA_three_2(wxCommandEvent& event);
    virtual void OnAA_three_3(wxCommandEvent& event);
    virtual void OnAA_three_M1(wxCommandEvent& event);
    virtual void OnAA_three_M2(wxCommandEvent& event);
    virtual void OnAA_three_M3(wxCommandEvent& event);
    virtual void OnAA_setit(int mode);

    virtual void Undo(wxCommandEvent& event);
    virtual void Redo(wxCommandEvent& event);
    virtual void updateUndoMenu () ;
    
    virtual void OnMarkAll(wxCommandEvent& event);
    virtual void OnTransformSequence(wxCommandEvent& event);
    virtual void OnExtractAA(wxCommandEvent& event);
    virtual void OnExport(wxCommandEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnCut(wxCommandEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnPaste(wxCommandEvent& event);
    virtual void OnViewMode(wxCommandEvent& event);
    virtual void OnEditMode(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnCircularLinear(wxCommandEvent& event);
    virtual void OnLigation(wxCommandEvent& event);
    virtual void OnCopyToNew(wxCommandEvent& event);
    virtual void OnAsNewFeature(wxCommandEvent& event);
    virtual void OnPrintImage(wxCommandEvent& event);
    virtual void OnRestriction(wxCommandEvent& event);
    virtual void OnHelp(wxCommandEvent& event);
    virtual void OnORFs(wxCommandEvent& event);
    virtual void OnEditORFs(wxCommandEvent& event);
    virtual void OnSeqPrint(wxCommandEvent& event);
    virtual void OnPrintReport(wxCommandEvent& event);
    virtual void OnToggleFeatures(wxCommandEvent& event);
    virtual void OnToggleRestriction(wxCommandEvent& event);
    virtual void OnRunPCR(wxCommandEvent& event);
    virtual void OnZoom ( wxCommandEvent &ev ) ;
    virtual void OnUpdateRefresh(wxUpdateUIEvent& event);
    
    int ly ;
    
    DECLARE_EVENT_TABLE()
};

#endif

