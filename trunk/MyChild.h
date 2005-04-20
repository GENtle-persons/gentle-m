/** \file
	\brief Contains the MyChild class, as well as the MySplitter helper class
*/
#ifndef _MYCHILD_H_
#define _MYCHILD_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/clipbrd.h>
#include <wx/splitter.h>

class PlasmidCanvas ;
class SequenceCanvas ;
class TVector ;
class TVectorTree ;
class TMainTree ;
class TMainTreeItem ;
class MyChild ;
class TURLtext ;
class TRestrictionEditor ;
class TVirtualGel ;

/**	\brief Specialized class based on wxSplitterWindow; used in MyChild to separate the different parts of the module
*/
class MySplitter : public wxSplitterWindow
    {
    public :
    MySplitter ( wxWindow *win , int id , MyChild *child ) ; ///< Constructor
    void OnChanged ( wxSplitterEvent &ev ) ; ///< Drag event handler
    MyChild *c ; ///< The calling MyChild

    DECLARE_EVENT_TABLE()
    } ;

/** \brief The very, very, *very* important module to handle DNA in all its beauty!
*/
class MyChild: public ChildBase
{
public:
    MyChild(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style); ///< Constructor
    ~MyChild(); ///< Destructor

    virtual void OnAA_none(wxCommandEvent& event); ///< "Display no amino acids" event handler
    virtual void OnAA_known(wxCommandEvent& event); ///< "Display only known amino acids" event handler
    virtual void OnAA_all(wxCommandEvent& event); ///< "Display amino acids for all reading frames" event handler
    virtual void OnAA_three(wxCommandEvent& event); ///< "Display three-letter amino acids" event handler
    virtual void OnAA_one(wxCommandEvent& event); ///< "Display one-letter amino acids" event handler
    virtual void OnAA_three_1(wxCommandEvent& event); ///< "Display amino acids, reading frame 1" event handler
    virtual void OnAA_three_2(wxCommandEvent& event); ///< "Display amino acids, reading frame 2" event handler
    virtual void OnAA_three_3(wxCommandEvent& event); ///< "Display amino acids, reading frame 3" event handler
    virtual void OnAA_three_M1(wxCommandEvent& event); ///< "Display amino acids, reading frame -1" event handler
    virtual void OnAA_three_M2(wxCommandEvent& event); ///< "Display amino acids, reading frame -2" event handler
    virtual void OnAA_three_M3(wxCommandEvent& event); ///< "Display amino acids, reading frame -3" event handler
    virtual void OnAA_setit(int mode); ///< Sets the amino acid display mode (called by the OnAA_ event handlers)

    virtual void OnActivate(wxActivateEvent& event); ///< Activation event handler
    virtual void Undo(wxCommandEvent& event); ///< Undo event handler
    virtual void Redo(wxCommandEvent& event); ///< Redo event handler (not implemented)
    virtual void updateUndoMenu () ; ///< Updates the undo menu item
    
    virtual void OnMarkAll(wxCommandEvent& event); ///< "Mark all" (Ctrl-A) event handler
    virtual void OnTransformSequence(wxCommandEvent& event); ///< "Transform sequence" event handler
    virtual void OnExtractAA(wxCommandEvent& event); ///< "Extract amino acids" event handler
    virtual void OnFind(wxCommandEvent& event); ///< "Find" event handler
    virtual void OnFileSave(wxCommandEvent& WXUNUSED(event) ) ; ///< "Save"/"Store" event handler
    virtual void OnQuit(wxCommandEvent& event); ///< "Quit" event handler
    virtual void OnCut(wxCommandEvent& event); ///< Cut event handler
    virtual void OnCopy(wxCommandEvent& event); ///< Copy event handler
    virtual void OnPaste(wxCommandEvent& event); ///< Paste event handler
    virtual void OnViewMode(wxCommandEvent& event); ///< Switch to fullscreen plasmid map event handler
    virtual void OnEditMode(wxCommandEvent& event); ///< Sqitch to edit mode event handler
    virtual void OnCircularLinear(wxCommandEvent& event); ///< Toggle circular/linear event handler
    virtual void OnLigation(wxCommandEvent& event); ///< Ligation event handler
    virtual void OnCopyToNew(wxCommandEvent& event); ///< "Copy as new sequence" event handler
    virtual void OnAsNewFeature(wxCommandEvent& event); ///< "Selection as new feature" event handler
    virtual void OnPrintImage(wxCommandEvent& event); ///< Print image event handler
    virtual void OnRestriction(wxCommandEvent& event); ///< Restriction event handler
    virtual void OnHelp(wxCommandEvent& event); ///< Help event handler
    virtual void OnORFs(wxCommandEvent& event); ///< "Show/hide open reading frames" event handler
    virtual void OnEditORFs(wxCommandEvent& event); ///< Change open reading frame settings event handler
    virtual void OnSeqPrint(wxCommandEvent& event); ///< Print sequence event handler
    virtual void OnPrintReport(wxCommandEvent& event); ///< "Print report" event handler
    virtual void OnToggleFeatures(wxCommandEvent& event); ///< Feature display on/off event handler
    virtual void OnToggleRestriction(wxCommandEvent& event); ///< "Restriction enzymes in sequence on/off" event handler
    virtual void OnToggleIDNA(wxCommandEvent& event); ///< Inverse DNA display event handler
    virtual void OnRunPCR(wxCommandEvent& event); ///< PCR event handler
    virtual void OnZoom ( wxCommandEvent &ev ) ; ///< Zoom event handler
    virtual void OnUpdateRefresh(wxUpdateUIEvent& event); ///< Update/Refresh event handler
    virtual void OnSequencingPrimer(wxCommandEvent& WXUNUSED(event)); ///< "Find sequencing primers" event handler
    virtual void OnRemoveSequencingPrimers(wxCommandEvent& WXUNUSED(event)); ///< "Remove sequencing primers" event handler
    virtual void OnAutoAnnotate(wxCommandEvent& WXUNUSED(event)); ///< Automatic annotation event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler (to catch weird messages)

    virtual void initme () ; ///< Initializes the module
    virtual void initPanels () ; ///< Initializes the panels
    virtual wxString getName () ; ///< Returns the module name
    virtual void EnforceRefesh () ; ///< Force display refresh
    virtual void updateSequenceCanvas ( bool remember = false ) ; ///< Refresh the sequence display
    virtual wxString doExtractAA ( bool coding = true ) ; ///< Extract amino acid sequence from DNA
    virtual void runRestriction ( wxString s ) ; ///< Run the restriction
    virtual void addFragmentsToGel ( wxString title , wxArrayInt &cuts , TVirtualGel *gel , TRestrictionEditor &ed ) ; ///< Add restriction fragments to gel
    virtual MyChild *doTransformSequence ( bool inNewVector , bool complement , bool invers ) ; ///< Transform the sequence
    
    TVectorTree *treeBox ; ///< Pointer to the sequence properties tree structure
    TURLtext *propBox ; ///< Pointer to the properties text box
    PlasmidCanvas *cPlasmid; ///< Pointer to the plasmid canvas
    MySplitter *sw , *swu , *swl ;

    bool showORFs , viewMode ;
    int sp1 , sp2 , aa_offset , ly ;
    int aa_state , aa_disp , orf_mode ;
    
    protected :
    virtual void updateToolbar () ; ///< Set the toolbar of this module
    virtual void initToolbar () ; ///< Setup the toolbar for this module
    virtual void initMenus () ; ///< Initialize the menus
    
    DECLARE_EVENT_TABLE()
};

#endif

