#ifndef _TEXTIMPORTDIALOG_H_
#define _TEXTIMPORTDIALOG_H_

#include "main.h"

class TVector ;
class TVectorItem ;
class MyFrame ;
class TStorage ;
        
class TextImportDialog : public wxDialog
    {
    public :
    TextImportDialog(wxWindow *parent, const wxString& title ) ;

    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;
    void OnCharHook(wxKeyEvent& event) ;

    wxTextCtrl *name , *sequence ;
    wxChoice *type ;
    wxString sName , sSequence ;

    DECLARE_EVENT_TABLE()
    } ;

#endif

