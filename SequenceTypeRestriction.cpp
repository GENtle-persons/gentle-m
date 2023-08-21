/** \file
	\brief Contains the SeqRestriction class members
*/
#include "SequenceCanvas.h"

int SeqRestriction::arrange ( int n )
    {
    unsigned int a ;
    int x , y , w , h , bo = can->border , lowy = 0 ;
    int lasta = 0 ;
    wxString t ;

    pl.slen = vec->getSequenceLength() + ( ( can && can->getEditMode() ) ? -1 : 0 ) ;
    vec->sortRestrictionSites() ;
    pl.prepare ( vec->rc.size() ) ;    
    for ( a = 0 ; a < vec->rc.size() ; a++ )
        {
		int from = vec->rc[a].getFrom() ;
		int to = vec->rc[a].getTo() ;
//        int from = vec->rc[a].getPos() - vec->rc[a].e->getCut() ;
//        int to = from + vec->rc[a].e->getSequence().length() - 1 ;
        if ( !vec->isEnzymeHidden ( vec->rc[a].e->getName() ) )
           pl.add ( a , from , to ) ;
        }    
    pl.makeLevels () ;

    // Setting basic values
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + pl.slen ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    
    can->MyGetClientSize ( &w , &h ) ;

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx - 1 ) ;
    if ( itemsperline == 0 ) itemsperline = 1 ;
	itemsperline *= can->blocksize ;

    pos.cleanup() ;
    bool direct = useDirectRoutines() ;
    if ( !direct ) pos.reserve ( pl.slen , 0 , true ) ;
    x = ox ;
    y = oy ;

    int icnt = 0 , rcnt = 0 ;
    for ( a = 0 ; a < pl.slen ; a++ )
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
              lasta = rcnt + 1 ;
              x = ox ;
              y += wy * ( can->seq.GetCount() + can->blankline ) ;
              }
           }
        }

    return lowy + bo*2 ;
    }
    
void SeqRestriction::show ( wxDC& dc )
    { 
	myass ( can , "SeqRestriction::show1" ) ;
	myass ( vec , "SeqRestriction::show2" ) ;
	mylog ( "SeqRestriction::show" , "1" ) ;
    int ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( _T("A") , &ch2 , &ch2 ) ;
    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    int n , csgc = can->NumberOfLines() , cbs = can->blocksize , bo = can->border ;
	mylog ( "SeqRestriction::show" , "2" ) ;
    for ( n = 0 ; n < csgc && can->seq[n] != this ; n++ ) // n is used below
	;
	mylog ( "SeqRestriction::show" , "3" ) ;
    int cw = can->charwidth , ch = can->charheight ;
    int ox = bo + cw + cw * endnumberlength ;
    int oy = n*ch+bo ;
    bool direct = useDirectRoutines() ;
        mylog ( "SeqRestriction::show" , "4" ) ;
    for ( int level = 0 ; level < pl.maxlevels ; level++ )
        {
        mylog ( "SeqRestriction::show" , wxString::Format ( "Round %d of %d" , level , pl.maxlevels ) ) ;
        int a , b ;
        char lc = ' ' ;
        int ly = -1 ;
        int yo = (level*2) * 2 - 6 ;
        if ( down ) yo = can->charheight / 2 - ( (level*2) * 2 ) ;
        int llx = 0 ;
        if ( !vec->getEnzymeRule()->use_color_coding )
        	{    
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
            }
        mylog ( "SeqRestriction::show" , "A" ) ;
        if ( can->isPrinting() && !can->getPrintToColor() )
           {
           dc.SetPen(*wxBLACK_PEN); 
           dc.SetTextForeground ( *wxBLACK ) ; 
           }
        int qlx = -1 , idx = -1;
        wxRect ra , rb ;
        mylog ( "SeqRestriction::show" , "B" ) ;
        for ( a = 0 ; a < vec->getSequenceLength() ; a++ )
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
                    
            
            int ty = ra.y ;
            int tz = ty + can->charheight ;
            
            bool insight = true ; // Meaning "is this part visible"
            if ( tz < ya ) insight = false ;
            if ( ty > yb ) insight = false ;
            if ( can->getDrawAll() ) insight = true ;

            if ( insight ) idx = pl.here ( b-1 , level ) ;
            if ( idx != -1 && insight )
               {
               c = '-' ;
               if ( pl.getID(idx) >= vec->rc.size() ) continue ;
               TRestrictionCut *rc = &(vec->rc[pl.getID(idx)]) ;

               if ( vec->getEnzymeRule()->use_color_coding && !(can->isPrinting() && !can->getPrintToColor()) )
                  {
            	  wxColour *col = vec->getEnzymeRule()->getColor ( vec->countCuts ( rc->e->getName() ) ) ;
            	  dc.SetPen(*MYPEN(*col)); 
            	  dc.SetTextForeground ( *col ) ;
              	  }

               if ( rc->getPos() == b-1 ) c = '|' ;
               else if ( rc->getPos() == b ) c = '#' ;
               
               char c2 = ' ' ;
               int ol = rc->getPos() + rc->getOverlap() ;
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
                  wxString t = rc->getDisplayName() ;
                  if ( down ) dc.DrawText ( t , llx , ly - ch2 ) ;
                  else dc.DrawText ( t , llx , ly ) ;
                  }
               }
            rb = ra ;
            lc = c ;
            if ( !can->getDrawAll() && ra.y > yb ) a = vec->getSequenceLength() ;
            }
        mylog ( "SeqRestriction::show" , "C" ) ;
        }
    dc.SetTextForeground ( wxColor ( *wxBLACK ) ) ;
	mylog ( "SeqRestriction::show" , "fin" ) ;
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
        
