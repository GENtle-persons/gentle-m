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
    TABIviewer(MyFrame *parent, const wxString& title) ;
    ~TABIviewer () ;
    
    void initme () ;
    virtual string getName () ;
    void showSequence () ;
    string getStat () ;
    void showStat () ;
    
    void OnMarkAll(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnCopyToNew(wxCommandEvent& event);
    void OnEditMode(wxCommandEvent& event);
    void OnEditName(wxCommandEvent& event);
    
    void OnHelplines(wxCommandEvent& event);
    void OnSpinWidth(wxSpinEvent& event);
    void OnSpinHeight(wxSpinEvent& event);
    void OnZoom(wxScrollEvent& event);

    void OnClose(wxCloseEvent& event) ;
    
    // Variables
    SequenceCanvas *sc ;
    wxTextCtrl *stat ;
    wxPanel *up ;
    wxSplitterWindow *hs ;
    int from ;
    string filename ;
    wxCheckBox *aidLines ;
    wxSpinCtrl *f_height , *f_width ;
    wxSlider *slider ;
    
    DECLARE_EVENT_TABLE()
    } ;
    
#endif

