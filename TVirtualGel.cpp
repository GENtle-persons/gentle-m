#include "TVirtualGel.h"
#include <wx/dir.h>
#include <wx/clipbrd.h>
#include <wx/printdlg.h>

#define LOW_LIMIT 20

BEGIN_EVENT_TABLE(TVirtualGel, MyChildBase)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)

    EVT_CHOICE(VG_PERCENT,TVirtualGel::OnPercent)
    EVT_CHOICE(VG_MARKER,TVirtualGel::OnMarker)
    EVT_CHECKBOX(VG_LABEL,TVirtualGel::OnLabel)
    EVT_MENU(SEQ_PRINT, TVirtualGel::OnPrint)

    // Dummies
    EVT_MENU(MDI_TOGGLE_FEATURES,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_UNDO,ChildBase::OnDummy)
    EVT_MENU(MDI_CUT,ChildBase::OnDummy)
    EVT_MENU(MDI_COPY,ChildBase::OnDummy)
    EVT_MENU(MDI_PASTE,ChildBase::OnDummy)
    EVT_MENU(MDI_EDIT_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_EXPORT,ChildBase::OnDummy)
    EVT_MENU(MDI_MARK_ALL,ChildBase::OnDummy)
    EVT_MENU(MDI_FILE_SAVE,ChildBase::OnDummy)
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

BEGIN_EVENT_TABLE(TMyGelControl, wxControl)
    EVT_PAINT(TMyGelControl::OnPaint)

    EVT_MOUSE_EVENTS(TMyGelControl::OnEvent)
    EVT_MENU(IV_MENU_SAVE_AS_BITMAP, TMyGelControl::OnSaveAsBitmap)
    EVT_MENU(IV_MENU_COPY, TMyGelControl::OnCopy)
    EVT_MENU(IV_MENU_PRINT, TMyGelControl::OnPrint)
END_EVENT_TABLE()


TVirtualGel::TVirtualGel(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    def = _T("GEL") ;
    vec = NULL ;
    percent = 1 ;
    cb_label = NULL ;
    ch_percent = NULL ;
    allow_print = true ;
    }

void TVirtualGel::initme ()
    {
    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    SetMenuBar(menu_bar);

    right = new TMyGelControl ( this , IV_IMAGE ) ;
    right->vg = this ;

    wxToolBar *toolBar = CreateToolBar(wxTB_HORIZONTAL);

    //wxBoxSizer *hs = new wxBoxSizer ( wxHORIZONTAL ) ;
    ch_percent = new wxChoice ( toolBar , VG_PERCENT , wxDefaultPosition , wxSize ( 80 , -1 ) ) ;
    ch_marker = new wxChoice ( toolBar , VG_MARKER , wxDefaultPosition , wxSize ( 250 , -1 ) ) ;
    cb_label = new wxCheckBox ( toolBar , VG_LABEL , txt("t_vg_show_label") ) ;
    cb_label->SetValue ( true ) ;


    myapp()->frame->InitToolBar(toolBar);
    myapp()->frame->addTool ( toolBar , MDI_TEXT_IMPORT ) ;
    myapp()->frame->addTool ( toolBar , MDI_FILE_OPEN ) ;
//  myapp()->frame->addTool ( toolBar , MDI_FILE_SAVE ) ;
    myapp()->frame->addTool ( toolBar , SEQ_PRINT ) ;

    toolBar->AddControl ( cb_label ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_vg_concentration") ) ) ;
    toolBar->AddControl ( ch_percent ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_vg_marker") ) ) ;
    toolBar->AddControl ( ch_marker ) ;

    myapp()->frame->addDefaultTools ( toolBar ) ;
    toolBar->Realize() ;

    toolbar = toolBar ;


/*
    hs->Add ( cb_label , 0 , wxEXPAND , 5 ) ;
    hs->Add ( new wxStaticText ( this , -1 , txt("t_vg_concentration") ) , 0 , wxEXPAND , 5 ) ;
    hs->Add ( ch_percent , 0 , wxEXPAND , 5 ) ;
    hs->Add ( new wxStaticText ( this , -1 , txt("t_vg_marker") ) , 0 , wxEXPAND , 5 ) ;
    hs->Add ( ch_marker , 0 , wxEXPAND , 5 ) ;
*/

    wxBoxSizer* vs = new wxBoxSizer ( wxVERTICAL ) ;
    vs->Add ( toolBar , 0 , wxEXPAND , 5 ) ;
    vs->Add ( right , 1 , wxEXPAND , 5 ) ;

    myapp()->frame->setChild ( this ) ;

    if ( type == _T("DNA") )
        {
        for ( int a = 3 ; a <= 30 ; a++ )
            ch_percent->Append ( wxString::Format ( _T("%1.1f %%") , ((float)a)/10.0 ) ) ;
        ch_percent->SetStringSelection ( _T("1.0 %") ) ;

        for ( int a = 0 ; a < myapp()->frame->dna_marker.GetCount() ; a++ )
            {
            ch_marker->Append ( myapp()->frame->dna_marker[a].BeforeFirst(':') ) ;
            }


        wxString mbf = _T("??") ;
        if ( myapp()->frame && ( myapp()->frame->dna_marker.size() > 0 ) )
            {
            mbf = myapp()->frame->dna_marker[0].BeforeFirst(':') ;
            }
        ch_marker->SetStringSelection ( myapp()->frame->LS->getOption ( _T("LASTDNAMARKER") , mbf ) ) ;

        lanes.push_back ( TGelLane() ) ;
        lanes[0].setMarker ( ch_marker->GetStringSelection() ) ;
        }
	
    SetSizer(vs);
//      SetSizerAndFit(vs);
    Show () ;
    Activate () ;

    }

wxString TVirtualGel::getName ()
    {
    return _T("Virtual Gel") ;
    }

void TVirtualGel::OnPercent ( wxCommandEvent &ev )
    {
    Refresh () ;
    }

void TVirtualGel::OnPrint ( wxCommandEvent &ev )
    {
    if ( right ) right->OnPrint ( ev ) ;
    }

void TVirtualGel::OnMarker ( wxCommandEvent &ev )
    {
    lanes[0].setMarker ( ch_marker->GetStringSelection() ) ;
    myapp()->frame->LS->setOption ( _T("LASTDNAMARKER") , ch_marker->GetStringSelection() ) ;
    Refresh () ;
    }

void TVirtualGel::OnLabel ( wxCommandEvent &ev )
    {
    Refresh () ;
    }


// ****************************************************************

TMyGelControl::TMyGelControl ( wxWindow *parent , int id )
    : wxControl ( parent , id )
    {
    printing = false ;
    }

void TMyGelControl::OnDraw(wxDC& dc)
    {
    if ( !vg->cb_label ) return ; //Pre-drawing
    if ( vg->lanes.size() == 0 ) return ; // no lanes, nothing to do

    // Initialize
    vg->ch_percent->GetStringSelection().ToDouble ( &vg->percent ) ;
//  vg->percent /= 10.0 ;

    int a , b ;
    int w , h ;
    dc.GetSize ( &w , &h ) ;
    int lw ; // Lane width
    int go ; // gel offset (y)

    if ( printing )
    	{
	w = w > h ? h : w ; // smallest side
	w /= 2 ; // half of it
	h = w ;
	lw = w / ( vg->lanes.size() > 6 ? vg->lanes.size() : 6 ) ; // min 6 lanes
	dc.SetDeviceOrigin ( w / 2 , h / 2 ) ; // center it
	go = h / 10 ;
    	}
    else
    	{
	lw = w / vg->lanes.size() ;
	if ( lw > 100 ) lw = 100 ;
	go = 50 ;
    	}

    int x = lw / 10 ;

    // Calculating presets
    vg->maxband = 0 ;
    vg->cutoff = 10000000 ;
    for ( a = 0 ; a < vg->lanes.size() ; a++ )
    	{
	for ( b = 0 ; b < vg->lanes[a].vi.GetCount() ; b++ )
            {
            if ( vg->lanes[a].vi[b] < LOW_LIMIT ) continue ; // HARD LIMIT
            if ( vg->lanes[a].vi[b] > vg->maxband )
                 vg->maxband = vg->lanes[a].vi[b] ;
            if ( vg->lanes[a].vi[b] < vg->cutoff )
                 vg->cutoff = vg->lanes[a].vi[b] ;
            }
	wxRect r ( x+go/10 , go , lw-go*2/10 , h - go*11/10 ) ;
	vg->lanes[a].pos = r ;
	x += lw ;
     	}

    vg->cutoff = vg->cutoff * 8 / 10 ;

   	// Drawing gel
    int fontfactor = 10 ;
    if ( printing ) fontfactor = w/5000 ;
    wxFont *normalFont = MYFONT ( fontfactor * 11 / 10 , wxFONTFAMILY_SWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ;
    wxFont *bigFont = MYFONT ( fontfactor * 14 / 10 , wxFONTFAMILY_SWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ;

    int tw , th ;
    wxString title = _T("t_gelname_") + vg->type ;
    title = wxString::Format ( txt(title) , vg->percent ) ;
    dc.SetTextForeground ( *wxBLACK ) ;
    dc.SetFont ( *bigFont ) ;
    dc.GetTextExtent ( title , &tw , &th ) ;
    dc.DrawText ( title , ( w - tw ) / 2 , 2 ) ;

    dc.SetPen ( *wxBLACK_PEN ) ;
    dc.SetBrush ( *wxWHITE_BRUSH ) ;
    dc.DrawRectangle ( vg->lanes[0].pos.GetLeft() ,
                       vg->lanes[0].pos.GetTop() ,
                       vg->lanes[vg->lanes.size()-1].pos.GetRight() - vg->lanes[0].pos.GetLeft() ,
                       vg->lanes[vg->lanes.size()-1].pos.GetBottom() - vg->lanes[0].pos.GetTop() ) ;

    // Drawing lanes
    for ( a = 0 ; a < vg->lanes.size() ; a++ )
    	{
        title = vg->lanes[a].name ;
        dc.SetTextForeground ( *wxBLACK ) ;
        dc.SetFont ( *normalFont ) ;

        int n = title.Freq ( ' ' ) + 1 ;
        if ( n > 3 ) n = 3 ; // Three lines max
        if ( n > 2 )
            {
            for ( int m = 0 ; m < n ; m++ )
                {
                wxString t = title.BeforeFirst ( ' ' ) ;
                title = title.AfterFirst ( ' ' ) ;
                dc.GetTextExtent ( t , &tw , &th ) ;
                dc.DrawText ( t ,
                              ( vg->lanes[a].pos.GetLeft() + vg->lanes[a].pos.GetRight() - tw ) / 2 ,
                              vg->lanes[a].pos.GetTop() - ( n - m ) * th ) ;
                }
            }
        else
            {
            dc.GetTextExtent ( title , &tw , &th ) ;
            dc.DrawText ( title ,
                          ( vg->lanes[a].pos.GetLeft() + vg->lanes[a].pos.GetRight() - tw ) / 2 ,
                          vg->lanes[a].pos.GetTop() - th * 3 / 2 ) ;
            }

        for ( b = 0 ; b < vg->lanes[a].vi.GetCount() ; b++ )
            drawBand ( dc , vg->lanes[a] , b ) ;

    	}
    }

void TMyGelControl::drawBand ( wxDC &dc , TGelLane &lane , int band )
    {
    if ( lane.vi[band] < LOW_LIMIT ) return ; // HARD LIMIT; don't draw anything below LOW_LIMIT
    int h = lane.pos.GetHeight() ;
    int y = getLanePos ( lane.vi[band] , h ) + lane.pos.GetTop() ;

    double w = lane.vw[band] ;
    w /= lane.unit_volume ;
    w /= 2 ;
    if ( w < 1 ) w = 1 ;

    for ( int b = 0 ; b < w ; b++ )
        {
        int i = (int) (b * 255 / ( w + 1 )) ;// b * 30 ;
        if ( i > 255 ) continue ;
        int c = 5 + b * b ;
        dc.SetPen ( *MYPEN ( wxColour ( i , i , i ) ) ) ;
        dc.DrawLine ( lane.pos.GetLeft() + c , y + b ,
                      lane.pos.GetRight() - c , y + b ) ;
        dc.DrawLine ( lane.pos.GetLeft() + c , y - b ,
                      lane.pos.GetRight() - c, y - b ) ;
        }

    // Label
    if ( !vg->cb_label->GetValue() ) return ; // Don't show labels
    wxString title = wxString::Format ( _T("%d") , lane.vi[band] ) ;
    if ( lane.vs[band] != _T("") ) title = lane.vs[band] ;

    int tw , th ;
    dc.SetTextForeground ( *wxBLUE ) ;

    int ps = 1000 ;
    int bw = lane.pos.GetRight() - lane.pos.GetLeft() ;
    dc.SetFont ( *MYFONT ( ps , wxFONTFAMILY_ROMAN , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;
    dc.GetTextExtent ( title , &tw , &th ) ;
    ps = ps * bw / tw ;
    dc.SetFont ( *MYFONT ( ps , wxFONTFAMILY_ROMAN , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;
    dc.GetTextExtent ( title , &tw , &th ) ;

    if ( th > bw / 5 )
       {
       ps = ps * ( bw / 5 ) / th ;
       dc.SetFont ( *MYFONT ( ps , wxFONTFAMILY_ROMAN , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;
       dc.GetTextExtent ( title , &tw , &th ) ;
       }

    tw = lane.pos.GetRight() - tw ;
    dc.DrawText ( title , tw , y ) ;
    }

int TMyGelControl::getLanePos ( int size , int height , float perc )
	{
	int border = height / 100 ;
	double ret = height - border * 2 ;
	ret *= fix_percent ( size ) - fix_percent ( vg->maxband ) ;
	ret /= fix_percent ( vg->cutoff ) - fix_percent ( vg->maxband ) ;
	return (int) ret + border ;
	}

double TMyGelControl::fix_percent ( int size , float perc )
    {
    if ( perc == 0 ) perc = vg->percent ;
    double ret = size ;
    ret = 400 / log10 ( ret / perc ) ;
/*	double a = -0.000235 ;
    double b = 1.8564 ;
    ret = pow ( 10 , a * ret + b ) * 10 ;*/
    return ret ;
    }

void TMyGelControl::OnPaint(wxPaintEvent& event)
    {
    wxPaintDC dc(this);
    OnDraw ( dc ) ;
    }

void TMyGelControl::OnEvent(wxMouseEvent& event)
    {
    wxPoint pt(event.GetPosition());

    if ( event.LeftDClick() )
        {
        wxPoint p = event.GetPosition() ;
        int a ;
        for ( a = 0 ; a < vg->lanes.size() ; a++ )
            {
            if ( vg->lanes[a].pos.GetLeft() > p.x ) continue ;
            if ( vg->lanes[a].pos.GetRight() < p.x ) continue ;
            break ;
            }
        if ( a == vg->lanes.size() ) return ;
//        if ( vg->lanes[a].type != "DNA" ) return ;
        wxString s = wxGetTextFromUser ( txt("t_vg_edit_name1") , txt("t_vg_edit_name2") , vg->lanes[a].name ) ;
        if ( s.IsEmpty() ) return ;
        vg->lanes[a].name = s ;
        vg->Refresh () ;
        }
    else if ( event.RightDown() )
        {
        wxMenu *cm = new wxMenu ;
        cm->Append ( IV_MENU_SAVE_AS_BITMAP , txt("m_save_as_bitmap") ) ;
        cm->Append ( IV_MENU_COPY , txt("m_copy_to_clipboard") ) ;
        cm->Append ( IV_MENU_PRINT , txt("m_print") ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;
        }
    }

void TMyGelControl::OnSaveAsBitmap(wxCommandEvent &event)
    {
    int w = 1024 ;
    int h = 768 ;
    wxBitmap bmp ( w , h , -1 ) ;
    wxMemoryDC dc ;
    dc.SelectObject ( bmp ) ;
    dc.Clear() ;
    OnDraw ( dc ) ;
    wxString title = _T("t_gelname_") + vg->type ;
    title = wxString::Format ( txt(title) , vg->percent ) ;
    myapp()->frame->saveImage ( &bmp , title ) ;
    }

void TMyGelControl::OnCopy(wxCommandEvent &event)
    {
    int w = 1024 ;
    int h = 768 ;
    wxBitmap bmp ( w , h , -1 ) ;
    wxMemoryDC dc ;
    dc.SelectObject ( bmp ) ;
    dc.Clear() ;
    OnDraw ( dc ) ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxBitmapDataObject ( bmp ) );
        wxTheClipboard->Close();
        }
    }

void TMyGelControl::OnPrint(wxCommandEvent &event)
    {
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;

    wxDC *dc = pd.GetPrintDC () ;
    dc->StartDoc ( vg->getName() ) ;
    dc->StartPage () ;
    printing = true ;
    OnDraw ( *dc ) ;
    printing = false ;
    dc->EndPage () ;
    dc->EndDoc () ;
    }

// *************************************************

TGelLane::TGelLane ()
    {
    unit_volume = 20.0 ;
    }

void TGelLane::clear ()
    {
    name = type = _T("") ;
    vi.Clear () ;
    vw.Clear () ;
    vs.Clear () ;
    unit_volume = 20.0 ; // l
    }

void TGelLane::setMarker ( wxString _name )
    {
    int a ;
    for ( a = 0 ; a < myapp()->frame->dna_marker.GetCount() ; a++ )
        {
        if ( myapp()->frame->dna_marker[a].BeforeFirst(':') == _name )
			break ;
        }
    if ( a == myapp()->frame->dna_marker.GetCount() ) return ; // Invalid marker name

    clear () ;
    name = _name ;
    wxString s = myapp()->frame->dna_marker[a] ;
    a = 0 ;
    while ( !s.IsEmpty() )
        {
        wxString t = s.BeforeFirst ( ',' ) ;
        s = s.AfterFirst ( ',' ) ;
        long l1 , l2 ;
        double d1 ;
        t.BeforeFirst(':').ToLong ( &l1 );
        t.AfterFirst(':').ToLong ( &l2 ) ;
        if ( a++ == 0 )
			{
			t.BeforeFirst(':').ToDouble ( &d1 );
			unit_volume = 20 ;
//			unit_volume = d1 ;
			}
        else add ( l1 , l2 ) ;
        }
    }

void TGelLane::add ( int size , int weight , wxString title )
    {
    vi.Add ( size ) ;
    vw.Add ( weight ) ;
    vs.Add ( title ) ;
    }

void TGelLane::add ( int size , int weight )
    {
    add ( size , weight , _T("") ) ;
    }

void TGelLane::add ( int size , wxString title )
    {
    add ( size , 1 , title ) ;
    }

