#ifndef _T_ITEM_EDIT_DIALOG_
#define _T_ITEM_EDIT_DIALOG_

#include "main.h"

class TVectorItem ;

class TItemEditDialog : public wxDialog
    {
    public :
    TItemEditDialog ( wxWindow *parent, const wxString& title ,
                        TVectorItem &_vi ) ;
    virtual ~TItemEditDialog () ;

    virtual void OnCharHook(wxKeyEvent& event) ;
    virtual void OnColor ( wxCommandEvent &ev ) ;
    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnUseOffset ( wxCommandEvent &ev ) ;
    virtual void OnList ( wxListEvent &ev ) ;
    virtual void initlb () ;
                                            
    // Variables
    TVectorItem *vi ;
    wxRadioBox *rb ;
    wxCheckBox *useOffset ;
    wxTextCtrl *offset ;
    wxListBox *lb ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
