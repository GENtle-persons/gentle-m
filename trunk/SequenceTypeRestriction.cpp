#include "SequenceCanvas.h"


int SequencePartList::getID ( int internalID ) { return vi[internalID] ; }
int SequencePartList::getFrom ( int internalID ) { return vx[internalID] ; }
int SequencePartList::getTo ( int internalID ) { return vy[internalID] ; }

void SequencePartList::prepare ( int size )
    {
    vi.Clear () ;
    vx.Clear () ;
    vy.Clear () ;
    vl.Clear () ;
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
        wxString t ( ' ' , maxlevels + 2 ) ;
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
    
void SequencePartList::here ( int pos , wxArrayInt &ret )
    {
    ret.Clear () ;
    for ( int a = 0 ; a < vi.GetCount() ; a++ )
        {
        if ( pos >= vx[a] && pos <= vy[a] ) ret.Add ( a ) ;
        else if ( vy[a] >= slen && pos <= vy[a] - slen ) ret.Add ( a ) ;
        }    
    }    

int SequencePartList::here ( int pos , int level )
    {
    for ( int a = 0 ; a < vl2[level].GetCount() ; a++ )
        {
        if ( pos >= vx[vl2[level][a]] && pos <= vy[vl2[level][a]] ) return vl2[level][a] ;
        if ( vy[a] < vx[a] )
           {
           if ( pos >= vx[a] || pos <= vy[a] ) return vl2[level][a] ;
           }    
        else if ( vy[a] >= slen )
           {
           if ( pos >= vx[a] || pos <= vy[a] - slen ) return vl2[level][a] ;
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

//************************************************ SeqRestriction

int SeqRestriction::arrange ( int n )
    {
    unsigned int a , b ;
    int x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 , cut , thepos ;
    wxString t ;
    
    pl.slen = vec->getSequenceLength() ;
    pl.prepare ( vec->rc.size() ) ;    
    for ( a = 0 ; a < vec->rc.size() ; a++ )
        {
        int from = vec->rc[a].pos - vec->rc[a].e->cut ;
        int to = from + vec->rc[a].e->sequence.length() - 1 ;
        if ( !vec->isEnzymeHidden ( vec->rc[a].e->name ) )
           pl.add ( a , from , to ) ;
        }    
    pl.makeLevels () ;

    // Setting basic values
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + vec->getSequenceLength() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    
    can->MyGetClientSize ( &w , &h ) ;

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx - 1 ) * can->blocksize ;

    pos.cleanup() ;
    bool direct = useDirectRoutines() ;
    if ( !direct ) pos.reserve ( vec->getSequenceLength() , 0 , true ) ;
    x = ox ;
    y = oy ;

    int icnt = 0 , rcnt = 0 ;
    for ( a = 0 ; a < vec->getSequenceLength() ; a++ )
        {
        icnt++ ;
        rcnt++ ;
        if ( !direct ) pos.add ( a+1 , x , y , wx-1 , wy-1 , true ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % can->blocksize == 0 )
           {
           x += wx-1 ;
           if ( icnt == itemsperline )
              {
              icnt = 0 ;
              lasta = rcnt + 1 ; //pos.r.size()+1 ;
              x = ox ;
              y += wy * ( can->seq.GetCount() + can->blankline ) ;
              }
           }
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
    int n , csgc = can->NumberOfLines() , cbs = can->blocksize , bo = can->border ;
    for ( n = 0 ; n < csgc && can->seq[n] != this ; n++ ) ;
    int cw = can->charwidth , ch = can->charheight ;
    int ox = bo + cw + cw * endnumberlength ;
    int oy = n*ch+bo ;
    bool direct = useDirectRoutines() ;
    for ( int level = 0 ; level < pl.maxlevels ; level++ )
        {
        int a , b , cut ;
        char u[100] ;
        char lc = ' ' ;
        int ly = -1 ;
        int yo = (level*2) * 2 - 6 ;
        if ( down ) yo = can->charheight / 2 - ( (level*2) * 2 ) ;
        int llx = 0 ;
        switch ( (level*2)%3 )
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
        int qlx = -1 , idx ;
        wxRect ra , rb ;
        for ( a = 0 ; a < vec->getSequenceLength() /*pos.p.GetCount()*/ ; a++ )
            {
            if ( can->hardstop > -1 && a > can->hardstop ) break ;
            char c = ' ' ;
            b = a + 1 ;
            
            if ( direct )
               {
               int px = a % itemsperline , py = a / itemsperline ;
               px = px * cw + ( px / cbs ) * ( cw - 1 ) + ox ;
               py = py * ch * csgc + oy ;
               ra = wxRect ( px , py , cw , ch ) ;
               }    
            else ra = getRect ( a ) ;
                    
            
            int tx = ra.x , ty = ra.y ;
            int tz = ty + can->charheight ;
            
            bool insight = true ; // Meaning "is this part visible"
            if ( tz < ya ) insight = false ;
            if ( ty > yb ) insight = false ;
            if ( can->getDrawAll() ) insight = true ;
            
            if ( insight ) idx = pl.here ( b-1 , level ) ;
            if ( idx != -1 && insight )
               {
               c = '-' ;
               TRestrictionCut *rc = &(vec->rc[pl.getID(idx)]) ;

               if ( rc->pos == b-1 ) c = '|' ;
               else if ( rc->pos == b ) c = '#' ;
               
               char c2 = ' ' ;
               int ol = rc->pos + rc->e->overlap ;
               if ( b-1 == ol ) c2 = '|' ; 
               else if ( b == ol ) c2 = '#' ; 
                              
               if ( qlx == -1 ) qlx = ra.x ;
               int lx = ra.x ;
               int x = ( lx + ra.GetRight() ) / 2 ;
               int bt = ra.y + yo ;
               if ( down ) bt = ( ra.y + ra.height ) + yo ;
               int y = ( ra.y + ( ra.y + ra.height ) ) / 2 + yo ;
               if ( lc != ' ' && ly == y ) lx = rb.GetRight() ;
               
               if ( c == '-' )
                  {
                  if ( c2 == ' ' ) dc.DrawLine ( lx , y , ra.GetRight() , y ) ;
                  }
               else if ( c == '#' )
                  {
                  if ( lx < x ) dc.DrawLine ( lx , y , x , y ) ;
                  dc.DrawLine ( x , y , ra.GetRight() , bt ) ;
                  }
               else if ( c == '|' )
                  {
                  if ( lx < x ) dc.DrawLine ( lx , bt , x , y ) ;
                  if ( c2 != '#' ) dc.DrawLine ( x , y , ra.GetRight() , y ) ;
                  }

               if ( c2 == '#' )
                  {
                  if ( lx < x && lc != '#' ) dc.DrawLine ( lx , y , x , y ) ;
                  dc.DrawLine ( x , y , ra.GetRight() , y + ( y - bt ) ) ;
                  }
               else if ( c2 == '|' )
                  {
                  if ( lx < x ) dc.DrawLine ( lx , y + ( y - bt ) , x , y ) ;
                  dc.DrawLine ( x , y , ra.GetRight() , y ) ;
                  }
                  
                  
               llx = ra.GetRight() ;
               ly = y ;
               qlx = x ;
               if ( b == pl.getTo ( idx ) )
                  {
                  if ( down ) dc.DrawText ( rc->e->name , llx , ly - ch2 ) ;
                  else dc.DrawText ( rc->e->name , llx , ly ) ;
                  }
               }
            rb = ra ;
            lc = c ;
            if ( !can->getDrawAll() && ra.y > yb ) a = vec->getSequenceLength() ;//pos.p.GetCount() ;
            }
        }
    dc.SetTextForeground ( wxColor ( *wxBLACK ) ) ;
    }
    
void SeqRestriction::initFromTVector ( TVector *v )
    {
    vec = v ;
//    s = vec->getSequence() ;
    down = false ;
    }

bool SeqRestriction::useDirectRoutines ()
    {
    return true ;
    }
        
