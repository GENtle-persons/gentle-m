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
    vector <int> p , m ;
    vector <wxRect> r , l ;
    
    virtual void cleanup ()
        {
        while ( p.size() ) p.pop_back() ;
        while ( r.size() ) r.pop_back() ;
        while ( l.size() ) l.pop_back() ;
        while ( m.size() ) m.pop_back() ;
        }
    virtual void add ( int np , int x , int y , int w , int h )
        {
        p.push_back ( np ) ;
        r.push_back ( wxRect ( x , y , w , h ) ) ;
        m.push_back ( 0 ) ;
        }
    virtual void addline ( int from , int to , int vfrom , int vto )
        {
        l.push_back ( wxRect ( from , to , vfrom , vto ) ) ;
        }
    virtual int getLine ( int y )
        {
        int ret = -1 ;
        for ( int a = 0 ; a < l.size() && ret == -1 ; a++ )
           {
           if ( l[a].width <= y && l[a].height >= y )
              ret = a ;
           }
        return ret ;
        }
    virtual int getItem ( wxPoint pt , int line )
        {
        int a ;
        for ( a = l[line].x ; a <= l[line].y ; a++ )
           if ( pt.x >= r[a].x && pt.x <= r[a].x+r[a].width &&
                pt.y >= r[a].y && pt.y <= r[a].y+r[a].height )
              return a ;
        return 0 ;
        }
    } ;

class SeqBasic
    {
    public :
    SeqBasic ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; }
    virtual void init ( SequenceCanvas *ncan = NULL ) ;
    virtual void initFromTVector ( TVector *v ) {} // Dummy
    virtual int  arrange ( int n ) { return 0 ; } ;
    virtual void show ( wxDC& dc ) {} ;
    virtual wxPoint showText ( int ystart , vector <string> &tout )
        { return wxPoint ( -1 , -1 ) ; } ;
    virtual string whatsthis () { return "BASIC" ; }
    SeqPos pos ;
    string s ;
    int blocksize , offset , endnumberlength ;
    SequenceCanvas *can ;
    bool takesMouseActions , shown ;
    } ;

class SeqNum : public SeqBasic
    {
    public :
    SeqNum ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; }
    virtual string whatsthis () { return "NUM" ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    } ;

class SeqDivider : public SeqBasic
    {
    public :
    SeqDivider ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; offset = 0 ; }
    virtual string whatsthis () { return "DIVIDER" ; }
    virtual void initFromTVector ( TVector *v ) ;
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    
    // Variables
    int itemsperline ;
    } ;
    
class SeqDNA : public SeqBasic
    {
    public :
    SeqDNA ( SequenceCanvas *ncan = NULL ) { vec = NULL ; init ( ncan ) ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual void initFromTVector ( TVector *v ) ;
    virtual string whatsthis () { return "DNA" ; }
    virtual wxPoint showText ( int ystart , vector <string> &tout ) ;
    virtual wxColor getBaseColor ( char b ) ;
        
    // Variables
    TVector *vec ;
    int itemsperline ;
    bool showNumbers ;
    wxColour fontColor ;
    string alternateName ;
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
    virtual string whatsthis () { return myname ; }
    
    // Variables
    string myname ;
    } ;

class SeqAlign : public SeqBasic
    {
    public :
    SeqAlign ( SequenceCanvas *ncan = NULL ) { init ( ncan ) ; myname = "Align" ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual string whatsthis () { return "ALIGN" ; }
    
    // Variables
    string myname ;
    int itemsperline ;
    } ;

class SeqRestriction : public SeqBasic
    {
    public :
    SeqRestriction ( SequenceCanvas *ncan = NULL ) { vec = NULL ; init ( ncan ) ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual void initFromTVector ( TVector *v ) ;
    virtual string whatsthis () { return "RESTRICTION" ; }
    
    // Variables
    TVector *vec ;
    vector <string> vs ;  // The different lines
    vector <string> ven ; // The vector name for each cut
    vector <int> yoff , eoe ;
    bool down ;
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
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual wxPoint showText ( int ystart , vector <string> &tout ) ;
    virtual void initFromTVector ( TVector *v ) ;
    virtual void initFromString ( string t ) ;
    virtual void analyzeProteases () ;
    virtual void updateProteases () ;
    virtual void fixOffsets ( TVector *v ) ;
    virtual string whatsthis () { return "AA" ; }
    
    // Variables
    TVector *vec ;
    int itemsperline ;
    int mode , disp ;
    bool primaryMode , showNumbers ;
    char unknownAA ;
    vector <TProteaseCut*> pc ;
    string pa_w ;
    vector <int> pa_wa ;
    vector <TProtease*> proteases ;
    vector <int> offsets ;
    vector <TVectorItem*> offset_items ;
    } ;

class SeqABI : public SeqDNA
    {
    public :
    SeqABI ( SequenceCanvas *ncan = NULL ) { at = NULL ; inv_compl = false ; init ( ncan ) ; }
    virtual int  arrange ( int n ) ;
    virtual void show ( wxDC& dc ) ;
    virtual void initFromFile ( string filename ) ;
    virtual string whatsthis () { return "ABI" ; }
    virtual void drawTopLine ( wxDC &dc , int y ) ;
    virtual wxColor getBaseColor ( char b ) ;
    virtual void setInvCompl ( bool x ) ;
    
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
    virtual string whatsthis () { return "FEATURE" ; }
    virtual void initFromTVector ( TVector *v ) ;
    virtual bool collide ( int a , int b ) ;
    virtual bool doesHit ( int a , int x ) ;
    
    // Variables
    vector <wxRect> vr ;
    vector <wxPen> pens ;
    int maxlayers ;
    SeqAA *aaa ;
    } ;

class SequenceCanvas : public wxScrolledWindow
    {
    public:
    SequenceCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual ~SequenceCanvas () { delete font ; }
    virtual void OnDraw(wxDC& dc);
    virtual void safeShow ( wxDC &dc ) ;
    virtual bool IsDirty() const { return m_dirty; }
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
    
    virtual void OnCopyResultDNA ( wxCommandEvent &ev ) ;
    virtual void OnCopyResultAA ( wxCommandEvent &ev ) ;
    virtual void OnNewFromResultDNA ( wxCommandEvent &ev ) ;
    virtual void OnNewFromResultAA ( wxCommandEvent &ev ) ;

    virtual void OnFocus(wxFocusEvent& event) ;
    virtual void OnKillFocus(wxFocusEvent& event) ;
    
    virtual string getSelection () ;
    
    virtual void updateEdit ( TVector *v , string id , int from ) ;
    virtual void arrange () ;
    virtual SeqBasic* findMouseTarget ( wxPoint pt , int &pos ) ;
    virtual SeqBasic* findID ( string id ) ;
    virtual void mark ( string id , int from , int to , int value = 1 ) ;
    virtual bool inMarkRange ( int x , int f , int t , int l ) ;
    virtual void ensureVisible ( int pos ) ;
    virtual int getBatchMark () ;
    virtual void MyGetClientSize ( int *w , int *h ) ;
    virtual void MyGetSize ( int *w , int *h ) ;
    virtual wxSize MyGetClientSize () ;
    virtual void MyGetViewStart ( int *x , int *y ) ;
    virtual void SilentRefresh () ;
    virtual bool doOverwrite () ;
    
    private :
    wxBitmap *getSequenceBitmap () ;
    
    public :
    
//    private:
    MyChild *p ;
    TAminoAcids *aa ;
    TPrimerDesign *pd ;
    bool m_dirty , editMode , doHide ;
    int blankline , charwidth , charheight , lowy , lastpos , vpy ;
    vector <SeqBasic *> seq ;
    wxFont *font , *smallFont , *varFont;
    SeqBasic *lastwhere ;
    string edit_id , edit_valid ;
    int lastmarked ;
    bool forceoverwrite , drawall , printing , wantOverwrite ;
    wxDC *print_dc ;
    int print_maxx ;
    
    int _from , _to ; // Mark
    bool printToColor ;
    ChildBase *child ; // Wanna-be universal com port to "parent"
    int maxendnumberlength ;
    int lastyoffset ;
    wxString lastToolTip ;

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

