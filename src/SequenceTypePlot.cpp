/** \file
    \brief Contains the SeqPlot class members
*/
#include "SequenceCanvas.h"

wxString SeqPlot::getTip ( int pos )
    {
    if ( pos < 0 ) return _T("") ;
    if ( pos >= s.length() ) return _T("") ;
    wxString r = wxString::Format(txt("seq_loc"),pos+1) ;
    if ( pos >= prop.size() || prop[pos].data.size() != 1 ) return r ;
    r += _T(" : ") ;
    r += s.GetChar(pos) ;
    r += wxString::Format ( _T(" [%2.2f]") , prop[pos].data[0] ) ;
    return r ;
    }

int SeqPlot::arrange ( int n )
    {
    if ( s.IsEmpty() ) return 0 ;
    int x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;

    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }

    if ( can->isMiniDisplay() ) can->MyGetClientSize ( &w , &h ) ;
    else can->MyGetClientSize ( &w , &h ) ;
//    w -= 20 ; // Scrollbar dummy
    if ( can->isMiniDisplay() && can->getAA() )
        {
        if ( can->getAA()->miniDisplayOptions == MINI_DISPLAY_REAL ) wx = w / s.length() + 1 ;
        if ( can->getAA()->miniDisplayOptions == MINI_DISPLAY_CONDENSED ) wx = w / s.length() + 1 ;
        }

    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx ) ;
    itemsperline *= can->blocksize ;

    pos.cleanup() ;
    x = ox ;
    y = oy ;
    pos.add ( -(++l) , bo , y , ox-wx-bo , wy-1 ) ; // Line number
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        if ( can->isMiniDisplay() && can->getAA() && can->getAA()->miniDisplayOptions == MINI_DISPLAY_CONDENSED )
            {
            x = ox + ( w - ox ) * a / static_cast<unsigned int>(s.length()) ;
            }
        pos.add ( a+1 , x , y , wx-1 , wy*lines-1 ) ;
        can->setLowX ( x + wx*2 ) ;
        lowy = y+wy*lines ;
        x += wx ;
        if ( (a+1) % can->blocksize == 0 )
            {
            x += wx-1 ;
            if ( x+wx*(can->blocksize+1) >= w )
                {
                pos.addline ( lasta , pos.p.GetCount() , y , y+wy*lines-1 ) ;
                lasta = pos.p.GetCount()+1 ;
                x = ox ;
                y += wy * ( can->seq.GetCount() + can->blankline ) ;
                if ( a+1 < s.length() )
                    pos.add ( -(++l) , bo , y , ox-wx-5 , wy-1 ) ; // Line number
                }
            }
        }
    if ( lasta != pos.p.GetCount()+1 )
        pos.addline ( lasta , pos.p.GetCount() , y , y+wy*lines-1 ) ;
    return lowy + bo*2 ;
    }


// Display


void SeqPlot::drawDottedLine ( wxDC &dc , const int x1 , const int y1 , const int x2 , const int y2 ) const
    {
    if ( can->isPrinting() )
        {
        dc.DrawLine ( x1 , y1 , x2 , y2 ) ;
        return ;
        }
    if ( x1 == x2 )
        {
        for ( int i = y1 ; i <= y2 ; i++ )
            if ( i & 1 ) dc.DrawPoint ( x1 , i ) ;
        }
    else
        {
        for ( int i = x1 ; i <= x2 ; i++ )
            if ( i & 1 ) dc.DrawPoint ( i , y1 ) ;
        }
    }

void SeqPlot::myRect ( wxDC &dc , const int x , const int y , const int w , const int h ) const
    {
    if ( can->isPrinting() )
        {
        for ( int i = 0 ; i <= h ; i++ )
            {
            dc.DrawLine ( x , y+i , x+w , y+i ) ;
            }
        }
    else
        {
        for ( int i = 0 ; i < w ; i++ )
            {
            for ( int j = 0 ; j < h ; j++ )
                {
                if ( ( x+i + y+j ) & 1 ) dc.DrawPoint ( x + i , y + j ) ;
                }
            }
        }
    }

void SeqPlot::fixMinMax ( float &f ) const
    {
    f = f > data_max ? data_max : f ;
    f = f < data_min ? data_min : f ;
    }

void SeqPlot::show ( wxDC& dc )
    {
    if ( s.IsEmpty() ) return ;
    mylog ( "SeqPlot::show" , "1" ) ;
    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int cnt = offset+1 ;
    wxString t ;
    char valid[256] ;
    for ( int a = 0 ; a < 256 ; a++ ) valid[a] = 0 ;
    valid['A'] = valid['C'] = valid['T'] = valid['G'] = valid[' '] = 1 ;
//  dc.SetTextBackground ( *wxWHITE ) ;
    dc.SetTextForeground ( fontColor ) ;
//  dc.SetBackgroundMode ( wxSOLID ) ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    int xa , xb , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    xa = -xa ;
    can->MyGetClientSize ( &xb , &yb ) ;
    yb += ya ;
    xb += xa ;
    int lx = 0 ;
    startOfLine = true ;
    mylog ( "SeqPlot::show" , "2" ) ;
    for ( int a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        mylog ( "SeqPlot::show" , "2a" ) ;
        if ( can->hardstop > -1 && a > can->hardstop ) break ;
        mylog ( "SeqPlot::show" , "2b" ) ;
        int b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tz = ty + can->charheight * lines ;
        bool insight = true ; // Meaning "is this part visible"
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( tx + can->charwidth < xa ) insight = false ;
        if ( tx > xb ) insight = false ;
        if ( can->getDrawAll() ) insight = true ;
        if ( !insight && ty > yb ) a = pos.p.GetCount() ;
        if ( b > 0 && !insight ) cnt++ ;
            mylog ( "SeqPlot::show" , "2c" ) ;
        if ( b > 0 && insight ) // Character
            {
            if ( lx == 0 ) lx = tx ;
            t = s.GetChar(b-1) ;
            if ( can->isPrinting() )
                {
                if (getMark ( a ) == 1 )
                    {
                    dc.SetBrush ( *MYBRUSH ( wxColour ( 230 , 230 , 230 ) ) ) ;
                    dc.SetPen(*wxTRANSPARENT_PEN);
                    dc.DrawRectangle ( tx , ty , can->charwidth , can->charheight ) ;
                    }
                mylog ( "SeqPlot::show" , "2c1" ) ;
                if ( !can->getPrintToColor() )
                    {
                    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
                    dc.SetTextForeground ( *wxBLACK ) ;
                    }
                }

            mylog ( "SeqPlot::show" , wxString::Format ( _T("2c2 (type %d)") , type ) ) ;
            switch ( type )
                {
                case CHOU_FASMAN : showChouFasman ( dc , b-1 , tx , ty , lx ) ; break ;
                case COILED_COIL : showChouFasman ( dc , b-1 , tx , ty , lx ) ; break ;
                case M_W : showMW ( dc , b-1 , tx , ty , lx ) ; break ;
                case P_I : showPI ( dc , b-1 , tx , ty , lx ) ; break ;
                case H_P : showHP ( dc , b-1 , tx , ty , lx ) ; break ;
                }
            lx = tx + can->charwidth ;
            cnt++ ;
            startOfLine = false ;
            mylog ( "SeqPlot::show" , "2c3" ) ;
            }
        else if ( insight ) // Front
           {
           mylog ( "SeqPlot::show" , "2d1" ) ;
           lx = 0 ;
           startOfLine = true ;
           if ( !can->isMiniDisplay() ) continue ;
           mylog ( "SeqPlot::show" , "2d2" ) ;
           dc.SetFont(*can->smallFont);
           if ( type == CHOU_FASMAN ) t = _T("Chou-Fasman") ;
           if ( type == COILED_COIL ) t = _T("Coiled-coil") ;
           else if ( type == M_W ) t = _T("MW") ;
           else if ( type == P_I ) t = _T("pI") ;
           else if ( type == H_P )
              {
              t = _T("t_method_") ;
              t += hp_method ;
              t = txt(t) ;
              t += wxString::Format( _T(" [%d]") , hp_window ) ;
              }
           mylog ( "SeqPlot::show" , "2d3" ) ;
           dc.SetTextForeground ( *wxBLACK ) ;
           int tw , th ;
           dc.GetTextExtent ( t , &tw , &th ) ;
           int ty = pos.r[a].y ;
           ty += lines * can->charheight ;
           mylog ( "SeqPlot::show" , "2d4" ) ;
#ifdef __WXGTK__
           ty += th / 2 ;
           dc.DrawText ( t , pos.r[a].x , ty ) ;
#else
           ty -= ( lines * can->charheight - tw ) / 2 ;
           dc.DrawRotatedText ( t , pos.r[a].x, ty , 90 ) ;
#endif
           dc.SetFont(*can->font);
           }
            mylog ( "SeqPlot::show" , "2e" ) ;
        }
    mylog ( "SeqPlot::show" , "3" ) ;
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }

void SeqPlot::showMW ( wxDC &dc , const int b , const int tx , const int ty , const int lx ) /* not const */
    {
    int ch = can->charheight / 2 ;
    int cw = can->charwidth ;
    int bottom = ty + lines * can->charheight ;
    for ( unsigned int u = 1 ; u < d1.GetCount() ; u++ )
        {
        int tz = ty + (u-1) * ch + 1 ;
        int tw = ( tx + cw ) - lx ;

        if ( getMark ( b+1 ) > 0 )
            {
            dc.SetPen ( *wxGREY_PEN ) ;
            myRect ( dc , lx , tz , tw , ch*can->charheight - 2 ) ;
            }
        }

        showPlot ( dc , b , tx , ty , lx , bottom - ty - ch ) ; // not const (should be)
    }

void SeqPlot::showPI ( wxDC &dc , const int b , const int tx , const int ty , const int lx ) /* not const */
    {
    // All the same...
    showMW ( dc , b , tx , ty , lx ) ; // not const because of showPlot
    }

void SeqPlot::showHP ( wxDC &dc , const int b , const int tx , const int ty , const int lx ) /* not const */
    {
    // All the same...
    showMW ( dc , b , tx , ty , lx ) ; // not const because of showPlot
    }

void SeqPlot::showNcoils ( wxDC &dc , const int b , const int tx , const int ty , const int lx ) const
    {
    }

void SeqPlot::showChouFasman ( wxDC &dc , const int b , const int tx , const int ty , const int lx ) /* not const */
    {
//     mylog ( "SeqPlot::showChouFasman" , "1" ) ;
    int bottom = ty + lines * can->charheight ;
    int ch = can->charheight / 2 ;
    int cw = can->charwidth ;
//  mylog ( "SeqPlot::showChouFasman" , "2" ) ;
    for ( unsigned int u = 1 ; u < d1.GetCount() ; u++ )
        {
        const wxPen *pen = wxRED_PEN ;
        if ( u == 2 ) pen = wxGREEN_PEN ;
        if ( u == 3 ) pen = wxBLUE_PEN ;
        int tz = ty + (u-1) * ch + 1 ;
        int tw = ( tx + cw ) - lx ;

        if ( getMark ( b+1 ) > 0 )
            {
            dc.SetPen ( *wxGREY_PEN ) ;
            myRect ( dc , lx , tz , tw , ch*can->charheight - 2 ) ;
            }

        if ( d1[u][(uint)b] == 'X' )
            {
            dc.SetPen(*pen);
            myRect ( dc , lx , tz , tw , ch - 2 ) ;
            dc.SetPen(*wxBLACK_PEN);
            dc.DrawLine ( lx , tz , lx + tw , tz ) ;
            dc.DrawLine ( lx , tz + ch - 2 , lx + tw , tz + ch - 2 ) ;
            if ( b == 0 || d1[u][(uint)b-1] == ' ' )
                dc.DrawLine ( lx , tz , lx , tz + ch - 2 ) ;
            if ( b + 1 == s.length() || d1[u][(uint)b+1] == ' ' )
                dc.DrawLine ( lx + tw , tz , lx + tw , tz + ch - 2 ) ;
            }
        else
            {
            dc.SetPen(*wxBLACK_PEN);
            dc.DrawLine ( lx , ty + (u-1)*ch + ch/2 ,
                          tx + cw , ty + (u-1)*ch + ch/2 ) ;
            }
        if ( startOfLine )
            {
            if ( type == CHOU_FASMAN )
                {
//  mylog ( "SeqPlot::showChouFasman" , "2a" ) ;
                if ( u == 1 ) drawSymbol ( 'a' , dc , lx-cw , ty+(u-1)*ch , lx-2 , ty+(u-1)*ch+ch ) ; // Alpha
                else if ( u == 2 ) drawSymbol ( 'b' , dc , lx-cw , ty+(u-1)*ch , lx-2 , ty+(u-1)*ch+ch ) ; // Beta
                else drawSymbol ( 'T' , dc , lx-cw , ty+(u-1)*ch , lx-2 , ty+(u-1)*ch+ch ) ; // Turn
                }
            else if ( type == COILED_COIL )
                {
//  mylog ( "SeqPlot::showChouFasman" , "2b" ) ;
                if ( u == 1 ) dc.DrawText ( _T("14") , lx-cw*2 , ty+(u-1)*ch-ch/2 ) ;
                else if ( u == 2 ) dc.DrawText ( _T("21") , lx-cw*2 , ty+(u-1)*ch-ch/2 ) ;
                else if ( u == 3 ) dc.DrawText ( _T("28") , lx-cw*2 , ty+(u-1)*ch-ch/2 ) ;
                }
            }
        }

//  mylog ( "SeqPlot::showChouFasman" , "3" ) ;
    showPlot ( dc , b , tx , ty , lx , bottom - d1.GetCount() * ch - ty ) ; // not const
//  mylog ( "SeqPlot::showChouFasman" , "4" ) ;
    }


void SeqPlot::showPlot ( wxDC &dc , const int b , const int tx , const int ty , const int lx , const int ph ) /* not const */
    {
    mylog ( "SeqPlot::showPlot" , "1" ) ;
    if ( !can->isMiniDisplay() ) return ;
    if ( ((int)data_h) == 0 ) data_h = 1 ; // not const //return ; // Flat data or something...
    mylog ( "SeqPlot::showPlot" , "2" ) ;
    int ch = can->charheight ;
    int cw = can->charwidth ;
    int bottom = ty + lines * can->charheight ;

    if ( can->isMiniDisplay() )
        {
        mylog ( "SeqPlot::showPlot" , "2a" ) ;
        if ( b == 0 ) plotr = wxRect ( lx , bottom - ph , 0 , ph ) ;
        plotr.SetWidth ( tx + cw - plotr.GetLeft() ) ;
        }

    mylog ( "SeqPlot::showPlot" , "3" ) ;
    // Dotted lines
    dc.SetPen ( *wxBLACK_PEN ) ;
    for ( float j = data_min + data_step ; j + data_step <= data_max ; j += data_step )
        {
        myass ( ((int)data_h) != 0 , "SeqPlot::showPlot divides by 0!" ) ;
        int k = bottom - ( (int)(j - data_min) ) * ph / ((int)data_h) ;
        drawDottedLine ( dc , lx , k , tx + cw , k ) ;
        }

    mylog ( "SeqPlot::showPlot" , "4" ) ;
    if ( can->isMiniDisplay() && b > 0 && b % 50 == 0 )
        {
        drawDottedLine ( dc , lx , bottom - ph , lx , bottom + ch/2 ) ;
        dc.SetFont ( *can->smallFont ) ;
        int qx , qy ;
        wxString ws ;
        ws = wxString::Format ( _T("%d") , b ) ;
        dc.GetTextExtent ( ws , &qx , &qy ) ;
        dc.DrawText ( ws , lx - qx/2 , bottom + ch/2 ) ;
        }

    // Colored curves
    mylog ( "SeqPlot::showPlot" , "5" ) ;
    for ( int u = 0 ; u < prop[b].data.size() ; u++ )
        {
        const wxPen *pen = wxRED_PEN ;
        if ( can->isPrinting() ) pen = wxBLACK_PEN ;
        else if ( u == 1 ) pen = wxGREEN_PEN ;
        else if ( u == 2 ) pen = wxBLUE_PEN ;
        dc.SetPen(*pen);
        float ny = prop[b].data[u] ;
        float oy = b==0?ny:prop[b-1].data[u] ;
        fixMinMax ( ny ) ;
        fixMinMax ( oy ) ;

        ny = bottom - ( ny - data_min ) * ph / data_h ;
        oy = bottom - ( oy - data_min )  * ph / data_h ;
        dc.DrawLine ( lx , (int)oy , tx + cw , (int)ny ) ;
        }

    // Border lines
    mylog ( "SeqPlot::showPlot" , "6" ) ;
    dc.SetPen ( *wxBLACK_PEN ) ;
    dc.DrawLine ( lx , bottom , tx + cw , bottom ) ;
    dc.DrawLine ( lx , bottom - ph , tx + cw , bottom - ph ) ;
    if ( startOfLine )
        {
        wxFont cf = dc.GetFont () ;
        dc.SetFont ( *can->smallFont ) ;
        dc.DrawLine ( tx , bottom - ph , tx , bottom ) ;
        int qx , qy ;
        wxString ws ;
        ws = wxString::Format ( _T("%.1f") , data_max ) ;
        dc.GetTextExtent ( ws , &qx , &qy ) ;
        dc.DrawText ( ws , tx - qx - cw/3 , bottom - ph - qy/2 ) ;
        dc.DrawLine ( tx - cw/3 , bottom - ph , tx , bottom - ph ) ;
        ws = wxString::Format ( _T("%.1f") , data_min ) ;
        dc.GetTextExtent ( ws , &qx , &qy ) ;
        dc.DrawText ( ws , tx - qx - cw/3 , bottom - qy/2 ) ;
        dc.DrawLine ( tx - cw/3 , bottom , tx , bottom ) ;
        dc.SetFont ( cf ) ;
        }

    mylog ( "SeqPlot::showPlot" , "7" ) ;
    if ( b + 1 == s.length () )
        {
        dc.DrawLine ( tx+cw , bottom - ph , tx+cw , bottom ) ;
        }
    mylog ( "SeqPlot::showPlot" , "8" ) ;
    }

void SeqPlot::drawSymbol ( const char c , wxDC &dc , const int x1 , const int y1 , const int x2 , const int y2 ) const
    {
    dc.SetPen(*wxBLACK_PEN);
    int w = x2 - x1 ;
    int h = y2 - y1 ;
    if ( c == 'T' )
        {
        int y3 = y2 - (h-1)/4 ;
        dc.DrawLine ( x1 , y1+1 , (x2-1) - (w-1)/3 , y1+1 ) ;
        dc.DrawLine ( x1 , y3 , (x2-1) - (w-1)/3 , y3 ) ;
        dc.DrawArc ( (x2-1) - (w-1)/3 , y3 , (x2-1) - (w-1)/3 , y1+1 , (x2-1) - (w-1)/3 , (y1+1+y3)/2 ) ;
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine ( (x2-1) - (w-1)/3 , y3-1 , (x2-1) - (w-1)/3 , y1+1+1 ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine ( x1 , y3 , x1 + (w-1)*2/3 , y3 - (h-1)/2 ) ;
        dc.DrawLine ( x1 , y3 , x1 + (w-1)*2/3 , y3 + (h-1)/2 ) ;
        }
    else if ( c == 'b' )
        {
        int _x1 = x1 + w/2 - h/6 ;
        dc.DrawLine ( _x1 , y1 , _x1 , y2 ) ;
        dc.DrawArc ( _x1 , y1+h/3 , _x1 , y1 , _x1 , y1+h/6 ) ;
        dc.DrawArc ( _x1 , y1+h*2/3 , _x1 , y1+h/3 , _x1 , y1+h*3/6 ) ;
        }
    else if ( c == 'a' )
        {
        int x3 = x2 - w/3 ;
        dc.DrawArc ( x3 , y1 + h/3 , x3 , y2 - h/3 , x1 + w/2 , y1 + h/2 ) ;
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine ( x3 , y1 + h/3 + 1 , x1 + w/2 , y1 + h/2 ) ;
        dc.DrawLine ( x3 , y2 - h/3 - 1 , x1 + w/2 , y1 + h/2 ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine ( x3 , y1 + h/3 , x2 , y2 ) ;
        dc.DrawLine ( x3 , y2 - h/3 , x2 , y1 ) ;
        }
    }

// Communication with the outside world :-)

void SeqPlot::init ( SequenceCanvas * const ncan )
    {
    SeqBasic::init ( ncan ) ;
    hp_window = 7 ;
    hp_method = _T("kyte-doolittle") ;
    }

void SeqPlot::initFromTVector ( const TVector * const v )
    {
    if ( ! vec ) {
        vec = new TVector ( NULL ) ;
    }
    vec->copy ( *v ) ;
    s = vec->getSequence() ;

    if ( can && can->getEditMode() && vec->getSequenceLength() && vec->getSequenceChar(vec->getSequenceLength()-1) == ' ' )
       {
       s.Truncate ( s.length()-1 ) ;
       }

    takesMouseActions = true ;
    }

void SeqPlot::setLines ( const int l )
    {
    wxArraySeqBasic v2 ;
    while ( can->seq[can->seq.GetCount()-1] != this )
        {
        v2.Add ( can->seq[can->seq.GetCount()-1] ) ;
        can->seq.RemoveAt ( can->seq.GetCount()-1 ) ;
        }
    for ( int a = 0 ; a < l ; a++ )
        {
        SeqBlank *bl = new SeqBlank ( can ) ;
        bl->initFromTVector ( vec ) ;
        can->seq.Add ( bl ) ;
        }
    for ( int a = lines ; a < v2.GetCount() ; a++ ) can->seq.Add ( v2[a] ) ;
    for ( int a = 0 ; a < lines ; a++ ) delete v2[a] ;

    lines = l ;
    }


// Calculation routines

void SeqPlot::useNcoils ()
    {
    mylog ( "SeqPlot::useNcoils" , "BEGIN" ) ;
    type = COILED_COIL ;
    d1.Clear () ;
    d2.Clear () ;
    d3.Clear () ;
    if ( s.IsEmpty() ) return ;
    mylog ( "SeqPlot::useNcoils" , "1" ) ;
    l_top = 4 ;
    l_bottom = 0 ;
//  if ( l_top + l_bottom + 1 > lines ) setLines ( l_top + l_bottom + 1 ) ;

    wxArrayFloat af[3] ;
    for ( int a = 0 ; s.length() > 0 && a < 3 ; a++ )
        {
        int b = a==0?14:(a==1?21:28) ;
        string x ;
        mylog ( "SeqPlot::useNcoils" , wxString::Format ( _T("BEGIN ncoils_function (%d): ") , b ) + s ) ;
        x = ncoils_function ( (const char*) s.mb_str() , b ) . c_str() ;
        mylog ( "SeqPlot::useNcoils" , "END ncoils_function" ) ;
        wxString t ( (const char*) x.c_str() , wxConvUTF8 ) ;
        wxArrayString ta ;
        explode ( _T("\n") , t , ta ) ;
        for ( b = 0 ; b < s.length() ; b++ )
            {
            if ( b >= ta.GetCount() ) break ;
            t = ta[b].Mid ( 18 ) ;
            double prob ;
            t.ToDouble ( &prob ) ;
            af[a].Add ( (float) prob ) ;
            }
        }
     mylog ( "SeqPlot::useNcoils" , "2" ) ;

    wxString x ;
    FILLSTRING ( x , ' ' , s.length() ) ;
    while ( d1.GetCount() < 4 ) d1.Add ( x ) ;
    mylog ( "SeqPlot::useNcoils" , "3" ) ;

    prop.clear () ;
    mylog ( "SeqPlot::useNcoils" , "4" ) ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( TVector::getAAprop ( (char) s.GetChar(a) ) ) ;
        prop[a].data.clear() ;
        prop[a].data.push_back ( af[0][a] ) ;
        prop[a].data.push_back ( af[1][a] ) ;
        prop[a].data.push_back ( af[2][a] ) ;
        d1[1].SetChar ( a , af[0][a] >= 0.5 ? 'X' : ' ' ) ;
        d1[2].SetChar ( a , af[1][a] >= 0.5 ? 'X' : ' ' ) ;
        d1[3].SetChar ( a , af[2][a] >= 0.5 ? 'X' : ' ' ) ;
        }

    mylog ( "SeqPlot::useNcoils" , "5" ) ;
    scanMinMax () ;
    mylog ( "SeqPlot::useNcoils" , "END" ) ;
    }

// See http://prowl.rockefeller.edu/aainfo/chou.htm for background
void SeqPlot::useChouFasman ()
    {
    mylog ( "SeqPlot::useChouFasman" , "1" ) ;
    type = CHOU_FASMAN ;
    d1.Clear () ;
    d2.Clear () ;
    d3.Clear () ;
    if ( s.IsEmpty() ) return ;
    l_top = 4 ;
    l_bottom = 0 ;
//  if ( l_top + l_bottom + 1 > lines ) setLines ( l_top + l_bottom + 1 ) ;

    mylog ( "SeqPlot::useChouFasman" , "2" ) ;
    prop.clear () ;
    for ( size_t a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( TVector::getAAprop ( s[a] ) ) ;
        prop[a].data.clear() ;
        while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
        }
    wxString x ;
    FILLSTRING ( x , ' ' , s.length() ) ;
    while ( d1.GetCount() < 4 ) d1.Add ( x ) ;
    mylog ( "SeqPlot::useChouFasman" , "3" ) ;
    scanChouFasman ( 4 , 6 , 0 , 100 , 4 , 100 , 5 ) ; // Alpha helices
    scanChouFasman ( 3 , 5 , 1 , 100 , 4 , 100 , 105 ) ; // Beta sheets
    mylog ( "SeqPlot::useChouFasman" , "4" ) ;

    // Deciding on overlapping regions
    for ( size_t a = 0 ; a < s.length() ; a++ )
        {
        if ( d1[1][a] != 'X' || d1[2][a] != 'X' ) continue ;
        unsigned int b ;
        float avg0 = 0 , avg1 = 0 ;
        for ( b = a ; b < s.length() && d1[1][b] == 'X' && d1[2][b] == 'X' ; b++ )
            {
            avg0 += prop[b].data[0] ;//prop[b].cf_pa ;
            avg1 += prop[b].data[1] ;//prop[b].cf_pb ;
            }
        int kill = 1 ; // delete alpha helix
        if ( avg1 < avg0 ) kill = 2 ; // delete beta sheet
        for ( uint c = a ; c < b ; c++ ) d1[kill][c] = ' ' ;
        }

    // Turns
     mylog ( "SeqPlot::useChouFasman" , "5" ) ;
    for ( size_t a = 0 ; a + 3 < s.length() ; a++ )
        {
        float p = 1 , avg = 0 , avg2 = 0 ;
        for ( int b = 0 ; b < 4 ; b++ )
            {
            p *= prop[a+b].cf_f[b] ;
            avg += prop[a+b].cf_pt ;
            avg2 += prop[a].cf_f[b] ;
            }
        prop[a].data[2] = avg2 ;
        avg /= 4.0 ;
        if ( p > 0.000075 && avg > 1 &&
                prop[a].cf_pa < prop[a].cf_pt &&
                prop[a].cf_pb < prop[a].cf_pt )
            {
            d1[3][a] = 'X' ;
            }
        }

    mylog ( "SeqPlot::useChouFasman" , "6" ) ;
    scanMinMax () ;
    mylog ( "SeqPlot::useChouFasman" , "FIN" ) ;
    }

// Maxima/minima
void SeqPlot::scanMinMax ()
    {
    data_max = data_min = prop[0].data[0] ;
    for ( int a = 0 ; a < prop[0].data.size() ; a++ )
        {
        for ( int b = 1 ; b < prop.size() ; b++ )
            {
            float d = prop[b].data[a] ;
            if ( data_max < d ) data_max = d ;
            if ( data_min > d ) data_min = d ;
            }
        }
    data_step = ( data_max - data_min ) / 5 ;
    if ( type == CHOU_FASMAN )
        {
        data_min = 0 ;
        data_max = 1.5 ;
        data_step = 0.2 ;
        }
    else if ( type == COILED_COIL )
        {
        data_min = 0 ;
        data_max = data_max * 11 / 10 ;
        if ( data_max > 1 ) data_max = 1 ;
        if ( data_max < 0.1 ) data_max = 0.1 ;
        data_step = 0.1 ;
        }
    else if ( type == M_W )
        {
        data_min = 0 ;
        data_max = 200 ;
        data_step = 50 ;
        }
    else if ( type == P_I )
        {
        data_min = 2.5 ;
        data_max = 11.5 ;
        data_step = 2.5 ;
        }
    else if ( type == H_P )
        {
        data_min = -4.5 ;
        data_max = 4.5 ;
        data_step = 1.5 ;
        }
    data_h = data_max - data_min ;
    }

void SeqPlot::scanChouFasman ( const int x , const int y , const int t , const int min , const int seek_cnt , const int seek_avg , const int avg )
    {
    for ( uint p = 0 ; p + y < s.length() ; p++ )
        {
        int cnt = 0 ;
        float avg2 = 0 ;
        for ( int i = 0 ; i < y ; i++ )
            {
            if ( (t==0?prop[p+i].cf_pa:prop[p+i].cf_pb) > min ) cnt++ ;
            avg2 += ( t==0?(float)prop[p+i].cf_pa:(float)prop[p+i].cf_pb ) ;
            }
        prop[p].data[t] = ( avg2 / (float) y ) / 100.0 ;
        if ( cnt < x ) continue ;
        if ( d1[t+1][p] == 'X' ) continue ;

        int from , to ;
        int sum = 255 ;
        for ( from = p ; from > 0 && sum >= seek_avg ; from-- )
           {
           sum = 0 ;
           for ( int j = 0 ; j < seek_cnt ; j++ )
               sum += (t==0?prop[from+j].cf_pa:prop[from+j].cf_pb) ;
           sum /= seek_cnt ;
           }
        sum = 255 ;
        for ( to = p+y-1 ; to+seek_cnt < s.length() && sum >= seek_avg ; to++ )
            {
            sum = 0 ;
            for ( int j = 0 ; j < seek_cnt ; j++ )
                sum += (t==0?prop[to+j].cf_pa:prop[to+j].cf_pb) ;
            sum /= seek_cnt ;
            }
        to += seek_cnt - 1 ;

        int avg0 = 0 , avg1 = 0 ;
        for ( int i = from ; i <= to ; i++ ) avg0 += prop[i].cf_pa ;
        for ( int i = from ; i <= to ; i++ ) avg1 += prop[i].cf_pb ;
        if ( ( t == 0 && to - from + 1 > avg && avg0 > avg1 ) || ( t == 1 && avg1 > avg && avg1 > avg0 ) )
            {
            for ( int i = from ; i <= to ; i++ ) d1[t+1][(uint)i] = 'X' ;
            }
        }
    }

void SeqPlot::useMW ()
    {
    type = M_W ;
    d1.Clear () ;
    d2.Clear () ;
    d3.Clear () ;
    if ( s.IsEmpty() ) return ;
    l_top = 4 ;
    l_bottom = 0 ;

    prop.clear () ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( TVector::getAAprop ( s.GetChar(a) ) ) ;
        prop[a].data.clear() ;
        prop[a].data.push_back ( prop[a].mw ) ;
//      while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
        }
    wxString x ;
    FILLSTRING ( x , ' ' , s.length() ) ;
    while ( d1.GetCount() < 4 ) d1.Add ( x ) ;
    scanMinMax () ;
    }

void SeqPlot::usePI ()
    {
    type = P_I ;
    d1.Clear () ;
    d2.Clear () ;
    d3.Clear () ;
    if ( s.IsEmpty() ) return ;
    l_top = 4 ;
    l_bottom = 0 ;

    prop.clear () ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( TVector::getAAprop ( s.GetChar(a) ) ) ;
        prop[a].data.clear() ;
        prop[a].data.push_back ( prop[a].pi ) ;
//      while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
        }
    wxString x ;
    FILLSTRING ( x , ' ' , s.length() ) ;
    while ( d1.GetCount() < 4 ) d1.Add ( x ) ;
    scanMinMax () ;
    }

void SeqPlot::useHP ()
    {
    type = H_P ;
    d1.Clear () ;
    d2.Clear () ;
    d3.Clear () ;
    if ( s.IsEmpty() ) return ;
    l_top = 4 ;
    l_bottom = 0 ;

    prop.clear () ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( TVector::getAAprop ( s.GetChar(a) ) ) ;
        prop[a].data.clear() ;
        }

    for ( int a = 0 ; a < s.length() ; a++ )
        {
        float avg = 0 ;
        for ( int b = 0 ; b < hp_window ; b++ )
            {
            int c = a - hp_window / 2 + b ;
            if ( c >= 0 && c < s.length() )
                {
                if ( hp_method == _T("kyte-doolittle") ) avg += prop[c].hp_kd ;
                if ( hp_method == _T("hopp-woods") ) avg += prop[c].hp_hw ;
                }
            }
        avg /= (float) hp_window ;

        prop[a].data.push_back ( avg ) ;
//      while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
        }
    wxString x ;
    FILLSTRING ( x , ' ' , s.length() ) ;
    while ( d1.GetCount() < 4 ) d1.Add ( x ) ;
    scanMinMax () ;
    }

