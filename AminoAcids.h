#ifndef _AMINO_ACIDS_H_
#define _AMINO_ACIDS_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TURLtext ;

class TAminoAcids : public ChildBase
    {
    public :
    TAminoAcids(wxWindow *parent, const wxString& title) ;
    virtual ~TAminoAcids () ;
    
    virtual void initme () ;
    virtual wxString getName () ;
    virtual void showSequence () ;
    virtual void showStat () ;
    
    virtual void OnMarkAll(wxCommandEvent& event);
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnCut(wxCommandEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnEditMode(wxCommandEvent& event);
    virtual void OnEditName(wxCommandEvent& event);
    virtual void OnPrint(wxCommandEvent& event);
    virtual void OnAsNewFeature(wxCommandEvent& event);
    virtual void OnBlastAA(wxCommandEvent& event);
    virtual void OnPhotometer(wxCommandEvent& event);
    virtual void invokeVectorEditor ( wxString what = "" , int num = 0 , bool forceUpdate = false ) ;
//    virtual void OnClose(wxCloseEvent& event) ;
    virtual void OnHorizontal ( wxCommandEvent& event ) ;
    virtual void OnListBox ( wxCommandEvent& event ) ;
    virtual void OnIP ( wxCommandEvent& event ) ;
    virtual void OnBacktranslate ( wxCommandEvent& event ) ;
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){};
    
    virtual void Undo(wxCommandEvent& event);
    virtual void Redo(wxCommandEvent& event);
    virtual void updateUndoMenu () ;
        
    // Variables
    SequenceCanvas *sc , *sc2 ;
    wxTextCtrl *stat ;
    TURLtext *desc ;
    wxSplitterWindow *hs , *vs ;
    int from ;
    int miniDisplayOptions ;
    
    wxWindow *curDisplay ;
    PlasmidCanvas *pc ;

    protected :
    virtual void updateToolbar () ;

    private :
    wxListBox *lb ;
    wxBoxSizer *h1 , *v1 ;
    wxChoice *inlinePlot ;
    wxString lastLBsel ;

    DECLARE_EVENT_TABLE()
    } ;
    
#endif

