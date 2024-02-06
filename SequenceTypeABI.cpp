/** \file
    \brief Contains the SeqABI class members
*/
#include "SequenceCanvas.h"

SeqABI::~SeqABI ()
    {
    if ( at ) delete at ;
    }

int SeqABI::get_bx ( int id , int idx )
    {
    if ( at )
        {
        int bx1 = (char) at->vf[id].data[idx*2] ;
        int bx2 = (unsigned char) at->vf[id].data[idx*2+1] ;
        int bx = bx1 * 256 + bx2 ;
        return bx ;
        }
    else
        {
        if ( id < 0 || id >= 4 ) return 0 ;
        if ( idx < 0 || idx >= sd.sequence.size() ) return 0 ;
        return sd.sequence[idx].peak_index ;
        }
    }

int SeqABI::arrange_sd ( int n )
    {
    int x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;

    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    force_add_line_number = view_from > 0 ? view_from - 1 : 0 ;
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

    int id = 0 ;

    // Initiate Color coding
    int intensity = 200 ;
    colors[0] = wxColour ( 0 , 0 , 0 ) ;
    colors[1] = wxColour ( intensity , 0 , 0 ) ;
    colors[2] = wxColour ( 0 , intensity , 0 ) ;
    colors[3] = wxColour ( 0 , 0 , intensity ) ;

    // Base order
//  id = at->getRecord ( _T("FWO_") , 1 ) ;
//  myass ( id != -1 , "SeqABI::arrange_1" ) ;

    // GATC
    base2color[0] = 'T' ;//AGC
    base2color[1] = 'G' ;//CT
    base2color[2] = 'C' ;//TAT
    base2color[3] = 'A' ;//GC

    // For painting
    minx = ox ;
    maxx = w - ox - wx ;
    int diffx = 0 ;
//  id = at->getRecord ( _T("PLOC") , 2 ) ;
//  myass ( id != -1 , "SeqABI::arrange_2" ) ;

    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number

//  int id2 = at->getRecord ( _T("DATA") , 9 ) ;
//  int lastx = at->vf[id2].nrecords ;
//  int lastx = 0 ; //!!!!
    int lastx = sd.tracer2[0].size() ;
    strange_compensation_factor = 0 ;

    if ( view_from > 0 ) diffx = get_bx ( id , view_from-1 ) * scalex ;
/*  if ( view_from > 0 )
        {
        diffx = sd.sequence[a].peak_index;
        if ( inv_compl ) diffx = lastx - diffx ;
        diffx *= scalex ;
        }
*/
    bool first = true ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        if ( view_from > 0 && a+1 < view_from ) continue ;
        if ( view_to > 0 && a >= view_to ) break ;
        int idx = a ;
        if ( inv_compl ) idx = s.length() - a - 1 ;

        int bx = get_bx ( 0 , idx ) ;
        if ( inv_compl ) bx = lastx - bx ;
        x = bx * scalex - diffx ;

        // Ugly hack for partial inverse/complementary view
        if ( first && view_from > 0 && inv_compl && x < 0 )
            {
            strange_compensation_factor = wx - x ;
            x += strange_compensation_factor ;
            diffx -= strange_compensation_factor ;
            }
        first = false ;

        if ( x > maxx )
            {
            pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
            lasta = pos.p.GetCount()+1 ;
            y += wy * ( can->seq.GetCount() + can->blankline ) ;
            if ( a+1 < s.length() )
                pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
            diffx += maxx ;
            x = bx * scalex - diffx ;
            }

        pos.add ( a+1 , x + minx - wx / 2 , y , wx-1 , wy-1 ) ;
        can->setLowX ( x + minx + wx*2 ) ;
        lowy = y+wy ;

        }
    if ( lasta != pos.p.GetCount()+1 )
        pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;

    lowy += wy * ( can->blankline ) ; // For sequence data

    return lowy + bo*2 ;
    }

int SeqABI::arrange ( int n )
    {
    if ( !at ) return arrange_sd ( n ) ;
    int x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;

    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    force_add_line_number = view_from > 0 ? view_from - 1 : 0 ;
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

    // Base order
    id = at->getRecord ( _T("FWO_") , 1 ) ;
    myass ( id != -1 , "SeqABI::arrange_1" ) ;

    // GATC
    int *ii = (int*) base2color ;
    *ii = at->vf[id].value ;

    // For painting
    minx = ox ;
    maxx = w - ox - wx ;
    int diffx = 0 ;
    id = at->getRecord ( _T("PLOC") , 2 ) ;
    myass ( id != -1 , "SeqABI::arrange_2" ) ;

    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number

    int id2 = at->getRecord ( _T("DATA") , 9 ) ;
    int lastx = at->vf[id2].nrecords ;
    strange_compensation_factor = 0 ;

    if ( view_from > 0 ) diffx = get_bx ( id , view_from-1 ) * scalex ;

    bool first = true ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        if ( view_from > 0 && a+1 < view_from ) continue ;
        if ( view_to > 0 && a >= view_to ) break ;
        int idx = a ;
        if ( inv_compl ) idx = s.length() - a - 1 ;

        int bx = get_bx ( id , idx ) ;
        if ( inv_compl ) bx = lastx - bx ;
        x = bx * scalex - diffx ;

        // Ugly hack for partial inverse/complementary view
        if ( first && view_from > 0 && inv_compl && x < 0 )
            {
            strange_compensation_factor = wx - x ;
            x += strange_compensation_factor ;
            diffx -= strange_compensation_factor ;
            }
        first = false ;

        if ( x > maxx )
            {
            pos.addline ( lasta , pos.p.GetCount() , y , y+wy-1 ) ;
            lasta = pos.p.GetCount()+1 ;
            y += wy * ( can->seq.GetCount() + can->blankline ) ;
            if ( a+1 < s.length() )
                pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
            diffx += maxx ;
            x = bx * scalex - diffx ;
            }

        pos.add ( a+1 , x + minx - wx / 2 , y , wx-1 , wy-1 ) ;
        can->setLowX ( x + minx + wx*2 ) ;
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
        wxString s2 ;
        for ( int a = 0 ; a < s.length() ; a++ ) s2 = ((wxChar)v2.getComplement( s.GetChar(a) ) ) + s2 ;
        s = s2 ;
        can->child->vec->setSequence ( s ) ;
        }

    inv_compl = x ;
    }

void SeqABI::show_sd ( wxDC& dc )
    {
    SeqDNA::show ( dc ) ;

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
    unsigned short int high = 0 ;

    for ( unsigned long data = 0 ; data < 4 ; data++ )
        {
        for ( int a = 0 ; a < sd.tracer2[data].size() ; a++ )
            {
            unsigned short int b = sd.tracer2[data][a] ;
            if ( b > high ) high = b ;
            }
        }

//    int limit_abs_x = ( view_to > 0 ) ? get_bx ( at->getRecord ( _T("PLOC") , 2 ) , view_to ) * scalex : -1 ;
    int limit_abs_x = ( view_to > 0 ) ? get_bx ( 0 , view_to ) * scalex : -1 ;
//  int limit_abs_x = -1 ; //!!!!

    // Painting
    for ( unsigned long data = 0 ; data < 4 ; data++ )
        {
        if ( can->isPrinting() && !can->getPrintToColor() )
            dc.SetPen ( *wxBLACK_PEN ) ;
        else
            dc.SetPen ( *MYPEN ( colors[data] ) ) ;

//      int id = at->getRecord ( _T("DATA") , 9 + data ) ;
//      myass ( id > -1 , "SeqABI::show_sd" ) ;
        long diffx = 0 ;
        long l = 0 ;
        wxPoint last ( minx , pos.l[l].height + hy ) ;
//      wxPoint last ( 0 , 0 ) ;
        long x = 0 , abs_x = 0 ;
        if ( view_from > 0 )
            {
//          x = -get_bx ( at->getRecord ( _T("PLOC") , 2 ) , view_from-1 ) * scalex ;
            x = -get_bx ( 0 , view_from-1 ) * scalex ;
            if ( inv_compl ) x += strange_compensation_factor ;
            }
        for ( long a = 0 ; a < sd.tracer2[data].size() ; a++ )
            {
            int idx = a ;
            if ( inv_compl ) idx = ( sd.tracer2[data].size() - a - 1 ) ;
            int b = sd.tracer2[data][idx] ;
            x += scalex ;
            abs_x += scalex ;
            while ( x > maxx )
                {
                x -= maxx ;
                diffx += maxx ;
                if ( l > 0 ) drawTopLine ( dc , pos.l[l].width - wy / 2 ) ;
                l++ ;
                if ( l < pos.l.size() )
                    {
                    int yo = last.y - pos.l[l-1].height ;
                    last = wxPoint ( minx , pos.l[l].height + yo ) ;
                    }
                }
            wxPoint cur ;
            if ( l < pos.l.size() )
                {
                b *= zoom ;
                if ( b > high ) b = high ;
                cur = wxPoint ( x + minx , hy - b * hy / high + pos.l[l].height ) ;
                if ( limit_abs_x != -1 && abs_x >= limit_abs_x ) break ;
                if ( cur.x >= minx ) dc.DrawLine ( last , cur ) ;
                }
            last = cur ;
            }
        if ( l > 0 ) drawTopLine ( dc , pos.l[l].width - wy / 2 ) ;
        }
    }

void SeqABI::show ( wxDC& dc )
    {
    if ( !at ) { show_sd ( dc ) ; return ; }
    SeqDNA::show ( dc ) ;

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
    for ( int data = 0 ; data < 4 ; data++ )
        {
        int id = at->getRecord ( _T("DATA") , 9 + data ) ;
        for ( int a = 0 ; a < at->vf[id].nrecords ; a++ )
           {
           int b1 = (char) at->vf[id].data[a*2] ;
           int b2 = (unsigned char) at->vf[id].data[a*2+1] ;
           int b = b1 * 256 + b2 ;
           if ( b > high ) high = b ;
           }
        }

    int limit_abs_x = ( view_to > 0 ) ? get_bx ( at->getRecord ( _T("PLOC") , 2 ) , view_to ) * scalex : -1 ;

    // Painting
    for ( int data = 0 ; data < 4 ; data++ )
        {
        if ( can->isPrinting() && !can->getPrintToColor() )
            dc.SetPen ( *wxBLACK_PEN ) ;
        else
            dc.SetPen ( *MYPEN ( colors[data] ) ) ;

        int id = at->getRecord ( _T("DATA") , 9 + data ) ;
        myass ( id > -1 , "SeqABI::show" ) ;
        int diffx = 0 ;
        int l = 0 ;
        wxPoint last ( minx , pos.l[l].height + hy ) ;
        int x = 0 , abs_x = 0 ;
        if ( view_from > 0 )
            {
            x = -get_bx ( at->getRecord ( _T("PLOC") , 2 ) , view_from-1 ) * scalex ;
            if ( inv_compl ) x += strange_compensation_factor ;
            }
        for ( int a = 0 ; a < at->vf[id].nrecords ; a++ )
            {
            int idx = a ;
            if ( inv_compl ) idx = ( at->vf[id].nrecords - a - 1 ) ;
            int b = get_bx ( id , idx ) ;
/*          int b1 = (unsigned char) at->vf[id].data[idx] ;
            int b2 = (unsigned char) at->vf[id].data[idx+1] ;
            int b = b1 * 256 + b2 ;*/
            x += scalex ;
            abs_x += scalex ;
            while ( x > maxx )
                {
                x -= maxx ;
                diffx += maxx ;
                if ( l > 0 ) drawTopLine ( dc , pos.l[l].width - wy / 2 ) ;
                l++ ;
                if ( l < pos.l.size() )
                    {
                    int yo = last.y - pos.l[l-1].height ;
                    last = wxPoint ( minx , pos.l[l].height + yo ) ;
                    }
                }
            wxPoint cur ;
            if ( l < pos.l.size() )
                {
                b *= zoom ;
                if ( b > high ) b = high ;
                cur = wxPoint ( x + minx , hy - b * hy / high + pos.l[l].height ) ;
                if ( limit_abs_x != -1 && abs_x >= limit_abs_x ) break ;
                if ( cur.x >= minx ) dc.DrawLine ( last , cur ) ;
                }
            last = cur ;
            }
        if ( l > 0 ) drawTopLine ( dc , pos.l[l].width - wy / 2 ) ;
        }

    }

void SeqABI::drawTopLine ( wxDC &dc , int y )
    {
    wxPen p = dc.GetPen () ;
    dc.SetPen(*wxLIGHT_GREY_PEN);
    dc.DrawLine ( 4 , y , minx + maxx , y ) ;
    dc.SetPen(p);
    }

void SeqABI::initFromFile ( const wxString& filename )
    {
    zoom = 1 ;
    showHelpLines = true ;
    screenScale = 2 ;

    SCFtype sc ;
    if ( sc.parse ( filename ) ) // SCF
        {
        if ( at ) delete at ;
        at = NULL ;
        sd = sc.sd ;
        s = sd.seq ;
        s.Replace ( _T(" ") , _T("N") ) ;
        s.Replace ( _T("-") , _T("N") ) ;
//      wxMessageBox ( s ) ;
        }
    else // ABI
        {
        if ( !at ) at = new ABItype ;
        at->parse ( filename ) ;
        s = at->getSequence() ;
        }
    vec->setSequence ( s ) ;
    showNumbers = true ;
    fontColor.Set ( 0 , 0 , 0 ) ;
    }

wxColor SeqABI::getBaseColor ( const char b )
    {
    for ( int a = 0 ; a < 4 ; a++ )
        {
        if ( b == base2color[a] )
            {
#ifdef __WXMAC__
            if ( !inv_compl ) return colors[a] ;
            else return colors[3-a] ;
#else
            if ( inv_compl ) return colors[a] ;
            else return colors[3-a] ;
#endif
            }
        }
    return wxColor ( _T("magenta") ) ;
    }

