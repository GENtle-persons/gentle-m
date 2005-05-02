/** \file
	\brief The vector items tree component.
*/
#ifndef _TVECTORTREE_H_
#define _TVECTORTREE_H_

#include "main.h"
#include <wx/imaglist.h>

class MyChild ;

/** \class TTreeItem
	\brief Item in TVectorTree
*/
class TTreeItem : public wxTreeItemData
    {
    public :
    TTreeItem () : wxTreeItemData() {} ; ///< Empty constructor
    TTreeItem ( wxString m , wxString _t = _T("") , void *_p = NULL )  ///< Constructor
    	: wxTreeItemData()
    	{ message = m ; type = _t ; p = _p ; } ;
    wxString message ; ///< The message to display
    wxString type ; ///< The item type
    void *p ; ///< Can contain multiple different values, depending on the type
    } ;

/** \class TVectorTree
	\brief The vector items tree class
*/
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

    void itemMark ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void itemMarkShow ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void itemEdit ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void itemAsNewSequence ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void itemDelete ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void itemShowHide ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsEdit ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsInfo ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsDel ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsShowHide ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsMark ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsMarkShow ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsAdd2Cocktail ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsAddAndCut ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void rsCutWithCocktail ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void vecEdit ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void blastDNA ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    void blastAA ( wxCommandEvent &ev ) ; ///< Event redirect to parent child
    
    // Variables
    MyChild *p ; ///< Pointer to the child containing this tree
    wxTextCtrl *textWindow ; ///< Pointer to the (con)text display area
    wxTreeItemId treeroot ; ///< The root item
    wxTreeItemId vroot ; ///< The vector root item
    wxTreeItemId enzroot ; ///< The enzyme root item
    
    
    DECLARE_EVENT_TABLE()
    } ;

#endif

