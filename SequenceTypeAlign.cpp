#include "SequenceCanvas.h"


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

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx ) ;
    itemsperline *= can->blocksize ;
    
    pos.cleanup() ;
    x = ox ;
    y = oy ;
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
//    TAlignment *win = (TAlignment*) can->child ;
    
    wxColour nbgc ;
    for ( a = 0 ; can->seq[a] != this ; a++ ) ;
    switch ( id%3 )
        {
        case 0 : nbgc = wxColour ( 200 , 200 , 255 ) ; break ;
        case 1 : nbgc = wxColour ( 200 , 255 , 200 ) ; break ;
        case 2 : nbgc = wxColour ( 200 , 255 , 255 ) ; break ;
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
    TAlignment *al = (TAlignment*) can->child ;
    dc.SetTextForeground ( fg ) ;
    dc.SetTextBackground ( bg ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;
    
    int xa , xb , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    xa = -xa ;
    can->MyGetClientSize ( &xb , &yb ) ;
    yb += ya ;
    xb += xa ;
    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tzx = tx + can->charwidth ;
        int tzy = ty + can->charheight ;
        bool insight = true ; // Meaning "is this part visible"
        if ( tzy < ya || ty > yb ) insight = false ;
        if ( b > 0 && ( tzx < xa || tx > xb ) ) insight = false ;
        if ( can->getDrawAll() ) insight = true ;
        if ( !insight && ty > yb ) a = pos.p.size() ;
        if ( !insight && tx > xb ) a = pos.p.size() ;
        if ( b > 0 && !insight ) cnt++ ;
        if ( b > 0 && insight ) // Character
           {
           t = s[b-1] ;
           if ( pos.m[a] == 1 )
              {
              dc.SetTextBackground ( *wxLIGHT_GREY ) ;
              dc.SetTextForeground ( *wxBLACK ) ;
              }
           else if ( myname != txt("t_identity") )
              {
              fg = al->findColors ( s[b-1] , can->seq[first]->s[b-1] , true ) ;
              bg = al->findColors ( s[b-1] , can->seq[first]->s[b-1] , false ) ;
              dc.SetTextForeground ( fg ) ;
              dc.SetTextBackground ( bg ) ;
              }
           if ( can->isPrinting() && !can->getPrintToColor() )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }
           if ( al->mono )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }
           if ( myname != txt("t_identity") && 
                al->cons && 
                first != me &&
                t[0] == can->seq[first]->s[b-1] )
                   t[0] = '.' ;

           dc.DrawText ( t.c_str(), pos.r[a].x, pos.r[a].y ) ;
           if ( al->bold )
              {
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              dc.DrawText ( t.c_str(), pos.r[a].x-1, pos.r[a].y ) ;
              dc.SetBackgroundMode ( wxSOLID ) ;
              }

           if ( pos.m[a] > 0 )
              {
              dc.SetTextForeground ( fg ) ;
              dc.SetTextBackground ( bg ) ;
              }
           cnt++ ;
           }
        else if ( b < 0 && insight ) // Front number
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
    
