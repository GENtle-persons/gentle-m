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
    TAminoAcids(wxMDIParentFrame *parent, const wxString& title) ;
    ~TAminoAcids () ;
    
    void initme ( MyApp *_app ) ;
    virtual string getName () ;
    void showSequence () ;
    void showStat () ;
    
    void OnMarkAll(wxCommandEvent& event);
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) ;
    void OnExport(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnEditMode(wxCommandEvent& event);
    void OnEditName(wxCommandEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnAsNewFeature(wxCommandEvent& event);
    void OnBlastAA(wxCommandEvent& event);
    void invokeVectorEditor ( string what = "" , int num = 0 , bool forceUpdate = false ) ;
    
    void OnClose(wxCloseEvent& event) ;
    
    // Variables
    SequenceCanvas *sc ;
    wxTextCtrl *stat ;
    TURLtext *desc ;
    wxSplitterWindow *hs , *vs ;
    MyApp *app ;
    int from ;
    
    DECLARE_EVENT_TABLE()
    } ;
    
#endif

