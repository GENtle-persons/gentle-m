#include "SequenceCanvas.h"

//************************************************ SeqFeature

void SeqFeature::show ( wxDC& dc )
    {
    int cw2 , ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( "A" , &cw2 , &ch2 ) ;
//    dc.SetFont(*can->font);
    wxBrush theLightBrush ( wxColour ( 230 , 230 , 230 ) , wxSOLID ) ;
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
    
    vector <int> li , lx , used ;
    while ( li.size() < vr.size() ) li.push_back ( -1 ) ;
    while ( lx.size() < vr.size() ) lx.push_back ( -1 ) ;
    while ( used.size() < vr.size() ) used.push_back ( 0 ) ;

    for ( a = 0 ; a < pos.p.size() ; a++ )
        {
        b = pos.p[a] ;
        int tx = pos.r[a].x , ty = pos.r[a].y ;
        int tz = ty + can->charheight ;
        bool insight = true ; // Meaning "is this part visible"
        if ( tz < ya ) insight = false ;
        if ( ty > yb ) insight = false ;
        if ( can->drawall ) insight = true ;
        else if ( ty > yb ) a = pos.p.size() ;
        if ( b > 0 ) // Character
           {
           t = " " ;
           int i ;
           vector <int> _i ;
           vector <string> _name ;
           vector <wxPoint> _point ;
           for ( i = 0 ; i < vr.size() ; i++ ) used[i] = 0 ;
           for ( i = 0 ; i < vr.size() ; i++ )
              {
              if ( doesHit ( i , b ) )
                 {
                 int l = vr[i].y ;
                 myass ( maxlayers >= l , "Oh no!" ) ;
                 used[l] = 1 ;
                 int x_from = lx[l] ;
                 if ( x_from < 0 ) x_from = tx ;
                 int x_to = tx + can->charwidth ;
                 int level = can->charheight ;
                 int lh = level * l / ( maxlayers + 1 ) ;
                 level = ty + level - lh - 1 ;
                 if ( insight )
                    {
                    int mode = atoi((vec->items[vr[i].GetX()].getParam("SEQUENCE_STYLE")).c_str()) ;
                    wxColour col ;
                    
                     if ( can->printing && !can->printToColor ) dc.SetPen ( *wxBLACK_PEN ) ;
                     else
                        {
                        col = vec->items[vr[i].GetX()].getFontColor() ;
                        int pw = 1 ;
//                        if ( mode == 1 ) pw = 2 ;
                        dc.SetPen ( wxPen ( col , pw , wxSOLID ) ) ;
//                        dc.SetPen ( pens[l] ) ;
                        }

                    // Offsets
                    if ( aaa && !newline && 
                         b-1 < aaa->offsets.size() && 
                         aaa->offsets[b-1] != -1 && 
                         aaa->offset_items[b-1] == &vec->items[vr[i].GetX()] )
                       {
                       dc.SetTextForeground ( col ) ;
                       wxString pn = wxString::Format ( "%d" , aaa->offsets[b-1] ) ;
                       int u1 , u2 ;
                       dc.GetTextExtent ( pn , &u1 , &u2 ) ;
                       dc.DrawText ( pn , x_to - can->charwidth , level - u2 ) ;
                       }
    
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
                    }
                 if ( newline || li[l] != i ) // Start of new item on this lane
                    {
                    if ( insight )
                       {
                       string name = vec->items[vr[i].x].name.c_str() ;
                       if ( newline && li[l] == i ) name = "(" + name + ")" ;
                       _i.push_back ( i ) ;
                       _name.push_back ( name ) ;
                       _point.push_back ( wxPoint ( x_from , level - ch2 ) ) ;
                       }
                    li[l] = i ;
                    }
                 lx[l] = x_to ;
                 }
              }
           for ( i = 0 ; i < vr.size() ; i++ )
              if ( !used[i] && lx[i] >= 0 )
                 lx[i] = -1 ;
              
           // Now drawing the names
           for ( i = 0 ; i < _i.size() ; i++ )
              {
               if ( can->printing && !can->printToColor )
                  {
                  dc.SetTextForeground ( *wxBLACK ) ;
                  }
               else
                  {
                  wxColour col = vec->items[vr[_i[i]].GetX()].getFontColor() ;
                  dc.SetTextForeground ( col ) ;
//                  dc.SetTextForeground ( pens[vr[_i[i]].y].GetColour() ) ;
                  }
               if ( _name[i] != "()" )
                  dc.DrawText ( _name[i].c_str() , _point[i] ) ;
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
    s = vec->sequence ;
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
//    while ( pens.size() ) pens.pop_back () ;
    pens.push_back ( *wxBLACK_PEN ) ;
    pens.push_back ( *wxRED_PEN ) ;
    pens.push_back ( wxPen ( wxColour ( 0 , 100 , 0 ) , 1 , wxSOLID  ) ) ;
    for ( a = 0 ; pens.size() <= maxlayers ; a++ ) pens.push_back ( pens[a] ) ;
    }
    
bool SeqFeature::collide ( int a , int b )
    {
    if ( vr[a].y != vr[b].y ) return false ;
    int i ;
    for ( i = 0 ; i < vec->sequence.length() ; i++ )
       if ( doesHit ( a , i+1 ) && doesHit ( b , i+1 ) )
          return true ;
    return false ;
    }
    
bool SeqFeature::doesHit ( int a , int x )
    {
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
    
