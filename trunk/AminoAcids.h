#ifndef _AMINO_ACIDS_H_
#define _AMINO_ACIDS_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TURLtext ;

typedef vector <int> tvi ;
typedef vector <char> tvc ;

class TAminoAcids : public ChildBase
    {
    public :
    TAminoAcids(MyFrame *parent, const wxString& title) ;
    virtual ~TAminoAcids () ;
    
    virtual void initme () ;
    virtual string getName () ;
    virtual void showSequence () ;
    virtual void showStat () ;
    
    virtual void OnMarkAll(wxCommandEvent& event);
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnExport(wxCommandEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnCut(wxCommandEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnEditMode(wxCommandEvent& event);
    virtual void OnEditName(wxCommandEvent& event);
    virtual void OnPrint(wxCommandEvent& event);
    virtual void OnAsNewFeature(wxCommandEvent& event);
    virtual void OnBlastAA(wxCommandEvent& event);
    virtual void invokeVectorEditor ( string what = "" , int num = 0 , bool forceUpdate = false ) ;
    virtual void OnClose(wxCloseEvent& event) ;
    virtual void OnHorizontal ( wxCommandEvent& event ) ;
    
    virtual void Undo(wxCommandEvent& event);
    virtual void Redo(wxCommandEvent& event);
    virtual void updateUndoMenu () ;
        
    // Variables
    SequenceCanvas *sc ;
    wxTextCtrl *stat ;
    TURLtext *desc ;
    wxSplitterWindow *hs , *vs ;
    int from ;
    
    DECLARE_EVENT_TABLE()
    } ;
    
#endif

