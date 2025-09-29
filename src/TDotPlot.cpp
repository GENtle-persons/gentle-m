#include "TDotPlot.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(TDotPlot, MyChildBase)
    EVT_CHAR_HOOK(TDotPlot::OnCharHook)
//  EVT_MENU(SEQ_PRINT, TCalculator::OnSeqPrint)
//  EVT_MENU(MDI_PRINT_REPORT,TCalculator::OnPrintPreview)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_COMMAND_SCROLL(GRAPH_ZOOM_X, TDotPlot::OnZoom)

    EVT_CHOICE(DP_SEQ1,TDotPlot::OnSelectSeq1)
    EVT_CHOICE(DP_SEQ2,TDotPlot::OnSelectSeq2)
    EVT_CHOICE(DP_WINDOW_SIZE,TDotPlot::OnSelectWindowSize)
    EVT_CHOICE(DP_MISMATCH_LIMIT,TDotPlot::OnSelectMismatches)

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
    EVT_MENU(MDI_EXPORT,ChildBase::OnDummy)
    EVT_MENU(MDI_MARK_ALL,ChildBase::OnDummy)
    EVT_MENU(MDI_FIND,ChildBase::OnDummy)
    EVT_MENU(AA_NONE,ChildBase::OnDummy)
    EVT_MENU(AA_KNOWN, ChildBase::OnDummy)
    EVT_MENU(AA_ALL, ChildBase::OnDummy)
    EVT_MENU(AA_THREE, ChildBase::OnDummy)
    EVT_MENU(AA_ONE, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_1, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_2, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_3, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M1, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M2, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M3, ChildBase::OnDummy)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TDotPlotPanel, wxScrolledWindow)
    EVT_PAINT(TDotPlotPanel::OnPaint)
    EVT_MOUSE_EVENTS(TDotPlotPanel::OnMouse)
    EVT_MENU(SAD_OPEN_SEQUENCE_1,TDotPlotPanel::OnOpenSequence1)
    EVT_MENU(SAD_OPEN_SEQUENCE_2,TDotPlotPanel::OnOpenSequence2)
    EVT_MENU(MDI_COPY,TDotPlotPanel::OnCopy)
    EVT_MENU(MDI_FILE_SAVE,TDotPlotPanel::OnSave)
END_EVENT_TABLE()


TDotPlot::TDotPlot(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    def = _T("DOTPLOT") ;
    vec = NULL ;
    allow_save = allow_copy = true ;

    panel = nullptr;
    zoom = nullptr;
    seq1 = seq2 = window_size = mismatch_limit = nullptr;
    open_seq1 = nullptr;
    toolbar = nullptr;

    }

TDotPlot::~TDotPlot ()
    {
    }

void TDotPlot::initme ()
    {
    //wxPrintf("D: TDotPlot::initme - before Menus\n") ;
    // Menus
    wxMenuBar * const menu_bar = new wxMenuBar;
    wxMenu * const file_menu = myapp()->frame->getFileMenu () ;
    wxMenu * const tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu * const help_menu = myapp()->frame->getHelpMenu () ;
    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );
    SetMenuBar(menu_bar);

    //wxPrintf("D: TDotPlot::initme - before Toolbar\n") ;

    // Toolbar
    wxToolBar * const toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    // toolBar->Reparent ( this ) ; // not needed!
    toolbar = toolBar ;
    myapp()->frame->InitToolBar(toolBar);
/*
    zoom = new wxSlider ( toolBar , GRAPH_ZOOM_X , 1 , 1 , 91 ,
                            wxDefaultPosition ,
                            wxDefaultSize ,
                            wxSL_HORIZONTAL ) ;
*/
    seq1 = new wxChoice ( toolBar , DP_SEQ1 ) ;
    seq2 = new wxChoice ( toolBar , DP_SEQ2 ) ;
    window_size = new wxChoice ( toolBar , DP_WINDOW_SIZE , wxDefaultPosition , wxSize ( 40 , -1 ) ) ;
    mismatch_limit = new wxChoice ( toolBar , DP_MISMATCH_LIMIT , wxDefaultPosition , wxSize ( 40 , -1 ) ) ;
    open_seq1 = new wxCheckBox ( toolBar , DP_OPEN_SEQ1 , txt("t_open_seq1") ) ;

    //wxPrintf("D: TDotPlot::initme - before addTool\n") ;

    myapp()->frame->addTool ( toolBar , MDI_TEXT_IMPORT ) ;
    myapp()->frame->addTool ( toolBar , MDI_FILE_OPEN ) ;
//  toolBar->AddControl ( new wxStaticText ( toolBar , -1 , _T("Zoom") ) ) ;
//  toolBar->AddControl ( zoom ) ;
//  toolBar->AddSeparator() ;

    //wxPrintf("D: TDotPlot::initme - before add controls\n") ;
//  toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_sequence1") ) ) ;
    toolBar->AddControl ( seq1 ) ;
//  toolBar->AddSeparator() ;
//  toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_sequence2") ) ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , _T("X") ) ) ;
    toolBar->AddControl ( seq2 ) ;
    toolBar->AddSeparator() ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_window_size") ) ) ;
    toolBar->AddControl ( window_size ) ;
    toolBar->AddSeparator() ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_mismatch_limit") ) ) ;
    toolBar->AddControl ( mismatch_limit ) ;
//  toolBar->AddSeparator() ;
    toolBar->AddControl ( open_seq1 ) ;

    //wxPrintf("D: TDotPlot::initme - before add default tools\n") ;
    myapp()->frame->addDefaultTools ( toolBar ) ;
    toolBar->Realize() ;

    //wxPrintf("D: TDotPlot::initme - before new TDotPlotPanel\n") ;
    // Misc
    panel = new TDotPlotPanel ( this ) ;
    for ( int a = 1 ; a < 20 ; a += 2 )
        {
        window_size->Append ( wxString::Format ( _T("%d") , a ) ) ;
        }
    for ( int a = 0 ; a < 10 ; a++ )
        {
        mismatch_limit->Append ( wxString::Format ( _T("%d") , a ) ) ;
        }
    window_size->SetStringSelection ( _T("9") ) ;
    mismatch_limit->SetStringSelection ( _T("2") ) ;

    //wxPrintf("D: TDotPlot::initme - before new wxBoxSizer\n") ;
    // Layout
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( panel , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

    //wxPrintf("D: TDotPlot::initme - before new setChild\n") ;
    // Finalize
    if (myapp() && myapp()->frame)
        {
        //wxPrintf("D: TDotPlot::initme - before new setChild with myapp and frame ok\n") ;
        myapp()->frame->setChild(this);
        }
        else
        {
        wxPrintf("Error: myapp() or myapp()->frame is not initialized.\n");
        }
    Maximize();
    //wxPrintf("D: TDotPlot::initme - before new Activate\n") ;
    Activate () ;
    //wxPrintf("D: TDotPlot::initme - before new update_sequence_lists\n") ;
    update_sequence_lists() ;
    //wxPrintf("D: TDotPlot::initme - end\n") ;
    }

wxString TDotPlot::getName () const
    {
    return txt("t_dotplot") ;
    }

void TDotPlot::update_sequence_lists ()
    {
    //wxPrintf("D: TDotPlot::update_sequence_lists - start\n") ;
    wxASSERT( seq1 ) ;
    wxASSERT( seq2 ) ;
    wxString s1 = seq1->GetStringSelection() ;
    wxString s2 = seq2->GetStringSelection() ;
    seq1->Clear() ;
    seq2->Clear() ;
    //wxPrintf("D: TDotPlot::update_sequence_lists - before loop with %d children\n", myapp()->frame->children.GetCount()) ;
    for ( int a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
        {
        if ( myapp()->frame->children[a]->def != _T("dna") ) continue ;
        wxPrintf("D: TDotPlot::update_sequence_lists - found child %d: %s\n", a, myapp()->frame->children[a]->getName());
        seq1->Append ( myapp()->frame->children[a]->getName() ) ;
        seq2->Append ( myapp()->frame->children[a]->getName() ) ;
        }
    if ( !s1.IsEmpty() )
        {
        //wxPrintf("D: TDotPlot::update_sequence_lists - restoring seq1 selection to %s\n", s1);
        seq1->SetStringSelection ( s1 ) ;
        }
    if ( !s2.IsEmpty() )
        {
        //wxPrintf("D: TDotPlot::update_sequence_lists - restoring seq2 selection to %s\n", s2);
        seq2->SetStringSelection ( s2 ) ;
        }   
//   if ( seq1->GetSelection() == wxNOT_FOUND && seq1->GetCount() > 0 ) seq1->SetSelection ( 0 ) ;
//   if ( seq2->GetSelection() == wxNOT_FOUND && seq2->GetCount() > 0 ) seq2->SetSelection ( 0 ) ;

    wxASSERT(panel) ;
    //wxPrintf("D: TDotPlot::update_sequence_lists - panel->Run\n") ;
    panel->Run () ;
    //wxPrintf("D: TDotPlot::update_sequence_lists - end\n") ;
    }

void TDotPlot::OnZoom(wxScrollEvent& event)
    {
    }


void TDotPlot::OnCharHook(wxKeyEvent& event)
    {
    }

void TDotPlot::OnSelectSeq1(wxCommandEvent& event)
    {
    panel->Run() ;
    }

void TDotPlot::OnSelectSeq2(wxCommandEvent& event)
    {
    panel->Run() ;
    }

void TDotPlot::OnSelectWindowSize(wxCommandEvent& event)
    {
    panel->Run() ;
    }

void TDotPlot::OnSelectMismatches(wxCommandEvent& event)
    {
    panel->Run() ;
    }


//__________________________________________________________________

TDotPlotPanel::TDotPlotPanel ( wxWindow *_parent )
: wxScrolledWindow ( _parent , -1 , wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxFULL_REPAINT_ON_RESIZE | wxALWAYS_SHOW_SB)
    {
    parent = (TDotPlot*) _parent ;
    offx = 40 ;
    offy = 40 ;
    seq1 = seq2 = nullptr ;
    }

void TDotPlotPanel::Run ()
    {
    //wxPrintf("D: TDotPlotPanel::Run - start\n") ;
    seq1 = seq2 = nullptr ;
    len1 = len2 = 0UL ;
    wxASSERT(parent) ;
    wxASSERT(parent->window_size) ;
    parent->window_size->GetStringSelection().ToLong ( &window ) ;
//  wxMessageBox ( parent->window_size->GetStringSelection() ) ; ;
//  return ;
    wxASSERT(parent->mismatch_limit) ;
    parent->mismatch_limit->GetStringSelection().ToLong ( &mismatch ) ;

    for ( unsigned int a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
        {
        if ( myapp()->frame->children[a]->def != _T("dna") ) continue ;
        wxString name = myapp()->frame->children[a]->getName() ;
        if ( !seq1 && parent->seq1->GetStringSelection() == name ) seq1 = (MyChild*) myapp()->frame->children[a] ;
        if ( !seq2 && parent->seq2->GetStringSelection() == name ) seq2 = (MyChild*) myapp()->frame->children[a] ;
        }

    if ( !seq1 || !seq2 )
        {
        wxPrintf("D: TDotPlotPanel::Run - one or both sequences not selected\n") ;
        }
    Recalc() ;
    Update() ;
    wxMouseEvent ev ;
    OnMouse ( ev ) ;
    //wxPrintf("D: TDotPlotPanel::Run - end\n") ;
    }

void TDotPlotPanel::Update ()
    {
    //wxPrintf("D: TDotPlotPanel::Update - start\n") ;
    GetViewStart ( &x1 , &y1 ) ;
    wxClientDC dc ( this ) ;
    wxBufferedDC dcb ( &dc , dc.GetSize() ) ;
    Redraw ( dcb ) ;
    //wxPrintf("D: TDotPlotPanel::Update - end\n") ;
    }

#define dppos(X,Y) (Y*len1+X)

void TDotPlotPanel::Recalc ()
    {
    //wxPrintf("D: TDotPlotPanel::Recalc - start\n") ;
    if ( !seq1 || !seq2 )
        {
        wxPrintf("D: TDotPlotPanel::Recalc - no sequences, returning\n") ;
        return ;
        }
    wxBeginBusyCursor() ;
    wxASSERT( seq1->vec ) ;
    wxASSERT( seq2->vec ) ;
    len1 = seq1->vec->getSequenceLength() ;
    len2 = seq2->vec->getSequenceLength() ;
    wxPrintf("D: TDotPlotPanel::Recalc - seq1 len: %ld, seq2 len: %ld, len1*len2: %ld\n" , len1 , len2 , len1*len2 ) ;
    if ( len1 * len2 > 1024UL * 1024UL * 100UL ) // testing if size > 100 MB
        {
        wxPrintf("D: TDotPlotPanel::Recalc - dotplot too large at %lu MB, limit at 50MB,returning\n", len1 * len2 / (1024UL * 1024UL)) ;
        wxEndBusyCursor() ;
        //wxPrintf("D: TDotPlotPanel::Recalc - end - early\n") ;
        return ;
        }
    data = vector <char> ( len1 * len2 + 1 ) ;
    //wxPrintf("D: TDotPlotPanel::Recalc - data size: %d\n", data.size() ) ;

    for ( unsigned long a = 0 ; a < len1 ; a++ )
        {
        for ( unsigned long b = 0 ; b < len2 ; b++ )
            {
//          bool ident = seq1->vec->getSequenceChar(a) == seq2->vec->getSequenceChar(b) ;
            data[dppos(a,b)] = CheckWindow ( a , b ) ;
            }
       }

    x1 = y1 = 0 ;
    //wxPrintf("D: TDotPlotPanel::Recalc - setting virtual size to %d x %d\n" , offx + len1 , offy + len2 ) ;
    SetVirtualSize ( offx + len1 , offy + len2 ) ;
    SetScrollRate ( 1 , 1 ) ;
    Draw2Memory () ;
//  SetScrollbars ( 1 , 1 , len1 , len2 ) ;
    wxEndBusyCursor() ;
    //wxPrintf("D: TDotPlotPanel::Recalc - end\n") ;
    }

int TDotPlotPanel::CheckWindow ( int x , int y )
    {
    int  match = 0 ;
    for ( int a = 0 ; a < window ; a++ )
        {
        if ( x+a >= len1 ) break ;
        if ( y+a >= len2 ) break ;
        if ( seq1->vec->getSequenceChar(a+x) == seq2->vec->getSequenceChar(a+y) ) match++ ;
        }
    if ( match + mismatch < window ) return false ;
    return true ;
    }

void TDotPlotPanel::OnPaint ( wxPaintEvent &event )
    {
    wxPaintDC dc(this);
    GetViewStart ( &x1 , &y1 ) ;
    Redraw ( dc ) ;
    }

void TDotPlotPanel::OnDraw(wxDC& dc)
    {
//  Redraw ( dc ) ;
    }

void TDotPlotPanel::Redraw ( wxDC &dc )
    {
    if ( !seq1 || !seq2 )
        {
        dc.Clear () ;
        return ;
        }

    wxCoord w , h ;
    dc.GetSize ( &w , &h ) ;
    w += x1 ;
    h += y1 ;
    dc.SetBackground ( *wxWHITE_BRUSH ) ;
    dc.Clear () ;
    dc.Blit(-x1, -y1, w, h, &memdc, 0, 0);
    }

void TDotPlotPanel::Draw2Memory ()
    {
    if ( !seq1 || !seq2 ) return ;
    membmp = wxBitmap ( offx+len1 , offy+len2 , 1 ) ;
    memdc.SelectObject ( membmp ) ;
    memdc.SetBackground ( *wxWHITE_BRUSH ) ;
    memdc.Clear() ;
    memdc.SetPen ( *wxBLACK_PEN ) ;
    wxFont font ( wxFontInfo ( 10 ).Family( wxFONTFAMILY_SWISS ).Style( wxFONTSTYLE_NORMAL ).Weight( wxFONTWEIGHT_NORMAL ) ) ;
    memdc.SetFont ( font ) ;

    for ( int x = 0 ; x < len1 ; x++ )
        {
        for ( int y = 0 ; y < len2 ; y++ )
             {
             if ( data[dppos(x,y)] ) memdc.DrawPoint ( x+offx , y+offy ) ;
             }
        }

    // Scales
    int step = 50 ;
    wxCoord w , h ;
    memdc.SetPen ( *wxBLACK_DASHED_PEN ) ;
    for ( int x = step-1 ; x < len1 ; x += step )
        {
        wxString s = wxString::Format ( _T("%d") , x+1 ) ;
        memdc.GetTextExtent ( s , &w , &h ) ;
        memdc.DrawText ( s , offx + x - w/2 , 2 ) ;
        memdc.DrawLine ( x + offx , 2 + h , x + offx , offy - 1 ) ;
        }

    for ( int y = step-1 ; y < len2 ; y += step )
        {
        wxString s = wxString::Format ( _T("%d") , y+1 ) ;
        memdc.GetTextExtent ( s , &w , &h ) ;
        memdc.DrawText ( s , 2 , offy + y - h/2 ) ;
        memdc.DrawLine ( 2 + w , y + offy , offx - 1 , y + offy ) ;
        }

    memdc.DrawText ( seq1->getName() , offx , offy/2 ) ;
    memdc.GetTextExtent ( seq2->getName() , &w , &h ) ;
    memdc.DrawRotatedText ( seq2->getName() , offx/2 , offy + w , 90 ) ;
    }

void TDotPlotPanel::OnMouse(wxMouseEvent& event)
    {
    if ( !seq1 || !seq2 ) return ;

    // Determine logical position
    wxPoint pt = event.GetPosition() ;
    int x = event.GetX() - offx + x1 ;
    int y = event.GetY() - offy + y1 ;
    if ( x < 0 ) x = 0 ;
    if ( x >= seq1->vec->getSequenceLength() ) x = seq1->vec->getSequenceLength() - 1 ;
    if ( y < 0 ) y = 0 ;
    if ( y >= seq2->vec->getSequenceLength() ) y = seq2->vec->getSequenceLength() - 1 ;
    last_mouse_logical_position = wxPoint ( x , y ) ;

    // Show positions in status text
    wxString pos_text = wxString::Format ( txt("t_dotplot_position") , x+1 , y+1 ) ;
    myapp()->frame->SetStatusText ( pos_text , 0 ) ;
    wxString nearby ;
    nearby = seq1->getName() + _T(" : " ) ;
    nearby += seq1->vec->getSequence().Mid(x-5,9) ;
    nearby += _T(" ; ") ;
    nearby += seq2->getName() + _T(" : " ) ;
    nearby += seq2->vec->getSequence().Mid(y-5,9) ;
    myapp()->frame->SetStatusText ( nearby , 1 ) ;

    // Double-click?
    if ( event.LeftDClick() )
        {
        wxCommandEvent ev ;
        if ( !parent->open_seq1->GetValue() ) OnOpenSequence1 ( ev ) ;
        else OnOpenSequence2 ( ev ) ;
        return ;
        }

    // Context menu
    if ( event.RightIsDown() )
        {
        wxMenu * const cm = new wxMenu ;
        cm->Append ( SAD_OPEN_SEQUENCE_1 , txt("m_sad_open_sequence_1") ) ;
        cm->Append ( SAD_OPEN_SEQUENCE_2 , txt("m_sad_open_sequence_2") ) ;
        cm->AppendSeparator () ;
        cm->Append ( MDI_COPY , txt("m_copy_image") ) ;
        cm->Append ( MDI_FILE_SAVE , txt("t_save_image") ) ;

        PopupMenu ( cm , pt ) ;
        }
    }

void TDotPlotPanel::OnOpenSequence1(wxCommandEvent& event)
    {
    myapp()->frame->mainTree->SelectItem ( seq1->inMainTree ) ;
    const int pos = last_mouse_logical_position.x ;
    seq1->cSequence->mark ( _T("DNA") , pos , pos , 1 ) ;
    seq1->cSequence->ensureVisible ( pos ) ;
    }

void TDotPlotPanel::OnOpenSequence2(wxCommandEvent& event)
    {
    myapp()->frame->mainTree->SelectItem ( seq2->inMainTree ) ;
    int pos = last_mouse_logical_position.y ;
    seq2->cSequence->mark ( _T("DNA") , pos , pos , 1 ) ;
    seq2->cSequence->ensureVisible ( pos ) ;
    }

void TDotPlotPanel::OnCopy(wxCommandEvent& event)
    {
    wxBitmap *bmp = new wxBitmap ( membmp ) ;//getSequenceBitmap () ;

    // Copy to clipboard
    if (wxTheClipboard->Open())
        {
        wxBitmapDataObject *bdo = new wxBitmapDataObject ( *bmp ) ;
        wxTheClipboard->SetData( bdo ) ;
        wxTheClipboard->Close();
        }
    }

void TDotPlotPanel::OnSave(wxCommandEvent& event)
    {
    const wxBitmap bmp(membmp); // Ensure const correctness
    myapp()->frame->saveImage(&bmp, txt("t_dotplot"));
    }
