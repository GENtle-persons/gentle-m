#include "PlasmidCanvas.h"

// DRAWING

void PlasmidCanvas::OnDrawLinear(wxDC& dc)
{
    if ( printing ) h = h * 2 / 3 ;
    int fontfactor = 10 ;
    if ( printing ) fontfactor = (w>h?h:w)/10000 ;
    wxFont *tinyFont = MYFONT ( fontfactor*4/5 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *smallFont = MYFONT ( fontfactor , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *normalFont = MYFONT ( fontfactor*6/5 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *bigFont = MYFONT ( fontfactor*7/5 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *hugeFont = MYFONT ( fontfactor*9/5 , wxSWISS , wxNORMAL , wxBOLD ) ;

    // Initial calculations    
    char t[100] ;
    int a , b ;
    int d , l = p->vec->getSequenceLength() - 1 ;
    for ( d = 1 ; d*10 < l ; d *= 10 ) ;
    
    int lineOff = w / 20 ;
    int lineLen = w - 2 * lineOff ;
    int lineH = h * 3 / 4 ;
    if ( p->def == "AminoAcids" ) lineH = h / 2 ;
    int markH = 20 ;
    wxCoord dx , dy ;
    
    dc.SetPen(*wxBLACK_PEN);
    
    // Sticky ends
    if ( p->vec->hasStickyEnds() )
        {
        wxString lu = p->vec->getStickyEnd(true,true) + " -";
        wxString ll = p->vec->getStickyEnd(true,false) + " -";
        wxString ru = "- " + p->vec->getStickyEnd(false,true) ;
        wxString rl = "- " + p->vec->getStickyEnd(false,false) ;
        wxCoord nx , ny ;
        
        dc.GetTextExtent ( lu , &dx , &dy ) ;
        dc.GetTextExtent ( ll , &nx , &ny ) ;
        if ( dx < nx ) { dx = nx - dx ; nx = 0 ; }
        else { nx = dx - nx ; dx = 0 ; }
        
        dc.DrawText ( lu , int(10 + dx) , 10 ) ;
        dc.DrawText ( ll , 10 + nx , 10 + dy ) ;
        
        dc.GetTextExtent ( ru , &dx , &dy ) ;
        dc.GetTextExtent ( rl , &nx , &ny ) ;
        if ( dx > nx ) nx = dx ;
        dc.DrawText ( ru , w - 10 - nx , 10 ) ;
        dc.DrawText ( rl , w - 10 - nx , 10 + dy ) ;
        }
    
    // Mark
    if ( getMarkFrom() != -1 )
        {
        int mf = getMarkFrom() - 1 ;
        int mt = getMarkTo() - 1 ;
        dc.SetBackgroundMode ( wxSOLID ) ;
        dc.SetBrush ( *wxLIGHT_GREY_BRUSH ) ;
        if ( mt <= l )
            {
            dc.DrawRectangle ( lineOff + lineLen * mf / (l+1) ,
                               lineH ,
                               lineLen * ( mt - mf ) / (l+1) ,
                               h - lineH ) ;
            }
        else
            {
            dc.DrawRectangle ( lineOff + lineLen * mf / (l+1) ,
                               lineH ,
                               lineLen - lineLen * mf / (l+1) ,
                               h - lineH ) ;
            dc.DrawRectangle ( lineOff ,
                               lineH ,
                               lineLen * ( mt - l ) / (l+1) ,
                               h - lineH ) ;
            }
        dc.SetBackgroundMode ( wxTRANSPARENT ) ;
        }
    
    // Baseline
    if ( p->vec->showGC() > 0 ) // %GC
    	{
	    int dh = h / 80 ;
	    int nob = p->vec->showGC() ;
	    for ( a = 0 ; a < nob ; a++ )
	    	{
 	    	int at = 0 , gc = 0 , other = 0 ;
	    	for ( b = l * a / nob ; b < l * ( a + 1 ) / nob ; b++ )
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
    		int x1 = lineOff + lineLen * a / nob ;
    		int x2 = lineOff + lineLen * (a+1) / nob ;
    		dc.DrawRectangle ( x1 ,
        					   lineH - dh ,
        					   x2 - x1 ,
        					   dh * 2 ) ;
	    	}    
	    showGClegend ( dc ) ;
    	dc.SetPen(*wxBLACK_PEN);
	    dc.DrawLine ( lineOff , lineH-dh , w - lineOff , lineH-dh ) ;
	    dc.DrawLine ( lineOff , lineH+dh , w - lineOff , lineH+dh ) ;
    	}
    else dc.DrawLine ( lineOff , lineH , w - lineOff , lineH ) ;

    // Numbers
    dc.SetFont(*smallFont);
    for ( a = 0 ; a < l ; a += d ) 
        {
        dc.DrawLine ( lineOff + lineLen * a / (l+1) ,
                      lineH ,
                      lineOff + lineLen * a / (l+1) ,
                      h - markH ) ;
        sprintf ( t , "%d" , a+1 ) ;
        dc.GetTextExtent ( t , &dx , &dy ) ;
        dc.DrawText ( t ,
                      lineOff + lineLen * a / (l+1) - dx/2 ,
                      h - markH ) ;
        }

    // Methylation sites
    dc.SetPen(*wxRED_PEN);
    for ( a = 0 ; a < p->vec->methyl.GetCount() ; a++ )
    	{
        dc.DrawLine ( lineOff + lineLen * p->vec->methyl[a] / (l+1) ,
                      lineH ,
                      lineOff + lineLen * p->vec->methyl[a] / (l+1) ,
                      lineH + h / 40 ) ;
    	}    
    dc.SetPen(*wxBLACK_PEN);

    // Recalc
    if ( p->vec->recalcvisual )
        {
        // Genes, items, etc.
        for ( a = 0 ; a < p->vec->items.size() ; a++ )
            {
            TVectorItem *i = &p->vec->items[a] ;
            int r1 = lineOff + lineLen * ( i->from - 1 ) / (l+1) ;
            int r2 = lineOff + lineLen * ( i->to - 1 ) / (l+1) ;
            if ( r1 <= r2 )
                {
                i->r1 = r1 ; // From X
                i->r2 = r2 ; // To X
                i->r3 = -1 ;
                }
            else
                {
                i->r1 = r1 ; // From X, right part
                i->r2 = lineOff + lineLen ; // To X, right part
                i->r3 = lineOff ; // From X, left part
                i->r4 = r2 ; // To X, left part
                i->r3 = i->r3 * STANDARDRADIUS / w ;
                i->r4 = i->r4 * STANDARDRADIUS / w ;
                }
            i->a1 = lineH - markH / 2 ; // From Y
            i->a2 = lineH + markH / 2 ; // To Y
            
            int tf = ( i->type % 3 ) * STANDARDRADIUS / 15 ;
        
            i->r1 = i->r1 * STANDARDRADIUS / w ;
            i->r2 = i->r2 * STANDARDRADIUS / w ;
            i->a1 = i->a1 * STANDARDRADIUS / h + tf ;
            i->a2 = i->a2 * STANDARDRADIUS / h + tf ;
            }
        
        // Restriction sites
        dc.SetFont(*tinyFont);
        for ( a = 0 ; a < p->vec->rc.size() ; a++ )
            {
            TRestrictionCut *c = &p->vec->rc[a] ;
            if ( p->vec->isEnzymeHidden ( c->e->name ) ) continue ;
            c->p = wxPoint ( lineOff + lineLen * c->pos / (l+1) , lineH - markH ) ;
            arrangeRestrictionSitesLinear ( a , dc ) ;
            }
        for ( a = 0 ; a < p->vec->rc.size() ; a++ )
            {
            TRestrictionCut *c = &p->vec->rc[a] ;
            c->lp.x = c->p.x * STANDARDRADIUS / w ;
            c->lp.y = c->p.y * STANDARDRADIUS / h ;
            }
        dc.SetFont(*smallFont);
        
        // ORFs
        if ( p->def == "dna" && p->showORFs )
           {
           p->showORFs = false ;
           wxCommandEvent event ;
           p->OnORFs ( event ) ;
           }
        
        p->vec->recalcvisual = false ;
        }

    // ORFs
    drawLinearORFs ( dc ) ;

    // Drawing items
    for ( a = 0 ; a < p->vec->items.size() ; a++ )
        {
        TVectorItem i = p->vec->items[a] ;
        if ( i.isVisible() )
            {
            drawLinearItem ( dc , 
                             i.r1 * w / STANDARDRADIUS , 
                             i.r2 * w / STANDARDRADIUS , 
                             i.a1 * h / STANDARDRADIUS , 
                             i.a2 * h / STANDARDRADIUS , 
                             &i ) ;
            if ( i.r3 != -1 )
                drawLinearItem ( dc , 
                                 i.r3 * w / STANDARDRADIUS , 
                                 i.r4 * w / STANDARDRADIUS , 
                                 i.a1 * h / STANDARDRADIUS , 
                                 i.a2 * h / STANDARDRADIUS , 
                                 &i ) ;
            }
        }
        
    // Drawing Restriction Sites
    dc.SetFont(*tinyFont);
    for ( a = 0 ; a < p->vec->rc.size() ; a++ )
        {
        TRestrictionCut *c = &p->vec->rc[a] ;
        if ( isEnzymeVisible ( c->e->name ) )
           {
           wxColour *col = p->vec->getEnzymeRule()->getColor ( p->vec->countCuts ( c->e->name ) ) ;
           dc.SetTextForeground ( *col ) ;
           dc.SetPen ( *MYPEN(*col) ) ;
           c->linearUpdate ( w , h ) ;
           int xx = lineOff + c->pos*lineLen/(l+1) ;
           dc.DrawLine ( c->lastrect.GetRight() , c->lastrect.GetBottom() , xx , lineH - markH ) ;
           dc.DrawLine ( xx , lineH , xx , lineH - markH - 1 ) ;
           dc.DrawText ( c->getNameAndPosition() , c->lastrect.x , c->lastrect.y ) ;
           }
        }
    dc.SetFont(*smallFont);
    dc.SetPen ( *wxBLACK_PEN ) ;
    dc.SetTextForeground ( *wxBLACK ) ;
}

void PlasmidCanvas::drawLinearORFs ( wxDC &dc )
    {
    int a ;
    int l = p->vec->getSequenceLength() ;
    for ( a = 0 ; a < p->vec->worf.size() ; a++ )
        {
        float mf = p->vec->worf[a].from ;
        float mt = p->vec->worf[a].to ;
        int rf = p->vec->worf[a].rf ;
        int lineOff = w / 20 ;
        int mh = h/2 ;
        int ah = h/20 ;
        mh += rf * ah ;
        if ( mt < mf ) mt += l ;
        mf = mf * ( w - lineOff * 2 ) / l + lineOff ;
        mt = mt * ( w - lineOff * 2 ) / l + lineOff ;
        wxColour col ;
        if ( rf == 1 || rf == -1 ) col = wxColour ( 200 , 0 , 0 ) ;
        if ( rf == 2 || rf == -2 ) col = wxColour ( 0 , 200 , 0 ) ;
        if ( rf == 3 || rf == -3 ) col = wxColour ( 0 , 0 , 200 ) ;
        dc.SetBackgroundMode ( wxSOLID ) ;
        dc.SetBrush ( *MYBRUSH(col) ) ;
        dc.SetPen ( *MYPEN(col) ) ;

        p->vec->worf[a].dist1 = mh - ah/4 ;
        p->vec->worf[a].dist2 = mh + ah/4 ;
        p->vec->worf[a].deg1 = mf ;
        p->vec->worf[a].deg2 = mt ;
        
        int dir = 1 ;
        if ( rf < 0 )
           {
           float mm = mf ;
           mf = mt ;
           mt = mm ;
           dir = -1 ;
           }
        dc.DrawLine ( (int)mf , (int)mh , (int)mt , (int)mh ) ;
        dc.DrawLine ( (int)mt , (int)mh , (int)(mt - dir*w/100) , (int)(mh - ah/4) ) ;
        dc.DrawLine ( (int)mt , (int)mh , (int)(mt - dir*w/100) , (int)(mh + ah/4) ) ;
        dc.DrawCircle ( (int)mf , (int)mh , (int)(ah/4) ) ;

        dc.SetBackgroundMode ( wxTRANSPARENT ) ;    
        dc.SetBrush ( *wxTRANSPARENT_BRUSH ) ;    
        }
    dc.SetPen(*wxBLACK_PEN);
    }
    
void PlasmidCanvas::arrangeRestrictionSitesLinear ( int a , wxDC &dc )
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    int x1 , y1 , x2 , y2 ;
    wxCoord dx , dy ;
	wxString t = c->getNameAndPosition () ;
    dc.GetTextExtent ( t , &dx , &dy ) ;
    x1 = c->p.x - dx ;
    y1 = c->p.y - dy ;
    x2 = c->p.x ;
    y2 = c->p.y ;
    bool doit = true ;
    int b ;
    while ( doit )
        {
        c->lastrect = wxRect ( x1 , y1 , x2-x1+1 , y2-y1 ) ;
        for ( b = 0 ; b < a &&
                      !p->vec->isEnzymeHidden(c->e->name) &&
                      !intersects(c->lastrect,p->vec->rc[b].lastrect) &&
                      !intersectsLine(c->lastrect,p->vec->rc[b].p)
                      ; b++ ) ;
        if ( b == a ) doit = false ;
        else
           {
           y1 -= dy ;
           y2 -= dy ;
           }
        }
    c->p.y = c->lastrect.GetBottom() ;
    }
    
bool PlasmidCanvas::intersectsLine ( wxRect &a , wxPoint p )
    {
    if ( a.GetBottom() > p.y &&
         a.GetLeft() < p.x &&
         a.GetRight() > p.x )
         return true ;
    return false ;
    }

void PlasmidCanvas::drawLinearItem ( wxDC& dc , int r1 , int r2 , float a1 , float a2 , TVectorItem *i )
    {
    if ( i->direction == -1 ) { int rr = r1 ; r1 = r2 ; r2 = rr ; }
    wxFont *normalFont = MYFONT ( 12 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont *smallFont = MYFONT ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    int r1b = r2 - ( r2 - r1 ) / 10 ;
    int y1 = (int) ( a2 - a1 ) / 3 ;
    int y2 = (int) ( a1 + a2 ) / 2 ;
    vector <wxPoint> pt ;
    pt.push_back ( wxPoint ( r1  , y2 - y1 ) ) ;
    pt.push_back ( wxPoint ( r1b , y2 - y1 ) ) ;
    pt.push_back ( wxPoint ( r1b , (int) a1 ) ) ;
    pt.push_back ( wxPoint ( r2  , y2 ) ) ;
    pt.push_back ( wxPoint ( r1b , (int) a2 ) ) ;
    pt.push_back ( wxPoint ( r1b , y2 + y1 ) ) ;
    pt.push_back ( wxPoint ( r1  , y2 + y1 ) ) ;

    // Drawing polygon
    wxPoint *wp ;
    wp = (wxPoint*) malloc ( sizeof ( wxPoint ) * (pt.size()+1) ) ;
    for ( int b = 0 ; b < pt.size() ; b++ ) wp[b] = pt[b] ;
    if ( !this->p->vec->getGenomeMode() ) dc.SetPen(*wxBLACK_PEN);
    else dc.SetPen ( *MYPEN((wxColour)i->getBrush()->GetColour()) ) ;
    dc.SetBrush ( *i->getBrush() ) ;
    dc.DrawPolygon ( pt.size() , wp , 0 , 0 ) ;
    free ( wp ) ;
    
    // Name
    if ( !i->getParam("PREDECESSOR").IsEmpty() ) return ; // Only first name
    char t[1000] ;
    wxCoord dx , dy ;
    wxColor fc = dc.GetTextForeground () ;
    dc.SetTextForeground ( i->getFontColor() ) ;
    if ( p->def == "dna" ) dc.SetFont(*normalFont);
    else if ( p->def == "AminoAcids" ) dc.SetFont(*smallFont);
    
    if ( !p->vec->getGenomeMode() )
        {
        sprintf ( t , "%s" , i->name.c_str() ) ;
        dc.GetTextExtent ( t , &dx , &dy ) ;
        dc.DrawText ( t ,
                  (r2+r1)/2 - dx/2 ,
                  (int) a2 - y1 + 1 ) ;
        }    
    dc.SetTextForeground ( fc ) ;    
    dc.SetPen(*wxBLACK_PEN);
    }

// EVENTS

int PlasmidCanvas::findORFlinear ( int x , int y )
    {
    int a , found = -1 ;
    for ( a = 0 ; a < p->vec->worf.size() ; a++ )
        {
        if ( x >= p->vec->worf[a].deg1 &&
             x <= p->vec->worf[a].deg2 &&
             y >= p->vec->worf[a].dist1 &&
             y <= p->vec->worf[a].dist2 )
             found = a ;
        }
    return found ;
    }

