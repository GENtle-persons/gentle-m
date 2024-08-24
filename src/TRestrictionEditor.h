/** \file
    \brief Contains the TRestrictionEditor class, as well as the TREcache and TFragment helper class
*/
#ifndef _TRESTRICTIONEDITOR_H_
#define _TRESTRICTIONEDITOR_H_

#include "main.h"
#include <wx/spinctrl.h>

class wxNotebook ;
class TRestrictionEnzyme ;

/** \brief Restriction enzyme site cache
*/
class TREcache
    {
    public :
    TREcache ( const wxString& a , const int b ) { enzyme = a ; cut = b ; } ///< Constructor
    wxString enzyme ; ///< Enzyme name
    int cut ; ///< Position of cut
    } ;

/** \brief A restriction fragment (from, to, length)
*/
class TFragment
    {
    public :
    int from , to , length ;
    double weight ; ///< Used in TProteolysis
    } ;

bool operator < ( const TFragment &f1 , const TFragment &f2 ) ;
bool operator == ( const TFragment &f1 , const TFragment &f2 ) ;

/** \brief The restriction editor dialog class
*/
class TRestrictionEditor : public wxDialog
    {
    public :
    TRestrictionEditor(MyFrame * const parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style); ///< Constructor
    ~TRestrictionEditor () ; ///< Destructor

    virtual void initme ( /* not const*/ TVector * const _v ) ; ///< Initialization
    virtual void initRestrictionPage () ; ///< Sets up the page
    virtual void pR_showGroupEnzymes ( const wxString& gr ) ; ///< Shows enzymes in selected enzyme group
    virtual wxArrayInt getcuts ( const wxString& enzyme ) ; ///< Returns the cut positions for the given enzyme
    virtual void pR_showFragments ( int i ) ; ///< Show the resulting fragments
    virtual void add2cocktail ( const wxString& s ) ; ///< Add enzyme to restriction cocktail
    virtual void del_from_cocktail ( const wxString& s ) ; ///< Remove enzyme from restriction cocktail
    virtual void refreshCocktail () ; ///< Update cocktail list
    virtual void getFragmentList ( wxArrayInt &cuts , vector <TFragment> &fragments , const bool clear = true ) const ; ///< Set list of fragments
    virtual void listFragments ( wxListCtrl * const list , wxArrayInt &vi ) ; ///< Show fragments in a list
    virtual void listEnzymesInGroup ( const wxString& gr , wxArrayString &vs ) const ; ///< List the enzymes in group
    virtual void iterateFragments ( const wxArrayInt& cuts , vector <TFragment> &fragments , const int depth = 0 ) const ;

    virtual void res_ll ( wxListEvent &event ) ; ///<  event handler
    virtual void res_coc_ll ( wxListEvent &event ) ; ///<  event handler
    virtual void res_ll_act ( wxListEvent &event ) ; ///<  event handler
    virtual void res_ll_col ( wxListEvent &event ) ; ///<  event handler
    virtual void res_dd ( wxCommandEvent &event ) ; ///<  event handler
    virtual void res_cb ( wxCommandEvent &event ) ; ///<  event handler
    virtual void res_ac ( wxCommandEvent &event ) ; ///<  event handler
    virtual void res_ct ( wxCommandEvent &event ) ; ///<  event handler
    virtual void res_checkbox ( wxCommandEvent &event ) ; ///<  event handler
    virtual void res_cb_add2gel ( wxCommandEvent &event ) ; ///< "Add to gel" checkbox event handler
    virtual void res_coc_rm ( wxCommandEvent &event ) ; ///<  event handler
    virtual void onOK ( wxCommandEvent &event ) ; ///< OK button event handler
    virtual void onCancel ( wxCommandEvent &event ) ; ///< Cancel button event handler
    virtual void onAddAll ( wxCommandEvent &event ) ; ///< "Add all" button event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    // Variables
    wxCheckBox *createFragments , *add2gel , *oneLaneEach , *partialDigestion ;
    wxListCtrl *el , *el2 , *rsl , *rsl2 ;
    wxRadioBox *rb ;
    wxChoice *gl ;
    wxCheckBox *nfst ;
    wxSpinCtrl *nfstv ;
    wxBitmapButton *bb , *bb2 ;
    wxButton *btOK , *btCC ;

    TVector *v ; ///< Vector operated on, not const because of invocation of setAction

    int _w , _h ;
    int numberoffragments ;
    vector <TREcache> cutcache ;
    wxArrayString nocut ; ///< List of enzymes already known not to cut Vector v
    wxArrayString cocktail ;
    wxArrayString *remoteCocktail ;
    wxString pre ;
    wxString last_enzyme_selected ;
    wxString last_enzyme_selected_coc ;
    wxArrayInt cocktailFragments ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
