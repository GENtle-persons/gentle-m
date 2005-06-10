#ifndef __TPHYLIP_H_
#define __TPHYLIP_H_

#include <wx/wx.h>

class TPhylip
	{
	public :
	TPhylip () ;
	virtual ~TPhylip () ;
	
	virtual wxString protpars ( wxString s ) ;
	virtual wxString dnapars ( wxString s ) ;
	virtual wxString dnamove ( wxString s ) ;
	virtual wxString dnapenny ( wxString s ) ;
	virtual wxString dnacomp ( wxString s ) ;
	virtual wxString dnainvar ( wxString s ) ;
	virtual wxString dnaml ( wxString s ) ;
	virtual wxString dnamlk ( wxString s ) ;
	virtual wxString proml ( wxString s ) ;
	virtual wxString promlk ( wxString s ) ;
	virtual wxString dnadist ( wxString s ) ;
	virtual wxString protdist ( wxString s ) ;
	virtual wxString restdist ( wxString s ) ;
	virtual wxString restml ( wxString s ) ;
	virtual wxString seqboot ( wxString s ) ;
	virtual wxString fitch ( wxString s ) ;
	virtual wxString kitsch ( wxString s ) ;
	virtual wxString neighbor ( wxString s ) ;
	virtual wxString contml ( wxString s ) ;
	virtual wxString gendist ( wxString s ) ;
	virtual wxString contrast ( wxString s ) ;
	virtual wxString pars ( wxString s ) ;
	virtual wxString mix ( wxString s ) ;
	virtual wxString move ( wxString s ) ;
	virtual wxString penny ( wxString s ) ;
	virtual wxString dollop ( wxString s ) ;
	virtual wxString dolmove ( wxString s ) ;
	virtual wxString dolpenny ( wxString s ) ;
	virtual wxString clique ( wxString s ) ;
	virtual wxString factor ( wxString s ) ;
	virtual wxString drawgram ( wxString s ) ;
	virtual wxString drawtree ( wxString s ) ;
	virtual wxString treedist ( wxString s ) ;
	virtual wxString consense ( wxString s ) ;
	virtual wxString retree ( wxString s ) ;
	
	private :
	virtual wxString runapp ( const wxString app , const wxString s ) ;
	
	wxString pylip_dir ;
	} ;


#endif
