#ifndef _TVECTOREDITOR_H_
#define _TVECTOREDITOR_H_

#include "main.h"
#include <wx/app.h>
#include <wx/colordlg.h>
#include <wx/checklst.h>

class TVector ;
class TVectorItem ;
class TEnzymeDialog ;
class TURLtext ;

class TVectorEditor : public wxDialog
    {
    public :
    TVectorEditor(wxWindow *parent, const wxString& title , TVector *_v ) ;
    ~TVectorEditor () ;
    
    void initialViewItem ( int num ) ;
    void initialViewEnzyme ( string e ) ;
    virtual void GetMyClientSize ( int *w , int *h , wxPanel *pan ) ;

    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;
    void SelChangeItems ( wxListEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;
    void addItem2list ( TVectorItem &i , int a ) ;
    
    void cleanup () ;
    void initPanProp () ;
    void initPanItem () ;
    void initPanEnzym () ;
    void initPanProt () ;
    void addOkCancel ( wxPanel *p ) ;
    void makeItemsList () ;
    void storeItemData () ;
    void hideEm () ;
    
    void commitVector () ;
    void commitItems () ;
    void commitEnzymes () ;
    void commitProteases () ;

    void showGroupEnzymes ( string gr ) ;
    void showEnzymeGroups () ;
    void enzymeSelChange ( wxEvent &ev ) ;
    void enzymeListDlbClick ( wxEvent &ev ) ;
    void enzymeAddEn ( wxEvent &ev ) ;
    void enzymeAddGr ( wxEvent &ev ) ;
    void enzymeAddToGr ( wxEvent &ev ) ;
    void enzymeAddToNewGr ( wxEvent &ev ) ;
    void enzymeDelGr ( wxEvent &ev ) ;
    void enzymeDelFromGr ( wxEvent &ev ) ;
    void enzymeDelEn ( wxEvent &ev ) ;
    void newEnzyme ( wxEvent &ev ) ;
    void importCloneEnzymes () ;
    void proteaseSelChange ( wxEvent &ev ) ;

    void newProtease ( wxEvent &ev ) ;
    void editProtease ( wxEvent &ev ) ;
    void showProteases () ;

    void itemAdd ( wxEvent &ev ) ;
    void itemDel ( wxEvent &ev ) ;
    void itemClr ( wxEvent &ev ) ;
    void itemCol ( wxEvent &ev ) ;
    void itemChoice ( wxEvent &ev ) ;

    wxListBox *listCE , *listGroups , *listGE ;
    vector <string> eig , ce ;
    
    TVector *v ;
    int bo , th ;
    int icur ;

    wxNotebook *nb ;
    wxPanel *panProp , *panItem , *panEnzym , *panProt ;
    wxButton *bClear , *bAdd , *bDel , *bCol , *b_atg , *b_dfg , *b_dg ;
    wxTextCtrl *name , *lu , *ll , *ru , *rl ;
    TURLtext *desc , *idesc , *pro_txt ;
    wxTextCtrl *iname , *ifrom , *ito ;
    wxListCtrl *items ;
    wxRadioBox *irb ;
    wxChoice *ichoice ;
    wxCheckBox *icb , *icv ;
    wxCheckListBox *prots ;
    bool hideProp , hideItem , hideEnzym ;
    
    vector <TVectorItem*> newitems ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif
