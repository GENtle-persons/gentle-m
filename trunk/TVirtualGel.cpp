#include "TVirtualGel.h"
#include <wx/dir.h>
#include <wx/clipbrd.h>
#include <wx/printdlg.h>

BEGIN_EVENT_TABLE(TVirtualGel, MyChildBase)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_SIZE(TVirtualGel::OnSize)

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
    def = "GEL" ;
    vec = NULL ;
    lanes.push_back ( TGelLane() ) ;
    lanes.push_back ( TGelLane() ) ;
    lanes.push_back ( TGelLane() ) ;
    lanes[0].setMarker ( "Mass Ruler" ) ;
    lanes[1].setMarker ( "Mass Ruler" ) ;
    lanes[2].setMarker ( "Mass Ruler" ) ;
    cutoff = 50 ;
    }

void TVirtualGel::initme ()
    {
    percent = 1 ;

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

    wxBoxSizer *vs = new wxBoxSizer ( wxVERTICAL ) ;
    vs->Add ( new wxStaticText ( this , -1 , "Test" ) , 0 , wxEXPAND , 5 ) ;
    vs->Add ( right , 1 , wxEXPAND , 5 ) ;

    myapp()->frame->setChild ( this ) ;

    this->SetSizer ( vs ) ;
    vs->Fit ( this ) ;    
    this->Show () ;
    }

wxString TVirtualGel::getName ()
    {
    return "Virtual Gel" ;
    }
    
// ****************************************************************

TMyGelControl::TMyGelControl ( wxWindow *parent , int id )
    : wxControl ( parent , id )
    {
    printing = false ;
    }
    
void TMyGelControl::OnDraw(wxDC& pdc)
    {
    if ( vg->lanes.size() == 0 ) return ; // no lanes, nothing to do
    int a , b ;
    int w , h ;
    pdc.GetSize ( &w , &h ) ;
    int lw = w / vg->lanes.size() ;
    if ( lw > 100 ) lw = 100 ;
    int x = lw / 10 ;

    // Calculating presets
    vg->maxband = 0 ;
    vg->cutoff = 10000000 ;
    for ( a = 0 ; a < vg->lanes.size() ; a++ )
    	{
	    for ( b = 0 ; b < vg->lanes[a].vi.GetCount() ; b++ )
	    	{
 	    	if ( vg->lanes[a].vi[b] > vg->maxband )
       			vg->maxband = vg->lanes[a].vi[b] ;
 	    	if ( vg->lanes[a].vi[b] < vg->cutoff )
       			vg->cutoff = vg->lanes[a].vi[b] ;
	    	}    
	    wxRect r ( x+5 , 20 , lw-10 , h - 20 ) ;
	    vg->lanes[a].pos = r ;
	    x += lw ;
     	}   	
     	
   	// Adding top and bottom margin
   	vg->maxband = vg->maxband * 12 / 10 ;
   	vg->cutoff = vg->cutoff * 8 / 10 ;

   	// Drawing gel
    pdc.SetPen ( *wxBLACK_PEN ) ;
    pdc.SetBrush ( *wxWHITE_BRUSH ) ;
    pdc.DrawRectangle ( vg->lanes[0].pos.GetLeft() ,
    					vg->lanes[0].pos.GetTop() ,
    					vg->lanes[vg->lanes.size()-1].pos.GetRight() - vg->lanes[0].pos.GetLeft() ,
    					vg->lanes[vg->lanes.size()-1].pos.GetBottom() - vg->lanes[0].pos.GetTop() ) ;

    // Drawing lanes
    for ( a = 0 ; a < vg->lanes.size() ; a++ )
    	{
	    if ( a == 0 ) vg->percent = 0.7 ;
	    if ( a == 1 ) vg->percent = 1.0 ;
	    if ( a == 2 ) vg->percent = 10 ;
	    
	    for ( b = 0 ; b < vg->lanes[a].vi.GetCount() ; b++ )
	    	{
 	    	drawBand ( pdc , vg->lanes[a] , b ) ;
	    	}    
	    
	    
    	}    
    }
    
void TMyGelControl::drawBand ( wxDC &dc , TGelLane &lane , int band )
	{
	int h = lane.pos.GetHeight() ;
	int y = getLanePos ( lane.vi[band] , h ) + lane.pos.GetTop() ;
	
	double w = lane.vw[band] ;
	w /= (double) lane.unit_volume ;
	w /= 2 ;
	
	for ( int b = 0 ; b < w ; b++ )
		{
		int i = b * 10 ;
		if ( i > 255 ) continue ;
		dc.SetPen ( *MYPEN ( wxColour ( i , i , i ) ) ) ;
    	dc.DrawLine ( lane.pos.GetLeft() + 5 , y + b , 
    				  lane.pos.GetRight() - 5 , y + b ) ;
    	dc.DrawLine ( lane.pos.GetLeft() + 5 , y - b , 
    				  lane.pos.GetRight() - 5 , y - b ) ;
        }		  
    dc.SetTextForeground ( *wxRED ) ;
    wxString title = wxString::Format ( "%d" , lane.vi[band] ) ;
    int tw , th ;
    dc.GetTextExtent ( title , &tw , &th ) ;
    dc.DrawText ( title , ( lane.pos.GetLeft() + lane.pos.GetRight() - tw ) / 2 , y - th / 2 ) ;
	}    
    
int TMyGelControl::getLanePos ( int size , int height , float percent )
	{	
	// Factor
	double x = height ;
	x *= log ( fix_percent ( vg->cutoff ) ) ;
	
	double max = x ;
	max /= log ( fix_percent ( vg->maxband ) ) ;

	double rh = height - max ;
	
	double ret = x ;
	ret /= log ( fix_percent ( size ) ) ;
	
	// Stretching
	ret -= max ;
	ret *= (double) height ;
	ret /= (double) rh ;
	
	return ret ;
	}    

double TMyGelControl::fix_percent ( int size , float percent )
	{
	if ( percent == 0 ) percent = vg->percent ;
	float ret = size ;
	ret = ret / percent ;
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

    if ( event.RightDown() )
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
/*    char t[1000] , *c , *d ;
    strcpy ( t , file.c_str() ) ;
    d = NULL ;
    for ( c = t ; *c ; c++ )
       if ( *c == '.' ) d = c ;
    if ( d ) *d = 0 ;
    wxFileDialog fd ( this , 
                        txt("t_export_as_bitmap") , 
                        dir ,
                        t ,
                        "*.bmp" ,
                        wxSAVE 	 ) ;
    if ( wxID_OK != fd.ShowModal() ) return ;
    i.SaveFile ( fd.GetPath() , wxBITMAP_TYPE_BMP ) ;*/
    }
    
void TMyGelControl::OnCopy(wxCommandEvent &event)
    {
/*    if (wxTheClipboard->Open())
      {
        wxTheClipboard->SetData( new wxBitmapDataObject ( *bmp ) );
        wxTheClipboard->Close();
      }*/
    }

void TMyGelControl::OnPrint(wxCommandEvent &event)
    {
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;

    wxDC *pdc = pd.GetPrintDC () ;
    pdc->StartDoc ( vg->getName() ) ;
    pdc->StartPage () ;
    printing = true ;
    OnDraw ( *pdc ) ;
    printing = false ;
    pdc->EndPage () ;
    pdc->EndDoc () ;
    }
    
// *************************************************

void TGelLane::clear ()
	{
	name = type = "" ;
	vi.Clear () ;
	vw.Clear () ;
	vs.Clear () ;
	unit_volume = 0 ; // µl
	}    

void TGelLane::setMarker ( wxString _name )
	{
	clear () ;
	name = _name ;
	if ( name == "Mass Ruler" )	
		{
		type = "DNA" ;
  		unit_volume = 20 ;
  		add ( 10000 , 200 ) ;
  		add (  8000 , 160 ) ;
  		add (  6000 , 120 ) ;
  		add (  5000 , 100 ) ;
  		add (  4000 ,  80 ) ;
  		add (  3000 ,  60 ) ;
  		add (  2500 ,  50 ) ;
  		add (  2000 ,  40 ) ;
  		add (  1500 ,  32 ) ;
  		add (  1031 , 200 ) ;
  		add (   900 , 180 ) ;
  		add (   800 , 160 ) ;
  		add (   700 , 140 ) ;
  		add (   600 , 120 ) ;
  		add (   500 , 200 ) ;
  		add (   400 ,  80 ) ;
  		add (   300 ,  60 ) ;
  		add (   200 ,  40 ) ;
  		add (   100 ,  20 ) ;
  		add (    80 ,  16 ) ;
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
	add ( size , weight , "" ) ;
	}

void TGelLane::add ( int size , wxString title )
	{
	add ( size , 0 , title ) ;
	}
    
