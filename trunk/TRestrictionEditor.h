#ifndef _TRESTRICTIONEDITOR_H_
#define _TRESTRICTIONEDITOR_H_

#include "main.h"
#include <wx/spinctrl.h>

class wxNotebook ;
class TRestrictionEnzyme ;


class TREcache
    {
    public :
    TREcache ( wxString a , int b ) { enzyme = a ; cut = b ; }
    wxString enzyme ;
    int cut ;
    } ;

class TRestrictionEditor : public wxDialog
    {
    public :
    TRestrictionEditor(MyFrame *parent, const wxString& title, const wxPoint& pos, 
                       const wxSize& size, const long style);

    virtual void initme ( TVector *_v ) ;
    virtual void initRestrictionPage () ;
    virtual void pR_showGroupEnzymes ( wxString gr ) ;
    virtual vector <int> getcuts ( string enzyme ) ;
    virtual void pR_showFragments ( int i ) ;
    virtual void add2cocktail ( wxString s ) ;
    virtual void del_from_cocktail ( wxString s ) ;
    virtual void refreshCocktail () ;
    virtual void listFragments ( wxListCtrl *list , vector <int> &vi ) ;
    
    virtual void res_ll ( wxListEvent &event ) ;
    virtual void res_coc_ll ( wxListEvent &event ) ;
    virtual void res_ll_act ( wxListEvent &event ) ;
    virtual void res_ll_col ( wxListEvent &event ) ;
    virtual void res_dd ( wxCommandEvent &event ) ;
    virtual void res_cb ( wxCommandEvent &event ) ;
    virtual void res_ac ( wxCommandEvent &event ) ;
    virtual void res_ct ( wxCommandEvent &event ) ;
    virtual void res_checkbox ( wxCommandEvent &event ) ;
    virtual void res_coc_rm ( wxCommandEvent &event ) ;
    virtual void onOK ( wxCommandEvent &event ) ;
    virtual void onCancel ( wxCommandEvent &event ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;
    
    // Variables
    wxListCtrl *el , *el2 , *rsl , *rsl2 ;
    wxRadioBox *rb ;
    wxChoice *gl ;
    wxCheckBox *nfst ;
    wxSpinCtrl *nfstv ;
    wxBitmapButton *bb , *bb2 ;
    wxButton *btOK , *btCC ;
    TVector *v ;
    int _w , _h ;
    int numberoffragments ;
    vector <TREcache> cutcache ;
    vector <string> nocut ;
    vector <string> cocktail , *remoteCocktail ;
    wxString pre , last_enzyme_selected , last_enzyme_selected_coc ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
