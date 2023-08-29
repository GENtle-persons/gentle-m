#include "CoolCanvas.h"
/*
CoolCanvas::CoolCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
    : PlasmidCanvas ( parent , pos , size )
    {

    }

int CoolCanvas::x2D ( int x , int y , int z )
    {
    return x + z + w/2 ;
    }

int CoolCanvas::y2D ( int x , int y , int z )
    {
    return y + z + h/2 ;
    }

wxPoint CoolCanvas::xy2D ( int x , int y , int z )
    {
    return wxPoint ( x2D ( x , y , z ) , y2D ( x , y , z ) ) ;
    }

void CoolCanvas::OnDrawCircular(wxDC& dc)
    {
    int mwh = w<h?w:h ;
    if ( printing ) mwh -= mwh / 5 ;
    int w2 = w * 4 / 10 ;
    int h2 = h * 3 / 10 ;
    string s = p->vec->getSequence() ;

    int a ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        float deg = a * 360 ;
        deg /= s.length() ;
        int x = deg2x ( deg , w2 ) ;
        int y = deg2y ( deg , h2 ) ;

        if ( y > h2*2/3 )
           {
           wxString text ;
           text += s[a] ;
           dc.DrawText ( text , xy2D ( x , 0 , y ) ) ;
           }
        else
           {
           dc.DrawPoint ( xy2D ( x , 0 , y ) ) ;
           }

        }
    }
*/
