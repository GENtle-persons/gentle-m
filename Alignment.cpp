/** \file
    \brief Contains the methods of the TAlignment class and its helper class, TAlignLine
*/
#include "Alignment.h"
#include "AlignmentAppearanceDialog.h"
#include <wx/textfile.h>
#include <wx/filename.h>

#ifdef __DEBIAN__
#ifndef USE_EXTERNAL_CLUSTALW
    #define USE_EXTERNAL_CLUSTALW
#endif
#endif

#ifndef USE_EXTERNAL_CLUSTALW
 #include "clustalw/clustalw.h"
 int clustalw_main(int argc,char **argv) ;
#endif

BEGIN_EVENT_TABLE(TAlignment, MyChildBase)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_CHECKBOX(ALIGN_HORIZ, TAlignment::OnHorizontal)
    EVT_MENU(MDI_FILE_SAVE, TAlignment::OnFileSave)
    EVT_MENU(MDI_MARK_ALL,TAlignment::OnMarkAll)

    EVT_MENU(ALIGNMENT_SETTINGS,TAlignment::OnSettings)
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
    EVT_MENU(SEQ_PRINT, TAlignment::OnSeqPrint)
    EVT_MENU(MDI_COPY,TAlignment::OnCopy)
    EVT_CHOICE(AA_FONTSIZE,TAlignment::OnFontsize)

    // Dummies
    EVT_MENU(MDI_TOGGLE_FEATURES,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_UNDO,ChildBase::OnDummy)
    EVT_MENU(MDI_CUT,ChildBase::OnDummy)
    EVT_MENU(MDI_PASTE,ChildBase::OnDummy)
    EVT_MENU(MDI_EDIT_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_EXPORT,ChildBase::OnExport)
    EVT_MENU(MDI_FIND,ChildBase::OnDummy)
    EVT_MENU(AA_NONE,TABIviewer::OnDummy)
    EVT_MENU(AA_KNOWN, TABIviewer::OnDummy)
    EVT_MENU(AA_ALL, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE, TABIviewer::OnDummy)
    EVT_MENU(AA_ONE, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_3, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M3, TABIviewer::OnDummy)

END_EVENT_TABLE()


TAlignment::TAlignment(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    name = "" ;
    database = "" ;
    keepIdentity = false ;
    threadRunning = false ;
    invs = cons = bold = mono = false ;
    def = _T("alignment") ;
    match = 2 ; // Match value
    mismatch = -1 ; // Mismatch score
    gap_penalty = -2 ; // Gap penalty
    matrix = _T("BLOSUM") ;
    gap = "-" ;
    algorithm = ALG_CW ;
    vec = NULL ;
    aaa = NULL ;
    colCur = NULL ;
    readTabColors ( myapp()->homedir.GetFullPath() + wxFileName::GetPathSeparator() + _T("default.tab") ) ;
    allow_print = allow_save = true ;
    }

TAlignment::~TAlignment ()
    {
    }

void TAlignment::OnFontsize ( wxCommandEvent& event )
    {
    long l ;
    wxString s = fontsize->GetStringSelection() ;
    s.ToLong ( &l ) ;
    sc->set_font_size ( (int) l ) ;
    updateSequence () ;
//    updateSequenceCanvas ( true ) ;
    }

bool TAlignment::isDNA ()
    {
    wxString s ;
    int a , b = 0 ;
    for ( a = 0 ; a < lines.size() ; a++ )
        {
        if ( !lines[a].isIdentity ) s += lines[a].s ;
        }

    for ( a = 0 ; a < s.length() ; a++ )
        {
        wxChar c = s.GetChar ( a ) ;
        if ( c == 'N' || c == ' ' || c == '-' ) continue ;
        if ( c == 'A' || c == 'C' || c == 'G' || c == 'T' ) continue ;
        b++ ;
        }

    // Guess : if more than 1/4 of the sequence are not ACTGN, it's an amino acid sequence
    if ( b >= s.length() / 4 ) return false ;
    return true ;
    }

bool TAlignment::isAA ()
    {
    return !isDNA() ;
    }

void TAlignment::readTabColors ( wxString filename )
    {
    // Now we read a color scheme in BioEdit format
    wxString fn = filename ;
    wxTextFile in ( fn ) ;
    in.Open ( *(myapp()->isoconv) ) ;
    int a ;
    wxColour *c = NULL ;
    for ( a = 0 ; a < 256 ; a++ )
        {
        c = colDNA ;
        c[a].Set ( 0 , 100 , 0  ) ;
        c = colAA ;
        c[a].Set ( 0 , 100 , 0  ) ;
        }
    bool initial = true ;
    while ( !in.Eof() )
        {
        wxString s ;
        if ( initial ) s = in.GetFirstLine() ;
        else s = in.GetNextLine() ;
        initial = false ;
        if ( s.IsEmpty() ) ;
        else if ( s.GetChar(0) == '/' )
           {
           if ( s == _T("/amino acids/") ) c = colAA ;
           else if ( s == _T("/nucleotides/") ) c = colDNA ;
           else c = NULL ;
           }
        else if ( c )
           {
           wxString t = in.GetNextLine () ; // s in int, t is list of chars
           int x = atoi ( s.mb_str() ) ;
           for ( unsigned int b = 0 ; b < t.length() ; b++ )
              {
              unsigned char ch = (unsigned char)t.GetChar(b) ;
              int red = ( x >> 16 ) & 255 ;
              int green = ( x >> 8 ) & 255 ;
              int blue = x & 255 ;
              c[ch].Set ( red , green , blue ) ;
              }
           }
        }
    colCur = colAA ;
    }


wxColour TAlignment::findColors ( char c1 , char c2 , bool fg )
    {
    wxColour r ;
    if ( invs && c1 != '-' ) fg = !fg ;
    if ( fg )
        {
        int cc = (unsigned char) c1 ;
        if ( !colCur ) r = *wxBLACK ;
        else if ( cc < 0 || cc >= 256 ) r = *wxBLACK ;
        else r = colCur[cc] ;
        }
    else r = *wxWHITE ;

//    if ( fg && c1 != '-' && c2 != '-' && c1 != c2 )
//        r = wxColour ( 200 , 0 , 0 ) ;

    return r ;
    }


void TAlignment::initme ()
    {
    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu ( FILE_MENU_SAVE|FILE_MENU_EXPORT ) ;
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

    // Not implemented yet
    menu_bar->FindItem(ALIGN_SOA )->Enable ( false ) ;
    menu_bar->FindItem(ALIGN_SOAI)->Enable ( false ) ;
    menu_bar->FindItem(ALIGN_SIML)->Enable ( false ) ;
    menu_bar->FindItem(ALIGN_SEQ )->Enable ( false ) ;
    menu_bar->FindItem(ALIGN_FEAT)->Enable ( false ) ;
    menu_bar->FindItem(ALIGN_RNA )->Enable ( false ) ;


    cons = true ;
    showIdentity = true ;

    Hide () ;

//    hs = new wxSplitterWindow ( this , SPLIT_ALIGNMENT ) ;

    // Sequence Canvas
    sc = new SequenceCanvas ( this , wxPoint ( 0 , 0 ) , wxSize ( -1 , -1 ) ) ;
    sc->blankline = 1 ;
    sc->child = this ;
    sc->EnableScrolling ( true , true ) ;

    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    toolBar->Reparent ( this ) ;
    toolbar = toolBar ;
    myapp()->frame->InitToolBar(toolBar);
    myapp()->frame->addTool ( toolBar , MDI_TEXT_IMPORT ) ;
    myapp()->frame->addTool ( toolBar , MDI_FILE_OPEN ) ;
    myapp()->frame->addTool ( toolBar , MDI_FILE_SAVE ) ;
    myapp()->frame->addTool ( toolBar , SEQ_PRINT ) ;
    if ( !myapp()->frame->mainToolBar ) toolBar->AddSeparator () ;
    toolBar->AddTool( ALIGNMENT_SETTINGS,
                wxEmptyString,
                myapp()->frame->bitmaps[21],
                txt("t_alignment_settings") ) ;
    toolBar->AddSeparator () ;

    wxCheckBox *mycb = new wxCheckBox ( toolBar , ALIGN_HORIZ , txt("t_horizontal") ) ;
    toolBar->AddControl ( mycb ) ;
    toolBar->AddSeparator () ;
    mmb = new wxChoice ( toolBar , -1 ) ;
    mmb->Append ( txt("t_mmb_insert_gap") ) ;
    mmb->Append ( txt("t_mmb_delete_gap") ) ;
    mmb->Append ( txt("t_mmb_insert_gap_others") ) ;
    mmb->Append ( txt("t_mmb_delete_gap_others") ) ;
    mmb->SetStringSelection ( txt("t_mmb_insert_gap") ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_mmb") ) ) ;
    toolBar->AddControl ( mmb ) ;
    fontsize = myapp()->frame->AddFontsizeTool ( toolBar , AA_FONTSIZE ) ;
    myapp()->frame->addDefaultTools ( toolBar ) ;
    toolbar = toolBar ;

    toolBar->Realize() ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( sc , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

//    Maximize () ;
    sc->SetFocus() ;
    myapp()->frame->setChild ( this ) ;
    readTabColors ( myapp()->homedir.GetFullPath() + wxFileName::GetPathSeparator() + _T("default.tab") ) ;
    }

void* TAlignment::Entry()
    {
    int a ;
    TAlignLine line ;
    wxString cwt = "clustalw.txt" ;
    wxString hd = myapp()->homedir.GetFullPath() ;

    wxString tmpdir = wxFileName::CreateTempFileName ( "" ) ;
    tmpdir.Replace ( "\\" , "/" ) ;
    tmpdir = tmpdir.BeforeLast ( '/' ) ;

    wxString tx = tmpdir + "/" + cwt ;

    wxString cwd = wxGetCwd() ;
    wxSetWorkingDirectory ( tmpdir ) ;

    wxFile out ( tx , wxFile::write ) ;
    for ( a = 0 ; a < lines.size() ; a++ )
        {
        out.Write ( wxString::Format ( ">%d\n" , a ) ) ;
        out.Write ( lines[a].v->getSequence() + "\n" ) ;
        }
    out.Close() ;

#ifdef USE_EXTERNAL_CLUSTALW // Use external ClustalW
    wxString bn ;

#ifdef __WXMSW__
    // This is not used anymore under Windows
    bn = tmpdir + "\\clustalw.bat" ;
    wxFile bat ( bn , wxFile::write ) ;
    bat.Write ( "@echo off\n")  ;
    bat.Write ( "cd " + tmpdir + "\n")  ;
    bat.Write ( "clustalw.exe clustalw.txt" +
                wxString::Format ( " /gapopen=%d" , gap_penalty ) +
                wxString::Format ( " /gapext=%d" , mismatch ) + "\n" ) ;
    bat.Close() ;
#else
     // External call to be used by Debian
     bn = "clustalw clustalw.txt" +
     wxString::Format ( " /gapopen=%d" , gap_penalty ) +
     wxString::Format ( " /gapext=%d" , mismatch ) + "\n" ;
//        cout << "Executing " << bn.mb_str() << endl ;
#endif
    wxExecute ( bn , wxEXEC_SYNC ) ;


#else // Using internal ClustalW - cool!


    wxString a1 = wxString::Format ( "/gapopen=%d" , gap_penalty ) ;
    wxString a2 = wxString::Format ( "/gapext=%d" , mismatch ) ;
    wxString text = tmpdir + "/clustalw.txt" ;
    char *av[4] ;
    av[0] = new char[100] ; strcpy ( av[0] , "clustalw.exe" ) ;
    av[1] = new char[500] ; strcpy ( av[1] , text.mb_str() ) ;
    av[2] = new char[100] ; strcpy ( av[2] , a1.mb_str() ) ;
    av[3] = new char[100] ; strcpy ( av[3] , a2.mb_str() ) ;
    clustalw_main ( 4 , av ) ;
#endif


    wxString aln = tmpdir + "/clustalw.aln" ;
    wxTextFile in ( aln ) ;
    in.Open ( *(myapp()->isoconv) ) ;
    wxString s = in.GetFirstLine() ;
    do {
        s = in.GetNextLine() ;
    } while ( s.IsEmpty() ) ;
    int off ;
    for ( off = s.length()-1 ; s.GetChar(off-1) != ' ' ; off-- ) ;
    line.isIdentity = true ;
    line.name = txt("t_identity") ;
    lines.push_back ( line ) ;
    bool first = true ;
    for ( a = 0 ; a < lines.size() ; a++ ) lines[a].s = "" ;
    while ( !in.Eof() )
        {
        for ( a = 0 ; a < lines.size() ; a++ )
            {
            if ( !first ) s = in.GetNextLine() ;
            else first = false ;
            long index ;
            wxString i = s.substr ( 0 , off-1 ) ;
            i.ToLong ( &index ) ;
            if ( s.GetChar(0) == ' ' )
                index = lines.size()-1 ;
            lines[index].s += s.substr ( off , s.length() ) ;
            }
            if ( !in.Eof() )
                s = in.GetNextLine() ; // Blank line
        }

    wxSetWorkingDirectory ( cwd ) ;

#ifndef __WXMSW__
    wxMutexGuiEnter() ;
#endif

    keepIdentity = true ;
    redoAlignments ( true ) ;
    keepIdentity = false ;

#ifndef __WXMSW__
    wxMutexGuiLeave() ;
#endif
    return NULL ;
    }


// Calculating alignments; all changes are lost!
void TAlignment::recalcAlignments ()
    {
    if ( !keepIdentity )
        {
        while ( lines.size() && lines[lines.size()-1].isIdentity )
            lines.pop_back () ;
        }
    if ( lines.size() == 0 ) return ;

    // Align
    int a ;
    TAlignLine line ;
    SetCursor ( *wxHOURGLASS_CURSOR ) ;
    if ( lines.size() <= 1 ) // Just one sequence
        {
        lines[0].ResetSequence() ;
        }
    else if ( algorithm == ALG_CW ) // Clustal-W
        {
            if ( threadRunning )
            {
                generateConsensusSequence ( false ) ;
                threadRunning = false ;
//              SetCursor ( *wxSTANDARD_CURSOR ) ;
            }
            else
            {
                SetCursor ( *wxSTANDARD_CURSOR ) ;
                threadRunning = true ;
                sc->Refresh () ;
//              wxSafeYield() ;
                wxThreadHelper::CreateThread () ;
                GetThread()->Run() ;
                return ;
            }
        }
    else // Internal routines
        {
        myass ( lines.size() > 0 , "Alignment::recalcAlignments:internal1" ) ;
//        while ( lines.size() > 2 ) lines.pop_back () ;
        for ( a = 0 ; a < lines.size() ; a++ ) lines[a].ResetSequence () ;

        for ( a = 1 ; a < lines.size() ; a++ )
            {
            wxString s0 = lines[0].s ;

            if ( algorithm == ALG_NW )
                NeedlemanWunsch ( s0 , lines[a].s ) ;
            else if ( algorithm == ALG_SW )
                SmithWaterman ( s0 , lines[a].s ) ;

            if ( lines[0].s == s0 ) continue ; // No gaps were introduced into first sequence

            int b ;
            for ( b = 0 ; b <= a ; b++ ) // All lines get the same length
                {
                while ( lines[b].s.length() < s0.length() )
                    lines[b].s += " " ;
                }
            for ( b = 0 ; b < s0.length() ; b++ ) // Insert gaps
                {
                if ( lines[0].s.GetChar(b) != s0.GetChar(b) ) // New gap
                    {
                    for ( int c = 0 ; c < a ; c++ )
                        {
                        for ( int d = s0.length()-1 ; d > b && d >= 0 ; d-- )
                            lines[c].s.SetChar ( d , lines[c].s.GetChar(d-1) ) ;
                        myass ( lines[c].s.length() > b , "Alignment::recalcAlignments:internal2" ) ;
                        lines[c].s.SetChar ( b , '-' ) ;
                        }
                    }
                }
           }

        generateConsensusSequence ( true ) ;
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
    CLEAR_DELETE ( sc->seq ) ;

    if ( doRecalc ) recalcAlignments () ;
    if ( threadRunning ) return ;

    // Display
    sc->maxendnumberlength = txt("t_identity").length() ;

    for ( a = 0 ; a < lines.size() ; a++ )
        {
        if ( lines[a].hasFeatures() )
            {
            lines[a].showFeatures () ;
            SeqFeature *f = new SeqFeature ( sc ) ;
            f->id = a ;
            sc->seq.Add ( f ) ;
            }
        SeqAlign *d = new SeqAlign ( sc ) ;
        sc->seq.Add ( d ) ;
        if ( lines[a].isIdentity )
            d->takesMouseActions = false ;
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
                if ( lines[g].s.GetChar(a) == '-' )
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
    wxString s ;
    for ( a = 0 ; a < lines[0].s.length() ; a++ )
        {
        char c = '*' ;
        for ( b = 1 ; b < lines.size() && c == '*' ; b++ )
            {
            if ( lines[0].s.GetChar(a) != lines[b].s.GetChar(a) /*&&
                    lines[0].s.GetChar(a) != '-' &&
                    lines[b].s.GetChar(a) != '-' */)
                c = ' ' ;
            }
        s += (wxChar) c ;
        }
    line.s = s ;
    if ( addit ) lines.push_back ( line ) ;

    // The REAL consensus sequence
    consensusSequence = lines[0].s ;
    for ( a = 0 ; a < consensusSequence.length() ; a++ )
        {
        int c[256] ;
        for ( b = 0 ; b < 256 ; b++ ) c[b] = 0 ;
        for ( b = 0 ; b + 1 < lines.size() ; b++ ) c[lines[b].s.GetChar(a)]++ ;
        consensusSequence.SetChar ( a , ' ' ) ;
        for ( b = 0 ; b < 256 ; b++ )
            {
            float f = 100 * c[b] ;
            f /= lines.size() - 1 ;
            if ( f >= 60 ) consensusSequence.SetChar ( a , b ) ;
            }
        }
    }

void TAlignment::myInsert ( int line , int pos , char what )
    {
    if ( lines[line].hasFeatures() )
        {
        lines[line].getFeatures()->insert_char ( '-' , pos , false ) ;
        lines[line].s = lines[line].getFeatures()->getSequence() ;
        }
    else lines[line].s.insert ( pos-1 , wxString ( (wxChar) what ) ) ;
    }

void TAlignment::myDelete ( int line , int pos )
    {
    if ( lines[line].hasFeatures() )
        {
        lines[line].getFeatures()->doRemoveNucleotide ( pos - 1 ) ;
        lines[line].s = lines[line].getFeatures()->getSequence() ;
        }
    else lines[line].s.erase ( pos-1 , 1 ) ;
    }

void TAlignment::callMiddleMouseButton ( int id , int pos , wxString _mode )
    {
    wxString mode = mmb->GetStringSelection () ;
    if ( !_mode.IsEmpty() ) mode = txt(_mode) ;
    unsigned int a , line = id ;
    if ( lines[line].s.GetChar(pos-1) != '-' && mode == txt("t_mmb_delete_gap") )
       {
       wxBell() ;
       return ;
       }

    wxArrayInt l2s ;
    l2s.Alloc ( lines.size() ) ;
    while ( l2s.GetCount() < lines.size() ) l2s.Add ( 0 ) ;
    for ( a = 0 ; a < sc->seq.GetCount() ; a++ )
        {
        if ( sc->seq[a]->whatsthis() == _T("ALIGN") )
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
            if ( line != a && lines[a].s.GetChar(pos-1) == '-' )
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
    if ( threadRunning ) return ;
    for ( unsigned int g = 0 ; g < sc->seq.GetCount() ; g++ )
        {
        if ( sc->seq[g]->whatsthis() != _T("FEATURE") ) continue ;
        SeqFeature *f = (SeqFeature*) sc->seq[g] ;
        int id = f->id ;
        if ( lines[id].getFeatures()->getType() == TYPE_AMINO_ACIDS )
            {
            colCur = colAA ;
            if ( aaa ) delete aaa ;
            aaa = new SeqAA ( NULL ) ;
            sc->seq[g] = aaa ;
            aaa->initFromString ( lines[id].getFeatures()->getSequence() ) ;
//            aaa->fixOffsets ( lines[id].getFeatures() ) ;
            aaa->can = sc ;
            sc->arrange () ;
            sc->seq[g] = f ;
            f->initFromTVector ( lines[id].getFeatures() ) ;
            f->aaa = aaa ;
            }
        else if ( lines[id].getFeatures()->getType() == TYPE_VECTOR )
            {
            colCur = colDNA ;
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

wxString TAlignment::getName ()
    {
    return name.IsEmpty() ? _T("Alignment") : name ;
    }

void TAlignment::OnCopy ( wxCommandEvent &ev ) // This is never actually called!
    {
//    if ( sc->markedFrom() == -1 ) return ;
    }

void TAlignment::OnMarkAll ( wxCommandEvent &ev )
    {
    int a ;
    myapp()->frame->lockDisplay ( true ) ;
    sc->unmark () ;
    sc->setLastWhere ( NULL ) ;
    for ( a = 0 ; a < sc->seq.size() ; a++ )
        {
        if ( sc->seq[a]->whatsthis() != _T("ALIGN") ) continue ;
        if ( !sc->seq[a]->takesMouseActions ) continue ;
        if ( !sc->getLastWhere() ) sc->setLastWhere ( sc->seq[a] ) ;
        sc->mark ( sc->seq[a] , 1 , sc->seq[a]->s.length() , 1 ) ;
        }
    myapp()->frame->lockDisplay ( false ) ;
    sc->SilentRefresh () ;
    }

void TAlignment::OnSettings ( wxCommandEvent &ev )
    {
    TAlignmentDialog ad ( this , txt("t_settings") ) ;
    int r = ad.ShowModal () ;
    if ( r != wxID_OK ) return ;

    lines.clear () ;
    for ( unsigned int a = 0 ; a < ad.vcn.GetCount() ; a++ )
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
    matrix = ad.alg_matrix->GetStringSelection() ;

    algorithm = ad.alg->GetSelection () ;

    redoAlignments () ;
    }

void TAlignment::prealigned ( wxArrayString &vs , wxArrayChildBase &vc )
    {
    lines.clear () ;
    for ( unsigned int a = 0 ; a < vc.GetCount() ; a++ )
        {
        TAlignLine line ;
        line.name = vc[a]->getName() ;
        line.v = vc[a]->vec ;
        line.ResetSequence() ;
        line.s = vs[a] ;
        lines.push_back ( line ) ;
        }

    redoAlignments ( false ) ;
    }

// HOMEMADE ALIGNMENT ALGORITHMS

int TAlignment::NeedlemanWunsch ( wxString &s1 , wxString &s2 )
    {
    return MatrixAlignment ( s1 , s2 , false ) ;
    }

int TAlignment::SmithWaterman ( wxString &s1 , wxString &s2 )
    {
    return MatrixAlignment ( s1 , s2 , true ) ;
    }

int TAlignment::MatrixAlignment ( wxString &_s1 , wxString &_s2 , bool local )
    {
    wxString s1 = _s1 ;
    wxString s2 = _s2 ;
    int a , b ;
    int M = s1.length() ;
    int N = s2.length() ;

    // Initializing backlink matrix

    vector <wxArrayInt> back ;
    wxArrayInt blank_b ;
    blank_b.Alloc ( N+1 ) ;
    back.reserve ( M+1 ) ;
    while ( blank_b.GetCount() < N+1 ) blank_b.Add ( 0 ) ;
    while ( back.size() < M+1 ) back.push_back ( blank_b ) ;

    // Initializing pseudo-matrix (simulated by two altering lines)
    int *matrix0 , *matrix1 , *matrix2 ;
    matrix0 = new int[N+1] ;
    matrix1 = new int[N+1] ;
    for ( a = 0 ; a < N+1 ; a++ ) matrix1[a] = 0 ;

    // Filling
    int i , j ;
    int max = -999999 ;
    wxArrayInt vi , vj ;
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
           int s = (s1.GetChar(i)==s2.GetChar(j))?match:mismatch ;

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
                  vi.Clear() ;
                  vj.Clear() ;
                  }
              vi.Add ( x ) ;
              vj.Add ( y ) ;
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
    wxString t1 , t2 ;
    if ( local )
        {
        for ( a = b = 0 ; a < vi.GetCount() ; a++ )
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
    wxString k1 , k2 ;
    // The beginning
    char gap0 = gap.GetChar(0) ;
    for ( a = b = 0 ; a < t1.size() ; a++ )
        if ( t1.GetChar(a) != gap0 ) b++ ;
    k1 = s1.substr ( 0 , mi - b ) ;
    for ( a = b = 0 ; a < t2.size() ; a++ )
        if ( t2.GetChar(a) != gap0 ) b++ ;
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

    _s1 = t1 ;
    _s2 = t2 ;
    return max ;
    }

void TAlignment::MatrixBacktrack ( vector <wxArrayInt> &back ,
                                    wxString s1 , wxString s2 ,
                                    wxString &t1 , wxString &t2 ,
                                    int i , int j )
    {
    t1 = "" ;
    t2 = "" ;
    while ( i > 0 || j > 0 )
        {
        if ( (back[i][j]&BACK_LU) == BACK_LU ) // upper left
           {
           t1 = s1.GetChar(--i) + t1 ;
           t2 = s2.GetChar(--j) + t2 ;
           }
        else if ( (back[i][j]&BACK_LEFT) > 0 ) // left
           {
           t1 = s1.GetChar(--i) + t1 ;
           t2 = gap + t2 ;
           }
        else if ( ( back[i][j]&BACK_UP ) > 0 ) // up
           {
           t1 = gap + t1 ;
           t2 = s2.GetChar(--j) + t2 ;
           }
        else break ;
        }
    }


void TAlignment::invokeOriginal ( int id , int pos )
    {
    ChildBase *c = NULL ;
    c = lines[id].FindOrigin() ;
    if ( !c ) return ;

    myapp()->frame->mainTree->SelectItem ( c->inMainTree ) ;
    if ( c->def == _T("dna") )
        {
        MyChild *c1 = ((MyChild*)c) ;
        c1->cSequence->mark ( _T("DNA") , pos , pos , 1 ) ;
        c1->cSequence->ensureVisible ( pos ) ;
        }
    else if ( c->def == _T("AminoAcids") )
        {
        TAminoAcids *c1 = ((TAminoAcids*)c) ;
        c1->sc->mark ( _T("AA") , pos , pos , 1 ) ;
        c1->sc->ensureVisible ( pos ) ;
        }
    else if ( c->def == _T("ABIviewer") )
        {
        TABIviewer *c1 = ((TABIviewer*)c) ;
        c1->sc->mark ( _T("ABI") , pos , pos , 1 ) ;
        c1->sc->ensureVisible ( pos ) ;
        }
    }

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
    if ( threadRunning ) return ;
    sc->arrange () ;
    sc->SilentRefresh() ;
    }

void TAlignment::OnMenuBold ( wxCommandEvent &ev )
    {
    bold = !bold ;
    if ( threadRunning ) return ;
    sc->arrange () ;
    sc->SilentRefresh() ;
    }

void TAlignment::OnMenuMono ( wxCommandEvent &ev )
    {
    mono = !mono ;
    if ( threadRunning ) return ;
    sc->arrange () ;
    sc->SilentRefresh() ;
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
    for ( unsigned int a = 0 ; a < sc->seq.GetCount() ; a++ )
        {
//        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuSoaI ( wxCommandEvent &ev )
    {
    for ( unsigned int a = 0 ; a < sc->seq.GetCount() ; a++ )
        {
//        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuSiml ( wxCommandEvent &ev )
    {
    for ( unsigned int a = 0 ; a < sc->seq.GetCount() ; a++ )
        {
//        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuSeq ( wxCommandEvent &ev )
    {
    for ( unsigned int a = 0 ; a < sc->seq.GetCount() ; a++ )
        {
//        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuFeat ( wxCommandEvent &ev )
    {
    for ( unsigned int a = 0 ; a < sc->seq.GetCount() ; a++ )
        {
//        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuRNA ( wxCommandEvent &ev )
    {
    for ( unsigned int a = 0 ; a < sc->seq.GetCount() ; a++ )
        {
//        SeqAlign *x = (SeqAlign*) sc->seq[a] ;
        }
    }

void TAlignment::OnMenuCons ( wxCommandEvent &ev )
    {
    cons = !cons ;
    if ( threadRunning ) return ;
    sc->arrange () ;
    sc->SilentRefresh() ;
    }

void TAlignment::OnMenuIdent ( wxCommandEvent &ev )
    {
    showIdentity = !showIdentity ;
    if ( threadRunning ) return ;
    if ( showIdentity )
        {
        int a = lines.size()-1 ;
        SeqAlign *d = new SeqAlign ( sc ) ;
        sc->seq.Add ( d ) ;
        d->takesMouseActions = false ;
        d->id = a ;
        d->s = lines[a].s ;
        d->myname = lines[a].name ;
        }
    else
        {
        delete sc->seq[sc->seq.GetCount()-1] ;
        sc->seq.RemoveAt ( sc->seq.GetCount()-1 ) ;
        }
    sc->arrange () ;
    sc->SilentRefresh() ;
    }

void TAlignment::OnHorizontal ( wxCommandEvent& event )
    {
    sc->toggleHorizontal () ;
    if ( threadRunning ) return ;
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

void TAlignment::OnFileSave ( wxCommandEvent &ev )
    {
    unsigned int a , b ;
    wxString s , d ;
    TGenBank gb ;
    for ( a = b = 0 ; a < lines.size() ; a++ ) b += lines[a].isIdentity?0:1 ;
    d = wxString::Format ( "%u\n" , b ) ;
    for ( a = 0 ; a < lines.size() ; a++ )
        {
        if ( lines[a].isIdentity ) continue ;

        wxString p = lines[a].v->getParams() ;
        lines[a].v->setParams ( "" ) ;
        d += lines[a].v->getName() + "\n" ;
        d += lines[a].v->getDatabase() + "\n" ;
        d += lines[a].s + "\n" ;
        wxArrayString ex ;
        gb.doExport ( lines[a].v , ex ) ;
        for ( b = 0 ; b < ex.GetCount() ; b++ )
            {
            s += ex[b] ;
            s += "\n" ;
            }
        lines[a].v->setParams ( p ) ;
        }

    // Add layout
    d += wxString::Format ( "%ld\n" , lines.size() ) ;
    for ( a = 0 ; a < lines.size() ; a++ )
        {
        wxString layout ;
        layout += "<layout>" ;
        for ( b = 0 ; b < lines[a].markup.size() ; b++ )
            layout += lines[a].markup[b].getXML() ;
        layout += "</layout>\n" ;
        d += layout ;
        }

    if ( !vec ) vec = new TVector ; // Wasting memory
    vec->setName ( name.IsEmpty() ? txt("t_alignment") : name ) ;
    vec->setDatabase ( database ) ;
    vec->setDescription ( d ) ;
    vec->setSequence ( s ) ;
    vec->setType ( TYPE_ALIGNMENT ) ;
    TManageDatabaseDialog dbd ( this , txt("t_store") , ACTION_MODE_SAVE , vec ) ;
    dbd.ShowModal () ;
    }

void TAlignment::doExport ( wxString filename , int filter )
    {
    wxFile out ( filename , wxFile::write ) ;

    int a , b ;
    if ( filter == 0 ) // GenBank
        {
        TGenBank gb ;
/*      wxArrayString ex ;
        gb.doExport ( vec , ex ) ;
        for ( unsigned int a = 0 ; a < ex.GetCount() ; a++ )
            out.Write ( ex[a] + "\n" ) ;
*/
        for ( a = 0 ; a < lines.size() ; a++ )
            {
            if ( !lines[a].v ) continue ;
            wxArrayString ex ;
            TVector tmpvec ;
            tmpvec.setFromVector ( *lines[a].v ) ;
            tmpvec.setSequence ( lines[a].s ) ;
            gb.doExport ( &tmpvec , ex ) ;
            for ( b = 0 ; b < ex.GetCount() ; b++ )
                out.Write ( ex[b] + "\n" ) ;
            }

        }
    else if ( filter == 1 ) // Plain text
        {
        for ( a = b = 0 ; a < lines.size() ; a++ )
            if ( lines[a].name.length() > b )
                 b = lines[a].name.length() ;
        for ( a = 0 ; a < lines.size() ; a++ )
            {
            wxString name = lines[a].name ;
            name.Pad ( b - name.length() , ' ' , false ) ;
            out.Write ( name ) ;
            out.Write ( " " ) ;
            out.Write ( lines[a].s ) ;
            out.Write ( "\n" ) ;
            }
        }
    else if ( filter == 2 ) // CSV
        {
        int offset ;
        int inc = wxGetNumberFromUser ( txt("t_number_csv_columns") , "" , "" , 100 , 10 , 200 ) ;
        for ( offset = 0 ; offset < lines[0].s.length() ; offset += inc )
            {
            for ( a = 0 ; a < lines.size() ; a++ )
                {
                out.Write ( "\"" + lines[a].name + "\"" ) ;
                for ( b = 0 ; b < inc && b + offset < lines[a].s.length() ; b++ )
                    {
                    out.Write ( ";" ) ;
                    out.Write ( "\"" + wxString ( lines[a].s.GetChar ( b+offset ) ) + "\"" ) ;
                    }
                out.Write ( "\n" ) ;
                }
            out.Write ( "\n" ) ;
            }
        }
    else if ( filter == 3 || filter == 4 ) // FASTA
        {
        for ( a = 0 ; a < lines.size() ; a++ )
            {
            if ( !lines[a].v ) continue ;
            wxString name = lines[a].name.Upper() ;
            name.Replace ( " " , "_" ) ;
            name = ">" + name.Left ( 59 ) ;
            out.Write ( name + "\n" ) ;
            wxString s = lines[a].s ;
            if ( filter == 4 )
                s.Replace ( "-" , "" ) ;
            while ( !s.IsEmpty() )
                {
                wxString t = s.Left ( 60 ) ;
                s = s.Mid ( 60 ) ;
                out.Write ( t + "\n" ) ;
                }
            }
        }
    else if ( filter == 5 ) // MSF
        {
        int namelimit = 50 ;
        int a , firstseq ;
        for ( firstseq = 0 ; firstseq < lines.size() && !lines[firstseq].v ; firstseq++ ) ;
        int maxlen = lines[firstseq].s.length() ;
        int type = lines[firstseq].v->getType() == TYPE_AMINO_ACIDS ? 'P' : 'N' ;
        int maxname = 0 ;
        for ( a = 0 ; a < lines.size() ; a++ )
            {
            if ( !lines[a].v ) continue ;
            maxname = lines[a].name.length() > maxname ? lines[a].name.length() : maxname ;
            }
        if ( maxname > namelimit ) maxname = namelimit ;

        out.Write ( wxString::Format ( _T("MSF:%d  Type:%c Check:0 ..\n\n") , maxlen , type ) ) ;
        for ( a = 0 ; a < lines.size() ; a++ )
            {
            if ( !lines[a].v ) continue ;
            wxString name = lines[a].name ;
            name.Replace ( " " , "_" ) ;
            name = name.Left ( namelimit ) ;
            name += wxString ( ' ' , maxname - name.length() + 2 ) ;
            wxString x = _T(" Name: ") + name ;
            x += wxString::Format ( _T(" Len: %5d Weight: 1.0 Check: 0\n") , lines[a].s.length() ) ;
            out.Write ( x ) ;
            }
        out.Write ( "\n//\n" ) ;

        int perline = ( 80 - maxname - 1 ) / 11 ;
        for ( int pos = 1 ; pos <= maxlen ; pos += perline * 10 )
            {
            for ( a = 0 ; a < lines.size() ; a++ )
                {
                if ( !lines[a].v ) continue ;
                wxString x = lines[a].name ;
                x.Replace ( " " , "_" ) ;
                x = x.Left ( namelimit ) ;
                x += wxString ( ' ' , maxname - x.length() + 1 ) ;
                for ( int b = 0 ; b < perline ; b++ )
                    {
                    wxString y = lines[a].s.Mid ( pos-1+b*10 , 10 ) ;
                    y.Replace ( "-" , "." ) ;
                    x += y + " " ;
                    }
                out.Write ( x + "\n" ) ;
                }
            out.Write ( "\n\n" ) ;
            }
        }
    out.Close () ;
    }

wxString TAlignment::getExportFilters ()
    {
    wxString wcGenBank = _T("GenBank (*.gb)|*.gb") ;
    wxString wcPlain = _T("Plain text|*.*") ;
    wxString wcCSV = _T("Comma-separated values (CSV)|*.csv") ;
    wxString wcFasta1 = _T("FASTA (with gaps)|*.fasta") ;
    wxString wcFasta2 = _T("FASTA (without gaps)|*.fasta") ;
    wxString wcMsf = _T("GCG alignment format (MSF)|*.msf") ;
    wxString wildcard = wcGenBank + "|" +
                        wcPlain + "|" +
                        wcCSV + "|" +
                        wcFasta1 + "|" +
                        wcFasta2 + + "|" +
                        wcMsf ;
    return wildcard ;
    }


void TAlignment::fromVector ( TVector *nv )
    {
    TGenBank gb ;
    vec = nv ;
    gb.paste ( vec->getSequence() ) ;
    wxString vdesc = vec->getDescription() ;
    wxArrayString vs ;
    explode ( "\n" , vdesc , vs ) ;
    int nol = atoi ( vs[0].mb_str() ) ; // Number of lines
    int n ;
    wxString broken ;
    TManageDatabaseDialog mdb ( this , _T("dummy") , ACTION_MODE_STARTUP ) ;
    lines.clear () ;
    for ( n = 0 ; n < nol ; n++ )
        {
        wxString name = vs[1+n*3] ;
        wxString db = vs[2+n*3] ;
        wxString seq = vs[3+n*3] ;

        TAlignLine line ;
        line.name = name ;

        bool success = false ;
        if ( !db.IsEmpty() ) success = mdb.do_load_DNA ( name , db ) ;
        if ( success ) line.v = mdb.v ;
        else
           {
           gb.vs = gb.vs_l[n] ;
           gb.vi = gb.vi_l[n] ;
           TVector *vv = new TVector ;
           gb.remap ( vv ) ;
           short type = TUReadSeq::getSeqType ( vv->getSequence() ) ;
           if ( type == TYPE_AMINO_ACIDS )
              {
              TAminoAcids *p = myapp()->frame->newAminoAcids ( vv , name ) ;
              delete vv ;
              vv = p->vec ;
              }
           else myapp()->frame->newFromVector ( vv , type ) ;

           if ( !db.IsEmpty() )
              {
              if ( !broken.IsEmpty() ) broken += ", " ;
              broken += vv->getName() ;
              }
           line.v = vv ;
           }
        line.s = seq ;
        lines.push_back ( line ) ;
        }

    name = vec->getName() ;
    database = vec->getDatabase() ;
    vec = NULL ;
    generateConsensusSequence ( true ) ;
    if ( !broken.IsEmpty() )
        {
        wxMessageBox ( wxString::Format(txt("t_align_not_found"),broken.c_str()) ) ;
        }
    redoAlignments ( false ) ;

    // Read markup XML
    int l = 0 ;
    for ( n = nol*3+2 ; n < vs.size() ; n++ )
        {
        wxString t = vs[n].Lower() ;
        if ( t.Left ( 8 ) != "<layout>" ) continue ;

        TiXmlDocument doc ;
        doc.Parse ( t.mb_str() ) ;
        if ( doc.Error() ) continue ;
        if ( !doc.FirstChild ( "layout" ) ) continue ;

        TiXmlNode *n ;
        TiXmlHandle h ( doc.FirstChild ( "layout" ) ) ;
        for ( n = h.FirstChild("scm").Node() ; n ;
            n = n->NextSibling("scm") )
            {
            SequenceCharMarkup scm ;
            getCharMarkup ( scm , l , lines[l].markup.size() , 0 ) ;
            scm.setFromXML ( n ) ;
            lines[l].markup.push_back ( scm ) ;
            }
        l++ ;
        }
    if ( l > 0 ) sc->SilentRefresh () ;
    }


void TAlignment::OnSeqPrint(wxCommandEvent& event)
    {
    sc->OnPrint ( event ) ;
    }

void TAlignment::RunPhylip ( int cmd )
    {
    TPhylip phylip ;
    if ( !phylip.IsOK() ) return ; // Something's wrong

    if ( cmd == PHYLIP_CMD_SETUP )
        {
        phylip.query_phylip_dir ( false ) ;
        return ;
        }

    wxString data ;

    unsigned int a , b ;
    int seqlen = 0 ;
    wxArrayString vs ;
    for ( a = b = 0 ; a < lines.size() ; a++ )
        {
        if ( lines[a].isIdentity ) continue ;
        wxString t ;
        t = wxString::Format ( "%d " , ++b ) ;
        lines[a].phylip_id = t.Trim().Trim(false) ;
        while ( t.length() < 20 ) t += " " ;
        t = t.Mid ( 0 , 20 ) ;
        t += lines[a].s ;
        seqlen = lines[a].s.length() ;
        vs.Add ( t ) ;
        }

    data += wxString::Format ( "%3d%7d\n" , (int) vs.GetCount() , seqlen ) ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        data += vs[a] + "\n" ;

    wxString out ;
    switch ( cmd )
        {
        case PHYLIP_CMD_PROTPARS : out = phylip.protpars ( data ) ; break ;
        case PHYLIP_CMD_PROTDIST : out = phylip.protdist ( data ) ; break ;
        case PHYLIP_CMD_DNAPARS : out = phylip.dnapars ( data ) ; break ;
        case PHYLIP_CMD_PROML : out = phylip.proml ( data ) ; break ;
        case PHYLIP_CMD_PROMLK : out = phylip.promlk ( data ) ; break ;
        case PHYLIP_CMD_SEQBOOT : out = phylip.seqboot ( data ) ; break ;
        case PHYLIP_CMD_CONSENSE : out = phylip.consense ( data ) ; break ;

        case PHYLIP_CMD_DNAMOVE : out = phylip.dnamove ( data ) ; break ;
        case PHYLIP_CMD_DNAPENNY : out = phylip.dnapenny ( data ) ; break ;
        case PHYLIP_CMD_DNACOMP : out = phylip.dnacomp ( data ) ; break ;
        case PHYLIP_CMD_DNAINVAR : out = phylip.dnainvar ( data ) ; break ;
        case PHYLIP_CMD_DNAML : out = phylip.dnaml ( data ) ; break ;
        case PHYLIP_CMD_DNAMLK : out = phylip.dnamlk ( data ) ; break ;
        case PHYLIP_CMD_DNADIST : out = phylip.dnadist ( data ) ; break ;

        case PHYLIP_CMD_RESTML : out = phylip.restml ( data ) ; break ;

        }

    TPhyloTree *tree = myapp()->frame->newPhyloTree () ;
    tree->setNewickTrees ( out , this ) ;
    tree->setModeDrawgram () ;
    }

void TAlignment::getCharMarkup ( SequenceCharMarkup &scm , int vline , int pos , int vfirst )
    {
    if ( vline < 0 || lines[vline].markup.size() <= pos || lines[vline].markup[pos].ignore )
       {
       scm.textcolor = findColors ( lines[vline].s.GetChar(pos) , lines[vfirst].s.GetChar(pos) , true ) ;
       scm.backcolor = findColors ( lines[vline].s.GetChar(pos) , lines[vfirst].s.GetChar(pos) , false ) ;
       }
    else scm = lines[vline].markup[pos] ;
    }








void TAlignment::editAppearance ( int from , int to , int firstline , int lastline )
    {
    AlignmentAppearanceDialog aad ( this , "!!" ) ;
    aad.setup ( from , to , firstline , lastline , this ) ;
    if ( wxID_OK == aad.ShowModal() )
       sc->SilentRefresh() ;
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
    if ( v ) s = v->getSequence() ;
    else s = "" ;
    }

ChildBase *TAlignLine::FindOrigin ()
    {
    unsigned int a ;
    for ( a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
        {
        if ( myapp()->frame->children[a]->vec == v )
           return myapp()->frame->children[a] ;
        }
    return NULL ;
    }

void TAlignLine::showFeatures ()
    {
    if ( features ) delete features ;
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
