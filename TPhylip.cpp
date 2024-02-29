#include "TPhylip.h"

#include "main.h"
#include <wx/file.h>

TPhylip::TPhylip ()
    {
    phylip_dir = myapp()->frame->LS->getOption ( _T("PHYLIP_DIR") , _T("") ) ;
    if ( phylip_dir.IsEmpty() ) query_phylip_dir () ;
    }

TPhylip::~TPhylip ()
    {
    }

void TPhylip::query_phylip_dir ( const bool setit )
    {
    wxString h = wxDirSelector ( txt("t_select_phylip_dir") , phylip_dir ) ;
    if ( !setit && h.IsEmpty() ) return ;
    phylip_dir = h ;
    if ( !phylip_dir.IsEmpty() ) myapp()->frame->LS->setOption ( _T("PHYLIP_DIR") , phylip_dir ) ;
    }

bool TPhylip::IsOK ()
    {
#ifdef __WXMAC__
    // Mac version is broken due to strange pipe problem with the "open -a" command
    wxMessageBox ( _T("Mac version of phylip is broken due to strange pipe problem with the 'open' command") ) ;
    return false ;
#endif
    if ( phylip_dir.IsEmpty() ) return false ;
    return true ;
    }

//*************************** Interface methods

wxString TPhylip::protpars ( const wxString& s )
    {
    return runapp ( _T("protpars") , s ) ;
    }


wxString TPhylip::dnapars ( const wxString& s )
    {
    return runapp ( _T("dnapars") , s ) ;
    }


wxString TPhylip::dnamove ( const wxString& s )
    {
    return runapp ( _T("dnamove") , s ) ;
    }


wxString TPhylip::dnapenny ( const wxString& s )
    {
    return runapp ( _T("dnapenny") , s ) ;
    }


wxString TPhylip::dnacomp ( const wxString& s )
    {
    return runapp ( _T("dnacomp") , s ) ;
    }


wxString TPhylip::dnainvar ( const wxString& s )
    {
    return runapp ( _T("dnainvar") , s ) ;
    }


wxString TPhylip::dnaml ( const wxString& s )
    {
    return runapp ( _T("dnaml") , s ) ;
    }


wxString TPhylip::dnamlk ( const wxString& s )
    {
    return runapp ( _T("dnamlk") , s ) ;
    }


wxString TPhylip::proml ( const wxString& s )
    {
    return runapp ( _T("proml") , s ) ;
    }


wxString TPhylip::promlk ( const wxString& s )
    {
    return runapp ( _T("promlk") , s ) ;
    }


wxString TPhylip::dnadist ( const wxString& s )
    {
    return runapp ( _T("dnadist") , s ) ;
    }


wxString TPhylip::protdist ( const wxString& s )
    {
    return runapp ( _T("protdist") , s ) ;
    }


wxString TPhylip::restdist ( const wxString& s )
    {
    return runapp ( _T("restdist") , s ) ;
    }


wxString TPhylip::restml ( const wxString& s )
    {
    return runapp ( _T("restml") , s ) ;
    }


wxString TPhylip::seqboot ( const wxString& s )
    {
    return runapp ( _T("seqboot") , s ) ;
    }


wxString TPhylip::fitch ( const wxString& s )
    {
    return runapp ( _T("fitch") , s ) ;
    }


wxString TPhylip::kitsch ( const wxString& s )
    {
    return runapp ( _T("kitsch") , s ) ;
    }


wxString TPhylip::neighbor ( const wxString& s )
    {
    return runapp ( _T("neighbor") , s ) ;
    }


wxString TPhylip::contml ( const wxString& s )
    {
    return runapp ( _T("contml") , s ) ;
    }


wxString TPhylip::gendist ( const wxString& s )
    {
    return runapp ( _T("gendist") , s ) ;
    }


wxString TPhylip::contrast ( const wxString& s )
    {
    return runapp ( _T("contrast") , s ) ;
    }


wxString TPhylip::pars ( const wxString& s )
    {
    return runapp ( _T("pars") , s ) ;
    }


wxString TPhylip::mix ( const wxString& s )
    {
    return runapp ( _T("mix") , s ) ;
    }


wxString TPhylip::move ( const wxString& s )
    {
    return runapp ( _T("move") , s ) ;
    }


wxString TPhylip::penny ( const wxString& s )
    {
    return runapp ( _T("penny") , s ) ;
    }


wxString TPhylip::dollop ( const wxString& s )
    {
    return runapp ( _T("dollop") , s ) ;
    }


wxString TPhylip::dolmove ( const wxString& s )
    {
    return runapp ( _T("dolmove") , s ) ;
    }


wxString TPhylip::dolpenny ( const wxString& s )
    {
    return runapp ( _T("dolpenny") , s ) ;
    }


wxString TPhylip::clique ( const wxString& s )
    {
    return runapp ( _T("clique") , s ) ;
    }


wxString TPhylip::factor ( const wxString& s )
    {
    return runapp ( _T("factor") , s ) ;
    }


wxString TPhylip::drawgram ( const wxString& s )
    {
    return runapp ( _T("drawgram") , s ) ;
    }


wxString TPhylip::drawtree ( const wxString& s )
    {
    return runapp ( _T("drawtree") , s ) ;
    }


wxString TPhylip::treedist ( const wxString& s )
    {
    return runapp ( _T("treedist") , s ) ;
    }


wxString TPhylip::consense ( const wxString& s )
    {
    return runapp ( _T("consense") , s ) ;
    }


wxString TPhylip::retree ( const wxString& s )
    {
    return runapp ( _T("retree") , s ) ;
    }


//***************************

wxString TPhylip::runapp ( const wxString& app , const wxString& s )
    {
    wxString cmd ;
    wxString tmpdir = phylip_dir ;
    wxString datafile = tmpdir + _T("/data.txt") ;
    wxString cmdfile = tmpdir + _T("/cmd.txt") ;
    wxString dummy = tmpdir + _T("/dummy.txt") ;

#ifdef __WXMAC__
    // Mac version is broken due to strange pipe problem with the "open -a" command
    cmd += _T("open ") + phylip_dir + _T("/") + app + _T(".app < ") + cmdfile + _T(" > ") + dummy ;
#else
    cmd += phylip_dir + _T("/") + app + _T(" < ") + cmdfile + _T(" > ") + dummy ;
#endif

    // Write data file
    wxFile data ( datafile , wxFile::write ) ;
    data.Write ( s ) ;
    data.Close () ;

    // Write cmd file
    wxFile command ( cmdfile , wxFile::write ) ;
    command.Write ( datafile + _T("\n") ) ;
    command.Write ( _T("Y\n") ) ;
    command.Close () ;

#ifdef __WXMAC__
    wxMessageBox ( cmd ) ;
    wxString owd = wxGetCwd () ;
    wxSetWorkingDirectory ( tmpdir ) ;
    wxExecute ( cmd , wxEXEC_SYNC ) ;
    wxSetWorkingDirectory ( owd ) ;
#else
    // Write batch file
    wxString batchfile = tmpdir + _T("/run.bat") ;
    wxFile bat ( batchfile , wxFile::write ) ;
    bat.Write ( _T("@echo off\n") ) ;
    bat.Write ( _T("cd ") + tmpdir + _T("\n") ) ;
    bat.Write ( cmd ) ;
    bat.Close () ;

    // Delete old files, if any
    wxRemoveFile ( tmpdir + _T("/outfile") ) ;
    wxRemoveFile ( tmpdir + _T("/outtree") ) ;

    // Run program
    wxExecute ( batchfile , wxEXEC_SYNC ) ;

    wxRemoveFile ( dummy ) ;
    wxRemoveFile ( batchfile ) ;
    wxRemoveFile ( cmdfile ) ;
//  wxRemoveFile ( datafile ) ;
#endif

    wxFile outtree ( tmpdir + _T("/outtree") ) ;
    long l = outtree.Length () ;
    char *n = new char[l+5] ;
    outtree.Read ( n , l ) ;
    outtree.Close () ;
    n[l] = 0 ;
    wxString result ( n , wxConvUTF8 ) ;
    delete [] n ;

//  result = "(3:0.23647,(4:0.12270,2:0.00296):0.29673,1:0.65529);(4:0.12270,2:0.00296);" ; // Test

    while ( result.Right ( 1 ) == _T(";") ) result = result.Left ( result.length() - 1 ) ;
    return result ; //.BeforeFirst ( ';' ) ;
    }
