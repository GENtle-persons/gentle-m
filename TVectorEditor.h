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
    virtual ~TVectorEditor () ;
    
    virtual void initialViewItem ( int num ) ;
    virtual void initialViewEnzyme ( wxString e ) ;
    virtual void GetMyClientSize ( int *w , int *h , wxPanel *pan ) ;

    virtual void OnOK ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnCancel ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void SelChangeItems ( wxListEvent &ev ) ;
    virtual void DeselItems ( wxListEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;
    virtual void addItem2list ( TVectorItem &i , int a ) ;
    
    virtual void cleanup () ;
    virtual void initPanProp () ;
    virtual void initPanItem () ;
    virtual void initPanEnzym () ;
    virtual void initPanProt () ;
    virtual void addOkCancel ( wxPanel *p ) ;
    virtual void makeItemsList () ;
    virtual void storeItemData ( int i = -1 ) ;
    virtual void hideEm () ;
    
    virtual void commitVector () ;
    virtual void commitItems () ;
    virtual void commitEnzymes () ;
    virtual void commitProteases () ;

    virtual void showGroupEnzymes ( wxString gr ) ;
    virtual void showEnzymeGroups () ;
    virtual void enzymeSelChange ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeListDlbClick ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeAddEn ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeAddGr ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeAddToGr ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeAddToNewGr ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeDelGr ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeDelFromGr ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void enzymeDelEn ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void newEnzyme ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void importCloneEnzymes ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void proteaseSelChange ( wxCommandEvent& WXUNUSED(event) ) ;

    void newProtease ( wxCommandEvent& WXUNUSED(event) ) ;
    void editProtease ( wxCommandEvent& WXUNUSED(event) ) ;
    void showProteases () ;

    void itemAdd ( wxCommandEvent& WXUNUSED(event) ) ;
    void itemDel ( wxCommandEvent& WXUNUSED(event) ) ;
    void itemClr () ;
    void itemCol ( wxListEvent &ev ) ;
    void itemCol2 ( wxCommandEvent& WXUNUSED(event) ) ;
    void itemChoice ( wxCommandEvent& WXUNUSED(event) ) ;

    bool hideProp , hideItem , hideEnzym ;

    private :
    virtual int getCurrentItem () ;
    virtual void clearItemSelection () ;

        
    wxListBox *listCE , *listGroups , *listGE ;
    wxArrayString eig , ce ;
    
    TVector *v ;
    int bo , th ;

    wxNotebook *nb ;
    wxPanel *panProp , *panItem , *panEnzym , *panProt ;
    wxButton *bAdd , *bDel , *bCol , *b_atg , *b_dfg , *b_dg ;
    wxTextCtrl *name , *lu , *ll , *ru , *rl ;
    TURLtext *desc , *idesc , *pro_txt ;
    wxTextCtrl *iname , *ifrom , *ito ;
    wxListCtrl *items ;
    wxRadioBox *irb ;
    wxChoice *ichoice ;
    wxCheckBox *icb , *icv ;
    wxCheckListBox *prots ;
    
    wxArrayTVectorItem newitems ;
    int lastSelection ;
    
    virtual void loadItemData ( int i ) ;
    virtual void updateItem ( TVectorItem &i ) ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif
