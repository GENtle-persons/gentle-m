#include "PlasmidCanvas.h"

// DRAWING

int PlasmidCanvas::deg2x ( const float& deg , const int& r ) const
{
    float f = sin ( (180-deg)*PI/180 ) * r ;
    return int ( f ) ;
}

int PlasmidCanvas::deg2y ( const float& deg , const int& r ) const
{
    float f = cos ( (180-deg)*PI/180 ) * r ;
    return int ( f ) ;
}

float PlasmidCanvas::xy2deg ( const float& x , const float& y ) const
{
    float f ;
    f = atan2(x,y) ;
    f = 180-f*180/PI ;
    return f ;
}

float PlasmidCanvas::xy2r ( const float& x , const float& y ) const
{
    float f ;
    f = sqrt(x*x+y*y) ;
    return f ;
}

void PlasmidCanvas::arrangeRestrictionSitesCircular ( wxDC &dc ) const
    {
    int l = p->vec->getSequenceLength() ;
    if ( p->vec->rc.size() == 0 ) return ;

    vector <TRestrictionCut> trc ;
    trc.reserve ( p->vec->rc.size() ) ;
    for ( int a = 0 ; a < p->vec->rc.size() ; a++ ) // Removing invisible
        {
        if ( p->vec->rc[a].isHidden ( p->vec ) )
           {
           trc.push_back ( p->vec->rc[a] ) ;
           p->vec->rc[a] = p->vec->rc[p->vec->rc.size()-1] ;
           p->vec->rc.pop_back() ;
           a-- ;
           }
        }

    p->vec->sortRestrictionSites() ;

    for ( int a = 0 ; a < p->vec->rc.size() ; a++ ) // Init
        {
        TRestrictionCut *c = &p->vec->rc[a] ;
        if ( c->isHidden ( p->vec ) ) continue ;
        float angle = (float) c->getPos() * 360 / l ;
        c->angle = angle ;
        c->angle3 = angle ; //(float) (a+1) * 360 / p->vec->rc.size() ;
        c->r1 = STANDARDRADIUS ;
        c->r2 = STANDARDRADIUS*22/20 ;
        c->r3 = STANDARDRADIUS*(22+zoom/50)/20 ;

        recalc_rc ( a , dc ) ;
        }

    for ( int a = 1 ; a < p->vec->rc.size() ; a++ ) // Ensure different angle
        {
        if ( p->vec->rc[a].isHidden ( p->vec ) ) continue ;
        if ( p->vec->rc[a-1].isHidden ( p->vec ) ) continue ;
        while ( p->vec->rc[a].angle3 <= p->vec->rc[a-1].angle3 )
           p->vec->rc[a].angle3 += 0.001 ;
        }

    bool redo = true ;
    int cnt = 200 ;
    while ( redo && cnt > 0 )
        {
        redo = false ;
        for ( int a = 0 ; a < p->vec->rc.size() ; a++ ) // Optimize
           redo |= optimizeCircularRestrictionSites ( a , dc ) ;
        cnt-- ;
        }

    // Appending hidden ones
    p->vec->rc.reserve ( trc.size() ) ;
    for ( int a = 0 ; a < trc.size() ; a++ )
        p->vec->rc.push_back ( trc[a] ) ;
    p->vec->sortRestrictionSites() ;
    }

bool PlasmidCanvas::optimizeCircularRestrictionSites ( const int a , wxDC &dc ) const
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    if ( p->vec->rc[a].isHidden ( p->vec ) ) return false ;
    bool ret = false ;
    if ( a > 0 && !p->vec->rc[a-1].isHidden ( p->vec ) &&
            intersects ( c->lastrect , p->vec->rc[a-1].lastrect ) )
        {
        push_rc_left ( a-1 , dc ) ;
        push_rc_right ( a , dc ) ;
        ret = true ;
        }
    if ( a < p->vec->rc.size()-1 && !p->vec->rc[a+1].isHidden ( p->vec ) &&
            intersects ( c->lastrect , p->vec->rc[a+1].lastrect ) )
        {
        push_rc_right ( a+1 , dc ) ;
        push_rc_left ( a , dc ) ;
        ret = true ;
        }
    return ret ;
    }

void PlasmidCanvas::push_rc_left ( const int a , wxDC &dc ) const
    {
    if ( p->vec->rc.size() < 2 ) return ;
    TRestrictionCut *c = &p->vec->rc[a] ;
    c->angle3 -= 1 ;
    recalc_rc ( a , dc ) ;
    float add = 0 ;
    int b = a - 1 ;
    if ( a == 0 ) { b = p->vec->rc.size()-1 ; add = 360 ; }
    TRestrictionCut *cl = &p->vec->rc[b] ;
    while ( c->angle3 + add < cl->angle3 ) push_rc_left ( b , dc ) ;
    }

void PlasmidCanvas::push_rc_right ( const int a , wxDC &dc ) const
    {
    if ( p->vec->rc.size() < 2 ) return ;
    TRestrictionCut *c = &p->vec->rc[a] ;
    c->angle3 += 1 ;
    recalc_rc ( a , dc ) ;
    float add = 0 ;
    int b = a + 1 ;
    if ( a == p->vec->rc.size()-1 ) { b = 0 ; add = 360 ; }
    TRestrictionCut *cr = &p->vec->rc[b] ;
    while ( c->angle3 > cr->angle3 + add ) push_rc_right ( b , dc ) ;
    }

void PlasmidCanvas::recalc_rc ( const int a, wxDC &dc ) const
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    wxPoint p2 ( deg2x ( c->angle , (int)c->r2 )+w/2 , deg2y ( c->angle , (int)c->r2 )+h/2 ) ;
    wxPoint p3 ( deg2x ( c->angle3 , (int)c->r3 )+w/2 , deg2y ( c->angle3 , (int)c->r3 )+h/2 ) ;
    c->lp = p3 ;
    makeLastRect ( a , dc ) ;
    }

void PlasmidCanvas::drawCircularORFs ( wxDC &dc ) const
    {
    int l = p->vec->getSequenceLength() ;
    for ( int a = 0 ; a < p->vec->countORFs() ; a++ )
        {
        float mf = p->vec->getORF(a)->get_from() ;
        float mt = p->vec->getORF(a)->get_to() ;
        int rf = p->vec->getORF(a)->get_rf() ;

        if ( mf > mt )
        {
        mt += l ;
        }

        float ro , roi = r / 10 ;
        ro = roi * 7 + roi * rf ;

        mf *= 360 ;
        mt *= 360 ;
        mf /= (float) l ;
        mt /= (float) l ;

        wxColour col ;
        if ( rf == 1 || rf == -1 ) col = wxColour ( 200 , 0 , 0 ) ;
        if ( rf == 2 || rf == -2 ) col = wxColour ( 0 , 200 , 0 ) ;
        if ( rf == 3 || rf == -3 ) col = wxColour ( 0 , 0 , 200 ) ;
        dc.SetBackgroundMode ( wxSOLID ) ;
        dc.SetBrush ( *MYBRUSH ( col ) ) ;
        dc.SetPen ( *MYPEN ( col ) ) ;

        int dir ;
        float mm , mn ;
        if ( rf < 0 )
           {
           dir = -1 ;
           mm = mf ;
           mn = mt ;
           }
        else
           {
           dir = 1 ;
           mm = mt ;
           mn = mf ;
           }
        dc.DrawCircle ( deg2x ( mn , (int)(r+ro/2) ) + w/2 ,
                        deg2y ( mn , (int)(r+ro/2) ) + h/2 ,
                        4 ) ;
        dc.DrawLine ( deg2x ( mm , (int)(r+ro/2) ) + w/2 ,
                      deg2y ( mm , (int)(r+ro/2) ) + h/2 ,
                      deg2x ( mm - dir*2 , (int)(r+ro/2-roi/4) ) + w/2 ,
                      deg2y ( mm - dir*2 , (int)(r+ro/2-roi/4) ) + h/2 ) ;
        dc.DrawLine ( deg2x ( mm , (int)(r+ro/2) ) + w/2 ,
                      deg2y ( mm , (int)(r+ro/2) ) + h/2 ,
                      deg2x ( mm - dir*2 , (int)(r+ro/2+roi/4) ) + w/2 ,
                      deg2y ( mm - dir*2 , (int)(r+ro/2+roi/4) ) + h/2 ) ;

        dc.SetBackgroundMode ( wxTRANSPARENT ) ;
        dc.SetBrush ( *wxTRANSPARENT_BRUSH ) ;
        dc.DrawEllipticArc ( (int)(w/2-r-ro/2) , (int)(h/2-r-ro/2) , (int)(r*2+ro) , (int)(r*2+ro) , 90-mt , 90-mf ) ;

        p->vec->getORF(a)->dist1 = r+ro/2-roi/4 ;
        p->vec->getORF(a)->dist2 = r+ro/2+roi/4 ;
        p->vec->getORF(a)->deg1 = mf;
        p->vec->getORF(a)->deg2 = mt ;
        if ( mt < mf ) p->vec->getORF(a)->deg2 = mt + 360 ;
        }
    dc.SetPen(*wxBLACK_PEN);
    }

void PlasmidCanvas::OnDrawCircular(wxDC& dc) /* not const */
{
    // Initial calculations
    char t[10000] ;
    wxCoord dx , dy ;
    int l = p->vec->getSequenceLength();
    int mwh ;
    mwh = w<h?w:h ;

    if ( printing ) mwh -= mwh / 5 ;

    int fontfactor = MYFONTSIZE * 10 / 8 ;
    if ( printing ) fontfactor = mwh/80;
    wxFont *tinyFont = MYFONT ( fontfactor*4/5 , wxFONTFAMILY_SWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ;
    wxFont *smallFont = MYFONT ( fontfactor , wxFONTFAMILY_SWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ;
    wxFont *normalFont = MYFONT ( fontfactor*6/5 , wxFONTFAMILY_SWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ;
    wxFont *bigFont = MYFONT ( fontfactor*7/5 , wxFONTFAMILY_SWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ;
//    wxFont *hugeFont = MYFONT ( fontfactor*9/5 , wxSWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_BOLD ) ;

    int d;
    for ( d = 1 ; d*10 < l ; d *= 10 ) ;
    r = (mwh*2/3)/2 ; // not const

    // Basic elements
    dc.SetPen(*wxBLACK_PEN);
    if ( printing ) dc.SetBrush ( *wxWHITE_BRUSH ) ;

    // Baseline
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    if ( p->vec->showGC() > 0 ) // %GC
        {
        int r1 = r + r/40 ;
        int r2 = r - r/40 ;
//      dc.DrawEllipse ( w/2-r - r/40 , h/2-r - r/40 , r*2 + r*2/40 , r*2 + r*2/40 ) ;
//      int dh = h / 80 ;
        int nob = p->vec->showGC() ;
        for ( int a = 0 ; a < nob ; a++ )
            {
            int at = 0 , gc = 0 , other = 0 ;
            for ( int b = l * a / nob ; b < l * ( a + 1 ) / nob ; b++ )
                {
                char c = p->vec->getSequenceChar ( b ) ;
                if ( c == 'A' || c == 'T' ) at++ ;
                else if ( c == 'G' || c == 'C' ) gc++ ;
                else other++ ;
                }
            int sum = at + gc + other ;
            if ( sum == 0 ) continue ;
            int per = gc * 100 / sum ;
            wxColour col ;
            makeGCcolor ( per , col ) ;
            dc.SetPen(*MYPEN(col));
            dc.SetBrush(*MYBRUSH(col));

            // Drawing polygon
            vector <wxPoint> p ;
            int b = a + 1 ;
            p.push_back ( wxPoint ( deg2x ( 360*a/nob , r1 ) , deg2y ( 360*a/nob , r1 ) ) ) ;
            for ( int c = int(360*a/nob) ; c%361 != int(360*b/nob)%361 ; c++ )
                {
                p.push_back ( wxPoint ( deg2x ( c , r1 ) , deg2y ( c , r1 ) ) ) ;
                }
            p.push_back ( wxPoint ( deg2x ( 360*b/nob , r1 ) , deg2y ( 360*b/nob , r1 ) ) ) ;
            p.push_back ( wxPoint ( deg2x ( 360*b/nob , r2 ) , deg2y ( 360*b/nob , r2 ) ) ) ;
            for ( int c = int(360*b/nob) ; c%361 != int(360*a/nob)%361 ; c-- )
                {
                p.push_back ( wxPoint ( deg2x ( c , r2 ) , deg2y ( c , r2 ) ) ) ;
                }
            p.push_back ( wxPoint ( deg2x ( 360*a/nob , r2 ) , deg2y ( 360*a/nob , r2 ) ) ) ;
            p.push_back ( wxPoint ( deg2x ( 360*a/nob , r1 ) , deg2y ( 360*a/nob , r1 ) ) ) ;
            wxPoint *wp ;
            wp = (wxPoint*) malloc ( sizeof ( wxPoint ) * (p.size()+1) ) ;
            for ( int b = 0 ; b < p.size() ; b++ ) wp[b] = p[b] ;
	    if (p.size()<2)
	        {
		wxPrintf("W: Not drawing polygon with <2  corners - A\n");
                }
	    else
	        {
                dc.DrawPolygon ( p.size() , wp , w/2 , h/2 ) ;
	        }
            free ( wp ) ;

            dc.SetPen(*wxBLACK_PEN);
            dc.DrawLine ( w/2+p[0].x , h/2+p[0].y , w/2+p[1].x , h/2+p[1].y ) ;
            dc.DrawLine ( w/2+p[2].x , h/2+p[2].y , w/2+p[3].x , h/2+p[3].y ) ;
            }
        showGClegend ( dc ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBackgroundMode ( wxTRANSPARENT ) ;
        dc.SetBrush ( *wxWHITE_BRUSH ) ;
//      dc.DrawEllipse ( w/2-r + r/40 , h/2-r + r/40 , r*2 - r*2/40 , r*2 - r*2/40 ) ;
        }
    else
        {
        dc.DrawEllipse ( w/2-r , h/2-r , r*2 , r*2 ) ;
        }
    dc.SetBackgroundMode ( wxSOLID ) ;

    // Marking
    if ( getMarkFrom() != -1 )
        {
        float mt = getMarkFrom() ;
        float mf = getMarkTo() ;
        mt /= (float) l ;
        mf /= (float) l ;
        mt *= 360 ;
        mf *= 360 ;
        mt = 90 - mt ;
        mf = 90 - mf ;
        dc.SetBackgroundMode ( wxSOLID ) ;
        dc.SetBrush ( *wxLIGHT_GREY_BRUSH ) ;
        if ( getMarkFrom() == getMarkTo() ) mf = mt - 0.1 ;
        else if ( mf > mt ) { float mm = mf ; mf = mt ; mt = mm ; }
        int r1 = p->vec->showGC() > 0 ? r - r/40 : r ;
        dc.DrawEllipticArc ( w/2-r1 , h/2-r1 , r1*2 , r1*2 , mf , mt ) ;
        dc.DrawLine ( w/2 , h/2 , deg2x(90-mf,r1)+w/2 , deg2y(90-mf,r1)+h/2 ) ;
        dc.DrawLine ( w/2 , h/2 , deg2x(90-mt,r1)+w/2 , deg2y(90-mt,r1)+h/2 ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBackgroundMode ( wxTRANSPARENT ) ;
        }

    // Show title
    if ( myapp()->frame->showVectorTitle )
        {
        char *c1 , *c2 ;
        strcpy ( t , p->vec->getName().mb_str() ) ;
        strcat ( t , " " ) ;
        int a = 0 ;
        dc.SetFont ( *bigFont ) ;
        dc.GetTextExtent ( wxString ( t , *wxConvCurrent ) , &dx , &dy ) ;
        for ( c1 = t ; *c1 ; c1++ )
            {
            if ( *c1 == ' ' ) a -= dy ;
            }
        for ( c1 = c2 = t ; *c1 ; c1++ )
            {
            if ( *c1 == ' ' )
                {
                *c1 = 0 ;
                wxString t2 ( c2 , *wxConvCurrent ) ;
                dc.GetTextExtent ( t2 , &dx , &dy ) ;
                dc.DrawText ( t2 , w/2-dx/2 , h/2-dy/2+a ) ;
                c2 = c1+1 ;
                a += dy ;
                }
            }
        }

    wxString t2 ;
    // Show length
    if ( myapp()->frame->showVectorLength )
        {
        t2 = wxString::Format ( txt("#bp") , p->vec->getSequenceLength() ) ;
//      sprintf ( t , txt("#bp") , p->vec->getSequenceLength() ) ;
        dc.SetFont ( *normalFont ) ;
        dc.GetTextExtent ( t2 , &dx , &dy ) ;
        dc.DrawText ( t2 , w/2-dx/2 , h/2-dy/2 ) ;
        }

    // Numbers
    dc.SetFont(*smallFont);
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;

    for ( int a = 0 ; a <= l ; a += d ) // d initialized at very beginning of function
        {
        float deg = a*360/l ;
        int r1 = r*17/20 ;
        int r2 = r*20/20 ;
        dc.DrawLine ( deg2x ( deg , r1 ) + w/2 ,
                      deg2y ( deg , r1 ) + h/2 ,
                      deg2x ( deg , r2 ) + w/2 ,
                      deg2y ( deg , r2 ) + h/2 ) ;

        // Numbers
        t2 = wxString::Format ( _T("%d") , a ) ;
//      sprintf ( t , "%d" , a ) ;
        dc.GetTextExtent ( t2 , &dx , &dy ) ;
        if ( deg > 180 && deg < 350 ) dx = 0 ;
        if ( deg < 15 ) dx = 0 ;
        if ( deg < 90 || deg > 270 ) dy = 0 ;
        if ( a > 0 ) dc.DrawText ( t2 , deg2x ( deg , r1 ) + w/2 - dx , deg2y ( deg , r1 ) + h/2 - dy ) ;
        }

    // Methylation sites
    dc.SetPen(*wxRED_PEN);
    for ( int a = 0 ; a < p->vec->countMethylationSites() ; a++ )
        {
        float deg = p->vec->getMethylationSite(a)*360/l ;
        int r1 = r*19/20 ;
        int r2 = r*20/20 ;
        dc.DrawLine ( deg2x ( deg , r1 ) + w/2 ,
                      deg2y ( deg , r1 ) + h/2 ,
                      deg2x ( deg , r2 ) + w/2 ,
                      deg2y ( deg , r2 ) + h/2 ) ;
        }
    dc.SetPen(*wxBLACK_PEN);

    if ( p->vec->displayUpdate() )
        {
        // Items (Genes etc.)
        for ( int a = 0 ; a < p->vec->items.size() ; a++ )
            {
            TVectorItem *i = &p->vec->items[a] ;
            float df = i->from*360.0/((float)l) ;
            float dt = i->to*360.0/((float)l) ;
            if ( dt < df ) dt += 360.0 ;
//            float dd = (dt-df)*5.0/100.0 ; // Arrow length in degrees
//            float ds = 1 ; // Paint steps for genes, in degrees
            int tf = 19 ;
            tf += ( i->type % 3 ) * 2 - 2 ;
            int r1 = STANDARDRADIUS*tf/20 ;
            int r2 = STANDARDRADIUS*(tf+2)/20 ;
            if ( i->r1 == -1 || i->r2 == -1 )
               {
               i->r1 = r1 ;
               i->r2 = r2 ;
               }
            i->a1 = df ;
            i->a2 = dt ;
            }

        // Restriction sites
        dc.SetFont ( *tinyFont ) ;
        arrangeRestrictionSitesCircular ( dc ) ;
        dc.SetFont ( *smallFont ) ;

        // ORFs
        if ( p->showORFs )
           {
           p->showORFs = false ;
           wxCommandEvent event ;
           p->OnORFs ( event ) ;
           }

        p->vec->updateDisplay ( false ) ;
        }

    // ORFs
    drawCircularORFs ( dc ) ;

    // Drawing items
    for ( int a = 0 ; a < p->vec->items.size() ; a++ )
        {
        TVectorItem *i = &p->vec->items[a] ;
        if ( i->isVisible() )
            {
            int r1 , r2 ;
            float df , dt ;
            r1 = i->r1 * 100 * r / ( STANDARDRADIUS * 100 ) ;
            r2 = i->r2 * 100 * r / ( STANDARDRADIUS * 100 ) ;
            df = i->a1 ;
            dt = i->a2 ;

            float dd = (dt-df)*5/100 ; // Arrow length in degrees
            float ds = 1 ; // Paint steps for genes, in degrees

            // Calculating polygon
            vector <wxPoint> p ;

            if ( i->getParam ( _T("AUTOMATIC") ) == _T("SEQUENCING PRIMER") )
                dd = ( dt - df ) / 2 ;

            if ( i->direction ==  1 && abs ( (int) ( dt - df ) ) > 3 )
                {
                dt -= dd ;
                p.push_back ( wxPoint ( deg2x ( df+dd , (r1+r2)/2 ) ,
                                        deg2y ( df+dd , (r1+r2)/2 ) ) ) ;
                }
            if ( i->direction == -1 )
                {
                df += dd ;
                p.push_back ( wxPoint ( deg2x ( df-dd , (r1+r2)/2 ) ,
                                        deg2y ( df-dd , (r1+r2)/2 ) ) ) ;
                }

            p.reserve ( (int)(( dt - df ) / ds * 2 + 10) ) ;
            for ( float b = df ; b < dt ; b += ds )
                    p.push_back ( wxPoint ( deg2x ( b , r1 ) , deg2y ( b , r1 ) ) ) ;


            if ( i->direction != 0 )
                p.push_back ( wxPoint ( deg2x ( dt+dd*i->direction , (r1+r2)/2 ) , deg2y ( dt+dd*i->direction , (r1+r2)/2 ) ) ) ;

            for ( float b = dt ; b > df ; b -= ds )
                    p.push_back ( wxPoint ( deg2x ( b , r2 ) , deg2y ( b , r2 ) ) ) ;

            // Drawing polygon
            wxPoint *wp ;
            wp = (wxPoint*) malloc ( sizeof ( wxPoint ) * (p.size()+1) ) ;
            for ( int b = 0 ; b < p.size() ; b++ ) wp[b] = p[b] ;
            if ( !this->p->vec->getGenomeMode() ) dc.SetPen(*wxBLACK_PEN);
            else dc.SetPen ( *MYPEN((wxColour)i->getBrush()->GetColour()) ) ;
            dc.SetBrush ( *(i->getBrush()) ) ;
	    if (p.size() < 2)
	        {
		wxPrintf("W: Not drawing polygon with <2 corners - B - attempted to draw item '%s' (%s)\n", i->name, i->desc ) ; 
		}
	    else
	        {
                dc.DrawPolygon ( p.size() , wp , w/2 , h/2 ) ;
	        }
            free ( wp ) ;
            }
        }

    for ( int a = 0 ; !p->vec->getGenomeMode() && a < p->vec->items.size() ; a++ ) // Item titles
        {
        TVectorItem *i = &p->vec->items[a] ;
        if ( i->isVisible() && i->getParam ( _T("PREDECESSOR") ).IsEmpty() )
            {
            int r1 = i->r1 * 100 * r / ( STANDARDRADIUS * 100 ) ;
//          int r2 = i->r2 * 100 * r / ( STANDARDRADIUS * 100 ) ;
            float df = i->a1 ;
            float dt = i->a2 ;

            float dd = (dt-df)*5/100 ; // Arrow length in degrees
//          float ds = 1 ; // Paint steps for genes, in degrees
            // Drawing name
            wxColor fc = dc.GetTextForeground () ;
            dc.SetTextForeground ( i->getFontColor() ) ;
            dc.SetFont(*normalFont);
            dc.GetTextExtent ( i->name , &dx , &dy ) ;
            dd = (df+(dt-df)/2) ;
            while ( dd >= 360 ) dd -= 360 ;
            if ( dd > 180 ) dx = 0 ;
            if ( dd < 90 || dd > 270 ) dy = 0 ;
            dc.DrawText ( i->name ,
                          w/2+deg2x(dd,r1-2)-dx ,
                          h/2+deg2y(dd,r1-2)-dy ) ;
            dc.SetTextForeground ( fc ) ;
            dc.SetPen(*wxBLACK_PEN);
            }
        }

    // Restriction sites
    dc.SetFont ( *tinyFont ) ;
    for ( int a = 0 ; a < p->vec->rc.size() ; a++ )
        {
        TRestrictionCut c = p->vec->rc[a] ;
        if ( isEnzymeVisible ( c.e->getName() ) )
           {
           wxColour *col = p->vec->getEnzymeRule()->getColor ( p->vec->countCuts ( c.e->getName() ) ) ;
           dc.SetTextForeground ( *col ) ;
           dc.SetPen ( *MYPEN(*col) ) ;
           c.r1 = c.r1 * 100 * r / ( STANDARDRADIUS * 100 ) ;
           c.r2 = c.r2 * 100 * r / ( STANDARDRADIUS * 100 ) ;
           c.r3 = c.r3 * 100 * r / ( STANDARDRADIUS * 100 ) ;
           wxPoint p1 ( deg2x ( c.angle , (int)c.r1 )+w/2 , deg2y ( c.angle , (int)c.r1 )+h/2 ) ;
           wxPoint p2 ( deg2x ( c.angle , (int)c.r2 )+w/2 , deg2y ( c.angle , (int)c.r2 )+h/2 ) ;
           dc.DrawLine ( p1 , p2 ) ;
           wxPoint p3 = p->vec->rc[a].lp ;

           p3 = makeLastRect ( a , dc ) ;
           if ( p3.x < w/2 ) p3.x = p->vec->rc[a].lastrect.GetRight() ;
           p3.y = ( p->vec->rc[a].lastrect.GetTop() + p->vec->rc[a].lastrect.GetBottom() ) / 2 ;

           dc.DrawLine ( p2 , p3 ) ;

           wxString u = c.getNameAndPosition () ;
//         char u[100] ;
//         sprintf ( u , "%s (%d)" , c.e->name.c_str() , c.pos ) ;
           p3.x = p->vec->rc[a].lastrect.GetLeft() ;
           p3.y = p->vec->rc[a].lastrect.GetTop() ;

           dc.DrawText ( u , p3.x , p3.y ) ;
           }
        }
    dc.SetFont(*smallFont);
    dc.SetPen ( *wxBLACK_PEN ) ;
    dc.SetTextForeground ( *wxBLACK ) ;
    }

int PlasmidCanvas::findORFcircular ( const float& angle , const float& radius ) const
    {
    int found = -1 ;
    for ( int a = 0 ; a < p->vec->countORFs() ; a++ )
        {
        if ( angle >= p->vec->getORF(a)->deg1 &&
             angle <= p->vec->getORF(a)->deg2 &&
             radius >= p->vec->getORF(a)->dist1 &&
             radius <= p->vec->getORF(a)->dist2 )
             found = a ;
        if ( p->vec->getORF(a)->deg2 > 360 &&
             angle <= p->vec->getORF(a)->deg2-360 &&
             radius >= p->vec->getORF(a)->dist1 &&
             radius <= p->vec->getORF(a)->dist2 )
             found = a ;
        }
    return found ;
    }

wxPoint PlasmidCanvas::makeLastRect ( const int a , wxDC &dc ) const
    {
    wxPoint p3 = p->vec->rc[a].lp ;
    p3.x = p3.x * 100 * r / ( STANDARDRADIUS * 100 ) + w/2 ;
    p3.y = p3.y * 100 * r / ( STANDARDRADIUS * 100 ) + h/2 ;

//  char u[100] ;
//  sprintf ( u , "%s (%d)" , p->vec->rc[a].e->name.c_str() , p->vec->rc[a].pos ) ;
    wxString u = p->vec->rc[a].getNameAndPosition () ;
    int te_x , te_y ;
    dc.GetTextExtent ( u , &te_x , &te_y ) ;
    p3.y -= te_y / 2 ;
//    if ( p3.x < w/2 ) p3.x -= te_x + 4 ;
    if ( p->vec->rc[a].angle3 > 180 ) p3.x -= te_x + 4 ;
    else p3.x += 4 ;
    p->vec->rc[a].lastrect = wxRect ( p3.x , p3.y , te_x , te_y ) ;

    return p3 ;
    }

int PlasmidCanvas::circular_pos ( const float& angle ) const
    {
    int l = p->vec->getSequenceLength() ;
    float a = l ;
    a /= 360.0 ;
    a *= angle ;
    int b = (int) a ;
    if ( b < 0 ) b = 0 ;
    if ( b >= l ) b = l - 1 ;
    return b+1 ;
    }

