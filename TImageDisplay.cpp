#include "TImageDisplay.h"
#include <wx/dir.h>
#include <wx/clipbrd.h>
#include <wx/printdlg.h>

BEGIN_EVENT_TABLE(TImageDisplay, MyChildBase)
    EVT_BUTTON(IV_BUTTON,TImageDisplay::OnDir)
    EVT_LISTBOX(IV_LIST,TImageDisplay::OnFile)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TMyImagePanel, wxPanel)
    EVT_SIZE(TMyImagePanel::OnSize)

    EVT_MOUSE_EVENTS(TMyImagePanel::OnEvent)
    EVT_MENU(IV_MENU_SAVE_AS_BITMAP, TMyImagePanel::OnSaveAsBitmap)
    EVT_MENU(IV_MENU_COPY, TMyImagePanel::OnCopy)
    EVT_MENU(IV_MENU_PRINT, TMyImagePanel::OnPrint)
END_EVENT_TABLE()


TImageDisplay::TImageDisplay(MyFrame *parent, const wxString& title) 
    : ChildBase(parent, title)
    {
    def = "IMAGE" ;
    vec = NULL ;
    r = new TIMGreader ;
    }

TImageDisplay::~TImageDisplay ()
    {
    delete r ;
    }
    
    
void TImageDisplay::OnClose(wxCloseEvent& event)
{
    // Removing the window from the main tree
    MyFrame *p = (MyFrame*)GetParent();
    p->mainTree->removeChild ( this ) ;
    p->SetTitle ( txt("gentle") ) ;
    SetTitle ( txt("gentle") ) ;
    p->removeChild ( this ) ;
    event.Skip();
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

    rl = new wxSplitterWindow ( this , -1 ) ;
    right = new TMyImagePanel ( rl , IV_IMAGE ) ;
    right->imdi = this ;
    
    wxSplitterWindow *ud ;
    ud = new wxSplitterWindow ( rl , -1 ) ;

    rl->SplitVertically ( ud , right , 300 ) ;
    
    lb = new wxListBox ( ud , IV_LIST , wxDefaultPosition , wxDefaultSize ,
                            0 , NULL , wxLB_SORT ) ;
    bu = new wxButton ( ud , IV_BUTTON , "" ) ;
    
    ud->SplitHorizontally ( bu , lb , 20 ) ;
    
    string s_dir = myapp()->frame->LS->getOption ( "IMGDIR" , wxGetCwd().c_str() ) ;    
    ShowDir ( s_dir.c_str() ) ;
    myapp()->frame->setChild ( this ) ;
    }
    
void TImageDisplay::ShowDir ( wxString s )
   {
    wxDir dir(s);
    lb->Clear() ;
    bu->SetTitle ( s ) ;

    if ( !dir.IsOpened() )
        return;

    myapp()->frame->LS->setOption ( "IMGDIR" , s.c_str() ) ;
    wxString filename;

    bool cont = dir.GetFirst(&filename, "*.img", wxDIR_FILES);
    while ( cont )
    {
        lb->Append ( filename ) ;
        cont = dir.GetNext(&filename);
    }
   }
   
void TImageDisplay::OnDir ( wxCommandEvent &event )
    {
    wxDirDialog dd ( this  ) ;
    dd.SetPath ( bu->GetTitle() ) ;
    if ( wxID_OK != dd.ShowModal() ) return ;
    ShowDir ( dd.GetPath() ) ;
    }
    
void TImageDisplay::OnFile ( wxCommandEvent &event )
    {
    wxString file = lb->GetStringSelection() ;
    wxString dir = bu->GetTitle() ;
    wxString fn = dir + "/" + file ;
    r->readFile ( fn.c_str() ) ;
    right->i = r->makeImage() ;

//    i.SaveFile ( "test.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxClientDC dc ( right ) ;
    dc.Clear() ;
    
    right->dir = dir ;
    right->file = file ;
    
    if ( right->bmp ) delete right->bmp ;
    right->bmp = new wxBitmap ( right->i ) ;
    right->Refresh () ;
    }

string TImageDisplay::getName ()
    {
    return "Image Viewer" ;
    }
    
// ****************************************************************

TMyImagePanel::TMyImagePanel ( wxWindow *parent , int id )
    : wxPanel ( parent , id )
    {
    bmp = NULL ;
    printing = false ;
    }
    
TMyImagePanel::~TMyImagePanel ()
    {
    if ( bmp ) delete bmp ;
    }

void TMyImagePanel::OnDraw(wxDC& pdc)
    {
    if ( !bmp ) return ;
//    if ( printing )
        {
        int w , h , iw , ih ;
        pdc.GetSize ( &w , &h ) ;

        int tw , th ;
        pdc.GetTextExtent ( file.c_str() , &tw , &th ) ;

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
        
        double factor = 0.6 ;
        xs *= factor ;
        ys *= factor ;        
        pdc.SetUserScale ( xs , ys ) ;
        
        double nw = iw ;
        double nh = ih ;
        nw *= xs ;
        nh *= ys ;
        
        double x = w ;
        double y = h ;
        x -= nw ;
        y -= nh ;
        x /= 2 ;
        y /= 2 ;
        x /= xs ;
        y /= ys ;
        
        pdc.DrawBitmap ( *bmp , x , y ) ;

        for ( int i = 0 ; i < imdi->r->items.size() ; i++ )
           {
           imdi->r->items[i].draw ( pdc , x , y , x+nw , y+nh ) ;
           }
        
        double tx = w - tw ;
        double ty = h ;
        tx /= (double) 2 * xs ;
        ty = y ;
        ty -= ( (double) th ) / ys ;
        pdc.DrawText ( file.c_str() , tx , ty ) ;

//        pdc.DrawLine ( 0 , 0 , tx , ty ) ;
        
        pdc.SetUserScale ( 1 , 1 ) ;
        
        }
//    else pdc.DrawBitmap ( *bmp , 0 , 0 ) ;
    }
    
void TMyImagePanel::Refresh (bool eraseBackground , const wxRect* rect)
    {
    wxClientDC dc ( (wxWindow*) this ) ;
    OnDraw ( dc ) ;
    }
    
void TMyImagePanel::OnSize(wxSizeEvent &event)
    {
    Refresh () ;
//    wxPanel::OnSize ( event ) ;
    }

void TMyImagePanel::OnEvent(wxMouseEvent& event)
    {
    wxPoint pt(event.GetPosition());

    if ( event.RightDown() && bmp )
        {
        wxMenu *cm = new wxMenu ;
        cm->Append ( IV_MENU_SAVE_AS_BITMAP , txt("m_save_as_bitmap") ) ;
        cm->Append ( IV_MENU_COPY , txt("m_copy_to_clipboard") ) ;
        cm->Append ( IV_MENU_PRINT , txt("m_print") ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;    
        }
    }
    
void TMyImagePanel::OnSaveAsBitmap(wxCommandEvent &event)
    {
    char t[1000] , *c , *d ;
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
    i.SaveFile ( fd.GetPath() , wxBITMAP_TYPE_BMP ) ;
    }
    
void TMyImagePanel::OnCopy(wxCommandEvent &event)
    {
    if (wxTheClipboard->Open())
      {
        wxTheClipboard->SetData( new wxBitmapDataObject ( *bmp ) );
        wxTheClipboard->Close();
      }
    }

void TMyImagePanel::OnPrint(wxCommandEvent &event)
    {
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;

    wxDC *pdc = pd.GetPrintDC () ;
    pdc->StartDoc ( file.c_str() ) ;
    pdc->StartPage () ;
    printing = true ;
    OnDraw ( *pdc ) ;
    printing = false ;
    pdc->EndPage () ;
    pdc->EndDoc () ;
    }
    
