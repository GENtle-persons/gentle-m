#ifndef _ABI_VIEWER_H_
#define _ABI_VIEWER_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;

typedef vector <int> tvi ;
typedef vector <char> tvc ;

class TABIviewer : public ChildBase
    {
    public :
    TABIviewer(wxWindow *parent, const wxString& title) ;
    virtual ~TABIviewer () ;
    
    virtual void initme () ;
    virtual string getName () ;
    virtual void showSequence () ;
    virtual string getStat () ;
    virtual void showStat () ;
    
    virtual void OnMarkAll(wxCommandEvent& event);
    virtual void OnFileSave(wxCommandEvent& event);
    virtual void OnExport(wxCommandEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnCopyToNew(wxCommandEvent& event);
    virtual void OnEditMode(wxCommandEvent& event);
    virtual void OnEditName(wxCommandEvent& event);
    virtual void OnSeqPrint(wxCommandEvent& event);
    virtual void OnHelplines(wxCommandEvent& event);
    virtual void OnInvCompl(wxCommandEvent& event);
    virtual void OnSpinWidth(wxSpinEvent& event);
    virtual void OnSpinHeight(wxSpinEvent& event);
    virtual void OnZoom(wxScrollEvent& event);
//    virtual void OnClose(wxCloseEvent& event) ;
    
    // Variables
    SequenceCanvas *sc ;
    wxTextCtrl *stat ;
    wxPanel *up ;
    wxSplitterWindow *hs ;
    int from ;
    string filename ;
    wxCheckBox *aidLines , *inv_compl ;
    wxSpinCtrl *f_height , *f_width ;
    wxSlider *slider ;
    
    DECLARE_EVENT_TABLE()
    } ;
    
#endif

