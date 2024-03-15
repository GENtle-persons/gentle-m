#ifndef __TPHYLIP_H_
#define __TPHYLIP_H_

#include <wx/wx.h>

class TPhylip
    {
    public :
    TPhylip () ;
    ~TPhylip () ;

    wxString protpars ( const wxString& s ) ;
    wxString dnapars ( const wxString& s ) ;
    wxString dnamove ( const wxString& s ) ;
    wxString dnapenny ( const wxString& s ) ;
    wxString dnacomp ( const wxString& s ) ;
    wxString dnainvar ( const wxString& s ) ;
    wxString dnaml ( const wxString& s ) ;
    wxString dnamlk ( const wxString& s ) ;
    wxString proml ( const wxString& s ) ;
    wxString promlk ( const wxString& s ) ;
    wxString dnadist ( const wxString& s ) ;
    wxString protdist ( const wxString& s ) ;
    wxString restdist ( const wxString& s ) ;
    wxString restml ( const wxString& s ) ;
    wxString seqboot ( const wxString& s ) ;
    wxString fitch ( const wxString& s ) ;
    wxString kitsch ( const wxString& s ) ;
    wxString neighbor ( const wxString& s ) ;
    wxString contml ( const wxString& s ) ;
    wxString gendist ( const wxString& s ) ;
    wxString contrast ( const wxString& s ) ;
    wxString pars ( const wxString& s ) ;
    wxString mix ( const wxString& s ) ;
    wxString move ( const wxString& s ) ;
    wxString penny ( const wxString& s ) ;
    wxString dollop ( const wxString& s ) ;
    wxString dolmove ( const wxString& s ) ;
    wxString dolpenny ( const wxString& s ) ;
    wxString clique ( const wxString& s ) ;
    wxString factor ( const wxString& s ) ;
    wxString drawgram ( const wxString& s ) ;
    wxString drawtree ( const wxString& s ) ;
    wxString treedist ( const wxString& s ) ;
    wxString consense ( const wxString& s ) ;
    wxString retree ( const wxString& s ) ;

    void query_phylip_dir ( bool setit = true ) ;
    bool IsOK () ;

    private :
    wxString runapp ( const wxString& app , const wxString& s ) ;

    wxString phylip_dir ;
    } ;


#endif
