#include "PlasmidCanvas.h"

// DRAWING

int PlasmidCanvas::deg2x ( float deg , int r )
{
    float f = sin ( (180-deg)*PI/180 ) * r ;
    return int ( f ) ;
}

int PlasmidCanvas::deg2y ( float deg , int r )
{
    float f = cos ( (180-deg)*PI/180 ) * r ;
    return int ( f ) ;
}

float PlasmidCanvas::xy2deg ( float x , float y )
{
    float f ;
    f = atan2(x,y) ;
    f = 180-f*180/PI ;
    return f ;
}

float PlasmidCanvas::xy2r ( float x , float y )
{
    float f ;
    f = sqrt(x*x+y*y) ;
    return f ;
}

void PlasmidCanvas::arrangeRestrictionSitesCircular ( wxDC &dc )
    {
    int a ;
    int l = p->vec->sequence.length() ;
    if ( p->vec->rc.size() == 0 ) return ;
    
    for ( a = 1 ; a < p->vec->rc.size() ; a++ ) // Sort by pos
        {
        if ( p->vec->rc[a].pos < p->vec->rc[a-1].pos )
           {
           TRestrictionCut dummy = p->vec->rc[a] ;
           p->vec->rc[a] = p->vec->rc[a-1] ;
           p->vec->rc[a-1] = dummy ;
           a = 0 ;
           }
        }
    
    for ( a = 0 ; a < p->vec->rc.size() ; a++ ) // Init
        {
        TRestrictionCut *c = &p->vec->rc[a] ;
        float angle = (float) c->pos * 360 / l ;
        c->angle = angle ;
        c->angle3 = angle ; //(float) (a+1) * 360 / p->vec->rc.size() ;
        c->r1 = STANDARDRADIUS ;
        c->r2 = STANDARDRADIUS*22/20 ;
        c->r3 = STANDARDRADIUS*(22+zoom/50)/20 ;
        
        recalc_rc ( a , dc ) ;
        }

    for ( a = 1 ; a < p->vec->rc.size() ; a++ ) // Ensure different angle
        {
        while ( p->vec->rc[a].angle3 <= p->vec->rc[a-1].angle3 )
           p->vec->rc[a].angle3 += 0.001 ;
        }
    
    bool redo = true ;
    int cnt = 0 ;
    while ( redo && cnt < 200 )
        {
        redo = false ;    
        for ( a = 0 ; a < p->vec->rc.size() ; a++ ) // Optimize
           redo |= optimizeCircularRestrictionSites ( a , dc ) ;
        }
        
    }
    
bool PlasmidCanvas::optimizeCircularRestrictionSites ( int a , wxDC &dc )
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    bool ret = false ;
    if ( a > 0 && intersects ( c->lastrect , p->vec->rc[a-1].lastrect ) )
        {
        push_rc_left ( a-1 , dc ) ; 
        push_rc_right ( a , dc ) ; 
        ret = true ; 
        }
    if ( a < p->vec->rc.size()-1 && intersects ( c->lastrect , p->vec->rc[a+1].lastrect ) )
        {
        push_rc_right ( a+1 , dc ) ; 
        push_rc_left ( a , dc ) ; 
        ret = true ; 
        }
    return ret ;
    }
    
void PlasmidCanvas::push_rc_left ( int a , wxDC &dc )
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    c->angle3 -= 1 ;
    recalc_rc ( a , dc ) ;
    if ( a == 0 ) return ;
    TRestrictionCut *cl = &p->vec->rc[a-1] ;
    while ( c->angle3 < cl->angle3 ) push_rc_left ( a-1 , dc ) ;
    }
    
void PlasmidCanvas::push_rc_right ( int a , wxDC &dc )
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    c->angle3 += 1 ;
    recalc_rc ( a , dc ) ;
    if ( a == p->vec->rc.size()-1 ) return ;
    TRestrictionCut *cr = &p->vec->rc[a+1] ;
    while ( c->angle3 > cr->angle3 ) push_rc_right ( a+1 , dc ) ;
    }
    
void PlasmidCanvas::recalc_rc ( int a, wxDC &dc )
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    wxPoint p2 ( deg2x ( c->angle , c->r2 )+w/2 , deg2y ( c->angle , c->r2 )+h/2 ) ;
    wxPoint p3 ( deg2x ( c->angle3 , c->r3 )+w/2 , deg2y ( c->angle3 , c->r3 )+h/2 ) ;
    c->p = p3 ;
    makeLastRect ( a , dc ) ;    
    }
    
void PlasmidCanvas::arrangeRestrictionSite ( int q , wxDC &dc )
    {
    TVector *v = p->vec ;
    int diff = 100 ;
    int dx , dy ;
    int a ;
    bool doOverlap ;
    do {
        doOverlap = false ;
        for ( a = 0 ; a < v->rc.size() && !doOverlap ; a++ )
           {
           if ( a != q && intersects ( v->rc[a].lastrect , v->rc[q].lastrect ) )
              {
              doOverlap = true ;
              
              dx = deg2x ( v->rc[q].angle , 10000 ) ;
              dy = deg2y ( v->rc[q].angle , 10000 ) ;
              dx = dx > 0 ? diff : -diff ;
              dy = dy > 0 ? diff : -diff ;

              v->rc[q].p.x += dx ;
              v->rc[q].p.y += dy ;
              makeLastRect ( q , dc ) ;
              }
           }
       } while ( doOverlap ) ;
    }
    
void PlasmidCanvas::drawCircularORFs ( wxDC &dc )
    {
    int a ;
    int l = p->vec->sequence.length() ;
    for ( a = 0 ; a < p->vec->worf.size() ; a++ )
        {
        float mf = p->vec->worf[a].from ;
        float mt = p->vec->worf[a].to ;
        int rf = p->vec->worf[a].rf ;
        
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
        dc.DrawCircle ( deg2x ( mn , r+ro/2 ) + w/2 ,
                        deg2y ( mn , r+ro/2 ) + h/2 ,
                        4 ) ;
        dc.DrawLine ( deg2x ( mm , r+ro/2 ) + w/2 ,
                      deg2y ( mm , r+ro/2 ) + h/2 ,
                      deg2x ( mm - dir*2 , r+ro/2-roi/4 ) + w/2 ,
                      deg2y ( mm - dir*2 , r+ro/2-roi/4 ) + h/2 ) ;
        dc.DrawLine ( deg2x ( mm , r+ro/2 ) + w/2 ,
                      deg2y ( mm , r+ro/2 ) + h/2 ,
                      deg2x ( mm - dir*2 , r+ro/2+roi/4 ) + w/2 ,
                      deg2y ( mm - dir*2 , r+ro/2+roi/4 ) + h/2 ) ;
                      
        dc.SetBackgroundMode ( wxTRANSPARENT ) ;    
        dc.SetBrush ( *wxTRANSPARENT_BRUSH ) ;    
        dc.DrawEllipticArc ( w/2-r-ro/2 , h/2-r-ro/2 , r*2+ro , r*2+ro , 90-mt , 90-mf ) ;

        p->vec->worf[a].dist1 = r+ro/2-roi/4 ;
        p->vec->worf[a].dist2 = r+ro/2+roi/4 ;
        p->vec->worf[a].deg1 = mf;
        p->vec->worf[a].deg2 = mt ;
        if ( mt < mf ) p->vec->worf[a].deg2 = mt + 360 ;
        }
    dc.SetPen(*wxBLACK_PEN);
    }

void PlasmidCanvas::OnDrawCircular(wxDC& dc)
{
    // Initial calculations    
    char t[10000] ;
    wxCoord dx , dy ;
    int l = p->vec->sequence.length();
    int a , mwh ;
    mwh = w<h?w:h ;

    if ( printing ) mwh -= mwh / 5 ;

    int fontfactor = 10 ;
    if ( printing ) fontfactor = mwh/80;
    wxFont *smallFont = MYFONT ( fontfactor , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *normalFont = MYFONT ( fontfactor*6/5 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *bigFont = MYFONT ( fontfactor*7/5 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *hugeFont = MYFONT ( fontfactor*9/5 , wxSWISS , wxNORMAL , wxBOLD ) ;
    
    int d ;
    for ( d = 1 ; d*10 < l ; d *= 10 ) ;
    r = (mwh*2/3)/2 ;

    // Basic elements
    dc.SetPen(*wxBLACK_PEN);
    if ( printing ) dc.SetBrush ( *wxWHITE_BRUSH ) ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    dc.DrawEllipse ( w/2-r , h/2-r , r*2 , r*2 ) ;
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
//        if ( mf == mt ) mf = mt - 0.1 ;
        if ( getMarkFrom() == getMarkTo() ) mf = mt - 0.1 ;
        else if ( mf > mt ) { float mm = mf ; mf = mt ; mt = mm ; }
        dc.DrawEllipticArc ( w/2-r , h/2-r , r*2 , r*2 , mf , mt ) ;
        dc.DrawLine ( w/2 , h/2 , deg2x(90-mf,r)+w/2 , deg2y(90-mf,r)+h/2 ) ;
        dc.DrawLine ( w/2 , h/2 , deg2x(90-mt,r)+w/2 , deg2y(90-mt,r)+h/2 ) ;
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBackgroundMode ( wxTRANSPARENT ) ;
        }

    // Show title
    if ( myapp()->frame->showVectorTitle )
        {
        char *c1 , *c2 ;
        strcpy ( t , p->vec->name.c_str() ) ;
        strcat ( t , " " ) ;
        a = 0 ;
        dc.SetFont ( *bigFont ) ;
        dc.GetTextExtent ( t , &dx , &dy ) ;
        for ( c1 = t ; *c1 ; c1++ )
           if ( *c1 == ' ' ) a -= dy ;
        for ( c1 = c2 = t ; *c1 ; c1++ )
           {
           if ( *c1 == ' ' )
              {
              *c1 = 0 ;
              dc.GetTextExtent ( c2 , &dx , &dy ) ;
              dc.DrawText ( c2 , w/2-dx/2 , h/2-dy/2+a ) ;
              c2 = c1+1 ;
              a += dy ;
              }
           }
        }
    
    // Show length
    if ( myapp()->frame->showVectorLength )
        {
        sprintf ( t , txt("#bp") , p->vec->sequence.length() ) ;
        dc.SetFont ( *normalFont ) ;
        dc.GetTextExtent ( t , &dx , &dy ) ;
        dc.DrawText ( t , w/2-dx/2 , h/2-dy/2 ) ;
        }
    
    // Numbers
    dc.SetFont(*smallFont);
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    for ( a = 0 ; a <= l ; a += d )
        {
            float deg = a*360/l ;
            int r1 = r*17/20 ;
            int r2 = r*20/20 ;
            dc.DrawLine ( deg2x ( deg , r1 ) + w/2 ,
                          deg2y ( deg , r1 ) + h/2 ,
                          deg2x ( deg , r2 ) + w/2 ,
                          deg2y ( deg , r2 ) + h/2 ) ;
                          
            // Numbers
            sprintf ( t , "%d" , a ) ;
            dc.GetTextExtent ( t , &dx , &dy ) ;
            if ( deg > 180 && deg < 350 ) dx = 0 ;
            if ( deg < 15 ) dx = 0 ;
            if ( deg < 90 || deg > 270 ) dy = 0 ;
            if ( a > 0 )
               dc.DrawText ( t ,
                             deg2x ( deg , r1 ) + w/2 - dx ,
                             deg2y ( deg , r1 ) + h/2 - dy ) ;
        }

    if ( p->vec->recalcvisual )
        {
        // Items (Genes etc.)
        for ( a = 0 ; a < p->vec->items.size() ; a++ )
            {
            TVectorItem *i = &p->vec->items[a] ;
            float df = i->from*360.0/((float)l) ;
            float dt = i->to*360.0/((float)l) ;
            if ( dt < df ) dt += 360.0 ;
            float dd = (dt-df)*5.0/100.0 ; // Arrow length in degrees
            float ds = 1 ; // Paint steps for genes, in degrees
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
        arrangeRestrictionSitesCircular ( dc ) ;

        // ORFs
        if ( p->showORFs )
           {
           p->showORFs = false ;
           wxCommandEvent event ;
           p->OnORFs ( event ) ;
           }

        p->vec->recalcvisual = false ;
        }

    // ORFs
    drawCircularORFs ( dc ) ;
         
    // Drawing items
    for ( a = 0 ; a < p->vec->items.size() ; a++ )
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
            if ( i->direction ==  1 ) 
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

            for ( float b = df ; b < dt ; b += ds )
                    p.push_back ( wxPoint ( deg2x ( b , r1 ) , deg2y ( b , r1 ) ) ) ;
            if ( i->direction ==  1 )
                        p.push_back ( wxPoint ( deg2x ( dt+dd , (r1+r2)/2 ) , 
                                                deg2y ( dt+dd , (r1+r2)/2 ) ) ) ;
            if ( i->direction == -1 )
                        p.push_back ( wxPoint ( deg2x ( dt-dd , (r1+r2)/2 ) , 
                                                deg2y ( dt-dd , (r1+r2)/2 ) ) ) ;
            for ( float b = dt ; b > df ; b -= ds )
                    p.push_back ( wxPoint ( deg2x ( b , r2 ) , deg2y ( b , r2 ) ) ) ;

            // Drawing polygon
            wxPoint *wp ;
            wp = (wxPoint*) malloc ( sizeof ( wxPoint ) * (p.size()+1) ) ;
            for ( int b = 0 ; b < p.size() ; b++ ) wp[b] = p[b] ;
            dc.SetPen(*wxBLACK_PEN);
            dc.SetBrush ( *(i->getBrush()) ) ;
            dc.DrawPolygon ( p.size() , wp , w/2 , h/2 ) ;
            free ( wp ) ;
            }
        }

    for ( a = 0 ; a < p->vec->items.size() ; a++ ) // Item titles
        {
        TVectorItem *i = &p->vec->items[a] ;
        if ( i->isVisible() && i->getParam ( "PREDECESSOR" ) == "" )
            {
            int r1 = i->r1 * 100 * r / ( STANDARDRADIUS * 100 ) ;
            int r2 = i->r2 * 100 * r / ( STANDARDRADIUS * 100 ) ;
            float df = i->a1 ;
            float dt = i->a2 ;
            
            float dd = (dt-df)*5/100 ; // Arrow length in degrees
            float ds = 1 ; // Paint steps for genes, in degrees
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
    dc.SetFont(*smallFont);
    for ( a = 0 ; a < p->vec->rc.size() ; a++ )
        {
        TRestrictionCut c = p->vec->rc[a] ;
        if ( isEnzymeVisible ( c.e->name ) )
           {
           c.r1 = c.r1 * 100 * r / ( STANDARDRADIUS * 100 ) ;
           c.r2 = c.r2 * 100 * r / ( STANDARDRADIUS * 100 ) ;
           c.r3 = c.r3 * 100 * r / ( STANDARDRADIUS * 100 ) ;
           wxPoint p1 ( deg2x ( c.angle , c.r1 )+w/2 , deg2y ( c.angle , c.r1 )+h/2 ) ;
           wxPoint p2 ( deg2x ( c.angle , c.r2 )+w/2 , deg2y ( c.angle , c.r2 )+h/2 ) ;
           dc.DrawLine ( p1 , p2 ) ;
           wxPoint p3 = p->vec->rc[a].p ;
            
           p3 = makeLastRect ( a , dc ) ;
           if ( p3.x < w/2 )
              p3.x = p->vec->rc[a].lastrect.GetRight() ;
           p3.y = ( p->vec->rc[a].lastrect.GetTop() + p->vec->rc[a].lastrect.GetBottom() ) / 2 ;
                    
           dc.DrawLine ( p2 , p3 ) ;
    
           char u[100] ;
           sprintf ( u , "%s (%d)" , c.e->name.c_str() , c.pos ) ;
//           sprintf ( u , "%s (%d) %f" , c.e->name.c_str() , c.pos , c.angle3 ) ;
           p3.x = p->vec->rc[a].lastrect.GetLeft() ;
           p3.y = p->vec->rc[a].lastrect.GetTop() ;
    
           dc.DrawText ( u , p3.x , p3.y ) ;
           }    
        }
}

wxPoint PlasmidCanvas::makeLastRect ( int a , wxDC &dc )
    {
    wxPoint p3 = p->vec->rc[a].p ;
    p3.x = p3.x * 100 * r / ( STANDARDRADIUS * 100 ) + w/2 ;
    p3.y = p3.y * 100 * r / ( STANDARDRADIUS * 100 ) + h/2 ;

    char u[100] ;
    sprintf ( u , "%s (%d)" , p->vec->rc[a].e->name.c_str() , p->vec->rc[a].pos ) ;
    int te_x , te_y ;
    dc.GetTextExtent ( u , &te_x , &te_y ) ;
    p3.y -= te_y / 2 ;
//    if ( p3.x < w/2 ) p3.x -= te_x + 4 ;
    if ( p->vec->rc[a].angle3 > 180 ) p3.x -= te_x + 4 ;
    else p3.x += 4 ;
    p->vec->rc[a].lastrect = wxRect ( p3.x , p3.y , te_x , te_y ) ;

    return p3 ;
    }
    
int PlasmidCanvas::circular_pos ( float angle )
    {
    int l = p->vec->sequence.length() ;
    float a = l ;
    a /= 360.0 ;
    a *= angle ;
    int b = (int) a ;
    if ( b < 0 ) b = 0 ;
    if ( b >= l ) b = l - 1 ;
    return b+1 ;
    }

// EVENTS

void PlasmidCanvas::OnEventCircular(wxMouseEvent& event)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    wxPoint pt(event.GetLogicalPosition(dc));
    wxPoint pt_abs(event.GetPosition());

    wxPoint pt2 , pto = pt ;
    pt2.x = pt.x-w/2 ;
    pt2.y = pt.y-h/2 ;
    int pos = 0 ;
    float angle  = xy2deg ( pt2.x , pt2.y ) ;
    float radius = xy2r ( pt2.x , pt2.y ) ;
    pt.x = deg2x ( angle , radius ) + w/2 ;
    pt.y = deg2y ( angle , radius ) + h/2 ;
    
    int vo = findVectorObject ( angle , radius ) ;
    int rs = findRestrictionSite ( pt.x , pt.y ) ;
    int orf = findORFcircular ( angle , radius ) ;
    string s = "" ;
    
    if ( event.LeftDown() )
        {
        CaptureMouse() ;
        initialclick = true ;
        mousediffx = mousediffy = 0 ;
        }
    else if ( event.LeftUp() )
        {
        lastbp = -1 ;
        initialclick = false ;
        ReleaseMouse() ;
        }
        
   if ( event.ControlDown() && zoom != 100 )
      {
        lastrestrictionsite = rs = -1 ;
        lastvectorobject = vo = -1 ;
        orf = -1 ;
        SetCursor(wxCursor(wxCURSOR_SIZING)) ;
        if ( event.LeftIsDown() )
           {
            if ( initialclick )
               {
               mousediffx = pt.x ;
               mousediffy = pt.y ;
               initialclick = false ;
               }
           int nx , ny ;
           GetViewStart ( &nx , &ny ) ;
           Scroll ( nx + mousediffx - pt.x , ny + mousediffy - pt.y ) ;
           }
      }
        
    if ( lastbp != -1 )
        {
        lastrestrictionsite = rs = -1 ;
        lastvectorobject = vo = -1 ;
        }
    
    // Passing over
    if ( rs != -1 )
        {
        s = p->vec->rc[rs].e->name ;
        char ttt[1000] ;
        sprintf ( ttt , txt("tt_rs") , s.c_str() , p->vec->countCuts ( s ) ) ;
        SetMyToolTip ( ttt , TT_RS ) ;
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        wxLogStatus(txt("rsite_status_bar") , s.c_str() ) ;
        if ( event.LeftDown() )
           {
#ifdef __WXMSW__
           p->treeBox->SelectItem ( p->treeBox->GetParent ( p->vec->rc[rs].treeid ) ) ;
#else
				   p->treeBox->SelectItem ( p->treeBox->GetParent ( ) ) ; // Frell
#endif
           }
        else if ( event.RightDown() )
           invokeRsPopup ( rs , pt_abs ) ;
        else if ( event.LeftDClick() )
           invokeVectorEditor ( "enzyme" , rs ) ;
        else if ( event.MiddleDown() )
           {
           TRestrictionEditor ed ( myapp()->frame , "" , wxPoint(-1,-1) , 
                      wxSize(600,400) , 
                      wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL);
           ed.pre = s.c_str() ;
           ed.cocktail = p->vec->cocktail ;
           ed.initme ( p->vec ) ;
           if ( ed.ShowModal () == wxID_OK )
              {
              p->vec->cocktail = ed.cocktail ;
              p->vec->doAction() ;
              }
           }
        }
    else if ( vo != -1 )
        {
        s = p->vec->items[vo].name ;
        char ttt[1000] ;
        sprintf ( ttt , "itemtype%d" , p->vec->items[vo].type ) ;
        string tt_type = txt(ttt) ;
        sprintf ( ttt , txt("tt_item") , tt_type.c_str() ,
                                        s.c_str() , 
                                        p->vec->items[vo].desc.c_str() ) ;
        SetMyToolTip ( ttt , TT_ITEM ) ;
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        wxLogStatus(txt("item_status_bar") , s.c_str() ) ;
        if ( event.LeftDown() )
           p->treeBox->SelectItem ( p->vec->items[vo].getTreeID() ) ;
        else if ( event.RightDown() )
             invokeItemPopup ( vo , pt_abs ) ;
        else if ( event.LeftDClick() )
           invokeVectorEditor ( "item" , vo ) ;
        else if ( event.MiddleDown() )
           {
           wxCommandEvent dummyEvent ;
           context_last_item = vo ;
           itemMarkShow ( dummyEvent ) ;
           p->cSequence->SetFocus () ;
           }
        
        }
    else if ( orf != -1 )
        {
        char ttt[1000] ;
        sprintf ( ttt ,
                  "%d-%d, %d" , 
                  p->vec->worf[orf].from ,
                  p->vec->worf[orf].to ,
                  p->vec->worf[orf].rf ) ;
        SetMyToolTip ( ttt , TT_ORF ) ;
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        wxLogStatus(txt("item_status_bar") , ttt ) ;
        if ( event.LeftDClick() )
           p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;
        }
    else
        {
        SetMyToolTip ( "" , TT_NONE ) ;
        if ( !event.ControlDown() ) SetCursor(*wxSTANDARD_CURSOR) ;
        float bp = circular_pos ( angle ) ;
        wxLogStatus(txt("bp"), int(bp) ) ;
//        wxLogStatus("%2.2f", angle ) ;
        if ( event.LeftDClick() ) invokeVectorEditor () ;
        else if ( event.RightDown() ) invokeVectorPopup ( pt_abs ) ;
        }
        
        

    if ( event.Dragging() && event.LeftIsDown() && lastrestrictionsite != -1 )
        {
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        rs = lastrestrictionsite ;
        lastvectorobject = -1 ;
        wxRect lr = p->vec->rc[rs].lastrect ;
        wxPoint q1 ;
        
        q1 = pto ;
        q1.x -= w/2 ;
        q1.y -= h/2 ;

        q1.x = q1.x * ( STANDARDRADIUS * 100 ) / ( 100 * r ) ;
        q1.y = q1.y * ( STANDARDRADIUS * 100 ) / ( 100 * r ) ;
        
        if ( initialclick )
           {
           mousediffx = p->vec->rc[rs].p.x - q1.x ;
           mousediffy = p->vec->rc[rs].p.y - q1.y ;
           initialclick = false ;
           }
        q1.x += mousediffx ;
        q1.y += mousediffy ;

        p->vec->rc[rs].p.x = q1.x ;
        p->vec->rc[rs].p.y = q1.y ;
        
        Refresh () ;
        }
    else if ( event.Dragging() && event.LeftIsDown() && lastvectorobject != -1 )
       {
       SetCursor(wxCursor(wxCURSOR_HAND)) ;
       vo = lastvectorobject ;
       lastrestrictionsite = -1 ;
       int dr = p->vec->items[vo].r2 - p->vec->items[vo].r1 ;
       float r1 = STANDARDRADIUS * radius / r - p->vec->items[vo].r1 ;
       p->vec->items[vo].r1 += int ( r1 ) ;
       p->vec->items[vo].r2 = p->vec->items[vo].r1 + dr ;
       updateLinkedItems ( p->vec , vo ) ;
       Refresh () ;
       }
/*    else if ( event.LeftIsDown() && event.RightIsDown() ) // Turning disabeled
       {
       int f = (pto.x-w/2)/10 ;
       p->vec->turn ( f ) ;
       Refresh () ;
       p->cSequence->arrange () ;
       p->cSequence->Refresh () ;
       }*/
    else if ( event.LeftIsDown() && orf != -1 )
        {
        p->cSequence->mark ( "DNA" ,
                                p->vec->worf[orf].from + 1 ,
                                p->vec->worf[orf].to + 1 ) ;
        }
    else if ( event.RightDown() && orf != -1 )
        {
        p->cSequence->mark ( "DNA" ,
                                p->vec->worf[orf].from + 1 ,
                                p->vec->worf[orf].to + 1 ) ;
        invokeORFpopup ( orf , pt_abs ) ;
        }
    else if ( event.LeftIsDown() && rs == -1 && vo == -1 && orf == -1 )
       {
       float bp = circular_pos ( angle ) ;
       if ( lastbp == -1 && initialclick ) lastbp = int(bp) ;
       if ( lastbp != -1 )
          {
          if ( lastbp != int(bp) )
             p->cSequence->mark ( "DNA" , lastbp , int(bp) ) ;
          else
             p->cSequence->mark ( "DNA" , -1 , -1 ) ;
          }
       }
    else if ( event.MiddleDown() && p->cSequence->_from != -1 )
       {
       p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;
       }
    else
       {
       lastrestrictionsite = rs ;
       lastvectorobject = vo ;
       }
}

int PlasmidCanvas::findORFcircular ( float angle , float radius )
    {
    int a , found = -1 ;
    for ( a = 0 ; a < p->vec->worf.size() ; a++ )
        {
        if ( angle >= p->vec->worf[a].deg1 &&
             angle <= p->vec->worf[a].deg2 &&
             radius >= p->vec->worf[a].dist1 &&
             radius <= p->vec->worf[a].dist2 )
             found = a ;
        if ( p->vec->worf[a].deg2 > 360 &&
             angle <= p->vec->worf[a].deg2-360 &&
             radius >= p->vec->worf[a].dist1 &&
             radius <= p->vec->worf[a].dist2 )
             found = a ;
        }
    return found ;
    }
