/** \file
	\brief Contains the SeqAlign class members
*/
#include "SequenceCanvas.h"

int SeqAlign::arrange ( int n )
    {
    int a , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    int ox = bo + wx + wx * endnumberlength  , oy = n*wy+bo ;
    can->MyGetClientSize ( &w , &h ) ;

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx ) ;
    itemsperline *= can->blocksize ;
    
    pos.cleanup() ;
    pos.reserve ( s.length() * 11 / 10 , s.length() / itemsperline ) ;
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
    pos.r.clear () ;
    return lowy + bo*2 ;
    }
    
void SeqAlign::show ( wxDC& dc )
    {
    can->SetFont(*can->font);
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    wxString t ;
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
    for ( a = 0 ; a < can->seq.GetCount() ; a++ )
        {
        if ( can->seq[a]->whatsthis() == _T("ALIGN") && first == -1 )
           first = a ;
        if ( can->seq[a] == this )
           me = a ;
        }
    wxColour fg = *wxBLACK , bg = *wxWHITE ;
    TAlignment *al = (TAlignment*) can->child ;
    dc.SetTextForeground ( fg ) ;
    dc.SetTextBackground ( bg ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;

    int n , bo = can->border ;
    for ( n = 0 ; can->seq[n] != this ; n++ ) ;
    int wx = can->charwidth , wy = can->charheight ;
    int ox = bo + wx + wx * endnumberlength  , oy = n*wy+bo ;
    
    int xa , xb , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    xa = -xa ;
    can->MyGetClientSize ( &xb , &yb ) ;
    yb += ya ;
    xb += xa ;
    int cnol = can->NumberOfLines() ;
    int ppgc = pos.p.GetCount() ;
    bool thisisidentity = ( myname == txt("t_identity") ) ;
    int last_wx = -1 ;

    for ( a = 0 ; a < ppgc ; a++ )
        {
        if ( can->hardstop > -1 && a > can->hardstop ) break ;
        int rax , ray = oy + wy * cnol * ( a / ( itemsperline + 1 ) ) ;
        if ( ray < ya ) continue ;
        b = pos.p[a] ;
        if ( b < 0 ) rax = bo ;
        else 
           {
           int x2 = ( b - 1 ) % itemsperline ;
           rax = ox ;
           rax += x2 * wx ;
           rax += ( x2 / can->blocksize ) * ( wx - 1 ) ;
           }    
        if ( can->isPrinting() ) rax += xa ; // PATCH AS PATCH CAN!!!

        int tx = rax , ty = ray ;
        int tzx = tx + wx ;
        int tzy = ty + wy ;
        bool insight = true ; // Meaning "is this part visible"
        if ( tzy < ya || ty > yb ) insight = false ;
        if ( b > 0 && ( tzx < xa || tx > xb ) ) insight = false ;
        if ( can->getDrawAll() ) insight = true ;
        if ( !insight && ty > yb ) a = ppgc ;
        if ( !insight && tx > xb ) a = ppgc ;
        if ( b > 0 && !insight ) cnt++ ;
        if ( b > 0 && insight ) // Character
           {
           t = s.GetChar(b-1) ;

           // Same as char in first sequence?
           if ( (!thisisidentity) &&
                al->cons &&
                first != me &&
                t.GetChar(0) != '-' &&
                t.GetChar(0) == can->seq[first]->s.GetChar(b-1) )
                   t.SetChar(0,'.') ;

           SequenceCharMarkup scm ;
           int mode = 0 ;
           if ( !thisisidentity )
              {
              al->getCharMarkup ( scm , id , b-1 , first ) ;
              if ( getMark ( a ) ) mode |= SEQUENCECHARMARKUP_MARK ;
              if ( al->mono ) mode |= SEQUENCECHARMARKUP_MONO ;
              if ( can->isPrinting() && !can->getPrintToColor() ) mode |= SEQUENCECHARMARKUP_MONO ;
              if ( al->bold ) mode |= SEQUENCECHARMARKUP_BOLD ;
              }

           scm.draw ( dc , wxRect ( rax , ray , wx , wy ) , t , mode , last_wx ) ;
           last_wx = rax + wx ;

/*
           // Get color for this char
           if ( !thisisidentity )
              {
              fg = al->findColors ( s.GetChar(b-1) , can->seq[first]->s.GetChar(b-1) , true ) ;
              bg = al->findColors ( s.GetChar(b-1) , can->seq[first]->s.GetChar(b-1) , false ) ;
              dc.SetTextForeground ( fg ) ;
              dc.SetTextBackground ( bg ) ;
              }

           // Mark?
          int pm = getMark ( a ) ;
          if ( pm == 1 ) // Marked (light gray background)
             {
             dc.SetBackgroundMode ( wxSOLID ) ;
             dc.SetTextBackground ( *wxLIGHT_GREY ) ;
             }

           // Printing to B&W printer?
           if ( can->isPrinting() && !can->getPrintToColor() )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           // Monochrome display?
           if ( al->mono )
              {
              dc.SetTextForeground ( *wxBLACK ) ;
              if ( pm != 1 ) dc.SetBackgroundMode ( wxTRANSPARENT ) ;
              }

           // Same as char in first sequence?
           if ( (!thisisidentity) && 
                al->cons && 
                first != me &&
                t.GetChar(0) != '-' &&
                t.GetChar(0) == can->seq[first]->s.GetChar(b-1) )
                   t.SetChar(0,'.') ;

           if ( t == _T(".") ) // Display round dot to show conservation
              {
              dc.SetPen(*wxTRANSPARENT_PEN);
              if ( al->invs )
                 {
                 dc.SetBrush ( *MYBRUSH ( dc.GetTextBackground() ) ) ;
                 dc.DrawRectangle ( rax-1 , ray , wx+2 , wy ) ;
                 }
              else if ( pm == 1 ) // Marked
                 {
                 dc.SetBrush ( *MYBRUSH ( *wxLIGHT_GREY ) ) ;
                 dc.DrawRectangle ( rax-1 , ray , wx+2 , wy ) ;
                 }
              dc.SetBrush ( *MYBRUSH ( dc.GetTextForeground() ) ) ;
              dc.DrawCircle ( rax + wx / 2 ,
                              ray + wy / 2 ,
                              wy / (al->bold?6:8) ) ;
              }
           else // Display character
              {
              dc.DrawText ( t , rax, ray ) ;       
              if ( al->bold )
                 {
                 dc.SetBackgroundMode ( wxTRANSPARENT ) ;
                 dc.DrawText ( t, rax-1, ray ) ;
                 dc.SetBackgroundMode ( wxSOLID ) ;
                 }
              }    
*/
           cnt++ ;
           }
        else if ( b < 0 && insight ) // Front number
           {
           last_wx = -1 ;
           dc.SetTextForeground ( *wxBLACK ) ;
           dc.SetTextBackground ( nbgc ) ;
           FILLSTRING ( t , (wxChar)' ' , endnumberlength ) ;
           dc.DrawText ( t , rax, ray ) ;
           dc.SetFont(*can->varFont);
           t = myname ;
           t = _T(" ") + t ;
           int tw , th ;
           do {
              t = t.substr ( 1 , t.length()-1 ) ;
              dc.GetTextExtent ( t , &tw , &th ) ;
              } while ( tw > endnumberlength * wx ) ;
           dc.DrawText ( t , rax, ray ) ;
           dc.SetTextBackground ( *wxWHITE ) ;
           dc.SetFont(*can->font);
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }
    

void SeqAlign::makeEndnumberLength()
    {
    endnumberlength = myname.length() ;
    while ( endnumberlength > 10 ) endnumberlength-- ;
    }
        
