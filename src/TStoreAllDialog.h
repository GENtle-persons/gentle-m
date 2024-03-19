#ifndef __STORE_ALL_DIALOG__
#define __STORE_ALL_DIALOG__

#include "main.h"

class TStoreAllDialog : public wxDialog
    {
    public :
    TStoreAllDialog ( wxWindow *_parent , const wxString& title ) ; ///< Constructor
    virtual ~TStoreAllDialog() ;
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Cancel button event handler
    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnAll ( wxCommandEvent &ev ) ;
    virtual void OnNone ( wxCommandEvent &ev ) ;
    virtual void OnInvert ( wxCommandEvent &ev ) ;

    private :
    void PopulateList () ;
    wxCheckListBox *list ;
    wxChoice *database ;
    wxArrayString db_name , db_file ;
    wxCheckBox *force_db , *auto_overwrite ;
    wxString defdb ;
    vector <ChildBase*> children ;

    DECLARE_EVENT_TABLE()
    } ;



#endif
