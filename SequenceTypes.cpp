/** \file
    \brief Contains the SeqBasic, SeqPrimer, SeqNum, and SeqDivider class members
*/
#include "SequenceCanvas.h"

//************************************************ SeqBasic

void SeqBasic::init ( SequenceCanvas * const ncan )
    {
    offset = 0 ;
    takesMouseActions = false ;
    can = ncan ;
    endnumberlength = 0 ;
    force_add_line_number = 0 ;
    }

void SeqBasic::logsize () const
    {
    mylog ( whatsthis() , wxString::Format ( _T("s %d") , s.length() ) ) ;
    mylog ( whatsthis() , wxString::Format ( _T("p %d") , pos.p.GetCount() * sizeof ( int ) ) ) ;
    mylog ( whatsthis() , wxString::Format ( _T("m %d") , pos.m.length() ) ) ;
    mylog ( whatsthis() , wxString::Format ( _T("r %d") , pos.r.size() * sizeof ( wxRect ) ) ) ;
    mylog ( whatsthis() , wxString::Format ( _T("l %d") , pos.l.size() * sizeof ( wxRect ) ) ) ;
    mylog ( "---" , "" ) ;
    }

void SeqBasic::editMode ( const bool on )
    {
    if ( on )
        {
//      s += " " ;
        }
    else
        {
        if ( !whatsthis().StartsWith ( _T("PRIMER") ) && !s.IsEmpty() && s.GetChar ( s.length() - 1 ) == ' ' )
             s = s.Left ( s.length() - 1 ) ;
        }
    }

void SeqBasic::addHighlight ( const int from , const int t , const wxColour& c )
    {
    int to (t) ;
    if ( can && can->child )
        {
        if ( to < from ) to += can->child->vec->getSequenceLength() ;
        }
    highlight_begin.Add ( from ) ;
    highlight_end.Add ( to ) ;
    highlight_color.push_back ( c ) ;
    }

wxColour SeqBasic::getHighlightColor ( const int pos , const wxColour& c )
    {
    if ( !can || !can->child || !can->child->vec ) return c ;
    int a , l = can->child->vec->getSequenceLength() ;
    for ( a = 0 ; a < highlight_end.size() ; a++ )
        {
        if ( pos >= highlight_begin[a] && pos <= highlight_end[a] )
            break ;
        if ( highlight_end[a] - l >= pos )
            break ;
        }
    if ( a == highlight_end.size() ) return c ;
    return highlight_color[a] ;
    }

void SeqBasic::clearHighlights ()
    {
    highlight_begin.Clear () ;
    highlight_end.Clear () ;
    highlight_color.clear () ;
    }


SeqBasic::~SeqBasic ()
    {
    // Farewell, my old faithful friend!
    }

//************************************************ SeqPrimer

bool SeqPrimer::useDirectRoutines () const { return true ; }

int SeqPrimer::arrange_direct ( const int n )
    {
//  arrange ( n ) ;
    return SeqDNA::arrange_direct ( n ) ;
    }

void SeqPrimer::show_direct ( wxDC& dc )
    {
    SeqDNA::show_direct ( dc ) ;
    }

void SeqPrimer::show ( wxDC& dc )
    {
    if ( useDirectRoutines() ) show_direct ( dc ) ;
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int cnt = offset+1 ;
    wxString t ;
    bool showLowercaseDNA = myapp()->frame->showLowercaseDNA ;

    dc.SetTextBackground ( *wxWHITE ) ;
    dc.SetTextForeground ( *wxBLACK ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;

    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    for ( int a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        int b = pos.p[a] ;
        int ty = pos.r[a].y ;
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

            if ( showLowercaseDNA ) dc.DrawText ( t.Lower() , pos.r[a].x, pos.r[a].y ) ;
            else dc.DrawText ( t , pos.r[a].x, pos.r[a].y ) ;

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
                //sprintf ( u , "%d" , cnt ) ;
                //t = u ;
                t = wxString::Format ( _T("%d") , cnt ) ;
                while ( t.length() < endnumberlength ) t = _T("0") + t ;
                }
            else t = alternateName ;
            dc.DrawText ( t , pos.r[a].x, pos.r[a].y ) ;
            }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

void SeqPrimer::initFromTVector ( /* const */ TVector * const v )
    {
    vec = v ; // FIXME: Needs copy constructor - not const
    s = vec->getSequence() ;
    for ( int a = 0 ; a < s.length() ; a++ ) s.SetChar(a,' ') ;
    takesMouseActions = true ;
    showNumbers = false ;
    }

void SeqPrimer::addPrimer ( const TPrimer * const p )
    {
    TVector d ;
    myass ( p , "SeqPrimer::addPrimer_0" ) ;
    myass ( vec , "SeqPrimer::addPrimer_1" ) ;
    d.setSequence ( s )  ;
    d.setCircular(vec->isCircular()) ;
    for ( int a = p->from ; a <= p->to ; a++ )
        {
        myass ( a-p->from >= 0 , "SeqPrimer::addPrimer_2" ) ;
        myass ( a-p->from < p->sequence.length() , "SeqPrimer::addPrimer_3" ) ;
        d.setNucleotide ( a-1 , p->sequence.GetChar(a-p->from) ) ;
        }
    s = d.getSequence () ;
    }

//************************************************ SeqNum

int SeqNum::arrange ( const int n )
    {
    int x , y , w , h , bo = can->border , lowy = 0 ;
    int lasta = 0 ;

    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo ;//, endnumber = offset + s.length() ;

    endnumberlength = 0 ;
    for ( int a = 0 ; a < can->seq.GetCount() ; a++ )
        {
        if ( can->seq[a]->whatsthis() == _T("ALIGN") )
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
    for ( int a = 0 ; a < s.length() ; a++ )
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
    int bm = dc.GetBackgroundMode () ;
    dc.SetTextForeground ( *wxBLACK ) ;
    dc.SetTextBackground ( *wxWHITE ) ;
    dc.SetBackgroundMode ( wxSOLID ) ;
    //char tt[100] ;
    for ( int a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        //sprintf ( tt , "%d" , pos.p[a]+offset ) ;
        dc.DrawText ( wxString::Format ( _T("%d") , pos.p[a]+offset ) , pos.r[a].x, pos.r[a].y ) ;
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

//************************************************ SeqDivider

int SeqDivider::arrange ( const int n )
    {
    int x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
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
    for ( int a = 0 ; a < s.length() ; a++ )
        {
//      pos.add ( a+1 , x , y , wx-1 , wy-1 ) ;
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
    int w , h ;
    can->MyGetSize ( &w , &h ) ;
    for ( int a = 0 ; a < pos.p.GetCount() ; a++ )
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

void SeqDivider::initFromTVector ( TVector * const v )
    {
//  vec = v ;
    s = v->getSequence() ;
    }
