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
    int a , x , y , w , h , l = 0 , bo = can->border , lowy = 0 ;
    int lasta = 0 ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    endnumberlength = 0 ;
    int ox = bo+wx , oy = n*wy+bo , endnumber = offset + s.length() ;
    while ( endnumber > 0 ) { endnumber /= 10 ; ox += wx ; endnumberlength++ ; }

    if ( can->isMiniDisplay() ) can->MyGetClientSize ( &w , &h ) ;
    else can->MyGetSize ( &w , &h ) ;
    w -= 20 ; // Scrollbar dummy
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
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( can->isMiniDisplay() && can->getAA() && can->getAA()->miniDisplayOptions == MINI_DISPLAY_CONDENSED )
           x = ox + ( w - ox ) * a / s.length() ;
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


void SeqPlot::drawDottedLine ( wxDC &dc , int x1 , int y1 , int x2 , int y2 )
    {
    if ( can->isPrinting() )
        {
        dc.DrawLine ( x1 , y1 , x2 , y2 ) ;
        return ;
        }
    int i ;
    if ( x1 == x2 )
        {
        for ( i = y1 ; i <= y2 ; i++ )
           if ( i & 1 ) dc.DrawPoint ( x1 , i ) ;
        }
    else
        {
        for ( i = x1 ; i <= x2 ; i++ )
           if ( i & 1 ) dc.DrawPoint ( i , y1 ) ;
        }
    }
    
void SeqPlot::myRect ( wxDC &dc , int x , int y , int w , int h )
    {
    if ( can->isPrinting() )
        {
        for ( int i = 0 ; i <= h ; i++ )
           dc.DrawLine ( x , y+i , x+w , y+i ) ;
        return ;
        }
    for ( int i = 0 ; i < w ; i++ )
        {
        for ( int j = 0 ; j < h ; j++ )
           {
           if ( ( x+i + y+j ) & 1 ) dc.DrawPoint ( x + i , y + j ) ;
           }
        }
    }
    
void SeqPlot::fixMinMax ( float &f )
    {
    f = f > data_max ? data_max : f ;
    f = f < data_min ? data_min : f ;
    }
    
void SeqPlot::show ( wxDC& dc )
    {
    if ( s.IsEmpty() ) return ;
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
    int xa , xb , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    xa = -xa ;
    can->MyGetClientSize ( &xb , &yb ) ;
    yb += ya ;
    xb += xa ;
    int lx = 0 ;
    startOfLine = true ;
    for ( a = 0 ; a < pos.p.GetCount() ; a++ )
        {
        if ( can->hardstop > -1 && a > can->hardstop ) break ;
        b = pos.p[a] ;
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
		   if ( !can->getPrintToColor() )
		   {
		       dc.SetBackgroundMode ( wxTRANSPARENT ) ;
		       dc.SetTextForeground ( *wxBLACK ) ;
		   }
	       }
           switch ( type )
              {
              case CHOU_FASMAN : showChouFasman ( dc , b-1 , tx , ty , lx ) ; break ;
              case M_W : showMW ( dc , b-1 , tx , ty , lx ) ; break ;
              case P_I : showPI ( dc , b-1 , tx , ty , lx ) ; break ;
              case H_P : showHP ( dc , b-1 , tx , ty , lx ) ; break ;
              }
           lx = tx + can->charwidth ;
           cnt++ ;
           startOfLine = false ;
           }
        else if ( insight ) // Front
           {           
           lx = 0 ;
           startOfLine = true ;
           if ( !can->isMiniDisplay() ) continue ;
           dc.SetFont(*can->smallFont);
           if ( type == CHOU_FASMAN ) t = _T("Chou-Fasman") ;
           else if ( type == M_W ) t = _T("MW") ;
           else if ( type == P_I ) t = _T("pI") ;
           else if ( type == H_P )
              {
              t = _T("t_method_") ;
              t += hp_method ;
              t = txt(t) ;
              t += wxString::Format( _T(" [%d]") , hp_window ) ;
              }
           dc.SetTextForeground ( *wxBLACK ) ;
           int tw , th ;
           dc.GetTextExtent ( t , &tw , &th ) ;
           int ty = pos.r[a].y ;
           ty += lines * can->charheight ;
#ifdef __WXGTK__
	   ty += th / 2 ;
	   dc.DrawText ( t , pos.r[a].x , ty ) ;
#else
           ty -= ( lines * can->charheight - tw ) / 2 ;
           dc.DrawRotatedText ( t , pos.r[a].x, ty , 90 ) ;
#endif
           dc.SetFont(*can->font);
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }
    
void SeqPlot::showMW ( wxDC &dc , int b , int tx , int ty , int lx )
    {
    int ch = can->charheight / 2 ;
    int cw = can->charwidth ;
    int bottom = ty + lines * can->charheight ;
    int u ;
    for ( u = 1 ; u < d1.GetCount() ; u++ )
        {
        int tz = ty + (u-1) * ch + 1 ;
        int tw = ( tx + cw ) - lx ;
        
        if ( getMark ( b+1 ) > 0 )
           {
           dc.SetPen ( *wxGREY_PEN ) ;
           myRect ( dc , lx , tz , tw , ch*can->charheight - 2 ) ;
           }
        }

    showPlot ( dc , b , tx , ty , lx , bottom - ty - ch ) ;
    }
    
void SeqPlot::showPI ( wxDC &dc , int b , int tx , int ty , int lx )
    {
    // All the same...
    showMW ( dc , b , tx , ty , lx ) ;
    }
    
void SeqPlot::showHP ( wxDC &dc , int b , int tx , int ty , int lx )
    {
    // All the same...
    showMW ( dc , b , tx , ty , lx ) ;
    }
    
void SeqPlot::showChouFasman ( wxDC &dc , int b , int tx , int ty , int lx )
    {
    int bottom = ty + lines * can->charheight ;
    int ch = can->charheight / 2 ;
    int cw = can->charwidth ;
    int u ;
    for ( u = 1 ; u < d1.GetCount() ; u++ )
        {
        wxPen *pen = wxRED_PEN ;
        if ( u == 2 ) pen = wxGREEN_PEN ;
        if ( u == 3 ) pen = MYPEN("BLUE") ;
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
           if ( u == 1 ) drawSymbol ( 'a' , dc , lx-cw , ty+(u-1)*ch , lx-2 , ty+(u-1)*ch+ch ) ; // Alpha
           else if ( u == 2 ) drawSymbol ( 'b' , dc , lx-cw , ty+(u-1)*ch , lx-2 , ty+(u-1)*ch+ch ) ; // Beta
           else drawSymbol ( 'T' , dc , lx-cw , ty+(u-1)*ch , lx-2 , ty+(u-1)*ch+ch ) ; // Turn
           }
        }
    
    showPlot ( dc , b , tx , ty , lx , bottom - d1.GetCount() * ch - ty ) ;
    }
        
    
void SeqPlot::showPlot ( wxDC &dc , int b , int tx , int ty , int lx , int ph )
    {    
    if ( !can->isMiniDisplay() ) return ;
    int ch = can->charheight ;
    int cw = can->charwidth ;
    int bottom = ty + lines * can->charheight ;
    int u ;
    
    if ( can->isMiniDisplay() )
        {
        if ( b == 0 ) plotr = wxRect ( lx , bottom - ph , 0 , ph ) ;
        plotr.SetWidth ( tx + cw - plotr.GetLeft() ) ;
        }
    
    // Dotted lines
    dc.SetPen ( *wxBLACK_PEN ) ;
    for ( float j = data_min + data_step ; j + data_step <= data_max ; j += data_step )
        {
        int k = bottom - ( (int)(j - data_min) ) * ph / ((int)data_h) ;
        drawDottedLine ( dc , lx , k , tx + cw , k ) ;
        }
        
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
    for ( u = 0 ; u < prop[b].data.size() ; u++ )
        {
        wxPen *pen = wxRED_PEN ;
        if ( can->isPrinting() ) pen = wxBLACK_PEN ;
        else if ( u == 1 ) pen = wxGREEN_PEN ;
        else if ( u == 2 ) pen = MYPEN("BLUE") ;
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
    if ( b + 1 == s.length () )
        {
        dc.DrawLine ( tx+cw , bottom - ph , tx+cw , bottom ) ;
        }
    }
    
void SeqPlot::drawSymbol ( char c , wxDC &dc , int x1 , int y1 , int x2 , int y2 )
    {
    dc.SetPen(*wxBLACK_PEN);
    int w = x2 - x1 ;
    int h = y2 - y1 ;
    if ( c == 'T' )
        {
        x2--; w--; y1++; h--;
        int y3 = y2 - h/4 ;
        dc.DrawLine ( x1 , y1 , x2 - w/3 , y1 ) ;
        dc.DrawLine ( x1 , y3 , x2 - w/3 , y3 ) ;
        dc.DrawArc ( x2 - w/3 , y3 , x2 - w/3 , y1 , x2 - w/3 , (y1+y3)/2 ) ;
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine ( x2 - w/3 , y3-1 , x2 - w/3 , y1+1 ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine ( x1 , y3 , x1 + w*2/3 , y3 - h/2 ) ;
        dc.DrawLine ( x1 , y3 , x1 + w*2/3 , y3 + h/2 ) ;
        }
    else if ( c == 'b' )
        {
        x1 += w/2 - h/6 ;
        dc.DrawLine ( x1 , y1 , x1 , y2 ) ;
        dc.DrawArc ( x1 , y1+h/3 , x1 , y1 , x1 , y1+h/6 ) ;
        dc.DrawArc ( x1 , y1+h*2/3 , x1 , y1+h/3 , x1 , y1+h*3/6 ) ;
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

void SeqPlot::init ( SequenceCanvas *ncan )
    {
    SeqBasic::init ( ncan ) ;
    hp_window = 7 ;
    hp_method = _T("kyte-doolittle") ;
    }

void SeqPlot::initFromTVector ( TVector *v )
    {
    vec = v ;
    s = vec->getSequence() ;

    if ( can && can->getEditMode() && v->getSequenceLength() &&
            v->getSequenceChar(v->getSequenceLength()-1) == ' ' )
       {
       s.Truncate ( s.length()-1 ) ;
       }

    takesMouseActions = true ;
    }

void SeqPlot::setLines ( int l )
    {
    int a ;
    wxArraySeqBasic v2 ;
    while ( can->seq[can->seq.GetCount()-1] != this )
        {
        v2.Add ( can->seq[can->seq.GetCount()-1] ) ;
        can->seq.RemoveAt ( can->seq.GetCount()-1 ) ;
        }
    for ( a = 0 ; a < l ; a++ )
        {
        SeqBlank *bl = new SeqBlank ( can ) ;
        bl->initFromTVector ( vec ) ;
        can->seq.Add ( bl ) ;
        }
    for ( a = lines ; a < v2.GetCount() ; a++ ) can->seq.Add ( v2[a] ) ;
    for ( a = 0 ; a < lines ; a++ ) delete v2[a] ;

    lines = l ;    
    }


// Calculation routines

// See http://prowl.rockefeller.edu/aainfo/chou.htm for background
void SeqPlot::useChouFasman ()
    {
    type = CHOU_FASMAN ;
    d1.Clear () ;
    d2.Clear () ;
    d3.Clear () ;
    if ( s.IsEmpty() ) return ;
    l_top = 4 ;
    l_bottom = 0 ;
//    if ( l_top + l_bottom + 1 > lines ) setLines ( l_top + l_bottom + 1 ) ;
    
    unsigned int a ;
    prop.clear () ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( vec->getAAprop ( s[a] ) ) ;
        prop[a].data.clear() ;
        while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
        }
    wxString x ;
    FILLSTRING ( x , ' ' , s.length() ) ;
    while ( d1.GetCount() < 4 ) d1.Add ( x ) ;
    scanChouFasman ( 4 , 6 , 0 , 100 , 4 , 100 , 5 ) ; // Alpha helices
    scanChouFasman ( 3 , 5 , 1 , 100 , 4 , 100 , 105 ) ; // Beta sheets
    
    // Deciding on overlapping regions
    for ( a = 0 ; a < s.length() ; a++ )
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
    for ( a = 0 ; a + 3 < s.length() ; a++ )
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
        
    scanMinMax () ;
    }
    
// Maxima/minima
void SeqPlot::scanMinMax ()
    {
    int a ;
    data_max = data_min = prop[0].data[0] ;
    for ( a = 0 ; a < prop[0].data.size() ; a++ )
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
    
void SeqPlot::scanChouFasman ( int x , int y , int t , int min , 
                                    int seek_cnt , int seek_avg , int avg )
    {
    uint p ;
    for ( p = 0 ; p + y < s.length() ; p++ )
        {
        int i , cnt = 0 ;
        float avg2 = 0 ;
        for ( i = 0 ; i < y ; i++ )
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
        for ( i = from ; i <= to ; i++ ) avg0 += prop[i].cf_pa ;
        for ( i = from ; i <= to ; i++ ) avg1 += prop[i].cf_pb ;
        if ( ( t == 0 && to - from + 1 > avg && avg0 > avg1 ) ||
             ( t == 1 && avg1 > avg && avg1 > avg0 ) )
           {
           for ( i = from ; i <= to ; i++ ) d1[t+1][(uint)i] = 'X' ;
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
    
    int a ;
    prop.clear () ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( vec->getAAprop ( s.GetChar(a) ) ) ;
        prop[a].data.clear() ;
        prop[a].data.push_back ( prop[a].mw ) ;
//        while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
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
    
    int a ;
    prop.clear () ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( vec->getAAprop ( s.GetChar(a) ) ) ;
        prop[a].data.clear() ;
        prop[a].data.push_back ( prop[a].pi ) ;
//        while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
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
    
    int a ;
    prop.clear () ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        prop.push_back ( vec->getAAprop ( s.GetChar(a) ) ) ;
        prop[a].data.clear() ;
        }
        
    for ( a = 0 ; a < s.length() ; a++ )
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
//        while ( prop[a].data.size() < 3 ) prop[a].data.push_back ( 0 ) ;
        }
    wxString x ;
    FILLSTRING ( x , ' ' , s.length() ) ;
    while ( d1.GetCount() < 4 ) d1.Add ( x ) ;
    scanMinMax () ;
    }
    
