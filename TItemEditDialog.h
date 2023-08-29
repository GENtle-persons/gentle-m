/** \file
	\brief Contains the TItemEditDialog class
*/
#ifndef _T_ITEM_EDIT_DIALOG_
#define _T_ITEM_EDIT_DIALOG_

#include "main.h"

class TVectorItem ;

/**	\brief Dialog to set item/feature color, offset, type etc.; called from TVectorEditor
*/
class TItemEditDialog : public wxDialog
    {
    public :
    TItemEditDialog ( wxWindow *parent, const wxString& title ,
                        TVectorItem &_vi ) ; ///< Constructor
    virtual ~TItemEditDialog () ; ///< Destructor

    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual void OnColor ( wxCommandEvent &ev ) ; ///< Color button event handler
    virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    virtual void OnUseOffset ( wxCommandEvent &ev ) ; ///< Offset checkbox event handler
    virtual void OnList ( wxCommandEvent & WXUNUSED(ev) ) ; ///< Context list box event handler
    virtual void initlb () ; ///< Initialized list box

    // Variables
    TVectorItem *vi ; ///< Pointer to the TVectorItem
    wxRadioBox *rb ; ///< The different item types
    wxCheckBox *useOffset ; ///< The offset checkbox
    wxTextCtrl *offset ; ///< The offset value
    wxListBox *lb ; ///< The list box

    DECLARE_EVENT_TABLE()
    } ;

#endif
