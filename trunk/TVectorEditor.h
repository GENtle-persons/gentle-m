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
    void initialViewEnzyme ( wxString e ) ;
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

    void showGroupEnzymes ( wxString gr ) ;
    void showEnzymeGroups () ;
    void enzymeSelChange ( wxCommandEvent &ev ) ;
    void enzymeListDlbClick ( wxCommandEvent &ev ) ;
    void enzymeAddEn ( wxCommandEvent &ev ) ;
    void enzymeAddGr ( wxCommandEvent &ev ) ;
    void enzymeAddToGr ( wxCommandEvent &ev ) ;
    void enzymeAddToNewGr ( wxCommandEvent &ev ) ;
    void enzymeDelGr ( wxCommandEvent &ev ) ;
    void enzymeDelFromGr ( wxCommandEvent &ev ) ;
    void enzymeDelEn ( wxCommandEvent &ev ) ;
    void newEnzyme ( wxCommandEvent &ev ) ;
    void importCloneEnzymes ( wxCommandEvent &ev ) ;
    void proteaseSelChange ( wxCommandEvent &ev ) ;

    void newProtease ( wxCommandEvent &ev ) ;
    void editProtease ( wxCommandEvent &ev ) ;
    void showProteases () ;

    void itemAdd ( wxCommandEvent &ev ) ;
    void itemDel ( wxCommandEvent &ev ) ;
    void itemClr ( wxCommandEvent &ev ) ;
    void itemCol ( wxListEvent &ev ) ;
    void itemCol2 ( wxCommandEvent &ev ) ;
    void itemChoice ( wxCommandEvent &ev ) ;

    bool hideProp , hideItem , hideEnzym ;

    private :
    wxListBox *listCE , *listGroups , *listGE ;
    wxArrayString eig , ce ;
    
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
    
    vector <TVectorItem*> newitems ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif
