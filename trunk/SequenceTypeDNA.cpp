#include "SequenceCanvas.h"

//************************************************ SeqDNA

int SeqDNA::arrange ( int n )
    {
    if ( useDirectRoutines() ) return arrange_direct ( n ) ;
    int a , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    int ox = bo + wx + wx * endnumberlength , oy = n*wy+bo ;
/*
    int endnumber = offset + s.length() ;
    endnumberlength = 0 ;
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
*/    
    can->MyGetClientSize ( &w , &h ) ;

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx - 1 ) * can->blocksize ;
    bool memsave = ( whatsthis() == "FEATURE" ) ;
    
    pos.cleanup() ;
    pos.reserve ( s.length() * 11 / 10 , s.length() / itemsperline , memsave ) ;
    x = ox ;
    y = oy ;
    int icnt = 0 , pcnt = 0 ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 , memsave ) ; // Line number
    for ( a = 0 ; a < s.length() ; a++ )
        {
        icnt++ ;
        pos.add ( a+1 , x , y , wx-1 , wy-1 , memsave ) ;
        pcnt++ ;
        can->setLowX ( x + wx*2 ) ;
        lowy = y+wy ;
        x += wx ;
        if ( (a+1) % can->blocksize == 0 )
           {
           x += wx-1 ;
           if ( icnt == itemsperline )
              {
              icnt = 0 ;
              if ( !memsave ) pos.addline ( lasta , pcnt/*pos.p.GetCount()*/ , y , y+wy-1 ) ;
              lasta = pos.p.GetCount()+1 ;
              x = ox ;
              y += wy * ( can->seq.GetCount() + can->blankline ) ;
              if ( a+1 < s.length() )
                 {
                 pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 , memsave ) ; // Line number
                 pcnt++ ;
                 }    
              }
           }
        }
    if ( lasta != pos.p.GetCount()+1 && !memsave ) 
        pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
    return lowy + bo*2 ;
    }
    
wxPoint SeqDNA::showText ( int ystart , wxArrayString &tout )
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

void SeqDNA::show ( wxDC& dc )
    {
    if ( useDirectRoutines() ) { show_direct ( dc ) ; return ; }
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    wxString t ;
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
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        if ( can->hardstop > -1 && a > can->hardstop ) break ;
        b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tz = ty + can->charheight ;
        bool insight = true ; // Meaning "is this part visible"
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( can->getDrawAll() ) insight = true ;
        if ( !insight && ty > yb ) a = pos.p.GetCount() ;
        if ( b > 0 && !insight ) cnt++ ;
        if ( b > 0 && insight ) // Character
           {
           t = s.GetChar(b-1) ;
           int pm = getMark ( a ) ;
           if ( pm == 1 ) // Marked (light gray background)
              {
              dc.SetBackgroundMode ( wxSOLID ) ;
              dc.SetTextBackground ( *wxLIGHT_GREY ) ;
              dc.SetTextForeground ( *wxBLACK ) ;
              }
           else if ( pm == 2 && can->doOverwrite() ) // Overwrite cursor
              {
              dc.SetBackgroundMode ( wxSOLID ) ;
              dc.SetTextBackground ( *wxBLACK ) ;
              }
           if ( pm == 2 && can->doOverwrite() ) dc.SetTextForeground ( *wxWHITE ) ;
           else dc.SetTextForeground ( getBaseColor ( t.GetChar(0) ) ) ;
           if ( can->isPrinting() && pm == 1 )
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

           dc.DrawText ( t , tx , ty ) ;

           if ( pm == 2 && !can->doOverwrite() ) // Insert cursor
              {
                 dc.SetPen(*wxBLACK_PEN);
                 dc.DrawLine ( tx-1 , ty , tx-1 , tz ) ;
                 dc.DrawLine ( tx-3 , ty , tx+2 , ty ) ;
                 dc.DrawLine ( tx-3 , tz , tx+2 , tz ) ;
              }
           if ( pm > 0 ) // Reverting cursor settings
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
           dc.DrawText ( t , pos.r[a].x, pos.r[a].y ) ;
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
    s = vec->getSequence() ;
    takesMouseActions = true ;
    showNumbers = true ;
    fontColor.Set ( 0 , 0 , 0 ) ;
    if ( invers )
       {
       int a ;
       for ( a = 0 ; a < s.length() ; a++ )
          s.SetChar ( a , vec->getComplement ( s.GetChar(a) ) ) ;
       takesMouseActions = false ;
       showNumbers = false ;
       alternateName = "" ;
       fontColor.Set ( 100 , 100 , 100 ) ;
       }
    }

// direct


bool SeqDNA::useDirectRoutines ()
    {
    return true ;
    }


int SeqDNA::arrange_direct ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    
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

void SeqDNA::show_direct ( wxDC& dc )
    {
    myass ( itemsperline , "DNA:show_direct_ipl" ) ;
    if ( !itemsperline ) return ;
    myass ( can , "SeqDNA::show_direct1" ) ;
    can->SetFont(*can->font);
    dc.SetFont(*can->font);
    int a , b , w , h , n , bo = can->border ;
    int csgc = can->NumberOfLines() , cbs = can->blocksize ;
    int cih = can->isHorizontal() ;
    int xa , xb , ya , yb ;
    for ( n = 0 ; n < can->seq.GetCount() && can->seq[n] != this ; n++ ) ;
    if ( n == can->seq.GetCount() ) return ;
    
    // Setting basic values
    int cw = can->charwidth , ch = can->charheight ;
    int ox = bo + cw + cw * endnumberlength ;
    int oy = n*ch+bo ;
    bool isPrimer = false ;
    if ( whatsthis().StartsWith ( "PRIMER" ) ) isPrimer = true ;
    
    can->MyGetClientSize ( &w , &h ) ;
    xb = w ;
    yb = h ;

    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    dc.SetTextForeground ( fontColor ) ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    xa = -xa ;
    xb += xa ;
    ya = -ya ;
    yb += ya ;
    
    myass ( ch , "SeqDNA::show_direct2a" ) ;
    myass ( csgc , "SeqDNA::show_direct2b" ) ;
    b = ( ya - ch - oy ) / ( ch * csgc ) * itemsperline ;
    for ( a = 0 ; a < b && a < s.length() ; a += itemsperline ) ;
        
    myass ( itemsperline , "SeqDNA::show_direct3" ) ;
    myass ( cbs , "SeqDNA::show_direct4" ) ;
    for ( a = 0 ; a < s.length() ; a++ )
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
        char ac = s.GetChar(a) ;
        if ( pm == 0 && !showNumber && ac == ' ' ) continue ;
        
        if ( pm == 1 ) // Marked (light gray background)
           {
           dc.SetBackgroundMode ( wxSOLID ) ;
           dc.SetTextBackground ( *wxLIGHT_GREY ) ;
           dc.SetTextForeground ( *wxBLACK ) ;
           }
        else if ( pm == 2 && can->doOverwrite() ) // Overwrite cursor
           {
           dc.SetBackgroundMode ( wxSOLID ) ;
           dc.SetTextBackground ( *wxBLACK ) ;
           dc.SetTextForeground ( *wxWHITE ) ;
           }
        else dc.SetTextForeground ( getBaseColor ( ac ) ) ;
        
        if ( isPrimer )
           {
           if ( s.GetChar(a) == vec->getSequenceChar(a) ) dc.SetTextForeground ( *wxBLUE ) ;
           else dc.SetTextForeground ( *wxRED ) ;
           }    
        
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

        dc.DrawText ( wxString ( ac ) , px , py ) ;
        
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
           dc.SetTextForeground ( fontColor ) ;
           }

        // Methylation
        if ( !invers && alternateName.IsEmpty() && wxNOT_FOUND != vec->methyl.Index ( a ) )
        	{
            dc.SetPen(*wxRED_PEN);
            dc.DrawLine ( px , py + ch - 2 , px + cw , py + ch - 2 ) ;
            dc.SetPen(*wxTRANSPARENT_PEN);
        	}    

        if ( showNumber )
           {
              mylog ( "SeqDNA::show_direct" , "A" ) ;
           wxString t ;
           if ( showNumbers )
              {
              mylog ( "SeqDNA::show_direct" , "B" ) ;
              t = wxString::Format ( "%d" , a + 1 ) ;
              int padd = endnumberlength - t.length() ;
              mylog ( "SeqDNA::show_direct" , wxString::Format ( "C: %d, %d" , endnumberlength , padd ) ) ;
              if ( padd > 0 && padd < 20 ) t.Pad ( padd , '0' , false ) ;
              mylog ( "SeqDNA::show_direct" , "D" ) ;
              }    
           else
              {
              mylog ( "SeqDNA::show_direct" , "B2" ) ;
              if ( isPrimer ) dc.SetTextForeground ( *wxBLUE ) ;
              else dc.SetTextForeground ( *wxBLACK ) ;
              mylog ( "SeqDNA::show_direct" , "C2" ) ;
              t = alternateName ;
              mylog ( "SeqDNA::show_direct" , "D2" ) ;
              }    
           dc.DrawText ( t , bo , py ) ;
              mylog ( "SeqDNA::show_direct" , "E" ) ;
           }    
        }    


    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }    

void SeqDNA::makeEndnumberLength()
    {
    int wx = can->charwidth ;
    int ox = wx + can->border ;
    int endnumber = offset + s.length() ;
    endnumberlength = 0 ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }
    
    if ( whatsthis() == "FEATURE" && 
         can->child && 
         can->child->def == "alignment" )
        {
        endnumberlength = can->maxendnumberlength ;
        }        
    }
        
        
int SeqDNA::getLine ( int y )
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

int SeqDNA::getItem ( wxPoint pt , int line )
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

int SeqDNA::getMarkSize ()    
    {
    if ( !useDirectRoutines() ) return SeqBasic::getMarkSize() ;
    return s.length()+1 ;
    }
    
int SeqDNA::getRectSize ()
    {
    if ( !useDirectRoutines() ) return SeqBasic::getRectSize() ;
    return 0 ;
    }
    
wxRect SeqDNA::getRect ( int i )
    {
    if ( !useDirectRoutines() ) return pos.r[i] ;
    
    wxRect ret ;
    ret.x = 0 ;
    ret.y = ( (i-1) / itemsperline ) * can->charheight * can->NumberOfLines() ;
    
    return ret ;
    }

int SeqDNA::getMark ( int i )
    {
    if ( !useDirectRoutines() ) return SeqBasic::getMark ( i ) ;
    if ( i >= s.length() ) return 0 ;
    return pos.getmark ( i ) ;
    }

void SeqDNA::setMark ( int i , int v )
    {
    if ( !useDirectRoutines() ) { SeqBasic::setMark ( i , v ) ; return ; }
    if ( i <= 0 ) return ;
    pos.mark ( i-1 , v ) ;
    }

int SeqDNA::getPos ( int i )
    {
    if ( !useDirectRoutines() ) return pos.p[i] ;
    return i ;
    }

void SeqDNA::setPos ( int i , int v )
    {
    if ( !useDirectRoutines() ) pos.p[i] = v ;
    }

