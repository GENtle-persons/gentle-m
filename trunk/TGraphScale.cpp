#include "TGraph.h"

//******************************************************** TGraphScale
 
TGraphScale::TGraphScale ( float _min , float _max , bool _horizontal , bool _left , wxString _name , wxString _unit , 
				wxColour _col , wxString _type )
	{
 	min = _min ;
   max = _max ;
 	top = max ;
 	bottom = min ;
 	horizontal = _horizontal ;
 	left = _left ;
 	col = *wxBLACK ;
 	name = _name ;
 	unit = _unit ;
 	type = _type ;
 	col = _col ;
 	outline = wxRect ( -1 , -1 , 0 , 0 ) ;
 	selected = false ;
 	show_mark = false ;
	}
	
int TGraphScale::GetWidth ()
	{
 	return horizontal ? 30 : 50 ;
	}
	
wxRect TGraphScale::CalcInternalRect ( wxRect &r )
	{
	wxRect ir = r ;
	int w = GetWidth() ;
	if ( horizontal )
		{
   		if ( left ) // Top
   			{
		    r.y += w ;
   			}
      	else // Bottom
       		{
   		    ir.y += ir.height - w ;
         	}        
      	ir.height = w ;
      	r.height -= w ;
		}
 	else
  		{
   		if ( left ) // Left
   			{
		    r.x += w ;
   			}
      	else // Right
       		{
   		    ir.x += ir.width - w ;
         	}        
      	ir.width = w ;
      	r.width -= w ;
    	}        
   	return ir ;
	}    

void TGraphScale::drawit ( wxDC &dc , wxRect &r , wxRect &inner )
	{
   	wxRect ir = CalcInternalRect ( r ) ;
   	int border = 2 ;
   	
   	if ( horizontal )
   		{
	    if ( ir.x < inner.x ) ir.x = inner.x ;
	    while ( ir.GetRight() > inner.GetRight() ) ir.width-- ;
	    outline = ir ;
	    ir.height -= border*2 ;
	    ir.y += border ;
   		}
    else
    	{
	    if ( ir.y < inner.y ) ir.y = inner.y ;
	    while ( ir.GetBottom() > inner.GetBottom() ) ir.height-- ;
	    outline = ir ;
	    ir.width -= border*2 ;
	    ir.x += border ;
     	}        
   	
   	// Selected? Draw nice background!
   	if ( selected )
   		{
	    dc.SetPen ( *MYPEN ( TGraphDisplay::prettyColor ) ) ;
	    dc.SetBrush ( *MYBRUSH ( TGraphDisplay::prettyColor ) ) ;
	    dc.DrawRectangle ( outline ) ;
	    dc.SetBrush ( *wxTRANSPARENT_BRUSH ) ;
   		}   
   	
   	// Calculate major and minor units
   	float multiplier = 10 ; //( unit == _T("s") ) ? 6 : 10 ;
   	float factor = 1 ;
   	while ( factor <= max ) factor *= multiplier ;
   	float f2 = top - bottom ;
   	while ( factor >= f2 ) factor /= multiplier ;
   	
   	float major = factor ;
   	float minor = major / 5 ;
   	
   	// Draw scale
   	dc.SetFont ( *MYFONT ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ) ;
   	dc.SetTextForeground ( col ) ;
   	dc.SetPen ( *MYPEN ( col ) ) ;
   	
   	float p = 0 ;
   	while ( p > bottom ) p -= minor ;
   	while ( p < bottom ) p += minor ;
   	if ( p != bottom ) p -= minor ;
   	while ( p <= top )
   		{
			wxString label ;
/*			if ( unit == _T("s") ) label = wxString::Format ( _T("%d:%2d") , p / 60 , ((int)p) % 60 ) ;
			else */label = wxString::Format ( _T("%f") , p ) ;
			DrawMark ( dc , p , ir , label , ( p - ((int)(p/major))*major ) == 0 ) ;
			p += minor ;
   		}    
	
	// Draw "backline" and name/unit
	wxString text = name ;
	if ( !unit.IsEmpty() ) text += _T(" [") + unit + _T("]") ;
   	int tw , th ;
   	dc.GetTextExtent ( text , &tw , &th ) ;
	if ( horizontal )
		{
  		if ( left )
    		{
          	dc.DrawLine ( ir.x , ir.y , ir.GetRight() , ir.y ) ;
          	dc.DrawText ( text , ir.x , ir.y ) ;
          	}   	
  		else
    		{
     		dc.DrawLine ( ir.x , ir.GetBottom() , ir.GetRight() , ir.GetBottom() ) ;
          	dc.DrawText ( text , ir.x , ir.GetBottom() - th ) ;
     		}  		
  		if ( show_mark )
  			{
	    	int c = GetRealCoord ( GetVirtualCoordinate ( mark , inner ) , inner ) ;
	    	dc.SetPen ( *wxRED_PEN ) ;
	    	dc.DrawLine ( c , ir.GetTop() , c , ir.GetBottom() ) ;
  			}    
		}		
	else
		{
		if ( left )
  			{
         dc.DrawLine ( ir.x , ir.y , ir.x , ir.GetBottom() ) ;
         dc.DrawRotatedText ( text , ir.x + 2 , ir.GetBottom() , 90 ) ;
         }   	
		else
  			{
    		dc.DrawLine ( ir.GetRight() , ir.y , ir.GetRight() , ir.GetBottom() ) ;
         dc.DrawRotatedText ( text , ir.GetRight()-th , ir.GetBottom() , 90 ) ;
      	}
  		if ( show_mark )
  			{
	    	int c = GetRealCoord ( GetVirtualCoordinate ( mark , inner ) , inner ) ;
	    	dc.SetPen ( *wxRED_PEN ) ;
	    	dc.DrawLine ( ir.GetLeft() , c , ir.GetRight() , c ) ;
  			}    
  		}
	last_inner = inner ;
	show_mark = false ;
	}    

void TGraphScale::DrawMark ( wxDC &dc , float p , wxRect &ir , wxString text , bool big )
	{
	int x , y , z ;
 	if ( horizontal )
 		{
    	x = GetRealCoord ( p , ir ) ;
    	y = ir.y + ir.height / 2 ;
    	z = big ? 0 : ir.height / 4 ;
    	if ( x < ir.x || x > ir.GetRight() ) return ;
 		}
 	else
  		{
    	x = ir.x + ir.width / 2 ;
    	y = GetRealCoord ( p , ir ) ;
    	z = big ? 0 : ir.width / 4 ;
    	if ( y < ir.y || y > ir.GetBottom() ) return ;
    	}        
   
	while ( text.Last() == '0' ) text = text.Left ( text.length() - 1 ) ;
   while ( text.Last() == '.' ) text = text.Left ( text.length() - 1 ) ;
   	
   int tw , th ;
   dc.GetTextExtent ( text , &tw , &th ) ;
   if ( horizontal )
   	{
	    if ( left ) dc.DrawLine ( x , y-z , x , ir.y ) ;
	    else dc.DrawLine ( x , y+z , x , ir.GetBottom() ) ;
	    x -= tw / 2 ;
	    y = left ? ir.GetBottom() - th : ir.y ;
   	}
   else
    	{
	    if ( left ) dc.DrawLine ( ir.x , y , x-z , y ) ;
	    else dc.DrawLine ( x+z , y , ir.GetRight() , y ) ;
	    y -= th / 2 ;
	    x = left ? ir.GetRight() - tw : ir.x ;
     	}        

   if ( big ) dc.DrawText ( text , x , y ) ;
	}
    
int TGraphScale::GetRealCoord ( float f , wxRect &inner )
	{
	float m2 = horizontal ? inner.width : inner.height ; // The pixel "width"
	float f2 = top - bottom ; // The internal "width"
	float ret = m2 * ( f - bottom ) / f2 ;
	if ( horizontal ) ret += (float ) inner.x ;
	else ret = (float) ( inner.GetBottom() ) - ret ;
 	return (int) ret ;
	}    

float TGraphScale::GetVirtualCoordinate ( int i , wxRect &inner )
	{
 	i -= horizontal ? inner.x : inner.y ;
 	float i2 = i ;
	float m2 = horizontal ? inner.width : inner.height ;
	float f2 = top - bottom ; // The internal "width"
	f2 = f2 * i2 / m2 ;
	if ( horizontal ) f2 += bottom ;
	else f2 = top - f2 ;
	return f2 ;
 	}    	

void TGraphScale::Drag ( int delta )
	{
	float df = GetVirtualCoordinate ( delta , last_inner ) ;
	df -= GetVirtualCoordinate ( 0 , last_inner ) ;
	top += df ;
	bottom += df ;
	if ( top > max )
 		{
   		bottom -= top - max ;
        top = max ;
        }    
    if ( bottom < min )
    	{
	    top += min - bottom ;
	    bottom = min ;
    	}    
	}    
