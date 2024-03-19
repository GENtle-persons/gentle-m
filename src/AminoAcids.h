/** \file
    \brief Contains the TAminoAcids module class
*/
#ifndef _AMINO_ACIDS_H_
#define _AMINO_ACIDS_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class TVector ;
class TURLtext ;
class TTestSuite ;

/** \brief The amino acid sequence display module
*/
class TAminoAcids : public ChildBase
    {
    public :
    TAminoAcids(wxWindow *parent, const wxString& title) ; ///< Constructor
    virtual ~TAminoAcids () ; ///< Destructor

    virtual void initme () ; ///< Initialization
    virtual wxString getName () const ; ///< Returns the name of the TVector structure
    virtual void showSequence () const ; ///< Update the sequence display
    virtual void showStat () const ; ///< Show the sequence statistics box

    virtual void OnMarkAll(wxCommandEvent& event); ///< "Mark all" event handler
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) ; ///< "Save sequence" event handler
    virtual void OnFind(wxCommandEvent& event); ///< Find event handler
    virtual void OnCut(wxCommandEvent& event); ///< "Cut" event handler
    virtual void OnCopy(wxCommandEvent& event); ///< "Copy" event handler
    virtual void OnPaste(wxCommandEvent& event); ///< "Paste" event handler
    virtual void OnEditMode(wxCommandEvent& event); ///< "Toggle edit mode" event handler
    virtual void OnEditName(wxCommandEvent& event); ///< "Edit name" event handler
    virtual void OnPrint(wxCommandEvent& event); ///< Print event handler
    virtual void OnAsNewFeature(wxCommandEvent& event); ///< "Selection as new feature" event handler
    virtual void OnBlastAA(wxCommandEvent& event); ///< "Blast sequence" event handler
    virtual void OnPhotometer(wxCommandEvent& event); ///< "Start photometer calculator" event handler
    virtual void invokeVectorEditor ( const wxString& what = _T("") , const int num = 0 , const bool forceUpdate = false ) ; ///< "Edit sequence dialog" event handler
    virtual void OnHorizontal ( wxCommandEvent& event ) ; ///< "Toggle horizontal/vertical" event handler
    virtual void OnListBox ( wxCommandEvent& event ) ; ///< "List box choice" event handler
    virtual void OnIP ( wxCommandEvent& event ) ; ///< "Inline plot dropdown box" event handler
    virtual void OnBacktranslate ( wxCommandEvent& event ) ; ///< "Backtranslate to DNA" event handler
    virtual void OnIPC ( wxCommandEvent& event ) ; ///< "IPC prediction" event handler
    virtual void OnSpeak(wxCommandEvent& WXUNUSED(event)); ///< Speak sequence event handler
    virtual void OnProteolysis(wxCommandEvent& WXUNUSED(event)); ///< Proteolysis event handler
    virtual void OnFontsize(wxCommandEvent& event); ///< Fontsize event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler

    virtual void Undo(wxCommandEvent& event); ///< "Undo" event handler
    virtual void Redo(wxCommandEvent& event); ///< "Redo" event handler (unused)
    virtual void updateUndoMenu () ; ///< Updates the message for the undo function
    virtual void handleListBox ( const wxString& t ) ; ///< Choses a new entry from the listbox
    virtual bool HasUndoData () ;

    // Variables
    SequenceCanvas *sc , *sc2 ;
    wxTextCtrl *stat ; ///< Pointer to the sequence statistics mini-display
    TURLtext *desc ; ///< Pointer to the sequence description mini-display
    wxSplitterWindow *hs , *vs ;
    int from ; ///< Cursor position (sometimes...)
    int miniDisplayOptions ; ///< Options for the mini-display

    wxWindow *curDisplay ;
    PlasmidCanvas *pc ; ///< Plasmid canvas, used in "scheme" mini-display
    wxBoxSizer *v0 ;

    protected :
    virtual void updateToolbar () ; ///< Updates the toolbar to the current module

    private :
    friend class TTestSuite ;
    wxListBox *lb ; ///< Pointer to the listbox with different "mini-displays"
    wxBoxSizer *h1 , *v1 ;
    wxChoice *inlinePlot ; ///< Inline plot dropdown box (in toolbar)
    wxString lastLBsel ; ///< The last selected "mini-display", as the string from the listbox
    wxChoice *fontsize ;

    DECLARE_EVENT_TABLE()
    } ;

#endif

