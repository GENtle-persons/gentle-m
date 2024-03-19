/** \file
    \brief Contains the TAlignment class and its helper class, TAlignLine
*/
#ifndef _ALIGNMENT_H_
#define _ALIGNMENT_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>
#include <wx/spinctrl.h>

#define BACK_LEFT 1
#define BACK_UP 2
#define BACK_LU 4

#define ALG_CW 0
#define ALG_SW 1
#define ALG_NW 2

class SeqAA ;
class SequenceCharMarkup ;
class SeqAlign ;

/** \brief Helper class for TAlignment; represents a line (a sequence) in the alignment
*/
class TAlignLine // pun intended
    {
    public :
    TAlignLine () ; ///< Constructor
    ~TAlignLine () ; ///< Destructor
    void ResetSequence () ; ///< Resets the settings to default
    ChildBase *FindOrigin () ; ///< Returns a pointer to the ChildBase structure containing the original sequence, or NULL
    void showFeatures () ; ///< Display the features of this sequence
    void hideFeatures () ; ///< Hide the features of this sequence
    TVector *getFeatures () ; ///< Returns a pointer to the TVector structure, so the features can be accessed
    bool hasFeatures () ; ///< Does this sequence have annotated items/features

    wxString name , s ;
    TVector *v ;
    bool isIdentity ; ///< Is this line an identity display (or a "real" sequence)?
    wxString phylip_id ;
    vector <SequenceCharMarkup> markup ;

    private :
    TVector *features ;
    } ;

/** \brief The alignment module class. Note: this is actually a child class of both ChildBase and wxThreadHelper!
*/
class TAlignment : public ChildBase,wxThreadHelper
    {
    public :
    TAlignment(wxWindow *parent, const wxString& title) ; ///< Constructor
    virtual ~TAlignment () ; ///< Destructor

    virtual void initme () ; ///< Initialization
    virtual wxString getName () const ; ///< Returns "Alignment"

    virtual void* Entry() ; // wxThreadHelper

    virtual int NeedlemanWunsch ( wxString &s1 , wxString &s2 )  ; ///< Needleman-Wunsch-align two sequences
    virtual int SmithWaterman ( wxString &s1 , wxString &s2 )  ; ///< Smith-Waterman-align two sequences
    virtual int MatrixAlignment ( wxString &_s1 , wxString &_s2 , bool local ) ; ///< Part of NW/SW alignment algorithm
    virtual void MatrixBacktrack ( vector <wxArrayInt> &back ,
                                   wxString s1 , wxString s2 , wxString &t1 , wxString &t2 ,
                                   int i , int j ) ; ///< Part of NW/SW alignment algorithm

    virtual void redoAlignments ( bool doRecalc = true ) ; ///< Repaint and (maybe) recalculate the alignment
    virtual void recalcAlignments () ; ///< Recalculate the alignment
    virtual wxColour findColors ( char c1 , char c2 , bool fg ) ; ///< Generate the appropriate foreground/background color, using current settings

    virtual void OnSettings ( wxCommandEvent &ev ) ; ///< Settings button event handler
    virtual void OnMarkAll ( wxCommandEvent &ev ) ; ///< "Mark all" event handler
    virtual void OnSeqPrint(wxCommandEvent& event); ///< "Print sequence" event handler
    virtual void OnFileSave ( wxCommandEvent &ev ) ; ///< Save event handler
    virtual void OnMenuBold ( wxCommandEvent &ev ) ; ///< "Display bold" event handler
    virtual void OnMenuMono ( wxCommandEvent &ev ) ; ///< "Display monochrome" event handler
    virtual void OnMenuNorm ( wxCommandEvent &ev ) ; ///< "Display normal" event handler
    virtual void OnMenuInvs ( wxCommandEvent &ev ) ; ///< "Display inverse" event handler
    virtual void OnMenuSoa ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnMenuSoaI ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnMenuSiml ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnMenuSeq ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnMenuFeat ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnMenuRNA ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnMenuCons ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnMenuIdent ( wxCommandEvent &ev ) ; ///< Some event handler
    virtual void OnHorizontal ( wxCommandEvent& event ) ; ///< "Display horizontal" event handler
    virtual void OnCopy ( wxCommandEvent& event ) ; ///< Copy event handler
    virtual void OnFontsize(wxCommandEvent& event); ///< Fontsize event handler

    virtual void invokeOriginal ( int id , int pos ) ; ///< Open the original sequence at the given position
    virtual void callMiddleMouseButton ( int id , int pos , wxString _mode = _T("") ) ; ///< Perform the middle mouse button function (the gap orgy)
    virtual void MoveUpDown ( int what , int where ) ; ///< Move a line into the given direction
    virtual void prealigned ( wxArrayString &vs , wxArrayChildBase &vc ) ; ///< Initialize with a prealigned list of sequences (for example, from a multi-sequence genbank file)
    virtual void fromVector ( TVector *nv ) ; ///< Initialize from a single vector (???)
    virtual bool isDNA () ; ///< Tries to determine wether this is a DNA or an amino acid alignment
    virtual bool isAA () ; ///< Tries to determine wether this is a DNA or an amino acid alignment

    virtual void RunPhylip ( int cmd ) ; ///< Takes Phylip commands (invoked from SequenceCanvas)
    virtual bool isThreadRunning() { return threadRunning ; } ///< Is a thread (alignment calculation) currently running?
    void getCharMarkup ( SequenceCharMarkup &scm , int vline , int pos , int vfirst ) ;
    void editAppearance ( int from , int to , int firstline , int lastline ) ;

    // Variables
    SequenceCanvas *sc ; ///< Pointer to the sequence canvas
    vector <TAlignLine> lines ; ///< The list of TAlignLine structures
    wxColour colDNA[256] , colAA[256] , *colCur ;
    wxString gap , matrix , consensusSequence , name , database ;
    int match , mismatch , gap_penalty , algorithm ;
    bool bold , mono , cons , invs , showIdentity ;

    private :
    wxChoice *mmb ;
    SeqAA *aaa ;
    bool threadRunning ;
    bool keepIdentity ;
    wxChoice *fontsize ;

    virtual void myInsert ( int line , int pos , char what ) ;
    virtual void myDelete ( int line , int pos ) ;
    virtual void updateSequence () ;
    virtual void generateConsensusSequence ( bool addit = true ) ;
    virtual void fixMenus ( int i ) ;

    protected :
    virtual wxString getExportFilters () ;
    virtual void doExport ( wxString filename , int filter ) ; ///< Export data, depending on filename and export type
    virtual void readTabColors ( wxString filename ) ;

    DECLARE_EVENT_TABLE()
    } ;



#endif
