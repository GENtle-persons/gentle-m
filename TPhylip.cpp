#include "TPhylip.h"

#include "main.h"
#include <wx/file.h>

TPhylip::TPhylip ()
	{
	phylip_dir = myapp()->frame->LS->getOption ( _T("PHYLIP_DIR") , "" ) ;
	if ( phylip_dir.IsEmpty() )
		{
		phylip_dir = wxDirSelector ( txt("t_select_phlip_dir") ) ;
		if ( !phylip_dir.IsEmpty() ) myapp()->frame->LS->setOption ( _T("PHYLIP_DIR") , phylip_dir ) ;
		}
//	phylip_dir = _T("C:\\Downloads\\phylip-3.63\\exe") ; // TESTING!!!
	}

TPhylip::~TPhylip ()
	{
	}

bool TPhylip::IsOK ()
	{
#ifdef __WXMAC__
	// Mac version is broken due to strange pipe problem with the "open -a" command
	wxMessageBox ( "Mac version is broken due to strange pipe problem with the 'open' command" ) ;
	return false ;
#endif
	if ( phylip_dir.IsEmpty() ) return false ;
	return true ;
	}

//*************************** Interface methods

wxString TPhylip::protpars ( wxString s )
	{
	return runapp ( _T("protpars") , s ) ;
	}


wxString TPhylip::dnapars ( wxString s )
	{
	return runapp ( _T("dnapars") , s ) ;
	}


wxString TPhylip::dnamove ( wxString s )
	{
	return runapp ( _T("dnamove") , s ) ;
	}


wxString TPhylip::dnapenny ( wxString s )
	{
	return runapp ( _T("dnapenny") , s ) ;
	}


wxString TPhylip::dnacomp ( wxString s )
	{
	return runapp ( _T("dnacomp") , s ) ;
	}


wxString TPhylip::dnainvar ( wxString s )
	{
	return runapp ( _T("dnainvar") , s ) ;
	}


wxString TPhylip::dnaml ( wxString s )
	{
	return runapp ( _T("dnaml") , s ) ;
	}


wxString TPhylip::dnamlk ( wxString s )
	{
	return runapp ( _T("dnamlk") , s ) ;
	}


wxString TPhylip::proml ( wxString s )
	{
	return runapp ( _T("proml") , s ) ;
	}


wxString TPhylip::promlk ( wxString s )
	{
	return runapp ( _T("promlk") , s ) ;
	}


wxString TPhylip::dnadist ( wxString s )
	{
	return runapp ( _T("dnadist") , s ) ;
	}


wxString TPhylip::protdist ( wxString s )
	{
	return runapp ( _T("protdist") , s ) ;
	}


wxString TPhylip::restdist ( wxString s )
	{
	return runapp ( _T("restdist") , s ) ;
	}


wxString TPhylip::restml ( wxString s )
	{
	return runapp ( _T("restml") , s ) ;
	}


wxString TPhylip::seqboot ( wxString s )
	{
	return runapp ( _T("seqboot") , s ) ;
	}


wxString TPhylip::fitch ( wxString s )
	{
	return runapp ( _T("fitch") , s ) ;
	}


wxString TPhylip::kitsch ( wxString s )
	{
	return runapp ( _T("kitsch") , s ) ;
	}


wxString TPhylip::neighbor ( wxString s )
	{
	return runapp ( _T("neighbor") , s ) ;
	}


wxString TPhylip::contml ( wxString s )
	{
	return runapp ( _T("contml") , s ) ;
	}


wxString TPhylip::gendist ( wxString s )
	{
	return runapp ( _T("gendist") , s ) ;
	}


wxString TPhylip::contrast ( wxString s )
	{
	return runapp ( _T("contrast") , s ) ;
	}


wxString TPhylip::pars ( wxString s )
	{
	return runapp ( _T("pars") , s ) ;
	}


wxString TPhylip::mix ( wxString s )
	{
	return runapp ( _T("mix") , s ) ;
	}


wxString TPhylip::move ( wxString s )
	{
	return runapp ( _T("move") , s ) ;
	}


wxString TPhylip::penny ( wxString s )
	{
	return runapp ( _T("penny") , s ) ;
	}


wxString TPhylip::dollop ( wxString s )
	{
	return runapp ( _T("dollop") , s ) ;
	}


wxString TPhylip::dolmove ( wxString s )
	{
	return runapp ( _T("dolmove") , s ) ;
	}


wxString TPhylip::dolpenny ( wxString s )
	{
	return runapp ( _T("dolpenny") , s ) ;
	}


wxString TPhylip::clique ( wxString s )
	{
	return runapp ( _T("clique") , s ) ;
	}


wxString TPhylip::factor ( wxString s )
	{
	return runapp ( _T("factor") , s ) ;
	}


wxString TPhylip::drawgram ( wxString s )
	{
	return runapp ( _T("drawgram") , s ) ;
	}


wxString TPhylip::drawtree ( wxString s )
	{
	return runapp ( _T("drawtree") , s ) ;
	}


wxString TPhylip::treedist ( wxString s )
	{
	return runapp ( _T("treedist") , s ) ;
	}


wxString TPhylip::consense ( wxString s )
	{
	return runapp ( _T("consense") , s ) ;
	}


wxString TPhylip::retree ( wxString s )
	{
	return runapp ( _T("retree") , s ) ;
	}


//***************************

wxString TPhylip::runapp ( wxString app , const wxString s )
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
	wxString owd = wxGetWorkingDirectory () ;
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
//	wxRemoveFile ( datafile ) ;
#endif

	wxFile outtree ( tmpdir + _T("/outtree") ) ;
	long l = outtree.Length () ;
	char *n = new char[l+5] ;
	outtree.Read ( n , l ) ;
	outtree.Close () ;
	n[l] = 0 ;
	wxString result ( n , wxConvUTF8 ) ;
	delete n ;
	
//	result = "(3:0.23647,(4:0.12270,2:0.00296):0.29673,1:0.65529);" ;
	
	return result.BeforeFirst ( ';' ) ;
	}
