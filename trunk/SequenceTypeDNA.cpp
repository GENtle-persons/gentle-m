#include "SequenceCanvas.h"

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
    
    if ( whatsthis() == "FEATURE" && 
         can->child && 
         can->child->def == "alignment" )
        {
        endnumberlength = can->maxendnumberlength ;
        ox = bo + wx + wx * endnumberlength ;
        }
    
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

void SeqDNA::show ( wxDC& dc )
    {
    dc.SetFont(*can->font);
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
        if ( can->hardstop > -1 && a > can->hardstop ) break ;
        b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tz = ty + can->charheight ;
        bool insight = true ; // Meaning "is this part visible"
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( can->getDrawAll() ) insight = true ;
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
           if ( can->isPrinting() && pos.m[a] == 1 )
              {
              dc.SetBrush ( *MYBRUSH ( wxColour ( 230 , 230 , 230 ) ) ) ;
              dc.SetPen(*wxTRANSPARENT_PEN);
              dc.DrawRectangle ( tx , ty , can->charwidth , can->charheight ) ;
              }
           if ( can->isPrinting() && !can->getPrintToColor() )
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
