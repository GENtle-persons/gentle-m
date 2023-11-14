/** \file
	\brief Contains the TABIviewer class
*/
#ifndef _ABI_VIEWER_H_
#define _ABI_VIEWER_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;

/**	\brief The ABI (sequencer data) viewer module class
*/
class TABIviewer : public ChildBase
    {
    public :
    TABIviewer(wxWindow *parent, const wxString& title) ; ///< Constructor
    virtual ~TABIviewer () ; ///< Destructor

    virtual void initme () ; ///< Initialization
    virtual wxString getName () const ; ///< Returns the module name
    virtual void showSequence () const ; ///< Show/refresh the sequence
    virtual wxString getStat () const ; ///< Get statistics as a wxString
    virtual void showStat () const ; ///< Show the statistics
    virtual void set_view_only ( int from , int to ) ; ///< Sets partial view
    virtual bool is_only_view () const { return view_from != -1 || view_to != -1 ; } ///< Partial view?
    virtual void toggle_inv_compl () ; ///< Switches between normal and inverse/complementary display
    virtual bool get_inv_compl () const ; ///< Returns the state of normal or inverse/complementary display

    virtual void OnMarkAll(wxCommandEvent& event); ///< Ctrl-A event handler
    virtual void OnFileSave(wxCommandEvent& event); ///< Save-as event handler
    virtual void OnFind(wxCommandEvent& event); ///< Find event handler
    virtual void OnCopy(wxCommandEvent& event); ///< Copy event handler
    virtual void OnCopyToNew(wxCommandEvent& event); ///< Copy-to-new event handler
    virtual void OnEditMode(wxCommandEvent& event); ///< Edit mode event handler
    virtual void OnEditName(wxCommandEvent& event);// Edit name event handler
    virtual void OnSeqPrint(wxCommandEvent& event); ///< Print sequence event handler
    virtual void OnHelplines(wxCommandEvent& event); ///< Grey helper lines event handler
    virtual void OnInvCompl(wxCommandEvent& event); ///< Invers/complement event handler
    virtual void OnSpinWidth(wxSpinEvent& event); ///< Peak width event handler
    virtual void OnSpinHeight(wxSpinEvent& event); ///< Peak height event handler
    virtual void OnZoom(wxScrollEvent& event); ///< Zoom event handler
    virtual void OnHorizontal(wxCommandEvent& event); ///< Horizontal mode event handler
    virtual void OnSpeak(wxCommandEvent& WXUNUSED(event)); ///< Speak sequence event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler
    virtual void OnFontsize(wxCommandEvent& event); ///< Fontsize event handler

    // Variables
    SequenceCanvas *sc ; ///< Pointer to the sequence canvas structure

    private :
    friend class MyFrame ;
    virtual void spinHeight() ; ///< Sets the height spin box
    virtual void set_view ( long _from , long _to ) ;

    // Variables
    wxTextCtrl *stat ; ///< Pointer to statistics text box
    int from , oldh ;
    wxString filename ; ///< The name of the imported file
    wxCheckBox *aidLines , *inv_compl ;
    wxSpinCtrl *f_height , *f_width ;
    wxSlider *slider ; ///< Pointer to the slider structure
    long view_from , view_to ;
    wxChoice *fontsize ;

    DECLARE_EVENT_TABLE()
    } ;

#endif

