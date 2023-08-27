/** \file
	\brief Contains the AlignmentAppearanceDialog class
*/
#ifndef _ALIGNMENTAPPEARANCEDIALOG_H_
#define _ALIGNMENTAPPEARANCEDIALOG_H_

#include "Alignment.h"

/**	\brief The class implementing the "Alignment appearance" dialog in the alignment module
*/
class AlignmentAppearanceDialog : public wxDialog
    {
    public :
    AlignmentAppearanceDialog ( wxWindow *_parent , const wxString& title ) ; ///< Constructor
	~AlignmentAppearanceDialog () ;
    virtual void setup ( int _from , int _to , int _firstline , int _lastline , TAlignment *_ali ) ; ///< Setup from marked region

    virtual void OnLineColorButton ( wxCommandEvent &event ) ; ///< Line color button event handler
    virtual void OnForegroundButton ( wxCommandEvent &event ) ; ///< Foreground color button event handler
    virtual void OnBackgroundButton ( wxCommandEvent &event ) ; ///< Background color button event handler
    virtual void OnOK ( wxCommandEvent &event ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent &event ) ; ///< Cancel button event handler
    virtual void OnReset ( wxCommandEvent &event ) ; ///< Reset button event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    
    private :
    void set_pen ( SequenceCharMarkup &scm , int id , int border ) ; ///< Sets a pen in the markup object; used by OnOK
    void addLine ( const wxString& name , wxArrayString &as , wxFlexGridSizer *sizer ) ; ///< Adds a line of elements (radio boxes, buttons)
    
    vector <wxRadioBox*> radioboxes ;
    vector <wxSpinCtrl*> thickness ;
    vector <wxColour> colors ;
    int line_color_buttons ;
    wxCheckBox *use_foreground , *use_background , *bold , *italics ;
    wxColour color_foreground , color_background ;
    int from , to , firstline , lastline ;
    TAlignment *ali ; ///< Pointer to the alignment
    
    DECLARE_EVENT_TABLE()
    } ;

#endif
