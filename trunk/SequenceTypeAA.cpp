#include "SequenceCanvas.h"

//************************************************ SeqAA

SeqAA::~SeqAA ()
    {
    int a ;
    for ( a = 0 ; can && a < can->seq.GetCount() ; a++ )
        {
        if ( can->seq[a]->whatsthis() == "FEATURE" )
           {
           SeqFeature *f = (SeqFeature*) can->seq[a] ;
           if ( f->aaa == this ) f->aaa = NULL ;
           }
        }
    CLEAR_DELETE ( pc ) ;
    }

int SeqAA::arrange ( int n )
    {
    if ( useDirectRoutines() ) return arrange_direct ( n ) ;
    wxMessageBox ( "One has to wonder...1" ) ;
    int a , b , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    int ox = bo + wx + wx * endnumberlength , oy = n*wy+bo ;

    can->MyGetClientSize ( &w , &h ) ;

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx - 1 ) * can->blocksize ;
    
    pos.cleanup() ;
    if ( primaryMode ) pos.reserve ( s.length() * 11 / 10 , s.length() / itemsperline ) ;
    else pos.reserve ( s.length() * 11 / 10 , 0 ) ;
    x = ox ;
    y = oy ;
    if ( showNumbers )
       pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    int icnt = 0 ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        icnt++ ;
        pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        can->setLowX ( x + wx*2 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % can->blocksize == 0 )
           {
           x += wx-1 ;
           if ( icnt == itemsperline )
              {
              icnt = 0 ;
              if ( primaryMode ) pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
              lasta = pos.p.GetCount()+1 ;
              x = ox ;
              y += wy * ( can->seq.GetCount() + can->blankline ) ;
              if ( showNumbers && a+1 < s.length() )
                 pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
              }
           }
        }
    if ( primaryMode && showNumbers && lasta != pos.p.GetCount()+1 ) 
        pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
        
    // Marking features
    wxString t = s ;
    for ( a = 0 ; a < t.length() ; a++ ) t[(uint)a] = ' ' ;
    
    x = 2 ;
    if ( mode != AA_ALL ) x = 0 ;
    
    for ( a = 0 ; vec && a < vec->items.size() ; a++ )
        {
        y = vec->items[a].to ;
        if ( y < vec->items[a].from ) y += s.length() ;
        for ( b = vec->items[a].from-1 ; b+x < y ; b++ )
           {
           t[(uint)(b+1)%s.length()] = 'X' ;
           }
        }
    
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        b = pos.p[a] ;
        if ( b > 0 ) // Char
           {
           if ( t.GetChar(b) == 'X' ) setMark ( a , 1 ) ;
           }
        }
        
    return lowy + bo*2 ;
    }
    
void SeqAA::show ( wxDC& dc )
    {
    if ( useDirectRoutines() ) { show_direct ( dc ) ; return ; }
    wxMessageBox ( "One has to wonder...2" ) ;
    int cw2 , ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( "A" , &cw2 , &ch2 ) ;
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    wxString t ;
    char u[100] ;
    wxColour bbg ( 150 , 150 , 255 ) ;
    dc.SetTextBackground ( *wxWHITE ) ;
    if ( primaryMode ) dc.SetTextForeground ( *wxBLACK ) ;
    else dc.SetTextForeground ( *wxLIGHT_GREY ) ;
//    dc.SetBackgroundMode ( wxSOLID ) ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;

    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        if ( can->hardstop > -1 && a > can->hardstop ) break ;
        b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tz = ty + can->charheight ;
        bool insight = true ;
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( can->getDrawAll() ) insight = true ;
        if ( !insight && ty > yb ) a = pos.p.GetCount() ;
        if ( b > 0 && !insight ) cnt++ ;
        if ( b > 0 && insight ) // Character
           {
           if ( getMark ( a ) == 1 )
              {
              if ( primaryMode )
                 {
                 dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxSOLID ) ;
                 dc.SetTextBackground ( *wxLIGHT_GREY ) ;
                 }
              else
                 {
                 dc.SetTextForeground ( bbg ) ;
                 dc.SetTextBackground ( *wxWHITE ) ;
                 }
              }
           else if ( getMark ( a ) == 2 && can->doOverwrite() )
              {
              dc.SetTextForeground ( *wxWHITE ) ;
              dc.SetTextBackground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxSOLID ) ;
              }
           t = s.GetChar(b-1) ;
           if ( can->isPrinting() && !can->getPrintToColor() )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           dc.DrawText ( t, pos.r[a].x, pos.r[a].y ) ;

           if ( getMark ( a ) == 2 && !can->doOverwrite() )
              {
                 int tx = pos.r[a].x , ty = pos.r[a].y ;
                 int tz = ty + can->charheight ;
                 dc.SetPen(*wxBLACK_PEN);
                 dc.DrawLine ( tx-1 , ty , tx-1 , tz ) ;
                 dc.DrawLine ( tx-3 , ty , tx+2 , ty ) ;
                 dc.DrawLine ( tx-3 , tz , tx+2 , tz ) ;
              }
           if ( getMark ( a ) > 0 )
              {
              dc.SetTextBackground ( *wxWHITE ) ;
              if ( primaryMode ) dc.SetTextForeground ( *wxBLACK ) ;
              else dc.SetTextForeground ( *wxLIGHT_GREY ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           // Protease cuts
           for ( int q = 0 ; q < pc.GetCount() ; q++ )
              {
              if ( b == pc[q]->cut )
                 {
                 int qx = pos.r[a].x - 2 ;
                 int qy = pos.r[a].y ;
                 if ( !pc[q]->left ) qx += can->charwidth + 4 ;
                 dc.SetTextForeground ( *wxBLACK ) ;
                 dc.SetPen(*wxGREY_PEN);
                 dc.DrawLine ( qx   , qy + 1 , qx   , qy + can->charheight - 2 ) ;
                 dc.SetPen(*wxBLACK_PEN);
                 dc.DrawLine ( qx+1 , qy + 1 , qx+1 , qy + can->charheight - 2 ) ;

                 wxString pn = pc[q]->protease->name ;
                 for ( int w = 0 ; w+1 < pn.length() ; w++ )
                    if ( pn.GetChar(w) == ' ' && pn.GetChar(w+1) == '(' )
                       pn = pn.substr ( 0 , w ) ;
                 dc.SetFont(*can->smallFont);
                 int u1 , u2 ;
                 dc.GetTextExtent ( pn , &u1 , &u2 ) ;
                 dc.DrawText ( pn , qx - u1/2 , qy - u2/2 ) ;
                 dc.SetFont(*can->font);
                 
                 if ( primaryMode ) dc.SetTextForeground ( *wxBLACK ) ;
                 else dc.SetTextForeground ( *wxLIGHT_GREY ) ;
                 }
              }
              
           cnt++ ;
           }
        else if ( insight ) // Front number
           {
           if ( primaryMode ) sprintf ( u , "%d" , cnt ) ;
           else sprintf ( u , "%d" , cnt/3 ) ;
           t = u ;
           while ( t.length() < endnumberlength ) t = "0" + t ;
           dc.DrawText ( t , pos.r[a].x, pos.r[a].y ) ;
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

wxPoint SeqAA::showText ( int ystart , wxArrayString &tout )
    {
    wxPoint p ( -1 , -1 ) ;
    int a , b , c ;
    wxString t ;
    int x = 0 , y = ystart-can->seq.GetCount() , ly = -1 ;
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        b = pos.p[a] ;
        if ( b > 0 ) // Character
           {
           t = s.GetChar(b-1) ;
           if ( pos.r[a].y != ly ) 
              {
              ly = pos.r[a].y ;
              y += can->seq.GetCount() ;
              x = 0 ;
              while ( y >= tout.GetCount() ) tout.Add ( "" ) ;
              }
           else x++ ;
           if ( (x) % (can->blocksize+1) == 0 ) x++ ;
           if ( !t.IsEmpty() )
              {
              if ( b >= can->markedFrom() && p.x == -1 ) p.x = y ;
              if ( b <= can->markedTo() ) p.y = y ;
              while ( tout[y].length() < x ) tout[y] += " " ;
              tout[y].SetChar ( x-1 , t.GetChar(0) ) ;
              }
           }
        }
    if ( can->getLastWhere() != this || can->markedFrom() == -1 || can->markedTo() < can->markedFrom() )
       p = wxPoint ( -1 , -1 ) ;
    return p ;
    }
    
void SeqAA::initFromString ( wxString t )
    {
    s = t ;
    vec = NULL ;
    showNumbers = true ;
    offsets.Clear() ;
//    offsets.Alloc ( s.length() ) ;
//    while ( offsets.GetCount() < s.length() ) offsets.Add ( -1 ) ;
    
    // Proteases
    updateProteases () ;
    pa_w = "" ;
    pa_wa.Clear() ;
    pa_wa.Alloc ( s.length() ) ;
    while ( pa_w != s )
       {
       pa_w += s.GetChar(pa_w.length()) ;
       pa_wa.Add ( pa_w.length() ) ;
       analyzeProteases () ;
       }
    }

void SeqAA::updateProteases ()
    {
    proteases.Clear () ;
    CLEAR_DELETE ( pc ) ;
    if ( !can ) return ;

    wxArrayString vs ;
    if ( can->child ) vs = can->child->vec->proteases ;
    else if ( can->p ) vs = can->p->vec->proteases ;
    else if ( can->getAA() ) vs = can->getAA()->vec->proteases ;

    int a ;
    proteases.Alloc ( vs.GetCount() ) ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        TProtease *pro = myapp()->frame->LS->getProtease ( vs[a] ) ;
        if ( pro ) proteases.Add ( pro ) ;
        }
    }
    
void SeqAA::analyzeProteases ()
    {
    if ( !can ) return ; 
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
             cut->cut = pa_wa[pa_w.length()-pr->len()+pr->cut+1] ;
             cut->left = true ;
             pc.Add ( cut ) ;
             }
          }
       }
    }

// Seems to be obsolete
void SeqAA::fixOffsets ( TVector *v )
    {
    offset_items.Clear() ;
    int a , b ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        int off = v->items[a].getOffset () ;
        int c = 0 ;
        for ( b = v->items[a].from ; off != -1 && b < v->items[a].to ; b++ )
           {
//           int c = b - v->items[a].from ;
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
    

        
void SeqAA::initFromTVector ( TVector *v )
    {
    showNumbers = true ;
    vec = v ;
    bool truncateEditSequence = false ;
    if ( can && can->getEditMode() && v->getSequenceLength() &&
            v->getSequenceChar(v->getSequenceLength()-1) == ' ' )
       {
       v->eraseSequence ( v->getSequenceLength()-1 , 1 ) ;
       truncateEditSequence = true ;
       }
    wxString t = vec->getSequence() ;
    s.Alloc ( t.length() ) ;
    FILLSTRING ( s , ' ' , t.length() ) ;
//    offsets.Clear() ;
    offset_items.Clear() ;
//    offsets.Alloc ( s.length() ) ;
//    while ( offsets.GetCount() < s.length() ) offsets.Add ( -1 ) ;
    updateProteases () ;
    if ( v->isCircular() ) t += t.substr ( 0 , 2 ) ;
    else t += "  " ;
    int a , b , sl = s.length() ;
    s += "  " ;

    if ( mode == AA_ALL )
        {
        for ( a = 0 ; a < sl ; a++ )
            s.SetChar(a,v->dna2aa ( t.substr(a,3) ).GetChar(0)) ;
        }
    else if ( mode == AA_KNOWN )
        {
        for ( a = 0 ; a < v->items.size() ; a++ )
           {
//           v->items[a].translate ( v , this ) ; // TESTING!!!
           v->items[a].getArrangedAA ( v , s , disp , this ) ;
           }
        }
    else
        {
        pa_w = "" ;
        pa_wa.Clear() ;
        int mymode = mode ;
        bool invert = false ;
        if ( mode == AA_THREE_M1 ) { mymode = AA_THREE_1 ; invert = true ; }
        if ( mode == AA_THREE_M2 ) { mymode = AA_THREE_2 ; invert = true ; }
        if ( mode == AA_THREE_M3 ) { mymode = AA_THREE_3 ; invert = true ; }
        pa_wa.Alloc ( sl ) ;
        for ( a = 0 ; a < sl ; a++ )
            {
            wxString u = t.substr(a,3) ;
            if ( invert )
               {
               u.SetChar( 0 , v->getComplement ( t.GetChar(a+2) ) ) ;
               u.SetChar( 1 , v->getComplement ( t.GetChar(a+1) ) ) ;
               u.SetChar( 2 , v->getComplement ( t.GetChar(a+0) ) ) ;
               }
            char c = v->dna2aa(u).GetChar(0) ;
            wxString three = v->one2three((int)c) ;

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
    for ( a = 0 ; a < s.length() ; a++ )
       if ( s.GetChar(a) == '?' ) s.SetChar(a, unknownAA) ;
    if ( truncateEditSequence )
       {
       v->addToSequence ( " " ) ;
       s += " " ;
       }
       
    int mem = 0 ;
//    mem += pa_w.length() ;
    mem += offsets.GetCount() * sizeof ( int ) ;
//    mem += pa_wa.GetCount() * sizeof ( int ) ;
//    mem += sizeof ( wxArrayTProteaseCut ) + sizeof ( wxArrayTProtease ) + sizeof ( wxArrayTVectorItem ) ;
    mylog ( "AA_mem" , wxString::Format ( "%d" , mem ) ) ;
    }
    
// direct


bool SeqAA::useDirectRoutines ()
    {
    return true ;
    }

int SeqAA::arrange_direct ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    int ox = bo + wx + wx * endnumberlength , oy = n*wy+bo ;
    can->MyGetClientSize ( &w , &h ) ;
    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx - 1 ) * can->blocksize ;
    pos.cleanup() ;
//    pos.m.Alloc ( s.length() ) ;
    if ( can->isHorizontal() ) can->setLowX ( ox + ( s.length() ) * can->charwidth ) ;
    int ret = ( s.length() + itemsperline - 1 ) / itemsperline ;
    ret = can->NumberOfLines() * ret * can->charheight + 1 ;
    return ret ;
    }    

void SeqAA::show_direct ( wxDC& dc )
    {
    myass ( itemsperline , "AA:show_direct_ipl" ) ;
    if ( !itemsperline ) return ;
    mylog ( "SeqAA::show_direct" , "0" ) ;
    can->SetFont(*can->font);
    dc.SetFont(*can->font);
    int a , b , w , h , n , bo = can->border ;
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
    else tf = wxColour ( 130 , 130 , 130 ) ;//*wxLIGHT_GREY ;
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
          dc.SetTextForeground ( tf ) ;
          }
       else if ( pm == 2 && can->doOverwrite() ) // Overwrite cursor
          {
          dc.SetBackgroundMode ( wxSOLID ) ;
          dc.SetTextBackground ( *wxBLACK ) ;
          }
       if ( pm == 2 && can->doOverwrite() ) dc.SetTextForeground ( *wxWHITE ) ;
       else dc.SetTextForeground ( tf ) ;
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

       dc.DrawText ( wxString ( s.GetChar(a) ) , px , py ) ;
       
       int pz = py + ch ;

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
          dc.SetTextForeground ( tf ) ;
          }

       // Protease cuts
       for ( int q = 0 ; q < pc.GetCount() ; q++ )
          {
          if ( a == pc[q]->cut )
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
                if ( pn.GetChar(w) == ' ' && pn.GetChar(w+1) == '(' )
                   pn = pn.substr ( 0 , w ) ;
             dc.SetFont(*can->smallFont);
             int u1 , u2 ;
             dc.GetTextExtent ( pn , &u1 , &u2 ) ;
             dc.DrawText ( pn , qx - u1/2 , qy - u2/2 ) ;
             dc.SetFont(*can->font);
             
             if ( primaryMode ) dc.SetTextForeground ( *wxBLACK ) ;
             else dc.SetTextForeground ( *wxLIGHT_GREY ) ;
             }
          }

        if ( showNumber && primaryMode )
           {
           wxString t = wxString::Format ( "%d" , a + 1 ) ;
           while ( endnumberlength > t.length() ) t = "0" + t ;
//           t.Pad ( endnumberlength - t.length() , '0' , false ) ;
           dc.DrawText ( t , bo , py ) ;
           }    
        }    


    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }
    
int SeqAA::getLine ( int y )
    {
    if ( !useDirectRoutines() ) return SeqBasic::getLine ( y ) ;
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

int SeqAA::getItem ( wxPoint pt , int line )
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

int SeqAA::getMarkSize ()    
    {
    if ( !useDirectRoutines() ) return SeqBasic::getMarkSize() ;
    return s.length()+1 ;
    }
    
int SeqAA::getRectSize ()
    {
    if ( !useDirectRoutines() ) return SeqBasic::getRectSize() ;
    return 0 ;
    }
    
wxRect SeqAA::getRect ( int i )
    {
    if ( !useDirectRoutines() ) return pos.r[i] ;
    
    wxRect ret ;
    ret.x = 0 ;
    ret.y = ( (i-1) / itemsperline ) * can->charheight * can->NumberOfLines() ;
    
    return ret ;
    }

int SeqAA::getMark ( int i )
    {
    if ( !useDirectRoutines() ) return SeqBasic::getMark ( i ) ;
    if ( i >= s.length() ) return 0 ;
    return pos.getmark ( i ) ;
    }

void SeqAA::setMark ( int i , int v )
    {
    if ( !useDirectRoutines() ) { SeqBasic::setMark ( i , v ) ; return ; }
    if ( i <= 0 ) return ;
    pos.mark ( i-1 , v ) ;
    }

int SeqAA::getPos ( int i )
    {
    if ( !useDirectRoutines() ) return pos.p[i] ;
    return i ;
    }

void SeqAA::setPos ( int i , int v )
    {
    if ( !useDirectRoutines() ) pos.p[i] = v ;
    }

void SeqAA::logsize ()
    {
    SeqBasic::logsize() ;
    mylog ( whatsthis() , wxString::Format ( "pa_w %d" , pa_w.length() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "!proteases %d" , proteases.GetCount() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "offsets %d" , sizeof(int)*offsets.GetCount() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "pa_wa %d" , sizeof(int)*pa_wa.GetCount() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "!offset_items %d" , offset_items.GetCount() ) ) ;
    }

