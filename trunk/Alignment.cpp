#include "Alignment.h"

BEGIN_EVENT_TABLE(TAlignment, MyChildBase)
    EVT_BUTTON(ALIGNMENT_SETTINGS,TAlignment::OnSettings)
    EVT_CLOSE(TAlignment::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_CHECKBOX(ALIGN_HORIZ, TAlignment::OnHorizontal)
    
    EVT_MENU(ALIGN_BOLD,TAlignment::OnMenuBold)
    EVT_MENU(ALIGN_MONO,TAlignment::OnMenuMono)
    EVT_MENU(ALIGN_NORM,TAlignment::OnMenuNorm)
    EVT_MENU(ALIGN_INVS,TAlignment::OnMenuInvs)
    EVT_MENU(ALIGN_SOA,TAlignment::OnMenuSoa)
    EVT_MENU(ALIGN_SOAI,TAlignment::OnMenuSoaI)
    EVT_MENU(ALIGN_SIML,TAlignment::OnMenuSiml)
    EVT_MENU(ALIGN_SEQ,TAlignment::OnMenuSeq)
    EVT_MENU(ALIGN_FEAT,TAlignment::OnMenuFeat)
    EVT_MENU(ALIGN_RNA,TAlignment::OnMenuRNA)
    EVT_MENU(ALIGN_CONS,TAlignment::OnMenuCons)
    EVT_MENU(ALIGN_IDENT,TAlignment::OnMenuIdent)

END_EVENT_TABLE()




TAlignment::TAlignment(MyFrame *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    invs = cons = bold = mono = false ;
    def = "alignment" ;
    match = 2 ; // Match value
    mismatch = -1 ; // Mismatch score
    gap_penalty = -2 ; // Gap penalty
    matrix = "BLOSUM" ;
    gap = "-" ;
    algorithm = ALG_CW ;
    vec = NULL ;
    aaa = NULL ;
    colCur = NULL ;
    
    // Now we read the default color scheme from BioEdit...
    wxString fn = myapp()->homedir + myapp()->slash + "default.tab" ;
    ifstream in ( fn.c_str() , ios::in ) ;
    int a ;
    vector <wxColour> *c = NULL ;
    while ( colDNA.size() < 256 ) colDNA.push_back ( wxColour(0,0,0) ) ;
    colAA = colDNA ;
    while ( !in.eof() )
        {
        string s ;
        getline ( in , s ) ;
        if ( s == "" ) ;
        else if ( s[0] == '/' )
           {
           if ( s == "/amino acids/" ) c = &colAA ;
           if ( s == "/nucleotides/" ) c = &colDNA ;
           }
        else if ( c )
           {
           string t ;
           getline ( in , t ) ; // s in int, t is list of chars
           int x = atoi ( s.c_str() ) ;
           for ( int b = 0 ; b < t.length() ; b++ )
              {
              (*c)[(unsigned char)t[b]] = wxColour (
                          ( x >> 16 ) & 255 ,
                          ( x >> 8 ) & 255 ,
                          x & 255 ) ;
              }
           }
        }
    colCur = &colAA ;
    }
    
TAlignment::~TAlignment ()
    {
    }


wxColour TAlignment::findColors ( char c1 , char c2 , bool fg )
    {
    int a ;
    wxColour r ;
    if ( invs && c1 != '-' ) fg = !fg ;
    if ( fg )
        {
        int cc = (unsigned char) c1 ;
        if ( !colCur ) r = *wxBLACK ;
        else if ( cc < 0 || cc >= colCur->size() ) r = *wxBLACK ;
        else r = (*colCur)[cc] ;
        }
    else r = *wxWHITE ;
    
//    if ( fg && c1 != '-' && c2 != '-' && c1 != c2 )
//        r = wxColour ( 200 , 0 , 0 ) ;
    
    return r ;
    }
    
void TAlignment::OnClose(wxCloseEvent& event)
{
    // Removing the window from the main tree
    MyFrame *p = myapp()->frame ;
    p->mainTree->removeChild ( this ) ;
    p->SetTitle ( txt("gentle") ) ;
    SetTitle ( txt("gentle") ) ;
    p->removeChild ( this ) ;
    event.Skip();
}
    
void TAlignment::initme ()
    {
    int bo = 5 ;

    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;
    wxMenu *view_menu = new wxMenu ;

    view_menu->Append(ALIGN_BOLD, txt("m_align_bold") , "" , true ) ;
    view_menu->Append(ALIGN_MONO, txt("m_align_mono") , "" , true ) ;
    view_menu->Append(ALIGN_CONS, txt("m_align_cons") , "" , true ) ;
    view_menu->AppendSeparator();
    view_menu->Append(ALIGN_NORM, txt("m_align_norm") , "" , true ) ;
    view_menu->Append(ALIGN_INVS, txt("m_align_invs") , "" , true ) ;
    view_menu->Append(ALIGN_SOA , txt("m_align_soa")  , "" , true ) ;
    view_menu->Append(ALIGN_SOAI, txt("m_align_soai") , "" , true ) ;
    view_menu->Append(ALIGN_SIML, txt("m_align_siml") , "" , true ) ;
    view_menu->Append(ALIGN_SEQ , txt("m_align_seq")  , "" , true ) ;
    view_menu->Append(ALIGN_FEAT, txt("m_align_feat") , "" , true ) ;
    view_menu->Append(ALIGN_RNA , txt("m_align_rna")  , "" , true ) ;
    view_menu->AppendSeparator();
    view_menu->Append(ALIGN_IDENT, txt("m_align_ident") , "" , true ) ;

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(view_menu, txt("m_view") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    SetMenuBar(menu_bar);
    menu_bar->FindItem(ALIGN_NORM)->Check ( true ) ;
    menu_bar->FindItem(ALIGN_CONS)->Check ( true ) ;
    menu_bar->FindItem(ALIGN_IDENT)->Check ( true ) ;
    cons = true ;
    showIdentity = true ;
    
    hs = new wxSplitterWindow ( this , SPLIT_ALIGNMENT ) ;

    // Sequence Canvas
    sc = new SequenceCanvas ( hs , wxPoint ( 0 , 0 ) , wxSize ( 100 , 100 ) ) ;
    sc->blankline = 1 ;
    sc->child = this ;
    sc->EnableScrolling ( true , true ) ;

    // Upper panel
    up = new wxPanel ( hs , -1 , wxDefaultPosition , wxSize ( 1000 , 100 ) ) ;
    
    int w , h ;
    up->GetClientSize ( &w , &h ) ;
    wxBitmapButton *sb = new wxBitmapButton ( up , ALIGNMENT_SETTINGS ,
            wxBitmap (myapp()->bmpdir+"\\align.bmp", wxBITMAP_TYPE_BMP),
            wxPoint ( bo , bo ) ,
            wxDefaultSize ,
            wxBU_AUTODRAW ,
            wxDefaultValidator ) ;
    new wxStaticText ( up , -1 , txt("t_settings") , wxPoint ( bo , h-20 ) );
    wxRect r = sb->GetRect () ;
    new wxStaticText ( up , -1 , txt("t_mmb") , wxPoint ( r.GetRight()+bo , r.GetTop() ) );
    mmb = new wxListBox ( up , -1 , 
                wxPoint ( r.GetRight() + bo , r.GetTop() + 15 ) ,
                wxSize ( 200 , r.GetBottom() - r.GetTop() ) ) ;
    mmb->Append ( txt("t_mmb_insert_gap") ) ;
    mmb->Append ( txt("t_mmb_delete_gap") ) ;
    mmb->Append ( txt("t_mmb_insert_gap_others") ) ;
    mmb->Append ( txt("t_mmb_delete_gap_others") ) ;
    mmb->SetStringSelection ( txt("t_mmb_insert_gap") ) ;


    hs->SplitHorizontally ( up , sc ,h+bo ) ;
    hs->SetMinimumPaneSize ( h+bo ) ;
    
#ifdef __WXMSW__ // LINUX
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    myapp()->frame->InitToolBar(toolBar);
    toolBar->AddTool( MDI_TEXT_IMPORT , 
                myapp()->frame->bitmaps[0] ,
                txt("m_new_sequence") ) ;
    toolBar->AddTool( MDI_FILE_OPEN, 
                myapp()->frame->bitmaps[1] ,
            txt("m_open") , txt("m_opentxt") );
    toolBar->AddSeparator () ;
    wxCheckBox *mycb = new wxCheckBox ( toolBar , ALIGN_HORIZ , txt("t_horizontal") ) ;
    toolBar->AddControl ( mycb ) ;
    toolBar->Realize() ;
#endif

    Maximize () ;
    wxCommandEvent ev ;
    OnSettings ( ev ) ;
    sc->SetFocus() ;
    myapp()->frame->setChild ( this ) ;
    }

// Calling clustalw.exe, eventually
void TAlignment::recalcAlignments ()
    {
    while ( lines.size() && lines[lines.size()-1].isIdentity )
        lines.pop_back () ;
    if ( lines.size() == 0 ) return ;

    // Align
    int a ;
    TAlignLine line ;
    SetCursor ( *wxHOURGLASS_CURSOR ) ;
    if ( lines.size() <= 1 ) // Just one sequence
        {
        lines[0].ResetSequence() ;
        }
    else if ( algorithm == ALG_CW ) // Clustal-W, external
        {

        wxString cwt = "clustalw.txt" ;
        wxString hd = myapp()->homedir ;
        wxString tx = hd + "\\" + cwt ;
        ofstream out ( tx.c_str() , ios::out ) ;
        for ( a = 0 ; a < lines.size() ; a++ )
           {
           out << ">" << wxString::Format ( "%d" , a ) << endl ;
           out << lines[a].v->sequence << endl ;
           }
        out.close () ;
        
#ifdef __WXMSW__
        wxString bn = hd + "\\clustalw.bat" ;
        ofstream bat ( bn.c_str() , ios::out ) ;
        bat << "@echo off" << endl ;
        bat << "cd " << hd << endl ;
        bat << "clustalw.exe clustalw.txt" ;
        bat << wxString::Format ( " /gapopen=%d" , gap_penalty ) ;
        bat << wxString::Format ( " /gapext=%d" , mismatch ) ;
        bat << " /matrix=" << matrix ;
        bat << endl ;
        bat.close() ;
        wxExecute ( bn , wxEXEC_SYNC ) ;
#endif

        ifstream in ( "clustalw.aln" , ios::in ) ;
        string s ;
        getline ( in , s ) ;
        do {
           getline ( in , s ) ;
           } while ( s == "" ) ;
        int off ;
        for ( off = s.length()-1 ; s[off-1] != ' ' ; off-- ) ;
        line.isIdentity = true ;
        line.name = txt("t_identity") ;
        lines.push_back ( line ) ;
        bool first = true ;
        for ( a = 0 ; a < lines.size() ; a++ ) lines[a].s = "" ;
        while ( !in.eof() )
           {
           for ( a = 0 ; a < lines.size() ; a++ )
              {
              if ( !first ) getline ( in , s ) ;
              else first = false ;
              int index = atoi ( s.substr ( 0 , off-1 ) . c_str() ) ;
              if ( s[0] == ' ' ) index = lines.size()-1 ;
              lines[index].s += s.substr ( off , s.length() ) ;
              }
           getline ( in , s ) ; // Blank line
           }

        generateConsensusSequence ( false ) ;
        }
    else // Internal routines
        {
        while ( lines.size() > 2 ) lines.pop_back () ;      
        for ( a = 0 ; a < 2 ; a++ ) lines[a].ResetSequence () ;

        if ( algorithm == ALG_NW )
           NeedlemanWunsch ( lines[0].s , lines[1].s ) ; 
        else if ( algorithm == ALG_SW )
           SmithWaterman ( lines[0].s , lines[1].s ) ; 

        generateConsensusSequence () ;
        }
        
    for ( a = 0 ; a < 1 ; a++ )
        {
        if ( !lines[a].isIdentity && lines[a].v->items.size() > 0 )
           lines[a].showFeatures () ;
        }
        
    SetCursor ( *wxSTANDARD_CURSOR ) ;
    }

void TAlignment::redoAlignments ( bool doRecalc )
    {
    int a ;
    // Cleaning up
    while ( sc->seq.size() )
        {
        delete sc->seq[sc->seq.size()-1] ;
        sc->seq.pop_back () ;
        }
        
    if ( doRecalc ) recalcAlignments () ;
        
    // Display
    sc->maxendnumberlength = strlen ( txt("t_identity") ) ;

    for ( a = 0 ; a < lines.size() ; a++ )
        {
        if ( lines[a].hasFeatures() )
           {
           lines[a].showFeatures () ;
           SeqFeature *f = new SeqFeature ( sc ) ;
           f->id = a ;
           sc->seq.push_back ( f ) ;
           }
        SeqAlign *d = new SeqAlign ( sc ) ;
        sc->seq.push_back ( d ) ;
        d->id = a ;
        d->s = lines[a].s ;
        d->myname = lines[a].name ;
        }

    // Features
    for ( int g = 0 ; g < lines.size() ; g++ )
        {
        if ( !lines[g].isIdentity && lines[g].hasFeatures() )
           {
//            lines[g].showFeatures () ;
            for ( a = 0 ; a < lines[g].s.length() ; a++ )
                {
                if ( lines[g].s[a] == '-' )
                   lines[g].getFeatures()->insert_char ( '-' , a+1 , false ) ;
                }
            }
        }

    updateSequence () ;
    }
    
void TAlignment::generateConsensusSequence ( bool addit )
    {
    int a , b ;
    // The stars'n'stripes sequence ;-)
    TAlignLine line ;
    line.isIdentity = true ;
    line.name = txt("t_identity") ;
    string s ;
    for ( a = 0 ; a < lines[0].s.length() ; a++ )
        if ( lines[0].s[a] == lines[1].s[a] ) s += "*" ;
        else s += " " ;
    if ( addit ) lines.push_back ( line ) ;
    
    // The REAL consensus sequence
    consensusSequence = lines[0].s ;
    for ( a = 0 ; a < consensusSequence.length() ; a++ )
        {
        int c[256] ;
        for ( b = 0 ; b < 256 ; b++ ) c[b] = 0 ;
        for ( b = 0 ; b + 1 < lines.size() ; b++ ) c[lines[b].s[a]]++ ;
        consensusSequence[a] = ' ' ;
        for ( b = 0 ; b < 256 ; b++ )
           {
           float f = 100 * c[b] ;
           f /= lines.size() - 1 ;
           if ( f >= 60 ) consensusSequence[a] = b ;
           }
        }
    }
    
void TAlignment::myInsert ( int line , int pos , char what )
    {
    if ( lines[line].hasFeatures() )
        {
        lines[line].getFeatures()->insert_char ( '-' , pos , false ) ;
        lines[line].s = lines[line].getFeatures()->sequence ;
        }
    else lines[line].s.insert ( pos-1 , wxString ( what ) ) ;
    }

void TAlignment::myDelete ( int line , int pos )
    {
    if ( lines[line].hasFeatures() )
        {
        lines[line].getFeatures()->doRemoveNucleotide ( pos - 1 ) ;
        lines[line].s = lines[line].getFeatures()->sequence ;
        }
    else lines[line].s.erase ( pos-1 , 1 ) ;
    }
        
void TAlignment::callMiddleMouseButton ( int id , int pos )
    {
    wxString mode = mmb->GetStringSelection () ;
    int a , line = id ;
    if ( lines[line].s[pos-1] != '-' && mode == txt("t_mmb_delete_gap") )
       {
       wxBell() ;
       return ;
       }

    vector <int> l2s ;
    while ( l2s.size() < lines.size() ) l2s.push_back ( 0 ) ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        if ( sc->seq[a]->whatsthis() == "ALIGN" )
           {
           SeqAlign *b = (SeqAlign*) sc->seq[a] ;
           l2s[b->id] = a ;
           }
        }

    for ( a = 0 ; a < lines.size() ; a++ )
        {
        if ( lines[a].isIdentity ) {}
        else if ( mode == txt("t_mmb_insert_gap") )
            {
            if ( line == a ) myInsert ( a , pos , '-' ) ;
            else myInsert ( a , lines[a].s.length()+1 , '-' ) ;
            }
        else if ( mode == txt("t_mmb_delete_gap") )
            {
            if ( line == a )
               {
               myDelete ( a , pos ) ;
               myInsert (  a , lines[a].s.length()+1 , '-' ) ;
               }
            }
        else if ( mode == txt("t_mmb_insert_gap_others") )
            {
            if ( line != a ) myInsert ( a , pos , '-' ) ;
            else myInsert ( a , lines[a].s.length()+1 , '-' ) ;
            }
        else if ( mode == txt("t_mmb_delete_gap_others") )
            {
            if ( line != a && lines[a].s[pos-1] == '-' )
               {
               myDelete ( a , pos ) ;
               myInsert ( a , lines[a].s.length()+1 , '-' ) ;
               }
            }
        SeqAlign *d = (SeqAlign*) sc->seq[l2s[a]] ;
        d->id = a ;
        d->s = lines[a].s ;
        }

    // Cleanup of '-' ends
    bool again = true ;
    while ( again )
        {
        int max = 0 ;
        for ( a = 0 ; a < lines.size() && !lines[a].isIdentity ; a++ ) max++ ;
        for ( a = 0 ; a < lines.size() && !lines[a].isIdentity && lines[a].s[lines[a].s.length()-1] == '-' ; a++ ) ;
        if ( a == max )
            {
            for ( a = 0 ; a < lines.size() ; a++ )
               if ( !lines[a].isIdentity )
                  myDelete ( a , lines[a].s.length() ) ;
            }
        else again = false ;
        }
    updateSequence () ;
    }
    
void TAlignment::updateSequence ()
    {
    for ( int g = 0 ; g < sc->seq.size() ; g++ )
        {
        if ( sc->seq[g]->whatsthis() != "FEATURE" ) continue ;
        SeqFeature *f = (SeqFeature*) sc->seq[g] ;
        int id = f->id ;
        if ( lines[id].getFeatures()->type == TYPE_AMINO_ACIDS )
            {
            colCur = &colAA ;
            if ( aaa ) delete aaa ;
            aaa = new SeqAA ( NULL ) ;
            sc->seq[g] = aaa ;
            aaa->initFromString ( lines[id].getFeatures()->sequence ) ;
            aaa->fixOffsets ( lines[id].getFeatures() ) ;
            aaa->can = sc ;
            sc->arrange () ;
            sc->seq[g] = f ;
            f->initFromTVector ( lines[id].getFeatures() ) ;
            f->aaa = aaa ;
            }
        else if ( lines[id].getFeatures()->type == TYPE_VECTOR )
            {
            colCur = &colDNA ;
            if ( aaa ) delete aaa ;
            aaa = new SeqAA ( NULL ) ;
            sc->seq[g] = aaa ;
            aaa->mode = AA_KNOWN ;
            aaa->disp = AA_ONE ;
            aaa->initFromTVector ( lines[id].getFeatures() ) ;
            aaa->showNumbers = false ;
            aaa->can = sc ;
            sc->arrange () ;
            sc->seq[g] = f ;
            f->initFromTVector ( lines[id].getFeatures() ) ;
            f->aaa = aaa ;
            }
        else f->initFromTVector ( lines[id].getFeatures() ) ;
        }
    sc->arrange () ;
    sc->SilentRefresh() ;
    }
    
string TAlignment::getName ()
    {
    return "Alignment" ;
    }

void TAlignment::OnSettings ( wxCommandEvent &ev )
    {
    TAlignmentDialog ad ( this , txt("t_settings") ) ;
    int r = ad.ShowModal () ;
    if ( r != wxID_OK ) return ;

    lines.clear () ;
    for ( int a = 0 ; a < ad.vcn.size() ; a++ )
        {
        TAlignLine line ;
        line.name = ad.vcn[a] ;
        line.v = ad.vcv[a] ;
        line.ResetSequence() ;
        lines.push_back ( line ) ;
        }
    
    match = ad.alg_match->GetValue() ;
    mismatch = ad.alg_mismatch->GetValue() ;
    gap_penalty = ad.alg_penalty->GetValue() ;
    matrix = ad.alg_matrix->GetStringSelection().c_str() ;
    
    algorithm = ad.alg->GetSelection () ;
    
    redoAlignments () ;
    }
    
// HOMEMADE ALIGNMENT ALGORITHMS

int TAlignment::NeedlemanWunsch ( string &s1 , string &s2 )
    {
    return MatrixAlignment ( s1 , s2 , false ) ;
    }
    
int TAlignment::SmithWaterman ( string &s1 , string &s2 )
    {
    return MatrixAlignment ( s1 , s2 , true ) ;
    }
    
int TAlignment::MatrixAlignment ( string &s1 , string &s2 , bool local )
    {
    int a , b ;
    int M = s1.length() ;
    int N = s2.length() ;

    // Initializing backlink matrix
    
    vector <tvc> back ;
    tvc blank_b ;
    while ( blank_b.size() < N+1 ) blank_b.push_back ( 0 ) ;
    while ( back.size() < M+1 ) back.push_back ( blank_b ) ;
    
    // Initializing pseudo-matrix (simulated by two altering lines)
    int *matrix0 , *matrix1 , *matrix2 ;
    matrix0 = new int[N+1] ;
    matrix1 = new int[N+1] ;
    for ( a = 0 ; a < N+1 ; a++ ) matrix1[a] = 0 ;
    
    // Filling
    int i , j ;
    int max = -999999 ;
    vector <int> vi , vj ;
    int mi = M , mj = N ;
    for ( i = 0 ; i < M ; i++ )
        {
        matrix2 = matrix0 ;
        matrix0 = matrix1 ;
        matrix1 = matrix2 ;
        *matrix1 = 0 ;
        for ( j = 0 ; j < N ; j++ )
           {
           int x = i+1 ;
           int y = j+1 ;
           int s = (s1[i]==s2[j])?match:mismatch ;
           
           // Maxima
           int m1 = matrix0[j] + s ;
           int m2 = matrix1[j] + gap_penalty ;
           int m3 = matrix0[y] + gap_penalty ;
           
           // Determining maximum
           int r = m1 > m2 ? m1 : m2 ;
           r = r > m3 ? r : m3 ;
           if ( local ) r = r > 0 ? r : 0 ;
           matrix1[y] = r ;
           
           if ( local && r >= max )
              {
              if ( r > max )
                  {
                  max = r ;
                  mi = x ;
                  mj = y ;
                  vi = tvi () ;
                  vj = tvi () ;
                  }
              vi.push_back ( x ) ;
              vj.push_back ( y ) ;
              }
           
           // The way back
           int n = 0 ;
           if ( r == m1 ) n |= BACK_LU ;
           if ( r == m2 ) n |= BACK_UP ;
           if ( r == m3 ) n |= BACK_LEFT ;
           back[x][y] = n ;
           }
        }
    
    // Backtracking
    string t1 , t2 ;
    if ( local )
        {
        for ( a = b = 0 ; a < vi.size() ; a++ )
           {
           MatrixBacktrack ( back , s1 , s2 , t1 , t2 , vi[a] , vj[a] ) ;
           if ( t1.length() > b )
              {
              b = t1.length() ;
              mi = vi[a] ;
              mj = vj[a] ;
              }
           }
        }
    else
        {
        mi = M ;
        mj = N ;
        }
        
    MatrixBacktrack ( back , s1 , s2 , t1 , t2 , mi , mj ) ;
    string k1 , k2 ;
    // The beginning
    for ( a = b = 0 ; a < t1.size() ; a++ )
        if ( t1[a] != gap[0] ) b++ ;
    k1 = s1.substr ( 0 , mi - b ) ;
    for ( a = b = 0 ; a < t2.size() ; a++ )
        if ( t2[a] != gap[0] ) b++ ;
    k2 = s2.substr ( 0 , mj - b ) ;
    while ( k1.length() < k2.length() ) k1 = "-" + k1 ;
    while ( k2.length() < k1.length() ) k2 = "-" + k2 ;
    t1 = k1 + t1 ;
    t2 = k2 + t2 ;
    
    // The end
    k1 = s1.substr ( mi ) ;
    k2 = s2.substr ( mj ) ;
    while ( k1.length() < k2.length() ) k1 += "-" ;
    while ( k2.length() < k1.length() ) k2 += "-" ;
    t1 += k1 ;
    t2 += k2 ;
    
    s1 = t1 ;
    s2 = t2 ;
    return max ;
    }
    
void TAlignment::MatrixBacktrack ( vector <tvc> &back , 
                                    string s1 , string s2 , 
                                    string &t1 , string &t2 , 
                                    int i , int j )
    {
    t1 = "" ;
    t2 = "" ;
    while ( i > 0 || j > 0 )
        {
        if ( (back[i][j]&BACK_LU) == BACK_LU ) // upper left
           {
           t1 = s1[--i] + t1 ;
           t2 = s2[--j] + t2 ;
           }
        else if ( (back[i][j]&BACK_LEFT) > 0 ) // left
           {
           t1 = s1[--i] + t1 ;
           t2 = gap + t2 ;
           }
        else if ( ( back[i][j]&BACK_UP ) > 0 ) // up
           {
           t1 = gap + t1 ;
           t2 = s2[--j] + t2 ;
           }
        else break ;
        }
    }


void TAlignment::invokeOriginal ( int id , int pos )
    {
    int a ;
    MyFrame *f = myapp()->frame ;
    ChildBase *c = NULL ;
    c = lines[id].FindOrigin() ;
    /*
    for ( a = 0 ; a < f->children.size() ; a++ )
        {
        if ( ( f->children[a]->def == "dna" || 
               f->children[a]->def == "AminoAcids" ||
               f->children[a]->def == "ABIviewer" )
                && f->children[a]->getName() == name )
           {
           if ( c )
              {
              wxMessageBox ( txt("t_ambigous_name") ) ;
              return ;
              }
           else c = f->children[a] ;
           }
        }*/
    if ( !c ) return ;

    c->Activate() ;
    if ( c->def == "dna" )
        {
        MyChild *c1 = ((MyChild*)c) ;
        c1->cSequence->mark ( "DNA" , pos , pos , 1 ) ;
        c1->cSequence->Scroll ( 0 , c1->cSequence->getBatchMark() ) ;
        }
    else if ( c->def == "AminoAcids" )
        {
        TAminoAcids *c1 = ((TAminoAcids*)c) ;
        c1->sc->mark ( "AA" , pos , pos , 1 ) ;
        c1->sc->Scroll ( 0 , c1->sc->getBatchMark() ) ;
        }
    else if ( c->def == "ABIviewer" )
        {
        TABIviewer *c1 = ((TABIviewer*)c) ;
        c1->sc->mark ( "ABI" , pos , pos , 1 ) ;
        c1->sc->Scroll ( 0 , c1->sc->getBatchMark() ) ;
        }
    }    

// Old internal code
/*
void TAlignment::redoAlignments ()
    {
    int a ;

    // Cleaning up
    while ( sc->seq.size() )
        {
        delete sc->seq[sc->seq.size()-1] ;
        sc->seq.pop_back () ;
        }

    if ( lines.size() == 0 ) return ;

    // Align
    while ( lines.size() ) qAlign.pop_back () ;
    for ( a = 0 ; a < qVec.size() ; a++ )
        qAlign.push_back ( qVec[a]->sequence ) ;

    if ( lines.size() > 1 )
        {
        SetCursor ( *wxHOURGLASS_CURSOR ) ;
        wxStopWatch sw ;

        int first , second ;
        for ( first = 0 ; first < lines.size() ; first++ )
           {
           for ( second = first+1 ; second < lines.size() ; second++ )
              {
              sw.Start () ;
              int rating = 0 ;
              if ( algorithm == ALG_NW )
                 rating = NeedlemanWunsch ( qAlign[first] , qAlign[second] ) ; 
              else if ( algorithm == ALG_SW )
                 rating = SmithWaterman ( qAlign[first] , qAlign[second] ) ; 
              long time = sw.Time() ;
              wxMessageBox ( wxString::Format ( "%d-%d: time %d ms, rating %d" , first , second , time , rating ) ) ;
              }
           }

        SetCursor ( *wxSTANDARD_CURSOR ) ;
        }
    else
        {
        qAlign[0] = qVec[0]->sequence ;
        }

    // Display
    SeqNum *n = new SeqNum ( sc ) ;
    sc->seq.push_back ( n ) ;
    n->s = qAlign[0] ;

    for ( a = 0 ; a < lines.size() ; a++ )
        {
        SeqAlign *d = new SeqAlign ( sc ) ;
        sc->seq.push_back ( d ) ;
        d->s = qAlign[a] ;
        d->myname = qName[a] ;
        }
    sc->arrange () ;
    sc->Refresh () ;
    }
*/

void TAlignment::fixMenus ( int i )
    {
    wxMenuBar *mb = GetMenuBar () ;
    mb->FindItem(ALIGN_BOLD)->Check ( bold ) ;
    mb->FindItem(ALIGN_MONO)->Check ( mono ) ;
    mb->FindItem(ALIGN_NORM)->Check ( false ) ;
    mb->FindItem(ALIGN_INVS)->Check ( false ) ;
    mb->FindItem(ALIGN_SOA)->Check ( false ) ;
    mb->FindItem(ALIGN_SOAI)->Check ( false ) ;
    mb->FindItem(ALIGN_SIML)->Check ( false ) ;
    mb->FindItem(ALIGN_SEQ)->Check ( false ) ;
    mb->FindItem(ALIGN_FEAT)->Check ( false ) ;
    mb->FindItem(ALIGN_RNA)->Check ( false ) ;
    
    mb->FindItem(i)->Check ( true ) ;    
    
    invs = mb->FindItem(ALIGN_INVS)->IsChecked () ;
    sc->arrange () ;
    sc->SilentRefresh() ;    
    }

void TAlignment::OnMenuBold ( wxCommandEvent &ev )
    {
    bold = !bold ;
    sc->arrange () ;
    sc->SilentRefresh() ;    
    }

void TAlignment::OnMenuMono ( wxCommandEvent &ev )
    {
    mono = !mono ;
    }

void TAlignment::OnMenuNorm ( wxCommandEvent &ev )
    {
    fixMenus ( ALIGN_NORM ) ;
    }

void TAlignment::OnMenuInvs ( wxCommandEvent &ev )
    {
    fixMenus ( ALIGN_INVS ) ;
    }

void TAlignment::OnMenuSoa ( wxCommandEvent &ev )
    {
    int a ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuSoaI ( wxCommandEvent &ev )
    {
    int a ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuSiml ( wxCommandEvent &ev )
    {
    int a ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuSeq ( wxCommandEvent &ev )
    {
    int a ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuFeat ( wxCommandEvent &ev )
    {
    int a ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuRNA ( wxCommandEvent &ev )
    {
    int a ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuCons ( wxCommandEvent &ev )
    {
    cons = !cons ;
    sc->arrange () ;
    sc->SilentRefresh() ;    
    }
    
void TAlignment::OnMenuIdent ( wxCommandEvent &ev )
    {
    showIdentity = !showIdentity ;
    if ( showIdentity )
        {
        int a = lines.size()-1 ;
        SeqAlign *d = new SeqAlign ( sc ) ;
        sc->seq.push_back ( d ) ;
        d->id = a ;
        d->s = lines[a].s ;
        d->myname = lines[a].name ;
        }
    else
        {
        delete sc->seq[sc->seq.size()-1] ;
        sc->seq.pop_back () ;
        }
    sc->arrange () ;
    sc->SilentRefresh() ;    
    }
    
void TAlignment::OnHorizontal ( wxCommandEvent& event )
    {
    sc->isHorizontal = !sc->isHorizontal ;
    sc->arrange () ;
    sc->SilentRefresh() ;    
    }
    
void TAlignment::MoveUpDown ( int what , int where )
    {
    while ( what != where )
        {
        int a = 1 ;
        if ( what > where ) a = -1 ;    
        TAlignLine dummy = lines[what] ;
        lines[what] = lines[what+a] ;
        lines[what+a] = dummy ;
        what += a ;
        }
    redoAlignments ( false ) ;    
    }

// *****************************************************************************

TAlignLine::TAlignLine ()
    {
    v = NULL ;
    isIdentity = false ;
    features = NULL ;
    }

TAlignLine::~TAlignLine ()
    {
    if ( features )
       {
       delete features ;
       features = NULL ;
       }
    v = NULL ;
    }
    
void TAlignLine::ResetSequence ()
    {
    if ( v ) s = v->sequence ;
    else s = "" ;
    }
    
ChildBase *TAlignLine::FindOrigin ()
    {
    int a ;
    for ( a = 0 ; a < myapp()->frame->children.size() ; a++ )
        {
        if ( myapp()->frame->children[a]->vec == v )
           return myapp()->frame->children[a] ;
        }
    return NULL ;
    }

void TAlignLine::showFeatures ()
    {
    features = new TVector ; // wasting memory each time
    features->setFromVector ( *v ) ;
    features->setWindow ( (ChildBase*) this ) ;
    }

void TAlignLine::hideFeatures ()
    {
    delete features ;
    features = NULL ;
    }

TVector *TAlignLine::getFeatures ()
    {
    return features ;
    }

bool TAlignLine::hasFeatures ()
    {
    return features != NULL ;
    }
    
