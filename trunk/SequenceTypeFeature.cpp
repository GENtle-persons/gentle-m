#include "SequenceCanvas.h"

//************************************************ SeqFeature

void SeqFeature::show ( wxDC& dc )
    {
    int cw2 , ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( "A" , &cw2 , &ch2 ) ;
//    dc.SetFont(*can->font);
    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    int a , b , cnt = offset+1 ;
    string t ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    bool newline = true ;
    
    wxArrayInt li , lx , used ;
    while ( li.GetCount() < vr.size() ) li.Add ( -1 ) ;
    while ( lx.GetCount() < vr.size() ) lx.Add ( -1 ) ;
    while ( used.GetCount() < vr.size() ) used.Add ( 0 ) ;

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
        else if ( ty > yb ) a = pos.p.GetCount() ;
        if ( b > 0 ) // Character
           {
           t = " " ;
           int i ;
           wxArrayInt _i ;
           wxArrayString _name ;
           vector <wxPoint> _point ;
           myass ( vec , "SeqFeature::show_0" ) ;
           myass ( used.size() >= vr.size() , "SeqFeature::show_1" ) ;

           for ( i = 0 ; i < vr.size() ; i++ ) used[i] = 0 ;
           for ( i = 0 ; i < vr.size() ; i++ )
              {
              if ( doesHit ( i , b ) )
                 {
                 int l = vr[i].y ;
                 myass ( maxlayers >= l , "SeqFeature::show_2" ) ;
                 myass ( l < used.size() , "SeqFeature::show_3" ) ;
                 myass ( l < lx.size() , "SeqFeature::show_4" ) ;
                 myass ( can , "SeqFeature::show_5" ) ;
                 myass ( maxlayers+1 != 0 , "SeqFeature::show_6" ) ;
                 used[l] = 1 ;
                 int x_from = lx[l] ;
                 if ( x_from < 0 ) x_from = tx ;
                 int x_to = tx + can->charwidth ;
                 int level = can->charheight ;
                 int lh = level * l / ( maxlayers + 1 ) ;
                 level = ty + level - lh - 1 ;

                 if ( insight )
                    {
                    myass ( vr[i].GetX() >= 0 , "SeqFeature::show_7" ) ;
                    myass ( vr[i].GetX() < vec->items.size() , "SeqFeature::show_8" ) ;
                    int mode = atoi((vec->items[vr[i].GetX()].getParam("SEQUENCE_STYLE")).c_str()) ;
                    wxColour col = *wxBLACK ;
                    wxPen *thisPen = wxBLACK_PEN ;
                    
                    if ( can->isPrinting() && !can->getPrintToColor() ) dc.SetPen ( *wxBLACK_PEN ) ;
                    else
                       {
                       col = vec->items[vr[i].GetX()].getFontColor() ;
                       int pw = 1 ;
                       thisPen = new wxPen ( col , pw , wxSOLID ) ;
                       }
                    dc.SetPen ( *thisPen ) ;

                    // Offsets
                    if ( aaa && !newline && 
                         b-1 < aaa->offsets.GetCount() && 
                         aaa->offsets[b-1] != -1 && 
                         aaa->offset_items[b-1] == &vec->items[vr[i].GetX()] )
                       {
                       myass ( b-1 >= 0 && b-1 < aaa->offsets.size() , "SeqFeature::show_9" ) ;
                       dc.SetTextForeground ( col ) ;
                       wxString pn = wxString::Format ( "%d" , aaa->offsets[b-1] ) ;
                       int u1 , u2 ;
                       dc.GetTextExtent ( pn , &u1 , &u2 ) ;
                       dc.DrawText ( pn , x_to - can->charwidth , level - u2 ) ;
                       }

                    myass ( can && can->charwidth , "SeqFeature::show_10" ) ;
                    if ( mode == FEAT_ALPHA ) // Alpha helix
                       {
                       int px , py = 0 ;
                       for ( px = x_from ; px <= x_to ; px++ )
                          {
                          int npy ;//= ( px % lh ) - lh/2 ;
                          npy = (float) sin ( (float) ( px - x_from ) * PI * 4 / can->charwidth ) * lh / 2 ;
                          dc.DrawLine ( px , level + py , px , level + npy ) ;
                          py = npy ;
                          }
                       }
                    else if ( mode == FEAT_BETA ) // Beta sheet
                       {
                       int o ;
                       for ( o = -lh/4 ; o <= lh/4 ; o++ )
                          {
                          dc.DrawLine ( x_from , level+o , x_to , level+o ) ;
                          dc.DrawLine ( x_to , level+o , x_to + cw2/2 , level ) ;
                          }
                       }
                    else if ( mode == FEAT_MUT ) // Mutation
                       {
                       int xw = can->charwidth / 2 ;
                       int yh = level - ty + lh ;
                       int xm = x_to - xw ;
                       int yo = yh / 4 ;
                       dc.DrawLine ( xm , level , xm , ty + yh ) ;
                       dc.DrawLine ( xm , level , xm - xw , level + yo ) ;
                       dc.DrawLine ( xm , level , xm + xw , level + yo ) ;
                       dc.DrawLine ( xm , ty + yh , xm - xw , ty + yh - yo ) ;
                       dc.DrawLine ( xm , ty + yh , xm + xw , ty + yh - yo ) ;
                       }
                    else if ( mode == FEAT_NOLINE ) // No line
                       {
                       // That's right boys, no line is drawn here!
                       }
                    else if ( mode == FEAT_VLINE ) // Vertical line
                       {
                       int xw = can->charwidth / 2 ;
                       int yh = level - ty + lh ;
                       int xm = x_to - xw ;
                       int yo = yh / 4 ;
                       dc.DrawLine ( xm , level , xm , ty + yh ) ;
                       }
                    else if ( mode == FEAT_NONE ) // Default
                       {
                       dc.DrawLine ( x_from , level , x_to , level ) ;
                       }
                       
                    dc.SetPen ( *wxBLACK_PEN ) ;
                    if ( thisPen != wxBLACK_PEN ) delete thisPen ;
                    }

                 myass ( l >= 0 && l < li.size() , "!!1" ) ;   
                 if ( newline || li[l] != i ) // Start of new item on this lane
                    {
                    if ( insight )
                       {
                       myass ( i >= 0 && i < vr.size() , "!!2" ) ;   
                       myass ( vr[i].x >= 0 && vr[i].x < vec->items.size() , "!!3" ) ;   
                       string name = vec->items[vr[i].x].name.c_str() ;
                       if ( newline && li[l] == i ) name = "(" + name + ")" ;
                       _i.Add ( i ) ;
                       _name.Add ( name.c_str() ) ;
                       _point.push_back ( wxPoint ( x_from , level - ch2 ) ) ;
                       }
                    li[l] = i ;
                    }
                 lx[l] = x_to ;
                 }
              }


           for ( i = 0 ; i < vr.size() ; i++ )
              {
              myass ( i >= 0 && i <= used.size() , "!!4" ) ;
              myass ( i >= 0 && i <= lx.size() , "!!5" ) ;
              if ( !used[i] && lx[i] >= 0 )
                 lx[i] = -1 ;
              }

              
           // Now drawing the names
           for ( i = 0 ; i < _i.GetCount() ; i++ )
              {
               if ( can->isPrinting() && !can->getPrintToColor() )
                  {
                  dc.SetTextForeground ( *wxBLACK ) ;
                  }
               else
                  {
                  myass ( i >= 0 && i < _i.GetCount() , "SeqFeature::show_names_1" ) ;
                  myass ( _i[i] >= 0 && _i[i] < vr.size() , "SeqFeature::show_names_2" ) ;
                  myass ( vr[_i[i]].GetX() >= 0 && vr[_i[i]].GetX() < vec->items.size() , "SeqFeature::show_names_3" ) ;
                  wxColour col = vec->items[vr[_i[i]].GetX()].getFontColor() ;
                  dc.SetTextForeground ( col ) ;
//                  dc.SetTextForeground ( pens[vr[_i[i]].y].GetColour() ) ;
                  }
               myass ( i >= 0 && i < _name.GetCount() , "SeqFeature::show_names_4" ) ;
               myass ( i >= 0 && i < _point.size() , "SeqFeature::show_names_5" ) ;
               if ( _name[i] != "()" )
                  dc.DrawText ( _name[i] , _point[i] ) ;
              }


           newline = false ;
           cnt++ ;
           }
        else // New line
           {
           newline = true ;
           int i ;
           for ( i = 0 ; i < vr.size() ; i++ )
              if ( lx[i] >= 0 )
                 lx[i] = -2 ;
           }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    }
    
void SeqFeature::initFromTVector ( TVector *v )
    {
    // misusing the wxRect class to store :
    // item ID as x
    // item display level as y
    // item.from as width
    // item.to as height
    int a , b ;
    vec = v ;
    s = vec->getSequence() ;
    maxlayers = 0 ;
    for ( a = 0 ; a < s.length() ; a++ ) s[a] = ' ' ;
    while ( vr.size() ) vr.pop_back () ;
    for ( a = 0 ; a < v->items.size() ; a++ )
       {
       if ( v->items[a].isVisible() )
          vr.push_back ( wxRect ( a , 0 , v->items[a].from , v->items[a].to ) ) ;
       }
       
    // Sorting, largest features first
    for ( a = 0 ; a+1 < vr.size() ; a++ )
       {
       b = a + 1 ;
       int la = vr[a].height - vr[a].width ;
       int lb = vr[b].height - vr[b].width ;
       if ( vr[a].height < vr[a].width ) la += s.length() ;
       if ( vr[b].height < vr[b].width ) lb += s.length() ;
       if ( la < lb )
          {
          wxRect r = vr[a] ;
          vr[a] = vr[b] ;
          vr[b] = r ;
          a = 0 ;
          }
       }
       
    // Aligning
    for ( a = 0 ; a < vr.size() ; a++ )
       {
       for ( b = 0 ; b < a ; b++ )
          {
          if ( collide ( a , b ) )
             {
             vr[a].y++ ;
             if ( vr[a].y > maxlayers ) maxlayers = vr[a].y ;
             b = -1 ;
             }
          }
       }
       
    pens.clear() ;
    pens.push_back ( *wxBLACK_PEN ) ;
    pens.push_back ( *wxRED_PEN ) ;
    pens.push_back ( *MYPEN(wxColour(0,100,0)) ) ;
    for ( a = 0 ; pens.size() <= maxlayers ; a++ ) pens.push_back ( pens[a] ) ;
    }
    
bool SeqFeature::collide ( int a , int b )
    {
    myass ( a < vr.size() , "SeqFeature::collide" ) ;
    myass ( b < vr.size() , "SeqFeature::collide" ) ;
    if ( vr[a].y != vr[b].y ) return false ;
    int i ;
    for ( i = 0 ; i < vec->getSequenceLength() ; i++ )
       if ( doesHit ( a , i+1 ) && doesHit ( b , i+1 ) )
          return true ;
    return false ;
    }
    
bool SeqFeature::doesHit ( int a , int x )
    {
    myass ( a < vr.size() , "SeqFeature::doesHit" ) ;
    int from = vr[a].width ;
    int to = vr[a].height ;
    if ( from <= to )
       {
       return ( x >= from && x <= to ) ;
       }
    else
       {
//       return false ;
       return ( x <= to || x >= from ) ;
       }
    }
    
