#include "SequenceCanvas.h"

//************************************************ SeqABI
    
SeqABI::~SeqABI ()
    {
    if ( at ) delete at ;
    }

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
    if ( can->isPrinting() )
       {
       scalex = w / 500 ;
       w = w * 9 / 10 ;
       }

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx ) ;
    itemsperline *= can->blocksize ;

    int id ;

    // Initiate Color coding
    int intensity = 200 ;
    colors[0] = wxColour ( 0 , 0 , 0 ) ;
    colors[1] = wxColour ( intensity , 0 , 0 ) ;
    colors[2] = wxColour ( 0 , intensity , 0 ) ;
    colors[3] = wxColour ( 0 , 0 , intensity ) ;
    /*
    if ( inv_compl )
        {
        wxColour dummy ;
        dummy = colors[0] ; colors[0] = colors[3] ; colors[3] = dummy ;
        dummy = colors[1] ; colors[1] = colors[2] ; colors[2] = dummy ;
        }
*/
    // Base order
    id = at->getRecord ( "FWO_" , 1 ) ;
    myass ( id != -1 , "SeqABI::arrange_1" ) ;
    
    // GATC
    int *ii = (int*) base2color ;
    *ii = at->vf[id].value ;
    
    // For painting
    minx = ox ;
    maxx = w - ox - wx ;
    int diffx = 0 ;
    id = at->getRecord ( "PLOC" , 2 ) ;
    myass ( id != -1 , "SeqABI::arrange_2" ) ;

    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    
    int id2 = at->getRecord ( "DATA" , 9 ) ;
    int bx1 = (char) at->vf[id2].data[(at->vf[id2].nrecords-1)*2] ;
    int bx2 = (unsigned char) at->vf[id2].data[(at->vf[id2].nrecords-1)*2+1] ;
    int lastx = at->vf[id2].nrecords ;
    
    
    for ( a = 0 ; a < s.length() ; a++ )
        {
        int idx = a ;
        if ( inv_compl ) idx = s.length() - a - 1 ;
        bx1 = (char) at->vf[id].data[idx*2] ;
        bx2 = (unsigned char) at->vf[id].data[idx*2+1] ;
        int bx = bx1 * 256 + bx2 ;
        
        if ( inv_compl ) bx = lastx - bx ;
        
        char t = s[a] ;
        x = bx * scalex - diffx ;


        if ( x > maxx )
           {
           pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
           lasta = pos.p.GetCount()+1 ;
           y += wy * ( can->seq.size() + can->blankline ) ;
           if ( a+1 < s.length() )
              pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
           diffx += maxx ;
           x = bx * scalex - diffx ;
           }

        pos.add ( a+1 , x + minx - wx / 2 , y , wx-1 , wy-1 ) ;
        lowy = y+wy ;

        }
    if ( lasta != pos.p.GetCount()+1 ) 
        pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
    
    lowy += wy * ( can->blankline ) ; // For sequence data
    
    return lowy + bo*2 ;
    }
    
void SeqABI::setInvCompl ( bool x )
    {
    if ( inv_compl != x ) // Inverting and complementing sequence
        {
        TVector v2 ;
        string s2 ;
        for ( int a = 0 ; a < s.length() ; a++ ) s2 = v2.getComplement( s[a] ) + s2 ;
        s = s2 ;
        can->child->vec->sequence = s ;
        }
    
    inv_compl = x ;
    }
    
void SeqABI::show ( wxDC& dc )
    {
    SeqDNA::show ( dc ) ;

    int data ;

    // Char-to-curve (the thin grey lines;-)
    int wx = can->charwidth ;
    int wy = can->charheight ;
    dc.SetPen(*wxLIGHT_GREY_PEN);
    for ( int a = 0 ; showHelpLines && a < pos.p.GetCount() ; a++ )
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
        if ( can->isPrinting() && !can->getPrintToColor() )
           dc.SetPen ( *wxBLACK_PEN ) ;
        else
           dc.SetPen ( *MYPEN ( colors[data] ) ) ;
        
        int a ;
        int id = at->getRecord ( "DATA" , 9 + data ) ;
        myass ( id > -1 , "SeqABI::show" ) ;
        int diffx = 0 ;
        int l = 0 ;
        wxPoint last ( minx , pos.l[l].GetHeight() + hy ) ;
        int x = 0 ;
        for ( a = 0 ; a < at->vf[id].nrecords ; a++ )
           {
           int idx = a*2 ;
           if ( inv_compl ) idx = ( at->vf[id].nrecords - a - 1 ) * 2 ;
           int b1 = (unsigned char) at->vf[id].data[idx] ;
           int b2 = (unsigned char) at->vf[id].data[idx+1] ;
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
    
void SeqABI::initFromFile ( wxString filename )
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
       {
       if ( b == base2color[a] )
          {
          if ( inv_compl ) return colors[a] ;
          else return colors[3-a] ;
          }
       }
    return wxColor ( "magenta" ) ;
    }
    
