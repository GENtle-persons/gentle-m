#include "SequenceCanvas.h"

//************************************************ SeqRestriction

int SeqRestriction::arrange ( int n )
    {
    unsigned int a , b ;
    int x , y , w , h , l = 0 , bo = 4 , lowy = 0 ;
    int lasta = 0 , cut , thepos ;
    wxString t = "" ;
    
    vs.Clear() ;
    ven.Clear() ;
    eoe.Clear() ;
    while ( t.length() < s.length() ) t += " " ;
    s = t ;
    for ( cut = 0 ; cut < vec->rc.size() ; cut++ )
        {
        TRestrictionEnzyme *e = vec->rc[cut].e ;
        if ( vec->isEnzymeHidden ( e->name ) ) continue ;
        vs.Add ( t ) ;
        eoe.Add ( -1 ) ;
        ven.Add ( e->name ) ;
        
        int vsl = vs.GetCount() - 1 ;
        
        for ( a = 0 ; a < e->sequence.length() ; a++ )
           {
           thepos = a + vec->rc[cut].pos - e->cut ;
           if ( thepos >= 0 && thepos < t.length() )
              vs[vsl][(uint)thepos] = '-' ;
           }
           
        // Arranging enzyme text
        eoe[vsl] = vec->rc[cut].pos - e->cut + e->sequence.length() ;
           
        thepos = vec->rc[cut].pos ;
        if ( thepos-1 >= 0 && thepos < t.length() )
           {
           vs[vsl][(uint)thepos] = '|' ;
           vs[vsl][(uint)vec->rc[cut].pos-1] = '#' ;
           }
        }
    
    // Merging all cuts
    for ( a = 0 ; a < s.length() ; a++ )
        {
        s[a] = ' ' ;
        for ( cut = 0 ; cut < vs.GetCount() ; cut++ )
           if ( s[a] == ' ' )
              s[a] = vs[cut][a] ;
        }
        
    // Generating Y offsets
    wxArrayString vt = vs ;

    // An yoff for each vs (and each vt)
    yoff.Clear() ;
    while ( yoff.GetCount() < vs.GetCount() ) yoff.Add ( yoff.GetCount() ) ;
    
    // Trying to merge several lines together
    for ( cut = 1 ; cut < vt.GetCount() ; cut++ )
        {
        x = -1 ;
        for ( a = 0 ; x == -1 && a < vt.GetCount() ; a++ )
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
    for ( int layer = 0 ; layer < vs.GetCount() ; layer++ )
        {
        x = ox ;
        y = oy ;
        for ( a = 0 ; a < s.length() ; a++ )
            {
            int xoff = 0 ;
            if ( (a+1) % can->blocksize == 0 ) xoff = wx/3 ;
            if ( vs[layer][a] != ' ' ) pos.add ( a+1 , x , y , wx-1+xoff , wy-1 ) ;
            if ( y+wy > lowy ) lowy = y+wy ;
            x += wx ;
            if ( (a+1) % can->blocksize == 0 )
               {
               x += wx ;
               if ( x+wx*(can->blocksize+1) >= w )
                  {
                  pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
                  lasta = pos.p.GetCount()+1 ;
                  x = ox ;
                  y += wy * ( can->seq.size() + can->blankline ) ;
                  }
               }
            }
        if ( lasta != pos.p.GetCount()+1 ) 
            pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
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
    for ( int layer = 0 ; layer < vs.GetCount() ; layer++ )
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
            case 1 : dc.SetPen(*MYPEN(wxColour(0,200,0))); 
                     dc.SetTextForeground ( wxColour ( 0 , 200 , 0 ) ) ; 
                     break ;
            case 2 : dc.SetPen(*MYPEN(wxColour(0,0,200))); 
                     dc.SetTextForeground ( wxColour ( 0 , 0 , 200 ) ) ; 
                     break ;
            }
        if ( can->isPrinting() && !can->getPrintToColor() )
           {
           dc.SetPen(*wxBLACK_PEN); 
           dc.SetTextForeground ( *wxBLACK ) ; 
           }
        int qlx = -1 ;
        for ( a = 0 ; a < pos.p.GetCount() ; a++ )
            {
            if ( can->hardstop > -1 && a > can->hardstop ) break ;
            b = pos.p[a] ;
            char c = vs[layer][(uint)b-1] ;
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
            if ( !can->getDrawAll() && pos.r[a].GetTop() > yb ) a = pos.p.GetCount() ;
            }
        }
    dc.SetTextForeground ( wxColor ( *wxBLACK ) ) ;
    }
    
void SeqRestriction::initFromTVector ( TVector *v )
    {
    vec = v ;
    s = vec->getWxSequence() ;
    down = false ;
    }
    
