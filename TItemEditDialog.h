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
    void OnColor ( wxCommandEvent &ev ) ;
    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;
    void OnUseOffset ( wxCommandEvent &ev ) ;
                                            
    // Variables
    TVectorItem *vi ;
    wxRadioBox *rb ;
    wxCheckBox *useOffset ;
    wxTextCtrl *offset ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
