#include "PlasmidCanvas.h"
#include <wx/printdlg.h>
#include <wx/dcbuffer.h>

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
    EVT_MENU(PC_SAVE_IMAGE, PlasmidCanvas::OnSaveImage)
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
    EVT_MENU(STRAND_COPY_35,PlasmidCanvas::OnStrandCopy35)
    EVT_MENU(STRAND_COPY_53,PlasmidCanvas::OnStrandCopy53)
    EVT_MENU(STRAND_COPY_BOTH,PlasmidCanvas::OnStrandCopyBoth)
    EVT_MENU(STRAND_NEW_35,PlasmidCanvas::OnStrandNew35)
    EVT_MENU(STRAND_NEW_53,PlasmidCanvas::OnStrandNew53)
    EVT_MENU(STRAND_NEW_BOTH,PlasmidCanvas::OnStrandNewBoth)
    EVT_MENU(CM_OPEN_FEATURE,PlasmidCanvas::OnOpenFeature)
    EVT_MENU(PC_TURNING_POINT,PlasmidCanvas::OnTurningPoint)
//    EVT_PAINT(PlasmidCanvas::OnPaint)
END_EVENT_TABLE()

void PlasmidCanvas::OnPaint(wxPaintEvent& event)
    {
/*  wxPaintDC dc(this);
    dc.Clear () ;
    OnDraw ( dc ) ;*/
    }

void PlasmidCanvas::setPrinting ( const bool b )
    {
    printing = b ;
    //wxPrintf( "D: PlasmidCanvas::setPrinting ( %d )\n" , b ) ;
    }
void PlasmidCanvas::setLastContextItem ( const long _l ) { context_last_item = _l ; }
void PlasmidCanvas::getMark ( int &i1 , int &i2 ) const { i1 = getMarkFrom() ; i2 = getMarkTo() ; }
void PlasmidCanvas::setRootChild ( MyChild * const _p ) { p = _p ; }
int PlasmidCanvas::getZoom () const { return zoom ; }

int PlasmidCanvas::getMarkFrom () const
    {
    if ( p && p->cSequence ) return p->cSequence->markedFrom() ;
    return -1 ;
    }

int PlasmidCanvas::getMarkTo () const
    {
    if ( p && p->cSequence ) return p->cSequence->markedTo() ;
    return -1 ;
    }

void PlasmidCanvas::setMark ( const int i1 , const int i2 )
    {
    if ( getMarkFrom() == i1 && getMarkTo() == i2 ) return ;
    if ( !p || !p->cSequence ) return ;
    SeqBasic *seq = NULL ;
    if ( p->def == _T("dna") ) seq = p->cSequence->findID ( _T("DNA") ) ;
    else if ( p->def == _T("AminoAcids") ) seq = p->cSequence->findID ( _T("AA") ) ;
    if ( seq ) p->cSequence->mark ( seq->whatsthis() , i1 , i2 ) ;
    }



// Define a constructor for my canvas
PlasmidCanvas::PlasmidCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size, wxSUNKEN_BORDER|wxVSCROLL|wxHSCROLL|wxFULL_REPAINT_ON_RESIZE)
    {
    SetBackgroundColour(wxColour(_T("WHITE")));

    p = NULL ;
    zoom = 100 ;
    lastvectorobject = -1 ;
    lastrestrictionsite = -1 ;
    setMark ( -1 , -1 ) ;
    initialclick = false ;
    printing = false ;
    painting = false ;
    lastbp = -1 ;
    lasttooltip = -1 ;
    hasBeenPainted = false ;
    captured = false ;
    tt = new wxToolTip ( _T("") ) ;
    }

PlasmidCanvas::~PlasmidCanvas ()
    {
    }

void PlasmidCanvas::Refresh ()
    {
    //wxPrintf( "D: PlasmidCanvas::Refresh - start\n" ) ;
    if ( p && p->cSequence->getEditMode() && p->def == _T("DNA") ) return ;
//  if ( painting ) return ;
//  painting = true ;
    wxClientDC dc ( (wxWindow*) this ) ;
    PrepareDC ( dc ) ;
    OnDraw ( dc ) ;
//  painting = false ;
    //wxPrintf( "D: PlasmidCanvas::Refresh - end\n" ) ;
    }

bool PlasmidCanvas::isEnzymeVisible ( const wxString& s ) const
    {
    return !p->vec->isEnzymeHidden ( s ) ;
    }

bool PlasmidCanvas::intersects ( const wxRect &a , const wxRect &b ) const
    {
    if ( a.GetRight() < b.GetLeft() ) return false ;
    if ( a.GetLeft() > b.GetRight() ) return false ;
    if ( a.GetTop() > b.GetBottom() ) return false ;
    if ( a.GetBottom() < b.GetTop() ) return false ;
    return true ;
    }

// Define the repainting behaviour
void PlasmidCanvas::OnDraw(wxDC& pdc) /* not const */
{
    //wxPrintf( "D: PlasmidCanvas::OnDraw - start\n" ) ;
    if ( !p || !p->vec )
        {
        wxPrintf( "D: PlasmidCanvas::OnDraw - ret !p || !p->vec\n" ) ;
        return ;
        }
    if ( myapp()->frame->isLocked() )
        {
        wxPrintf( "D: PlasmidCanvas::OnDraw - ret locked\n" ) ;
        return ;
        }
    if ( !p->IsShown() )
        {
        wxPrintf( "D: PlasmidCanvas::OnDraw - ret !shown\n" ) ;
        return ;
        }
    if ( !p->IsEnabled() )
        {
        wxPrintf( "D: PlasmidCanvas::OnDraw - ret !enabled\n" ) ;
        return ;
        }
    if ( p->vec->getSequenceLength() == 0 )
        {
        pdc.Clear () ;
        wxPrintf( "D: PlasmidCanvas::OnDraw - ret 0==length\n" ) ;
        return ;
        }
    if ( printing )
        {
        hasBeenPainted = true ; // not const
        pdc.GetSize ( &w , &h ) ;
        pdc.Clear() ;
        if ( p->vec->isCircular() )
            {
            OnDrawCircular ( pdc ) ;
            }
        else
            {
            OnDrawLinear ( pdc ) ;
            }
        }
    else
        {
        if ( painting ) return ;
        painting = true ; // not const
        int vx , vy ;
        GetViewStart ( &vx , &vy ) ;
        GetClientSize(&w, &h);
        if ( w == 0 || h == 0 ) { painting = false ; pdc.Clear() ; return ; }
        hasBeenPainted = true ; // not const
        wxBitmap bmp ( w , h , -1 ) ;
        GetVirtualSize ( &w , &h ) ;
        if ( h < 20 )
            {
            pdc.Clear () ;
            painting = false ; // not const
            return ;
            }
/*
        wxBufferedDC dc ( &pdc , wxSize ( w , h ) ) ;
        dc.Clear () ;
        if ( p->vec->isCircular() ) OnDrawCircular ( dc ) ;
        else OnDrawLinear ( dc ) ;
*/
        wxMemoryDC dc ;
        dc.SelectObject ( bmp ) ;
        dc.Clear() ;
        dc.SetDeviceOrigin ( -vx , -vy ) ;
        if ( p->vec->isCircular() )
            {
            OnDrawCircular ( dc ) ;
            }
        else
            {
            OnDrawLinear ( dc ) ;
            }
        dc.SetDeviceOrigin ( 0 , 0 ) ;
        pdc.Blit ( vx , vy , w , h , &dc , 0 , 0 ) ;
        painting = false ;
        }
        //wxPrintf( "D: PlasmidCanvas::OnDraw - end\n" ) ;
}

void PlasmidCanvas::OnCopyImage ( wxCommandEvent& ev )
    {
    //wxPrintf( "D: PlasmidCanvas::OnCopyImage - start\n" ) ;

    if ( !p || !p->vec )
        {
        wxPrintf( "D: PlasmidCanvas::OnCopyImage - ret !p || !p->vec\n" ) ;
        return ;
        }

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
//         dc.SetUserScale ( 0.6 , 1 ) ;
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
    //wxPrintf( "D: PlasmidCanvas::OnCopyImage - end\n" ) ;
    }

void PlasmidCanvas::OnSaveImage ( wxCommandEvent& ev )
    {
    wxASSERT( p && p->vec ) ;
    w = 2000 ;
    h = 1600 ;
    wxBitmap bmp ( w , h , -1 ) ;
    wxMemoryDC dc ;
    dc.SelectObject ( bmp ) ;
    dc.Clear() ;
    if ( p->vec->isCircular() ) OnDrawCircular ( dc ) ;
    else OnDrawLinear ( dc ) ;

    myapp()->frame->saveImage ( &bmp , p->getName() ) ;

    GetClientSize(&w, &h);
    }

wxString PlasmidCanvas::getSelection() const
    {
    wxASSERT( p && p->cSequence ) ;
    if ( !p || !p->cSequence || p->cSequence->getEditMode() || p->def != _T("dna") ) return _T("") ;
    return p->cSequence->getSelection() ;
//    if ( getMarkFrom() == -1 ) return "" ;
//    return p->vec->getSubstring ( getMarkFrom() , getMarkTo() ) ;
    }

void PlasmidCanvas::OnEvent(wxMouseEvent& event)
    {
    if ( !p || !p->vec || !p->cSequence )
        {
        wxPrintf("E: PlasmidCanvas::OnEvent: No plasmid/vector/sequence.\n") ;
        SetCursor(*wxSTANDARD_CURSOR);
        return;
        }
    if ( !hasBeenPainted ) return ;
    if ( w == 0 || h == 0 ) return ;

    bool middledown = event.MiddleDown() ;
    if ( event.AltDown() && event.LeftDown() )
       {
       event.m_leftDown = false ;
       event.m_middleDown = true ;
       middledown = true ;
       }

    int mode ;
    if ( p->vec->isCircular() ) mode = MODE_CIRCULAR ;
    else mode = MODE_LINEAR ;

    wxClientDC dc(this);
    PrepareDC(dc);
    wxPoint pt(event.GetLogicalPosition(dc));
    wxPoint pt_abs(event.GetPosition());

    wxPoint pt2 , pto = pt ;
    int vo = -1 , rs = -1 , orf = -1 , lineOff = w/20 ;
    float angle = 0 , radius = 0 ;
    wxString s , id = _T("DNA") ;
    if ( p->def == _T("AminoAcids") ) id = _T("AA") ;

    // Preparations
    switch(mode)
        {
        case MODE_CIRCULAR:
            pt2.x = pt.x-w/2 ;
            pt2.y = pt.y-h/2 ;
            angle  = xy2deg ( pt2.x , pt2.y ) ;
            radius = xy2r ( pt2.x , pt2.y ) ;
            pt.x = deg2x ( angle , (int)radius ) + w/2 ;
            pt.y = deg2y ( angle , (int)radius ) + h/2 ;
            vo = findVectorObjectCircular ( angle , radius ) ;
            rs = findRestrictionSite ( pt.x , pt.y ) ;
            orf = findORFcircular ( angle , radius ) ;
            break;
        case MODE_LINEAR:
            vo = -1 ;
            rs = findRestrictionSite ( pt.x , pt.y ) ;
            orf = findORFlinear ( pt.x , pt.y ) ;
            pt2.x = pt.x * STANDARDRADIUS / w ;
            pt2.y = pt.y * STANDARDRADIUS / h ;
            vo = findVectorObjectLinear ( pt2 ) ;
            break;
        default:
            wxPrintf("E: Unknown mode: %d", mode ) ;
            return ;
            break;
        }

    // Capturing/releasing mouse for left click
    if ( event.LeftDown() )
        {
        CaptureMouse() ;
        captured = true ;
        initialclick = true ;
        mousediffx = mousediffy = 0 ;
        }
    else if ( event.LeftUp() )
        {
        lastbp = -1 ;
        initialclick = false ;
        if (captured)
            {
            ReleaseMouse() ;
            }
        else
            {
            // Upon some wild double-clicks, at times a release is attempted on a window that was not captured
            // This demands some further investigation.
            wxPrintf( "D: Prevented attempt to release a window that was not captured.\n" ) ;
            }
        captured = false ;
        }

    // Scroll wheel
    int wr = event.GetWheelDelta() ;
    if ( wr != 0 )
        {
        wr = event.GetWheelRotation() * event.GetLinesPerAction() * ( wr < 0 ? 1 : -1 ) * zoom * 4 / 100 ;
        }

    if ( wr != 0 && zoom != 100 )
        {
        int horiz = 0 ; //event.GetWheelAxis() ; // ACTIVATE FOR wxWidgets >= 2.9.0
        int wrx = horiz ? wr : 0 ;
        int wry = horiz ? 0 : wr ;
        int nx , ny ;
        GetViewStart ( &nx , &ny ) ;
        Scroll ( nx + wrx , ny + wry ) ;
        }

   // Moving canvas in zoom mode
   if ( event.CmdDown() && zoom != 100 )
       {
       lastrestrictionsite = rs = -1 ;
       lastvectorobject = vo = -1 ;
       orf = -1 ;
       SetCursor(wxCursor(wxCURSOR_SIZING)) ;
       if ( event.LeftIsDown() )
           {
           if ( initialclick )
               {
               mousediffx = pt.x ;
               mousediffy = pt.y ;
               initialclick = false ;
               }
           int nx , ny ;
           GetViewStart ( &nx , &ny ) ;
           Scroll ( nx + mousediffx - pt.x , ny + mousediffy - pt.y ) ;
           }
      }

    // Determining bp in linear mode
    int bp = -1 ;
    if ( mode == MODE_LINEAR && pt.x >= lineOff && pt.x <= w - lineOff )
        {
        bp = pt.x - lineOff ;
        bp = bp * ( p->vec->getSequenceLength() - 1 ) / ( w - lineOff * 2 ) + 1 ;
        }

    // Initializing
    if ( lastbp != -1 )
        {
        lastrestrictionsite = rs = -1 ;
        lastvectorobject = vo = -1 ;
        }

    // Passing over
    if ( rs != -1 ) // Passing over restriction site
        {
        wxASSERT(rs >= 0 && rs < p->vec->countRCs());
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        s = p->vec->rc[rs].e->getName() ;
        wxLogStatus(txt("rsite_status_bar") , s.c_str() ) ;
//      char ttt[1000] ;
//      sprintf ( ttt , txt("tt_rs") , s.c_str() , p->vec->countCuts ( s ) ) ;
        SetMyToolTip ( wxString::Format ( txt("tt_rs") , s.c_str() , p->vec->countCuts ( s ) ) , TT_RS ) ;
        if ( event.LeftDown() )
            {
#ifdef __WXMSW__
            if ( p->def == _T("dna") ) p->treeBox->SelectItem ( p->treeBox->GetParent ( ) ) ; // Frell
#endif
            }
        else if ( middledown )
            {
            p->runRestriction ( s );
//          if ( !p->runRestriction ( s ) )
                {
                p->vec->recalculateCuts() ;
                p->vec->updateDisplay() ;
                p->Refresh () ;
                p->cSequence->SetFocus() ;
                }
            return ;
            }
        else if ( event.RightDown() )
            invokeRsPopup ( rs , pt_abs ) ;
        else if ( event.LeftDClick() )
            invokeVectorEditor ( _T("enzyme") , rs ) ;
        }
    else if ( vo != -1 ) // Passing over feature
        {
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        s = p->vec->items[vo].name ;
        wxLogStatus(txt("item_status_bar") , s.c_str() ) ;
//      char ttt[1000] ;
//      sprintf ( ttt , _T("itemtype%d") , p->vec->items[vo].type ) ;
        wxString tt_type = txt(wxString::Format(_T("itemtype%d") , p->vec->items[vo].type)) ; //txt(ttt) ;
        wxString ttt2 = wxString::Format ( txt("tt_item") , tt_type.c_str() , s.c_str() , p->vec->items[vo].desc.c_str() ) ;
        while ( ttt2.Replace ( _T("\n") , _T(",") ) ) ;
        while ( ttt2.Replace ( _T("\r") , _T(" ") ) ) ;
        SetMyToolTip ( ttt2 , TT_ITEM ) ;

/*      sprintf ( ttt , txt("tt_item") , tt_type.c_str() , s.c_str() , p->vec->items[vo].desc.c_str() ) ;
        for ( int a = 0 ; ttt[a] ; a++ ) // Fixing multiline tooltip into single line
            {
            if ( ttt[a] == '\n' ) ttt[a] = ',' ;
            if ( ttt[a] == '\r' ) ttt[a] = ' ' ;
            }
        SetMyToolTip ( ttt , TT_ITEM ) ;*/

        if ( event.LeftDown() )
            {
            if ( p->def == _T("dna") )
                p->treeBox->SelectItem ( p->vec->items[vo].getTreeID() ) ;
            }
        else if ( event.RightDown() )
            invokeItemPopup ( vo , pt_abs ) ;
        else if ( event.LeftDClick() )
            invokeVectorEditor ( _T("item") , vo ) ;
        else if ( middledown )
            {
            wxCommandEvent dummyEvent ;
            context_last_item = vo ;
            int o_from = p->cSequence->markedFrom() ;
            int o_to = p->cSequence->markedTo() ;
            itemMarkShow ( dummyEvent ) ;
            if ( o_from != -1 && event.ShiftDown() && !p->cSequence->getEditMode() )
                 {
                 if ( p->cSequence->markedFrom() < o_from )
                     o_from = p->cSequence->markedFrom() ;
                 if ( p->cSequence->markedTo() > o_to )
                     o_to = p->cSequence->markedTo() ;
                 wxString id = p->cSequence->seq[p->cSequence->lastmarked]->whatsthis() ;
                 p->cSequence->mark ( id , o_from , o_to ) ;
                 }
            p->cSequence->SetFocus () ;
            }
        }
    else if ( orf != -1 )
        {
        wxString ttt = p->vec->getORF(orf)->getText() ;
        SetMyToolTip ( ttt , TT_ORF ) ;
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        wxLogStatus(txt("item_status_bar") , ttt.c_str() ) ;
        if ( event.LeftDClick() )
            p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;
        }
    else
        {
        SetMyToolTip ( _T("") , TT_NONE ) ;
        if ( !event.CmdDown() ) SetCursor(*wxSTANDARD_CURSOR) ;
        if ( mode == MODE_CIRCULAR )
            {
            wxLogStatus(txt("bp"), int(circular_pos ( angle )) ) ;
            }
        else if ( mode == MODE_LINEAR )
            {
            if ( bp != -1 ) wxLogStatus(txt("bp"), bp ) ;
            }
        if ( event.LeftDClick() ) invokeVectorEditor () ;
        else if ( event.RightDown() ) invokeVectorPopup ( pt_abs ) ;
        }

    // Dragging
    if ( event.Dragging() && event.LeftIsDown() && lastrestrictionsite != -1 )
        { // Dragging restriction site
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        rs = lastrestrictionsite ;
        wxRect lr = p->vec->rc[rs].lastrect ;
        lastvectorobject = -1 ;
        wxPoint q1 ;
        if ( mode == MODE_CIRCULAR )
            {
            q1 = pto ;
            q1.x -= w/2 ;
            q1.y -= h/2 ;
            q1.x = q1.x * ( STANDARDRADIUS * 100 ) / ( 100 * r ) ;
            q1.y = q1.y * ( STANDARDRADIUS * 100 ) / ( 100 * r ) ;
            }
        else if ( mode == MODE_LINEAR )
            {
            q1 = wxPoint ( lr.GetRight() , lr.GetBottom() ) ;
            q1.x -= ( lr.GetLeft() + lr.GetRight() ) / 2 - pt.x ;
            q1.y -= ( lr.GetTop() + lr.GetBottom() ) / 2 - pt.y ;
            q1.x = q1.x * STANDARDRADIUS / w ;
            q1.y = q1.y * STANDARDRADIUS / h ;
            }

        if ( initialclick )
           {
           mousediffx = p->vec->rc[rs].lp.x - q1.x ;
           mousediffy = p->vec->rc[rs].lp.y - q1.y ;
           initialclick = false ;
           }
        q1.x += mousediffx ;
        q1.y += mousediffy ;
        p->vec->rc[rs].lp = q1 ;
        if ( mode == MODE_LINEAR ) p->vec->rc[rs].linearUpdate ( w , h ) ;
        Refresh () ;
        }
    else if ( event.Dragging() && event.LeftIsDown() && lastvectorobject != -1 )
        { // Dragging vector item
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        vo = lastvectorobject ;
        lastrestrictionsite = -1 ;
        if ( mode == MODE_CIRCULAR )
            {
            int dr = p->vec->items[vo].r2 - p->vec->items[vo].r1 ;
            float r1 = STANDARDRADIUS * radius / r - p->vec->items[vo].r1 ;
            p->vec->items[vo].r1 += int ( r1 ) ;
            p->vec->items[vo].r2 = p->vec->items[vo].r1 + dr ;
            }
       else if ( mode == MODE_LINEAR )
            {
            float hh = p->vec->items[vo].a2 - p->vec->items[vo].a1 ;
            p->vec->items[vo].a1 = pt2.y - hh/2 ;
            p->vec->items[vo].a2 = p->vec->items[vo].a1 + hh ;
            }
       updateLinkedItems ( p->vec , vo ) ;
       Refresh () ;
       }
/*    else if ( event.LeftIsDown() && event.RightIsDown() ) // Turning disabeled
       {
       int f = (pto.x-w/2)/10 ;
       p->vec->turn ( f ) ;
       Refresh () ;
       p->cSequence->arrange () ;
       p->cSequence->Refresh () ;
       }*/
    else if ( event.LeftIsDown() && orf != -1 )
        {
        p->cSequence->mark ( id ,
                                p->vec->getORF(orf)->get_from() + 1 ,
                                p->vec->getORF(orf)->get_to() + 1 ) ;
        }
    else if ( event.RightDown() && orf != -1 )
        {
        p->cSequence->mark ( id ,
                                p->vec->getORF(orf)->get_from() + 1 ,
                                p->vec->getORF(orf)->get_to() + 1 ) ;
        invokeORFpopup ( orf , pt_abs ) ;
        }
    else if ( event.LeftIsDown() && rs == -1 && vo == -1 && orf == -1 )
        {
        if ( mode == MODE_CIRCULAR )
           {
           float bp = circular_pos ( angle ) ;
           if ( lastbp == -1 && initialclick ) lastbp = int(bp) ;
           if ( lastbp != -1 )
              {
              if ( lastbp != int(bp) )
                  p->cSequence->mark ( id , lastbp , int(bp) ) ;
              else
                  p->cSequence->unmark () ;
              }
           }
        else if ( mode == MODE_LINEAR )
           {
           if ( initialclick && lastbp == -1 ) lastbp = bp ;
           if ( lastbp != -1 )
              {
              if ( lastbp > bp ) p->cSequence->mark ( id , bp , lastbp ) ;
              else p->cSequence->mark ( id , lastbp , bp ) ;
              }
           }
        }
    else if ( middledown && p->cSequence->markedFrom() != -1 )
        {
        p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;
        }
    else if ( p->cSequence->markedFrom() == -1 && ( middledown || ( event.Dragging() && middledown ) ) )
        {
        if ( mode == MODE_CIRCULAR ) bp = circular_pos ( angle ) ;
        p->cSequence->mark ( id , bp , bp ) ;
        p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;
        p->cSequence->unmark () ;
        }
    else
        { // If nothing else...
        lastrestrictionsite = rs ;
        lastvectorobject = vo ;
        }
    }

void PlasmidCanvas::updateLinkedItems ( TVector * const vec , const int in )
    {
    TVectorItem *i , *orig = & ( vec->items[in] ) ;
    i = orig ;
    wxASSERT(i);
    wxString s = i->getParam ( _T("PREDECESSOR") ) ;
    while ( !s.IsEmpty() )
        {
        int cur = vec->find_item( s ) ;
        if ( cur == -1 ) return ; // Not found
        i = & ( vec->items[cur] ) ;
        s = i->getParam ( _T("PREDECESSOR") ) ;
        }
    if ( i->getParam ( _T("SUCCESSOR") ).IsEmpty() ) return ;
    do {
        s = i->getParam ( _T("SUCCESSOR") ) ;
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
        int cur = vec->find_item( s ) ;
        if ( cur == -1 ) return ; // Not found
        i = &vec->items[cur] ;
        } while ( !s.IsEmpty() ) ;
    }

void PlasmidCanvas::SetMyToolTip ( const wxString& s , const int mode ) /* not const */
    {
    if ( lasttooltip == mode && tt->GetTip() == s ) return ;
    lasttooltip = mode ;

    SetToolTip ( NULL ) ;

    tt = new wxToolTip ( _T("") ) ;
    tt->Enable ( false ) ;
    if ( myapp()->frame->showToolTips ) SetToolTip ( tt ) ;

    if ( mode == TT_NONE )
        {
        return ;
        }

    SetToolTip ( NULL ) ;
    tt = new wxToolTip ( s ) ;
    tt->Enable ( true ) ;
    if ( myapp()->frame->showToolTips ) SetToolTip ( tt ) ;
    }

int PlasmidCanvas::findVectorObjectLinear ( const wxPoint& pp ) const
    {
    wxASSERT( p && p->vec ) ;
    int vo = -1 ;
    for ( int a = 0 ; a < p->vec->items.size() ; a++ )
        {
        const TVectorItem i = p->vec->items[a] ;
        const wxRect rra ( (int)i.r1 , (int)i.a1 , (int)(i.r2-i.r1) , (int)(i.a2-i.a1) ) ;
        const wxRect rrb ( (int)i.r3 , (int)i.a1 , (int)(i.r4-i.r3) , (int)(i.a2-i.a1) ) ;
        if ( pointinrect ( pp.x , pp.y , rra ) && p->vec->items[a].isVisible() ) vo = a ;
        if ( i.r3 != -1 && pointinrect ( pp.x , pp.y , rrb ) ) vo = a ;
        }
    return vo ;
    }

int PlasmidCanvas::findVectorObjectCircular ( const float& angle , const float& _radius ) const
    {
    wxASSERT( p && p->vec ) ;
    float radius = STANDARDRADIUS * _radius / r ;
    for ( int a = p->vec->items.size() -1 ; a >= 0 ; a-- )
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

int PlasmidCanvas::findRestrictionSite ( const int x , const int y ) const
    {
    wxASSERT( p && p->vec ) ;
    for ( int a = 0 ; a < p->vec->rc.size() ; a++ )
        {
        if ( pointinrect ( x , y , p->vec->rc[a].lastrect ) )
            {
            return a ;
            }
        }
    return -1 ;
    }

bool PlasmidCanvas::pointinrect ( const int x , const int y , const wxRect &a ) const
    {
    if ( x >= a.GetLeft() && x <= a.GetRight() && y >= a.GetTop() && y <= a.GetBottom() )
        {
        return true ;
        }
    return false ;
    }

void PlasmidCanvas::invokeVectorEditor ( const wxString& what , const int num , const bool forceUpdate )
    {
    wxASSERT( p && p->vec && p->cSequence ) ;
    if ( p->def == _T("AminoAcids") )
        {
        wxCommandEvent ce ;
        p->cSequence->vecEdit ( ce ) ;
        return ;
        }
    p->vec->undo.start ( txt("u_vec_edit") ) ;
    TVectorEditor ve ( this , txt("t_vector_editor") , p->vec ) ;
    bool changed = p->vec->isChanged() ;
    p->vec->setChanged ( false ) ;

    if ( what == _T("item") )
        {
        ve.initialViewItem ( num ) ;
        }
    else if ( what == _T("enzyme") )
        {
        ve.initialViewEnzyme ( p->vec->rc[num].e->getName() ) ;
        }

    ve.ShowModal () ;
    ve.cleanup () ;
    if ( forceUpdate || p->vec->isChanged() )
        {
        myapp()->frame->mainTree->SetItemText ( p->inMainTree , p->getName() ) ;
        p->treeBox->initme() ;
        p->showName() ;
        p->treeBox->SelectItem ( p->treeBox->vroot ) ;
        p->updateSequenceCanvas() ;
        Refresh () ;
        p->vec->undo.stop() ;
        }
    else p->vec->undo.abort() ;
    p->vec->setChanged ( changed | p->vec->isChanged() ) ;
    if ( p->cSequence ) p->cSequence->SetFocus() ;
    }

void PlasmidCanvas::print ()
    {
    wxASSERT( p && p->vec ) ;
    //wxPrintf( "D: PlasmidCanvas::print - start\n" ) ;
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;

    //wxPrintf( "D: PlasmidCanvas::print A\n" ) ;
    wxDC *pdc = pd.GetPrintDC () ;
    if ( !pdc )
        {
        wxPrintf("E: PlasmidCanvas::print: No print DC available.\n");
        return ;
        }
    pdc->StartDoc ( p->vec->getName() ) ;
    pdc->StartPage () ;
    printing = true ;
    //wxPrintf( "D: PlasmidCanvas::print B\n" ) ;
    OnDraw ( *pdc ) ;
    //wxPrintf( "D: PlasmidCanvas::print C\n" ) ;
    printing = false ;
    pdc->EndPage () ;
    pdc->EndDoc () ;
    //wxPrintf( "D: PlasmidCanvas::print - end\n" ) ;
    }

void PlasmidCanvas::makeGCcolor ( const int percent , wxColour &col ) const
    {
    int red , green , blue ;
    if ( percent < 50 )
        {
        red = 255 * ( 50 - percent ) / 50 ; // > 0 since percent<50
        green = 255 * percent / 50 ;
        blue = 0 ;
        }
     else
        {
        red = 0 ;
        green = 255 * ( 50 - (percent-50) ) / 50 ;
        blue = 255 * (percent-50) / 50 ;
        }
    col.Set ( red , green , blue ) ;
    }

void PlasmidCanvas::showGClegend ( wxDC &dc ) const
    {
    wxASSERT( p && p->vec ) ;
    int fontfactor = 10 ;
    if ( printing ) fontfactor = (w>h?h:w)/70 ;
    wxFont smallFont ( wxFontInfo ( fontfactor*2/3 ).Family( wxFONTFAMILY_SWISS ).Style( wxFONTSTYLE_NORMAL ).Weight( wxFONTWEIGHT_NORMAL ) ) ;
    wxFont normalFont( wxFontInfo ( fontfactor*6/5 ).Family( wxFONTFAMILY_SWISS ).Style( wxFONTSTYLE_NORMAL ).Weight( wxFONTWEIGHT_NORMAL ) ) ;
    int nw = w / 5 ;
    dc.SetFont( normalFont );
    wxString t = wxString::Format ( txt("t_gc_blocks") , p->vec->getSequenceLength() / p->vec->showGC() ) ;
    int tw , th ;
    dc.GetTextExtent ( t , &tw , &th ) ;

    wxRect r ;
    if ( tw * 11 / 10 > nw ) nw = tw ;
    if ( p->vec->isLinear() )
        {
        r = wxRect ( w/2 - nw/2 , h / 40 , nw , th*5/2 ) ;
        }
    else
        {
        r = wxRect ( w/40 , h - h/40 - th*5/2 , nw , th*5/2 ) ;
        }

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle ( r ) ;
    dc.SetTextForeground ( *wxBLACK ) ;
    dc.DrawText ( t , r.GetLeft() + th/10 , r.GetTop() + th/10 ) ;
    dc.SetFont( smallFont );
    for ( int a = 0 ; a < 11 ; a++ )
        {
        wxColour col ;
        makeGCcolor ( a * 10 , col ) ;
        dc.SetBrush ( *MYBRUSH(col) ) ;
        int nw2 = nw / 11 ;
        dc.DrawRectangle ( r.GetLeft() + nw2 * a + nw2/5 , r.GetBottom() - nw2*9/10 , nw2*8/10 , nw2*8/10 ) ;
        t = wxString::Format ( _T("%d") , a * 10 ) ;
        dc.GetTextExtent ( t , &tw , &th ) ;
        dc.DrawText ( t , r.GetLeft() + nw2 * a + nw2/2 - tw/2 , r.GetBottom() - nw2*9/10 - th ) ;
        }
    }

