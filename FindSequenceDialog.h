#ifndef _FindSequenceDialog_h_
#define	_FindSequenceDialog_h_

#include "main.h"

class FindSequenceDialog : public wxDialog
    {
    public :
    FindSequenceDialog ( wxWindow *parent, const wxString& title ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;
    virtual void OnSearch ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnTextChange ( wxCommandEvent &ev ) ;
    virtual void OnLB ( wxCommandEvent &ev ) ;
    virtual void OnLBdclick ( wxCommandEvent &ev ) ;

    wxString allowed_chars ;

    private :
    virtual bool doesMatch ( char a , char b ) ;
    virtual int subsearch ( const wxString &s , const wxString &sub , int start ) ;
    virtual void sequenceSearch ( bool invers = false ) ;
    virtual void aaSearch () ;
    virtual void itemSearch () ;
    virtual void restrictionSearch () ;
    virtual void doAction ( bool doubleclick ) ;
    
    ChildBase *c ;
    wxTextCtrl *t ;
    int bo , fh ;
    int p , last ;
    wxListBox *lb ;
    wxArrayInt vi ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
