/** \file
	\brief Contains the classes SequenceCanvas, SeqBasic and its children, and the SequencePartList and SeqPos helper classes
*/
#ifndef _SequenceCanvas_H_
#define _SequenceCanvas_H_

#include "main.h"

class MyApp ;
class MyChild ;
class TAminoAcids ;
class TPrimerDesign ;
class SequenceCanvas ;
class TVector ;
class TPrimer ;
class ABItype ;
class TProtease ;
class TProteaseCut ;
class TAAProp ;
class TAlignment ;

/** \brief This class assists in the restriction site layout
*/
class SequencePartList
    {
    public :
    SequencePartList () { maxlevels = slen = 0 ; } ; ///< Constructor
	virtual ~SequencePartList () {} ; ///< Dummy destructor
    virtual void prepare ( int size ) ; ///< Prepares memory for storage (improves speed on huge sequences)
    virtual void add ( int id , int from , int to ) ; ///< Adds an item
    virtual void makeLevels () ; ///< Generates the number of necessary levels to show items non-overlapping
    virtual int here ( int pos , int level ) ; ///< The item at that position at that level
    virtual int getID ( int internalID ) ; ///< The "outside" ID of the item (different from the internal one)
    virtual int getFrom ( int internalID ) ; ///< Returns the beginning of the item
    virtual int getTo ( int internalID ) ; ///< Returns the end of the item
    virtual int size () ; ///< Returns the size in bytes of the structure (debugging only)
    virtual int getLevel ( int i ) ;

    int maxlevels , slen ;
    
    private :
    wxArrayInt vi , vl , vx , vy ;
    vector <wxArrayInt> vl2 ; ///< Items per position per level
    } ;    
    
/** \brief SeqPos manages the positions of all items for a single "type" (for example, DNA)

 The vectors p, r and m are always the same size, and contain information
   about the same item at the same index:
       
 p contains the seqeunce position matching the item
 
 r contains the visual position of the item
 
 m contains the marking of the item:
     
   0 = not marked
   
   1 = marked
   
   2 = cursor in edit mode

 Vector l contains additional positioning information, namely the visual
   position of a horizontal line of items, for faster access when trying
   to determine the target of a mouse click
*/
class SeqPos
    {
    public :
	SeqPos () {} ; ///< Constructor (empty)
	virtual ~SeqPos () {} ; ///< Dummy destructor
	
    virtual void cleanup () ; ///< Empty the class
    virtual void add ( int np , int x , int y , int w , int h , bool memsave = false ) ; ///< Add a "char"
    virtual void addline ( int from , int to , int vfrom , int vto ) ; ///< Add a "virtual" line (for faster seeking where the mouse cursor is over)
    virtual int getLine ( int y ) ; ///< Gets a line for the y position
    virtual int getItem ( wxPoint pt , int line ) ; ///< Gets the item in the line for the position
    virtual void reserve ( int n , int n2 = -1 , bool memsave = false ) ; ///< Reserves memory (faster for huge sequences)
    
    virtual void mark ( int where , int value ) ; ///< Mark part of the sequence
    virtual int getmark ( int where ) ; ///< The "type" of marikg at the position

    wxArrayInt p ; ///< The data
    wxString m ; ///< The marking data
    vector <wxRect> r ; ///< Bounding rectangles for the data
    vector <wxRect> l ; ///< Bounding rectangles for the lines
    
    private :
    wxArrayInt mark_from , mark_to ;
    int mark_value ;
    } ;

/** \brief The parent class for all sequence types, including DNA, amino acids, features, restriction sites, ABI peaks, etc.
*/
class SeqBasic
    {
    public :
    SeqBasic ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; } ///< Constructor
    virtual ~SeqBasic () ; ///< Destructor
    virtual void init ( SequenceCanvas *ncan = NULL ) ; ///< Initialization
    virtual void initFromTVector ( TVector *v ) {} ///< Set from a TVector class
    virtual int  arrange ( int n ) { return 0 ; } ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) {} ; ///< Show
    virtual wxPoint showText ( int ystart , wxArrayString &tout )
        { return wxPoint ( -1 , -1 ) ; } ; ///< Show as text (rarely used)
    virtual wxString whatsthis () { return _T("BASIC") ; } ///< Returns the linetype
    virtual void makeEndnumberLength() { endnumberlength = 0 ; } ; ///< Calculates the width needed for the leading numbers
    
    virtual bool useDirectRoutines () { return false ; } ///< Do we draw directly (or do we use SeqPos)?
    virtual int getMarkSize () { return pos.m.length() ; } ///< Returns the length of the SeqPos marked part of the sequence
    virtual int getRectSize () { return pos.r.size() ; } ///< Returns the number of SeqPos rectangles
    virtual wxRect getRect ( int i ) { return pos.r[i] ; } ///< Returns the SeqPos rect for a "char"
    virtual int getMark ( int i ) { return pos.m.GetChar(i) - ' ' ; } ///< Returns the mark value for a "char"
    virtual void setMark ( int i , int v ) { pos.m.SetChar ( i , v + ' ' ) ; } ///< Sets the marking of a "char"
    virtual int getPos ( int i ) { return pos.p[i] ; } ///< Returns the internal ID of the "char"
    virtual void setPos ( int i , int v ) { pos.p[i] = v ; } ///< Sets the internal ID of the "char"
    virtual int getLine ( int y ) { return pos.getLine ( y ) ; } ///< Returns the line number for the y position
    virtual int getItem ( wxPoint pt , int line ) { return pos.getItem ( pt , line ) ; } ///< Returns the "char" at that position
    virtual bool isDisplayOnly () { return false ; } ///< Do we show something?
    virtual void logsize () ; ///< Some memory calculation for debugging, I think...
    virtual void editMode ( bool on = true ) ; ///< Set edit mode for this sequence line
    
    // Variables
    wxString s ; ///< The sequence as a text; may be blank for some linetypes
    int offset , endnumberlength , itemsperline ;
    SequenceCanvas *can ; ///< The containing SequenceCanvas
    bool takesMouseActions , shown ;
    
    protected :
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; } ///< Arrange quickly (bypassing SeqPos)
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ; ///< Show quickly

    SeqPos pos ; ///< Position class (empty when using direct routines)
    } ;

/** \brief Sequence display class showing numbers "inline"
*/
class SeqNum : public SeqBasic
    {
    public :
    SeqNum ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; } ///< Constructor
    virtual wxString whatsthis () { return _T("NUM") ; } ///< Returns the linetype
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual bool isDisplayOnly () { return true ; } ///< Do we show something?
    } ;

/** \brief Sequence display class showing a horizontal divider
*/
class SeqDivider : public SeqBasic
    {
    public :
    SeqDivider ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; } ///< Constructor
    virtual wxString whatsthis () { return _T("DIVIDER") ; } ///< Returns the linetype
    virtual void initFromTVector ( TVector *v ) ; ///< Set from a TVector class
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual bool isDisplayOnly () { return true ; } ///< Do we show something?
    } ;
    
/** \brief Sequence display class showing a blank line
*/
class SeqBlank : public SeqDivider
    {
    public :
    SeqBlank ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; } ///< Constructor
    virtual wxString whatsthis () { return _T("BLANK") ; } ///< Returns the linetype
    virtual void show ( wxDC& dc ) {} ; ///< Show
    virtual bool isDisplayOnly () { return true ; } ///< Do we show something?
    } ;

/** \brief Sequence display class showing DNA
*/
class SeqDNA : public SeqBasic
    {
    public :
    SeqDNA ( SequenceCanvas *ncan = NULL ) { vec = NULL ; invers = false ; init ( ncan ) ; } ///< Constructor
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) ;
    virtual void initFromTVector ( TVector *v ) ; ///< Set from a TVector class
    virtual wxString whatsthis () { return invers ? _T("IDNA") : _T("DNA") ; } ///< Returns the linetype
    virtual wxPoint showText ( int ystart , wxArrayString &tout ) ; ///< Show as text (rarely used)
    virtual wxColor getBaseColor ( char b ) ; ///< Returns the color to draw a base/nucleotide in
    virtual int arrange_direct ( int n ) ; ///< Arrange quickly (bypassing SeqPos)
    virtual void show_direct ( wxDC& dc ) ; ///< Show quickly
    virtual void makeEndnumberLength() ; ///< Calculates the width needed for the leading numbers

    virtual bool useDirectRoutines () ; ///< Do we draw directly (or do we use SeqPos)?
    virtual int getMarkSize () ; ///< Returns the length of the SeqPos marked part of the sequence
    virtual int getRectSize () ; ///< Returns the number of SeqPos rectangles
    virtual wxRect getRect ( int i ) ; ///< Returns the SeqPos rect for a "char"
    virtual int getMark ( int i ) ; ///< Returns the mark value for a "char"
    virtual void setMark ( int i , int v ) ; ///< Sets the marking of a "char"
    virtual int getPos ( int i ) ; ///< Returns the internal ID of the "char"
    virtual void setPos ( int i , int v ) ; ///< Sets the internal ID of the "char"
    virtual int getLine ( int y ) ; ///< Returns the line number for the y position
    virtual int getItem ( wxPoint pt , int line ) ; ///< Returns the "char" at that position
        
    // Variables
    TVector *vec ; ///< Pointer to the vector containing the DNA sequence
    bool showNumbers ; ///< Show leading numbers? (Not for IDNA, for example)
    wxColour fontColor ; ///< The default font color
    wxString alternateName ; ///< An alternate name, to display instead of the leading numbers
    bool invers ; ///< This sequence is inverse DNA
    } ;

/** \brief Sequence display class showing primers
*/
class SeqPrimer : public SeqDNA
    {
    public :
    SeqPrimer ( SequenceCanvas *ncan = NULL ) { 
        vec = NULL ; 
        init ( ncan ) ; 
        myname = _T("PRIMER") ; } ///< Constructor
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual void initFromTVector ( TVector *v ) ; ///< Set from a TVector class
    virtual void addPrimer ( TPrimer *p ) ; ///< Adds a primer to the sequence
    virtual wxString whatsthis () { return myname ; } ///< Returns the linetype
    virtual int arrange_direct ( int n ) ; ///< Arrange quickly (bypassing SeqPos)
    virtual void show_direct ( wxDC& dc ) ; ///< Show quickly
    virtual bool useDirectRoutines () ; ///< Do we draw directly (or do we use SeqPos)?
    
    // Variables
    wxString myname ; ///< Alternative name
    } ;

/** \brief Sequence display class showing alignments
*/
class SeqAlign : public SeqBasic
    {
    public :
    SeqAlign ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; myname = _T("Align") ; id = -1 ; } ///< Constructor
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual wxString whatsthis () { return _T("ALIGN") ; } ///< Returns the linetype
    virtual void makeEndnumberLength() ; ///< Calculates the width needed for the leading numbers
    
    // Variables
    wxString myname ; ///< Alternative name
    int id ; ///< Internal counter to differentiate several alignment lines
    } ;

/** \brief Sequence display class showing restriction sites
*/
class SeqRestriction : public SeqBasic
    {
    public :
    SeqRestriction ( SequenceCanvas *ncan = NULL ) { vec = NULL ; init ( ncan ) ; } ///< Constructor
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual void initFromTVector ( TVector *v ) ; ///< Set from a TVector class
    virtual wxString whatsthis () { return _T("RESTRICTION") ; } ///< Returns the linetype
    virtual bool isDisplayOnly () { return true ; } ///< The hell if I know what this does!
    virtual bool useDirectRoutines () ; ///< Do we draw directly (or do we use SeqPos)?
    
    // Variables
    TVector *vec ; ///< Pointer to the original sequence data
    bool down ; ///< Is this below the matching SeqDNA line (or above)?
    SequencePartList pl ; ///< Some cache thingy
    } ;

/** \brief Sequence display class showing amino acids
*/
class SeqAA : public SeqBasic
    {
    public :
    SeqAA ( SequenceCanvas *ncan = NULL ) {
        vec = NULL ;
        init ( ncan ) ; 
        mode = AA_ALL ; 
        primaryMode = false ; 
        unknownAA = '?' ; } ///< Constructor
    virtual ~SeqAA () ; ///< Destructor
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual wxPoint showText ( int ystart , wxArrayString &tout ) ; ///< Show as text (rarely used)
    virtual void initFromTVector ( TVector *v ) ; ///< Set from a TVector class
    virtual void initFromString ( wxString t ) ; ///< Set from a string
    virtual void analyzeProteases () ; ///< Find protease cuts
    virtual void updateProteases () ; ///< Set protease cuts in display
    virtual void fixOffsets ( TVector *v ) ; ///< Feature offsets fix (see SeqFeature)
    virtual wxString whatsthis () { return _T("AA") ; } ///< Returns the linetype
    virtual int arrange_direct ( int n ) ; ///< Arrange quickly (bypassing SeqPos)
    virtual void show_direct ( wxDC& dc ) ; ///< Show quickly
    virtual bool useDirectRoutines () ; ///< Do we draw directly (or do we use SeqPos)?
    virtual int getMarkSize () ; ///< Returns the length of the SeqPos marked part of the sequence
    virtual int getRectSize () ; ///< Returns the number of SeqPos rectangles
    virtual wxRect getRect ( int i ) ; ///< Returns the SeqPos rect for a "char"
    virtual int getMark ( int i ) ; ///< Returns the mark value for a "char"
    virtual void setMark ( int i , int v ) ; ///< Sets the marking of a "char"
    virtual int getPos ( int i ) ; ///< Returns the internal ID of the "char"
    virtual void setPos ( int i , int v ) ; ///< Sets the internal ID of the "char"
    virtual int getLine ( int y ) ; ///< Returns the line number for the y position
    virtual int getItem ( wxPoint pt , int line ) ; ///< Returns the "char" at that position
    virtual void logsize () ; ///< Some internal debugging thingy
    
    // Variables
    TVector *vec ; ///< Pointer to the original sequence data
    int mode , disp ;
    bool primaryMode , showNumbers ;
    char unknownAA ;
    wxArrayTProteaseCut pc ; ///< List of protease cuts
    wxString pa_w ;
    wxArrayTProtease proteases ; ///< List of proteases to look for
    wxArrayInt offsets , pa_wa ;
    wxArrayTVectorItem offset_items ;
    } ;

/** \brief Sequence display class showing ABI sequencer peaks
*/
class SeqABI : public SeqDNA
    {
    public :
    SeqABI ( SequenceCanvas *ncan = NULL ) { at = NULL ; inv_compl = false ; init ( ncan ) ; } ///< Constructor
    virtual ~SeqABI () ; ///< Destructor
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual void initFromFile ( wxString filename ) ; ///< Setup sequence from ABI file
    virtual wxString whatsthis () { return _T("ABI") ; } ///< Returns the linetype
    virtual void drawTopLine ( wxDC &dc , int y ) ; ///< Some layout thingy
    virtual wxColor getBaseColor ( char b ) ; ///< Returns the color to draw the base in
    virtual void setInvCompl ( bool x ) ; ///< Display inverse/complement sequence
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; } ///< Arrange quickly (bypassing SeqPos)
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ; ///< Show quickly
    virtual bool useDirectRoutines () { return false ; } ///< Do we draw directly (or do we use SeqPos)?
    
    // Variables
    ABItype *at ; ///< Pointer to the calling ABI module
    int minx , maxx ;
    int scalex ;
    char base2color[4] ;
    wxColour colors[4] ;
    bool showHelpLines ; ///< Do we show these little grey helper lines?
    int screenScale ;
    int zoom ; ///< Zoom factor
    bool inv_compl ; ///< Use inverse/complement?
    } ;

/** \brief Sequence display class showing annotated features
*/
class SeqFeature : public SeqDNA
    {
    public :
    SeqFeature ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; aaa = NULL ; } ///< Constructor
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual wxString whatsthis () { return _T("FEATURE") ; } ///< Returns the linetype
    virtual void initFromTVector ( TVector *v ) ; ///< Set from a TVector class
    virtual bool collide ( int a , int b ) ; ///< Do features #a and #b overlap?
    virtual bool doesHit ( int a , int x ) ; ///< Is position x within feature #a
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; } ///< Arrange quickly (bypassing SeqPos)
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ; ///< Show quickly
    virtual bool useDirectRoutines () { return false ; } ///< Do we draw directly (or do we use SeqPos)?
    virtual bool isDisplayOnly () { return true ; } ///< WTF??
    
    // Variables
    vector <wxRect> vr ;
    vector <wxPen> pens ; ///< Different pens (colors) to use
    int maxlayers ; ///< Number of layers/levels
    SeqAA *aaa ; ///< Pointer to the associated amino acid sequence
    int id ; ///< Only used by TAlignment : lines[id]
    SequencePartList pl ; ///< List of (overlapping?) parts
    } ;

/** \brief Sequence display class showing plot data in the amino acid module
*/
class SeqPlot : public SeqDNA
    {
    public :
    SeqPlot ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; lines = 0 ; } ///< Constructor
    virtual void show ( wxDC& dc ) ; ///< Show
    virtual wxString whatsthis () { return _T("PLOT") ; } ///< Returns the linetype
    virtual void initFromTVector ( TVector *v ) ; ///< Set from a TVector class
    virtual int  arrange ( int n ) ; ///< Arrange "chars" as line n
    virtual void setLines ( int l ) ; ///< What line(s) this spans
    virtual void useChouFasman () ; ///< Chou-Fasman-Plot
    virtual void useNcoils () ; ///< Coiled-coil-Plot
    virtual void useMW () ; ///< Molecular weight plot
    virtual void usePI () ; ///< Isoelectric point plot
    virtual void useHP () ; ///< Hydrophobicity plot
    virtual void showPlot ( wxDC &dc , int b , int tx , int ty , int lx , int ph ) ; ///< Draws the plot
    virtual void init ( SequenceCanvas *ncan = NULL ) ; ///< Initialization
    virtual wxString getTip ( int pos ) ; ///< Returns the tooltip text for this position in the sequence
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; } ///< Arrange quickly (bypassing SeqPos)
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ; ///< Show quickly
    virtual bool useDirectRoutines () { return false ; } ///< Do we draw directly (or do we use SeqPos)?

    private :
    virtual void scanMinMax () ; ///< Determine minimum/maximum values
    virtual void scanChouFasman ( int x , int y , int t , int min , 
                                    int seek_cnt , int seek_avg , int avg ) ;
    virtual void drawSymbol ( char c , wxDC &dc , int x1 , int y1 , int x2 , int y2 ) ;
    virtual void showChouFasman ( wxDC &dc , int b , int tx , int ty , int lx ) ; ///< Display Chou-Fasman
    virtual void showNcoils ( wxDC &dc , int b , int tx , int ty , int lx ) ; ///< Display Coiled-coil
    virtual void showMW ( wxDC &dc , int b , int tx , int ty , int lx ) ; ///< Display molecular weight
    virtual void showPI ( wxDC &dc , int b , int tx , int ty , int lx ) ; ///< Display isoelectric point
    virtual void showHP ( wxDC &dc , int b , int tx , int ty , int lx ) ; ///< Display hydrophobicity
    virtual void fixMinMax ( float &f ) ; ///< What the hell does this do??
    virtual void drawDottedLine ( wxDC &dc , int x1 , int y1 , int x2 , int y2 ) ; ///< Draws a horizontal/vertical helper line
    virtual void myRect ( wxDC &dc , int x , int y , int w , int h ) ; ///< Draws a "special" rectangle
    enum { CHOU_FASMAN , P_I , M_W , H_P , COILED_COIL } type ;
    int lines , l_top, l_bottom ;
    wxArrayString d1 , d2 , d3 ;
    vector <TAAProp> prop ;
    bool startOfLine ;
    float data_max , data_min , data_h , data_step ;
    wxRect plotr ;
    int hp_window ;
    wxString hp_method ;
    } ;

/** \brief The main sequence display class

	This class gets filled with "sequence lines", which are descendants of SeqBasic.
	It manages the "total" layout, mouse and keyboard actions, scrolling,
	and context menus.
*/
class SequenceCanvas : public wxScrolledWindow
    {
    public:
    SequenceCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size); ///< Constructor
    virtual ~SequenceCanvas () ; ///< Destructor
    virtual void OnDraw(wxDC& dc); ///< Draws the sequence
    virtual void safeShow ( wxDC &dc ) ; ///< Draws the sequence in a memory device context, then blits it over to the "real" one
    virtual void OnEvent(wxMouseEvent& event); ///< Mouse event handler
    virtual void OnSize(wxSizeEvent &event); ///< Resize event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    virtual void vecEdit ( wxCommandEvent &ev ) ; ///<  Edit event handler
    virtual void blastDNA ( wxCommandEvent &ev ) ; ///<  "BLAST DNA" event handler
    virtual void blastAA ( wxCommandEvent &ev ) ; ///<  "BLAST amino acids" event handler
    virtual void OnCopyText ( wxCommandEvent &ev ) ; ///<  "Copy as text" event handler
    virtual void OnCopyImage ( wxCommandEvent &ev ) ; ///<  "Copy as image" event handler
    virtual void OnSaveImage ( wxCommandEvent &ev ) ; ///<  "Save as image" event handler
    virtual void OnPrint ( wxCommandEvent &ev ) ; ///<  Printing event handler
    virtual void OnCut ( wxCommandEvent &ev ) ; ///<  Cut event handler
    virtual void OnCopy ( wxCommandEvent &ev ) ; ///<  Copy event handler
    virtual void OnPrimerForward ( wxCommandEvent &ev ) ; ///<  "Run PCR with forward primer" event handler
    virtual void OnPrimerBackward ( wxCommandEvent &ev ) ; ///<  "Run PCR with backward primer" event handler
    virtual void OnPrimerBoth ( wxCommandEvent &ev ) ; ///<  "Run PCR with both primers" event handler
    virtual void OnPrimerMutation ( wxCommandEvent &ev ) ; ///<  "Run PCR with mutation primers" event handler
    virtual void OnSilmut ( wxCommandEvent &ev ) ; ///<  "Silent mutation" event handler
    virtual void OnWhatCuts(wxCommandEvent& event); ///<  "Find restriction enzymes that cut" event handler
    virtual void OnSelAsNewPrimer( wxCommandEvent &ev ) ; ///<  "Selection as new primer" event handler

    virtual void OnViewOriginal ( wxCommandEvent &ev ) ; ///<  "View original" mini-display event handler
    virtual void OnViewReal ( wxCommandEvent &ev ) ; ///<  View 1:1" mini-display event handler
    virtual void OnViewCondensed ( wxCommandEvent &ev ) ; ///<  View condensed" mini-display event handler
    virtual void OnBacktranslate ( wxCommandEvent &ev ) ; ///<  "Backtranslate amino acids into DNA" event handler

    virtual void OnPhylip ( wxCommandEvent &ev ) ; ///<  "Phylip" event handler (all in one)

    virtual void OnSeqUp ( wxCommandEvent &ev ) ; ///<  "Move alignment sequence up" event handler
    virtual void OnSeqDown ( wxCommandEvent &ev ) ; ///<  "Move alignment sequence down" event handler
    virtual void OnSeqTop ( wxCommandEvent &ev ) ; ///<  "Move alignment sequence to top" event handler
    virtual void OnSeqBottom ( wxCommandEvent &ev ) ; ///<  "Move alignment sequence to bottom" event handler
    virtual void OnToggleFeat ( wxCommandEvent &ev ) ; ///<  "Toggle feature display for this alignment sequence" event handler
    
    virtual void OnCopyResultDNA ( wxCommandEvent &ev ) ; ///<  "Copy DNA resulting from PCR" event handler
    virtual void OnCopyResultAA ( wxCommandEvent &ev ) ; ///<  "Copy amino acids for DNA resulting from PCR" event handler
    virtual void OnNewFromResultDNA ( wxCommandEvent &ev ) ; ///<  "New DNA module resulting from PCR" event handler
    virtual void OnNewFromResultAA ( wxCommandEvent &ev ) ; ///<  "New amino acids module for DNA resulting from PCR" event handler
    
    virtual void OnInsertGap ( wxCommandEvent &ev ) ; ///<  "Insert gap into alignment sequence" event handler
    virtual void OnDeleteGap ( wxCommandEvent &ev ) ; ///<  "Delete gap from alignment sequence" event handler
    virtual void OnInsertOtherGaps ( wxCommandEvent &ev ) ; ///<  "Insert gap into all other alignment sequences" event handler
    virtual void OnDeleteOtherGaps ( wxCommandEvent &ev ) ; ///<  "Delete gap from all other alignment sequences" event handler

    virtual void OnStrandCopy35(wxCommandEvent& event); ///<  "Copy the 3'->5' strand" event handler
    virtual void OnStrandCopy53(wxCommandEvent& event); ///<  "Copy the 5'->3' strand" event handler
    virtual void OnStrandCopyBoth(wxCommandEvent& event); ///<  "Copy both strands as a text" event handler
    virtual void OnStrandNew35(wxCommandEvent& event); ///<  "Generate new DNA module from 3'->5' strand" event handler
    virtual void OnStrandNew53(wxCommandEvent& event); ///<  "Generate new DNA module from 5'->3' strand" event handler
    virtual void OnStrandNewBoth(wxCommandEvent& event); ///<  "Generate new DNA modules from both strands" event handler

    virtual void OnInsertRestrictionSiteLeft ( wxCommandEvent &ev ) ; ///<  "Insert restriction site left of the marking" event handler
    virtual void OnInsertRestrictionSiteRight ( wxCommandEvent &ev ) ; ///<  "Insert restriction site right of the marking" event handler

    virtual void OnPaste ( wxCommandEvent &ev ) ; ///<  Paste event handler
    virtual void OnFocus(wxFocusEvent& event) ; ///<  "Display gets focus" event handler
    virtual void OnKillFocus(wxFocusEvent& event) ; ///<  "Display loses focus" event handler
    virtual void rsHideLimit ( wxCommandEvent &ev ) ; ///<  "Limit restriction enzymes" event handler
    
    virtual wxString getSelection () ; ///< Returns the current selection as a wxString
    
    virtual void updateEdit ( TVector *v , wxString id , int from ) ; ///< Updates the sequences and display once a key was pressed and, thus, the sequence altered
    virtual void arrange () ; ///< Arranges tghe layout for all the seq structures
    virtual SeqBasic* findMouseTarget ( wxPoint pt , int &pos ) ; ///< Returns a pointer to the seq structure (and the position inside) the given point is within
    virtual int findMouseTargetItem ( wxPoint pt ) ; ///< Returns the ID of the seq structure the given point is within
    virtual SeqBasic* findID ( wxString id ) ; ///< Returns the pointer to the seq structure with the given ID string
    virtual void mark ( wxString id , int from , int to , int value = 1 ) ; ///< Marks part of a sequence
    virtual bool inMarkRange ( int x , int f , int t , int l ) ; ///< Is this span withing the marked range?
    virtual void ensureVisible ( int pos ) ; ///< Make sure the position is in the visible part of the display; scroll if necessary
    virtual int getBatchMark () ; ///< I forgot what this does
    virtual void MyGetClientSize ( int *w , int *h ) ; ///< Wrapper function; will compensate for printing device context
    virtual void MyGetSize ( int *w , int *h ) ; ///< Wrapper function; will compensate for printing device context
    virtual wxSize MyGetClientSize () ; ///< Wrapper function; will compensate for printing device context
    virtual void MyGetViewStart ( int *x , int *y ) ; ///< Wrapper function; will compensate for printing device context
    virtual void SilentRefresh () ; ///< Refresh without cleaning background first (no flicker)
    virtual bool doOverwrite () ; ///< Are we in edit overwrite mode?
    virtual TAminoAcids *getAA() ; ///< Returns pointer to amino acid module, or NULL
    virtual TPrimerDesign *getPD() ; ///< Returns pointer to primer design module, or NULL
    virtual TAlignment *getAln() ; ///< Returns pointer to alignment module, or NULL
    
    virtual bool getPrintToColor () { return printToColor ; } ///< Are we printing to a color printer?
    virtual void setPrintToColor ( bool _b ) { printToColor = _b ; } ///< We are printing to a color printer!
    virtual bool getDrawAll () { return drawall ; } ///< Are we drawing everything ( not just the visible part)?
    virtual void setDrawAll ( bool _b ) { drawall = _b ; } ///< We are drawing everything ( not just the visible part)
    virtual bool isHorizontal () { return horizontal ; } ///< Is the display set to horizontal mode?
    virtual void setHorizontal ( bool _b ) { horizontal = _b ; } ///< Set the display to horizontal mode
    virtual void toggleHorizontal () { setHorizontal ( !isHorizontal() ) ; } ///< Toggle horizontal/vertical mode
    virtual bool getHide () { return hide ; } ///< Is the display hidden (so we don't have to draw anything)?
    virtual void doHide ( bool _b ) { hide = _b ; } ///< Hide the display
    virtual bool isMiniDisplay () { return miniDisplay ; } ///< Is this a mini display in the amino acid module?
    virtual void setMiniDisplay ( bool _b ) { miniDisplay = _b ; } ///< This is a mini display in the amino acid module
    virtual SeqBasic *getLastWhere() { return lastwhere ; } ///< Returns a pointer to the last marked seq structure
    virtual bool isPrinting () { return printing ; } ///< Are we printing (or drawing on the screen)?
    virtual void forceOverwrite ( bool _b ) { forceoverwrite = _b ; } ///< Ensure overwrite mode (for example, ABI and PCR mode)
    virtual bool getEditMode () { return editMode ; } ///< Are we editing?
    virtual void setEditMode ( bool _b ) { editMode = _b ; } ///< Sets wether we're editing
    virtual void setLowX ( int _i ) { if ( lowx < _i ) lowx = _i ; } ///< Minimum X coodrinate to draw
    virtual void startEdit ( wxString id ) ; ///< Initiate editing mode
    virtual void stopEdit () ; ///< Terminate editing mode
    virtual int markedFrom () { return _from ; } ///< Return first marked position, or -1
    virtual int markedTo () { return _to ; } ///< Return last marked position, or -1
    virtual void unmark () ; ///< Removes any marking
    virtual int NumberOfLines() { return seq.GetCount() + blankline ; } ///< Returns the number of seq struntures AND blank lines

    ChildBase *child ; ///< Wanna-be universal pointer to the containing module
    MyChild *p ; ///< Pointer to the containing MyChild; if this is not within the DNA module, NULL
    int blankline , charwidth , charheight , hardstop ;
    int lastmarked , maxendnumberlength , lastyoffset , blocksize , border ;
    wxArraySeqBasic seq ; ///< The list of SeqBasic structures
    wxFont *font , *smallFont , *varFont;
    wxString edit_id , edit_valid ;
    
    private :
    virtual wxBitmap *getSequenceBitmap () ; ///< Returns the sequences display as a bitmap
    virtual TVector *getPCR_DNA_vector() ; ///< Returns a pointer to the DNA vector in PCR mode; otherwise, NULL
    virtual void showContextMenu ( SeqBasic *where , int pos , wxPoint pt ) ; ///< Generates the context menu
    virtual void insertRestrictionSite ( bool left ) ; ///< Creates a new restriction site via simulated editing, left or right of the current mark
    virtual void editSequence ( int k , wxKeyEvent& event ) ;
    virtual void editCharPressed ( int k , TVector *v , wxString *the_sequence ) ;
    virtual void editSpecialKeyPressed ( int k , TVector *v , wxString *the_sequence , int wy , wxKeyEvent& event ) ;

    bool printToColor , drawall , horizontal , hide , miniDisplay ;
    bool marking , drawing , printing , wantOverwrite , forceoverwrite ;
    bool editMode ;
    wxDC *print_dc ;
    int _from , _to ; // Mark
    int lastclick , lowx , lowy , lastpos , print_maxx , vpx , vpy ;
    wxString lastToolTip ;
    SeqAlign *last_al ;
    SeqBasic *lastwhere ;
    
    DECLARE_EVENT_TABLE()
    };

/** \brief This helper class of SequenceCanvas can memorize a marking of the sequences, and restore it
*/
class TMarkMem
    {
    public :
    TMarkMem ( SequenceCanvas *_sc = NULL ) ; ///< Constructor
	virtual ~TMarkMem () {} ; ///< Dummy destructor
    virtual void unmark () ; ///< Resets the marking
    virtual void remark () ; ///< Reapplies the stored marking
    
    private :
    SequenceCanvas *sc ;
    int f , t , l ;
    } ;

#endif

