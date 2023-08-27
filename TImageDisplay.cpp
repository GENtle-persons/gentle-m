#include "TImageDisplay.h"
#include <wx/dir.h>
#include <wx/clipbrd.h>
#include <wx/printdlg.h>

BEGIN_EVENT_TABLE(TImageDisplay, MyChildBase)
    EVT_BUTTON(IV_BUTTON,TImageDisplay::OnDir)
    EVT_LISTBOX(IV_LIST,TImageDisplay::OnFile)
    EVT_CHECKBOX(IV_CB, TImageDisplay::OnCB)
    EVT_CHECKBOX(IV_CB_INVERT, TImageDisplay::OnCBinvert)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_MENU(MDI_COPY,TImageDisplay::OnCopy)
    EVT_MENU(MDI_FILE_SAVE,TImageDisplay::OnSave)
    EVT_MENU(SEQ_PRINT, TImageDisplay::OnPrint)

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

BEGIN_EVENT_TABLE(TMyImagePanel, wxPanel)
    EVT_SIZE(TMyImagePanel::OnSize)
    EVT_PAINT(TMyImagePanel::OnPaint)

    EVT_MOUSE_EVENTS(TMyImagePanel::OnEvent)
    EVT_MENU(IV_MENU_SAVE_AS_BITMAP, TMyImagePanel::OnSaveAsBitmap)
    EVT_MENU(MDI_COPY, TMyImagePanel::OnCopy)
    EVT_MENU(SEQ_PRINT, TMyImagePanel::OnPrint)
END_EVENT_TABLE()


TImageDisplay::TImageDisplay(wxWindow *parent, const wxString& title) 
    : ChildBase(parent, title)
    {
    def = _T("IMAGE") ;
    vec = NULL ;
    r = new TIMGreader ;
    }

TImageDisplay::~TImageDisplay ()
    {
    delete r ;
    }
    
void TImageDisplay::initme ()
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

    right = new TMyImagePanel ( this , IV_IMAGE ) ;
    right->imdi = this ;
    
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *vx = new wxBoxSizer ( wxVERTICAL ) ;

    lb = new wxListBox ( this , IV_LIST , wxDefaultPosition , wxDefaultSize ,
                            0 , NULL , wxLB_SORT ) ;
    bu = new wxButton ( this , IV_BUTTON , _T("") , wxDefaultPosition , wxSize ( 180 , -1 ) ) ;
    cb = new wxCheckBox ( this , IV_CB , txt("img_show_text" ) ) ;
    invert = new wxCheckBox ( this , IV_CB_INVERT , txt("t_invert" ) ) ;
    
    v0->Add ( bu , 0 , wxEXPAND , 5 ) ;
    v0->Add ( lb , 1 , wxEXPAND , 5 ) ;
    
    h1->Add ( cb , 0 , wxEXPAND , 5 ) ;
    h1->Add ( invert , 0 , wxEXPAND , 5 ) ;
    v0->Add ( h1 , 0 , wxEXPAND , 5 ) ;
    
    h0->Add ( v0 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( right , 1 , wxEXPAND , 5 ) ;

    if ( !myapp()->frame->mainToolBar )
       {
       myapp()->frame->setDummyToolbar ( this ) ;
       myapp()->frame->addTool ( toolbar , MDI_FILE_SAVE ) ;
       myapp()->frame->addTool ( toolbar , SEQ_PRINT ) ;
       myapp()->frame->addTool ( toolbar , MDI_COPY ) ;
       myapp()->frame->addDefaultTools ( toolbar ) ;
       toolbar->Realize() ;
       vx->Add ( toolbar , 0 , wxEXPAND , 2 ) ;
       }

    vx->Add ( h0 , 1 , wxEXPAND , 5 ) ;
    SetSizer ( vx ) ;
    vx->Fit ( this ) ;
    
    cb->SetValue ( true ) ;
    wxString s_dir = myapp()->frame->LS->getOption ( _T("IMGDIR") , wxGetCwd() ) ;
    ShowDir ( s_dir ) ;
    myapp()->frame->setChild ( this ) ;
    }
    
void TImageDisplay::ShowDir ( wxString s )
	{
    wxBeginBusyCursor() ;
    wxDir dir(s);
    lb->Clear() ;
    bu->SetLabel ( s ) ;

    if ( !dir.IsOpened() )
        return;

    myapp()->frame->LS->setOption ( _T("IMGDIR") , s ) ;
    wxString filename;
    
    wxArrayString vs ;
    dir.GetAllFiles ( s , &vs , _T("") , wxDIR_FILES ) ; // To save listing all types...
/*    dir.GetAllFiles ( s , &vs , "*.img" ) ;
    dir.GetAllFiles ( s , &vs , "*.tif" ) ;
    dir.GetAllFiles ( s , &vs , "*.tiff" ) ;
    dir.GetAllFiles ( s , &vs , "*.jpg" ) ;
    dir.GetAllFiles ( s , &vs , "*.jpeg" ) ;
    dir.GetAllFiles ( s , &vs , "*.png" ) ;
    dir.GetAllFiles ( s , &vs , "*.bmp" ) ;
    dir.GetAllFiles ( s , &vs , "*.gif" ) ;
    dir.GetAllFiles ( s , &vs , "*.pcx" ) ;
    dir.GetAllFiles ( s , &vs , "*.pnm" ) ;
*/    
    int a ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
    	{
	    vs[a] = vs[a].AfterLast ( '/' ) ;
	    vs[a] = vs[a].AfterLast ( '\\' ) ;
     	}        
    vs.Sort () ;
    
    for ( a = 0 ; a < vs.GetCount() ; a++ )
    	lb->Append ( vs[a] ) ;
   	wxEndBusyCursor() ;
   	}
   
void TImageDisplay::OnCopy ( wxCommandEvent &event )
    {
	if ( right ) right->OnCopy ( event ) ;
	}

void TImageDisplay::OnPrint ( wxCommandEvent &event )
    {
	if ( right ) right->OnPrint ( event ) ;
	}

void TImageDisplay::OnSave ( wxCommandEvent &event )
    {
	if ( right ) right->OnSaveAsBitmap ( event ) ;
	}

void TImageDisplay::OnDir ( wxCommandEvent &event )
    {
    wxDirDialog dd ( this  , txt("t_choose_dir") , bu->GetLabel() ) ;
    if ( wxID_OK != dd.ShowModal() ) return ;
    ShowDir ( dd.GetPath() ) ;
    }
    
void TImageDisplay::OnFile ( wxCommandEvent &event )
    {
    wxBeginBusyCursor () ;
    wxString file = lb->GetStringSelection() ;
    wxString dir = bu->GetLabel() ;
    wxString fn = dir + _T("/") + file ;
    
    if ( fn.AfterLast('.').Upper() == _T("IMG") )
	    {
    	r->readFile ( fn ) ;
    	right->i = r->makeImage() ;
    	}   	
    else
	    {
		wxLogNull logNo; // Supress error message
		if ( !right->i.LoadFile ( fn , wxBITMAP_TYPE_ANY ) )
			{
			right->ClearBackground() ;
			ClearBackground() ;
			Refresh ( true ) ;
			right->Refresh ( true ) ;
			allow_save = allow_copy = allow_print = false ;
			myapp()->frame->updateCCP ( this ) ;
			}
	    }    
    
    wxClientDC dc ( right ) ;
    dc.Clear() ;
    
    right->dir = dir ;
    right->file = file ;
    
    if ( right->bmp ) delete right->bmp ;
    right->bmp = new wxBitmap ( right->i ) ;
    right->Refresh () ;
	allow_save = allow_copy = allow_print = true ;
	myapp()->frame->updateCCP ( this );
    wxEndBusyCursor () ;
    }
    
void TImageDisplay::OnCB ( wxCommandEvent &event )
	{
	right->show_text = cb->GetValue() ;
	right->ClearBackground () ;
	right->Refresh () ;
	}    

void TImageDisplay::OnCBinvert ( wxCommandEvent &event )
	{
	right->invert = invert->GetValue() ;
	right->ClearBackground () ;
	right->Refresh () ;
	}    

const wxString TImageDisplay::getName () const
    {
    return _T("Image Viewer") ;
    }
    
// ****************************************************************

TMyImagePanel::TMyImagePanel ( wxWindow *parent , int id )
    : wxPanel ( parent , id )
    {
    bmp = NULL ;
    printing = false ;
    show_text = true ;
    invert = false ;
    }
    
TMyImagePanel::~TMyImagePanel ()
    {
    if ( bmp ) delete bmp ;
    }

void TMyImagePanel::OnDraw(wxDC& pdc)
    {
    if ( !bmp ) return ;

    pdc.SetFont ( *MYFONT ( 8 , wxFONTFAMILY_MODERN , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;

    int w , h , iw , ih ;
    pdc.GetSize ( &w , &h ) ;

    int tw , th ;
    pdc.GetTextExtent ( file , &tw , &th ) ;

    iw = bmp->GetWidth() ;
    ih = bmp->GetHeight() ;
    
    double xs , ys ;
    xs = w ;
    xs /= iw ;
    ys = xs ;
    if ( ih * ys > (h) )
       {
       ys = h ;
       ys /= ih ;
       xs = ys ;
       }
    
    double factor = 0.95 ;
            
    xs *= factor ;
    ys *= factor ;        
    
    double nw = iw ;
    double nh = ih ;
    nw *= xs ;
    nh *= ys ;
    
    wxImage imago = bmp->ConvertToImage () ;
    imago = imago.Rescale ( nw , nh ) ;
    wxBitmap bmp2 ( imago ) ;
    
    double x , y ;
    x = ( w - nw ) / 2 ;
    y = ( h - nh ) / 2 ;
    
    pdc.DrawBitmap ( bmp2 , (int)x , (int)y ) ;

    if ( invert )
    	{
        wxRasterOperationMode lf = pdc.GetLogicalFunction() ;
        pdc.SetLogicalFunction ( wxINVERT ) ;
        pdc.SetBrush ( *wxWHITE_BRUSH ) ;
        pdc.SetPen ( *wxWHITE_PEN ) ;
        pdc.DrawRectangle ( (int) x , (int) y , (int) (nw) , (int) (nh) ) ;
        pdc.SetLogicalFunction ( lf ) ;
        }    
    
    pdc.SetTextForeground ( wxColour ( 0 , 0 , 150 ) ) ;
    if ( show_text )
    	{
        for ( int i = 0 ; i < imdi->r->items.size() ; i++ )
           imdi->r->items[i].draw ( pdc , (int)x , (int)y , (int)(x+nw) , (int)(y+nh) ) ;
        
        double tx , ty ;
        tx = ( w - tw ) / 2 ;
        ty = y - th / 2 ;
        pdc.DrawText ( file , (int)tx , (int)ty ) ;
        }    
    }
    
void TMyImagePanel::Refresh (bool eraseBackground , const wxRect* rect)
    {
    wxClientDC dc ( (wxWindow*) this ) ;
    OnDraw ( dc ) ;
    }
    
void TMyImagePanel::OnSize(wxSizeEvent &event)
    {
    Refresh () ;
    }

void TMyImagePanel::OnEvent(wxMouseEvent& event)
    {
    wxPoint pt(event.GetPosition());

    if ( event.RightDown() && bmp )
        {
        wxMenu *cm = new wxMenu ;
        cm->Append ( IV_MENU_SAVE_AS_BITMAP , txt("m_save_as_bitmap") ) ;
        cm->Append ( MDI_COPY , txt("m_copy_to_clipboard") ) ;
        cm->Append ( SEQ_PRINT , txt("m_print") ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;    
        }
    }
    
void TMyImagePanel::WriteIntoBitmap(wxBitmap &bmp2)
    {
    bmp2 = wxBitmap ( bmp->GetWidth() , bmp->GetHeight() , bmp->GetDepth() ) ;
    wxMemoryDC memdc ;
    memdc.SelectObject ( bmp2 ) ;
    memdc.Clear () ;
    OnDraw ( memdc ) ;    
    }
    
void TMyImagePanel::OnSaveAsBitmap(wxCommandEvent &event)
    {
    char t[1000] , *c , *d ;
    strcpy ( t , file.mb_str() ) ;
    d = NULL ;
    for ( c = t ; *c ; c++ )
       if ( *c == '.' ) d = c ;
    if ( d ) *d = 0 ;    

    wxBitmap bmp2 ;
    WriteIntoBitmap ( bmp2 ) ;
    myapp()->frame->saveImage ( &bmp2 , wxString ( t , *wxConvCurrent ) ) ;
    }
    
void TMyImagePanel::OnCopy(wxCommandEvent &event)
    {
    if (wxTheClipboard->Open())
      {
      wxBitmap bmp ;
      WriteIntoBitmap ( bmp ) ;
      wxTheClipboard->SetData( new wxBitmapDataObject ( bmp ) );
      wxTheClipboard->Close();
      }
    }

void TMyImagePanel::OnPrint(wxCommandEvent &event)
    {
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;

    wxDC *pdc = pd.GetPrintDC () ;
    pdc->StartDoc ( file ) ;
    pdc->StartPage () ;
    printing = true ;
    OnDraw ( *pdc ) ;
    printing = false ;
    pdc->EndPage () ;
    pdc->EndDoc () ;
    }

void TMyImagePanel::OnPaint(wxPaintEvent& event)
	{
    wxPaintDC dc(this);
    OnDraw ( dc ) ;
    }

