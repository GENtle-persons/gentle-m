/** \file
    \brief Contains the SeqAA class members
*/
#include "SequenceCanvas.h"

SeqAA::~SeqAA ()
    {
    for ( int a = 0 ; can && a < can->seq.GetCount() ; a++ )
        {
        if ( can->seq[a]->whatsthis() == _T("FEATURE") )
           {
           SeqFeature *f = (SeqFeature*) can->seq[a] ;
           if ( f->aaa == this ) f->aaa = NULL ;
           }
        }
    CLEAR_DELETE ( pc ) ;
    }

int SeqAA::arrange ( const int n )
    {
    if ( useDirectRoutines() ) return arrange_direct ( n ) ;
    wxMessageBox ( _T("One has to wonder...1") ) ;
    assert(0);
    }

void SeqAA::show ( wxDC& dc )
    {
    if ( useDirectRoutines() ) { show_direct ( dc ) ; return ; }
    wxMessageBox ( _T("One has to wonder...2") ) ;
    }

wxPoint SeqAA::showText ( const int ystart , wxArrayString &tout ) const
    {
    wxPoint p ( -1 , -1 ) ;
    wxString t ;
    int x = 0 , y = ystart-can->seq.GetCount() , ly = -1 ;
    for ( int a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        int b = pos.p[a] ;
        if ( b > 0 ) // Character
            {
            t = s.GetChar(b-1) ;
            if ( pos.r[a].y != ly )
               {
               ly = pos.r[a].y ;
               y += can->seq.GetCount() ;
               x = 0 ;
               while ( y >= tout.GetCount() ) tout.Add ( _T("") ) ;
               }
           else x++ ;
           if ( (x) % (can->blocksize+1) == 0 ) x++ ;
           if ( !t.IsEmpty() )
               {
               if ( b >= can->markedFrom() && p.x == -1 )
                   {
                   p.x = y ;
                   }
               if ( b <= can->markedTo() )
                   {
                   p.y = y ;
                   }
               while ( tout[y].length() < x ) tout[y] += _T(" ") ;
               tout[y].SetChar ( x-1 , t.GetChar(0) ) ;
               }
           }
        }
    if ( can->getLastWhere() != this || can->markedFrom() == -1 || can->markedTo() < can->markedFrom() )
        {
        p = wxPoint ( -1 , -1 ) ;
        }
    return p ;
    }

void SeqAA::initFromString ( const wxString& s )
    {
    vec = NULL ;
    showNumbers = true ;
    offsets.Clear() ;
//  offsets.Alloc ( s.length() ) ;
//  while ( offsets.GetCount() < s.length() ) offsets.Add ( -1 ) ;

    // Proteases
    updateProteases () ;
    pa_w = _T("") ;
    pa_wa.Clear() ;
    pa_wa.Alloc ( s.length() ) ;
    while ( pa_w != s )
        {
        pa_w += s.GetChar(pa_w.length()) ;
        pa_wa.Add ( pa_w.length() ) ;
        analyzeProteases () ;
        }
    }

void SeqAA::updateProteases () /* not const */
    {
    proteases.Clear () ; // not const
    CLEAR_DELETE ( pc ) ;
    if ( !can ) return ;

    wxArrayString vs ;
    if ( can->child ) vs = can->child->vec->proteases ;
    else if ( can->p ) vs = can->p->vec->proteases ;
    else if ( can->getAA() ) vs = can->getAA()->vec->proteases ;

    proteases.Alloc ( vs.GetCount() ) ;
    for ( int a = 0 ; a < vs.GetCount() ; a++ )
        {
        TProtease *pro = myapp()->frame->LS->getProtease ( vs[a] ) ;
        if ( pro ) proteases.Add ( pro ) ;
        }
    }

void SeqAA::analyzeProteases () /* not const */
    {
    if ( !can ) return ;
    int compensator = primaryMode ? 0 : 1 ;
    for ( int q = 0 ; q < proteases.GetCount() ; q++ )
        {
        TProtease *pr = proteases[q] ;
        if ( pr->len() <= pa_w.length() )
            {
            wxString w2 = pa_w.substr ( pa_w.length() - pr->len() , pr->len() ) ;
            if ( pr->does_match ( w2 ) )
                {
                TProteaseCut *cut = new TProteaseCut ;
                cut->protease = pr ;
                cut->cut = pa_wa[pa_w.length()-pr->len()+pr->cut+1] + compensator ;
                cut->left = true ;
                pc.Add ( cut ) ; // not const
                }
            }
        }
    }

// Seems to be obsolete
void SeqAA::fixOffsets ( TVector * const v )
    {
    offset_items.Clear() ;
    for ( int a = 0 ; a < v->items.size() ; a++ )
        {
        const int off = v->items[a].getOffset () ;
        int c = 0 ;
        for ( int b = v->items[a].from ; off != -1 && b < v->items[a].to ; b++ )
           {
//         int c = b - v->items[a].from ;
           char x =  v->getSequenceChar ( b-1 ) ;
           if ( ( b - 1 ) % 10 == 0 && c > 0 && x != '-' )
              {
              offsets.Alloc ( b ) ;
              offset_items.Alloc ( b ) ;
              while ( offsets.GetCount() < b ) offsets.Add ( -1 ) ;
              while ( offset_items.GetCount() < b ) offset_items.Add ( NULL ) ;
              offsets[b-1] = c + off ;
              offset_items[b-1] = &v->items[a] ;
              }
           if ( x != '-' ) c++ ;
           }
        }
    }



void SeqAA::initFromTVector ( const TVector * const v )
    {
    //wxPrintf( "D: SeqAA::initFromTVector - start\n" ) ;

    if ( ! v )
        {
        wxPrintf ( "E: SeqAA::initFromTVector ( NULL ) - abort\n" ) ;
        abort() ;
        }

    //wxPrintf( "D: SeqAA::initFromTVector( %s ) - start\n" , v->getName() ) ;

    showNumbers = true ;
    if ( ! vec )
        {
        //wxPrintf ( "D: SeqAA::initFromTVector: allocating new vector.\n" ) ;
        vec = new TVector ( ) ;
        }
    else
        {
        //wxPrintf ( "D: SeqAA::initFromTVector: reusing existing vector - why?\n" ) ;
        }
    //wxPrintf ( "D: SeqAA::initFromTVector: pre vec->copy()\n" ) ;
    vec->copy ( *v ) ;
    //wxPrintf ( "D: SeqAA::initFromTVector: post vec->copy()\n" ) ;

    bool truncateEditSequence = false ;
    if ( can && can->getEditMode() && vec->getSequenceLength() && vec->getSequenceChar(vec->getSequenceLength()-1) == ' ' )
        {
        //wxPrintf ( "D: SeqAA::initFromTVector: can && can->getEditMode() && vec->getSequenceLength() && vec->getSequenceChar(vec->getSequenceLength()-1) == ' '\n" ) ;
        vec->eraseSequence ( vec->getSequenceLength()-1 , 1 ) ;
        truncateEditSequence = true ;
        }

    wxString t = vec->getSequence() ;
    if (0 >= t.length())
        {
        //wxPrintf("D: SeqAA::initFromTVector: Assigned the empty string with t=%ld.\n" , t.length() ) ;
        s="";
        }
    else
        {
        //wxPrintf("D: SeqAA::initFromTVector: Assigning blanks of length %ld\n", t.length() ) ;
        wxString empty(' ', t.length() ) ;
        s = empty ; 
        }

    //wxPrintf("D: String s is regular: '%s'\n", s ) ;
//  offsets.Clear() ;
    offset_items.Clear() ;
//  offsets.Alloc ( s.length() ) ;
//  while ( offsets.GetCount() < s.length() ) offsets.Add ( -1 ) ;
    updateProteases () ;

    if ( vec->isCircular() )
        {
        t += t.substr ( 0 , 2 ) ;
        }
    else
        {
        t += _T("  ") ;
        }

    const int sl = s.length() ;
    s += _T("  ") ;

    if ( mode == AA_ALL )
        {
        for ( int a = 0 ; a < sl ; a++ )
            {
            s.SetChar(a,vec->dna2aa ( t.substr(a,3) ).GetChar(0)) ;
            }
        }
    else if ( mode == AA_KNOWN )
        {
        for ( int a = 0 ; a < vec->items.size() ; a++ )
            {
//          vec->items[a].translate ( vec , this ) ; // TESTING!!!
            wxPrintf( "D: SeqAA::initFromTVector invoking getArrangedAA @  %d of %ld with disp=%d\n", a, vec->items.size(), disp ) ;
//          wxPrintf( "D: SeqAA::initFromTVector invoking getArrangedAA @  s=%s\n", s ) ;
            vec->items[a].getArrangedAA ( vec , s , disp , this ) ;
            }
        }
    else
        {
        pa_w = _T("") ;
        pa_wa.Clear() ;
        int mymode = mode ;
        bool invert = false ;
        if ( mode == AA_THREE_M1 ) { mymode = AA_THREE_1 ; invert = true ; }
        if ( mode == AA_THREE_M2 ) { mymode = AA_THREE_2 ; invert = true ; }
        if ( mode == AA_THREE_M3 ) { mymode = AA_THREE_3 ; invert = true ; }
        pa_wa.Alloc ( sl ) ;
        for ( int a = 0 ; a < sl ; a++ )
            {
            wxString u = t.substr(a,3) ;
            if ( invert )
                {
                u.SetChar( 0 , vec->getComplement ( t.GetChar(a+2) ) ) ;
                u.SetChar( 1 , vec->getComplement ( t.GetChar(a+1) ) ) ;
                u.SetChar( 2 , vec->getComplement ( t.GetChar(a+0) ) ) ;
                }
            char c = vec->dna2aa(u).GetChar(0) ;
            wxString three = vec->one2three((int)c) ;

            // Protease analysis
            pa_w += c ;
            pa_wa.Add ( a ) ;
            analyzeProteases () ;

            if ( ( mymode == AA_THREE_1 && (a+0) % 3 == 0 ) ||
                 ( mymode == AA_THREE_2 && (a-1) % 3 == 0 ) ||
                 ( mymode == AA_THREE_3 && (a-2) % 3 == 0 ) )
                {
                if ( disp == AA_ONE ) s.SetChar(a,c) ;
                else
                    {
                    s.SetChar(a+0, three.GetChar(0) ) ;
                    s.SetChar(a+1, three.GetChar(1) ) ;
                    s.SetChar(a+2, three.GetChar(2) ) ;
                    }
                }
            }
        }

    s = s.substr ( 0 , sl ) ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        if ( s.GetChar(a) == '?' )
            {
            s.SetChar(a, unknownAA) ;
            }
        }
    if ( truncateEditSequence )
       {
       //wxPrintf ( "D; SeqAA::initFromTVector: truncateEditSequence\n" ) ;
       vec->addToSequence ( _T(" ") ) ;
       s += _T(" ") ;
       }

    int mem = 0 ;
//  mem += pa_w.length() ;
    mem += offsets.GetCount() * sizeof ( int ) ;
//  mem += pa_wa.GetCount() * sizeof ( int ) ;
//  mem += sizeof ( wxArrayTProteaseCut ) + sizeof ( wxArrayTProtease ) + sizeof ( wxArrayTVectorItem ) ;
    mylog ( "AA_mem" , wxString::Format ( "%d" , mem ) ) ;

    //wxPrintf( "D: SeqAA::initFromTVector - end\n" ) ;
    }

// direct


bool SeqAA::useDirectRoutines () const
    {
    return true ;
    }

int SeqAA::arrange_direct ( const int n ) /* not const */
    {
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth ; //, wy = can->charheight ;
    int bo = can->border ;
    int ox = bo + wx + wx * endnumberlength ;//, oy = n*wy+bo ;
    int w , h ;
    can->MyGetClientSize ( &w , &h ) ;
    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx - 1 ) ;
    if ( itemsperline == 0 ) itemsperline = 1 ;
    itemsperline *= can->blocksize ;
    pos.cleanup() ; // not const
//  pos.m.Alloc ( s.length() ) ;
    if ( can->isHorizontal() ) can->setLowX ( ox + ( s.length() ) * can->charwidth ) ;
    int ret = ( s.length() + itemsperline - 1 ) / itemsperline ;
    ret *= can->NumberOfLines() * can->charheight + 1 ;
    return ret ;
    }

void SeqAA::show_direct ( wxDC& dc )
    {
    //wxPrintf("D: SeqAA::show_direct - start\n" ) ;
    myass ( itemsperline , "AA:show_direct_ipl" ) ;
    if ( !itemsperline ) return ;
    mylog ( "SeqAA::show_direct" , "0" ) ;
    can->SetFont(*can->font);
    dc.SetFont(*can->font);
    int b , w , h , n , bo = can->border ;
    int csgc = can->NumberOfLines() , cbs = can->blocksize ;
    int cih = can->isHorizontal() ;
    int xa , xb , ya , yb ;
    for ( n = 0 ; n < csgc && can->seq[n] != this ; n++ ) ;
    if ( n == csgc ) return ;
    mylog ( "SeqAA::show_direct" , "1" ) ;

    // Setting basic values
    int cw = can->charwidth , ch = can->charheight ;
    int ox = bo + cw + cw * endnumberlength ;
    int oy = n*ch+bo ;

    can->MyGetClientSize ( &w , &h ) ;
    xb = w ;
    yb = h ;

    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    wxColour tf ;
    if ( primaryMode ) tf = *wxBLACK ;
    else tf = myapp()->frame->aa_color ;; // wxColour ( 130 , 130 , 130 ) ;//*wxLIGHT_GREY ;
    dc.SetTextForeground ( tf ) ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;

    dc.GetDeviceOrigin ( &xa , &ya ) ;
    xa = -xa ;
    xb += xa ;
    ya = -ya ;
    yb += ya ;

    mylog ( "SeqAA::show_direct" , "2" ) ;
    b = ( ya - ch - oy ) / ( ch * csgc ) * itemsperline ;
    mylog ( "SeqAA::show_direct" , "3" ) ;

    int a ;

    for ( a = 0 ; a < b && a < s.length() ; a += itemsperline ) ;
    for ( ; a < s.length() ; a++ )
        {
        int px = a % itemsperline , py = a / itemsperline ;

        bool showNumber = ( px == 0 ) ;

        px = px * cw + ( px / cbs ) * ( cw - 1 ) + ox ;
        py = py * ch * csgc + oy ;

        if ( !can->getDrawAll() )
            {
            if ( py + ch < ya ) continue ;
            if ( py > yb ) break ;
            if ( cih )
                {
                if ( px + cw < xa ) continue ;
                if ( px > xb ) continue ;
                }
            }

       int pm = getMark ( a ) ;
       if ( pm == 1 ) // Marked (light gray background)
           {
           dc.SetBackgroundMode ( wxSOLID ) ;
           dc.SetTextBackground ( *wxLIGHT_GREY ) ;
           dc.SetTextForeground ( getHighlightColor ( a , tf ) ) ;
           }
       else if ( pm == 2 && can->doOverwrite() ) // Overwrite cursor
           {
           dc.SetBackgroundMode ( wxSOLID ) ;
           dc.SetTextBackground ( *wxBLACK ) ;
           }
       if ( pm == 2 && can->doOverwrite() ) dc.SetTextForeground ( *wxWHITE ) ;
       else dc.SetTextForeground ( getHighlightColor ( a , tf ) ) ;
       if ( can->isPrinting() && pm == 1 )
           {
           dc.SetBrush ( *MYBRUSH ( wxColour ( 230 , 230 , 230 ) ) ) ;
          dc.SetPen(*wxTRANSPARENT_PEN);
           dc.DrawRectangle ( px , py , cw , ch ) ;
           }
       if ( can->isPrinting() && !can->getPrintToColor() )
           {
           dc.SetBackgroundMode ( wxTRANSPARENT ) ;
           dc.SetTextForeground ( *wxBLACK ) ;
           }

       // Show the char
        wxChar ch2 = s.GetChar(a) ;
        if ( ch2 == '|' ) ch2 = myapp()->frame->stopcodon ;
        dc.DrawText ( wxString ( ch2 ) , px , py ) ;

        int pz = py + ch ;

        if ( show_diff_to && ch2 != ' ' ) // Mark changed amino acids in PCR
            {
            if ( show_diff_to->s.GetChar(a) != ch2 )
                {
                dc.SetPen(*wxRED_PEN);
                dc.DrawLine ( px-1 , pz , px-1+cw , pz ) ;
                }
            }

       if ( pm == 2 && !can->doOverwrite() ) // Insert cursor
           {
                dc.SetPen(*wxBLACK_PEN);
                dc.DrawLine ( px-1 , py , px-1 , pz ) ;
                dc.DrawLine ( px-3 , py , px+2 , py ) ;
                dc.DrawLine ( px-3 , pz , px+2 , pz ) ;
           }
       if ( pm > 0 ) // Reverting cursor settings
           {
           dc.SetBackgroundMode ( wxTRANSPARENT ) ;
           dc.SetTextForeground ( getHighlightColor ( a , tf ) ) ;
           }

        // Protease cuts
        for ( int q = 0 ; q < pc.GetCount() ; q++ )
            {
            if ( a == pc[q]->cut - pc[q]->left )
                {
                int qx = px ;
                int qy = py ;
                if ( !pc[q]->left ) qx += cw + 4 ;
                dc.SetTextForeground ( *wxBLACK ) ;
                dc.SetPen(*wxGREY_PEN);
                dc.DrawLine ( qx   , qy + 1 , qx   , qy + can->charheight - 2 ) ;
                dc.SetPen(*wxBLACK_PEN);
                dc.DrawLine ( qx+1 , qy + 1 , qx+1 , qy + can->charheight - 2 ) ;

                wxString pn = pc[q]->protease->name ;
                for ( int w = 0 ; w+1 < pn.length() ; w++ )
                    {
                    if ( pn.GetChar(w) == ' ' && pn.GetChar(w+1) == '(' )
                        {
                        pn = pn.substr ( 0 , w ) ;
                        }
                    }
                dc.SetFont(*can->smallFont);
                int u1 , u2 ;
                dc.GetTextExtent ( pn , &u1 , &u2 ) ;
                dc.DrawText ( pn , qx - u1/2 , qy - u2/2 ) ;
                dc.SetFont(*can->font);

                if ( primaryMode ) dc.SetTextForeground ( getHighlightColor ( a , *wxBLACK ) ) ;
                else dc.SetTextForeground ( myapp()->frame->aa_color /* *wxLIGHT_GREY */ ) ;
                }
            }

        if ( showNumber && primaryMode )
            {
            wxString t = wxString::Format ( _T("%d") , a + 1 ) ;
            while ( endnumberlength > t.length() ) t = _T("0") + t ;
//          t.Pad ( endnumberlength - t.length() , '0' , false ) ;
            dc.DrawText ( t , bo , py ) ;
            }
        }

    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    //wxPrintf("D: SeqAA::show_direct - end\n" ) ;
    }

int SeqAA::getLine ( const int _y ) const
    {
    if ( !useDirectRoutines() ) return SeqBasic::getLine ( _y ) ;

    int y (_y) ;
    if ( can->charheight == 0 ) return -1 ;
    int n ;
    for ( n = 0 ; can->seq[n] != this ; n++ ) ;
    y -= can->border ;
    y /= can->charheight ;
    if ( y % can->NumberOfLines() == n )
        {
        int ret = y / can->NumberOfLines() ;
        if ( ret < 0 ) return -1 ;
        return ret ;
        }
    else return -1 ;
    }

int SeqAA::getItem ( const wxPoint& pt , const int line ) const
    {
    if ( !useDirectRoutines() ) return SeqBasic::getItem ( pt , line ) ;
    if ( line < 0 ) return 0 ;
    int n , x = pt.x , y = pt.y ;
    for ( n = 0 ; can->seq[n] != this ; n++ ) ;
    int cw = can->charwidth , ch = can->charheight ;
    int ox = can->border + cw + cw * endnumberlength ;
    if ( ch == 0 ) return 0 ;
    y -= can->border ;
    y /= ch ;
    y /= can->NumberOfLines() ;

    x -= ox ;
    if ( x < 0 ) return -1 ;
    int j = 0 ;
    while ( x >= cw * can->blocksize + cw - 1 )
        {
        j += can->blocksize ;
        x -= cw * can->blocksize + cw - 1 ;
        }
    while ( x >= 0 && j < itemsperline )
        {
        if ( x >= 0 && x <= cw )
           {
           int ret = line * itemsperline + j + 1 ;
           if ( ret > s.length() ) return -1 ;
           return ret ;
           }
        j++ ;
        x -= cw ;
        if ( j % can->blocksize == 0 ) x -= cw - 1 ;
        }

    return 0 ;
    }

int SeqAA::getMarkSize () const
    {
    if ( !useDirectRoutines() ) return SeqBasic::getMarkSize() ;
    return s.length()+1 ;
    }

int SeqAA::getRectSize () const
    {
    if ( !useDirectRoutines() ) return SeqBasic::getRectSize() ;
    return 0 ;
    }

wxRect SeqAA::getRect ( const int i ) const
    {
    if ( !useDirectRoutines() ) return pos.r[i] ;

    wxRect ret ;
    ret.x = 0 ;
    ret.y = ( (i-1) / itemsperline ) * can->charheight * can->NumberOfLines() ;

    return ret ;
    }

int SeqAA::getMark ( const int i ) const
    {
    if ( !useDirectRoutines() ) return SeqBasic::getMark ( i ) ;
    if ( i >= s.length() ) return 0 ;
    return pos.getmark ( i ) ;
    }

void SeqAA::setMark ( const int i , const int v )
    {
    if ( !useDirectRoutines() ) { SeqBasic::setMark ( i , v ) ; return ; }
    if ( i <= 0 ) return ;
    pos.mark ( i-1 , v ) ;
    }

int SeqAA::getPos ( const int i ) const
    {
    if ( !useDirectRoutines() ) return pos.p[i] ;
    return i ;
    }

void SeqAA::setPos ( const int i , const int v ) /* not const */
    {
    if ( !useDirectRoutines() ) pos.p[i] = v ;
    }

void SeqAA::logsize () const
    {
    SeqBasic::logsize() ;
    mylog ( whatsthis() , wxString::Format ( "pa_w %d" , pa_w.length() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "!proteases %d" , proteases.GetCount() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "offsets %d" , sizeof(int)*offsets.GetCount() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "pa_wa %d" , sizeof(int)*pa_wa.GetCount() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "!offset_items %d" , offset_items.GetCount() ) ) ;
    }

