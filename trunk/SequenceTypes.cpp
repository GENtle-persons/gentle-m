#include "SequenceCanvas.h"

//************************************************ SeqBasic

void SeqBasic::init ( SequenceCanvas *ncan )
    {
    offset = 0 ;
    takesMouseActions = false ;
    can = ncan ;
    endnumberlength = 0 ;
    }

void SeqBasic::logsize ()
    {
    mylog ( whatsthis() , wxString::Format ( "s %d" , s.length() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "p %d" , pos.p.GetCount() * sizeof ( int ) ) ) ;
    mylog ( whatsthis() , wxString::Format ( "m %d" , pos.m.length() ) ) ;
    mylog ( whatsthis() , wxString::Format ( "r %d" , pos.r.size() * sizeof ( wxRect ) ) ) ;
    mylog ( whatsthis() , wxString::Format ( "l %d" , pos.l.size() * sizeof ( wxRect ) ) ) ;
    mylog ( "---" , "" ) ;
    }    
    
void SeqBasic::editMode ( bool on )
	{
	if ( on )
		{
//		s += " " ;
		}
	else
 		{
 		if ( !s.IsEmpty() && s.GetChar ( s.length() - 1 ) == ' ' )
 			s = s.Left ( s.length() - 1 ) ;
   		}        
	}    

SeqBasic::~SeqBasic ()
    {
    // Farewell, my old faithful friend!
    }

//************************************************ SeqPrimer

bool SeqPrimer::useDirectRoutines () { return true ; }

int SeqPrimer::arrange_direct ( int n )
    {
//    arrange ( n ) ;
    SeqDNA::arrange_direct ( n ) ;
    }    

void SeqPrimer::show_direct ( wxDC& dc )
    {
//    show ( dc ) ;
    SeqDNA::show_direct ( dc ) ;
    }    

void SeqPrimer::show ( wxDC& dc )
    {
    if ( useDirectRoutines() ) show_direct ( dc ) ;
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    wxString t ;
    char u[100] ;
    dc.SetTextBackground ( *wxWHITE ) ;
    dc.SetTextForeground ( *wxBLACK ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;
    
    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
        {
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
           t = s.GetChar(b-1) ;
           int pm = getMark ( a ) ;
           if ( pm == 1 )
              {
              dc.SetTextBackground ( *wxLIGHT_GREY ) ;
              dc.SetTextForeground ( *wxBLACK ) ;
              }
           else if ( pm == 2 && can->doOverwrite() )
              {
              dc.SetTextBackground ( *wxBLACK ) ;
              dc.SetTextForeground ( *wxWHITE ) ;
              }
           if ( s.GetChar(b-1) == vec->getSequenceChar(b-1) ) dc.SetTextForeground ( *wxBLUE ) ;
           else dc.SetTextForeground ( *wxRED ) ;
           if ( can->isPrinting() && !can->getPrintToColor() )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           dc.DrawText ( t , pos.r[a].x, pos.r[a].y ) ;

           if ( pm == 2 && !can->doOverwrite() )
              {
                 int tx = pos.r[a].x , ty = pos.r[a].y ;
                 int tz = ty + can->charheight ;
                 dc.SetPen(*wxBLACK_PEN);
                 dc.DrawLine ( tx-1 , ty , tx-1 , tz ) ;
                 dc.DrawLine ( tx-3 , ty , tx+2 , ty ) ;
                 dc.DrawLine ( tx-3 , tz , tx+2 , tz ) ;
              }
           if ( pm > 0 )
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
           dc.DrawText ( t , pos.r[a].x, pos.r[a].y ) ;
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

void SeqPrimer::initFromTVector ( TVector *v )
    {
    vec = v ;
    s = vec->getSequence() ;
    for ( int a = 0 ; a < s.length() ; a++ ) s.SetChar(a,' ') ;
    takesMouseActions = true ;
    showNumbers = false ;
    }
    
void SeqPrimer::addPrimer ( TPrimer *p )
    {
    int a ;
    TVector d ;
    myass ( p , "SeqPrimer::addPrimer_0" ) ;
    myass ( vec , "SeqPrimer::addPrimer_1" ) ;
    d.setSequence ( s )  ;
    d.setCircular(vec->isCircular()) ;
    for ( a = p->from ; a <= p->to ; a++ )
        {
        myass ( a-p->from >= 0 , "SeqPrimer::addPrimer_2" ) ;
        myass ( a-p->from < p->sequence.length() , "SeqPrimer::addPrimer_3" ) ;
        d.setNucleotide ( a-1 , p->sequence.GetChar(a-p->from) ) ;
        }
    s = d.getSequence () ;
    }
    
//************************************************ SeqNum

int SeqNum::arrange ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    
    endnumberlength = 0 ;
    for ( a = 0 ; a < can->seq.GetCount() ; a++ )
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
        if ( (a+1) % can->blocksize == 0 )
           {
           blockstart = true ;
           x += wx-1 ;
           if ( x+wx*(can->blocksize+1) >= w )
              {
              pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
              lasta = pos.p.GetCount()+1 ;
              x = ox ;
              y += wy * ( can->seq.GetCount() + can->blankline ) ;
              }
           }
        }
    if ( lasta != pos.p.GetCount()+1 ) 
        pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
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
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
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
    int a , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
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
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    for ( a = 0 ; a < s.length() ; a++ )
        {
//        pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % can->blocksize == 0 )
           {
           x += wx-1 ;
           if ( x+wx*(can->blocksize+1) >= w )
              {
              pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
              lasta = pos.p.GetCount()+1 ;
              x = ox ;
              y += wy * ( can->seq.GetCount() + can->blankline ) ;
              if ( a+1 < s.length() )
                 pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
              }
           }
        }
    if ( lasta != pos.p.GetCount()+1 ) 
        pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
    return lowy + bo*2 ;
    }
    
void SeqDivider::show ( wxDC& dc )
    {
    int a , w , h ;
    can->MyGetSize ( &w , &h ) ;
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        int y = pos.r[a].y + can->charheight/2 ;
        dc.SetPen(*wxGREY_PEN);
        dc.DrawLine ( 0 , y-2 , w , y-2 ) ;
        dc.DrawLine ( 0 , y-1 , w , y-1 ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine ( 0 , y , w , y ) ;
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine ( 0 , y+1 , w , y+1 ) ;
        dc.DrawLine ( 0 , y+2 , w , y+2 ) ;
        }
    }

void SeqDivider::initFromTVector ( TVector *v )
    {
//    vec = v ;
    s = v->getSequence() ;
    }

