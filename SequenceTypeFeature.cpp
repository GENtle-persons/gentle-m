/** \file
	\brief Contains the SeqFeature class members
*/
#include "SequenceCanvas.h"

void SeqFeature::show ( wxDC& dc )
    {
    myass ( can , "SeqFeature::show can==NULL" ) ;
    myass ( vec , "SeqFeature::show vec==NULL" ) ;
    myass ( can->smallFont , "SeqFeature::show can->smallFont==NULL" ) ;
    mylog ( "SeqFeature::show" , "BEGIN" ) ;
    int cw2 , ch2 ;
    dc.SetFont(*can->smallFont);
    dc.GetTextExtent ( _T("A") , &cw2 , &ch2 ) ;
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
    li.Alloc ( pl.maxlevels ) ;
    lx.Alloc ( pl.maxlevels ) ;
    used.Alloc ( pl.maxlevels ) ;
    while ( li.GetCount() < pl.maxlevels ) li.Add ( -1 ) ;
    while ( lx.GetCount() < pl.maxlevels ) lx.Add ( -1 ) ;
    while ( used.GetCount() < pl.maxlevels ) used.Add ( 0 ) ;

    bool drawOffsets = true ;
    if ( can->child && can->child->def == _T("alignment") ) drawOffsets = false ;

    mylog ( "SeqFeature::show" , "ENTERING OUTER LOOP" ) ;
    for ( int l = 0 ; l < pl.maxlevels ; l++ )
        {
        int pcnt = 0 ;
	     mylog ( "SeqFeature::show" , wxString::Format ( _T("Outer loop (%d of %d)") , l , pl.maxlevels ) ) ;
        for ( a = 0 ; a < pos.r.size() ; a++ )
            {
            if ( can->hardstop > -1 && a > can->hardstop ) break ;
            b = pcnt+1 ;//pos.p[a] ;
            if ( a == 0 || ( a > 0 && pos.r[a].y != pos.r[a-1].y ) ) b = -1 ;
            else pcnt++ ;
            int tx = pos.r[a].x , ty = pos.r[a].y ;
            int tz = ty + can->charheight ;
            bool insight = true ; // Meaning "is this part visible"
            if ( tz < ya ) insight = false ;
            if ( ty > yb ) insight = false ;
            if ( can->getDrawAll() ) insight = true ;
            else if ( ty > yb ) a = pos.r.size() ;
            if ( b > 0 ) // Character
               {
               t = _T(" ") ;
               int i ;
               wxArrayInt _i ;
               wxArrayString _name ;
               vector <wxPoint> _point ;

               for ( i = 0 ; i < pl.maxlevels ; i++ ) used[i] = 0 ;
               i = pl.here ( b-1 , l ) ;

               if ( i != -1 )
                 {
                 myass ( l < used.GetCount() , "SeqFeature::show 1" ) ;
                 myass ( l < lx.GetCount() , "SeqFeature::show 2" ) ;
                 myass ( maxlayers != -1 , "SeqFeature::show 3" ) ;
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
                    myass ( vec , "UGH!" ) ;
                    int myid = pl.getID(i) ;
                    if ( myid >= vec->items.size() ) continue ; // Patchy
                    wxString tmp = vec->items[myid].getParam ( _T("SEQUENCE_STYLE") ) ;
                    long mode ;
                    tmp.ToLong ( &mode ) ;
                    wxColour col = *wxBLACK ;

                    if ( can->isPrinting() && !can->getPrintToColor() ) dc.SetPen ( *wxBLACK_PEN ) ;
                    else
                       {
                       col = vec->items[myid].getFontColor() ;
                       wxPen *mypen = MYPEN ( col ) ;
                       myass ( mypen , "Pen is NULL" ) ;
                       dc.SetPen ( *mypen ) ;
                       }

/*                    if ( aaa && !newline &&
                         b-1 < aaa->offsets.GetCount()  &&
                         aaa->offsets[b-1] != -1 &&
                         aaa->offset_items[b-1] == &vec->items[pl.getID(i)] )*/

                    // Offsets
                    if ( drawOffsets )
                       {
                       int o = -1 ;
                       if ( (b-1) % 10 == 0 && !newline && vec->items[pl.getID(i)].from != b )
                          {
                          if ( can->getAA() )
                             {
                             TVectorItem *ip = &vec->items[pl.getID(i)] ;
                             myass ( ip , "SeqFeature::show ip==NULL" ) ;
                             if ( ip->getOffset () != -1 )
                             	o = b + ip->getOffset() - ip->from ;
                             }
                          else
                          	{
                               o = vec->items[pl.getID(i)].getOffsetAt ( b-1 ) ;
                            }
                          }
                       if ( o != -1 )
                          {
                          dc.SetTextForeground ( col ) ;
                          wxString pn = wxString::Format ( _T("%d") , o ) ;
                          int u1 , u2 ;
                          dc.GetTextExtent ( pn , &u1 , &u2 ) ;
                          dc.DrawText ( pn , x_to - can->charwidth , level - u2 ) ;
                          }
                       }

                    if ( mode == FEAT_ALPHA ) // Alpha helix
                       {
                       int px , py = 0 ;
                       for ( px = x_from ; px <= x_to ; px++ )
                          {
                          int npy ;
                          npy = (int)((float) sin ( (float) ( px - x_from ) * PI * 4 / can->charwidth ) * lh / 2 );
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
                       dc.DrawLine ( xm , level , xm , ty + yh ) ;
                       }
                    else if ( mode == FEAT_NONE ) // Default
                       {
                       dc.DrawLine ( x_from , level , x_to , level ) ;
                       }

                    dc.SetPen ( *wxBLACK_PEN ) ;
                    }

                 if ( newline || li[l] != i ) // Start of new item on this lane
                    {
                    if ( insight )
                       {
                       wxString name = vec->items[pl.getID(i)].name ;
                       if ( newline && li[l] == i ) name = _T("(") + name + _T(")") ;
                       _i.Add ( i ) ;
                       _name.Add ( name ) ;
                       _point.push_back ( wxPoint ( x_from , level - ch2 ) ) ;
                       }
                    li[l] = i ;
                    }
                 lx[l] = x_to ;
                 }

               for ( i = 0 ; i < pl.maxlevels ; i++ )
                  {
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
                      wxColour col = vec->items[pl.getID(_i[i])/*vr[_i[i]].GetX()*/].getFontColor() ;
                      dc.SetTextForeground ( col ) ;
                      }
                   if ( _name[i] != _T("()") )
                      {
                      dc.DrawText ( _name[i] , _point[i] ) ;
                      }
                  }


               newline = false ;
               cnt++ ;
               }
            else // New line
               {
               newline = true ;
               int i ;
               for ( i = 0 ; i < pl.maxlevels ; i++ )
                  if ( lx[i] >= 0 )
                     lx[i] = -2 ;
               }
            }
        }
    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
    mylog ( "SeqFeature::show" , "END" ) ;
    }

void SeqFeature::initFromTVector ( TVector *v )
    {
    vec = v ;
    int a ;

    s = vec->getSequence() ;
    if ( can && can->getEditMode() && v->getSequenceLength() &&
            v->getSequenceChar(v->getSequenceLength()-1) == ' ' )
       {
       s.Truncate ( s.length()-1 ) ;
       }

    pl.slen = s.length() ;
    pl.prepare ( vec->items.size() ) ;
    for ( a = 0 ; a < vec->items.size() ; a++ )
        {
        if ( vec->items[a].isVisible() )
           pl.add ( a , vec->items[a].from-1 , vec->items[a].to-1 ) ;
        }
    pl.makeLevels () ;
    maxlayers = pl.maxlevels ;
    }

bool SeqFeature::collide ( int a , int b )
    {
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
       return ( x <= to || x >= from ) ;
       }
    }

