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
    wxString t ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;
    int xa , ya , yb ;
    dc.GetDeviceOrigin ( &xa , &ya ) ;
    ya = -ya ;
    can->MyGetClientSize ( &xa , &yb ) ;
    yb += ya ;
    bool newline = true ;
    
    wxArrayInt li , lx , used ;
    li.Alloc ( pl.size() ) ;
    lx.Alloc ( pl.size() ) ;
    used.Alloc ( pl.size() ) ;
    while ( li.GetCount() < pl.size() ) li.Add ( -1 ) ;
    while ( lx.GetCount() < pl.size() ) lx.Add ( -1 ) ;
    while ( used.GetCount() < pl.size() ) used.Add ( 0 ) ;

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
           myass ( used.GetCount() >= pl.size() , "SeqFeature::show_1" ) ;

           for ( i = 0 ; i < pl.size() ; i++ ) used[i] = 0 ;
           for ( i = 0 ; i < pl.size() ; i++ )
              {
              if ( doesHit ( i , b ) )
                 {
                 int l = pl.getLevel ( i ) ; //vr[i].y ;
                 myass ( maxlayers >= l , "SeqFeature::show_2" ) ;
                 myass ( l < used.GetCount() , "SeqFeature::show_3" ) ;
                 myass ( l < lx.GetCount() , "SeqFeature::show_4" ) ;
                 myass ( can , "SeqFeature::show_5" ) ;
                 myass ( maxlayers+1 != 0 , "SeqFeature::show_6" ) ;
                 used[l] = 1 ;
                 int x_from = lx[l] ;
                 if ( x_from < 0 ) x_from = tx ;
                 int x_to = tx + can->charwidth ;
                 int level = can->charheight ;
                 int lh = level * l / ( maxlayers + 1 ) ;
                 level = ty + level - lh - 1 ;
                 lh = ( ( can->charheight / 2 ) / ( maxlayers + 1 ) ) ;
                 if ( lh < 5 ) lh = 5 ;

                 if ( insight )
                    {
                    int mode = atoi((vec->items[pl.getID(i)/*vr[i].GetX()*/].getParam("SEQUENCE_STYLE")).c_str()) ;
                    wxColour col = *wxBLACK ;
                    
                    if ( can->isPrinting() && !can->getPrintToColor() ) dc.SetPen ( *wxBLACK_PEN ) ;
                    else
                       {
                       col = vec->items[pl.getID(i)/*vr[i].GetX()*/].getFontColor() ;
                       dc.SetPen ( *MYPEN ( col ) ) ;
                       }

                    // Offsets
                    if ( aaa && !newline && 
                         b-1 < aaa->offsets.GetCount() && 
                         aaa->offsets[b-1] != -1 && 
                         aaa->offset_items[b-1] == &vec->items[pl.getID(i)/*vr[i].GetX()*/] )
                       {
                       myass ( b-1 >= 0 && b-1 < aaa->offsets.GetCount() , "SeqFeature::show_9" ) ;
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
                    }

                 myass ( l >= 0 && l < li.GetCount() , "!!1" ) ;   
                 if ( newline || li[l] != i ) // Start of new item on this lane
                    {
                    if ( insight )
                       {
                       myass ( i >= 0 && i < pl.size() , "!!2" ) ;   
                       wxString name = vec->items[pl.getID(i)/*vr[i].GetX()*/].name ;
                       if ( newline && li[l] == i ) name = "(" + name + ")" ;
                       _i.Add ( i ) ;
                       _name.Add ( name ) ;
                       _point.push_back ( wxPoint ( x_from , level - ch2 ) ) ;
                       }
                    li[l] = i ;
                    }
                 lx[l] = x_to ;
                 }
              }


           for ( i = 0 ; i < pl.size() ; i++ )
              {
              myass ( i >= 0 && i <= used.GetCount() , "!!4" ) ;
              myass ( i >= 0 && i <= lx.GetCount() , "!!5" ) ;
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
                  myass ( _i[i] >= 0 && _i[i] < pl.size() , "SeqFeature::show_names_2" ) ;
                  wxColour col = vec->items[pl.getID(_i[i])/*vr[_i[i]].GetX()*/].getFontColor() ;
                  dc.SetTextForeground ( col ) ;
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
           for ( i = 0 ; i < pl.size() ; i++ )
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
    vec = v ;
    int a ;
    s = vec->getSequence() ;
    pl.slen = s.length() ;
    pl.prepare ( vec->items.size() ) ;    
    for ( a = 0 ; a < vec->items.size() ; a++ )
        {
        if ( vec->items[a].isVisible() )
           pl.add ( a , vec->items[a].from , vec->items[a].to ) ;
        }    
    pl.makeLevels () ;
    maxlayers = pl.maxlevels ;
    }
    
bool SeqFeature::collide ( int a , int b )
    {/*
    myass ( a < vr.size() , "SeqFeature::collide" ) ;
    myass ( b < vr.size() , "SeqFeature::collide" ) ;
    if ( vr[a].y != vr[b].y ) return false ;
    int i ;
    for ( i = 0 ; i < vec->getSequenceLength() ; i++ )
       if ( doesHit ( a , i+1 ) && doesHit ( b , i+1 ) )
          return true ;*/
    return false ;
    }
    
bool SeqFeature::doesHit ( int a , int x )
    {
    int from = pl.getFrom ( a ) ; //vr[a].width ;
    int to = pl.getTo ( a ) ; //vr[a].height ;
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
    
