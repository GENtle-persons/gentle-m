#ifndef _TSEQUENCING_ASSISTANT_DIALOG_H_
#define _TSEQUENCING_ASSISTANT_DIALOG_H_

#include "main.h"

class TStorage ;
class MyFrame ;
class MyChild ;
class TRestrictionEnzyme ;
class ChildBase ;
class TPrimer ;
class TPrimerDesign ;

class TSequencingAssistantDialog : public wxDialog
    {
    public :
    TSequencingAssistantDialog (wxWindow *parent , const wxString& title ) ; ///< Constructor
    virtual ~TSequencingAssistantDialog () ;
    void fix_rev_compl ( const int sel = -2 ) ;
    
    wxChoice *sequence , *abi1 , *abi2 ;
    vector <ChildBase*> dna , abi ;
    
    private :
    void OnCheckOK ( wxCommandEvent &ev ) ;
    void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    wxButton *ok ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
