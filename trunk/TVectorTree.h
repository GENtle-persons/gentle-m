#ifndef _TVECTORTREE_H_
#define _TVECTORTREE_H_

#include "main.h"
#include <wx/imaglist.h>

class MyChild ;

class TTreeItem : public wxTreeItemData
    {
    public :
    TTreeItem () : wxTreeItemData() {} ;
    TTreeItem ( wxString m , wxString _t = "" , void *_p = NULL ) : wxTreeItemData() { message = m ; type = _t ; p = _p ; } ;
    wxString message , type ;
    void *p ;
    } ;

class TVectorTree : public wxTreeCtrl
    {
    public :
    TVectorTree ( ChildBase *parent , int i ) ;
    
    void initme () ;
    void OnEvent ( wxTreeEvent &event ) ;
    void OnActivation ( wxTreeEvent &event ) ;
    void OnRightClick ( wxTreeEvent &event ) ;
    void OnPrimerForward ( wxCommandEvent &ev ) ;
    void OnPrimerBackward ( wxCommandEvent &ev ) ;
    void OnPrimerBoth ( wxCommandEvent &ev ) ;
    void OnPrimerMutation ( wxCommandEvent &ev ) ;
    void ToggleEnzymeVisibility ( TRestrictionEnzyme *e ) ;

    void itemMark ( wxCommandEvent &ev ) ;
    void itemMarkShow ( wxCommandEvent &ev ) ;
    void itemEdit ( wxCommandEvent &ev ) ;
    void itemAsNewSequence ( wxCommandEvent &ev ) ;
    void itemDelete ( wxCommandEvent &ev ) ;
    void itemShowHide ( wxCommandEvent &ev ) ;
    void rsEdit ( wxCommandEvent &ev ) ;
    void rsInfo ( wxCommandEvent &ev ) ;
    void rsDel ( wxCommandEvent &ev ) ;
    void rsShowHide ( wxCommandEvent &ev ) ;
    void rsMark ( wxCommandEvent &ev ) ;
    void rsMarkShow ( wxCommandEvent &ev ) ;
    void rsAdd2Cocktail ( wxCommandEvent &ev ) ;
    void rsAddAndCut ( wxCommandEvent &ev ) ;
    void rsCutWithCocktail ( wxCommandEvent &ev ) ;
    void vecEdit ( wxCommandEvent &ev ) ;
    void blastDNA ( wxCommandEvent &ev ) ;
    void blastAA ( wxCommandEvent &ev ) ;
    
    // Variables
    MyChild *p ;
    wxTextCtrl *textWindow ;
    wxTreeItemId treeroot , vroot , enzroot ;
    
    
    DECLARE_EVENT_TABLE()
    } ;

#endif

