#include "SequenceCanvas.h"

//************************************************ SeqBasic

void SeqBasic::init ( SequenceCanvas *ncan )
    {
    offset = 0 ;
    blocksize = 10 ;
    takesMouseActions = false ;
    can = ncan ;
    endnumberlength = 0 ;
    }

//************************************************ SeqDNA

int SeqDNA::arrange ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy

    itemsperline = ( w - ox ) / ( ( blocksize + 1 ) * wx ) ;
    itemsperline *= blocksize ;
    
    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    for ( a = 0 ; a < s.length() ; a++ )
        {
        pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % blocksize == 0 )
           {
           x += wx-1 ;
           if ( x+wx*(blocksize+1) >= w )
              {
              pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
              lasta = pos.p.size()+1 ;
              x = ox ;
              y += wy * ( can->seq.size() + can->blankline ) ;
              if ( a+1 < s.length() )
                 pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
              }
           }
        }
    if ( lasta != pos.p.size()+1 ) 
        pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
    return lowy + bo*2 ;
    }
    
wxPoint SeqDNA::showText ( int ystart , vector <string> &tout )
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
           if ( (x) % (blocksize+1) == 0 ) x++ ;
           if ( t != "" )
              {
              if ( b >= can->_from && p.x == -1 ) p.x = y ;
              if ( b <= can->_to ) p.y = y ;
              while ( tout[y].length() < x ) tout[y] += " " ;
              tout[y][x-1] = t[0] ;
              }
           }
        }
    if ( can->lastwhere != this || can->_from == -1 || can->_to < can->_from )
       p = wxPoint ( -1 , -1 ) ;
    return p ;
    }

void SeqDNA::show ( wxDC& dc )
    {
    dc.SetFont(*can->font);
    wxBrush theLightBrush ( wxColour ( 230 , 230 , 230 ) , wxSOLID ) ;
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    string t ;
    char u[100] , valid[256] ;
    for ( a = 0 ; a < 256 ; a++ ) valid[a] = 0 ;
    valid['A'] = valid['C'] = valid['T'] = valid['G'] = valid[' '] = 1 ;
//    dc.SetTextBackground ( *wxWHITE ) ;
    dc.SetTextForeground ( fontColor ) ;
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
        bool insight = true ; // Meaning "is this part visible"
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( can->drawall ) insight = true ;
        if ( !insight && ty > yb ) a = pos.p.size() ;
        if ( b > 0 && !insight ) cnt++ ;
        if ( b > 0 && insight ) // Character
           {
           t = s[b-1] ;
           if ( pos.m[a] == 1 ) // Marked (light gray background)
              {
              dc.SetBackgroundMode ( wxSOLID ) ;
              dc.SetTextBackground ( *wxLIGHT_GREY ) ;
              dc.SetTextForeground ( *wxBLACK ) ;
              }
           else if ( pos.m[a] == 2 && can->doOverwrite() ) // Overwrite cursor
              {
              dc.SetBackgroundMode ( wxSOLID ) ;
              dc.SetTextBackground ( *wxBLACK ) ;
              }
           if ( pos.m[a] == 2 && can->doOverwrite() ) dc.SetTextForeground ( *wxWHITE ) ;
           else dc.SetTextForeground ( getBaseColor ( t[0] ) ) ;
           if ( can->printing && pos.m[a] == 1 )
              {
              dc.SetBrush ( theLightBrush ) ;
              dc.SetPen(*wxTRANSPARENT_PEN);
              dc.DrawRectangle ( tx , ty , can->charwidth , can->charheight ) ;
              }
           if ( can->printing && !can->printToColor )
              {
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              dc.SetTextForeground ( *wxBLACK ) ;
              }

           dc.DrawText ( t.c_str() , tx , ty ) ;

           if ( pos.m[a] == 2 && !can->doOverwrite() ) // Insert cursor
              {
                 dc.SetPen(*wxBLACK_PEN);
                 dc.DrawLine ( tx-1 , ty , tx-1 , tz ) ;
                 dc.DrawLine ( tx-3 , ty , tx+2 , ty ) ;
                 dc.DrawLine ( tx-3 , tz , tx+2 , tz ) ;
              }
           if ( pos.m[a] > 0 ) // Reverting cursor settings
              {
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              dc.SetTextForeground ( fontColor ) ;
              }
           cnt++ ;
           }
        else if ( insight ) // Front number
           {
           if ( showNumbers )
              {
              sprintf ( u , "%d" , cnt ) ;
              t = u ;
              while ( t.length() < endnumberlength ) t = "0" + t ;
              }
           else t = alternateName ;
           dc.SetTextForeground ( *wxBLACK ) ;
           dc.DrawText ( t.c_str() , pos.r[a].x, pos.r[a].y ) ;
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }
    
wxColor SeqDNA::getBaseColor ( char b )
    {
    if ( b == 'N' ) return *wxRED ;
    return fontColor ;
    }
    
void SeqDNA::initFromTVector ( TVector *v )
    {
    vec = v ;
    s = vec->sequence ;
    takesMouseActions = true ;
    showNumbers = true ;
    fontColor.Set ( 0 , 0 , 0 ) ;
    }

//************************************************ SeqPrimer

void SeqPrimer::show ( wxDC& dc )
    {
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    string t ;
    char u[100] ;
    dc.SetTextBackground ( *wxWHITE ) ;
    dc.SetTextForeground ( *wxBLACK ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;
    
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
        if ( can->drawall ) insight = true ;
        if ( !insight && ty > yb ) a = pos.p.size() ;
        if ( b > 0 && !insight ) cnt++ ;
        if ( b > 0 && insight ) // Character
           {
           t = s[b-1] ;
           if ( pos.m[a] == 1 )
              {
              dc.SetTextBackground ( *wxLIGHT_GREY ) ;
              dc.SetTextForeground ( *wxBLACK ) ;
              }
           else if ( pos.m[a] == 2 && can->doOverwrite() )
              {
              dc.SetTextBackground ( *wxBLACK ) ;
              dc.SetTextForeground ( *wxWHITE ) ;
              }
           if ( s[b-1] == vec->sequence[b-1] ) dc.SetTextForeground ( *wxBLUE ) ;
           else dc.SetTextForeground ( *wxRED ) ;
           if ( can->printing && !can->printToColor )
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
              dc.SetTextForeground ( *wxBLACK ) ;
              }
           cnt++ ;
           }
        else if ( insight ) // Front number
           {
           dc.SetTextForeground ( *wxBLUE ) ;
           if ( showNumbers )
              {
              sprintf ( u , "%d" , cnt ) ;
              t = u ;
              while ( t.length() < endnumberlength ) t = "0" + t ;
              }
           else t = alternateName ;
           dc.DrawText ( t.c_str() , pos.r[a].x, pos.r[a].y ) ;
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

void SeqPrimer::initFromTVector ( TVector *v )
    {
    vec = v ;
    s = vec->sequence ;
    for ( int a = 0 ; a < s.length() ; a++ ) s[a] = ' ' ;
    takesMouseActions = true ;
    showNumbers = false ;
    }
    
void SeqPrimer::addPrimer ( TPrimer *p )
    {
    int a ;
    for ( a = p->from ; a <= p->to ; a++ )
        s[a-1] = p->sequence[a-p->from] ;
    }
    
    
//************************************************ SeqRestriction

int SeqRestriction::arrange ( int n )
    {
    int a , b , x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 , cut , thepos ;
    string t = "" ;
    
    while ( vs.size() ) vs.pop_back () ;
    while ( ven.size() ) ven.pop_back () ;
    while ( eoe.size() ) eoe.pop_back () ;
    while ( t.length() < s.length() ) t += " " ;
    s = t ;
    for ( cut = 0 ; cut < vec->rc.size() ; cut++ )
        {
        vs.push_back ( t ) ;
        eoe.push_back ( -1 ) ;
        TRestrictionEnzyme *e = vec->rc[cut].e ;
        ven.push_back ( e->name ) ;
        for ( a = 0 ; a < e->sequence.length() ; a++ )
           {
           thepos = a+vec->rc[cut].pos-e->cut ;
           if ( thepos >= 0 && thepos < t.length() )
              vs[cut][thepos] = '-' ;
           }
           
        // Arranging enzyme text
        eoe[cut] = vec->rc[cut].pos - e->cut + e->sequence.length() ;
           
        thepos = vec->rc[cut].pos ;
        if ( thepos-1 >= 0 && thepos < t.length() )
           {
           vs[cut][thepos] = '|' ;
           vs[cut][vec->rc[cut].pos-1] = '#' ;
           }
        }
    
    // Merging all cuts
    for ( a = 0 ; a < s.length() ; a++ )
        {
        s[a] = ' ' ;
        for ( cut = 0 ; cut < vs.size() ; cut++ )
           if ( s[a] == ' ' )
              s[a] = vs[cut][a] ;
        }
        
    // Generating Y offsets
    vector <string> vt = vs ;

    // An yoff for each vs (and each vt)
    while ( yoff.size() > 0 ) yoff.pop_back() ;
    while ( yoff.size() < vs.size() ) yoff.push_back ( yoff.size() ) ;
    
    // Trying to merge several lines together
    for ( cut = 1 ; cut < vt.size() ; cut++ )
        {
        x = -1 ;
        for ( a = 0 ; x == -1 && a < vt.size() ; a++ )
           {
           for ( b = 0 ; b < s.length() && ( vt[cut][b] == ' ' || vt[a][b] == ' ' ) ; b++ ) ;
           if ( b == s.length() ) x = a ;
           }
           
        // At this point, x is the first lane where vt[cut] would merge with
        // If x == -1, no match was found, so it stays where it is
        if ( x != -1 )
           {
           yoff[cut] = x ;
           for ( a = 0 ; x != cut && a < s.length() ; a++ )
              {
              if ( vt[cut][a] != ' ' )
                 {
                 vt[x][a] = vt[cut][a] ;
                 vt[cut][a] = ' ' ;
                 }
              }
           }
        else yoff[cut] = cut ;
        }

    // Setting basic values
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy

    pos.cleanup() ;
    for ( int layer = 0 ; layer < vs.size() ; layer++ )
        {
        x = ox ;
        y = oy ;
        for ( a = 0 ; a < s.length() ; a++ )
            {
            int xoff = 0 ;
            if ( (a+1) % blocksize == 0 ) xoff = wx/3 ;
            if ( vs[layer][a] != ' ' ) pos.add ( a+1 , x , y , wx-1+xoff , wy-1 ) ;
            if ( y+wy > lowy ) lowy = y+wy ;
            x += wx ;
            if ( (a+1) % blocksize == 0 )
               {
               x += wx ;
               if ( x+wx*(blocksize+1) >= w )
                  {
                  pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
                  lasta = pos.p.size()+1 ;
                  x = ox ;
                  y += wy * ( can->seq.size() + can->blankline ) ;
                  }
               }
            }
        if ( lasta != pos.p.size()+1 ) 
            pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
        }
    return lowy + bo*2 ;
    }
    
void SeqRestriction::show ( wxDC& dc )
    {
    int cw2 , ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( "A" , &ch2 , &ch2 ) ;
    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    for ( int layer = 0 ; layer < vs.size() ; layer++ )
        {
        int a , b , cut ;
        char u[100] ;
        char lc = ' ' ;
        int ly = -1 ;
        int yo = yoff[layer] * 2 - 6 ;
        if ( down ) yo = can->charheight / 2 - ( yoff[layer] * 2 ) ;
        int llx = 0 ;
        switch ( yoff[layer]%3 )
            {
            case 0 : dc.SetPen(*wxRED_PEN); 
                     dc.SetTextForeground ( wxColor ( *wxRED ) ) ;
                     break ;
            case 1 : dc.SetPen(wxPen(wxColour(0,200,0),1,wxSOLID)); 
                     dc.SetTextForeground ( wxColour ( 0 , 200 , 0 ) ) ; 
                     break ;
            case 2 : dc.SetPen(wxPen(wxColour(0,0,200),1,wxSOLID)); 
                     dc.SetTextForeground ( wxColour ( 0 , 0 , 200 ) ) ; 
                     break ;
            }
        if ( can->printing && !can->printToColor )
           {
           dc.SetPen(wxPen(*wxBLACK,1,wxSOLID)); 
           dc.SetTextForeground ( *wxBLACK ) ; 
           }
        int qlx = -1 ;
        for ( a = 0 ; a < pos.p.size() ; a++ )
            {
            b = pos.p[a] ;
            char c = vs[layer][b-1] ;
            if ( c != ' ' )
               {
               if ( qlx == -1 ) qlx = pos.r[a].GetLeft() ;
               int lx = pos.r[a].GetLeft() ;
               int x = ( lx + pos.r[a].GetRight() ) / 2 ;
               int bt = pos.r[a].GetTop() + yo ;
               if ( down ) bt = pos.r[a].GetBottom() + yo ;
               int y = ( pos.r[a].GetTop() + pos.r[a].GetBottom() ) / 2 + yo ;
               if ( lc != ' ' && ly == y ) lx = pos.r[a-1].GetRight() ;
               
               if ( c == '-' )
                  {
                  dc.DrawLine ( lx , y , pos.r[a].GetRight() , y ) ;
                  }
               else if ( c == '#' )
                  {
                  if ( lx < x )
                     dc.DrawLine ( lx , y , x , y ) ;
                  dc.DrawLine ( x , y , pos.r[a].GetRight() , bt ) ;
                  }
               else if ( c == '|' )
                  {
                  dc.DrawLine ( lx , bt , x , y ) ;
                  dc.DrawLine ( x , y , pos.r[a].GetRight() , y ) ;
                  }
               llx = pos.r[a].GetRight() ;
               ly = y ;
               qlx = x ;
               }
            if ( b == eoe[layer] )
               {
               if ( down ) dc.DrawText ( ven[layer].c_str() , llx , ly - ch2 ) ;
               else dc.DrawText ( ven[layer].c_str() , llx , ly ) ;
               }
            lc = c ;
            if ( pos.r[a].GetTop() > yb ) a = pos.p.size() ;
            }
        }
    dc.SetTextForeground ( wxColor ( *wxBLACK ) ) ;
    }
    
void SeqRestriction::initFromTVector ( TVector *v )
    {
    vec = v ;
    s = vec->sequence ;
    down = false ;
    }


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

    itemsperline = ( w - ox ) / ( ( blocksize + 1 ) * wx ) ;
    itemsperline *= blocksize ;
    
    pos.cleanup() ;
    x = ox ;
    y = oy ;
    if ( showNumbers )
       pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    for ( a = 0 ; a < s.length() ; a++ )
        {
        pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % blocksize == 0 )
           {
           x += wx-1 ;
           if ( x+wx*(blocksize+1) >= w )
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
        if ( can->drawall ) insight = true ;
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
           if ( can->printing && !can->printToColor )
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
           if ( (x) % (blocksize+1) == 0 ) x++ ;
           if ( t != "" )
              {
              if ( b >= can->_from && p.x == -1 ) p.x = y ;
              if ( b <= can->_to ) p.y = y ;
              while ( tout[y].length() < x ) tout[y] += " " ;
              tout[y][x-1] = t[0] ;
              }
           }
        }
    if ( can->lastwhere != this || can->_from == -1 || can->_to < can->_from )
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
    else if ( can->aa ) vs = can->aa->vec->proteases ;
    
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
        for ( b = v->items[a].from ; off != -1 && b < v->items[a].to ; b++ )
           {
           int c = b - v->items[a].from ;
           if ( ( b - 1 ) % 10 == 0 && c > 0 )
              {
              while ( offsets.size() < b ) offsets.push_back ( -1 ) ;
              while ( offset_items.size() < b ) offset_items.push_back ( NULL ) ;
              offsets[b-1] = c + off ;
              offset_items[b-1] = &v->items[a] ;
              }
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

//************************************************ SeqAlign

int SeqAlign::arrange ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    
    endnumberlength = 0 ;
    for ( a = 0 ; a < can->seq.size() ; a++ )
        {
        if ( can->seq[a]->whatsthis() == "ALIGN" )
           {
           SeqAlign *z = (SeqAlign*) can->seq[a] ;
           if ( z->myname.length() > endnumberlength )
              endnumberlength = z->myname.length() ;
           }
        }
    if ( endnumberlength > can->maxendnumberlength )
        endnumberlength = can->maxendnumberlength ;
    
    ox += wx * endnumberlength ;
    can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy

    itemsperline = ( w - ox ) / ( ( blocksize + 1 ) * wx ) ;
    itemsperline *= blocksize ;
    
    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    for ( a = 0 ; a < s.length() ; a++ )
        {
        pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % blocksize == 0 )
           {
           x += wx-1 ;
           if ( x+wx*(blocksize+1) >= w )
              {
              pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
              lasta = pos.p.size()+1 ;
              x = ox ;
              y += wy * ( can->seq.size() + can->blankline ) ;
              if ( a+1 < s.length() )
                 pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
              }
           }
        }
    if ( lasta != pos.p.size()+1 ) 
        pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
    return lowy + bo*2 ;
    }
    
void SeqAlign::show ( wxDC& dc )
    {
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    string t ;
    char u[100] ;
    
    wxColour nbgc ;
    for ( a = 0 ; can->seq[a] != this ; a++ ) ;
    switch ( a%3 )
        {
        case 0 : nbgc = wxColour ( 255 , 200 , 200 ) ; break ;
        case 1 : nbgc = wxColour ( 200 , 200 , 255 ) ; break ;
        case 2 : nbgc = wxColour ( 200 , 255 , 200 ) ; break ;
        }
    
    int first = -1 , me = -1 ;
    for ( a = 0 ; a < can->seq.size() ; a++ )
        {
        if ( can->seq[a]->whatsthis() == "ALIGN" && first == -1 )
           first = a ;
        if ( can->seq[a] == this )
           me = a ;
        }
    wxColour fg = *wxBLACK , bg = *wxWHITE ;
    TAlignment *al = (TAlignment*) can->p ;
    dc.SetTextForeground ( fg ) ;
    dc.SetTextBackground ( bg ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        b = pos.p[a] ;
        if ( b > 0 ) // Character
           {
           t = s[b-1] ;
           if ( pos.m[a] == 1 )
              {
              dc.SetTextBackground ( *wxLIGHT_GREY ) ;
              dc.SetTextForeground ( *wxBLACK ) ;
              }
           else if ( first != me )
              {
              fg = al->findColors ( s[b-1] , can->seq[first]->s[b-1] , true ) ;
              bg = al->findColors ( s[b-1] , can->seq[first]->s[b-1] , false ) ;
              dc.SetTextForeground ( fg ) ;
              dc.SetTextBackground ( bg ) ;
              }
           if ( can->printing && !can->printToColor )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           dc.DrawText ( t.c_str(), pos.r[a].x, pos.r[a].y ) ;

           if ( pos.m[a] > 0 )
              {
              dc.SetTextForeground ( fg ) ;
              dc.SetTextBackground ( bg ) ;
              }
           cnt++ ;
           }
        else // Front number
           {
           dc.SetTextForeground ( *wxBLACK ) ;
           dc.SetTextBackground ( nbgc ) ;
           for ( t = "" ; t.length() < endnumberlength ; t += " " ) ;
           dc.DrawText ( t.c_str() , pos.r[a].x, pos.r[a].y ) ;
           dc.SetFont(*can->varFont);
           t = myname ;
           t = " " + t ;
           int tw , th ;
           do {
              t = t.substr ( 1 , t.length()-1 ) ;
              dc.GetTextExtent ( t.c_str() , &tw , &th ) ;
              } while ( tw > pos.r[a].width ) ;
           dc.DrawText ( t.c_str() , pos.r[a].x, pos.r[a].y ) ;
           dc.SetTextBackground ( *wxWHITE ) ;
           dc.SetFont(*can->font);
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }
    
//************************************************ SeqNum

int SeqNum::arrange ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    
    endnumberlength = 0 ;
    for ( a = 0 ; a < can->seq.size() ; a++ )
        {
        if ( can->seq[a]->whatsthis() == "ALIGN" )
           {
           SeqAlign *z = (SeqAlign*) can->seq[a] ;
           if ( z->myname.length() > endnumberlength )
              endnumberlength = z->myname.length() ;
           }
        }
    if ( endnumberlength > can->maxendnumberlength )
        endnumberlength = can->maxendnumberlength ;
    
    ox += wx * endnumberlength ;
    can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy

    pos.cleanup() ;
    x = ox ;
    y = oy ;
    bool blockstart = true ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( blockstart ) pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        blockstart = false ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % blocksize == 0 )
           {
           blockstart = true ;
           x += wx-1 ;
           if ( x+wx*(blocksize+1) >= w )
              {
              pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
              lasta = pos.p.size()+1 ;
              x = ox ;
              y += wy * ( can->seq.size() + can->blankline ) ;
              }
           }
        }
    if ( lasta != pos.p.size()+1 ) 
        pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
    return lowy + bo*2 ;
    }
    
void SeqNum::show ( wxDC& dc )
    {
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int a , bm = dc.GetBackgroundMode () ;
    dc.SetTextForeground ( *wxBLACK ) ;
    dc.SetTextBackground ( *wxWHITE ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;
    char tt[100] ;
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        sprintf ( tt , "%d" , pos.p[a]+offset ) ;
        dc.DrawText ( tt, pos.r[a].x, pos.r[a].y ) ;
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

//************************************************ SeqDivider

int SeqDivider::arrange ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy

    itemsperline = ( w - ox ) / ( ( blocksize + 1 ) * wx ) ;
    itemsperline *= blocksize ;
    
    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    for ( a = 0 ; a < s.length() ; a++ )
        {
//        pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % blocksize == 0 )
           {
           x += wx-1 ;
           if ( x+wx*(blocksize+1) >= w )
              {
              pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
              lasta = pos.p.size()+1 ;
              x = ox ;
              y += wy * ( can->seq.size() + can->blankline ) ;
              if ( a+1 < s.length() )
                 pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
              }
           }
        }
    if ( lasta != pos.p.size()+1 ) 
        pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
    return lowy + bo*2 ;
    }
    
void SeqDivider::show ( wxDC& dc )
    {
    int a , w , h ;
    can->MyGetSize ( &w , &h ) ;
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        int y = pos.r[a].y + can->charheight/2 ;
        dc.SetPen(*wxGREY_PEN);
        dc.DrawLine ( 0 , y-1 , w , y-1 ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine ( 0 , y , w , y ) ;
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine ( 0 , y+1 , w , y+1 ) ;
        }
    }

void SeqDivider::initFromTVector ( TVector *v )
    {
//    vec = v ;
    s = v->sequence ;
    }


//************************************************ SeqABI
    

int SeqABI::arrange ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy
    scalex = screenScale ;
    if ( can->printing )
       {
       scalex = w / 500 ;
       w = w * 9 / 10 ;
       }

    itemsperline = ( w - ox ) / ( ( blocksize + 1 ) * wx ) ;
    itemsperline *= blocksize ;

    int id ;

    // Initiate Color coding
    int intensity = 200 ;
    colors[0] = wxColour ( 0 , 0 , 0 ) ;
    colors[1] = wxColour ( intensity , 0 , 0 ) ;
    colors[2] = wxColour ( 0 , intensity , 0 ) ;
    colors[3] = wxColour ( 0 , 0 , intensity ) ;

    // Base order
    id = at->getRecord ( "FWO_" , 1 ) ;
    assert ( id != -1 ) ;
    
    // GATC
    int *ii = (int*) base2color ;
    *ii = at->vf[id].value ;
    
    // For painting
    minx = ox ;
    maxx = w - ox - wx ;
    int diffx = 0 ;
    id = at->getRecord ( "PLOC" , 2 ) ;
    assert ( id != -1 ) ;

    
    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    
    for ( a = 0 ; a < s.length() ; a++ )
        {
        int bx1 = (char) at->vf[id].data[a*2] ;
        int bx2 = (unsigned char) at->vf[id].data[a*2+1] ;
        int bx = bx1 * 256 + bx2 ;
        char t = s[a] ;
        x = bx * scalex - diffx ;


        if ( x > maxx )
           {
           pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
           lasta = pos.p.size()+1 ;
           y += wy * ( can->seq.size() + can->blankline ) ;
           if ( a+1 < s.length() )
              pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
           diffx += maxx ;
           x = bx * scalex - diffx ;
           }

        pos.add ( a+1 , x + minx - wx / 2 , y , wx-1 , wy-1 ) ;
        lowy = y+wy ;

        }
    if ( lasta != pos.p.size()+1 ) 
        pos.addline ( lasta , pos.p.size() , y , y+wy-1 ) ;
    
    lowy += wy * ( can->blankline ) ; // For sequence data
    
    return lowy + bo*2 ;
    }
    
void SeqABI::show ( wxDC& dc )
    {
    SeqDNA::show ( dc ) ;

    int data ;

    // Char-to-curve (the thin grey lines;-)
    int wx = can->charwidth ;
    int wy = can->charheight ;
    dc.SetPen(*wxLIGHT_GREY_PEN);
    for ( int a = 0 ; showHelpLines && a < pos.p.size() ; a++ )
        {
        if ( pos.p[a] > 0 )
           {
           int tx = pos.r[a].x + wx / 2 , ty = pos.r[a].y ;
           dc.DrawLine ( tx , ty + wy , tx , ty + wy * can->blankline ) ;
           }
        }
    
    // Determining height scaling factors
    int hy = ( can->blankline - 1 ) * can->charheight ;
    int high = 0 ;
    for ( data = 0 ; data < 4 ; data++ )
        {
        int id = at->getRecord ( "DATA" , 9 + data ) ;
        for ( int a = 0 ; a < at->vf[id].nrecords ; a++ )
           {
           int b1 = (char) at->vf[id].data[a*2] ;
           int b2 = (unsigned char) at->vf[id].data[a*2+1] ;
           int b = b1 * 256 + b2 ;
           if ( b > high ) high = b ;
           }
        }
    
    // Painting
    for ( data = 0 ; data < 4 ; data++ )
        {
        if ( can->printing && !can->printToColor )
           dc.SetPen ( wxPen ( *wxBLACK , 1 , wxSOLID ) ) ;
        else
           dc.SetPen ( wxPen ( colors[data] , 1 , wxSOLID ) ) ;
        
        int a ;
        int id = at->getRecord ( "DATA" , 9 + data ) ;
        assert ( id > -1 ) ;
        int diffx = 0 ;
        int l = 0 ;
        wxPoint last ( minx , pos.l[l].GetHeight() + hy ) ;
        int x = 0 ;
        for ( a = 0 ; a < at->vf[id].nrecords ; a++ )
           {
           int b1 = (unsigned char) at->vf[id].data[a*2] ;
           int b2 = (unsigned char) at->vf[id].data[a*2+1] ;
           int b = b1 * 256 + b2 ;
           x += scalex ;
           while ( x > maxx )
              {
              x -= maxx ;
              diffx += maxx ;
              if ( l > 0 ) drawTopLine ( dc , pos.l[l].GetWidth() - wy / 2 ) ;
              l++ ;
              if ( l < pos.l.size() )
                 {
                 int yo = last.y - pos.l[l-1].GetHeight() ;
                 last = wxPoint ( minx , pos.l[l].GetHeight() + yo ) ;
                 }
              }
           wxPoint cur ;
           if ( l < pos.l.size() )
              {
              b *= zoom ;
              if ( b > high ) b = high ;
              cur = wxPoint ( x + minx , hy - b * hy / high + pos.l[l].GetHeight() ) ;
              dc.DrawLine ( last , cur ) ;
              }
           last = cur ;
           }
        if ( l > 0 ) drawTopLine ( dc , pos.l[l].GetWidth() - wy / 2 ) ;
        }

    }
    
void SeqABI::drawTopLine ( wxDC &dc , int y )
    {
    wxPen p = dc.GetPen () ;
    dc.SetPen(*wxLIGHT_GREY_PEN);
    dc.DrawLine ( 4 , y , minx + maxx , y ) ;
    dc.SetPen(p);
    }
    
void SeqABI::initFromFile ( string filename )
    {
    zoom = 1 ;
    showHelpLines = true ;
    screenScale = 2 ;
    if ( !at ) at = new ABItype ;
    at->parse ( filename ) ;
    s = at->getSequence() ;
    vec->sequence = s ;
    showNumbers = true ;
    fontColor.Set ( 0 , 0 , 0 ) ;
    }
    
wxColor SeqABI::getBaseColor ( char b )
    {
    for ( int a = 0 ; a < 4 ; a++ )
       if ( b == base2color[a] ) return colors[3-a] ; // Don't ask...
    return wxColor ( "magenta" ) ;
    }
    
//************************************************ SeqFeature

void SeqFeature::show ( wxDC& dc )
    {
    int cw2 , ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( "A" , &cw2 , &ch2 ) ;
//    dc.SetFont(*can->font);
    wxBrush theLightBrush ( wxColour ( 230 , 230 , 230 ) , wxSOLID ) ;
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    string t ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    bool newline = true ;
    
    vector <int> li , lx , used ;
    while ( li.size() < vr.size() ) li.push_back ( -1 ) ;
    while ( lx.size() < vr.size() ) lx.push_back ( -1 ) ;
    while ( used.size() < vr.size() ) used.push_back ( 0 ) ;
    
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tz = ty + can->charheight ;
        bool insight = true ; // Meaning "is this part visible"
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( can->drawall ) insight = true ;
        if ( ty > yb ) a = pos.p.size() ;
        if ( b > 0 ) // Character
           {
           t = " " ;
           int i ;
           vector <int> _i ;
           vector <string> _name ;
           vector <wxPoint> _point ;
           for ( i = 0 ; i < vr.size() ; i++ ) used[i] = 0 ;
           for ( i = 0 ; i < vr.size() ; i++ )
              {
              if ( doesHit ( i , b ) )
                 {
                 int l = vr[i].y ;
                 wxASSERT_MSG ( maxlayers >= l , "Oh no!" ) ;
                 used[l] = 1 ;
                 int x_from = lx[l] ;
                 if ( x_from < 0 ) x_from = tx ;
                 int x_to = tx + can->charwidth ;
                 int level = can->charheight ;
                 int lh = level * l / ( maxlayers + 1 ) ;
                 level = ty + level - lh - 1 ;
                 if ( insight )
                    {
                    int mode = atoi((vec->items[vr[i].GetX()].getParam("SEQUENCE_STYLE")).c_str()) ;
                    wxColour col ;
                    
                     if ( can->printing && !can->printToColor ) dc.SetPen ( *wxBLACK_PEN ) ;
                     else
                        {
                        col = vec->items[vr[i].GetX()].getFontColor() ;
                        int pw = 1 ;
//                        if ( mode == 1 ) pw = 2 ;
                        dc.SetPen ( wxPen ( col , pw , wxSOLID ) ) ;
//                        dc.SetPen ( pens[l] ) ;
                        }

                    // Offsets
                    if ( aaa && !newline && 
                         b-1 < aaa->offsets.size() && 
                         aaa->offsets[b-1] != -1 && 
                         aaa->offset_items[b-1] == &vec->items[vr[i].GetX()] )
                       {
                       dc.SetTextForeground ( col ) ;
                       wxString pn = wxString::Format ( "%d" , aaa->offsets[b-1] ) ;
                       int u1 , u2 ;
                       dc.GetTextExtent ( pn , &u1 , &u2 ) ;
                       dc.DrawText ( pn , x_to - can->charwidth , level - u2 ) ;
                       }
    
                    if ( mode == FEAT_ALPHA ) // Alpha helix
                       {
                       int px , py = 0 ;
                       for ( px = x_from ; px <= x_to ; px++ )
                          {
                          int npy ;//= ( px % lh ) - lh/2 ;
                          npy = (float) sin ( (float) ( px - x_from ) * PI * 4 / can->charwidth ) * lh / 2 ;
                          dc.DrawLine ( px , level + py , px , level + npy ) ;
                          py = npy ;
                          }
                       }
                    else if ( mode == FEAT_BETA ) // Beta sheet
                       {
                       int o ;
                       for ( o = -lh/4 ; o <= lh/4 ; o++ )
                          {
                          dc.DrawLine ( x_from , level+o , x_to , level+o ) ;
                          dc.DrawLine ( x_to , level+o , x_to + cw2/2 , level ) ;
                          }
                       }
                    else if ( mode == FEAT_MUT ) // Mutation
                       {
                       int xw = can->charwidth / 2 ;
                       int yh = level - ty + lh ;
                       int xm = x_to - xw ;
                       int yo = yh / 4 ;
                       dc.DrawLine ( xm , level , xm , ty + yh ) ;
                       dc.DrawLine ( xm , level , xm - xw , level + yo ) ;
                       dc.DrawLine ( xm , level , xm + xw , level + yo ) ;
                       dc.DrawLine ( xm , ty + yh , xm - xw , ty + yh - yo ) ;
                       dc.DrawLine ( xm , ty + yh , xm + xw , ty + yh - yo ) ;
                       }
                    else if ( mode == FEAT_NOLINE ) // No line
                       {
                       // That's right boys, no line is drawn here!
                       }
                    else if ( mode == FEAT_VLINE ) // Vertical line
                       {
                       int xw = can->charwidth / 2 ;
                       int yh = level - ty + lh ;
                       int xm = x_to - xw ;
                       int yo = yh / 4 ;
                       dc.DrawLine ( xm , level , xm , ty + yh ) ;
                       }
                    else if ( mode == FEAT_NONE ) // Default
                       {
                       dc.DrawLine ( x_from , level , x_to , level ) ;
                       }
                    }
                 if ( newline || li[l] != i ) // Start of new item on this lane
                    {
                    if ( insight )
                       {
                       string name = vec->items[vr[i].x].name.c_str() ;
                       if ( newline && li[l] == i ) name = "(" + name + ")" ;
                       _i.push_back ( i ) ;
                       _name.push_back ( name ) ;
                       _point.push_back ( wxPoint ( x_from , level - ch2 ) ) ;
                       }
                    li[l] = i ;
                    }
                 lx[l] = x_to ;
                 }
              }
           for ( i = 0 ; i < vr.size() ; i++ )
              if ( !used[i] && lx[i] >= 0 )
                 lx[i] = -1 ;
              
           // Now drawing the names
           for ( i = 0 ; i < _i.size() ; i++ )
              {
               if ( can->printing && !can->printToColor )
                  {
                  dc.SetTextForeground ( *wxBLACK ) ;
                  }
               else
                  {
                  wxColour col = vec->items[vr[_i[i]].GetX()].getFontColor() ;
                  dc.SetTextForeground ( col ) ;
//                  dc.SetTextForeground ( pens[vr[_i[i]].y].GetColour() ) ;
                  }
               if ( _name[i] != "()" )
                  dc.DrawText ( _name[i].c_str() , _point[i] ) ;
              }

           newline = false ;
           cnt++ ;
           }
        else // New line
           {
           newline = true ;
           int i ;
           for ( i = 0 ; i < vr.size() ; i++ )
              if ( lx[i] >= 0 )
                 lx[i] = -2 ;
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }
    
void SeqFeature::initFromTVector ( TVector *v )
    {
    // misusing the wxRect class to store :
    // item ID as x
    // item display level as y
    // item.from as width
    // item.to as height
    int a , b ;
    vec = v ;
    s = vec->sequence ;
    maxlayers = 0 ;
    for ( a = 0 ; a < s.length() ; a++ ) s[a] = ' ' ;
    while ( vr.size() ) vr.pop_back () ;
    for ( a = 0 ; a < v->items.size() ; a++ )
       {
       if ( v->items[a].isVisible() )
          vr.push_back ( wxRect ( a , 0 , v->items[a].from , v->items[a].to ) ) ;
       }
       
    // Sorting, largest features first
    for ( a = 0 ; a+1 < vr.size() ; a++ )
       {
       b = a + 1 ;
       int la = vr[a].height - vr[a].width ;
       int lb = vr[b].height - vr[b].width ;
       if ( vr[a].height < vr[a].width ) la += s.length() ;
       if ( vr[b].height < vr[b].width ) lb += s.length() ;
       if ( la < lb )
          {
          wxRect r = vr[a] ;
          vr[a] = vr[b] ;
          vr[b] = r ;
          a = 0 ;
          }
       }
       
    // Aligning
    for ( a = 0 ; a < vr.size() ; a++ )
       {
       for ( b = 0 ; b < a ; b++ )
          {
          if ( collide ( a , b ) )
             {
             vr[a].y++ ;
             if ( vr[a].y > maxlayers ) maxlayers = vr[a].y ;
             b = -1 ;
             }
          }
       }
       
    pens.clear() ;
//    while ( pens.size() ) pens.pop_back () ;
    pens.push_back ( *wxBLACK_PEN ) ;
    pens.push_back ( *wxRED_PEN ) ;
    pens.push_back ( wxPen ( wxColour ( 0 , 100 , 0 ) , 1 , wxSOLID  ) ) ;
    for ( a = 0 ; pens.size() <= maxlayers ; a++ ) pens.push_back ( pens[a] ) ;
    }
    
bool SeqFeature::collide ( int a , int b )
    {
    if ( vr[a].y != vr[b].y ) return false ;
    int i ;
    for ( i = 0 ; i < vec->sequence.length() ; i++ )
       if ( doesHit ( a , i+1 ) && doesHit ( b , i+1 ) )
          return true ;
    return false ;
    }
    
bool SeqFeature::doesHit ( int a , int x )
    {
    int from = vr[a].width ;
    int to = vr[a].height ;
    if ( from <= to )
       {
       return ( x >= from && x <= to ) ;
       }
    else
       {
//       return false ;
       return ( x <= to || x >= from ) ;
       }
    }
