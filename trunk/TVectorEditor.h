/** \file
	\brief Contains the TVectorEditor class
*/
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
class TEnzymeSettingsTab ;
class TEnzymeRules ;

/**	\brief The sequence edit dialog
*/
class TVectorEditor : public wxDialog
    {
    public :
    TVectorEditor(wxWindow *parent, const wxString& title , TVector *_v ) ; ///< Constructor
    ~TVectorEditor () ; ///< Destructor
    
    virtual void initialViewItem ( int num ) ; ///< Initially, view item #num
    virtual void initialViewEnzyme ( wxString e ) ; ///< Initially, view enzyme e
    virtual void GetMyClientSize ( int *w , int *h , wxPanel *pan ) ; ///< Client size of the panel/tab

    virtual void OnOK ( wxCommandEvent& WXUNUSED(event) ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent& WXUNUSED(event) ) ; ///< Cancel button event handler
    virtual void SelChangeItems ( wxListEvent &ev ) ; ///< "Item selected" event handler
    virtual void DeselItems ( wxListEvent &ev ) ; ///< "Item unselected" event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual void addItem2list ( TVectorItem &i , int a ) ; ///< Adds an item at position a to the item list
    
    virtual void cleanup () ; ///< Clean up the dialog
    virtual void initPanProp () ; ///< Initialize properties tab
    virtual void initPanItem () ; ///< Initialize items tab
    virtual void initPanEnzym () ; ///< Initialize enzymes tab
    virtual void initPanEnzym2 () ; ///< Initialize enzymes(2) tab
    virtual void initPanProt () ; ///< Initialize proteases tab
    virtual void addOkCancel ( wxWindow *p ) ; ///< Add OK/Cancel buttons to a tab
    virtual void makeItemsList () ; ///< Generates the list of items
    virtual void storeItemData ( int i = -1 ) ; ///< Stores data with an item
    virtual void hideEm () ;
    
    virtual void commitVector () ; ///< Do changes to the original vector
    virtual void commitItems () ; ///< Do changes to the original items list
    virtual void commitEnzymes () ; ///< Do changes to the original enzyme settings
    virtual void commitProteases () ; ///< Do changes to the original proteases list

    virtual void showGroupEnzymes ( wxString gr ) ; ///< Slow list of enzymes in group gr
    virtual void showEnzymeGroups () ; ///< Show list of available enzyme groups
    void showProteases () ; ///< Lists the proteases
    
    virtual void enzymeSelChange ( wxCommandEvent& WXUNUSED(event) ) ; ///< "A different enzyme was selected" event handler
    virtual void enzymeListDlbClick ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Double-click on enzyme" event handler
    virtual void enzymeAddEn ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Add enzyme" event handler
    virtual void enzymeAddGr ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Add enzyme group" event handler
    virtual void enzymeAddToGr ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Add enzyme(s) to group" event handler
    virtual void enzymeAddToNewGr ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Add enzyme(s) to new group" event handler
    virtual void enzymeDelGr ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Delete enzyme group" event handler
    virtual void enzymeDelFromGr ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Delete enzyme from group" event handler
    virtual void enzymeDelEn ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Delete enzyme" event handler
    virtual void newEnzyme ( wxCommandEvent& WXUNUSED(event) ) ; ///< "New enzyme" event handler
    virtual void importCloneEnzymes ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Import CLONE-format enzyme list" event handler (not used anymore)
    virtual void proteaseSelChange ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Protease selection change" event handler

    void newProtease ( wxCommandEvent& WXUNUSED(event) ) ; ///< "New protease" event handler
    void editProtease ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Edit protease" event handler

    void itemAdd ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Add item" event handler
    void itemDel ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Delete item" event handler
    void itemClr () ; ///< Clears the item list (I think...)
    void itemCol ( wxListEvent &ev ) ;
    void itemCol2 ( wxCommandEvent& WXUNUSED(event) ) ;
    void itemChoice ( wxCommandEvent& WXUNUSED(event) ) ;

    bool hideProp , hideItem , hideEnzym ;

    private :
    virtual int getCurrentItem () ; ///< Returns the ID of the currently selected item
    virtual void clearItemSelection () ; ///< Clears the current item selection
    virtual void itemColInternal() ;

        
    wxListBox *listCE , *listGroups , *listGE ;
    wxArrayString eig , ce ;
    
    TVector *v ;
    int bo , th ;

    wxNotebook *nb ;
    wxPanel *panProp , *panItem , *panEnzym , *panProt ;
    TEnzymeSettingsTab *panEnzyme2 ;
    wxButton *bAdd , *bDel , *bCol , *b_atg , *b_dfg , *b_dg ;
    wxTextCtrl *name , *lu , *ll , *ru , *rl ;
    TURLtext *desc , *idesc , *pro_txt ;
    wxTextCtrl *iname , *ifrom , *ito ;
    wxListCtrl *items ;
    wxRadioBox *irb ;
    wxChoice *ichoice ;
    wxCheckBox *icb , *icv ;
    wxCheckListBox *prots ;
    TEnzymeRules *oldEnzymeRules ;
    
    wxArrayTVectorItem newitems ;
    int lastSelection ;
    bool e_diduseit ;
    
    virtual void loadItemData ( int i ) ;
    virtual void updateItem ( TVectorItem &i ) ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif
