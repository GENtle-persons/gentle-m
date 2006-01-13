#ifndef __TPHYLIP_H_
#define __TPHYLIP_H_

#include <wx/wx.h>

class TPhylip
	{
	public :
	TPhylip () ;
	~TPhylip () ;
	
	wxString protpars ( wxString s ) ;
	wxString dnapars ( wxString s ) ;
	wxString dnamove ( wxString s ) ;
	wxString dnapenny ( wxString s ) ;
	wxString dnacomp ( wxString s ) ;
	wxString dnainvar ( wxString s ) ;
	wxString dnaml ( wxString s ) ;
	wxString dnamlk ( wxString s ) ;
	wxString proml ( wxString s ) ;
	wxString promlk ( wxString s ) ;
	wxString dnadist ( wxString s ) ;
	wxString protdist ( wxString s ) ;
	wxString restdist ( wxString s ) ;
	wxString restml ( wxString s ) ;
	wxString seqboot ( wxString s ) ;
	wxString fitch ( wxString s ) ;
	wxString kitsch ( wxString s ) ;
	wxString neighbor ( wxString s ) ;
	wxString contml ( wxString s ) ;
	wxString gendist ( wxString s ) ;
	wxString contrast ( wxString s ) ;
	wxString pars ( wxString s ) ;
	wxString mix ( wxString s ) ;
	wxString move ( wxString s ) ;
	wxString penny ( wxString s ) ;
	wxString dollop ( wxString s ) ;
	wxString dolmove ( wxString s ) ;
	wxString dolpenny ( wxString s ) ;
	wxString clique ( wxString s ) ;
	wxString factor ( wxString s ) ;
	wxString drawgram ( wxString s ) ;
	wxString drawtree ( wxString s ) ;
	wxString treedist ( wxString s ) ;
	wxString consense ( wxString s ) ;
	wxString retree ( wxString s ) ;
	
	void query_phylip_dir ( bool setit = true ) ;
	bool IsOK () ;
	
	private :
	wxString runapp ( wxString app , const wxString s ) ;
	
	wxString phylip_dir ;
	} ;


#endif
