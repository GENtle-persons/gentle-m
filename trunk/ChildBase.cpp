#include "main.h"
//#include "ChildBase.h"

ChildBase::ChildBase ()
    {
    def = "" ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }
    
ChildBase::ChildBase(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
        : MyChildBase((MyFrameType*)parent, -1, pos, size, style)
    {
    def = "" ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }
        
ChildBase::ChildBase(wxWindow *parent, const wxString& title)
        : MyChildBase((MyFrameType*)parent, -1)
    {
    def = "" ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }


wxToolBar *ChildBase::CreateToolBar ( int i )
{
#ifdef __WXMSW__
  return ChildBase::CreateToolBar ( i , -1 , "" ) ;
#else
  wxToolBar *bar = new wxToolBar ( myapp()->frame , -1 ) ;
  //  SetToolBar ( bar ) ;
  return bar ;
  //  return myapp()->frame->GetToolBar () ;
#endif
}

void ChildBase::Maximize ( bool isit )
{
  Activate () ;
}

void ChildBase::showName ( wxString x )
    {
    if ( x.IsEmpty() )
       {
       x = getName() ;
       if ( vec && vec->isChanged() ) x += "*" ;
       }
    if ( myapp()->frame->GetTitle() != x )
        myapp()->frame->SetTitle ( x ) ;
    }
    
void ChildBase::OnFocus(wxFocusEvent& event)
    {
    myass ( myapp() , "Oh no! No application defined!" ) ;
    myass ( myapp()->frame , "Oh no! No frame defined!" ) ;
    if ( myapp()->frame->dying ) return ;
    
    Activate () ;
    showName ( ) ;
    myapp()->frame->mainTree->SelectItem ( inMainTree ) ;
    SetFocus () ;
    if ( cSequence ) cSequence->SetFocus() ;
    }

bool ChildBase::caniclose(wxCloseEvent& event)
    {
    if ( /*event.CanVeto() && */vec && vec->isChanged() )
       {
       int r = wxMessageBox ( wxString::Format ( txt("t_close_anyway") , getName().c_str() ) , 
                            txt("msg_box"), wxICON_QUESTION | wxYES | wxNO | wxCANCEL ) ;
       
       if ( r == wxYES )
          {
          wxCommandEvent ev ;
          OnFileSave ( ev ) ;
          return true ;
          }
       else if ( r == wxCANCEL )
          {
          event.Veto();
          return false ;
          }
       }
    return true ;
    }

void ChildBase::updateUndoMenu () 
    {
    }
    
wxToolBar *ChildBase::CreateToolBar ( int i , int j , wxString s )
    {
    toolbar = new wxToolBar ( this , -1 ) ;
    return toolbar ;
    }
    
void ChildBase::SetMenuBar ( wxMenuBar *menu_bar )
    {
    menubar = menu_bar ;
    }
    
wxMenuBar *ChildBase::GetMenuBar ()
    {
    return menubar ;
    }
    
void ChildBase::Activate ()
    {
    myapp()->frame->setActiveChild ( this ) ;
    if ( myapp()->frame->isLocked() ) return ;
    showName () ;
    SetFocus();
    if ( cSequence ) cSequence->SetFocus() ;
    }
    
wxToolBar *ChildBase::GetToolBar ()
    {
    return toolbar ;
    }
    
void ChildBase::SetIcon ( wxIcon icon )
    {
    }
    
void ChildBase::OnClose(wxCloseEvent& event)
{
    if ( !caniclose ( event ) )
        {
        event.Veto() ;
        return ;
        }
    
    myass ( this , "ChildBase::OnClose" ) ;
    myapp()->frame->mainTree->removeChild ( this ) ;
    myapp()->frame->SetTitle ( txt("gentle") ) ;
    SetTitle ( txt("gentle") ) ;
    event.Skip();
    myapp()->frame->removeChild ( this ) ;
}

void ChildBase::OnExport (wxCommandEvent& event)
    {
    if ( !vec && def != "alignment" ) return ;

    wxString wcGenBank = "GenBank (*.gb)|*.gb" ;
    wxString wcFasta = "Fasta|*.*" ;
    wxString wcEMBL = "EMBL|*.*" ;
    wxString wcPlain = "Plain text|*.*" ;
    wxString wcIG = "IntelliGenetics format (*.ig)|*.ig" ;
    wxString wcClone = "CLONE|*.*" ;
    wxString wildcard = wcGenBank + "|" +
                        wcPlain + "|" +
                        wcFasta + "|" + 
                        wcEMBL + "|" + 
                        wcIG ; 
    wxString lastdir = myapp()->frame->LS->getOption ( "LAST_IMPORT_DIR" , "C:" ) ;
    wxFileDialog d ( this , txt("export_file") , lastdir , "" , wildcard , wxSAVE ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;

    myapp()->frame->LS->setOption ( "LAST_IMPORT_DIR" , d.GetDirectory() ) ;
    doExport ( d.GetPath() , d.GetFilterIndex () ) ;    
    }   
    
void ChildBase::doExport ( wxString filename , int filter )
    {
    wxFile out ( filename , wxFile::write ) ;
    exportVector ( vec , out , filter ) ;
    out.Close () ;
    }    

void ChildBase::exportVector ( TVector *vec , wxFile &out , int filter )
    {
    if ( filter == 0 ) // GeneBank 
        {
        TGenBank gb ;
        wxArrayString ex ;
        gb.doExport ( vec , ex ) ;
        for ( int a = 0 ; a < ex.GetCount() ; a++ )
           out.Write ( ex[a] + "\n" ) ;
        }
    else if ( filter == 1 ) // Plain text
        {
        out.Write ( vec->getSequence() + "\n" ) ;
        }
    else if ( filter == 2 ) // FASTA
        {
        out.Write ( ">" + vec->getName() + "\n" ) ;
        wxString s = vec->getSequence() ;
        while ( !s.IsEmpty() )
           {
           out.Write ( s.Left ( 80 ) + "\n" ) ;
           s = s.Mid ( 80 ) ;
           }    
        }
    else if ( filter == 3 ) // EMBL
        {
        arrangedExport ( out , "ID   " , vec->getName() , 70 ) ;
        arrangedExport ( out , "NM   " , vec->getDescription() , 70 ) ;
        out.Write ( "SQ   Sequence\n" ) ;
        arrangedExport ( out , "     " , vec->getSequence().Lower() , 70 ) ;
        out.Write ( "//\n" ) ;
        }
    else if ( filter == 4 ) // IG
        {
        wxString n = vec->getName() ;
        n.Replace ( " " , "_" ) ;
        out.Write ( n + "\n" ) ;
        wxString s = vec->getSequence() ;
        if ( vec->isCircular() ) s += "2" ;
        else s += "1" ;
        while ( !s.IsEmpty() )
           {
           out.Write ( s.Left ( 79 ) + "\n" ) ;
           s = s.Mid ( 80 ) ;
           }    
        }
    }
        
void ChildBase::arrangedExport ( wxFile &out , wxString n , wxString s , int l )
    {
    int sl = l - n.length() ;
    wxString blank ( ' ' , n.length() ) ;
    while ( !s.IsEmpty() )
       {
       out.Write ( n + s.Left ( sl ) + "\n" ) ;
       s = s.Mid ( sl ) ;
       n = blank ;
       }    
    }
        
