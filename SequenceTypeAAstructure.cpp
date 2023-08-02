/** \file
	\brief Contains the SeqAAstructure class members
*/
#include "SequenceCanvas.h"

SeqAAstructure::SeqAAstructure ( SequenceCanvas *ncan , SeqAA *_aa )
	{
	aa = _aa ;
    init ( ncan ) ;
    int min = 20 , max = 250 ;
    brush_C = (wxBrush*) wxBLACK_BRUSH ;
    pen_C = (wxPen*) wxBLACK_PEN ;
    brush_N = MYBRUSH ( wxColour ( min , min , max ) ) ;
    pen_N = MYPEN ( wxColour ( min , min , max ) ) ;
    brush_S = MYBRUSH ( wxColour ( max , max , min ) ) ;
    pen_S = MYPEN ( wxColour ( max , max , min ) ) ;
    brush_O = MYBRUSH ( wxColour ( max , min , min ) ) ;
    pen_O = MYPEN ( wxColour ( max , min , min ) ) ;
	}

SeqAAstructure::~SeqAAstructure ()
	{
	}

int SeqAAstructure::arrange ( int n )
    {
	if ( !can || !aa ) return 0 ;
    int w , h , bo = can->border ;
    s = aa->s ;
    
    // Setting basic values
    can->SetFont(*can->font);
    int wx = can->charwidth , wy = can->charheight ;
    int ox = bo + wx + wx * endnumberlength ;
    can->MyGetClientSize ( &w , &h ) ;
    itemsperline = ( w - ox ) / ( ( can->blocksize + 1 ) * wx - 1 ) ;
    if ( itemsperline == 0 ) itemsperline = 1 ;
	itemsperline *= can->blocksize ;
    pos.cleanup() ;
//    pos.m.Alloc ( s.length() ) ;
    if ( can->isHorizontal() ) can->setLowX ( ox + ( s.length() ) * can->charwidth ) ;
    int ret = ( s.length() + itemsperline - 1 ) / itemsperline ;
    ret = can->NumberOfLines() * ret * can->charheight + 1 ;
    return ret ;
	}

void SeqAAstructure::show ( wxDC& dc )
    {
	if ( !can || !aa ) return ;
    myass ( itemsperline , "AA:SeqAAstructure:show" ) ;
    if ( !itemsperline ) return ;
    mylog ( "SeqAAstructure::show_direct" , "0" ) ;
    can->SetFont(*can->font);
    dc.SetFont(*can->font);
    int a , b , w , h , n , bo = can->border ;
    int csgc = can->NumberOfLines() , cbs = can->blocksize ;
    int cih = can->isHorizontal() ;
    int xa , xb , ya , yb ;
    for ( n = 0 ; n < csgc && can->seq[n] != this ; n++ ) ;
    if ( n == csgc ) return ;
    mylog ( "SeqAAstructure::show_direct" , "1" ) ;
    
    // Setting basic values
    int cw = can->charwidth , ch = can->charheight ;
    int ox = bo + cw + cw * endnumberlength ;
    int oy = n*ch+bo ;
    
    can->MyGetClientSize ( &w , &h ) ;
    xb = w ;
    yb = h ;

    wxColour tbg = dc.GetTextBackground () ;
    wxColour tfg = dc.GetTextForeground () ;
    int bm = dc.GetBackgroundMode () ;
    wxColour tf ;
    tf = wxColour ( 130 , 130 , 130 ) ;
    dc.SetTextForeground ( tf ) ;
    dc.SetBackgroundMode ( wxTRANSPARENT ) ;

    dc.GetDeviceOrigin ( &xa , &ya ) ;
    xa = -xa ;
    xb += xa ;
    ya = -ya ;
    yb += ya ;
    
    mylog ( "SeqAAstructure::show_direct" , "2" ) ;
    b = ( ya - ch - oy ) / ( ch * csgc ) * itemsperline ;
    mylog ( "SeqAAstructure::show_direct" , "3" ) ;
    for ( a = 0 ; a < b && a < s.length() ; a += itemsperline ) ;
        
    for ( ; a < s.length() ; a++ )
        {
		int px = a % itemsperline , py = a / itemsperline ;
		
		px = px * cw + ( px / cbs ) * ( cw - 1 ) + ox ;
		py = py * ch * csgc + oy ;
		
		if ( !can->getDrawAll() )
			{
			if ( py + ch < ya ) continue ;
			if ( py > yb ) break ;
			if ( cih )
				{
				if ( px + cw < xa ) continue ;
				if ( px > xb ) continue ;
				}    
			}    
		
		// Show the amino acid
		wxChar ch2 = s.GetChar(a) ;
		draw_amino_acid ( dc , ch2 , px , py , cw , ch ) ;
        }    

    dc.SetBackgroundMode ( bm ) ;
    dc.SetTextBackground ( tbg ) ;
    dc.SetTextForeground ( tfg ) ;
	}

void SeqAAstructure::draw_amino_acid ( wxDC &dc , char as , int x , int y , int w , int h )
	{
	if ( as == '|' || as == ' ' ) return ;
	unsigned int a ;
	int point_radius = w / ( can->isPrinting() ? 8 : 6 ) ;
	wxPoint base ( x , y ) ;
	
	vector <wxPoint> atom_pos , bonds ;
	wxString atom_type ;
	add_atom ( atom_pos , atom_type , 'N' , w * 1 / 9 , h * 2 / 9 ) ; // 0
	add_atom ( atom_pos , atom_type , 'C' , w * 4 / 9 , h * 2 / 9 ) ; // 1
	add_atom ( atom_pos , atom_type , 'C' , w * 7 / 9 , h * 2 / 9 ) ; // 2
	add_atom ( atom_pos , atom_type , 'O' , w * 7 / 9 , h * 0 / 9 ) ; // 3
	
	add_bond ( bonds , 0 , 1 ) ;
	add_bond ( bonds , 1 , 2 ) ;
	add_bond ( bonds , 2 , 3 , true ) ;

	switch ( as )
		{
		case 'G' : // Gly/Glycine; Nothing to see here, please move along
			break ;
		case 'A' : // Ala/Alanin
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_bond ( bonds ,  1 ,  4 ) ;
			break ;
		case 'C' : // Cyc/Cysteine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'S' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_bond ( bonds ,  1 ,  4 ) ;
			add_bond ( bonds ,  4 ,  5 ) ;
			break ;
		case 'T' : // Thr/Threonine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'O' , w * 7 /  9 , h *  5 / 9 ) ; // 6
			add_bond ( bonds ,  1 ,  4 ) ;
			add_bond ( bonds ,  4 ,  5 ) ;
			add_bond ( bonds ,  4 ,  6 ) ;
			break ;
		case 'I' : // Ile/Isoleucine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 7 /  9 , h *  5 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  9 / 9 ) ; // 7
			add_bond ( bonds ,  1 ,  4 ) ;
			add_bond ( bonds ,  4 ,  5 ) ;
			add_bond ( bonds ,  4 ,  6 ) ;
			add_bond ( bonds ,  5 ,  7 ) ;
			break ;
		case 'R' : // Arg/Arginine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'N' , w * 4 /  9 , h * 11 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h * 13 / 9 ) ; // 8
			add_atom ( atom_pos , atom_type , 'N' , w * 7 /  9 , h * 13 / 9 ) ; // 9
			add_atom ( atom_pos , atom_type , 'N' , w * 4 /  9 , h * 15 / 9 ) ; // 10
			add_bond ( bonds ,  1 ,  4 ) ;
			add_bond ( bonds ,  4 ,  5 ) ;
			add_bond ( bonds ,  5 ,  6 ) ;
			add_bond ( bonds ,  6 ,  7 ) ;
			add_bond ( bonds ,  7 ,  8 ) ;
			add_bond ( bonds ,  8 ,  9 , true ) ;
			add_bond ( bonds ,  8 , 10 ) ;
			break ;
		case 'F' : // Phe/Phenylalanine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 2 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h *  9 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'C' , w * 2 /  9 , h * 11 / 9 ) ; // 8
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h * 11 / 9 ) ; // 9
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h * 13 / 9 ) ; // 10
			add_bond ( bonds ,  1 ,  4 ) ;
			add_bond ( bonds ,  4 ,  5 ) ;
			add_bond ( bonds ,  5 ,  6 , true ) ;
			add_bond ( bonds ,  5 ,  7 ) ;
			add_bond ( bonds ,  6 ,  8 ) ;
			add_bond ( bonds ,  7 ,  9 , true ) ;
			add_bond ( bonds ,  8 , 10 , true ) ;
			add_bond ( bonds ,  9 , 10 ) ;
			break ;
		case 'Y' : // Tyr/Tyrosine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 2 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h *  9 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'C' , w * 2 /  9 , h * 11 / 9 ) ; // 8
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h * 11 / 9 ) ; // 9
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h * 13 / 9 ) ; // 10
			add_atom ( atom_pos , atom_type , 'O' , w * 4 /  9 , h * 15 / 9 ) ; // 11
			add_bond ( bonds ,  1 ,  4 ) ;
			add_bond ( bonds ,  4 ,  5 ) ;
			add_bond ( bonds ,  5 ,  6 , true ) ;
			add_bond ( bonds ,  5 ,  7 ) ;
			add_bond ( bonds ,  6 ,  8 ) ;
			add_bond ( bonds ,  7 ,  9 , true ) ;
			add_bond ( bonds ,  8 , 10 , true ) ;
			add_bond ( bonds ,  9 , 10 ) ;
			add_bond ( bonds , 10 , 11 ) ;
			break ;
		case 'M' : // Met/methionine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'S' , w * 4 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h * 11 / 9 ) ; // 7
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 5 , 6 ) ;
			add_bond ( bonds , 6 , 7 ) ;
			break ;
		case 'V' : // Val/Valine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 2 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h *  7 / 9 ) ; // 6
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 4 , 6 ) ;
			break ;
		case 'L' : // Leu/Leucine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 2 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h *  9 / 9 ) ; // 7
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 5 , 6 ) ;
			add_bond ( bonds , 5 , 7 ) ;
			break ;
		case 'D' : // Asp/Aspartic acid
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'O' , w * 2 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'O' , w * 6 /  9 , h *  9 / 9 ) ; // 7
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 5 , 6 ) ;
			add_bond ( bonds , 5 , 7 , true ) ;
			break ;
		case 'E' : // Glu/Glutamic acid
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'O' , w * 2 /  9 , h * 11 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'O' , w * 6 /  9 , h * 11 / 9 ) ; // 8
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 5 , 6 ) ;
			add_bond ( bonds , 6 , 7 ) ;
			add_bond ( bonds , 6 , 8 , true ) ;
			break ;
		case 'N' : // Asn/Asparagine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'O' , w * 7 /  9 , h *  7 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'N' , w * 4 /  9 , h *  9 / 9 ) ; // 7
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 5 , 6 , true ) ;
			add_bond ( bonds , 5 , 7 ) ;
			break ;
		case 'K' : // Lys/Lysine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h * 11 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'N' , w * 4 /  9 , h * 13 / 9 ) ; // 8
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 5 , 6 ) ;
			add_bond ( bonds , 6 , 7 ) ;
			add_bond ( bonds , 6 , 8 ) ;
			break ;
		case 'S' : // Ser/serine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'O' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			break ;
		case 'Q' : // Gln/Glutamine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'O' , w * 7 /  9 , h *  9 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'N' , w * 4 /  9 , h * 11 / 9 ) ; // 8
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 4 , 6 ) ;
			add_bond ( bonds , 6 , 7 , true ) ;
			add_bond ( bonds , 6 , 8 ) ;
			break ;
		case 'W' : // Trp/Tryptophane
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 9 /  9 , h *  7 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 9 /  9 , h *  9 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'N' , w * 7 /  9 , h * 11 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'C' , w * 5 /  9 , h *  7 / 9 ) ; // 8
			add_atom ( atom_pos , atom_type , 'C' , w * 5 /  9 , h *  9 / 9 ) ; // 9
			add_atom ( atom_pos , atom_type , 'C' , w * 3 /  9 , h *  5 / 9 ) ; // 10
			add_atom ( atom_pos , atom_type , 'C' , w * 3 /  9 , h * 11 / 9 ) ; // 11
			add_atom ( atom_pos , atom_type , 'C' , w * 1 /  9 , h *  7 / 9 ) ; // 12
			add_atom ( atom_pos , atom_type , 'C' , w * 1 /  9 , h *  9 / 9 ) ; // 13
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 5 ) ;
			add_bond ( bonds , 5 , 6 , true ) ;
			add_bond ( bonds , 6 , 7 ) ;
			add_bond ( bonds , 5 , 8 ) ;
			add_bond ( bonds , 7 , 9 ) ;
			add_bond ( bonds , 8 , 9 , true ) ;
			add_bond ( bonds , 8 , 10 ) ;
			add_bond ( bonds , 10 , 12 , true ) ;
			add_bond ( bonds , 12 , 13 ) ;
			add_bond ( bonds , 11 , 13 , true ) ;
			add_bond ( bonds , 9 , 11 ) ;
			break ;
		case 'H' : // His/Histidine
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'N' , w * 9 /  9 , h *  9 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 6 /  9 , h * 11 / 9 ) ; // 6
			add_atom ( atom_pos , atom_type , 'N' , w * 3 /  9 , h *  9 / 9 ) ; // 7
			add_atom ( atom_pos , atom_type , 'C' , w * 4 /  9 , h *  7 / 9 ) ; // 8
			add_atom ( atom_pos , atom_type , 'C' , w * 8 /  9 , h *  7 / 9 ) ; // 9
			add_bond ( bonds , 1 , 4 ) ;
			add_bond ( bonds , 4 , 8 ) ;
			add_bond ( bonds , 9 , 5 ) ;
			add_bond ( bonds , 5 , 6 , true ) ;
			add_bond ( bonds , 6 , 7 ) ;
			add_bond ( bonds , 7 , 8 ) ;
			add_bond ( bonds , 8 , 9 , true ) ;
			break ;
		case 'P' : // Pro/Proline
			add_atom ( atom_pos , atom_type , 'C' , w * 0 /  9 , h * 5 / 9 ) ; // 4
			add_atom ( atom_pos , atom_type , 'C' , w * 5 /  9 , h * 5 / 9 ) ; // 5
			add_atom ( atom_pos , atom_type , 'C' , w * 5 / 18 , h * 7 / 9 ) ; // 6
			add_bond ( bonds , 0 , 4 ) ;
			add_bond ( bonds , 4 , 6 ) ;
			add_bond ( bonds , 6 , 5 ) ;
			add_bond ( bonds , 5 , 1 ) ;
			break ;
		}
	
	// Draw bonds
	if ( can->isPrinting() ) dc.SetPen ( *pen_C ) ;
	else dc.SetPen ( *wxGREY_PEN ) ;
	for ( a = 0 ; a < bonds.size() ; a++ )
		{
		bool d = false ;
		if ( a+1 < bonds.size() && bonds[a] == bonds[a+1] )
			{
			a++ ;
			d = true ;
			}
		wxPoint p1 = atom_pos[bonds[a].x] + base ;
		wxPoint p2 = atom_pos[bonds[a].y] + base ;
		if ( d )
			{
			if ( can->isPrinting() )
				{
				int hdiff = p1.x - p2.x ;
				int vdiff = p1.y - p2.y ;
				if ( hdiff != 0 ) hdiff /= abs ( hdiff ) ;
				if ( vdiff != 0 ) vdiff /= abs ( vdiff ) ;
				wxPoint diff ( vdiff * point_radius / 2 , hdiff * point_radius ) ;
				dc.DrawLine ( p1 + diff , p2 + diff ) ;
				dc.DrawLine ( p1 - diff , p2 - diff ) ;
				}
			else
				{
				dc.SetPen ( *pen_C ) ;
				dc.DrawLine ( p1 , p2 ) ;
				dc.SetPen ( *wxGREY_PEN ) ;
				}
			}
		else
			{
			dc.DrawLine ( p1 , p2 ) ;
			}
		}
	
	// Draw atoms
	dc.SetPen ( *pen_C ) ;
	for ( a = 0 ; a < atom_pos.size() ; a++ )
		{
		switch ( (char)atom_type.GetChar ( a ) )
			{
			case 'C' : if ( can->isPrinting() ) dc.SetPen ( *pen_C ) ; dc.SetBrush ( *brush_C ) ; break ;
			case 'S' : if ( can->isPrinting() ) dc.SetPen ( *pen_S ) ; dc.SetBrush ( *brush_S ) ; break ;
			case 'N' : if ( can->isPrinting() ) dc.SetPen ( *pen_N ) ; dc.SetBrush ( *brush_N ) ; break ;
			case 'O' : if ( can->isPrinting() ) dc.SetPen ( *pen_O ) ; dc.SetBrush ( *brush_O ) ; break ;
			}
		int rad = atom_type.GetChar ( a ) == 'C' ? point_radius * 2 / 3 : point_radius ;
		rad = rad < 1 ? 1 : rad ;
		dc.DrawCircle ( atom_pos[a] + base , rad ) ;
		}
	}

void SeqAAstructure::add_atom ( vector <wxPoint> &atom_pos , wxString &atom_type , char atom , int x , int y )
	{
	atom_pos.push_back ( wxPoint ( x , y ) ) ;
	atom_type += atom ;
	}

void SeqAAstructure::add_bond ( vector <wxPoint> &bond , int from , int to , bool double_bond )
	{
	bond.push_back ( wxPoint ( from , to ) ) ;
	if ( double_bond ) bond.push_back ( wxPoint ( from , to ) ) ; // Again
	}
