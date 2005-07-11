/** \file
	\brief Contains the FindSequenceDialog class
*/
#ifndef _FindSequenceDialog_h_
#define	_FindSequenceDialog_h_

#include "main.h"

WX_DECLARE_STRING_HASH_MAP ( wxChar , wxHashChar ) ;

/**	\brief The class implementing the "Find" dialog in various ChildBase modules
*/
class FindSequenceDialog : public wxDialog
    {
    public :
    FindSequenceDialog ( wxWindow *parent, const wxString& title ) ; ///< Constructor
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual void OnSearch ( wxCommandEvent &ev ) ; ///< Search button event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    virtual void OnTextChange ( wxCommandEvent &ev ) ; ///< Search text change event handler
    virtual void OnLB ( wxCommandEvent &ev ) ; ///< List box choice change event handler
    virtual void OnLBdclick ( wxCommandEvent &ev ) ; ///< List box double click event handler

    wxString allowed_chars ; ///< Chars one can search for

    private :
	 virtual wxString getQuery () ;
    virtual bool doesMatch ( char a , char b ) ; ///< Returns if a matches b ( more than "is equal"!)
    virtual int subsearch ( const wxString &s , const wxString &sub , int start ) ; ///< Compares a string and a substring
    virtual void sequenceSearch ( bool invers = false ) ; ///< Search in sequence
    virtual void aaSearch () ; ///< Search in amino acid sequence
    virtual void itemSearch () ; ///< Search in items
    virtual void restrictionSearch () ; ///< Search for restriction enzymes
    virtual void doAction ( bool doubleclick ) ; ///< Perform action, now that a list item has been double-clicked
    virtual void aaSubSearch ( const wxString &s , int start , int dir , wxString rf ) ; ///< Search in resulting amino acid sequence
    
    ChildBase *c ; ///< Pointer to calling ChildBase class
    wxTextCtrl *t ; ///< The text control
    int p , last ;
    wxListBox *lb ; ///< The results list box control
    wxArrayInt vi ;
    wxHashChar codonhash ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
