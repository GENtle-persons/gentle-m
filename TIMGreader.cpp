/** \file
	\brief Contains the TIMGreader class, and its TIMGitem helper class
*/
#include "TIMGreader.h"
#include <wx/image.h>

TIMGreader::~TIMGreader ()
    {
    if ( buffer ) delete buffer ;
    if ( output ) delete output ;
    }

int TIMGreader::getInt ( int adr )
   {
   int a1 = (unsigned char) buffer[adr] ;
   int a2 = (unsigned char) buffer[adr+1] ;
   int r = a1 + a2 * 256 ;
   return r ;
   }
   
void TIMGreader::readFile ( wxString fn )
    {
    wxFile file ( fn ) ;
    size = file.Length() ;
    buffer = new unsigned char [size+5];
    file.Read ( (char*)buffer, size );
    file.Close() ;
    }
    
wxImage TIMGreader::makeImage()
    {
    int w_adr = 3549 ;
    int h_adr = w_adr + 2 ;
    int xoff = 0 , yoff = 0 ;
    
    w = getInt ( w_adr ) ;
    h = getInt ( h_adr ) ;
    
//    xoff = -getInt ( 3577 ) ;
//    yoff = -getInt ( 3577 ) ;
//    yoff = -buffer[3577] ;
      
    int cur = 4089 ;
    output = (unsigned char*) malloc ( w * h * 3 ) ;
    int offset = 0 , x , y ;
    for ( y = 0 ; y < h ; y++ )
       {
       for ( x = 0 ; x < w ; x++ )
          {
          int col = buffer[cur++] ;
          output[offset++] = col ;
          output[offset++] = col ;
          output[offset++] = col ;
          }
       }
    wxImage i ( w , h ) ;
    i.SetData ( output ) ;
    
    // Items
    items.clear() ;
    x = 0 ;
    for ( y = size - 1 ; y > 5 && x < 5 ; y-- )
       {
       for ( x = 0 ; y-x > 0 && x < 5 && buffer[y-x] == 255 ; x++ ) ;
       }
    for ( y++ ; y > 10 && y + 3 < size ; y++ )
       {
       if ( buffer[y] == 0 && buffer[y+1] == buffer[y-1] &&
              buffer[y+2] == buffer[y-2] && buffer[y+3] == buffer[y-3] &&
              buffer[y+1] == buffer[y+2] && buffer[y+2] == buffer[y+3] &&
              buffer[y+1] == 255 )
          { // "FF FF FF 00 FF FF FF" | y at 00
          TIMGitem it ;
          it.type = IMGTYPE_TEXT ;
          
          for ( x = y + 7 ; buffer[x] ; x++ ) ;
          for ( x++ ; buffer[x] == 0 ; x++ ) ;
          
          it.p1.x = getInt ( x + 1 ) + xoff ;
          it.p1.y = getInt ( x + 5 ) + yoff ;
          
          it.orig = wxSize ( w , h ) ;
          for ( x = 7 ; x < 50 && buffer[y+x] ; x++ )
             it.s += buffer[y+x] ;
             
          it.font_size = ( 243 - buffer[y-61] ) / 3 + 10 ;
          it.font_name = wxString ( (char*) buffer + y - 43 , *wxConvCurrent ) ;
             
          items.push_back ( it ) ;
          }
       }
    
    return i ;
    }
    
//

void TIMGitem::draw ( wxDC &dc , int x1 , int y1 , int x2 , int y2 ) 
    {
    cur = wxSize ( x2 - x1 , y2 - y1 ) ;
    off = wxPoint ( x1 , y1 ) ;
    
//    dc.DrawLine ( xx ( 0 ) , yy ( 0 ) , xx ( orig.GetWidth() ) , yy ( orig.GetHeight() ) ) ;
//    wxString u = wxString::Format ( "Image dimensions : %d:%d" , orig.GetWidth() , orig.GetHeight() ) ;
//    dc.DrawText ( u , xx ( 0 ) , yy ( 0 ) ) ;

    if ( type == IMGTYPE_TEXT )
        {
        wxFont oldfont = dc.GetFont() ;
        dc.SetFont ( *MYFONT ( font_size*2/3 , wxMODERN , wxNORMAL , wxNORMAL /*, false , font_name.c_str()*/ ) ) ;

        int px = xx ( p1.x ) ;
        int py = yy ( p1.y ) ;
        
        wxString t = s + _T(" {") + font_name + wxString::Format ( _T(":%d} (%d:%d) [%d:%d]") , 
                                font_size ,
                                p1.x , p1.y , 
                                px , py ) ;

        t = s ; // Back to the roots ;-)
        
        dc.DrawText ( t , px , py ) ;
        dc.SetFont ( oldfont ) ;
        }
    }
    
int TIMGitem::xx ( int x )
    {
    return off.x + x * 19 / 24 ;
    }

int TIMGitem::yy ( int y )
    {
    return off.y + y * 9 / 12 ;
    }

