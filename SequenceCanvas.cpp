#include "SequenceCanvas.h"
#include <wx/printdlg.h>
#include <wx/datetime.h>

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

    EVT_MENU(SEQ_UP,SequenceCanvas::OnSeqUp)
    EVT_MENU(SEQ_DOWN,SequenceCanvas::OnSeqDown)
    EVT_MENU(SEQ_TOP,SequenceCanvas::OnSeqTop)
    EVT_MENU(SEQ_BOTTOM,SequenceCanvas::OnSeqBottom)
    EVT_MENU(SEQ_FEAT_HIDE,SequenceCanvas::OnToggleFeat)
    EVT_MENU(SEQ_FEAT_SHOW,SequenceCanvas::OnToggleFeat)

    EVT_MENU(PRIMER_FORWARD, SequenceCanvas::OnPrimerForward)
    EVT_MENU(PRIMER_BACKWARD, SequenceCanvas::OnPrimerBackward)
    EVT_MENU(PRIMER_BOTH, SequenceCanvas::OnPrimerBoth)
    EVT_MENU(PRIMER_MUTATION, SequenceCanvas::OnPrimerMutation)

    EVT_MENU(MDI_CUT, SequenceCanvas::OnCut)
    EVT_MENU(MDI_COPY, SequenceCanvas::OnCopy)

    EVT_SET_FOCUS(SequenceCanvas::OnFocus)
    EVT_KILL_FOCUS(SequenceCanvas::OnKillFocus)
END_EVENT_TABLE()

// Define a constructor for my canvas
SequenceCanvas::SequenceCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size, wxSUNKEN_BORDER|wxHSCROLL)
{
    SetBackgroundColour(wxColour("WHITE"));

    font = new wxFont ( 12 , wxMODERN , wxNORMAL , wxNORMAL ) ; 
    smallFont = new wxFont ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    varFont = new wxFont ( 11 , wxROMAN  , wxNORMAL , wxNORMAL ) ;
    charwidth = 0 ;
    charheight = 0 ;
    blankline = 0 ;
    lastpos = -1 ;
    lastmarked = -1 ;
    p = NULL ;
    aa = NULL ;
    pd = NULL ;
    doHide = false ;
    forceoverwrite = false ;
    wantOverwrite = false ;
    maxendnumberlength = 9 ;
    lastyoffset = 0 ;
    blocksize = 10 ;

    m_dirty = FALSE;
    editMode = false ;
    edit_id = "DNA" ;
    edit_valid = "ACTG" ;
    _from = -1 ;
    drawall = false ;
    printing = false ;
    print_dc = NULL ;
    child = NULL ;   
    isHorizontal = false ; 
    }

SeqBasic* SequenceCanvas::findID ( string id ) 
    {
    int a ;
    for ( a = 0 ; a < seq.size() ; a++ )
        if ( seq[a]->whatsthis() == id )
           return seq[a] ;
    return NULL ;
    }
    
void SequenceCanvas::MyGetClientSize ( int *w , int *h )
    {
    if ( printing )
       {
       print_dc->GetSize ( w , h ) ;
       }
    else
       {
       GetClientSize ( w , h ) ;
//       if ( isHorizontal ) *w = 1000000 ;
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
       if ( isHorizontal ) *w = 1000000 ;
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
       if ( isHorizontal ) size.SetWidth ( 1000000 ) ;
       return size ;
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
    
void SequenceCanvas::updateEdit ( TVector *v , string id , int from )
    {
    if ( aa )
        {
        aa->vec->setChanged () ;
        for ( int a = 0 ; a < seq.size() ; a++ )
            {
            if ( seq[a]->whatsthis() == "AA" )
               ((SeqAA*) seq[a])->initFromString ( aa->vec->sequence ) ;
            }
        arrange () ;
        aa->showStat() ;
        }
    else if ( v )
        {
        v->setChanged () ;
        for ( int a = 0 ; a < seq.size() ; a++ )
            seq[a]->initFromTVector ( v ) ;
        arrange () ;
        }
    else if ( pd )
        {
/*        for ( int a = 0 ; a < seq.size() ; a++ )
            {
            if ( seq[a]->whatsthis() == edit_id )
               ((SeqAA*) seq[a])->initFromString ( aa->vec->sequence ) ;
            }*/
        seq.pop_back () ;
        seq.pop_back () ;
        seq.pop_back () ;
        seq.pop_back () ;
        pd->updatePrimersFromSequence () ;
        pd->updateResultSequence() ;
        arrange () ;
        }
    mark ( id , from , from , 2 ) ;
    }

void SequenceCanvas::OnCharHook(wxKeyEvent& event)
    {
    if ( seq.size() == 0 ) return ;
    int k = event.GetKeyCode () ;
    if ( editMode )
        {
        int a , b = -1 ;
        string id = edit_id , valid = edit_valid ;
        for ( a = 0 ; a < seq.size() && b == -1 ; a++ )
            if ( seq[a]->whatsthis() == id )
               b = a ;
        if ( b == -1 )
           {
           event.Skip() ; // Illegal ID
           return ;
           }
        SeqDNA *dna = (SeqDNA*) seq[b] ;
//        int from = p->cPlasmid->mark_from ;
        int from = _from ;
        string *the_sequence ;
        if ( dna->whatsthis() == "AA" ) the_sequence = &aa->vec->sequence ;
        else if ( dna->whatsthis() == "DNA" ) the_sequence = &dna->vec->sequence ;
        else the_sequence = &dna->s ;
        TVector *v = NULL ;
        if ( p ) v = dna->vec ;

        // Getting the layout
        int vx , vy , wx , wy ;
        MyGetViewStart ( &vx , &vy ) ;
        MyGetClientSize ( &wx , &wy ) ;
        wy /= charheight ;
        wy -= wy % ( seq.size() + 1 ) ;
        
        if ( k >= 'a' && k <= 'z' ) k = k - 'a' + 'A' ;
        for ( a = 0 ; a < valid.length() && valid[a] != k ; a++ ) ;
        if ( a == valid.length() )
           {
           int cpl = dna->itemsperline ;
           int page = wy * cpl / ( seq.size()+blankline ) ;
           if ( k == WXK_RIGHT )
              {
              if ( event.ControlDown () )
                 {
                 from += blocksize - 1 ;
                 from -= from % blocksize ;
                 if ( from > the_sequence->length() ) from = the_sequence->length();
                 mark ( id , from+1 , from+1 , 2 ) ;
                 }
              else if ( from < the_sequence->length() )
                 mark ( id , from+1 , from+1 , 2 ) ;
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
              mark ( id , from+cpl , from+cpl , 2 ) ;
           else if ( k == WXK_UP && from-cpl > 0 )
              mark ( id , from-cpl , from-cpl , 2 ) ;
           else if ( k == WXK_HOME )
              mark ( id , 1 , 1 , 2 ) ;
           else if ( k == WXK_END )
              mark ( id , the_sequence->length() , the_sequence->length() , 2 ) ;
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
           else if ( !forceoverwrite && k == WXK_DELETE && from <= the_sequence->length()-1 )
              {
              if ( p && v ) v->doRemove ( from , from , false ) ;
              else the_sequence->erase ( from-1 , 1 ) ;
              if ( from > the_sequence->length() ) from = the_sequence->length() ;
              if ( v ) v->recalculateCuts() ;
              updateEdit ( v , id , from ) ;
              }
           else if ( !forceoverwrite && k == WXK_BACK && from > 1 )
              {
              if ( p && v ) v->doRemove ( from-1 , from-1 , false ) ;
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
              return ;
              }
           }
        else
           {
           string dummy ;
           dummy = (char) k ;
           if ( v )
              {
              v->insert_char ( k , from , doOverwrite() ) ;
              v->recalculateCuts() ;
              }
           else
              {
              if ( doOverwrite() ) the_sequence->erase ( from-1 , 1 ) ;
              the_sequence->insert ( from-1 , dummy ) ;
              }
           updateEdit ( v , id , from + 1 ) ;
           }
           
        // Scrolling so the cursor is visible
        int ny = vy ;
        while ( ny > 0 && ny > vpy ) ny-- ;
        while ( ny + wy <= vpy ) ny++ ;
        Scroll ( -1 , ny ) ;
        if ( p )
           wxLogStatus(txt("seq_ed_loc"), p->cPlasmid->mark_from ) ;
        }
    else event.Skip() ;
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
    else if ( pd )
        {
        TVector *vec = pd->w ;
        TVectorEditor ve ( this , txt("t_vector_editor") , vec ) ;
        ve.initialViewEnzyme ( "" ) ;
        ve.hideProp = true ;
        ve.hideItem = true ;
        ve.hideEm () ;
        int x = ve.ShowModal () ;
        ve.cleanup () ;
        if ( x == wxID_OK )
            {
            for ( x = 0 ; x < 4 ; x++ ) seq.pop_back() ;
            pd->updateResultSequence() ;
            arrange () ;
            Refresh () ;
            }
        }
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
    else if ( aa )
        {
        string seq = getSelection() ;
        myapp()->frame->blast ( seq , "blastp" ) ;
        }
    }

void SequenceCanvas::OnCut ( wxCommandEvent &ev )
    {
    if ( p ) { p->OnCut ( ev ) ; return ; }
    if ( !aa || _from == -1 ) return ;
    string s = getSelection () ;
    if ( s == "" ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();

        aa->vec->doRemove ( _from , _to ) ;
        _from = -1 ;
        
        for ( int a = 0 ; a < seq.size() ; a++ )
           {
           if ( seq[a]->whatsthis() == "AA" )
              {
              SeqAA *x = (SeqAA*) seq[a] ;
              x->initFromString ( aa->vec->sequence ) ;
              }
//           else seq[a]->initFromTVector ( aa->vec ) ;
           }

        arrange() ;
        Refresh() ;
        }
    }

void SequenceCanvas::OnCopyText ( wxCommandEvent &ev )
    {
    int a ;
    string s , t ;
    vector <string> out ;
    int minline = -1 , maxline = -1 ;
    for ( a = 0 ; a < seq.size() ; a++ )
        {
        wxPoint p = seq[a]->showText ( a , out ) ;
        if ( p.x >= 0 ) p.x -= a ;
        if ( p.y >= 0 )
           {
           p.y -= a ;
           p.y += seq.size()-1 ;
           }
        if ( ( minline == -1 || p.x < minline ) && p.x >= 0 ) minline = p.x ;
        if ( p.y > maxline && p.y >= 0 ) maxline = p.y ;
        }

    while ( out.size() && out[out.size()-1] == "" )
        out.pop_back() ;

    if ( minline < 0 ) minline = 0 ;
    if ( maxline < 0 ) maxline = out.size()-1 ;

    s = "" ;
    for ( a = minline ; a <= maxline ; a++ )
//    for ( a = 0 ; a < out.size() ; a++ )
        s += out[a] + "\n" ;

    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }
    }

void SequenceCanvas::OnPrint ( wxCommandEvent &ev )
    {
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;
    
    wxPrintDialogData pdd = pd.GetPrintDialogData() ;
    wxPrintData pD = pdd.GetPrintData() ;
    printToColor = pD.GetColour() ;
    bool wasHorizontal = isHorizontal ;
    isHorizontal = false ;
//    if ( printToColor ) wxMessageBox ( "Color!" ) ;

    TMarkMem mm ( this ) ; // Temporary mark for printing

    string name = "GENtle" ;
    if ( p && p->vec ) name = p->vec->name ;

    print_dc = pd.GetPrintDC () ;
    print_dc->StartDoc ( name.c_str() ) ;
    printing = true ;
    drawall = true ;
    
    int w , h ;
    MyGetSize ( &w , &h ) ;
    int pagetop = h / 20 ;
    int pagebottom = h / 40 ;
    int vish = h - pagetop - pagebottom ;
    
    int fs = w / 10 ;
    if ( aa ) fs *= 2 ;
    
    wxFont *oldfont = font ;
    wxFont *oldsmallFont = smallFont ;
    wxFont bigfont ( w/30 , wxMODERN , wxNORMAL , wxNORMAL ) ;
    wxFont medfont ( w/80 , wxMODERN , wxNORMAL , wxNORMAL ) ;
    font = new wxFont ( fs/10 , wxMODERN , wxNORMAL , wxNORMAL ) ; 
    smallFont = new wxFont ( fs/15 , wxSWISS , wxNORMAL , wxNORMAL ) ;

    print_maxx = -w ;

    OnDraw ( *print_dc ) ;

    mm.unmark () ;
    mm.remark () ;

    int xoff = ( w - print_maxx ) / 2 ;

    int yoff = -pagetop ;
    int page = 0 ;
    int totalpages = 0 ;

    int dummy = vish % charheight ;
    int lines = ( vish - dummy ) / charheight ;
    lines %= seq.size() + blankline ;
    dummy += lines * charheight ;
    
    wxDateTime now = wxDateTime::Now();
    string printtime = now.Format("%c", wxDateTime::Local).c_str() ;

    
    do {
       totalpages++ ;
       yoff += vish - dummy ;
       } while ( yoff < lowy ) ;
       
    yoff = -pagetop ;
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
       
       int tw , th ;
       string s ;
       // Title
       if ( p ) s = p->vec->name ;
       else if ( aa ) s = aa->vec->name ;
       else if ( child ) s = child->getName() ;
       print_dc->SetTextBackground ( *wxWHITE ) ;
       print_dc->SetFont ( bigfont ) ;
       print_dc->GetTextExtent ( s.c_str() , &tw , &th ) ;
       print_dc->DrawText ( s.c_str() ,
                            ( w - tw ) / 2 - xoff ,
                            yoff + ( pagetop - th ) / 2 ) ;
       print_dc->SetTextBackground ( *wxWHITE ) ;

       // Page number
       char t[100] ;
       sprintf ( t , txt("t_page_of") , page , totalpages ) ;
       print_dc->SetFont ( medfont ) ;
       print_dc->GetTextExtent ( t , &tw , &th ) ;
       print_dc->DrawText ( t ,
                            w - tw - xoff * 2 ,
                            yoff + h - ( pagebottom + dummy + th ) / 2 ) ;

       // Date
       print_dc->SetFont ( medfont ) ;
       print_dc->GetTextExtent ( printtime.c_str() , &tw , &th ) ;
       print_dc->DrawText ( printtime.c_str() ,
                            0 ,
                            yoff + h - ( pagebottom + dummy + th ) / 2 ) ;
       
       
       print_dc->EndPage () ;

       yoff += vish - dummy ;
       } while ( yoff < lowy ) ;

    drawall = false ;
    printing = false ;
    delete font ;
    delete smallFont ;
    font = oldfont ;
    smallFont = oldsmallFont ;
    
    print_dc->EndDoc () ;
    delete print_dc ;
    print_dc = NULL ;
    isHorizontal = wasHorizontal ;
    charwidth = 0 ; // To force re-arrange
    Refresh () ;
    
    mm.unmark () ;
    mm.remark () ;
/*    if ( __id > -1 && __from != -1 )
       {
       mark ( seq[__id]->whatsthis() , -1 , -1 ) ;
       mark ( seq[__id]->whatsthis() , __from , __to ) ;
       }*/

    }

void SequenceCanvas::OnSaveImage ( wxCommandEvent &ev )
    {
    wxString wildcard = "Bitmap (*.bmp)|*.bmp" ; 
    string lastdir = myapp()->frame->LS->getOption ( "LAST_IMPORT_DIR" , "C:" ) ;
    wxFileDialog d ( this , txt("t_save_image") , lastdir.c_str() , "" , wildcard , wxSAVE|wxOVERWRITE_PROMPT ) ;
    if ( d.ShowModal() != wxID_OK ) return ;
    wxString filename = d.GetPath() ;

    wxBitmap *bmp = getSequenceBitmap () ;
    bmp->SaveFile ( filename , wxBITMAP_TYPE_BMP ) ;
    }    
    
void SequenceCanvas::OnCopyImage ( wxCommandEvent &ev )
    {
    int w , h ;
    MyGetClientSize ( &w , &h ) ;
    h = lowy ;
    
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
    wxBitmap *bmp = new wxBitmap ( w , h , -1 ) ;
    wxMemoryDC pdc ;
    pdc.SelectObject ( *bmp ) ;
    pdc.Clear() ;
    arrange () ;
    drawall = true ;
    safeShow ( pdc ) ;
    drawall = false ;
    return bmp ;
    }

int SequenceCanvas::getBatchMark ()
    {
    int r = vpy ;
    r -= vpy % seq.size() ;
    return r ;
    }
        
void SequenceCanvas::OnCopy ( wxCommandEvent &ev )
    {
    if ( p ) { p->OnCopy ( ev ) ; return ; }
    string s = getSelection () ;
    if ( s == "" ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }
    }
    
string SequenceCanvas::getSelection ()
    {
    if ( p ) return p->vec->getSubstring ( _from , _to ) ;
    if ( aa && _from != -1 ) return aa->vec->getSubstring ( _from , _to ) ;
    if ( pd && _from != -1 && lastmarked != -1 )
        return seq[lastmarked]->s.substr ( _from-1 , _to-_from+1 ) ;
    if ( child && child->def == "ABIviewer" )
        return seq[lastmarked]->s.substr ( _from-1 , _to-_from+1 ) ;
    return "" ;
    }
    
void SequenceCanvas::ensureVisible ( int pos )
    {
    int vx , vy , wx , wy ;
    MyGetViewStart ( &vx , &vy ) ;
    MyGetClientSize ( &wx , &wy ) ;
    wy /= charheight ;
    wy -= wy % ( seq.size() + 1 ) ;

    int ny = pos ;
    while ( ny > 0 && ny > vpy ) ny-- ;
    while ( ny + wy <= vpy ) ny++ ;
    Scroll ( -1 , ny ) ;
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

void SequenceCanvas::mark ( string id , int from , int to , int value )
    {
    if ( seq.size() == 0 ) return ;
    int a , b = -1 , cnt = 0 ;
    vpy = -1 ; 
    for ( a = 0 ; a < seq.size() && b == -1 ; a++ )
        if ( seq[a]->whatsthis() == id )
           b = a ;
    lastmarked = b ;
    if ( b == -1 ) // Illegal ID
       {
       _from = _to = -1 ;
       return ; 
       }

    int seqlen ;
    if ( p && p->vec ) seqlen = p->vec->sequence.length() ;
    else seqlen = seq[b]->s.length() ;

    int l = seqlen ;
    for ( a = 0 ; a < seq[b]->pos.m.size() ; a++ )
        {
        if ( inMarkRange ( seq[b]->pos.p[a] , from , to , l ) ) 
           {
           seq[b]->pos.m[a] = value ;
           cnt += value ;
           if ( vpy == -1 )
              vpy = seq[b]->pos.r[a].GetTop() / charheight ;
           }
        else 
           seq[b]->pos.m[a] = 0 ;
        }
        
    for ( int other = 0 ; other < seq.size() ; other++ )
        {
        bool canbemarked = seq[other]->takesMouseActions ;
        if ( other == b ) canbemarked = false ;
        for ( a = 0 ; canbemarked && a < seq[other]->pos.m.size() ; a++ )
            {
            seq[other]->pos.m[a] = 0 ;
            }
        }

    if ( !printing )
        {
        // Refreshing sequence canvas
        SilentRefresh () ;
        }
    
    if ( from > to ) to += l ;
    if ( cnt == 0 ) from = -1 ;
    
    _from = from ;
    _to = to ;
    if ( p )
        {
        // Refreshing plasmid canvas
        p->cPlasmid->mark_from = from ;
        p->cPlasmid->mark_to = to ;
        if ( !editMode )
            {
            p->cPlasmid->Refresh() ;
            
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
	    MyFrame *f = myapp()->frame ;
            f->SetStatusText ( tt , 1 ) ;
            }
        }
    else if ( pd )
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
    if ( seq.size() == 0 ) return ;
    if ( charheight == 0 ) return ;
    int vx , vy ;
    int oldlowy = lowy ;
    int oldlowx = lowx ;
    lowy = 0 ;
    lowx = 0 ;
    lastmarked = -1 ;
    int a , b ;
    
    if ( isHorizontal )
        {
        blocksize = 0 ;
        for ( a = 0 ; a < seq.size() ; a++ )
           {
           if ( seq[a]->s.length() + 1 > blocksize )
              blocksize = seq[a]->s.length() + 1 ;
           }
        }
    else blocksize = 10 ;
    
    for ( a = 0 ; a < seq.size() ; a++ )
        {
        b = seq[a]->arrange ( a ) ;
        if ( b > lowy ) lowy = b ;
        }
    
    if ( printing ) return ;
    
    MyGetViewStart ( &vx , &vy ) ;
    if ( !isHorizontal && lowy != oldlowy )
       SetScrollbars ( 0 , charheight , vy , lowy/charheight , false ) ;
    if ( isHorizontal && lowx != oldlowx )
       SetScrollbars ( charwidth , 0 , lowx/charwidth , lowx/charwidth , false ) ;
    if ( p && p->cPlasmid->mark_from != -1 && !editMode )
        {
        if ( !printing ) p->cPlasmid->mark_from = -1 ;
        p->cPlasmid->Refresh () ;
        }
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
    if ( seq.size() == 0 ) return ;
    if ( doHide ) return ;
    int wx , wy ;
    dc.SetFont ( *font ) ;
    dc.GetTextExtent ( "A" , &wx , &wy ) ;
    if ( wx != charwidth || wy != charheight )
        {
        charwidth = wx ;
        charheight = wy ;
        arrange () ;
        if ( printing && print_maxx < 0 )
            {
            for ( int a = 0 ; a < seq.size() ; a++ )
               {
               for ( int b = 0 ; b < seq[a]->pos.r.size() ; b++ )
                  {
                  if ( seq[a]->pos.r[b].GetRight() > print_maxx )
                     print_maxx = seq[a]->pos.r[b].GetRight() ;
                  }
               }
            return ;
            }
        }
        
    if ( printing ) return ;
        
    int vx , vy ;
    GetViewStart ( &vx , &vy ) ;

    if ( myapp()->frame->enhancedRefresh && !printing && !isHorizontal )
        { // MemoryDC
        int w , h ;
        MyGetClientSize ( &w , &h ) ;
        int yoff = wy * vy ;
        wxBitmap bmp ( w , h , -1 ) ;
        wxMemoryDC pdc ;
        pdc.SelectObject ( bmp ) ;
        pdc.Clear() ;
        pdc.SetDeviceOrigin ( 0 , -yoff ) ;
        safeShow ( pdc ) ;
        pdc.SetDeviceOrigin ( 0 , 0 ) ;
        dc.Blit ( 0 , yoff , w , h , &pdc , 0 , 0 ) ;
        }
    else 
        { // Direct DC
        dc.Clear() ;
        safeShow ( dc ) ;
        }
}

void SequenceCanvas::safeShow ( wxDC &dc )
    {
    int a ;
    for ( a = 0 ; a < seq.size() ; a++ ) seq[a]->shown = false ;
    for ( a = 0 ; a < seq.size() ; a++ )
       {
       if ( seq[a]->whatsthis() != "RESTRICTION" &&
            seq[a]->whatsthis() != "FEATURE" )
          {
          seq[a]->show ( dc ) ;
          seq[a]->shown = true ;
          }
       }
    
    for ( a = 0 ; a < seq.size() ; a++ )
       {
       if ( !seq[a]->shown )
          seq[a]->show ( dc ) ;
       }
    }
    
void SequenceCanvas::OnEvent(wxMouseEvent& event)
{
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
        if ( isHorizontal ) Scroll ( vx-wr , -1 ) ;
        else Scroll ( -1 , vy-wr ) ;
        return ;
        }
    
    wxString newToolTip ;
    int pos ;
    SeqBasic *where ;
    where = findMouseTarget ( pt , pos ) ;
    if ( pos <= 0 ) pos = -1 ;
    else pos = where->pos.p[pos] ;
    if ( pos != -1 && where && where->takesMouseActions )
        {
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        wxLogStatus(txt("seq_loc"), pos ) ;
        newToolTip = wxString::Format(txt("seq_loc"),pos) ;
        }
    else if ( where && child && child->def == "alignment" )
        {
        SeqAlign *al = (SeqAlign*)where ;
        if ( al->myname == txt("t_identity") ) {} // Do nothing
        else if ( al->whatsthis() == "FEATURE" ) {} // Do nothing
        else if ( al->s[pos-1] == '-' ) wxLogStatus ( al->myname.c_str() ) ;
        else
           {
           int a , b ;
           for ( a = b = 0 ; a < pos ; a++ )
              if ( al->s[a] != '-' )
                b++ ;
                
           newToolTip = al->myname.c_str() ;
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
        else if ( aa ) aa->invokeVectorEditor() ;
        else if ( where && child && child->def == "alignment" )
           {
           SeqAlign *al = (SeqAlign*)where ;
           if ( al->myname == txt("t_identity") ) {} // Do nothing
           else if ( al->whatsthis() == "FEATURE" ) {} // Do nothing
           else if ( al->s[pos] == '-' ) wxLogStatus ( al->myname.c_str() ) ;
           else
              {
              int a , b ;
              for ( a = b = 0 ; a < pos ; a++ )
                 if ( al->s[a] != '-' )
                   b++ ;
              TAlignment *ali = (TAlignment*) child ;
              ali->invokeOriginal ( al->id , b ) ;
//              wxLogStatus(wxString::Format("%s (%d)",al->myname.c_str(),b)) ;
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
               for ( int a = 0 ; a < seq.size() ; a++ )
                  if ( seq[a]->takesMouseActions )
                     mark ( seq[a]->whatsthis() , 0 , 0 , 0 ) ;
               }
            }
        else if ( !editMode )
            {
            lastpos = -1 ;
            for ( int a = 0 ; a < seq.size() ; a++ )
               if ( seq[a]->takesMouseActions )
                  mark ( seq[a]->whatsthis() , 0 , 0 , 0 ) ;
            }
        }
    else if ( event.LeftUp() )
        {
        ReleaseMouse() ;
        lastpos = -1 ;
        }

    if ( event.RightDown() )
        {
        wxMenu *cm ;
        if ( p && p->cPlasmid )
           {
           cm = p->cPlasmid->invokeVectorPopup ( pt , true ) ;
           }
        else if ( aa )
           {
           cm = new wxMenu ;
           cm->Append ( AMINOACIDS_EDIT_NAME , txt("m_edit_aa") ) ;
           if ( _from != -1  )
              {
               cm->Append ( MDI_CUT , txt("m_cut") ) ;
               cm->Append ( MDI_COPY , txt("m_copy") ) ;
               cm->Append ( MDI_AS_NEW_FEATURE , txt("m_as_new_feature") ) ;
               }
           cm->Append ( AMINOACIDS_BLAST_AA , txt("m_blast_aa") ) ;
           }
        else if ( pd )
           {
           cm = new wxMenu ;
           wxMenu *crd = new wxMenu ;
           cm->Append ( PC_VECTOR_EDIT , txt ( "t_vector_editor" ) ) ;
           cm->Append ( POPUP_DUMMY , txt("m_result_dna" ) , crd ) ;
           crd->Append ( SEQ_COPY_RESLUT_DNA , txt("m_copy_result_dna") ) ;
           crd->Append ( SEQ_NEW_FROM_RESLUT_DNA , txt("m_new_from_result_dna") ) ;
           
           if ( pd->aa_state != AA_ALL && pd->aa_state != AA_NONE )
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
              }
           }
        else if ( child && child->def == "ABIviewer" )
           {
           cm = new wxMenu ;
           cm->Append ( MDI_COPY_TO_NEW , txt("m_copy_to_new") ) ;
           if ( _from != -1  )
              {
              cm->Append ( MDI_COPY , txt("m_copy") ) ;
              }
           cm->AppendSeparator () ;
           }
        else if ( child && child->def == "alignment" && ( where || ( findMouseTargetItem ( pt ) != -1 ) ) )
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
        
        wxMenu *ca = new wxMenu ;
        ca->Append ( SEQ_COPY_TEXT , txt("m_copy_as_text") ) ;
        ca->Append ( SEQ_COPY_IMAGE , txt("m_copy_as_image") ) ;
        ca->Append ( SEQ_SAVE_IMAGE , txt("m_save_as_image") ) ;
        cm->Append ( SEQ_COPY_AS , txt("m_copy_as") , ca ) ;
        cm->Append ( SEQ_PRINT , txt("m_print_sequence") ) ;
        PopupMenu ( cm , event.GetPosition() ) ;
        delete cm ;
        }

    if ( event.Dragging() || ( event.LeftDown() && where && where->takesMouseActions ) )
        {
        wxSize cs = MyGetClientSize () ;
        int qx , qy ;
        MyGetViewStart ( &qx , &qy ) ;
        int nol = 1 ;
        if ( my > cs.y ) Scroll ( 0 , qy+nol ) ;
        else if ( my < 0 ) Scroll ( 0 , qy-nol ) ;
    
        if ( pos != -1 && lastpos != -1 && lastwhere == where )
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

    xpos = pt.x;
    ypos = pt.y;
}

SeqBasic* SequenceCanvas::findMouseTarget ( wxPoint pt , int &pos )
    {
    int a , b ;
    SeqBasic *ret = NULL ;
    pos = 0 ;
    for ( a = 0 ; a < seq.size() && pos == 0 ; a++ )
        {
        b = seq[a]->pos.getLine ( pt.y ) ;
        if ( b != -1 )
           {
           ret = seq[a] ;
           pos = ret->pos.getItem ( pt , b ) ;
           if ( !pos ) ret = NULL ;
           }
        }
    return ret ;
    }

int SequenceCanvas::findMouseTargetItem ( wxPoint pt )
    {
    int a , b ;
    for ( a = 0 ; a < seq.size() ; a++ )
        {
        b = seq[a]->pos.getLine ( pt.y ) ;
        if ( b != -1 ) return a ;
        }
    return -1 ;
    }


// The following handlers are called *only* if this is a
// PrimerDesign canvas!

void SequenceCanvas::OnCopyResultDNA ( wxCommandEvent &ev )
    {
    SeqDNA *rd = (SeqDNA*) seq[seq.size()-3] ;
    string s ;
    for ( int a = 0 ; a < rd->s.length() ; a++ )
       if ( rd->s[a] != ' ' )
          s += rd->s[a] ;
    if ( s == "" ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }    
    }
    
void SequenceCanvas::OnCopyResultAA ( wxCommandEvent &ev )
    {
    SeqAA *rd = (SeqAA*) seq[seq.size()-2] ;
    string s ;
    for ( int a = 0 ; a < rd->s.length() ; a++ )
       if ( rd->s[a] != ' ' )
          {
          if ( pd->aa_state == AA_THREE_1 ||
               pd->aa_state == AA_THREE_2 ||
               pd->aa_state == AA_THREE_3 )
          s += rd->s[a] ;
          else s = rd->s[a] + s ;
          }
    if ( s == "" ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }    
    }
    
void SequenceCanvas::OnSilmut ( wxCommandEvent &ev )
    {
    pd->OnSilmut ( ev ) ;
    }
    
void SequenceCanvas::OnNewFromResultDNA ( wxCommandEvent &ev )
    {
    SeqDNA *rd = (SeqDNA*) seq[seq.size()-3] ;
    string s ;
    int a ;
    for ( a = 0 ; a < rd->s.length() ; a++ )
       if ( rd->s[a] != ' ' )
          s += rd->s[a] ;
    if ( s == "" ) return ;

    TVector *nv = new TVector ;
    *nv = *pd->w ;
    nv->removeBlanksFromVector () ;
    if ( nv->sequence.length() != pd->w->sequence.length() ) nv->setCircular ( false ) ;
//    nv->re = pd->w->re ;
//    nv->sequence = s ;
    nv->name = pd->vec->name + " (" + string ( txt ( "t_pcr_result" ) ) + ")" ;
    nv->desc = pd->vec->desc + "\n" + string ( txt ( "t_pcr_result" ) ) ;
    nv->setChanged () ;
//    nv->setCircular ( false ) ;
    nv->recalculateCuts() ;
    nv->recalcvisual = true ;
    nv->items.push_back ( TVectorItem ( nv->name.c_str() , 
                                        nv->desc.c_str() , 
                                        1 , 
                                        s.length() , 
                                        VIT_MISC ) ) ;
    if ( nv->isCircular() ) nv->items[nv->items.size()-1].setVisible ( false ) ;
    myapp()->frame->newFromVector ( nv ) ;
    }
    
void SequenceCanvas::OnNewFromResultAA ( wxCommandEvent &ev )
    {
    SeqAA *rd = (SeqAA*) seq[seq.size()-2] ;
    string s ;
    for ( int a = 0 ; a < rd->s.length() ; a++ )
       if ( rd->s[a] != ' ' )
          {
          if ( pd->aa_state == AA_THREE_1 ||
               pd->aa_state == AA_THREE_2 ||
               pd->aa_state == AA_THREE_3 )
          s += rd->s[a] ;
          else s = rd->s[a] + s ;
          }
    if ( s == "" ) return ;

    string seq = s ;
    string n = pd->vec->name + " (" ;
    n += txt ( "t_pcr_result" ) ;
    n += ")" ;
    myapp()->frame->newAminoAcids ( seq , n ) ;
    }

void SequenceCanvas::SilentRefresh ()
    {
    if ( isHorizontal ) Clear () ;
    wxClientDC dc(this);
    PrepareDC(dc);
    OnDraw ( dc ) ;
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
    TSilmutDialog sd ( p , txt("t_what_cuts") , M_WHATCUTS ) ;
    sd.initme ( p->vec , _from , _to ) ;
    if ( wxID_OK != sd.ShowModal () ) return ;    
    
    int a ;
    TRestrictionEnzyme *e = sd.getEnzyme() ;
    for ( a = 0 ; a < p->vec->re.size() && p->vec->re[a] != e ; a++ ) ;
    if ( a == p->vec->re.size() )
        {
        p->vec->re.push_back ( e ) ;
        p->vec->recalculateCuts() ;
        p->vec->recalcvisual = true ;
        p->cPlasmid->Refresh() ;
        p->updateSequenceCanvas ( true ) ;
        p->treeBox->initme() ;
        p->Refresh () ;
        }
    }
    

void SequenceCanvas::OnSeqUp ( wxCommandEvent &ev )
    {
    TAlignment *al = (TAlignment*) child ;
    al->MoveUpDown ( last_al->id , last_al->id - 1 ) ;
    }

void SequenceCanvas::OnSeqDown ( wxCommandEvent &ev )
    {
    TAlignment *al = (TAlignment*) child ;
    al->MoveUpDown ( last_al->id , last_al->id + 1 ) ;
    }

void SequenceCanvas::OnSeqTop ( wxCommandEvent &ev )
    {
    TAlignment *al = (TAlignment*) child ;
    al->MoveUpDown ( last_al->id , 0 ) ;
    }

void SequenceCanvas::OnSeqBottom ( wxCommandEvent &ev )
    {
    TAlignment *al = (TAlignment*) child ;
    al->MoveUpDown ( last_al->id , al->lines.size()-1 ) ;
    }

void SequenceCanvas::OnToggleFeat ( wxCommandEvent &ev )
    {
    TAlignment *al = (TAlignment*) child ;
    TAlignLine *all = &al->lines[last_al->id] ;
    if ( all->hasFeatures() ) all->hideFeatures() ;
    else all->showFeatures() ;
    al->redoAlignments ( false ) ;
    }

// -------------------------------------------------------- TMarkMem

TMarkMem::TMarkMem ( SequenceCanvas *_sc )
    {
    sc = _sc ;
    if ( !sc ) return ;
    f = sc->_from ;
    t = sc->_to ;
    l = sc->lastmarked ;
    }
    
void TMarkMem::remark()
    {
    if ( !sc ) return ;
    if ( l >= 0 && l < sc->seq.size() && f != -1 )
       {
       string ls = sc->seq[l]->whatsthis() ;
       int len = sc->seq[l]->s.length() ;
       if ( t > len ) t -= len ;
       sc->mark ( ls , f , t ) ;
       }
    }

void TMarkMem::unmark()
    {
    if ( !sc ) return ;
    if ( l >= 0 && l < sc->seq.size() && f != -1 )
       {
       string ls = sc->seq[l]->whatsthis() ;
       sc->mark ( ls , -1 , -1 ) ;
       }
    }
    
