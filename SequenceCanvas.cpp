#include "SequenceCanvas.h"
#include <wx/printdlg.h>
#include <wx/datetime.h>
#include <wx/choicdlg.h>

#define SCROLL_TO_END (-100)

// ---------------------------------------------------------------------------
// SequenceCanvas
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SequenceCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(SequenceCanvas::OnEvent)
    EVT_SIZE(SequenceCanvas::OnSize)
    EVT_CHAR(SequenceCanvas::OnCharHook)

    EVT_MENU(PC_VECTOR_EDIT, SequenceCanvas::vecEdit)
    EVT_MENU(PC_BLAST_DNA, SequenceCanvas::blastDNA)
    EVT_MENU(PC_BLAST_AA, SequenceCanvas::blastAA)

    EVT_MENU(SEQ_COPY_TEXT, SequenceCanvas::OnCopyText)
    EVT_MENU(SEQ_COPY_IMAGE, SequenceCanvas::OnCopyImage)
    EVT_MENU(SEQ_SAVE_IMAGE, SequenceCanvas::OnSaveImage)
    EVT_MENU(SEQ_PRINT, SequenceCanvas::OnPrint)

    EVT_MENU(SEQ_COPY_RESLUT_DNA, SequenceCanvas::OnCopyResultDNA)
    EVT_MENU(SEQ_COPY_RESLUT_AA, SequenceCanvas::OnCopyResultAA)
    EVT_MENU(SEQ_NEW_FROM_RESLUT_DNA, SequenceCanvas::OnNewFromResultDNA)
    EVT_MENU(SEQ_NEW_FROM_RESLUT_AA, SequenceCanvas::OnNewFromResultAA)
    EVT_MENU(PC_WHAT_CUTS,SequenceCanvas::OnWhatCuts)

    EVT_MENU(SEQ_AA_VIEW_ORIGINAL,SequenceCanvas::OnViewOriginal)
    EVT_MENU(SEQ_AA_VIEW_REAL,SequenceCanvas::OnViewReal)
    EVT_MENU(SEQ_AA_VIEW_CONDENSED,SequenceCanvas::OnViewCondensed)
    EVT_MENU(SEQ_AA_BACKTRANSLATE,SequenceCanvas::OnBacktranslate)
    EVT_MENU(SEQ_INSERT_RESTRICTION_SITE_LEFT,SequenceCanvas::OnInsertRestrictionSiteLeft)
    EVT_MENU(SEQ_INSERT_RESTRICTION_SITE_RIGHT,SequenceCanvas::OnInsertRestrictionSiteRight)

    EVT_MENU(SEQ_UP,SequenceCanvas::OnSeqUp)
    EVT_MENU(SEQ_DOWN,SequenceCanvas::OnSeqDown)
    EVT_MENU(SEQ_TOP,SequenceCanvas::OnSeqTop)
    EVT_MENU(SEQ_BOTTOM,SequenceCanvas::OnSeqBottom)
    EVT_MENU(SEQ_FEAT_HIDE,SequenceCanvas::OnToggleFeat)
    EVT_MENU(SEQ_FEAT_SHOW,SequenceCanvas::OnToggleFeat)

    EVT_MENU(SEQ_INSERT_GAP,SequenceCanvas::OnInsertGap)
    EVT_MENU(SEQ_DELETE_GAP,SequenceCanvas::OnDeleteGap)
    EVT_MENU(SEQ_INSERT_OTHER_GAPS,SequenceCanvas::OnInsertOtherGaps)
    EVT_MENU(SEQ_DELETE_OTHER_GAPS,SequenceCanvas::OnDeleteOtherGaps)

    EVT_MENU(PRIMER_FORWARD, SequenceCanvas::OnPrimerForward)
    EVT_MENU(PRIMER_BACKWARD, SequenceCanvas::OnPrimerBackward)
    EVT_MENU(PRIMER_BOTH, SequenceCanvas::OnPrimerBoth)
    EVT_MENU(PRIMER_MUTATION, SequenceCanvas::OnPrimerMutation)
    
    EVT_MENU(STRAND_COPY_35,SequenceCanvas::OnStrandCopy35)
    EVT_MENU(STRAND_COPY_53,SequenceCanvas::OnStrandCopy53)
    EVT_MENU(STRAND_COPY_BOTH,SequenceCanvas::OnStrandCopyBoth)
    EVT_MENU(STRAND_NEW_35,SequenceCanvas::OnStrandNew35)
    EVT_MENU(STRAND_NEW_53,SequenceCanvas::OnStrandNew53)
    EVT_MENU(STRAND_NEW_BOTH,SequenceCanvas::OnStrandNewBoth)

    EVT_MENU(MDI_CUT, SequenceCanvas::OnCut)
    EVT_MENU(MDI_COPY, SequenceCanvas::OnCopy)
    EVT_MENU(MDI_SEL_AS_NEW_PRIMER, SequenceCanvas::OnSelAsNewPrimer)

    EVT_MENU(PC_RS_HIDE_LIMIT, SequenceCanvas::rsHideLimit)
    EVT_MENU(MDI_PASTE, SequenceCanvas::OnPaste)

    EVT_SET_FOCUS(SequenceCanvas::OnFocus)
    EVT_KILL_FOCUS(SequenceCanvas::OnKillFocus)
END_EVENT_TABLE()

// Define a constructor for my canvas
SequenceCanvas::SequenceCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size, wxSUNKEN_BORDER|wxHSCROLL|wxFULL_REPAINT_ON_RESIZE)
{
    SetBackgroundColour(wxColour("WHITE"));

    font = MYFONT ( 12 , wxMODERN , wxNORMAL , wxNORMAL ) ; 
    smallFont = MYFONT ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    varFont = MYFONT ( 11 , wxROMAN  , wxNORMAL , wxNORMAL ) ;
    charwidth = 0 ;
    charheight = 0 ;
    blankline = 0 ;
    lastpos = -1 ;
    lastmarked = -1 ;
    p = NULL ;
    doHide ( false ) ;
    forceOverwrite ( false ) ;
    wantOverwrite = false ;
    maxendnumberlength = 9 ;
    lastyoffset = 0 ;
    blocksize = 10 ;
    lowx = lowy = -1 ;
    marking = false ;
    drawing = false ;
    hardstop = -1 ;
    border = 4 ;

    setMiniDisplay ( false ) ;
    editMode = false ;
    edit_id = "DNA" ;
    edit_valid = "ACTG" ;
    _from = -1 ;
    setDrawAll ( false ) ;
    printing = false ;
    print_dc = NULL ;
    child = NULL ;   
    setHorizontal ( false ) ; 
    }
    
SequenceCanvas::~SequenceCanvas()
    {
    CLEAR_DELETE ( seq ) ;
    }
    
void SequenceCanvas::unmark ()
	{
	if ( lastmarked < 0 || lastmarked >= seq.GetCount() ) return ;
	if ( markedFrom() == -1 ) return ;
	mark ( seq[lastmarked]->whatsthis() , -1 , -1 ) ;
	}    

SeqBasic* SequenceCanvas::findID ( wxString id ) 
    {
    int a ;
    for ( a = 0 ; a < seq.GetCount() ; a++ )
        if ( seq[a]->whatsthis() == id )
           return seq[a] ;
    return NULL ;
    }
    
void SequenceCanvas::MyGetClientSize ( int *w , int *h )
    {
    if ( printing )
       {
       print_dc->GetSize ( w , h ) ;
       if ( *h > lowy ) *h = lowy ;
       }
    else
       {
       GetClientSize ( w , h ) ;
       if ( !drawing && isHorizontal() && !isMiniDisplay() ) *w = 1000000 ;
       }
    }
    
wxSize SequenceCanvas::MyGetClientSize ()
    {
    if ( printing )
       {
       return print_dc->GetSize () ;
       }
    else
       {
       wxSize size = GetClientSize () ;
       if ( isHorizontal() && !isMiniDisplay() ) size.SetWidth ( 1000000 ) ;
       return size ;
       }
    }
    
void SequenceCanvas::MyGetSize ( int *w , int *h )
    {
    if ( printing )
       {
       print_dc->GetSize ( w , h ) ;
       }
    else
       {
       GetSize ( w , h ) ;
       if ( isHorizontal() ) *w = 1000000 ;
       }
    }
    
void SequenceCanvas::MyGetViewStart ( int *x , int *y )
    {
    if ( printing )
       {
       *x = 0 ;
       *y = 0 ;
       }
    else
       {
       GetViewStart ( x , y ) ;
       }
    }
    
void SequenceCanvas::updateEdit ( TVector *v , wxString id , int from )
    {
    if ( getAA() )
        {
        getAA()->vec->setChanged () ;
        getAA()->showStat();
        getAA()->showSequence () ;
        }
    else if ( v )
        {
        v->setChanged () ;
        for ( int a = 0 ; a < seq.GetCount() ; a++ )
           {
           seq[a]->initFromTVector ( v ) ;
           }    
        arrange () ;
        }
    else if ( getPD() )
        {
        getPD()->updatePrimersFromSequence () ;
        }
    mark ( id , from , from , 2 ) ;
    }

void SequenceCanvas::editSequence ( int k , wxKeyEvent& event )
	{
    int a , b = -1 ;
    wxString id = edit_id , valid = edit_valid ;
    for ( a = 0 ; a < seq.GetCount() && b == -1 ; a++ )
        if ( seq[a]->whatsthis() == id )
           b = a ;
    if ( b == -1 )
       {
       event.Skip() ; // Illegal ID
       return ;
       }
    SeqDNA *dna = (SeqDNA*) seq[b] ;
    int from = _from ;
    wxString *the_sequence ;
    if ( dna->whatsthis() == "AA" ) the_sequence = getAA()->vec->getSequencePointer() ;
    else if ( dna->whatsthis() == "DNA" ) the_sequence = dna->vec->getSequencePointer() ;
    else the_sequence = &dna->s ;
    TVector *v = NULL ;
    if ( p ) v = dna->vec ;
    else if ( getAA() ) v = getAA()->vec ;

    // Getting the layout
    int vx , vy , wx , wy ;
    MyGetViewStart ( &vx , &vy ) ;
    MyGetClientSize ( &wx , &wy ) ;
    wx /= charwidth ;
    wx -= wx % ( seq.GetCount() + 1 ) ;
    wy /= charheight ;
    wy -= wy % ( seq.GetCount() + 1 ) ;
    
    // Checking key validity
    if ( k >= 'a' && k <= 'z' ) k = k - 'a' + 'A' ;
    for ( a = 0 ; a < valid.length() && valid.GetChar(a) != k ; a++ ) ;
    if ( a == valid.length() && k >= 'A' && k <= 'Z' )
       {
       if ( wxMessageBox ( txt("t_allow_all_chars") , 
                             txt("t_question") , 
                             wxICON_QUESTION|wxYES_NO ) == wxYES )
          {
          edit_valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZ|-" ;
          valid = edit_valid ;
          for ( a = 0 ; a < valid.length() && valid.GetChar(a) != k ; a++ ) ;
          }
       }
    
    if ( a == valid.length() ) editSpecialKeyPressed ( k , v , the_sequence , wy , event ) ;
    else editCharPressed ( k , v , the_sequence ) ;
       
    // Scrolling so the cursor is visible
    int nx = vx , ny = vy ;
    while ( nx > 0 && nx > vpx ) nx-- ;
    while ( nx + wx <= vpx ) nx++ ;
    while ( ny > 0 && ny > vpy ) ny-- ;
    while ( ny + wy <= vpy ) ny++ ;
    if ( isHorizontal() ) ny = -1 ;
    else nx = -1 ;
    Scroll ( nx , ny ) ;
    if ( p )
       wxLogStatus(txt("seq_ed_loc"), p->cPlasmid->getMarkFrom() ) ;
	}   
 
void SequenceCanvas::editSpecialKeyPressed ( int k , TVector *v , wxString *the_sequence , int wy , wxKeyEvent& event )
    {
    if ( NumberOfLines() == 0 )
    	{
	    event.Skip() ;
	    return ;
    	}    
    wxString id = edit_id ;
    int from = markedFrom () ;
    int cpl = findID(edit_id)->itemsperline ;
    int page = wy * cpl / NumberOfLines() ;

    if ( k == WXK_RIGHT )
      {
      if ( event.ControlDown () )
         {
         from += blocksize - 1 ;
         from -= from % blocksize ;
         if ( from > the_sequence->length() ) from = the_sequence->length() - 1 ;
         mark ( id , from+1 , from+1 , 2 ) ;
         }
      else if ( from < the_sequence->length() )
         mark ( id , from+1 , from+1 , 2 ) ;
      else mark ( id , the_sequence->length() , the_sequence->length() , 2 ) ;
      }
    else if ( k == WXK_LEFT )
      {
      if ( event.ControlDown () )
         {
         from -= blocksize ;
         while ( from > 1 && (from-1) % blocksize > 0 ) from++ ;
         if ( from < 1 ) from = 1 ;
         mark ( id , from , from , 2 ) ;
         }
      else if ( from > 1 )
         mark ( id , from-1 , from-1 , 2 ) ;
      }
    else if ( k == WXK_DOWN && from+cpl <= the_sequence->length() )
   	  {
      mark ( id , from+cpl , from+cpl , 2 ) ;
      }    
    else if ( k == WXK_UP && from-cpl > 0 )
   	  {
      mark ( id , from-cpl , from-cpl , 2 ) ;
      }   	
    else if ( k == WXK_HOME )
      {
      mark ( id , 1 , 1 , 2 ) ;
      }   	
    else if ( k == WXK_END )
      {
      mark ( id , the_sequence->length() , the_sequence->length() , 2 ) ;
      }   	
    else if ( k == WXK_PRIOR )
      {
      from -= page ;
      if ( from < 1 ) from = 1 ;
      mark ( id , from , from , 2 ) ;
      }
    else if ( k == WXK_NEXT )
      {
      from += page ;
      if ( from > the_sequence->length() ) from = the_sequence->length() ;
      mark ( id , from , from , 2 ) ;
      }
//
    else if ( !forceoverwrite && k == WXK_DELETE && from != -1 && from+1 <= the_sequence->length() )
      {
      mylog ( "DEL" , wxString::Format ( "1 (%d of %d / %d)" , from , the_sequence->length() , (int)v ) ) ;
      if ( v ) v->doRemove ( from , from , false ) ;
      else the_sequence->erase ( from-1 , 1 ) ;
      mylog ( "DEL" , "2" ) ;
      if ( from > the_sequence->length() ) from = the_sequence->length() ;
      if ( v ) v->recalculateCuts() ;
      mylog ( "DEL" , "3" ) ;
      updateEdit ( v , id , from ) ;
      mylog ( "DEL" , "4" ) ;
      }
//
    else if ( !forceoverwrite && k == WXK_BACK && from > 1 )
      {
      if ( v ) v->doRemove ( from-1 , from-1 , false ) ;
      else the_sequence->erase ( from-2 , 1 ) ;
      if ( v ) v->recalculateCuts() ;
      updateEdit ( v , id , from-1 ) ;
      }

    else if ( !forceoverwrite && k == WXK_INSERT )
      {
      wantOverwrite = !wantOverwrite ;
      mark ( id , from , from , 2 ) ;
      if ( doOverwrite() ) wxLogStatus(txt("seq_ed_ovr"),1) ;
      else wxLogStatus(txt("seq_ed_ins"),1) ;
      }

    else
      {
      event.Skip() ; // Not a key used here
      }
    } 

void SequenceCanvas::editCharPressed ( int k , TVector *v , wxString *the_sequence )
	{
    wxString id = edit_id ;
	int from = markedFrom () ;
    wxString dummy ;
    int new_from = from + 1 ;
    
    if ( from == -1 )
    	{
	    mylog ( "SequenceCanvas::editCharPressed" , "No mark" ) ;
        return ;
        }    
   
    if ( k == '.' && getPD() )
       {
       int u ;
       for ( u = 0 ; u < seq.GetCount() && seq[u]->whatsthis() != edit_id ; u++ ) ;
       if ( edit_id == "PRIMER_UP" ) k = seq[u+1]->s.GetChar(from-1) ;
       if ( edit_id == "PRIMER_DOWN" ) k = seq[u-1]->s.GetChar(from-1) ;
       }
       
    dummy = (char) k ;           
    if ( v )
       {
       mylog ( "SequenceCanvas::editCharPressed" , "1" ) ;
       v->insert_char ( k , from , doOverwrite() ) ;
       mylog ( "SequenceCanvas::editCharPressed" , "2" ) ;
       v->recalculateCuts() ;
       mylog ( "SequenceCanvas::editCharPressed" , "3" ) ;
       }
    else
       {
       if ( !forceoverwrite || from <= the_sequence->length() )
          {
          if ( doOverwrite() ) the_sequence->erase ( from-1 , 1 ) ;
          wxStringInsert ( *the_sequence , from-1 , dummy ) ;
          if ( forceoverwrite && from == the_sequence->length() )
             new_from = from ;
          }
       else
          {
          wxBell() ;
          return ;
          }
       }
    updateEdit ( v , id , new_from ) ;
	}    

void SequenceCanvas::OnCharHook(wxKeyEvent& event)
    {
    if ( seq.GetCount() == 0 ) return ;
    int k = event.GetKeyCode () ;
    if ( editMode ) editSequence ( k , event ) ;
    else // Not edit mode
        {
        if ( k == WXK_HOME ) ensureVisible ( 0 ) ;
        else if ( k == WXK_END ) ensureVisible ( SCROLL_TO_END ) ;
        else event.Skip() ;
        }
    }
    
bool SequenceCanvas::doOverwrite ()
    {
    if ( forceoverwrite ) return true ;
    return wantOverwrite ;
    }
    
void SequenceCanvas::vecEdit ( wxCommandEvent &ev )
    {
    if ( p )
        {
        p->cPlasmid->vecEdit ( ev ) ;
        }
    else if ( getPD() )
        {
        TVectorEditor ve ( this , txt("t_vector_editor") , getPD()->w ) ;
        ve.initialViewEnzyme ( "" ) ;
        ve.hideProp = true ;
        ve.hideItem = true ;
        ve.hideEm () ;
        int x = ve.ShowModal () ;
        ve.cleanup () ;
        if ( x == wxID_OK )
            {
            getPD()->vec->re = getPD()->w->re ;
            for ( x = 0 ; x < 4 ; x++ ) seq.RemoveAt ( seq.GetCount()-1 ) ;
            getPD()->updateResultSequence() ;
            arrange () ;
            Refresh () ;
            }
        }
    else if ( child && child->def == "AminoAcids" )
        {
        TAminoAcids *ama = (TAminoAcids*) child ;
        ama->OnEditName ( ev ) ;
        }
    SetFocus() ;
    }

void SequenceCanvas::blastDNA ( wxCommandEvent &ev )
    {
    p->cPlasmid->blastDNA ( ev ) ;
    }

void SequenceCanvas::OnPrimerForward ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerForward ( ev ) ; }

void SequenceCanvas::OnPrimerBackward ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerBackward ( ev ) ; }

void SequenceCanvas::OnPrimerBoth ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerBoth ( ev ) ; }

void SequenceCanvas::OnPrimerMutation ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerMutation ( ev ) ; }

void SequenceCanvas::blastAA ( wxCommandEvent &ev )
    {
    if ( p && p->cPlasmid ) p->cPlasmid->blastAA ( ev ) ;
    else if ( getAA() )
        {
        wxString seq = getSelection() ;
        myapp()->frame->blast ( seq , "blastp" ) ;
        }
    }

void SequenceCanvas::OnCut ( wxCommandEvent &ev )
    {
    if ( p ) { p->OnCut ( ev ) ; return ; }
    if ( !getAA() || _from == -1 ) return ;
    wxString s = getSelection () ;
    if ( s.IsEmpty() ) return ;
    if (!wxTheClipboard->Open()) return ;

    wxTheClipboard->SetData( new wxTextDataObject(s) );
    wxTheClipboard->Close();

    getAA()->vec->doRemove ( _from , _to ) ;
    _from = -1 ;
    
    for ( int a = 0 ; a < seq.GetCount() ; a++ )
       {
       if ( seq[a]->whatsthis() == "AA" )
          {
          SeqAA *x = (SeqAA*) seq[a] ;
          x->initFromString ( getAA()->vec->getSequence() ) ;
          }
       else seq[a]->initFromTVector ( getAA()->vec ) ;
       }

    arrange() ;
    Refresh() ;
    getAA()->OnListBox ( ev ) ;
    }

void SequenceCanvas::OnCopyText ( wxCommandEvent &ev )
    {
    int a ;
    wxString s , t ;
    wxArrayString out ;
    int minline = -1 , maxline = -1 ;
    for ( a = 0 ; a < seq.GetCount() ; a++ )
        {
        wxPoint p = seq[a]->showText ( a , out ) ;
        if ( p.x >= 0 ) p.x -= a ;
        if ( p.y >= 0 )
           {
           p.y -= a ;
           p.y += seq.GetCount()-1 ;
           }
        if ( ( minline == -1 || p.x < minline ) && p.x >= 0 ) minline = p.x ;
        if ( p.y > maxline && p.y >= 0 ) maxline = p.y ;
        }

    while ( out.GetCount() && out.Last().IsEmpty() )
        out.Remove ( out.GetCount()-1 ) ;

    if ( minline < 0 ) minline = 0 ;
    if ( maxline < 0 ) maxline = out.GetCount()-1 ;

    s = "" ;
    for ( a = minline ; a <= maxline ; a++ )
        s += out[a] + "\n" ;

    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }
    }

void SequenceCanvas::OnPrint ( wxCommandEvent &ev )
    {
    wxPrintDialogData pdd ;
    pdd.EnableSelection ( _from != -1 ) ;
    wxPrintDialog pd ( this , &pdd ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;
    
    wxBeginBusyCursor() ;
    
    pdd = pd.GetPrintDialogData() ;
    wxPrintData pD = pdd.GetPrintData() ;
    bool printSelection = pdd.GetSelection() ;
    setPrintToColor ( pD.GetColour() ) ;
    bool wasHorizontal = isHorizontal() ;
    setHorizontal ( false ) ;

    TMarkMem mm ( this ) ; // Temporary mark for printing

    wxString name = "GENtle" ;
    if ( p && p->vec ) name = p->vec->getName() ;

    print_dc = pd.GetPrintDC () ;
    print_dc->StartDoc ( name ) ;
    printing = true ;
    setDrawAll ( true ) ;
    
    int w , h ;
    MyGetSize ( &w , &h ) ;
    int pagetop = h / 20 ;
    int pagebottom = h / 40 ;
    int vish = h - pagetop - pagebottom ;
    
    int fs = w / 10 ;
    if ( getAA() ) fs *= 2 ;
    
    wxFont *oldfont = font ;
    wxFont *oldvarfont = varFont ;
    wxFont *oldsmallFont = smallFont ;
    wxFont *bigfont = MYFONT ( w/30 , wxMODERN , wxNORMAL , wxNORMAL ) ;
    wxFont *medfont = MYFONT ( w/80 , wxMODERN , wxNORMAL , wxNORMAL ) ;
    font = MYFONT ( fs/10 , wxMODERN , wxNORMAL , wxNORMAL ) ; 
    smallFont = MYFONT ( fs/15 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    varFont = MYFONT ( fs/11 , wxROMAN  , wxNORMAL , wxNORMAL ) ;

    print_maxx = -w ;

    OnDraw ( *print_dc ) ;

    mm.unmark () ;
    mm.remark () ;

    int xoff = ( w - print_maxx ) / 2 ;
    int dummy = vish % charheight ;

    int beginning = -pagetop ;
    if ( printSelection )
       {
       int a , _f = -1 , _t = -1 , _fin = -1 ;
       for ( a = 0 ; a < seq[lastmarked]->getMarkSize() ; a++ )
          {
          if ( seq[lastmarked]->getMark ( a ) == 1 )
             {
             if ( _f == -1 ) _f = seq[lastmarked]->getRect(a).y ;
             _t = seq[lastmarked]->getRect(a).y ;
             }
          if ( _t == seq[lastmarked]->getRect(a).y ) _fin = a ;
          }
       if ( _f != -1 )
          {
          _f -= charheight * ( lastmarked ) ;
          beginning += _f ;
          
          _t -= charheight * ( lastmarked ) ;
          _t += charheight * ( seq.GetCount() + blankline + 1 ) ;
          lowy = _t ;
          
          hardstop = _fin ;
//          mm.unmark() ;
          }
       }
    
    int yoff = beginning ;
    int page = 0 ;
    int totalpages = 0 ;

    int lines = ( vish - dummy ) / charheight ;
    lines %= seq.GetCount() + blankline ;
    dummy += lines * charheight ;
    
    wxDateTime now = wxDateTime::Now();
    wxString printtime = now.Format("%c", wxDateTime::Local) ;

    
    do {
       totalpages++ ;
       yoff += vish - dummy ;
       } while ( yoff < lowy ) ;
       
    yoff = beginning ;
    
    do {
       page++ ;
       print_dc->SetDeviceOrigin ( xoff , -yoff ) ;
       print_dc->StartPage () ;
       
       // Drawing page top and bottom
       safeShow ( *print_dc ) ;
       print_dc->SetPen ( *wxWHITE_PEN ) ;
       print_dc->SetBrush ( *wxWHITE_BRUSH ) ;
       print_dc->DrawRectangle ( -xoff , yoff , w , pagetop ) ;
       print_dc->DrawRectangle ( -xoff , yoff + h - pagebottom - dummy , w , pagebottom + dummy ) ;
       
       if ( printSelection && yoff + h - pagebottom - dummy > lowy )
          print_dc->DrawRectangle ( -xoff , lowy , w , yoff + h - pagebottom - dummy - lowy ) ;
       
       int tw , th ;
       wxString s ;
       
       // Title
       if ( p ) s = p->vec->getName() ;
       else if ( getAA() ) s = getAA()->vec->getName() ;
       else if ( child ) s = child->getName() ;
       print_dc->SetTextBackground ( *wxWHITE ) ;
       print_dc->SetFont (* bigfont ) ;
       print_dc->GetTextExtent ( s , &tw , &th ) ;
       print_dc->DrawText ( s ,
                            ( w - tw ) / 2 - xoff ,
                            yoff + ( pagetop - th ) / 2 ) ;
       print_dc->SetTextBackground ( *wxWHITE ) ;

       // Page number
       wxString t = wxString::Format ( txt("t_page_of") , page , totalpages ) ;
       print_dc->SetFont ( *medfont ) ;
       print_dc->GetTextExtent ( t , &tw , &th ) ;
       print_dc->DrawText ( t ,
                            w - tw - xoff * 2 ,
                            yoff + h - ( pagebottom + dummy + th ) / 2 ) ;

       // Date
       print_dc->SetFont ( *medfont ) ;
       print_dc->GetTextExtent ( printtime , &tw , &th ) ;
       print_dc->DrawText ( printtime ,
                            0 ,
                            yoff + h - ( pagebottom + dummy + th ) / 2 ) ;
       
       
       print_dc->EndPage () ;

       yoff += vish - dummy ;
       } while ( yoff < lowy ) ;

    setDrawAll ( false ) ;
    printing = false ;
    font = oldfont ;
    varFont = oldvarfont ;
    smallFont = oldsmallFont ;
    
    print_dc->EndDoc () ;
    hardstop = -1 ;
    delete print_dc ;
    print_dc = NULL ;
    setHorizontal ( wasHorizontal ) ;
    charwidth = 0 ; // To force re-arrange
    if ( child ) child->Activate () ;
    SilentRefresh () ;
    mm.unmark () ;
    mm.remark () ;
    wxEndBusyCursor() ;
    }

void SequenceCanvas::OnSaveImage ( wxCommandEvent &ev )
    {
    wxBitmap *bmp = getSequenceBitmap () ;
    myapp()->frame->saveImage ( bmp , child ? child->getName() : "" ) ;
//    delete bmp ;
    }    
    
void SequenceCanvas::OnCopyImage ( wxCommandEvent &ev )
    {
    int w , h ;
    MyGetClientSize ( &w , &h ) ;
    h = lowy ;

// Metafile support only exists for windows, so...
#ifdef __WXMSW__
    if ( myapp()->frame->useMetafile ) // Use metafile DC
        {
        GetClientSize(&w, &h);
        wxMetafileDC dc ;
//        dc.SetUserScale ( 10 , 10 ) ;
        OnDraw ( dc ) ;
        wxMetafile *mf = dc.Close();
        if (mf)
           {
           bool success = mf->SetClipboard((int)(dc.MaxX() + 10), (int)(dc.MaxY() + 10));
           delete mf;
           return ;
           }
        }
#endif
    
    if ( w * h * 3 >= 32*1024*1024 )
        {
        wxMessageDialog md ( this , txt("t_too_large_to_copy") ,
                                txt("msg_box") , wxYES|wxNO|wxICON_QUESTION ) ;
        if ( md.ShowModal() == wxID_YES )
           {
           OnSaveImage ( ev ) ;
           return ;
           }
        }
    
    wxBitmap *bmp = getSequenceBitmap () ;
    
    // Copy to clipboard
    if (wxTheClipboard->Open())
        {
        wxBitmapDataObject *bdo = new wxBitmapDataObject ( *bmp ) ;
        wxTheClipboard->SetData( bdo ) ; 
        wxTheClipboard->Close();
        }
    
    }
    
wxBitmap *SequenceCanvas::getSequenceBitmap ()
    {
    int w , h ;
    MyGetClientSize ( &w , &h ) ;
    h = lowy ;

    int vx = 0 , vy = 0 ;
    int yoff = charheight * vy ;
    if ( isHorizontal() )
    	{
	    w = 0 ;
	    for ( int a = 0 ; a < seq.GetCount() ; a++ )
	    	{
 	    	for ( int b = 0 ; b < seq[a]->getRectSize()  ; b++ )
 	    		{
  		    	int nw = seq[a]->getRect(b).GetRight() ;
  		    	if ( nw > w ) w = nw ;
 	    		}    
	    	}    
    	}    
    wxBitmap *bmp = new wxBitmap ( w , h , -1 ) ;
    wxMemoryDC pdc ;
    pdc.SelectObject ( *bmp ) ;
    pdc.Clear() ;
    arrange () ;
    setDrawAll ( true ) ;
    safeShow ( pdc ) ;
    setDrawAll ( false ) ;
    return bmp ;
    }

int SequenceCanvas::getBatchMark ()
    {
    int r = vpy ;
    r -= vpy % NumberOfLines() ;//seq.GetCount() ;
    return r ;
    }
        
void SequenceCanvas::OnCopy ( wxCommandEvent &ev )
    {
    if ( p ) { p->OnCopy ( ev ) ; return ; }
    wxString s = getSelection () ;
    if ( s.IsEmpty() ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }
    }
    
wxString SequenceCanvas::getSelection ()
    {
    if ( getEditMode() ) return "" ;
    if ( markedFrom() == -1 ) return "" ;
    if ( p ) return p->vec->getSubstring ( _from , _to ) ;
    if ( getAA() && _from != -1 ) return getAA()->vec->getSubstring ( _from , _to ) ;
    if ( getPD() && _from != -1 && lastmarked != -1 )
        return seq[lastmarked]->s.substr ( _from-1 , _to-_from+1 ) ;
    if ( child && child->def == "ABIviewer" )
        {
        if ( _from == -1 ) return "" ;
        return seq[lastmarked]->s.substr ( _from-1 , _to-_from+1 ) ;
        }
    return "" ;
    }
    
void SequenceCanvas::ensureVisible ( int pos )
    {
    if ( charwidth == 0 || charheight == 0 ) return ;
    if ( NumberOfLines() == 0 ) return ;
    int vx , vy , wx , wy ;
    MyGetViewStart ( &vx , &vy ) ;
    MyGetClientSize ( &wx , &wy ) ;
    wy /= charheight ;
    wy -= wy % NumberOfLines() ;
    
    wx /= charwidth ;
    wx -= wx % NumberOfLines() ;

    if ( pos == SCROLL_TO_END ) 
        {
        if ( isHorizontal() ) Scroll ( lowx , -1 ) ;
        else Scroll ( -1 , lowy ) ;
        return ;
        }

    int nx = pos , ny = pos ;
    if ( isHorizontal() )
        {
        ny = -1 ;
        while ( nx > 0 && nx > vpx ) nx-- ;
        while ( nx + wx <= vpx ) nx++ ;
        }
    else
        {
        nx = -1 ;
        while ( ny > 0 && ny > vpy ) ny-- ;
        while ( ny + wy <= vpy ) ny++ ;
        }
    Scroll ( nx , ny ) ;
    }

bool SequenceCanvas::inMarkRange ( int x , int f , int t , int l )
    {
    if ( x >= f && x <= t ) return true ;
    if ( f > t )
        {
        if ( x >= 0 && x <= t ) return true ;
        if ( x >= f && x <= l ) return true ;
        }
    return false ;
    }

void SequenceCanvas::mark ( wxString id , int from , int to , int value )
    {
    mylog ( "MARK" , wxString::Format ( "%s %d-%d %d" , id.c_str() , from , to , value ) ) ;
    if ( seq.GetCount() == 0 ) return ;
    if ( marking ) return ;
    if ( getEditMode() && from != -1 && from != to ) return ;
    marking = true ;
    if ( child && child->cSequence && isMiniDisplay() )
        {
        myass ( child , "Mark1" ) ;
        myass ( child->cSequence , "Mark2" ) ;
        child->cSequence->mark ( "AA" , from , to , value ) ;
        }
    else if ( !isMiniDisplay() && getAA() && getAA()->sc2 )
        {
        myass ( getAA()->sc2->seq.GetCount() , "Mark3" ) ;
        getAA()->sc2->mark ( getAA()->sc2->seq[0]->whatsthis() , from , to , value ) ;        
        }
    marking = false ;

    int a , b = -1 , cnt = 0 ;
    vpx = -1 ;
    vpy = -1 ; 
    for ( a = 0 ; a < seq.GetCount() && b == -1 ; a++ )
        if ( seq[a]->whatsthis() == id )
           b = a ;
    lastmarked = b ;
    if ( b == -1 ) // Illegal ID
       {
       _from = _to = -1 ;
       return ; 
       }

    if ( charwidth == 0 || charheight == 0 ) SilentRefresh () ;

    int seqlen ;
    if ( p && p->vec ) seqlen = p->vec->getSequenceLength() ;
    else seqlen = seq[b]->s.length() ;

    int l = seqlen ;
    for ( a = 0 ; a < seq[b]->getMarkSize() ; a++ )
        {
        if ( inMarkRange ( seq[b]->getPos(a) , from , to , l ) ) 
           {
           seq[b]->setMark ( a , value ) ;
           cnt += value ;
           if ( vpx == -1 ) vpx = seq[b]->getRect(a).x / charwidth ;
           if ( vpy == -1 ) vpy = seq[b]->getRect(a).y / charheight ;
           }
        else 
           seq[b]->setMark ( a , 0 ) ;
        }

//        
    for ( int other = 0 ; other < seq.GetCount() ; other++ )
        {
        bool canbemarked = seq[other]->takesMouseActions ;
        if ( other == b ) canbemarked = false ;
        for ( a = 0 ; canbemarked && a < seq[other]->getMarkSize() ; a++ )
            {
            seq[other]->setMark ( a , 0 ) ;
            }
        }
        
    if ( !printing )
        {
        // Refreshing sequence canvas
        SilentRefresh () ;
        }
    
    if ( from > to ) to += l ;
    if ( cnt == 0 ) from = -1 ;

//

    _from = from ;
    _to = to ;
    if ( p )
        {
        // Refreshing plasmid canvas
        p->cPlasmid->setMark ( from , to ) ;
        if ( !editMode )
            {
            p->cPlasmid->Refresh() ;
            
            char tt[1000] ;
            if ( value == 1 && seqlen > 0 )
               {
               int to2 = to ;
               while ( to2 > seqlen )
                  to2 -= seqlen ;
               sprintf ( tt , txt("X-Y (# bp)") , from , to2 , to-from+1 ) ;
               if ( from == -1 ) *tt = 0 ;
               }
            else *tt = 0 ;
            MyFrame *f = myapp()->frame ;
            f->SetStatusText ( tt , 1 ) ;
            }
        }
    else if ( child && child->def == "AminoAcids" )
        {
        TAminoAcids *ama = (TAminoAcids*) child ;
        if ( ama->curDisplay == ama->pc )
           {
            // Refreshing plasmid canvas
            ama->pc->setMark ( from , to ) ;
            if ( !editMode )
                {
                char tt[1000] ;
                ama->pc->Refresh() ;
                if ( value == 1 )
                   {
                   int to2 = to ;
                   while ( to2 > seqlen )
                      to2 -= seqlen ;
                   sprintf ( tt , txt("X-Y (# aa)") , from , to2 , to-from+1 ) ;
                   if ( from == -1 ) *tt = 0 ;
                   }
                else *tt = 0 ;
                MyFrame *f = myapp()->frame ;
                f->SetStatusText ( tt , 1 ) ;
                }
            }        
        }
    else if ( getPD() )
        {
        if ( !editMode )
           {
            char tt[1000] ;
            if ( value == 1 )
               {
               int to2 = to ;
               while ( to2 > seqlen )
                  to2 -= seqlen ;
               sprintf ( tt , txt("X-Y (# bp)") , from , to2 , to-from+1 ) ;
               if ( from == -1 ) *tt = 0 ;
               }
            else *tt = 0 ;
            MyFrame *f = (MyFrame*) myapp()->frame ;
            f->SetStatusText ( tt , 1 ) ;
           }
        }
    }

void SequenceCanvas::arrange ()
    {
    mylog ( "SequenceCanvas::arrange" , "1" ) ;
    if ( seq.GetCount() == 0 ) return ;
    if ( charheight == 0 ) return ;
    int vx , vy ;
    int oldlowy = lowy ;
    int oldlowx = lowx ;
    lowy = 0 ;
    lowx = 0 ;
    lastmarked = -1 ;
    int a , b ;
    
    wxBeginBusyCursor() ;    
    
    if ( isHorizontal() )
        {
        blocksize = 0 ;
        for ( a = 0 ; a < seq.GetCount() ; a++ )
           {
           if ( seq[a]->s.length() + 1 > blocksize )
              blocksize = seq[a]->s.length() + 1 ;
           }
        }
    else blocksize = 10 ;
    
    // End-number length
    maxendnumberlength = 0 ;
    for ( a = 0 ; a < seq.GetCount() ; a++ )
        {
        seq[a]->makeEndnumberLength() ;
        if ( seq[a]->endnumberlength > maxendnumberlength )
           maxendnumberlength = seq[a]->endnumberlength ;
        }
    for ( a = 0 ; a < seq.GetCount() ; a++ )
        seq[a]->endnumberlength = maxendnumberlength ;
            
    mylog ( "SequenceCanvas::arrange" , "2" ) ;

    // Arranging
    for ( a = 0 ; a < seq.GetCount() ; a++ )
        {
        mylog ( "SequenceCanvas::arrange" , "Trying " + seq[a]->whatsthis() ) ;
        b = seq[a]->arrange ( a ) ;
        if ( b > lowy ) lowy = b ;
        }
    if ( isMiniDisplay() ) lowx += charwidth*2 ;
    mylog ( "SequenceCanvas::arrange" , "3" ) ;
    
    if ( printing ) return ;

    MyGetViewStart ( &vx , &vy ) ;
    if ( !isHorizontal() && lowy != oldlowy )
       SetScrollbars ( 0 , charheight , vy , (lowy+charheight-1)/charheight , false ) ;
    if ( isHorizontal() && lowx != oldlowx )
       SetScrollbars ( charwidth , 0 , lowx/charwidth , lowx/charwidth , false ) ;
    
    if ( p && p->cPlasmid->getMarkFrom() != -1 && !editMode )
        {
        if ( !printing ) p->cPlasmid->setMark ( -1 , -1 ) ;
        p->cPlasmid->Refresh () ;
        }
    wxEndBusyCursor() ;
    mylog ( "SequenceCanvas::arrange" , "4" ) ;
    }

void SequenceCanvas::OnSize(wxSizeEvent &event)
    {
    TMarkMem m ( this ) ;
    arrange () ;
    m.remark () ;
    }
    

// Define the repainting behaviour
void SequenceCanvas::OnDraw(wxDC& dc)
{
    mylog ( "SequenceCanvas::OnDraw" , "1" ) ;
    if ( drawing ) return ;
    if ( getAln() && getAln()->isThreadRunning() )
    {
	if ( !printing )
	{
	    dc.SetFont ( *font ) ;
		dc.SetTextForeground ( *wxBLACK ) ;
		dc.SetTextBackground ( *wxWHITE ) ;
	    dc.DrawText ( txt("t_clustal_running") , 10 , 10 ) ;
	}
	return ;
    }
    if ( seq.GetCount() == 0 ) return ;
    if ( getHide() ) return ;
    if ( myapp()->frame->isLocked() ) return ;
    if ( child && !child->IsShown() ) return ;

    drawing = true ;
    dc.SetFont ( *font ) ;
    int wx , wy ;
    dc.GetTextExtent ( "A" , &wx , &wy ) ;
    mylog ( "SequenceCanvas::OnDraw" , "2a" ) ;
    if ( wx != charwidth || wy != charheight )
        {
        charwidth = wx ;
        charheight = wy ;
        arrange () ;
        mylog ( "SequenceCanvas::OnDraw" , "2b" ) ;
        if ( printing && print_maxx < 0 )
            {
            for ( int a = 0 ; a < seq.GetCount() ; a++ )
               {
               for ( int b = 0 ; b < seq[a]->getRectSize() ; b++ )
                  {
                  if ( seq[a]->getRect(b).GetRight() > print_maxx )
                     print_maxx = seq[a]->getRect(b).GetRight() ;
                  }
               }
            drawing = false ;
            return ;
            }
        }
	mylog ( "SequenceCanvas::OnDraw" , "3" ) ;
        
    if ( printing )
        {
        drawing = false ;
        return ;
        }
	mylog ( "SequenceCanvas::OnDraw" , "4" ) ;
        
    int vx , vy ;
    GetViewStart ( &vx , &vy ) ;

    if ( myapp()->frame->enhancedRefresh && !printing )
        { // MemoryDC
        mylog ( "!" , "1" ) ;
        int w , h ;
        MyGetClientSize ( &w , &h ) ;
        int xoff = wx * vx ;
        int yoff = wy * vy ;
        wxBitmap bmp ( w , h , -1 ) ;
        wxMemoryDC pdc ;
        pdc.SelectObject ( bmp ) ;
        pdc.Clear() ;
        pdc.SetDeviceOrigin ( -xoff , -yoff ) ;
        mylog ( "SequenceCanvas::OnDraw" , "4a" ) ;
        safeShow ( pdc ) ;
        mylog ( "SequenceCanvas::OnDraw" , "4b" ) ;
        mylog ( "!" , "2" ) ;
        pdc.SetDeviceOrigin ( 0 , 0 ) ;
        dc.Blit ( xoff , yoff , w , h , &pdc , 0 , 0 ) ;
        mylog ( "!" , "3" ) ;
        }
    else 
        { // Direct DC
        mylog ( "!" , "1a" ) ;
        dc.Clear() ;
        safeShow ( dc ) ;
        mylog ( "!" , "2a" ) ;
        }
    drawing = false ;
    dc.SetFont ( *wxNORMAL_FONT ) ; //!!!
    mylog ( "SequenceCanvas::OnDraw" , "Done" ) ;
}

void SequenceCanvas::safeShow ( wxDC &dc )
    {
    int a ;
    for ( a = 0 ; a < seq.GetCount() ; a++ ) seq[a]->shown = false ;
    for ( a = 0 ; a < seq.GetCount() ; a++ )
       {
       if ( seq[a]->whatsthis() != "RESTRICTION" &&
            seq[a]->whatsthis() != "FEATURE" )
          {
          mylog ( "SequenceCanvas::safeShow" , wxString::Format ( "Trying %d (%s)" , a , seq[a]->whatsthis().c_str() ) ) ;
          seq[a]->show ( dc ) ;
          seq[a]->shown = true ;
          }
       }
    
    for ( a = 0 ; a < seq.GetCount() ; a++ )
       {
       if ( !seq[a]->shown )
          {
          mylog ( "SequenceCanvas::safeShow" , wxString::Format ( "Finalizing %d (%s)" , a , seq[a]->whatsthis().c_str() ) ) ;
          seq[a]->show ( dc ) ;
          }    
       }
    mylog ( "SequenceCanvas::safeShow" , "Done" ) ;
    }
    
void SequenceCanvas::OnEvent(wxMouseEvent& event)
{
    if ( drawing ) return ;
    if ( getAln() && getAln()->isThreadRunning() ) return ;
    wxClientDC dc(this);
    PrepareDC(dc);

    wxPoint pt(event.GetLogicalPosition(dc));
    int my = dc.LogicalToDeviceY ( pt.y ) ;

    int wr = event.GetWheelDelta() ;
    if ( wr != 0 ) wr = event.GetWheelRotation() / wr ;
    if ( wr != 0 )
        {
        int vx , vy ;
        MyGetViewStart ( &vx , &vy ) ;
        if ( isHorizontal() ) Scroll ( vx-wr , -1 ) ;
        else Scroll ( -1 , vy-wr ) ;
        return ;
        }
    
    wxString newToolTip ;
    int pos ;
    SeqBasic *where ;
    where = findMouseTarget ( pt , pos ) ;
    if ( pos <= 0 ) pos = -1 ;
    else pos = where->getPos(pos) ;
    if ( pos != -1 && where && where->takesMouseActions )
        {
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        if ( where->whatsthis() == "PLOT" && isMiniDisplay() )
           {
           SeqPlot *plot = (SeqPlot*) where ;
           wxString q = plot->getTip ( pos-1 ) ;
           wxLogStatus ( q ) ;
           newToolTip = q ;
           }
        else
           {
           wxString q = wxString::Format(txt("seq_loc"),pos) ;
           if ( where->whatsthis() == "AA" && getAA() )
              {
              char c = where->s.GetChar ( pos - 1 ) ;
              wxString qx = getAA()->vec->one2three ( c ) ;
              wxString qy = txt(wxString::Format("t_aa_%c",c).c_str()) ;
              q += " [" + wxString ( c ) + "; " + qx + "; " + qy + "]" ;
              }
           wxLogStatus ( q ) ;
           newToolTip = q ;
           }
        }
    else if ( where && child && child->def == "alignment" )
        {
        SeqAlign *al = (SeqAlign*)where ;
        if ( al->myname == txt("t_identity") ) {} // Do nothing
        else if ( al->whatsthis() == "FEATURE" ) {} // Do nothing
        else if ( al->s.GetChar(pos-1) == '-' ) wxLogStatus ( al->myname ) ;
        else
           {
           int a , b ;
           for ( a = b = 0 ; a < pos ; a++ )
              if ( al->s.GetChar(a) != '-' )
                b++ ;
                
           newToolTip = al->myname ;
           newToolTip += ", " ;
           newToolTip += wxString::Format(txt("seq_loc"),b) ;
           wxLogStatus(wxString::Format("%s (%d)",al->myname.c_str(),b)) ;
           }
        }
    else
        {
        wxLogStatus("") ;
        SetCursor(wxCursor(*wxSTANDARD_CURSOR)) ;
        }

    if ( newToolTip != lastToolTip )
       {
       lastToolTip = newToolTip ;
       SetToolTip ( newToolTip ) ;
       }
        
    if ( event.MiddleDown() && where )
        {
        if ( child && child->def == "alignment" )
           {
           SeqAlign *al = (SeqAlign*)where ;
           TAlignment *alg = (TAlignment*) child ;
           if ( al->myname == txt("t_identity") ) {} // Do nothing
           else if ( al->whatsthis() == "FEATURE" ) {} // Do nothing
           else alg->callMiddleMouseButton ( al->id , pos ) ;
           }
        }
    
    if ( event.LeftDClick() )
        {
        if ( p && p->cPlasmid ) p->cPlasmid->invokeVectorEditor() ;
        else if ( getAA() ) getAA()->invokeVectorEditor() ;
        else if ( getPD() ) { wxCommandEvent ev ; vecEdit ( ev ) ; }
        else if ( where && child && child->def == "alignment" )
           {
           SeqAlign *al = (SeqAlign*)where ;
           if ( al->myname == txt("t_identity") ) {} // Do nothing
           else if ( al->whatsthis() == "FEATURE" ) {} // Do nothing
           else if ( al->s.GetChar(pos-1) == '-' ) wxLogStatus ( al->myname ) ;
           else
              {
              lastToolTip = "" ;
              SetToolTip ( "" ) ;
              int a , b ;
              for ( a = b = 0 ; a < pos ; a++ )
                 if ( al->s.GetChar(a) != '-' )
                   b++ ;
              TAlignment *ali = (TAlignment*) child ;
              ali->invokeOriginal ( al->id , b ) ;
              }
           }
        }
    else if ( event.LeftDown() )
        {
        if ( where && where->takesMouseActions )
           {
            CaptureMouse() ;
            if ( pos != -1 && lastpos == -1 )
               {
               lastpos = pos ;
               lastwhere = where ;
               }
            else
               {
               lastpos = -1 ;
               for ( int a = 0 ; a < seq.GetCount() ; a++ )
                  if ( seq[a]->takesMouseActions )
                     mark ( seq[a]->whatsthis() , 0 , 0 , 0 ) ;
               }
            }
        else if ( !editMode )
            {
            lastpos = -1 ;
            for ( int a = 0 ; a < seq.GetCount() ; a++ )
               {
               if ( seq[a]->takesMouseActions )
                  {
                  mark ( seq[a]->whatsthis() , 0 , 0 , 0 ) ;
//                  break ;
                  }
               }
            }
        }
    else if ( event.LeftUp() )
        {
        ReleaseMouse() ;
        lastpos = -1 ;
        }

    if ( event.RightDown() )
        showContextMenu ( where , pos , event.GetPosition() ) ;

    if ( event.Dragging() || ( event.LeftDown() && where && where->takesMouseActions ) )
        {
        wxSize cs = MyGetClientSize () ;
        int qx , qy ;
        MyGetViewStart ( &qx , &qy ) ;
        int nol = 1 ;
        if ( my > cs.y ) Scroll ( 0 , qy+nol ) ;
        else if ( my < 0 ) Scroll ( 0 , qy-nol ) ;
    
        if ( pos != -1 && lastpos != -1 && getLastWhere() == where )
            {
            if ( editMode )
               {
               if ( seq[lastmarked] == where )
                  mark ( where->whatsthis() , pos , pos , 2 ) ;
               }
            else
               {
               if ( pos >= lastpos )
                  mark ( where->whatsthis() , lastpos , pos , 1 ) ;
               else
                  mark ( where->whatsthis() , pos , lastpos , 1 ) ;
               }
            }
        }
}

void SequenceCanvas::showContextMenu ( SeqBasic *where , int pos , wxPoint pt )
    {
    wxMenu *cm ;
    if ( p && p->cPlasmid ) // DNA
       {
       cm = p->cPlasmid->invokeVectorPopup ( pt , true ) ;
       }
    else if ( getAA() ) // Amino acids
       {
       cm = new wxMenu ;
       cm->Append ( AMINOACIDS_EDIT_NAME , txt("m_edit_aa") ) ;
       cm->Append ( SEQ_AA_BACKTRANSLATE, txt("m_aa_backtranslate") ) ;
       if ( _from != -1  )
          {
           cm->Append ( MDI_CUT , txt("m_cut") ) ;
           cm->Append ( MDI_COPY , txt("m_copy") ) ;
           cm->Append ( MDI_AS_NEW_FEATURE , txt("m_as_new_feature") ) ;
           }
       cm->Append ( AMINOACIDS_BLAST_AA , txt("m_blast_aa") ) ;
       }
    else if ( getPD() ) // Primer design
       {
       cm = new wxMenu ;
       wxMenu *crd = new wxMenu ;
       cm->Append ( PC_VECTOR_EDIT , txt ( "t_vector_editor" ) ) ;
       cm->Append ( POPUP_DUMMY , txt("m_result_dna" ) , crd ) ;
       crd->Append ( SEQ_COPY_RESLUT_DNA , txt("m_copy_result_dna") ) ;
       crd->Append ( SEQ_NEW_FROM_RESLUT_DNA , txt("m_new_from_result_dna") ) ;
       
       if ( getPD()->aa_state != AA_ALL && getPD()->aa_state != AA_NONE )
          {
          wxMenu *cra = new wxMenu ;
          cm->Append ( POPUP_DUMMY , txt("m_result_aa" ) , cra ) ;
          cra->Append ( SEQ_COPY_RESLUT_AA , txt("m_copy_result_aa") ) ;
          cra->Append ( SEQ_NEW_FROM_RESLUT_AA , txt("m_new_from_result_aa") ) ;
          if ( _from != -1 ) cm->Append ( PD_SILMUT , txt("m_silmut") ) ;
          }
                  
       if ( _from != -1  )
          {
          cm->AppendSeparator () ;
          cm->Append ( MDI_COPY , txt("m_copy") ) ;
          cm->Append ( MDI_SEL_AS_NEW_PRIMER , txt("m_sel_as_new_primer") ) ;
          cm->Append ( SEQ_INSERT_RESTRICTION_SITE_LEFT , txt("m_insert_restiction_site_left") ) ;
          cm->Append ( SEQ_INSERT_RESTRICTION_SITE_RIGHT , txt("m_insert_restiction_site_right") ) ;
          }
       }
    else if ( child && child->def == "ABIviewer" ) // Sequencer data
       {
       cm = new wxMenu ;
       cm->Append ( MDI_COPY_TO_NEW , txt("m_copy_to_new") ) ;
       if ( _from != -1  )
          {
          cm->Append ( MDI_COPY , txt("m_copy") ) ;
          }
       cm->AppendSeparator () ;
       }
    else if ( child && child->def == "alignment" && ( where || ( findMouseTargetItem ( pt ) != -1 ) ) ) // Alignment
       {
       int item = findMouseTargetItem ( pt ) ;
       cm = new wxMenu ;
       SeqAlign *al ;
       if ( item != -1 ) al = (SeqAlign*)seq[item] ;
       else al = (SeqAlign*) where ;
       if ( al->myname == txt("t_identity") ) {} // Do nothing
       else if ( al->whatsthis() == "FEATURE" ) {} // Do nothing
       else
          {
          last_al = al ;
          if ( where ) 
             {
             lastclick = pos ;
             wxMenu *cc = new wxMenu ;
             cc->Append ( SEQ_INSERT_GAP , txt("t_mmb_insert_gap") ) ;
             cc->Append ( SEQ_DELETE_GAP , txt("t_mmb_delete_gap") ) ;
             cc->Append ( SEQ_INSERT_OTHER_GAPS , txt("t_mmb_insert_gap_others") ) ;
             cc->Append ( SEQ_DELETE_OTHER_GAPS , txt("t_mmb_delete_gap_others") ) ;
             cm->Append ( SEQ_COPY_AS , txt("m_align_gap") , cc ) ;
             } 

          wxMenu *cb = new wxMenu ;
          TAlignment *ali = (TAlignment*) child ;
          if ( last_al->id > 0 ) cb->Append ( SEQ_UP , txt("t_seq_up") ) ;
          if ( last_al->id + 2 < ali->lines.size() ) cb->Append ( SEQ_DOWN , txt("t_seq_down") ) ;
          if ( last_al->id > 0 ) cb->Append ( SEQ_TOP , txt("t_seq_top") ) ;
          if ( last_al->id + 2 < ali->lines.size() ) cb->Append ( SEQ_BOTTOM , txt("t_seq_bottom") ) ;
          if ( cb->GetMenuItemCount() > 0 ) cm->Append ( SEQ_COPY_AS , txt("t_seq_move") , cb ) ;
          if ( ali->lines[last_al->id].hasFeatures() ) cm->Append ( SEQ_FEAT_HIDE , txt("t_hide_feature_line") ) ;
          else cm->Append ( SEQ_FEAT_SHOW , txt("t_show_feature_line") ) ;
          cm->AppendSeparator () ;
          }
       }
    else
       {
       cm = new wxMenu ;
       }
       
    if ( isMiniDisplay() )
       {
       wxMenu *ca = new wxMenu ;
       ca->Append ( SEQ_AA_VIEW_ORIGINAL , txt("m_aa_view_original") ) ;
       ca->Append ( SEQ_AA_VIEW_REAL , txt("m_aa_view_real") ) ;
       ca->Append ( SEQ_AA_VIEW_CONDENSED , txt("m_aa_view_condensed") ) ;
       cm->Append ( SEQ_AA_VIEW , txt("m_aa_view") , ca ) ;
       }
    
    cm->Append ( SEQ_COPY_IMAGE , txt("m_copy_as_image") ) ;
    cm->Append ( SEQ_SAVE_IMAGE , txt("m_save_as_image") ) ;
    cm->Append ( SEQ_PRINT , txt("m_print_sequence") ) ;
    PopupMenu ( cm , pt ) ;
    delete cm ;
    }    


SeqBasic* SequenceCanvas::findMouseTarget ( wxPoint pt , int &pos )
    {
    int a , b ;
    SeqBasic *ret = NULL ;
    pos = 0 ;
    for ( a = 0 ; a < seq.GetCount() && pos == 0 ; a++ )
        {
        if ( seq[a]->isDisplayOnly() ) continue ;
        b = seq[a]->getLine ( pt.y ) ;

        if ( b != -1 )
           {
           ret = seq[a] ;
           pos = ret->getItem ( pt , b ) ;
           if ( pos == 0 ) ret = NULL ;
           }
        }
    return ret ;
    }

int SequenceCanvas::findMouseTargetItem ( wxPoint pt )
    {
    int a , b ;
    for ( a = 0 ; a < seq.GetCount() ; a++ )
        {
        b = seq[a]->getLine ( pt.y ) ;
        if ( b != -1 ) return a ;
        }
    return -1 ;
    }


// The following handlers are called *only* if this is a
// PrimerDesign canvas!

void SequenceCanvas::OnCopyResultDNA ( wxCommandEvent &ev )
    {
    TVector *nv = getPCR_DNA_vector() ;
    if ( !nv ) return ;
    
    wxString s = nv->getSequence() ;
    delete nv ;

    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }    
    }
    
void SequenceCanvas::OnCopyResultAA ( wxCommandEvent &ev )
    {
    SeqAA *rd = (SeqAA*) seq[seq.GetCount()-2] ;
    wxString s ;
    for ( int a = 0 ; a < rd->s.length() ; a++ )
       { 
       char sa = rd->s.GetChar(a) ;
       if ( sa != ' ' )
          {
          if ( getPD()->aa_state == AA_THREE_1 ||
               getPD()->aa_state == AA_THREE_2 ||
               getPD()->aa_state == AA_THREE_3 )
          s += sa ;
          else s = sa + s ;
          }
       }    
    if ( s.IsEmpty() ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }    
    }
    
void SequenceCanvas::OnSilmut ( wxCommandEvent &ev )
    {
    myass ( getPD() , "SequenceCanvas::OnSilmut" ) ;
    getPD()->OnSilmut ( ev ) ;
    }
    
TVector *SequenceCanvas::getPCR_DNA_vector()
    {
    myass ( getPD() , "SequenceCanvas::OnNewFromResultDNA_0" ) ;
    myass ( getPD()->w , "SequenceCanvas::OnNewFromResultDNA_1" ) ;
    int a , b , c ;
    TVector n ;
    n.setFromVector ( *getPD()->w ) ;
    char blank = '-' ;
    wxString s , t ;
    s = n.getSequence() ;
    t = s ;
    if ( getPD()->w->isCircular() )
        {
        t += t ;
        for ( a = 0 ; a < t.length() && t.GetChar(a) != ' ' ; a++ ) t.SetChar(a,' ') ;
        if ( a == t.length() ) t = s ; // Whole circular vector
        }
    
    for ( a = 0 ; a < t.length() && t.GetChar(a) == ' ' ; a++ ) ;
    if ( a == t.length() ) return NULL ; // Nothing here

    for ( b = a ; b < t.length() && t.GetChar(b) != ' ' ; b++ ) ;
    b-- ;

    for ( c = 0 ; c < n.getSequenceLength() ; c++ )
        n.alterSequence ( c , n.getSequenceChar(c)==' '?'-':n.getSequenceChar(c) ) ;
    return n.newFromMark ( a+1 , b+1 ) ;
    }
    
void SequenceCanvas::OnNewFromResultDNA ( wxCommandEvent &ev )
    {
    myapp()->frame->newFromVector ( getPCR_DNA_vector() ) ;
    }
    
void SequenceCanvas::OnNewFromResultAA ( wxCommandEvent &ev )
    {
    SeqAA *rd = (SeqAA*) seq[seq.GetCount()-2] ;
    wxString s ;
    for ( int a = 0 ; a < rd->s.length() ; a++ )
       {
       char sa = rd->s.GetChar(a) ; 
       if ( sa != ' ' )
          {
          if ( getPD()->aa_state == AA_THREE_1 ||
               getPD()->aa_state == AA_THREE_2 ||
               getPD()->aa_state == AA_THREE_3 )
          s += sa ;
          else s = sa + s ;
          }
       }    
    if ( s.IsEmpty() ) return ;

    wxString seq = s ;
    wxString n = getPD()->vec->getName() + " (" ;
    n += txt ( "t_pcr_result" ) ;
    n += ")" ;
    myapp()->frame->newAminoAcids ( seq , n ) ;
    }

void SequenceCanvas::SilentRefresh ()
    {
//    if ( isHorizontal() ) Clear () ;
    wxClientDC dc(this);
    PrepareDC(dc);
    OnDraw ( dc ) ;
    mylog ( "SequenceCanvas::SilentRefresh" , "Done" ) ;
    }
    
void SequenceCanvas::OnFocus(wxFocusEvent& event)
    {
    event.Skip () ;
    }

void SequenceCanvas::OnKillFocus(wxFocusEvent& event)
    {
    event.Skip () ;
    }

void SequenceCanvas::OnWhatCuts(wxCommandEvent& event)
    {
    myass ( p , "SequenceCanvas::OnWhatCuts_1" ) ;
    TSilmutDialog sd ( p , txt("t_what_cuts") , M_WHATCUTS ) ;
    sd.initme ( p->vec , _from , _to ) ;
    if ( wxID_OK != sd.ShowModal () ) return ;    
    
    int a ;
    TRestrictionEnzyme *e = sd.getEnzyme() ;
    for ( a = 0 ; a < p->vec->re.GetCount() && p->vec->re[a] != e ; a++ ) ;
    if ( a == p->vec->re.GetCount() )
        {
        p->vec->undo.start ( txt("u_what_cuts") ) ;
        p->vec->re.Add ( e ) ;
        p->vec->recalculateCuts() ;
        p->vec->recalcvisual = true ;
        p->vec->undo.stop () ;
        p->cPlasmid->Refresh() ;
        p->updateSequenceCanvas ( true ) ;
        p->treeBox->initme() ;
        p->Refresh () ;
        }
    }
    

void SequenceCanvas::OnSeqUp ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnSeqUp" ) ;
    getAln()->MoveUpDown ( last_al->id , last_al->id - 1 ) ;
    }

void SequenceCanvas::OnSeqDown ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnSeqDown" ) ;
    getAln()->MoveUpDown ( last_al->id , last_al->id + 1 ) ;
    }

void SequenceCanvas::OnSeqTop ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnSeqTop" ) ;
    getAln()->MoveUpDown ( last_al->id , 0 ) ;
    }

void SequenceCanvas::OnSeqBottom ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnSeqBottom" ) ;
    getAln()->MoveUpDown ( last_al->id , getAln()->lines.size()-1 ) ;
    }

void SequenceCanvas::OnToggleFeat ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnToggleFeat" ) ;
    TAlignLine *all = &getAln()->lines[last_al->id] ;
    if ( all->hasFeatures() ) all->hideFeatures() ;
    else all->showFeatures() ;
    getAln()->redoAlignments ( false ) ;
    }
    
void SequenceCanvas::OnInsertGap ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnInsertGap" ) ;
    getAln()->callMiddleMouseButton ( last_al->id , lastclick , "t_mmb_insert_gap" ) ;
    }
    
void SequenceCanvas::OnDeleteGap ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnDeleteGap" ) ;
    getAln()->callMiddleMouseButton ( last_al->id , lastclick , "t_mmb_delete_gap" ) ;
    }
    
void SequenceCanvas::OnInsertOtherGaps ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnInsertOtherGaps" ) ;
    getAln()->callMiddleMouseButton ( last_al->id , lastclick , "t_mmb_insert_gap_others" ) ;
    }
    
void SequenceCanvas::OnDeleteOtherGaps ( wxCommandEvent &ev )
    {
    myass ( getAln() , "SequenceCanvas::OnDeleteOtherGaps" ) ;
    getAln()->callMiddleMouseButton ( last_al->id , lastclick , "t_mmb_delete_gap_others" ) ;
    }
    
void SequenceCanvas::OnViewOriginal ( wxCommandEvent &ev )
    {
    myass ( getAA() , "SequenceCanvas::OnViewOriginal" ) ;
    getAA()->miniDisplayOptions = MINI_DISPLAY_ORIGINAL ;
    getAA()->OnListBox ( ev ) ;
    }

void SequenceCanvas::OnViewReal ( wxCommandEvent &ev )
    {
    myass ( getAA() , "SequenceCanvas::OnViewReal" ) ;
    getAA()->miniDisplayOptions = MINI_DISPLAY_REAL ;
    getAA()->OnListBox ( ev ) ;
    }

void SequenceCanvas::OnViewCondensed ( wxCommandEvent &ev )
    {
    myass ( getAA() , "SequenceCanvas::OnViewCondensed" ) ;
    getAA()->miniDisplayOptions = MINI_DISPLAY_CONDENSED ;
    getAA()->OnListBox ( ev ) ;
    }

void SequenceCanvas::OnSelAsNewPrimer ( wxCommandEvent &ev )
    {
    if ( _from < 0 ) return ;
    if ( !getPD() ) return ;
    int id = -1 ;
    if ( seq[lastmarked]->whatsthis() == "PRIMER_UP" ) id = lastmarked + 1 ;
    if ( seq[lastmarked]->whatsthis() == "PRIMER_DOWN" ) id = lastmarked - 1 ;
    if ( id == -1 ) return ;
    wxString sel = seq[id]->s.substr ( _from-1 , _to-_from+1 ) ;
    if ( seq[lastmarked]->whatsthis() == "PRIMER_DOWN" )
        {
        int a ;
        wxString sel2 ;
        for ( a = 0 ; a < sel.length() ; a++ )
           sel2 = sel.GetChar(a) + sel2 ;
        sel = sel2 ;
        }
    getPD()->AddPrimer ( sel ) ;
    }
        
TAminoAcids *SequenceCanvas::getAA()
    {
    if ( !child ) return NULL ;
    if ( child->def != "AminoAcids" ) return NULL ;
    return (TAminoAcids*) child ;
    }
    
TPrimerDesign *SequenceCanvas::getPD()
    {
    if ( !child ) return NULL ;
    if ( child->def != "PrimerDesign" ) return NULL ;
    return (TPrimerDesign*) child ;
    }

TAlignment *SequenceCanvas::getAln()
    {
    if ( !child ) return NULL ;
    if ( child->def != "alignment" ) return NULL ;
    return (TAlignment*) child ;
    }

void SequenceCanvas::startEdit ( wxString id )
    {
    mylog ( "startEdit" , "1" ) ;
    setEditMode ( true ) ;
    findID(id)->s.Append ( " " ) ;
    if ( child ) child->vec->addToSequence ( " " ) ;
    for ( int a = 0 ; a < seq.GetCount() ; a++ ) seq[a]->editMode ( true ) ;
    arrange () ;
    if ( _from == -1 ) mark ( id , 1 , 1 , 2 ) ;
    else mark ( id , _from , _from , 2 ) ;
    SetFocus() ;
    mylog ( "startEdit" , "END" ) ;
    }
    
void SequenceCanvas::stopEdit ()
    {
    mylog ( "stopEdit" , "1" ) ;
    setEditMode ( false ) ;
    if ( child && child->vec )
    	{
	    if ( child->vec->getSequenceLength() < 2 ) child->vec->setSequence ( "" ) ;
        else child->vec->eraseSequence ( child->vec->getSequenceLength()-1 , 1 ) ;
        }    
    wxString id ;
    if ( lastmarked >= 0 && lastmarked < seq.GetCount() )
        {
        id = seq[lastmarked]->whatsthis() ;
//        if ( child ) seq[lastmarked]->s = child->vec->getSequence () ;
//        else seq[lastmarked]->s.erase ( seq[lastmarked]->s.length()-1 , 1 ) ;
        }
    mylog ( "stopEdit" , "2" ) ;
    for ( int a = 0 ; a < seq.GetCount() ; a++ ) seq[a]->editMode ( false ) ;
    arrange () ;
    mylog ( "stopEdit" , "3" ) ;
    if ( id != "" ) unmark () ; //mark ( id , -1 , -1 ) ;
    mylog ( "stopEdit" , "END" ) ;
    SilentRefresh () ;
    }

void SequenceCanvas::rsHideLimit ( wxCommandEvent &ev )
    {
    if ( !p ) return ;
    wxTextEntryDialog ted ( this , txt("m_hide_enzymes_limit") , "" , "2" ) ;
    if ( wxID_OK != ted.ShowModal() ) return ;
    int limit = atoi ( ted.GetValue().c_str() ) ;
    int a , b ;
    for ( a = 0 ; a < p->vec->re.GetCount() ; a++ )
       {
       int cnt = 0 ;
       for ( b = 0 ; b < p->vec->rc.size() ; b++ )
          if ( p->vec->rc[b].e == p->vec->re[a] ) cnt++ ;
       for ( b = 0 ; b < p->vec->hiddenEnzymes.GetCount() && p->vec->hiddenEnzymes[b] != p->vec->re[a]->name ; b++ ) ;
       if ( cnt > limit && b == p->vec->hiddenEnzymes.GetCount() )
          p->treeBox->ToggleEnzymeVisibility ( p->vec->re[a] ) ;
       }
    p->vec->recalculateCuts() ;
    p->vec->recalcvisual = true ;
    if ( p && p->cPlasmid ) p->cPlasmid->Refresh () ;
    arrange () ;
    Refresh () ;
    }
    
void SequenceCanvas::OnBacktranslate ( wxCommandEvent &ev )
    {
    int a ;
    TVS cc ;
    cc.push_back ( txt("t_codon_catalog_abstract") ) ;
    
    // Read available code catalog groups
    wxTextFile in ( myapp()->homedir + "/codon_catalog.csv" ) ;
    in.Open () ;
    wxString s = in[0] ;
    in.Close () ;
    wxArrayString as ;
    explode ( "," , s , as ) ;
    for ( a = 2 ; a < as.GetCount() ; a++ )
    	{
	    s = as[a] ;
	    s.Replace ( "\"" , "" ) ;
    	cc.push_back ( s ) ;
     	}   	
    
    // Ask to specify code catalog group
    wxString *list = new wxString[cc.size()] ;
    for ( a = 0 ; a < cc.size() ; a++ ) list[a] = cc[a] ;
    wxSingleChoiceDialog scd ( (wxWindow*)this , "" , txt("t_codon_catalog") , cc.size() , list ) ;
    if ( scd.ShowModal() != wxID_OK ) return ;
    
    // Set up code catalog group
    wxString mode ;
    if ( scd.GetSelection() > 0 )
    	mode = scd.GetStringSelection() ;
   	
   	// Run the damn thing!
    TVector *nv = getAA()->vec->backtranslate ( mode ) ;
    nv->setDescription ( txt("t_backtranslated") + mode + "\n" + nv->getDescription() ) ;
    myapp()->frame->newFromVector ( nv , TYPE_VECTOR ) ;
    }    
    
void SequenceCanvas::OnPaste ( wxCommandEvent &ev )
    {
    if ( !getEditMode() ) return ;
    if (!wxTheClipboard->Open()) return ;
    if (!wxTheClipboard->IsSupported( wxDF_TEXT )) return ;

    if ( p && p->vec ) p->vec->undo.start ( txt("u_cut") ) ;

    wxTextDataObject data;
    wxTheClipboard->GetData( data );
    wxString s = data.GetText() ;
    wxTheClipboard->Close();
    
    int a ;
    doHide ( true ) ;
    SetCursor ( *wxHOURGLASS_CURSOR ) ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        wxKeyEvent ev ;
        ev.m_keyCode = s.GetChar(a) ;
        OnCharHook ( ev ) ;
        }
    doHide ( false ) ;
    Refresh () ;
    SetCursor ( *wxSTANDARD_CURSOR ) ;
    if ( p && p->vec ) p->vec->undo.stop() ;    
    }
    
void SequenceCanvas::OnInsertRestrictionSiteLeft ( wxCommandEvent &ev )
    {
    insertRestrictionSite ( true ) ;
    }    
    
void SequenceCanvas::OnInsertRestrictionSiteRight ( wxCommandEvent &ev )
    {
    insertRestrictionSite ( false ) ;
    }    

void SequenceCanvas::insertRestrictionSite ( bool left )
    {
    if ( _from == -1 ) return ;
    if ( getAA() ) return ;
    TVector *v = NULL ;
    if ( child ) v = child->vec ;
    if ( !v ) return ;
    wxBeginBusyCursor() ;
    wxArrayString vs ;
    myapp()->frame->LS->getEnzymesInGroup ( txt("All") , vs ) ;
    vs.Sort () ;
    wxString *as = new wxString [ vs.GetCount() ] ;
    for ( int a = 0 ; a < vs.GetCount() ; a++ ) as[a] = vs[a] ;
    wxSingleChoiceDialog scd ( this , txt("t_vec_enzym") , "" , vs.GetCount() , as ) ;
    wxEndBusyCursor() ;
    if ( wxID_OK != scd.ShowModal() )
        {
        delete [] as ;
        return ;
        }    
    delete [] as ;
    
    TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme(scd.GetStringSelection()) ;
    wxString se = e->sequence ;

    int a , p1 = _from - se.length() - 1 ;
    if ( !left ) p1 = _to ;
    
    TVector tv ;
    tv.setFromVector ( *v ) ;
    tv.setSequence ( seq[lastmarked]->s ) ;
    for ( a = 0 ; a < se.length() ; a++ )
        {
        char c = se.GetChar(a) ;
        if ( seq[lastmarked]->whatsthis() == "PRIMER_DOWN" )
           c = tv.getComplement ( c ) ;
        tv.setNucleotide ( p1+a , c ) ;
        }    
    seq[lastmarked]->s = tv.getSequence() ;


    for ( a = 0 ; a < v->re.GetCount() && v->re[a] != e ; a++ ) ;
    if ( a == v->re.GetCount() ) v->re.Add ( e ) ;

    if ( getPD() ) v = NULL ;
    wxString id = seq[lastmarked]->whatsthis() ;
    TMarkMem m ( this ) ;
    updateEdit ( v , id , _from ) ;
    m.remark() ;

    if ( !getPD() ) return ;
    wxCommandEvent event ;
    getPD()->OnSpinText( event);
    }        


void SequenceCanvas::OnStrandCopy53(wxCommandEvent& event)
	{
	if ( !p || !p->vec ) return ;
	wxString s = p->vec->getStrand53() ;
	s = "5'->3' : 5'-" + s + "-3'" ;
    if (!wxTheClipboard->Open()) return ;
    wxTheClipboard->SetData( new wxTextDataObject(s) );
    wxTheClipboard->Close();
	}    

void SequenceCanvas::OnStrandCopy35(wxCommandEvent& event)
	{
	if ( !p || !p->vec ) return ;
	wxString s = p->vec->getStrand35() ;
	s = "3'->5' : 5'-" + s + "-3'" ;
    if (!wxTheClipboard->Open()) return ;
    wxTheClipboard->SetData( new wxTextDataObject(s) );
    wxTheClipboard->Close();
	}    

void SequenceCanvas::OnStrandCopyBoth(wxCommandEvent& event)
	{
	if ( !p || !p->vec ) return ;
	wxString s = p->vec->getStrand53() ;
	s = "5'->3' : 5'-" + s + "-3'\n" ;
	s += "3'->5' : 5'-" + p->vec->getStrand35() + "-3'" ;
    if (!wxTheClipboard->Open()) return ;
    wxTheClipboard->SetData( new wxTextDataObject(s) );
    wxTheClipboard->Close();
	}    

void SequenceCanvas::OnStrandNew53(wxCommandEvent& event)
	{
	if ( !p || !p->vec ) return ;
	int a ;
	wxString vname = p->vec->getName() ;
	p->vec->setName ( vname + " (5'->3')" ) ;
	MyChild *c = p->doTransformSequence ( true , false , false ) ;
	p->vec->setName ( vname ) ;

	wxString lu = p->vec->getStickyEnd ( true , true ) ;
	for ( a = 0 ; a < lu.length() ; a++ )
		c->vec->insert_char ( lu.GetChar(a) , a+1 , false ) ;
	c->vec->addToSequence ( p->vec->getStickyEnd ( false , true ) ) ;

	c->vec->setStickyEnd ( true , true , "" ) ;
	c->vec->setStickyEnd ( false , true , "" ) ;
	c->vec->setStickyEnd ( true , false , "" ) ;
	c->vec->setStickyEnd ( false , false , "" ) ;
	c->cSequence->findID("DNA")->initFromTVector ( c->vec ) ;
	c->updateSequenceCanvas ( false ) ;
	}    

void SequenceCanvas::OnStrandNew35(wxCommandEvent& event)
	{
	if ( !p || !p->vec ) return ;
	int a ;
	wxString vname = p->vec->getName() ;
	p->vec->setName ( vname + " (3'->5')" ) ;
	MyChild *c = p->doTransformSequence ( true , false , true ) ;
	p->vec->setName ( vname ) ;

	wxString rl = p->vec->getStickyEnd ( false , false ) ;
	for ( a = 0 ; a < rl.length() ; a++ )
		c->vec->insert_char ( rl.GetChar(rl.length()-a-1) , a+1 , false ) ;
	wxString ll = p->vec->getStickyEnd ( true , false ) ;
	for ( a = 0 ; a < rl.length() ; a++ )
		c->vec->insert_char ( ll.GetChar(ll.length()-a-1) , c->vec->getSequenceLength()+1 , false ) ;

	c->vec->setStickyEnd ( true , true , "" ) ;
	c->vec->setStickyEnd ( false , true , "" ) ;
	c->vec->setStickyEnd ( true , false , "" ) ;
	c->vec->setStickyEnd ( false , false , "" ) ;
	c->cSequence->findID("DNA")->initFromTVector ( c->vec ) ;
	c->updateSequenceCanvas ( false ) ;
	}    

void SequenceCanvas::OnStrandNewBoth(wxCommandEvent& event)
	{
	OnStrandNew53 ( event ) ;
	OnStrandNew35 ( event ) ;
	}    


// -------------------------------------------------------- TMarkMem

TMarkMem::TMarkMem ( SequenceCanvas *_sc )
    {
    sc = _sc ;
    if ( !sc ) return ;
    f = sc->markedFrom() ;
    t = sc->markedTo() ;
    l = sc->lastmarked ;
    }
    
void TMarkMem::remark()
    {
    if ( !sc ) return ;
    if ( l >= 0 && l < sc->seq.GetCount() && f != -1 )
       {
       wxString ls = sc->seq[l]->whatsthis() ;
       int len = sc->seq[l]->s.length() ;
       if ( t > len ) t -= len ;
       sc->mark ( ls , f , t , sc->getEditMode() ? 2 : 1 ) ;
       }
    }

void TMarkMem::unmark()
    {
    if ( !sc ) return ;
    if ( l >= 0 && l < sc->seq.GetCount() && f != -1 )
       {
       sc->unmark() ;
//       wxString ls = sc->seq[l]->whatsthis() ;
//       sc->mark ( ls , -1 , -1 ) ;
       }
    }
    
// -------------------------------------------------------- SeqPos


void SeqPos::cleanup ()
    {
    p.Clear () ;
    m.Clear () ;
    r.clear () ;
    l.clear () ;
    mark_from.Clear () ;
    mark_to.Clear () ;
    }
    
void SeqPos::add ( int np , int x , int y , int w , int h , bool memsave )
    {
    if ( !memsave ) p.Add ( np ) ;
    r.push_back ( wxRect ( x , y , w , h ) ) ;
    if ( !memsave ) m.Append ( ' ' ) ;
    }
    
void SeqPos::addline ( int from , int to , int vfrom , int vto )
    {
    l.push_back ( wxRect ( from , to , vfrom , vto ) ) ;
    }
    
int SeqPos::getLine ( int y )
    {
    int ret = -1 ;
    for ( int a = 0 ; a < l.size() /*&& ret == -1 */; a++ )
       {
       if ( l[a].width <= y && l[a].height >= y )
          return a ;//ret = a ;
       }
    return -1 ; //ret ;
    }
    
int SeqPos::getItem ( wxPoint pt , int line )
    {
    int a ;
    for ( a = l[line].x ; a <= l[line].y ; a++ )
       if ( pt.x >= r[a].x && pt.x <= r[a].x+r[a].width &&
            pt.y >= r[a].y && pt.y <= r[a].y+r[a].height )
          return a ;
    return 0 ;
    }
    
void SeqPos::reserve ( int n , int n2 , bool memsave )
    {
    r.reserve ( n ) ;
    if ( !memsave ) l.reserve ( n2 == -1 ? n / 5 : n2 ) ; // Guessing...
    if ( !memsave ) p.Alloc ( n ) ;
    if ( !memsave ) m.Alloc ( n ) ;
    }
        
void SeqPos::mark ( int where , int value )
    {
    int a ;
    if ( value < 1 )
        {
        for ( a = 0 ; a < mark_from.GetCount() ; a++ )
           {
           if ( where >= mark_from[a] && where <= mark_to[a] )
              {
              if ( where == mark_from[a] ) mark_from[a]++ ;
              else if ( where == mark_to[a] ) mark_to[a]-- ;
              else
                 {
                 mark_from.Add ( where + 1 ) ;
                 mark_to.Add ( mark_to[a] ) ;
                 mark_to[a] = where - 1 ;
                 }    
              }    
           }    
        for ( a = 0 ; a < mark_from.GetCount() ; a++ )
           {
           if ( mark_from[a] > mark_to[a] )
              {
              mark_from.RemoveAt ( a ) ;
              mark_to.RemoveAt ( a ) ;
              a-- ;
              }    
           }    
        return ;
        }    
    mark_value = value ;
    for ( a = 0 ; a < mark_from.GetCount() ; a++ )
        {
        if ( where == mark_from[a]-1 ) { mark_from[a] = where ; return ; }
        if ( where == mark_to[a]+1 ) { mark_to[a] = where ; return ; }
        }    
    mark_from.Add ( where ) ;
    mark_to.Add ( where ) ;
    }
        
int SeqPos::getmark ( int where )
    {
    int a ;
    for ( a = 0 ; a < mark_from.GetCount() ; a++ )
        {
        if ( mark_from[a] <= where && mark_to[a] >= where ) return mark_value ;
        }    
    return -1 ;
    }

//------------------------------------------------------------- SequencePartList

int SequencePartList::getID ( int internalID ) { return vi[internalID] ; }
int SequencePartList::getFrom ( int internalID ) { return vx[internalID] ; }
int SequencePartList::getTo ( int internalID ) { return vy[internalID] ; }

void SequencePartList::prepare ( int size )
    {
    vi.Clear () ;
    vx.Clear () ;
    vy.Clear () ;
    vl.Clear () ;
    vl2.clear () ;
    vi.Alloc ( size ) ;
    vl.Alloc ( size ) ;
    vx.Alloc ( size ) ;
    vy.Alloc ( size ) ;
    }    

void SequencePartList::add ( int id , int from , int to )
    {
    vi.Add ( id ) ;
    vx.Add ( from ) ;
    vy.Add ( to ) ;
    vl.Add ( 0 ) ;
    }    
    
void SequencePartList::makeLevels ()
    {
    int a , b ;
    wxString t ;
    maxlevels = 0 ;
    vl2.clear () ;
    for ( a = 0 ; a < vi.GetCount() ; a++ )
        {
        wxString t ( ' ' , maxlevels + 5 ) ;
        for ( b = 0 ; b < a ; b++ )
           {
           if ( vx[b] <= vy[a] && vy[b] >= vx[a] )
              t.SetChar ( vl[b] , '*' ) ;
           }    
        vl[a] = t.First ( ' ' ) ;
        if ( vl[a]+1 > maxlevels ) maxlevels = vl[a]+1 ;
        while ( vl2.size() < maxlevels ) vl2.push_back ( wxArrayInt() ) ;
        vl2[vl[a]].Add ( a ) ;
        }    
    }    
    
int SequencePartList::here ( int pos , int level )
    {
    for ( int a = 0 ; a < vl2[level].GetCount() ; a++ )
        {
        int b = vl2[level][a] ;
        if ( pos >= vx[b] && pos <= vy[b] ) return b ;
        if ( vy[b] < vx[b] )
           {
           if ( pos >= vx[b] || pos <= vy[b] ) return b ;
           }    
        else if ( vy[b] >= slen )
           {
           if ( pos >= vx[b] || pos <= vy[b] - slen ) return b ;
           }    
        }    
    return -1 ;
    }    
    
int SequencePartList::size ()
    {
    return vi.GetCount() ;
    }    

int SequencePartList::getLevel ( int i )
    {
    return vl[i] ;
    }    
    
