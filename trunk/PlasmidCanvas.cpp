#include "PlasmidCanvas.h"
#include <wx/printdlg.h>

// ---------------------------------------------------------------------------
// PlasmidCanvas
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PlasmidCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(PlasmidCanvas::OnEvent)
    EVT_MENU(PC_ITEM_MARK, PlasmidCanvas::itemMark)
    EVT_MENU(PC_ITEM_MARK_SHOW, PlasmidCanvas::itemMarkShow)
    EVT_MENU(PC_ITEM_EDIT, PlasmidCanvas::itemEdit)
    EVT_MENU(PC_ITEM_AS_NEW_SEQUENCE, PlasmidCanvas::itemAsNewSequence)
    EVT_MENU(PC_ITEM_DELETE, PlasmidCanvas::itemDelete)
    EVT_MENU(PC_ITEM_HIDE, PlasmidCanvas::itemShowHide)
    EVT_MENU(PC_ITEM_SHOW, PlasmidCanvas::itemShowHide)
    EVT_MENU(PC_ITEM_COPY_DNA, PlasmidCanvas::itemCopyDNA)
    EVT_MENU(PC_ITEM_COPY_AA, PlasmidCanvas::itemCopyAA)
    EVT_MENU(PC_ITEM_AS_NEW_AA_SEQUENCE, PlasmidCanvas::itemAsNewAA)
    EVT_MENU(PC_RS_INFO, PlasmidCanvas::rsInfo)
    EVT_MENU(PC_RS_EDIT, PlasmidCanvas::rsEdit)
    EVT_MENU(PC_RS_DEL, PlasmidCanvas::rsDel)
    EVT_MENU(PC_RS_SHOW_HIDE, PlasmidCanvas::rsShowHide)
    EVT_MENU(PC_RS_HIDE_LIMIT, PlasmidCanvas::rsHideLimit)
    EVT_MENU(PC_RS_MARK, PlasmidCanvas::rsMark)
    EVT_MENU(PC_RS_MARK_SHOW, PlasmidCanvas::rsMarkShow)
    EVT_MENU(PC_RS_ADD2COCKTAIL, PlasmidCanvas::rsAdd2Cocktail)
    EVT_MENU(PC_RS_ADD_AND_CUT, PlasmidCanvas::rsAddAndCut)
    EVT_MENU(PC_RS_CUT_WITH_COCKTAIL, PlasmidCanvas::rsCutWithCocktail)
    EVT_MENU(PC_VECTOR_EDIT, PlasmidCanvas::vecEdit)
    EVT_MENU(PC_BLAST_DNA, PlasmidCanvas::blastDNA)
    EVT_MENU(PC_BLAST_AA, PlasmidCanvas::blastAA)
    EVT_MENU(PC_COPY_IMAGE, PlasmidCanvas::OnCopyImage)
    EVT_MENU(PC_ITEM_BLAST_DNA, PlasmidCanvas::itemBlastDNA)
    EVT_MENU(PC_ITEM_BLAST_AA, PlasmidCanvas::itemBlastAA)
    EVT_MENU(PRIMER_FORWARD, PlasmidCanvas::OnPrimerForward)
    EVT_MENU(PRIMER_BACKWARD, PlasmidCanvas::OnPrimerBackward)
    EVT_MENU(PRIMER_BOTH, PlasmidCanvas::OnPrimerBoth)
    EVT_MENU(PRIMER_MUTATION, PlasmidCanvas::OnPrimerMutation)
    EVT_MENU(ORF_COPY_DNA, PlasmidCanvas::orfCopyDNA)
    EVT_MENU(ORF_COPY_AA, PlasmidCanvas::orfCopyAA)
    EVT_MENU(ORF_AS_NEW_ITEM, PlasmidCanvas::orfAsNewItem)
    EVT_MENU(ORF_AS_NEW_DNA, PlasmidCanvas::orfAsNewDNA)
    EVT_MENU(ORF_AS_NEW_AA, PlasmidCanvas::orfAsNewAA)
    EVT_MENU(ORF_BLAST_DNA, PlasmidCanvas::orfBlastDNA)
    EVT_MENU(ORF_BLAST_AA, PlasmidCanvas::orfBlastAA)
    EVT_MENU(MDI_FILL_KLENOW,PlasmidCanvas::OnFillKlenow)
    EVT_MENU(MDI_RUN_PCR,PlasmidCanvas::OnRunPCR)
    EVT_MENU(PC_WHAT_CUTS,PlasmidCanvas::OnWhatCuts)
END_EVENT_TABLE()


// Define a constructor for my canvas
PlasmidCanvas::PlasmidCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size,
                           wxSUNKEN_BORDER|wxVSCROLL|wxHSCROLL)
{
    SetBackgroundColour(wxColour("WHITE"));

    p = NULL ;
    zoom = 100 ;
    lastvectorobject = -1 ;
    lastrestrictionsite = -1 ;
    setMarkFrom ( -1 ) ;
    initialclick = false ;
    printing = false ;
    painting = false ;
    lastbp = -1 ;
    lasttooltip = -1 ;
    hasBeenPainted = false ;
    tt = new wxToolTip ( "" ) ;
}

PlasmidCanvas::~PlasmidCanvas ()
    {
    }

void PlasmidCanvas::Refresh ()
    {
    if ( p->cSequence->getEditMode() && p->def == "DNA" ) return ;
    if ( painting ) return ;
    painting = true ;
    wxClientDC dc ( (wxWindow*) this ) ;
    PrepareDC ( dc ) ;
    OnDraw ( dc ) ;
    painting = false ;
    }
    
bool PlasmidCanvas::isEnzymeVisible ( wxString s )
    {
    return !p->vec->isEnzymeHidden ( s ) ;
    }
    
bool PlasmidCanvas::intersects ( wxRect &a , wxRect &b )
    {
    if ( a.GetRight() < b.GetLeft() ) return false ;
    if ( a.GetLeft() > b.GetRight() ) return false ;
    if ( a.GetTop() > b.GetBottom() ) return false ;
    if ( a.GetBottom() < b.GetTop() ) return false ;
    return true ;
    }

// Define the repainting behaviour
void PlasmidCanvas::OnDraw(wxDC& pdc)
{
    if ( !p || !p->vec ) return ;
    if ( p->vec->getSequenceLength() == 0 ) return ;
    if ( printing )
        {
        hasBeenPainted = true ;
        pdc.GetSize ( &w , &h ) ;
        pdc.Clear() ;
        if ( p->vec->isCircular() ) OnDrawCircular ( pdc ) ;
        else OnDrawLinear ( pdc ) ;
        }
    else
        {
        int vx , vy ;
        GetViewStart ( &vx , &vy ) ;
        GetClientSize(&w, &h);
        if ( w == 0 || h == 0 ) return ;
        hasBeenPainted = true ;
        wxBitmap bmp ( w , h , -1 ) ;
        GetVirtualSize ( &w , &h ) ;
        wxMemoryDC dc ;
        dc.SelectObject ( bmp ) ;
        dc.Clear() ;
        dc.SetDeviceOrigin ( -vx , -vy ) ;
        if ( p->vec->isCircular() ) OnDrawCircular ( dc ) ;
        else OnDrawLinear ( dc ) ;
        dc.SetDeviceOrigin ( 0 , 0 ) ;
        pdc.Blit ( vx , vy , w , h , &dc , 0 , 0 ) ;
        }
}

void PlasmidCanvas::OnCopyImage ( wxCommandEvent &ev )
    {
    if ( !p || !p->vec ) return ;

// Metafile support only exists for windows, so...
#ifdef __WXMSW__
    if ( myapp()->frame->useMetafile ) // Use metafile DC
        {
        GetClientSize(&w, &h);
        wxMetafileDC dc ;

        if ( p->vec->isCircular() )
           {
           dc.SetUserScale ( (float) h/w*10 , 10 ) ;
           OnDrawCircular ( dc ) ;
           }
        else
           {
//           dc.SetUserScale ( 0.6 , 1 ) ;
           OnDrawLinear ( dc ) ;
           }

        wxMetafile *mf = dc.Close();
        if (mf)
        {
          bool success = mf->SetClipboard((int)(dc.MaxX() + 10), (int)(dc.MaxY() + 10));
          delete mf;
        }
        
        }
    else
#endif
        {
        w = 1000 ;
        h = 800 ;
        wxBitmap bmp ( w , h , -1 ) ;
        wxMemoryDC dc ;
        dc.SelectObject ( bmp ) ;
        dc.Clear() ;
        if ( p->vec->isCircular() ) OnDrawCircular ( dc ) ;
        else OnDrawLinear ( dc ) ;
        
        if (wxTheClipboard->Open())
            {
            wxTheClipboard->SetData( new wxBitmapDataObject(bmp) );
            wxTheClipboard->Close();
            }
    
        GetClientSize(&w, &h);
        }
    }


string PlasmidCanvas::getSelection()
    {
    if ( getMarkFrom() != -1 ) return p->vec->getSubstring ( getMarkFrom() , getMarkTo() ) ;
    else return "" ;
    }
    

void PlasmidCanvas::OnEvent(wxMouseEvent& event)
{
    if ( !p || !p->vec ) return ;
    if ( !hasBeenPainted ) return ;
    if ( p->vec->isCircular() ) OnEventCircular ( event ) ;
    else OnEventLinear ( event ) ;
}

void PlasmidCanvas::updateLinkedItems ( TVector *vec , int in )
    {
    TVectorItem *i , *orig = &vec->items[in] ;
    i = orig ;
    wxString s ;
    int cur ;
    s = i->getParam ( "PREDECESSOR" ) ;
    while ( s != "" )
        {
        cur = vec->find_item( s ) ;
        if ( cur == -1 ) return ; // Not found
        i = &vec->items[cur] ;
        s = i->getParam ( "PREDECESSOR" ) ;
        }
    if ( i->getParam ("SUCCESSOR" ) == "" ) return ;
    do {
        s = i->getParam ("SUCCESSOR" ) ;
        if ( vec->isLinear() )
           {
           i->a1 = orig->a1 ;
           i->a2 = orig->a2 ;
           }
        else
           {
           i->r1 = orig->r1 ;
           i->r2 = orig->r2 ;
           }
        cur = vec->find_item( s ) ;
        if ( cur == -1 ) return ; // Not found
        i = &vec->items[cur] ;        
        } while ( s != "" ) ;
    }

void PlasmidCanvas::SetMyToolTip ( string s , int mode )
    {
    if ( lasttooltip == mode && tt->GetTip() == s.c_str() ) return ;
    lasttooltip = mode ;

    SetToolTip ( NULL ) ;

    tt = new wxToolTip ( "" ) ;
    tt->Enable ( false ) ;
    SetToolTip ( tt ) ;
    
    if ( mode == TT_NONE )
        {
        return ;
        }
    
    SetToolTip ( NULL ) ;
    tt = new wxToolTip ( s.c_str() ) ;
    tt->Enable ( true ) ;
    SetToolTip ( tt ) ;
    }

int PlasmidCanvas::findVectorObject ( float angle , float radius )
    {
    int a ;
    radius = STANDARDRADIUS*radius/r ;
    for ( a = p->vec->items.size() -1 ; a >= 0 ; a-- )
        {
        float a2 = 0 ;
        if ( p->vec->items[a].a2 > 360 )
           a2 = 360 ;
        if ( p->vec->items[a].isVisible() &&
             radius >= p->vec->items[a].r1 &&
             radius <= p->vec->items[a].r2 )
             {
             if ( ( angle >= p->vec->items[a].a1 && angle <= p->vec->items[a].a2 ) || 
                  ( angle+a2 >= p->vec->items[a].a1 && angle+a2 <= p->vec->items[a].a2 ) )
              {
              return a ;
              }
           }
        }
    return -1 ;
    }
    
int PlasmidCanvas::findRestrictionSite ( int x , int y )
    {
    int a ;
    for ( a = 0 ; a < p->vec->rc.size() ; a++ )
        {
        if ( pointinrect ( x , y , p->vec->rc[a].lastrect ) )
             return a ;
        }
    return -1 ;
    }

bool PlasmidCanvas::pointinrect ( int x , int y , wxRect &a )
    {
    if ( x >= a.GetLeft() &&
         x <= a.GetRight() &&
         y >= a.GetTop() &&
         y <= a.GetBottom() )
         return true ;
    return false ;
    }
    
void PlasmidCanvas::invokeVectorEditor ( string what , int num , bool forceUpdate )
    {
    if ( p->def == "AminoAcids" )
       {
       wxCommandEvent ce ;
       p->cSequence->vecEdit ( ce ) ;
       return ;
       }
    p->vec->undo.start ( txt("u_vec_edit") ) ;
    TVectorEditor ve ( this , txt("t_vector_editor") , p->vec ) ;
    bool changed = p->vec->isChanged() ;
    p->vec->setChanged ( false ) ;
    
    if ( what == "item" )
        {
        ve.initialViewItem ( num ) ;
        }
    else if ( what == "enzyme" )
        {
        ve.initialViewEnzyme ( p->vec->rc[num].e->name ) ;
        }

    int x = ve.ShowModal () ;
    ve.cleanup () ;
    if ( forceUpdate || p->vec->isChanged() )
        {
        myapp()->frame->mainTree->SetItemText ( p->inMainTree , p->getName().c_str() ) ;
        p->treeBox->initme() ;
        p->showName() ;
        p->treeBox->SelectItem ( p->treeBox->vroot ) ;
        p->updateSequenceCanvas() ;
        Refresh () ;
        p->vec->undo.stop() ;
        }
    else p->vec->undo.abort() ;
    p->vec->setChanged ( changed | p->vec->isChanged() ) ;
    }
    
void PlasmidCanvas::print ()
    {
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;

    wxDC *pdc = pd.GetPrintDC () ;
    pdc->StartDoc ( p->vec->getName().c_str() ) ;
    pdc->StartPage () ;
    printing = true ;
    OnDraw ( *pdc ) ;
    printing = false ;
    pdc->EndPage () ;
    pdc->EndDoc () ;
    }
    

