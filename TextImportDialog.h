/** \file
	\brief Contains the TextImportDialog class
*/
#ifndef _TEXTIMPORTDIALOG_H_
#define _TEXTIMPORTDIALOG_H_

#include "main.h"

class TVector ;
class TVectorItem ;
class MyFrame ;
class TStorage ;

/**	\brief The dialog where one can enter a sequence manually
*/
class TextImportDialog : public wxDialog
    {
    public :
    TextImportDialog(wxWindow *parent, const wxString& title ) ; ///< Constructor
    ~TextImportDialog () ; ///< Destructor

    void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    void OnCharHook(wxKeyEvent& event) ; ///< Keys event handler

    wxTextCtrl *name , *sequence ;
    wxChoice *type ; ///< Sequence type dropdown box
    wxString sName , sSequence ;

    DECLARE_EVENT_TABLE()
    } ;

#endif

