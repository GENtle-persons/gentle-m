/** \file
    \brief Contains the ChildBase class
*/
#include "main.h"
#include "ChildBase.h"
#include <wx/treectrl.h>

ChildBase::ChildBase ()
    {
    init () ;
    }

ChildBase::ChildBase(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
        : MyChildBase((MyFrameType*)parent, -1, pos, size, style|wxFULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN)
    {
    init () ;
    }

ChildBase::ChildBase(wxWindow *parent, const wxString& title)
        : MyChildBase((MyFrameType*)parent, -1,wxDefaultPosition,wxDefaultSize,wxFULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN)
    {
    init () ;
    }

void ChildBase::init ()
    {
    def = _T("") ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    allow_cut = allow_copy = allow_paste = allow_find = false ;
    allow_save = allow_print = allow_undo = false ;
    }

wxToolBar *ChildBase::CreateToolBar ( int i )
{
  i = wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL | wxTB_DOCKABLE ; // Override
  return ChildBase::CreateToolBar ( i , -1 , _T("") ) ;
}

void ChildBase::Maximize ( bool isit )
{
  Activate () ;
}

void ChildBase::showName ( const wxString& _x )
    {
    wxString x(_x) ;
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
    if ( myapp()->frame->dying )
        return ;

    Activate () ;
    showName () ;

    //SetFocusedItem ( inMainTree ) ;

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

wxToolBar *ChildBase::CreateToolBar ( int i , int j ,const wxString& s )
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

void ChildBase::initHelp ()
    {
    while ( myapp()->frame->count_help() > 1 )
        {
        myapp()->frame->pop_help() ;
        }
    if ( def == _T("dna") ) myapp()->frame->push_help ( _T("GENtle/DNA") ) ;
    else if ( def == _T("AminoAcids") ) myapp()->frame->push_help ( _T("GENtle/Protein") ) ;
    else if ( def == _T("ABIviewer") ) myapp()->frame->push_help ( _T("GENtle/Sequencing") ) ;
    else if ( def == _T("alignment") ) myapp()->frame->push_help ( _T("GENtle/Alignments") ) ;
    else if ( def == _T("EXTERNAL") ) myapp()->frame->push_help ( _T("GENtle/Web_interface") ) ;
    else if ( def == _T("PrimerDesign") ) myapp()->frame->push_help ( _T("GENtle/PCR_and_Primer_Design") ) ;
    else if ( def == _T("CALCULATOR") ) myapp()->frame->push_help ( _T("GENtle/Calculators") ) ;
    else if ( def == _T("GRAPH") ) myapp()->frame->push_help ( _T("GENtle/Graph") ) ;
    else if ( def == _T("IMAGE") ) myapp()->frame->push_help ( _T("GENtle/Image_Viewer") ) ;
    else if ( def == _T("PHYLOTREE") ) myapp()->frame->push_help ( _T("GENtle/Protein") ) ;
    else if ( def == _T("GEL") ) myapp()->frame->push_help ( _T("GENtle/Virtual_Gel") ) ;
    else if ( def == _T("RESTRICTION_IDENTIFIER") ) myapp()->frame->push_help ( _T("GENtle/Restriction_Identifier") ) ;
    else if ( def == _T("DOTPLOT") ) myapp()->frame->push_help ( _T("GENtle/Dot_plot") ) ;
    }

void ChildBase::Activate ()
    {
    initHelp () ;
    myapp()->frame->setActiveChild ( this ) ;
    SetMyMenuBar () ;
    if ( myapp()->frame->isLocked() ) return ;
    updateToolbar () ;
    myapp()->frame->updateCCP ( this ) ;
    showName () ;
    SetFocus();
    if ( cSequence ) cSequence->SetFocus() ;

    // Mark and status bar
    for ( int a = 0 ; a < myapp()->frame->GetStatusBar()->GetFieldsCount() ; a++ )
        {
        myapp()->frame->SetStatusText ( _T("") , a ) ;
        }
    if ( cSequence && cSequence->markedFrom() != -1 && cSequence->lastmarked < cSequence->seq.GetCount() )
        {
        cSequence->mark ( cSequence->seq[cSequence->lastmarked]->whatsthis() , cSequence->markedFrom() , cSequence->markedTo() , cSequence->getEditMode() ? 2 : 1 ) ;
        }

    if ( myapp()->frame->update_child_list )
        {
        myapp()->frame->update_child_list = false ;
        myapp()->frame->notifyChildrenChanged() ;
        }

//    Refresh () ;
    }

wxToolBar *ChildBase::GetToolBar ()
    {
    return toolbar ;
    }

void ChildBase::SetIcon ( const wxIcon& icon )
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
//    SetTitle ( txt("gentle") ) ; // 2.8
    event.Skip();
    myapp()->frame->removeChild ( this ) ;
}

void ChildBase::OnExport (wxCommandEvent& event)
    {
    if ( !vec && def != "alignment" ) return ;

    wxString wildcard = getExportFilters () ;
    wxString lastdir = myapp()->frame->LS->getOption ( _T("LAST_IMPORT_DIR") , "C:" ) ;
    wxFileDialog d ( this , txt("export_file") , lastdir , _T("") , wildcard , wxFD_SAVE|wxFD_OVERWRITE_PROMPT ) ;
    d.SetFilterIndex ( myapp()->frame->LS->getOption ( _T("LAST_EXPORT_FILTER") , 0 ) ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;

    myapp()->frame->LS->setOption ( _T("LAST_EXPORT_FILTER") , d.GetFilterIndex() ) ;
    myapp()->frame->LS->setOption ( _T("LAST_IMPORT_DIR") , d.GetDirectory() ) ;
    doExport ( d.GetPath() , d.GetFilterIndex() ) ;
    }

wxString ChildBase::getExportFilters () const
    {
    wxString wcGenBank = _T("GenBank (*.gb)|*.gb") ;
    wxString wcFasta = _T("Fasta|*.*") ;
    wxString wcEMBL = _T("EMBL|*.*") ;
    wxString wcPlain = _T("Plain text|*.*") ;
    wxString wcIG = _T("IntelliGenetics format (*.ig)|*.ig") ;
    wxString wcClone = _T("CLONE|*.*") ;
    wxString wcGCviewXML = _T("CGview (XML)|*.xml") ;
    wxString wcCSV = _T("Comma-separated values (CSV)|*.csv") ;
    wxString wildcard = wcGenBank + "|" + wcPlain + "|" + wcFasta + "|" + wcEMBL + "|" + wcIG + "|" + wcGCviewXML + "|" + wcCSV ;
    return wildcard ;
    }

void ChildBase::doExport ( const wxString& filename , int filter )
    {
    wxFile out ( filename , wxFile::write ) ;
    exportVector ( vec , out , filter , filename ) ;
    out.Close () ;
    }

void ChildBase::exportVector ( TVector *vec , wxFile &out , int filter , const wxString& filename )
    {
    if ( filter == 0 ) // GeneBank
        {
        TGenBank gb ;
        wxArrayString ex ;
        gb.doExport ( vec , ex ) ;
        for ( unsigned int a = 0 ; a < ex.GetCount() ; a++ )
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
        arrangedExport ( out , _T("     ") , vec->getSequence().Lower() , 70 ) ;
        out.Write ( _T("//\n") ) ;
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
    else if ( filter == 5 ) // GCview XML
        {
        CGview cgv ( vec ) ;
        if ( !cgv.runSettingsDialog () ) return ; // Cancel
        out.Write ( cgv.getXML() ) ;
        cgv.postProcess ( filename ) ;
        }
    }

void ChildBase::arrangedExport ( wxFile &out , const wxString& n , const wxString& _s , int l )
    {
    int sl = l - n.length() ;
    wxString s (_s) ;
    wxString blank ( ' ' , n.length() ) ;
    wxString o(n);
    while ( !s.IsEmpty() )
       {
       out.Write ( o + s.Left ( sl ) + "\n" ) ;
       s = s.Mid ( sl ) ;
       o = blank ;
       }
    }

bool ChildBase::HasUndoData ()
     {
     return false ;
     }
