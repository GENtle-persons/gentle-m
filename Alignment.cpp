#include "Alignment.h"

BEGIN_EVENT_TABLE(TAlignment, wxMDIChildFrame)
    EVT_BUTTON(ALIGNMENT_SETTINGS,TAlignment::OnSettings)
    EVT_CLOSE(TAlignment::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TAlignmentDialog, wxDialog)
    EVT_BUTTON(AL_ADD,TAlignmentDialog::OnAdd)
    EVT_BUTTON(AL_DEL,TAlignmentDialog::OnDel)
    EVT_BUTTON(AL_UP,TAlignmentDialog::OnUp)
    EVT_BUTTON(AL_DOWN,TAlignmentDialog::OnDown)
    EVT_BUTTON(AL_OK,TAlignmentDialog::OnOK)
    EVT_BUTTON(AL_CANCEL,TAlignmentDialog::OnCancel)
    EVT_CHAR_HOOK(TAlignmentDialog::OnCharHook)
END_EVENT_TABLE()



TAlignment::TAlignment(wxMDIParentFrame *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    def = "alignment" ;
    match = 2 ; // Match value
    mismatch = -1 ; // Mismatch score
    gap_penalty = -2 ; // Gap penalty
    matrix = "BLOSUM" ;
    gap = "-" ;
    algorithm = ALG_SW ;
    vec = NULL ;
    }
    
TAlignment::~TAlignment ()
    {
//    myapp()->my_children.DeleteObject(this);    
    }


wxColour TAlignment::findColors ( char c1 , char c2 , bool fg )
    {
    int a ;
    wxColour r ;
    if ( fg ) r = *wxBLACK ;
    else r = *wxWHITE ;
    
    if ( fg && c1 != '-' && c2 != '-' && c1 != c2 )
        r = wxColour ( 200 , 0 , 0 ) ;
    
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

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    SetMenuBar(menu_bar);

    hs = new wxSplitterWindow ( this , SPLIT_ALIGNMENT ) ;

    // Sequence Canvas
    sc = new SequenceCanvas ( hs , wxPoint ( 0 , 0 ) , wxSize ( 100 , 100 ) ) ;
    sc->blankline = 1 ;
    sc->child = this ;

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


    hs->SplitHorizontally ( up , sc ,h+bo ) ;
    hs->SetMinimumPaneSize ( h+bo ) ;
    
#ifdef __WXMSW__ // LINUX
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    myapp()->frame->InitToolBar(toolBar);
    toolBar->AddTool( MDI_TEXT_IMPORT , 
                wxBitmap (myapp()->bmpdir+"\\new.bmp", wxBITMAP_TYPE_BMP),
                txt("m_new_sequence") ) ;
    toolBar->AddTool( MDI_FILE_OPEN, 
            wxBitmap (myapp()->bmpdir+"\\open.bmp", wxBITMAP_TYPE_BMP), 
            txt("m_open") , txt("m_opentxt") );
    toolBar->Realize() ;
#endif

    wxCommandEvent ev ;
    OnSettings ( ev ) ;
    sc->SetFocus() ;
    myapp()->frame->setChild ( this ) ;
    }

// Callung clustalw.exe
void TAlignment::redoAlignments ()
    {
    // Cleaning up
    while ( sc->seq.size() )
        {
        delete sc->seq[sc->seq.size()-1] ;
        sc->seq.pop_back () ;
        }

    if ( qName.size() == 0 ) return ;

    // Align
    int a ;
    qAlign.clear () ;
    qName.clear () ;
    while ( qAlign.size() ) qAlign.pop_back () ;
    for ( a = 0 ; a < qVec.size() ; a++ )
        qName.push_back ( qVec[a]->name ) ;

    if ( qName.size() <= 1 ) // Just one sequence
        {
        qAlign.push_back ( qVec[0]->sequence ) ;
        }
    else if ( algorithm == ALG_CW ) // Clustal-W, external
        {
        SetCursor ( *wxHOURGLASS_CURSOR ) ;

        wxString cwt = "clustalw.txt" ;
        wxString hd = myapp()->homedir ;
        wxString tx = hd + "\\" + cwt ;
        ofstream out ( tx.c_str() , ios::out ) ;
        for ( a = 0 ; a < qVec.size() ; a++ )
           {
           out << ">" << qName[a] << endl ;
           out << qVec[a]->sequence << endl ;
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
        qAlign.clear() ;
        while ( qAlign.size() < qVec.size() ) qAlign.push_back ( "" ) ;
        qName.push_back ( txt("t_consensus") ) ;
        qAlign.push_back ( "" ) ; // Consensus sequence
        while ( !in.eof() )
           {
           for ( a = 0 ; a < qAlign.size() ; a++ )
              {
              if ( qAlign[0] != "" ) getline ( in , s ) ;
              qAlign[a] += s.substr ( off , s.length() ) ;
              }
           getline ( in , s ) ; // Blank line
           }

        SetCursor ( *wxSTANDARD_CURSOR ) ;
        }
    else // Internal routines
        {
        SetCursor ( *wxHOURGLASS_CURSOR ) ;
        wxStopWatch sw ;
        
        for ( a = 0 ; a < 2 ; a++ ) qAlign.push_back ( qVec[a]->sequence ) ;

        if ( algorithm == ALG_NW )
           NeedlemanWunsch ( qAlign[0] , qAlign[1] ) ; 
        else if ( algorithm == ALG_SW )
           SmithWaterman ( qAlign[0] , qAlign[1] ) ; 
/*
        int first , second ;
        for ( first = 0 ; first < qAlign.size() ; first++ )
           {
           for ( second = first+1 ; second < qAlign.size() ; second++ )
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
*/

        while ( qName.size() > 2 ) qName.pop_back () ;
        SetCursor ( *wxSTANDARD_CURSOR ) ;
        }

    // Display
    sc->maxendnumberlength = strlen ( txt("t_consensus") ) ;
    SeqNum *n = new SeqNum ( sc ) ;
    sc->seq.push_back ( n ) ;
    n->s = qAlign[0] ;

    for ( a = 0 ; a < qName.size() ; a++ )
        {
        SeqAlign *d = new SeqAlign ( sc ) ;
        sc->seq.push_back ( d ) ;
        d->s = qAlign[a] ;
        d->myname = qName[a] ;
        }
    sc->arrange () ;
    sc->Refresh () ;
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

    if ( qName.size() == 0 ) return ;

    // Align
    while ( qAlign.size() ) qAlign.pop_back () ;
    for ( a = 0 ; a < qVec.size() ; a++ )
        qAlign.push_back ( qVec[a]->sequence ) ;

    if ( qName.size() > 1 )
        {
        SetCursor ( *wxHOURGLASS_CURSOR ) ;
        wxStopWatch sw ;

        int first , second ;
        for ( first = 0 ; first < qAlign.size() ; first++ )
           {
           for ( second = first+1 ; second < qAlign.size() ; second++ )
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

    for ( a = 0 ; a < qName.size() ; a++ )
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
    
string TAlignment::getName ()
    {
    return "Alignment" ;
    }

void TAlignment::OnSettings ( wxCommandEvent &ev )
    {
    TAlignmentDialog ad ( this , txt("t_settings") ) ;
    int r = ad.ShowModal () ;
    if ( r != wxID_OK ) return ;

    qName = ad.vcn ;
    qVec = ad.vcv ;
    match = ad.alg_match->GetValue() ;
    mismatch = ad.alg_mismatch->GetValue() ;
    gap_penalty = ad.alg_penalty->GetValue() ;
    matrix = ad.alg_matrix->GetStringSelection().c_str() ;
    
    algorithm = ad.alg->GetSelection () ;
    
    redoAlignments () ;
    }

// ******************************************* TAlignmentDialog

TAlignmentDialog::TAlignmentDialog(wxWindow *parent, const wxString& title )
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
    bo = 5 ;
    th = 15 ;
    al = (TAlignment*) parent ;
    int w , h ;
    GetClientSize ( &w , &h ) ;
    nb = new wxNotebook ( this , -1 , wxPoint ( 0 , 0 ) , wxSize ( w , h-40 ) ) ;
    init_what () ;
    init_how () ;
    init_disp () ;
    wxButton *b = new wxButton ( this , AL_OK , txt("b_OK") , wxPoint ( w/3 , h-30 ) ) ;
    new wxButton ( this , AL_CANCEL , txt("b_cancel") , wxPoint ( w/2 , h-30 ) ) ;
    b->SetDefault () ;
    b->SetFocus () ;
    }
    
TAlignmentDialog::~TAlignmentDialog ()
    {
    nb->DeleteAllPages() ;
    }
    
void TAlignmentDialog::init_what ()
    {
    pwhat = new wxPanel ( this , -1 ) ;
    nb->AddPage ( pwhat , txt("t_sequences") ) ;
    wxPanel *p = pwhat ;
    int w , h ;
    p->GetClientSize ( &w , &h ) ;
    new wxStaticText ( p , -1 , txt("al_cur") , wxPoint ( bo , bo ) ) ;
    new wxStaticText ( p , -1 , txt("al_all") , wxPoint ( bo + w*2/3 , bo ) ) ;
    cur = new wxListBox ( p , AL_CUR , 
                wxPoint ( bo , bo*2+th ) , 
                wxSize ( w/3-bo , h-bo*2-th ) ,
                0 , NULL , wxLB_EXTENDED ) ;
    all = new wxListBox ( p , AL_ALL , 
                wxPoint ( bo + w*2/3 , bo*2+th ) , 
                wxSize ( w*2/3-bo , h-bo*2-th ) ,
                0 , NULL , wxLB_EXTENDED ) ;
    new wxButton ( p , AL_ADD , txt("<-- add") , wxPoint ( w*3/8 , th*2 ) , wxSize ( w/4 , th*3/2 ) ) ;
    new wxButton ( p , AL_DEL , txt("del -->") , wxPoint ( w*3/8 , th*4 ) , wxSize ( w/4 , th*3/2 ) ) ;
    new wxButton ( p , AL_UP , txt("b_up_in_list") , wxPoint ( w/3+bo , th*7 ) , wxSize ( w/6 , th*3/2 ) ) ;
    new wxButton ( p , AL_DOWN , txt("b_down_in_list") , wxPoint ( w/3+bo , th*9 ) , wxSize ( w/6 , th*3/2 ) ) ;
    
    int a ;
    MyFrame *f = myapp()->frame ;
    // All
    for ( a = 0 ; a < f->children.size() ; a++ )
        {
        if ( f->children[a]->def == "dna" || f->children[a]->def == "AminoAcids" )
           {
           vav.push_back ( ((MyChild*)f->children[a])->vec ) ;
           van.push_back ( f->children[a]->getName() ) ;
           all->Append ( f->children[a]->getName().c_str() ) ;
           }
        }
    
    // Current
    for ( a = 0 ; a < al->qName.size() ; a++ )
        {
        vcv.push_back ( al->qVec[a] ) ;
        vcn.push_back ( al->qName[a] ) ;
        cur->Append ( al->qName[a].c_str() ) ;
        }
        
    if ( vcn.size() == 0 )
        {
        for ( a = 0 ; a < van.size() ; a++ )
           {
           vcn.push_back ( van[a] ) ;
           vcv.push_back ( vav[a] ) ;
           cur->Append ( van[a].c_str() ) ;
           }
        }
    }
    
void TAlignmentDialog::init_how ()
    {
    phow = new wxPanel ( this , -1 ) ;
    nb->AddPage ( phow , txt("t_algorithm") ) ;
    wxPanel *p = phow ;
    int w , h ;
    p->GetClientSize ( &w , &h ) ;
    wxStaticText *st ;
    wxRect r ;
    st = new wxStaticText ( p , -1 , txt("t_algorithm") , wxPoint ( bo , th ) ) ;
    r = st->GetRect () ;
    alg = new wxListBox ( p , AL_ALG , 
                wxPoint ( bo , r.GetBottom()+bo ) , 
                wxSize ( w/2-bo , h/2 ) ,
                0 , NULL , wxLB_SINGLE ) ;
    alg->Append ( txt("t_smith_waterman") ) ;
    alg->Append ( txt("t_needleman_wunsch") ) ;
    alg->Append ( txt("t_clustal_w") ) ;
    alg->SetSelection ( al->algorithm ) ;
    
    new wxStaticText ( p , -1 , txt("t_alg_param") , wxPoint ( w/2+bo , th ) ) ;
    st = new wxStaticText ( p , -1 , txt("t_match") , wxPoint ( w/2+bo , th*2+bo ) ) ;
    r = st->GetRect() ;
    int xo = r.GetRight() ;
    
    st = new wxStaticText ( p , -1 , txt("t_mismatch") , wxPoint ( w/2+bo , th*4+bo ) ) ;
    r = st->GetRect() ;
    if ( xo < r.GetRight() ) xo = r.GetRight() ;
    
    st = new wxStaticText ( p , -1 , txt("t_gap_penalty") , wxPoint ( w/2+bo , th*6+bo ) ) ;
    r = st->GetRect() ;
    if ( xo < r.GetRight() ) xo = r.GetRight() ;
    xo += bo ;
    
    alg_match = new wxSpinCtrl ( p , AL_MATCH , "" ,
                                    wxPoint ( xo , th*2 ) ,
                                    wxSize ( w/5 , th*3/2 ) ) ;
    alg_match->SetRange ( -100 , 100 ) ;
    alg_match->SetValue ( al->match ) ;

    alg_mismatch = new wxSpinCtrl ( p , AL_MISMATCH , "" ,
                                    wxPoint ( xo , th*4 ) ,
                                    wxSize ( w/5 , th*3/2 ) ) ;
    alg_mismatch->SetRange ( -100 , 100 ) ;
    alg_mismatch->SetValue ( al->mismatch ) ;

    alg_penalty = new wxSpinCtrl ( p , AL_PENALTY , "" ,
                                    wxPoint ( xo , th*6 ) ,
                                    wxSize ( w/5 , th*3/2 ) ) ;
    alg_penalty->SetRange ( -100 , 100 ) ;
    alg_penalty->SetValue ( al->gap_penalty ) ;
    
    alg_matrix = new wxChoice ( p , AL_MATRIX , wxPoint ( xo , th*8 ) ) ;
    alg_matrix->Append ( txt("t_matrix_blosum") ) ;
    alg_matrix->Append ( txt("t_matrix_pam") ) ;
    alg_matrix->Append ( txt("t_matrix_gonnet") ) ;
    alg_matrix->Append ( txt("t_matrix_id") ) ;
    alg_matrix->SetSelection ( 0 ) ;
    }

void TAlignmentDialog::init_disp ()
    {
    pdisp = new wxPanel ( this , -1 ) ;
    nb->AddPage ( pdisp , txt("t_display") ) ;
    wxPanel *p = pdisp ;
    int w , h ;
    p->GetClientSize ( &w , &h ) ;
    }

void TAlignmentDialog::OnOK ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }

void TAlignmentDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }

void TAlignmentDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TAlignmentDialog::OnAdd ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = all->GetSelections ( sel ) ;
    for ( a = 0 ; a < n ; a++ )
        {
        for ( b = 0 ; b < vcv.size() && vcv[b] != vav[sel[a]] ; b++ ) ;
        if ( b == vcv.size() )
           {
           vcv.push_back ( vav[sel[a]] ) ;
           vcn.push_back ( van[sel[a]] ) ;
           cur->Append ( van[sel[a]].c_str() ) ;
           }
        }
    }

void TAlignmentDialog::OnDel ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = cur->GetSelections ( sel ) ;
    for ( a = 0 ; a < n ; a++ ) vcv[sel[a]] = NULL ;
    for ( a = 0 ; a < vcv.size() ; a++ )
        {
        if ( vcv[a] == NULL )
           {
           for ( b = a+1 ; b < vcv.size() ; b++ )
              {
              vcv[b-1] = vcv[b] ;
              vcn[b-1] = vcn[b] ;
              }
           vcv.pop_back () ;
           vcn.pop_back () ;
           a-- ;
           }
        }
    cur->Clear () ;
    for ( a = 0 ; a < vcn.size() ; a++ )
        cur->Append ( vcn[a].c_str() ) ;
    }

void TAlignmentDialog::OnUp ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = cur->GetSelections ( sel ) ;
    if ( n != 1 ) return ;
    b = sel[0] ;
    if ( b == 0 ) return ;
    TVector *d_v = vcv[b] ; vcv[b] = vcv[b-1] ; vcv[b-1] = d_v ;
    string d_n = vcn[b] ; vcn[b] = vcn[b-1] ; vcn[b-1] = d_n ;
    cur->Clear () ;
    for ( a = 0 ; a < vcn.size() ; a++ )
        cur->Append ( vcn[a].c_str() ) ;
    cur->SetSelection ( b-1 ) ;
    }

void TAlignmentDialog::OnDown ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = cur->GetSelections ( sel ) ;
    if ( n != 1 ) return ;
    b = sel[0] ;
    if ( b == vcv.size()-1 ) return ;
    TVector *d_v = vcv[b] ; vcv[b] = vcv[b+1] ; vcv[b+1] = d_v ;
    string d_n = vcn[b] ; vcn[b] = vcn[b+1] ; vcn[b+1] = d_n ;
    cur->Clear () ;
    for ( a = 0 ; a < vcn.size() ; a++ )
        cur->Append ( vcn[a].c_str() ) ;
    cur->SetSelection ( b+1 ) ;
    }

