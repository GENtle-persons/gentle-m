#ifndef _ProgramOptionsDialog_h_
#define _ProgramOptionsDialog_h_

#include "main.h"

class ProgramOptionsDialog : public wxDialog
    {
    public :
    ProgramOptionsDialog(wxWindow *parent, const wxString& title ) ;

    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;
    void OnCharHook(wxKeyEvent& event) ;
    
    wxChoice *language ;
    wxCheckBox *enhancedDisplay , *vectorTitle , *vectorLength ,
                *loadLastProject , *useMetafile , *showSplashScreen ,
                *checkUpdate , *useInternalHelp ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif
