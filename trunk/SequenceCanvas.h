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

// This class assists in the restriction site layout
class SequencePartList
    {
    public :
    SequencePartList () { maxlevels = slen = 0 ; } ;
    virtual void prepare ( int size ) ;
    virtual void add ( int id , int from , int to ) ;
    virtual void makeLevels () ;
    virtual int here ( int pos , int level ) ;
    virtual int getID ( int internalID ) ;
    virtual int getFrom ( int internalID ) ;
    virtual int getTo ( int internalID ) ;
    virtual int size () ;
    virtual int getLevel ( int i ) ;

    int maxlevels , slen ;
    
    private :
    wxArrayInt vi , vl , vx , vy ;
    vector <wxArrayInt> vl2 ;
    } ;    
    
// SeqPos manages the positions of all items for a single "type" (e.g., DNA)
// The vectors p, r and m are always the same size, and contain information
//   about the same item at the same index:
// p contains the seqeunce position matching the item
// r contains the visual position of the item
// m contains the marking of the item:
//   0 = not marked
//   1 = marked
//   2 = cursor in edit mode
//
// Vector l contains additional positioning information, namely the visual
//   position of a horizontal line of items, for faster access when trying
//   to determine the target of a mouse click
class SeqPos
    {
    public :
    wxArrayInt p ;
    wxString m ;
    vector <wxRect> r ;
    vector <wxRect> l ;
    
    virtual void cleanup () ;
    virtual void add ( int np , int x , int y , int w , int h , bool memsave = false ) ;
    virtual void addline ( int from , int to , int vfrom , int vto ) ;
    virtual int getLine ( int y ) ;
    virtual int getItem ( wxPoint pt , int line ) ;
    virtual void reserve ( int n , int n2 = -1 , bool memsave = false ) ;
    
    virtual void mark ( int where , int value ) ;
    virtual int getmark ( int where ) ;
    
    private :
    wxArrayInt mark_from , mark_to ;
    int mark_value ;
    } ;

class SeqBasic
    {
    public :
    SeqBasic ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; }
    virtual ~SeqBasic () ;
    virtual void init ( SequenceCanvas *ncan = NULL ) ;
    virtual void initFromTVector ( TVector *v ) {} // Dummy
    virtual int  arrange ( int n ) { return 0 ; } ;
    virtual void show ( wxDC& dc ) {} ;
    virtual wxPoint showText ( int ystart , wxArrayString &tout )
        { return wxPoint ( -1 , -1 ) ; } ;
    virtual wxString whatsthis () { return "BASIC" ; }
    virtual void makeEndnumberLength() { endnumberlength = 0 ; } ;
    
    virtual bool useDirectRoutines () { return false ; }
    virtual int getMarkSize () { return pos.m.length() ; }
    virtual int getRectSize () { return pos.r.size() ; }
    virtual wxRect getRect ( int i ) { return pos.r[i] ; }
    virtual int getMark ( int i ) { return pos.m.GetChar(i) - ' ' ; }
    virtual void setMark ( int i , int v ) { pos.m.SetChar ( i , v + ' ' ) ; }
    virtual int getPos ( int i ) { return pos.p[i] ; }
    virtual void setPos ( int i , int v ) { pos.p[i] = v ; }
    virtual int getLine ( int y ) { return pos.getLine ( y ) ; }
    virtual int getItem ( wxPoint pt , int line ) { return pos.getItem ( pt , line ) ; }
    virtual bool isDisplayOnly () { return false ; }
    virtual void logsize () ;
    virtual void editMode ( bool on = true ) ;
    
    // Variables
    wxString s ;
    int offset , endnumberlength , itemsperline ;
    SequenceCanvas *can ;
    bool takesMouseActions , shown ;
    
    protected :
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; }
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ;

    SeqPos pos ;
    } ;

class SeqNum : public SeqBasic
    {
    public :
    SeqNum ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; }
    virtual wxString whatsthis () { return "NUM" ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual bool isDisplayOnly () { return true ; }
    } ;

class SeqDivider : public SeqBasic
    {
    public :
    SeqDivider ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; }
    virtual wxString whatsthis () { return "DIVIDER" ; }
    virtual void initFromTVector ( TVector *v ) ;
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual bool isDisplayOnly () { return true ; }
    } ;
    
class SeqBlank : public SeqDivider
    {
    public :
    SeqBlank ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; }
    virtual wxString whatsthis () { return "BLANK" ; }
    virtual void show ( wxDC& dc ) {} ;
    virtual bool isDisplayOnly () { return true ; }
    } ;

class SeqDNA : public SeqBasic
    {
    public :
    SeqDNA ( SequenceCanvas *ncan = NULL ) { vec = NULL ; invers = false ; init ( ncan ) ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual void initFromTVector ( TVector *v ) ;
    virtual wxString whatsthis () { return invers ? "IDNA" : "DNA" ; }
    virtual wxPoint showText ( int ystart , wxArrayString &tout ) ;
    virtual wxColor getBaseColor ( char b ) ;
    virtual int arrange_direct ( int n ) ;
    virtual void show_direct ( wxDC& dc ) ;
    virtual void makeEndnumberLength() ;

    virtual bool useDirectRoutines () ;
    virtual int getMarkSize () ;
    virtual int getRectSize () ;
    virtual wxRect getRect ( int i ) ;
    virtual int getMark ( int i ) ;
    virtual void setMark ( int i , int v ) ;
    virtual int getPos ( int i ) ;
    virtual void setPos ( int i , int v ) ;
    virtual int getLine ( int y ) ;
    virtual int getItem ( wxPoint pt , int line ) ;
        
    // Variables
    TVector *vec ;
    bool showNumbers ;
    wxColour fontColor ;
    wxString alternateName ;
    bool invers ;
    } ;

class SeqPrimer : public SeqDNA
    {
    public :
    SeqPrimer ( SequenceCanvas *ncan = NULL ) { 
        vec = NULL ; 
        init ( ncan ) ; 
        myname = "PRIMER" ; }
    virtual void show ( wxDC& dc ) ;
    virtual void initFromTVector ( TVector *v ) ;
    virtual void addPrimer ( TPrimer *p ) ;
    virtual wxString whatsthis () { return myname ; }
    virtual int arrange_direct ( int n ) ;
    virtual void show_direct ( wxDC& dc ) ;
    virtual bool useDirectRoutines () ;
    
    // Variables
    wxString myname ;
    } ;

class SeqAlign : public SeqBasic
    {
    public :
    SeqAlign ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; myname = "Align" ; id = -1 ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual wxString whatsthis () { return "ALIGN" ; }
    virtual void makeEndnumberLength() ;
    
    // Variables
    wxString myname ;
    int id ;
    } ;

class SeqRestriction : public SeqBasic
    {
    public :
    SeqRestriction ( SequenceCanvas *ncan = NULL ) { vec = NULL ; init ( ncan ) ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual void initFromTVector ( TVector *v ) ;
    virtual wxString whatsthis () { return "RESTRICTION" ; }
    virtual bool isDisplayOnly () { return true ; }
    virtual bool useDirectRoutines () ;
    
    // Variables
    TVector *vec ;
    bool down ;
    SequencePartList pl ;
    } ;

class SeqAA : public SeqBasic
    {
    public :
    SeqAA ( SequenceCanvas *ncan = NULL ) {
        vec = NULL ;
        init ( ncan ) ; 
        mode = AA_ALL ; 
        primaryMode = false ; 
        unknownAA = '?' ; }
    virtual ~SeqAA () ;
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual wxPoint showText ( int ystart , wxArrayString &tout ) ;
    virtual void initFromTVector ( TVector *v ) ;
    virtual void initFromString ( wxString t ) ;
    virtual void analyzeProteases () ;
    virtual void updateProteases () ;
    virtual void fixOffsets ( TVector *v ) ;
    virtual wxString whatsthis () { return "AA" ; }
    virtual int arrange_direct ( int n ) ;
    virtual void show_direct ( wxDC& dc ) ;
    virtual bool useDirectRoutines () ;
    virtual int getMarkSize () ;
    virtual int getRectSize () ;
    virtual wxRect getRect ( int i ) ;
    virtual int getMark ( int i ) ;
    virtual void setMark ( int i , int v ) ;
    virtual int getPos ( int i ) ;
    virtual void setPos ( int i , int v ) ;
    virtual int getLine ( int y ) ;
    virtual int getItem ( wxPoint pt , int line ) ;
    virtual void logsize () ;
    
    // Variables
    TVector *vec ;
    int mode , disp ;
    bool primaryMode , showNumbers ;
    char unknownAA ;
    wxArrayTProteaseCut pc ;
    wxString pa_w ;
    wxArrayTProtease proteases ;
    wxArrayInt offsets , pa_wa ;
    wxArrayTVectorItem offset_items ;
    } ;

class SeqABI : public SeqDNA
    {
    public :
    SeqABI ( SequenceCanvas *ncan = NULL ) { at = NULL ; inv_compl = false ; init ( ncan ) ; }
    virtual ~SeqABI () ;
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual void initFromFile ( wxString filename ) ;
    virtual wxString whatsthis () { return "ABI" ; }
    virtual void drawTopLine ( wxDC &dc , int y ) ;
    virtual wxColor getBaseColor ( char b ) ;
    virtual void setInvCompl ( bool x ) ;
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; }
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ;
    virtual bool useDirectRoutines () { return false ; }
    
    // Variables
    ABItype *at ;
    int minx , maxx ;
    int scalex ;
    char base2color[4] ;
    wxColour colors[4] ;
    bool showHelpLines ;
    int screenScale ;
    int zoom ;
    bool inv_compl ;
    } ;

class SeqFeature : public SeqDNA
    {
    public :
    SeqFeature ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; aaa = NULL ; }
    virtual void show ( wxDC& dc ) ;
    virtual wxString whatsthis () { return "FEATURE" ; }
    virtual void initFromTVector ( TVector *v ) ;
    virtual bool collide ( int a , int b ) ;
    virtual bool doesHit ( int a , int x ) ;
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; }
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ;
    virtual bool useDirectRoutines () { return false ; }
    virtual bool isDisplayOnly () { return true ; }
    
    // Variables
    vector <wxRect> vr ;
    vector <wxPen> pens ;
    int maxlayers ;
    SeqAA *aaa ;
    int id ; // Only used by TAlignment : lines[id]
    SequencePartList pl ;
    } ;

class SeqPlot : public SeqDNA
    {
    public :
    SeqPlot ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; lines = 0 ; }
    virtual void show ( wxDC& dc ) ;
    virtual wxString whatsthis () { return "PLOT" ; }
    virtual void initFromTVector ( TVector *v ) ;
    virtual int  arrange ( int n ) ;
    virtual void setLines ( int l ) ;
    virtual void useChouFasman () ;
    virtual void useMW () ;
    virtual void usePI () ;
    virtual void useHP () ;
    virtual void showPlot ( wxDC &dc , int b , int tx , int ty , int lx , int ph ) ;
    virtual void init ( SequenceCanvas *ncan = NULL ) ;
    virtual wxString getTip ( int pos ) ;
    virtual int arrange_direct ( int n ) { return arrange ( n ) ; }
    virtual void show_direct ( wxDC& dc ) { show ( dc ) ; } ;
    virtual bool useDirectRoutines () { return false ; }

    private :
    virtual void scanMinMax () ;
    virtual void scanChouFasman ( int x , int y , int t , int min , 
                                    int seek_cnt , int seek_avg , int avg ) ;
    virtual void drawSymbol ( char c , wxDC &dc , int x1 , int y1 , int x2 , int y2 ) ;
    virtual void showChouFasman ( wxDC &dc , int b , int tx , int ty , int lx ) ;
    virtual void showMW ( wxDC &dc , int b , int tx , int ty , int lx ) ;
    virtual void showPI ( wxDC &dc , int b , int tx , int ty , int lx ) ;
    virtual void showHP ( wxDC &dc , int b , int tx , int ty , int lx ) ;
    virtual void fixMinMax ( float &f ) ;
    virtual void drawDottedLine ( wxDC &dc , int x1 , int y1 , int x2 , int y2 ) ;
    virtual void myRect ( wxDC &dc , int x , int y , int w , int h ) ;
    enum { CHOU_FASMAN , P_I , M_W , H_P } type ;
    int lines , l_top, l_bottom ;
    wxArrayString d1 , d2 , d3 ;
    vector <TAAProp> prop ;
    bool startOfLine ;
    float data_max , data_min , data_h , data_step ;
    wxRect plotr ;
    int hp_window ;
    wxString hp_method ;
    } ;

class SequenceCanvas : public wxScrolledWindow
    {
    public:
    SequenceCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual ~SequenceCanvas () ;
    virtual void OnDraw(wxDC& dc);
    virtual void safeShow ( wxDC &dc ) ;
    virtual void OnEvent(wxMouseEvent& event);
    virtual void OnSize(wxSizeEvent &event);
    virtual void OnCharHook(wxKeyEvent& event) ;

    virtual void vecEdit ( wxCommandEvent &ev ) ;
    virtual void blastDNA ( wxCommandEvent &ev ) ;
    virtual void blastAA ( wxCommandEvent &ev ) ;
    virtual void OnCopyText ( wxCommandEvent &ev ) ;
    virtual void OnCopyImage ( wxCommandEvent &ev ) ;
    virtual void OnSaveImage ( wxCommandEvent &ev ) ;
    virtual void OnPrint ( wxCommandEvent &ev ) ;
    virtual void OnCut ( wxCommandEvent &ev ) ;
    virtual void OnCopy ( wxCommandEvent &ev ) ;
    virtual void OnPrimerForward ( wxCommandEvent &ev ) ;
    virtual void OnPrimerBackward ( wxCommandEvent &ev ) ;
    virtual void OnPrimerBoth ( wxCommandEvent &ev ) ;
    virtual void OnPrimerMutation ( wxCommandEvent &ev ) ;
    virtual void OnSilmut ( wxCommandEvent &ev ) ;
    virtual void OnWhatCuts(wxCommandEvent& event);
    virtual void OnSelAsNewPrimer( wxCommandEvent &ev ) ;

    virtual void OnViewOriginal ( wxCommandEvent &ev ) ;
    virtual void OnViewReal ( wxCommandEvent &ev ) ;
    virtual void OnViewCondensed ( wxCommandEvent &ev ) ;
    virtual void OnBacktranslate ( wxCommandEvent &ev ) ;

    virtual void OnSeqUp ( wxCommandEvent &ev ) ;
    virtual void OnSeqDown ( wxCommandEvent &ev ) ;
    virtual void OnSeqTop ( wxCommandEvent &ev ) ;
    virtual void OnSeqBottom ( wxCommandEvent &ev ) ;
    virtual void OnToggleFeat ( wxCommandEvent &ev ) ;
    
    virtual void OnCopyResultDNA ( wxCommandEvent &ev ) ;
    virtual void OnCopyResultAA ( wxCommandEvent &ev ) ;
    virtual void OnNewFromResultDNA ( wxCommandEvent &ev ) ;
    virtual void OnNewFromResultAA ( wxCommandEvent &ev ) ;
    
    virtual void OnInsertGap ( wxCommandEvent &ev ) ;
    virtual void OnDeleteGap ( wxCommandEvent &ev ) ;
    virtual void OnInsertOtherGaps ( wxCommandEvent &ev ) ;
    virtual void OnDeleteOtherGaps ( wxCommandEvent &ev ) ;

    virtual void OnStrandCopy35(wxCommandEvent& event);
    virtual void OnStrandCopy53(wxCommandEvent& event);
    virtual void OnStrandCopyBoth(wxCommandEvent& event);
    virtual void OnStrandNew35(wxCommandEvent& event);
    virtual void OnStrandNew53(wxCommandEvent& event);
    virtual void OnStrandNewBoth(wxCommandEvent& event);

    virtual void OnInsertRestrictionSiteLeft ( wxCommandEvent &ev ) ;
    virtual void OnInsertRestrictionSiteRight ( wxCommandEvent &ev ) ;

    virtual void OnPaste ( wxCommandEvent &ev ) ;
    virtual void OnFocus(wxFocusEvent& event) ;
    virtual void OnKillFocus(wxFocusEvent& event) ;
    virtual void rsHideLimit ( wxCommandEvent &ev ) ;
    
    virtual wxString getSelection () ;
    
    virtual void updateEdit ( TVector *v , wxString id , int from ) ;
    virtual void arrange () ;
    virtual SeqBasic* findMouseTarget ( wxPoint pt , int &pos ) ;
    virtual int findMouseTargetItem ( wxPoint pt ) ;
    virtual SeqBasic* findID ( wxString id ) ;
    virtual void mark ( wxString id , int from , int to , int value = 1 ) ;
    virtual bool inMarkRange ( int x , int f , int t , int l ) ;
    virtual void ensureVisible ( int pos ) ;
    virtual int getBatchMark () ;
    virtual void MyGetClientSize ( int *w , int *h ) ;
    virtual void MyGetSize ( int *w , int *h ) ;
    virtual wxSize MyGetClientSize () ;
    virtual void MyGetViewStart ( int *x , int *y ) ;
    virtual void SilentRefresh () ;
    virtual bool doOverwrite () ;
    virtual TAminoAcids *getAA() ;
    virtual TPrimerDesign *getPD() ;
    virtual TAlignment *getAln() ;
    
    virtual bool getPrintToColor () { return printToColor ; }
    virtual void setPrintToColor ( bool _b ) { printToColor = _b ; }
    virtual bool getDrawAll () { return drawall ; }
    virtual void setDrawAll ( bool _b ) { drawall = _b ; }
    virtual bool isHorizontal () { return horizontal ; }
    virtual void setHorizontal ( bool _b ) { horizontal = _b ; }
    virtual void toggleHorizontal () { setHorizontal ( !isHorizontal() ) ; }
    virtual bool getHide () { return hide ; }
    virtual void doHide ( bool _b ) { hide = _b ; }
    virtual bool isMiniDisplay () { return miniDisplay ; }
    virtual void setMiniDisplay ( bool _b ) { miniDisplay = _b ; }
    virtual SeqBasic *getLastWhere() { return lastwhere ; }
    virtual bool isPrinting () { return printing ; }
    virtual void forceOverwrite ( bool _b ) { forceoverwrite = _b ; }
    virtual bool getEditMode () { return editMode ; }
    virtual void setEditMode ( bool _b ) { editMode = _b ; }
    virtual void setLowX ( int _i ) { if ( lowx < _i ) lowx = _i ; }
    virtual void startEdit ( wxString id ) ;
    virtual void stopEdit () ;
    virtual int markedFrom () { return _from ; }
    virtual int markedTo () { return _to ; }
    virtual void unmark () ;
    virtual int NumberOfLines() { return seq.GetCount() + blankline ; }

    ChildBase *child ; // Wanna-be universal com port to "parent"
    MyChild *p ;
    int blankline , charwidth , charheight , hardstop ;
    int lastmarked , maxendnumberlength , lastyoffset , blocksize , border ;
    wxArraySeqBasic seq ;
    wxFont *font , *smallFont , *varFont;
    wxString edit_id , edit_valid ;
    
    private :
    virtual wxBitmap *getSequenceBitmap () ;
    virtual TVector *getPCR_DNA_vector() ;
    virtual void showContextMenu ( SeqBasic *where , int pos , wxPoint pt ) ;
    virtual void insertRestrictionSite ( bool left ) ;
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

class TMarkMem
    {
    public :
    TMarkMem ( SequenceCanvas *_sc = NULL ) ;
    virtual void unmark () ;
    virtual void remark () ;
    
    private :
    SequenceCanvas *sc ;
    int f , t , l ;
    } ;

#endif

