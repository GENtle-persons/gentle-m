#include "SequenceCanvas.h"

//************************************************ SeqAA

int SeqAA::arrange ( int n )
    {
    int a , b , x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx ) ;
    itemsperline *= can->blocksize ;
    
    pos.cleanup() ;
    x = ox ;
    y = oy ;
    if ( showNumbers )
       pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    for ( a = 0 ; a < s.length() ; a++ )
        {
        pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        can->setLowX ( x + wx*2 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % can->blocksize == 0 )
           {
           x += wx-1 ;
           if ( x+wx*(can->blocksize+1) >= w )
              {
              pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
              lasta = pos.p.size()+1 ;
              x = ox ;
              y += wy * ( can->seq.size() + can->blankline ) ;
              if ( showNumbers && a+1 < s.length() )
                 pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
              }
           }
        }
    if ( showNumbers && lasta != pos.p.size()+1 ) 
        pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
        
    // Marking features
    string t = s ;
    for ( a = 0 ; a < t.length() ; a++ ) t[a] = ' ' ;
    
    x = 2 ;
    if ( mode != AA_ALL ) x = 0 ;
    
    for ( a = 0 ; vec && a < vec->items.size() ; a++ )
        {
        y = vec->items[a].to ;
        if ( y < vec->items[a].from ) y += s.length() ;
        for ( b = vec->items[a].from-1 ; b+x < y ; b++ )
           {
           t[(b+1)%s.length()] = 'X' ;
           }
        }
    
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        b = pos.p[a] ;
        if ( b > 0 ) // Char
           {
           if ( t[b] == 'X' ) pos.m[a] = 1 ;
           }
        }
        
    return lowy + bo*2 ;
    }
    
void SeqAA::show ( wxDC& dc )
    {
    int cw2 , ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( "A" , &cw2 , &ch2 ) ;
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    string t ;
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
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tz = ty + can->charheight ;
        bool insight = true ;
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( can->getDrawAll() ) insight = true ;
        if ( !insight && ty > yb ) a = pos.p.size() ;
        if ( b > 0 && !insight ) cnt++ ;
        if ( b > 0 && insight ) // Character
           {
           if ( pos.m[a] == 1 )
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
           else if ( pos.m[a] == 2 && can->doOverwrite() )
              {
              dc.SetTextForeground ( *wxWHITE ) ;
              dc.SetTextBackground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxSOLID ) ;
              }
           t = s[b-1] ;
           if ( can->isPrinting() && !can->getPrintToColor() )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           dc.DrawText ( t.c_str(), pos.r[a].x, pos.r[a].y ) ;

           if ( pos.m[a] == 2 && !can->doOverwrite() )
              {
                 int tx = pos.r[a].x , ty = pos.r[a].y ;
                 int tz = ty + can->charheight ;
                 dc.SetPen(*wxBLACK_PEN);
                 dc.DrawLine ( tx-1 , ty , tx-1 , tz ) ;
                 dc.DrawLine ( tx-3 , ty , tx+2 , ty ) ;
                 dc.DrawLine ( tx-3 , tz , tx+2 , tz ) ;
              }
           if ( pos.m[a] > 0 )
              {
              dc.SetTextBackground ( *wxWHITE ) ;
              if ( primaryMode ) dc.SetTextForeground ( *wxBLACK ) ;
              else dc.SetTextForeground ( *wxLIGHT_GREY ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           // Protease cuts
           for ( int q = 0 ; q < pc.size() ; q++ )
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

                 string pn = pc[q]->protease->name ;
                 for ( int w = 0 ; w+1 < pn.length() ; w++ )
                    if ( pn[w] == ' ' && pn[w+1] == '(' )
                       pn = pn.substr ( 0 , w ) ;
                 dc.SetFont(*can->smallFont);
                 int u1 , u2 ;
                 dc.GetTextExtent ( pn.c_str() , &u1 , &u2 ) ;
                 dc.DrawText ( pn.c_str() , qx - u1/2 , qy - u2/2 ) ;
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
           dc.DrawText ( t.c_str() , pos.r[a].x, pos.r[a].y ) ;
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

wxPoint SeqAA::showText ( int ystart , vector <string> &tout )
    {
    wxPoint p ( -1 , -1 ) ;
    int a , b , c ;
    string t ;
    int x = 0 , y = ystart-can->seq.size() , ly = -1 ;
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        b = pos.p[a] ;
        if ( b > 0 ) // Character
           {
           t = s[b-1] ;
           if ( pos.r[a].y != ly ) 
              {
              ly = pos.r[a].y ;
              y += can->seq.size() ;
              x = 0 ;
              while ( y >= tout.size() ) tout.push_back ( "" ) ;
              }
           else x++ ;
           if ( (x) % (can->blocksize+1) == 0 ) x++ ;
           if ( t != "" )
              {
              if ( b >= can->_from && p.x == -1 ) p.x = y ;
              if ( b <= can->_to ) p.y = y ;
              while ( tout[y].length() < x ) tout[y] += " " ;
              tout[y][x-1] = t[0] ;
              }
           }
        }
    if ( can->getLastWhere() != this || can->_from == -1 || can->_to < can->_from )
       p = wxPoint ( -1 , -1 ) ;
    return p ;
    }
    
void SeqAA::initFromString ( string t )
    {
    s = t ;
    vec = NULL ;
    showNumbers = true ;
    offsets.clear() ;
    while ( offsets.size() < s.length() ) offsets.push_back ( -1 ) ;
    
    // Proteases
    updateProteases () ;
    pa_w = "" ;
    pa_wa.clear() ;
    while ( pa_w != s )
       {
       pa_w += s[pa_w.length()] ;
       pa_wa.push_back ( pa_w.length() ) ;
       analyzeProteases () ;
       }
    }

void SeqAA::updateProteases ()
    {
    proteases.clear () ;
    pc.clear () ;
    if ( !can ) return ;

    vector <string> vs ;
    if ( can->child ) vs = can->child->vec->proteases ;
    else if ( can->p ) vs = can->p->vec->proteases ;
    else if ( can->getAA() ) vs = can->getAA()->vec->proteases ;
    
    int a ;
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        TProtease *pro = myapp()->frame->LS->getProtease ( vs[a] ) ;
        if ( pro ) proteases.push_back ( pro ) ;
        }
    }
    
void SeqAA::analyzeProteases ()
    {
    if ( !can ) return ;
    for ( int q = 0 ; q < proteases.size() ; q++ )
       {
       TProtease *pr = proteases[q] ;
       if ( pr->len() <= pa_w.length() )
          {
          string w2 = pa_w.substr ( pa_w.length() - pr->len() , pr->len() ) ;
          if ( pr->does_match ( w2 ) )
             {
             TProteaseCut *cut = new TProteaseCut ;
             cut->protease = pr ;
             cut->cut = pa_wa[pa_w.length()-pr->len()+pr->cut+1] ;
             cut->left = true ;
             pc.push_back ( cut ) ;
             }
          }
       }
    }

void SeqAA::fixOffsets ( TVector *v )
    {
    offset_items.clear() ;
    int a , b ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        int off = v->items[a].getOffset () ;
        int c = 0 ;
        for ( b = v->items[a].from ; off != -1 && b < v->items[a].to ; b++ )
           {
//           int c = b - v->items[a].from ;
           char x =  v->sequence[b-1] ;
           if ( ( b - 1 ) % 10 == 0 && c > 0 && x != '-' )
              {
              while ( offsets.size() < b ) offsets.push_back ( -1 ) ;
              while ( offset_items.size() < b ) offset_items.push_back ( NULL ) ;
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
    string t = vec->sequence ;
    s = "" ;
    while ( s.length() < t.length() ) s += " " ;
    offsets.clear() ;
    offset_items.clear() ;
    while ( offsets.size() < s.length() ) offsets.push_back ( -1 ) ;
    updateProteases () ;
    if ( v->isCircular() ) t += t.substr ( 0 , 2 ) ;
    else t += "  " ;
    int a , b , sl = s.length() ;
    s += "  " ;
    if ( mode == AA_ALL )
        {
        for ( a = 0 ; a < sl ; a++ )
            s[a] = v->dna2aa ( t.substr(a,3) )[0] ;
        }
    else if ( mode == AA_KNOWN )
        {
        for ( a = 0 ; a < v->items.size() ; a++ )
           {
           int rf = v->items[a].getRF () ;
           int dir = v->items[a].direction ;
           int voff = v->items[a].getOffset() ;
           char c = ' ' ;
           int coff ;
           bool complement , roundOnce = false ;
           pa_w = "" ;
           pa_wa.clear() ;
           if ( dir == 1 )
              {
              b = v->items[a].from - 1 ;
              coff = 0 ;
              complement = false ;
              }
           else
              {
              b = v->items[a].to - 1 ;
              coff = -2 ;
              complement = true ;
              }
           b += dir * (rf-1) ;
           while ( c != '|' && rf != 0 )
              {
              string three ;
              three += v->getNucleotide ( b + 0 * dir , complement ) ;
              three += v->getNucleotide ( b + 1 * dir , complement ) ;
              three += v->getNucleotide ( b + 2 * dir , complement ) ;
              c = v->dna2aa ( three )[0] ;
              three = v->one2three((int)c) ;
              
              // Protease analysis
              pa_w += c ;
              pa_wa.push_back ( b+coff ) ;
              analyzeProteases () ;
              
              // Offset?
              int pawl = pa_w.length() ;
              if ( voff != -1 && ( b + coff ) % 10 == 0 ) //&& ( pawl == 1 || (pawl+voff-1) % 10 == 0 ) )
                 {
                 while ( offsets.size() <= b+coff ) offsets.push_back ( -1 ) ;
                 while ( offset_items.size() <= b+coff ) offset_items.push_back ( NULL ) ;
                 offsets[b+coff] = voff + pawl - 1 ;
                 offset_items[b+coff] = &(v->items[a]) ; //????
                 }

              // Output
              if ( disp == AA_ONE ) s[b+coff] = c ;
              else
                 {
                 s[b+0+coff] = three[0] ;
                 s[b+1+coff] = three[1] ;
                 s[b+2+coff] = three[2] ;
                 }
              b += dir * 3 ;
              if ( !v->isCircular() && b+dir*3 < 0 ) rf = 0 ;
              if ( !v->isCircular() && b+dir*3 > v->sequence.length() ) rf = 0 ;
              if ( v->isCircular() && ( b < 0 || b >= v->sequence.length() ) )
                 {
                 if ( roundOnce ) rf = 0 ;
                 else if ( b < 0 ) b += v->sequence.length() ;
                 else b -= v->sequence.length() ;
                 roundOnce = true ;
                 }
//              if ( v->isCircular() && b < 0 ) b += v->sequence.length() ;
//              if ( v->isCircular() && b >= v->sequence.length() ) b -= v->sequence.length() ;
              }
           }
        }
    else
        {
        pa_w = "" ;
        pa_wa.clear() ;
        int mymode = mode ;
        bool invert = false ;
        if ( mode == AA_THREE_M1 ) { mymode = AA_THREE_1 ; invert = true ; }
        if ( mode == AA_THREE_M2 ) { mymode = AA_THREE_2 ; invert = true ; }
        if ( mode == AA_THREE_M3 ) { mymode = AA_THREE_3 ; invert = true ; }
        for ( a = 0 ; a < sl ; a++ )
            {
            string u = t.substr(a,3) ;
            if ( invert )
               {
               u[0] = v->getComplement ( t[a+2] ) ;
               u[1] = v->getComplement ( t[a+1] ) ;
               u[2] = v->getComplement ( t[a+0] ) ;
               }
            char c = v->dna2aa ( u )[0] ;
            string three = v->one2three((int)c) ;

            // Protease analysis
            pa_w += c ;
            pa_wa.push_back ( a ) ;
            analyzeProteases () ;

            if ( ( mymode == AA_THREE_1 && (a+0) % 3 == 0 ) ||
                 ( mymode == AA_THREE_2 && (a-1) % 3 == 0 ) ||
                 ( mymode == AA_THREE_3 && (a-2) % 3 == 0 ) )
                {
                if ( disp == AA_ONE ) s[a] = c ;
                else
                   {
                   s[a+0] = three[0] ;
                   s[a+1] = three[1] ;
                   s[a+2] = three[2] ;
                   }
                }
            }
        }    
    
    s = s.substr ( 0 , sl ) ;
    for ( a = 0 ; a < s.length() ; a++ )
       if ( s[a] == '?' ) s[a] = unknownAA ;
    }
    
