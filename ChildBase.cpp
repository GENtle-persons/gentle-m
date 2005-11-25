/** \file
	\brief Contains the ChildBase class
*/
#include "main.h"
//#include "ChildBase.h"

ChildBase::ChildBase ()
    {
    def = _T("") ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }
    
ChildBase::ChildBase(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
        : MyChildBase((MyFrameType*)parent, -1, pos, size, style|wxFULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN)
    {
    def = _T("") ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }
        
ChildBase::ChildBase(wxWindow *parent, const wxString& title)
        : MyChildBase((MyFrameType*)parent, -1,wxDefaultPosition,wxDefaultSize,wxFULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN)
    {
    def = _T("") ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }


wxToolBar *ChildBase::CreateToolBar ( int i )
{
//#ifdef __WXMSW__
  return ChildBase::CreateToolBar ( i , -1 , _T("") ) ;
/*#else
  wxToolBar *bar = new wxToolBar ( myapp()->frame , -1 ) ;
  return bar ;
  #endif*/
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
       if ( vec && vec->isChanged() ) x += _T("*") ;
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
    
    
void ChildBase::updateToolbar ()
    {
    }    

void ChildBase::updateSequenceCanvas ( bool remember ) // Dummy
    {
    }    

void ChildBase::SetMyMenuBar ()
	{
    if ( menubar && myapp()->frame->GetMenuBar() != menubar )
		{
		myapp()->frame->SetMenuBar ( menubar ) ;
#ifdef __WXMAC__
		// The following is a butt-ugly patch
		// to get wxMac to refresh the menu bar
		// The freeze/thaw stuff is there in the hopes of speeding it up
		myapp()->frame->Freeze() ;
		wxDialog d ( (wxWindow*)this , -1 , txt("") , wxPoint ( 0 , 0 ) , wxSize ( 1 , 1 ) ) ;
		d.Freeze() ;
		d.Show() ;
		d.Destroy() ;
		myapp()->frame->Thaw() ;
#endif
		}
	}

void ChildBase::Activate ()
    {
    myapp()->frame->setActiveChild ( this ) ;
	SetMyMenuBar () ;
    if ( myapp()->frame->isLocked() ) return ;
    updateToolbar () ;
    showName () ;
    SetFocus();
    if ( cSequence ) cSequence->SetFocus() ;

    // Mark and status bar
    int a ;
    for ( a = 0 ; a < myapp()->frame->GetStatusBar()->GetFieldsCount() ; a++ )
    	myapp()->frame->SetStatusText ( _T("") , a ) ;
   	if ( cSequence && 
    	 cSequence->markedFrom() != -1 && 
  		 cSequence->lastmarked < cSequence->seq.GetCount() )
   		{
   		cSequence->mark ( cSequence->seq[cSequence->lastmarked]->whatsthis() , 
     						cSequence->markedFrom() , 
           					cSequence->markedTo() , 
                			cSequence->getEditMode() ? 2 : 1 ) ;
 		}
//    Refresh () ;
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

    toolbar = NULL ;
    myass ( this , "ChildBase::OnClose" ) ;
    myapp()->frame->mainTree->removeChild ( this ) ;
    myapp()->frame->SetTitle ( txt("gentle") ) ;
    SetTitle ( txt("gentle") ) ;
    event.Skip();
    myapp()->frame->removeChild ( this ) ;
}

void ChildBase::OnExport (wxCommandEvent& event)
    {
    if ( !vec && def != _T("alignment") ) return ;

	 wxString wildcard = getExportFilters () ;
    wxString lastdir = myapp()->frame->LS->getOption ( _T("LAST_IMPORT_DIR") , _T("C:") ) ;
    wxFileDialog d ( this , txt("export_file") , lastdir , _T("") , wildcard , wxSAVE|wxOVERWRITE_PROMPT ) ;
    d.SetFilterIndex ( myapp()->frame->LS->getOption ( _T("LAST_EXPORT_FILTER") , 0 ) ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;

    myapp()->frame->LS->setOption ( _T("LAST_EXPORT_FILTER") , d.GetFilterIndex() ) ;
    myapp()->frame->LS->setOption ( _T("LAST_IMPORT_DIR") , d.GetDirectory() ) ;
    doExport ( d.GetPath() , d.GetFilterIndex() ) ;    
    }   
    
wxString ChildBase::getExportFilters ()
	{
    wxString wcGenBank = _T("GenBank (*.gb)|*.gb") ;
    wxString wcFasta = _T("Fasta|*.*") ;
    wxString wcEMBL = _T("EMBL|*.*") ;
    wxString wcPlain = _T("Plain text|*.*") ;
    wxString wcIG = _T("IntelliGenetics format (*.ig)|*.ig") ;
    wxString wcClone = _T("CLONE|*.*") ;
    wxString wcGCviewXML = _T("CGview (XML)|*.xml") ;
    wxString wcCSV = _T("Comma-separated values (CSV)|*.csv") ;
    wxString wildcard = wcGenBank + _T("|") +
                        wcPlain + _T("|") +
                        wcFasta + _T("|") +
                        wcEMBL + _T("|") +
                        wcIG + _T("|") +
                        wcGCviewXML + _T("|") +
								wcCSV ;
	return wildcard ;
	}
    
void ChildBase::doExport ( wxString filename , int filter )
    {
    wxFile out ( filename , wxFile::write ) ;
    exportVector ( vec , out , filter , filename ) ;
    out.Close () ;
    }    

void ChildBase::exportVector ( TVector *vec , wxFile &out , int filter , wxString filename )
    {
    if ( filter == 0 ) // GeneBank 
        {
        TGenBank gb ;
        wxArrayString ex ;
        gb.doExport ( vec , ex ) ;
        for ( unsigned int a = 0 ; a < ex.GetCount() ; a++ )
           out.Write ( ex[a] + _T("\n") ) ;
        }
    else if ( filter == 1 ) // Plain text
        {
        out.Write ( vec->getSequence() + _T("\n") ) ;
        }
    else if ( filter == 2 ) // FASTA
        {
        out.Write ( _T(">") + vec->getName() + _T("\n") ) ;
        wxString s = vec->getSequence() ;
        while ( !s.IsEmpty() )
           {
           out.Write ( s.Left ( 80 ) + _T("\n") ) ;
           s = s.Mid ( 80 ) ;
           }    
        }
    else if ( filter == 3 ) // EMBL
        {
        arrangedExport ( out , _T("ID   ") , vec->getName() , 70 ) ;
        arrangedExport ( out , _T("NM   ") , vec->getDescription() , 70 ) ;
        out.Write ( _T("SQ   Sequence\n") ) ;
        arrangedExport ( out , _T("     ") , vec->getSequence().Lower() , 70 ) ;
        out.Write ( _T("//\n") ) ;
        }
    else if ( filter == 4 ) // IG
        {
        wxString n = vec->getName() ;
        n.Replace ( _T(" ") , _T("_") ) ;
        out.Write ( n + _T("\n") ) ;
        wxString s = vec->getSequence() ;
        if ( vec->isCircular() ) s += _T("2") ;
        else s += _T("1") ;
        while ( !s.IsEmpty() )
           {
           out.Write ( s.Left ( 79 ) + _T("\n") ) ;
           s = s.Mid ( 80 ) ;
           }    
        }
    else if ( filter == 5 ) // GCview XML
        {
        CGview cgv ( vec ) ;
        if ( !cgv.runSettingsDialog () ) return ; // Cancel
        out.Write ( cgv.getXML() ) ;
        cgv.postProcess ( filename ) ;
        }
    }
        
void ChildBase::arrangedExport ( wxFile &out , wxString n , wxString s , int l )
    {
    int sl = l - n.length() ;
    wxString blank ( ' ' , n.length() ) ;
    while ( !s.IsEmpty() )
       {
       out.Write ( n + s.Left ( sl ) + _T("\n") ) ;
       s = s.Mid ( sl ) ;
       n = blank ;
       }    
    }
        
