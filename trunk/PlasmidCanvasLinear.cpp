#include "PlasmidCanvas.h"

// DRAWING

void PlasmidCanvas::OnDrawLinear(wxDC& dc)
{
    if ( printing ) h = h * 2 / 3 ;
    int fontfactor = 10 ;
    if ( printing ) fontfactor = (w>h?h:w)/10000 ;
    wxFont smallFont ( fontfactor , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont normalFont ( fontfactor*6/5 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont bigFont ( fontfactor*7/5 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    wxFont hugeFont ( fontfactor*9/5 , wxSWISS , wxNORMAL , wxBOLD ) ;

    // Initial calculations    
    char t[100] ;
    int a , b ;
    int d , l = p->vec->sequence.length() - 1 ;
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
        string lu = p->vec->getStickyEnd(true,true) + " -";
        string ll = p->vec->getStickyEnd(true,false) + " -";
        string ru = "- " + p->vec->getStickyEnd(false,true) ;
        string rl = "- " + p->vec->getStickyEnd(false,false) ;
        wxCoord nx , ny ;
        
        dc.GetTextExtent ( lu.c_str() , &dx , &dy ) ;
        dc.GetTextExtent ( ll.c_str() , &nx , &ny ) ;
        if ( dx < nx ) { dx = nx - dx ; nx = 0 ; }
        else { nx = dx - nx ; dx = 0 ; }
        
        dc.DrawText ( lu.c_str() , int(10 + dx) , 10 ) ;
        dc.DrawText ( ll.c_str() , 10 + nx , 10 + dy ) ;
        
        dc.GetTextExtent ( ru.c_str() , &dx , &dy ) ;
        dc.GetTextExtent ( rl.c_str() , &nx , &ny ) ;
        if ( dx > nx ) nx = dx ;
        dc.DrawText ( (char*)ru.c_str() , w - 10 - nx , 10 ) ;
        dc.DrawText ( (char*)rl.c_str() , w - 10 - nx , 10 + dy ) ;
        }
    
    // Mark
    if ( mark_from != -1 )
        {
        int mf = mark_from - 1 ;
        int mt = mark_to - 1 ;
        dc.SetBackgroundMode ( wxSOLID ) ;
        dc.SetBrush ( *wxLIGHT_GREY_BRUSH ) ;
        if ( mt <= l )
            {
            dc.DrawRectangle ( lineOff + lineLen * mf / l ,
                               lineH ,
                               lineLen * ( mt - mf ) / l ,
                               h - lineH ) ;
            }
        else
            {
            dc.DrawRectangle ( lineOff + lineLen * mf / l ,
                               lineH ,
                               lineLen - lineLen * mf / l ,
                               h - lineH ) ;
            dc.DrawRectangle ( lineOff ,
                               lineH ,
                               lineLen * ( mt - l ) / l ,
                               h - lineH ) ;
            }
        dc.SetBackgroundMode ( wxTRANSPARENT ) ;
        }
    
    // Baseline
    dc.DrawLine ( lineOff , lineH , w - lineOff , lineH ) ;

    // Numbers
    dc.SetFont(smallFont);
    for ( a = 0 ; a < l ; a += d ) 
        {
        dc.DrawLine ( lineOff + lineLen * a / l ,
                      lineH ,
                      lineOff + lineLen * a / l ,
                      h - markH ) ;
        sprintf ( t , "%d" , a+1 ) ;
        dc.GetTextExtent ( t , &dx , &dy ) ;
        dc.DrawText ( t ,
                      lineOff + lineLen * a / l - dx/2 ,
                      h - markH ) ;
        }

    // Recalc
    if ( p->vec->recalcvisual )
        {
        // Genes, items, etc.
        for ( a = 0 ; a < p->vec->items.size() ; a++ )
            {
            TVectorItem *i = &p->vec->items[a] ;
            int r1 = lineOff + lineLen * ( i->from - 1 ) / l ;
            int r2 = lineOff + lineLen * ( i->to - 1 ) / l ;
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
        for ( a = 0 ; a < p->vec->rc.size() ; a++ )
            {
            TRestrictionCut *c = &p->vec->rc[a] ;
            c->p = wxPoint ( lineOff + lineLen * c->pos / l , lineH - markH ) ;
            arrangeRestrictionSitesLinear ( a , dc ) ;
            }
        for ( a = 0 ; a < p->vec->rc.size() ; a++ )
            {
            TRestrictionCut *c = &p->vec->rc[a] ;
            c->lp.x = c->p.x * STANDARDRADIUS / w ;
            c->lp.y = c->p.y * STANDARDRADIUS / h ;
            }
        
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
    dc.SetFont(smallFont);
    for ( a = 0 ; a < p->vec->rc.size() ; a++ )
        {
        TRestrictionCut *c = &p->vec->rc[a] ;
        if ( isEnzymeVisible ( c->e->name ) )
           {
           c->linearUpdate ( w , h ) ;
           int xx = lineOff + c->pos*lineLen/l ;
           dc.DrawLine ( c->lastrect.GetRight() , c->lastrect.GetBottom() , xx , lineH - markH ) ;
           dc.DrawLine ( xx , lineH , xx , lineH - markH - 1 ) ;
           sprintf ( t , "%s" , c->e->name.c_str() ) ;
           dc.DrawText ( t , c->lastrect.x , c->lastrect.y ) ;
           }
        }

}

void PlasmidCanvas::drawLinearORFs ( wxDC &dc )
    {
    int a ;
    int l = p->vec->sequence.length() ;
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
        wxBrush brush ( col , wxSOLID ) ;
        wxPen pen ( col , 1 , wxSOLID ) ;
        dc.SetBackgroundMode ( wxSOLID ) ;
        dc.SetBrush ( brush ) ;
        dc.SetPen ( pen ) ;

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
        dc.DrawLine ( mf , mh , mt , mh ) ;
        dc.DrawLine ( mt , mh , mt - dir*w/100 , mh - ah/4 ) ;
        dc.DrawLine ( mt , mh , mt - dir*w/100 , mh + ah/4 ) ;
        dc.DrawCircle ( mf , mh , ah/4 ) ;

        dc.SetBackgroundMode ( wxTRANSPARENT ) ;    
        dc.SetBrush ( *wxTRANSPARENT_BRUSH ) ;    
        }
    dc.SetPen(*wxBLACK_PEN);
    }
    
void PlasmidCanvas::arrangeRestrictionSitesLinear ( int a , wxDC &dc )
    {
    TRestrictionCut *c = &p->vec->rc[a] ;
    int x1 , y1 , x2 , y2 ;
    char t[1000] ;
    wxCoord dx , dy ;
    sprintf ( t , "%s" , c->e->name.c_str() ) ;
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
    wxFont normalFont ( 12 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    int r1b = r2 - ( r2 - r1 ) / 10 ;
    int y1 = (int) ( a2 - a1 ) / 3 ;
    int y2 = (int) ( a1 + a2 ) / 2 ;
    vector <wxPoint> p ;
    p.push_back ( wxPoint ( r1  , y2 - y1 ) ) ;
    p.push_back ( wxPoint ( r1b , y2 - y1 ) ) ;
    p.push_back ( wxPoint ( r1b , (int) a1 ) ) ;
    p.push_back ( wxPoint ( r2  , y2 ) ) ;
    p.push_back ( wxPoint ( r1b , (int) a2 ) ) ;
    p.push_back ( wxPoint ( r1b , y2 + y1 ) ) ;
    p.push_back ( wxPoint ( r1  , y2 + y1 ) ) ;

    // Drawing polygon
    wxPoint *wp ;
    wp = (wxPoint*) malloc ( sizeof ( wxPoint ) * (p.size()+1) ) ;
    for ( int b = 0 ; b < p.size() ; b++ ) wp[b] = p[b] ;
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush ( *i->getBrush() ) ;
    dc.DrawPolygon ( p.size() , wp , 0 , 0 ) ;
    free ( wp ) ;
    
    // Name
    char t[1000] ;
    wxCoord dx , dy ;
    wxColor fc = dc.GetTextForeground () ;
    dc.SetTextForeground ( i->getFontColor() ) ;
    dc.SetFont(normalFont);
    sprintf ( t , "%s" , i->name.c_str() ) ;
    dc.GetTextExtent ( t , &dx , &dy ) ;
    dc.DrawText ( t ,
                  (r2+r1)/2 - dx/2 ,
                  (int) a2 - y1 + 1 ) ;
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

void PlasmidCanvas::OnEventLinear(wxMouseEvent& event)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    wxPoint pt(event.GetLogicalPosition(dc));
    wxPoint pt_abs(event.GetPosition());
    
    int vo = -1 ;
    int rs = findRestrictionSite ( pt.x , pt.y ) ;
    int orf = findORFlinear ( pt.x , pt.y ) ;
    string s ;
    int lineOff = w/20 ;
    
    int a ;
    wxPoint pp ;
    pp.x = pt.x * STANDARDRADIUS / w ;
    pp.y = pt.y * STANDARDRADIUS / h ;
    for ( a = 0 ; a < p->vec->items.size() ; a++ )
        {
        TVectorItem i = p->vec->items[a] ;
        wxRect rra ( i.r1 , (int)i.a1 , i.r2-i.r1 , (int)i.a2-i.a1 ) ;
        wxRect rrb ( i.r3 , (int)i.a1 , i.r4-i.r3 , (int)i.a2-i.a1 ) ;
        if ( pointinrect ( pp.x , pp.y , rra ) &&
                p->vec->items[a].isVisible() ) vo = a ;
        if ( i.r3 != -1 && pointinrect ( pp.x , pp.y , rrb ) ) vo = a ;
        }

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

    int bp = -1 ;
    if ( pt.x >= lineOff && pt.x <= w - lineOff )
        {
        bp = pt.x - lineOff ;
        bp = bp * ( p->vec->sequence.length() - 1 ) / ( w - lineOff * 2 ) + 1 ;
        }

    if ( lastbp != -1 )
        {
        lastrestrictionsite = rs = -1 ;
        lastvectorobject = vo = -1 ;
        }
    
    // Passing over
    if ( rs != -1 )
        {
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        s = p->vec->rc[rs].e->name ;
        wxLogStatus(txt("rsite_status_bar") , s.c_str() ) ;
        char ttt[1000] ;
        sprintf ( ttt , txt("tt_rs") , s.c_str() , p->vec->countCuts ( s ) ) ;
        SetMyToolTip ( ttt , TT_RS ) ;
        if ( event.LeftDown() )
           {
           if ( p->def == "dna" ) p->treeBox->SelectItem ( p->treeBox->GetParent ( p->vec->rc[rs].treeid ) ) ;
           }
        else if ( event.MiddleDown() )
           {
           TRestrictionEditor ed ( myapp()->frame , "" , wxPoint(-1,-1) , wxSize(600,400) , 
                      wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL);
           ed.pre = s.c_str() ;
           ed.cocktail = p->vec->cocktail ;
           ed.remoteCocktail = &p->vec->cocktail ;
           ed.initme ( p->vec ) ;
           if ( ed.ShowModal () == wxID_OK )
              {
              p->vec->cocktail = ed.cocktail ;
              p->vec->doAction() ;
              }
           }
        else if ( event.RightDown() )
           invokeRsPopup ( rs , pt_abs ) ;
        else if ( event.LeftDClick() )
           invokeVectorEditor ( "enzyme" , rs ) ;
        }
    else if ( vo != -1 )
        {
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        s = p->vec->items[vo].name ;
        wxLogStatus(txt("item_status_bar") , s.c_str() ) ;
        char ttt[1000] ;
        sprintf ( ttt , "itemtype%d" , p->vec->items[vo].type ) ;
        string tt_type = txt(ttt) ;
        sprintf ( ttt , txt("tt_item") , tt_type.c_str() ,
                                        s.c_str() , 
                                        p->vec->items[vo].desc.c_str() ) ;
        SetMyToolTip ( ttt , TT_ITEM ) ;
        if ( event.LeftDown() )
           {
           if ( p->def == "dna" )
              p->treeBox->SelectItem ( p->vec->items[vo].getTreeID() ) ;
           }
        else if ( event.RightDown() )
             invokeItemPopup ( vo , pt_abs ) ;
        else if ( event.LeftDClick() )
           invokeVectorEditor ( "item" , vo ) ;
        else if ( event.MiddleDown() )
           {
           wxCommandEvent dummyEvent ;
           context_last_item = vo ;
           itemMarkShow ( dummyEvent ) ;
//           p->cSequence->mark ( "DNA" , p->vec->items[vo].from , p->vec->items[vo].to ) ;
//           p->OnCopyToNew ( dummyEvent ) ;
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
        if ( bp != -1 ) wxLogStatus(txt("bp"), bp ) ;
        if ( event.LeftDClick() ) invokeVectorEditor () ;
        else if ( event.RightDown() ) invokeVectorPopup ( pt_abs ) ;
        }

    string id = "dna" ;
    if ( p->def == "AminoAcids" ) id = "AA" ;
    
    // Dragging
    if ( event.Dragging() && event.LeftIsDown() && lastrestrictionsite != -1 )
        {
        SetCursor(wxCursor(wxCURSOR_HAND)) ;
        rs = lastrestrictionsite ;

        wxRect lr = p->vec->rc[rs].lastrect ;
        wxPoint np ( lr.GetRight() , lr.GetBottom() ) ;
        np.x -= ( lr.GetLeft() + lr.GetRight() ) / 2 - pt.x ;
        np.y -= ( lr.GetTop() + lr.GetBottom() ) / 2 - pt.y ;
        np.x = np.x * STANDARDRADIUS / w ;
        np.y = np.y * STANDARDRADIUS / h ;
        
        if ( initialclick )
           {
           mousediffx = p->vec->rc[rs].lp.x - np.x ;
           mousediffy = p->vec->rc[rs].lp.y - np.y ;
           initialclick = false ;
           }
        np.x += mousediffx ;
        np.y += mousediffy ;
        
        p->vec->rc[rs].lp = np ;
        p->vec->rc[rs].linearUpdate ( w , h ) ;
        
        Refresh () ;
        }
    else if ( event.Dragging() && event.LeftIsDown() && lastvectorobject != -1 )
       {
       SetCursor(wxCursor(wxCURSOR_HAND)) ;
       vo = lastvectorobject ;
       float hh = p->vec->items[vo].a2 - p->vec->items[vo].a1 ;
       p->vec->items[vo].a1 = pp.y - hh/2 ;
       p->vec->items[vo].a2 = p->vec->items[vo].a1 + hh ;
       Refresh () ;
       }
    else if ( event.LeftIsDown() && orf != -1 )
        {
        p->cSequence->mark ( id ,
                                p->vec->worf[orf].from + 1 ,
                                p->vec->worf[orf].to + 1 ) ;
        }
    else if ( event.RightDown() && orf != -1 )
        {
        p->cSequence->mark ( id ,
                                p->vec->worf[orf].from + 1 ,
                                p->vec->worf[orf].to + 1 ) ;
        invokeORFpopup ( orf , pt_abs ) ;
        }
    else if ( event.LeftIsDown() && rs == -1 && vo == -1 )
       {
       if ( initialclick && lastbp == -1 ) lastbp = bp ;
       if ( lastbp != -1 ) 
          {
          if ( lastbp > bp ) p->cSequence->mark ( id , bp , lastbp ) ;
          else p->cSequence->mark ( id , lastbp , bp ) ;
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
