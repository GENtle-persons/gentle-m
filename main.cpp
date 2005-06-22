/** \file main.cpp
	\brief Contains global functions, includes, and the MyApp class
 */


/**	\mainpage GENtle - the source code documentation

	\section players The big players
	
	There are some classes that are of fundamental importance to understanding the GENtle code.
	<ul>
	<li>MyFrame - The base window of the whole application
 	<li>ChildBase - The "mother" of all modules, including the oddly named DNA module, MyChild
	<ul>
	<li>PlasmidCanvas - The colorful map used in the DNA and, partially, amino acid modules
	<li>SequenceCanvas - The universal sequence text class, used in most of the modules
	<ul>
	<li>SeqBasic - The base class for each "line type" in a SequenceCanvas, including DNA, amino acids, features, restriction enzymes, ABI peaks, etc.
	</ul>
	</ul>
    <li>TStorage - The class to communicate with databases
    <li>TVector - The class to store all sequence information, be it DNA or amino acids
    </ul>
	
*/

/////////////////////////////////////////////////////////////////////////////
// Name:        GENtle
// Purpose:     DNA/AA manipulation
// Author:      Magnus Manske
// Modified by:
// Created:     2002
// Copyright:   (c) Magnus Manske
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".


#include "main.h"
#include <wx/tipdlg.h> 
#include <wx/splash.h>
#include <wx/filesys.h>
#include <wx/file.h>

#ifdef __WXMSW__
#include "wx/msw/registry.h"
#endif


using namespace std ;

IMPLEMENT_APP(MyApp)

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxArrayFloat);

// GLOBAL FUNCTIONS
int cmpint(int *first, int *second)
    {
    return *first > *second ;
    }    
    
int cmpre(TRestrictionEnzyme *first, TRestrictionEnzyme *second)
    {
    return first > second ; //????
    }    

void wxStringInsert ( wxString &s , int from , wxString t )
    {
    s = s.Mid ( 0 , from ) + t + s.Mid ( from ) ;
    }    

void explode ( wxString sep , wxString s , wxArrayString &r )
    {
    int a , b ;
    wxString n ;
    r.Clear () ;
    for ( a = 0 ; a + sep.Length() <= s.Length() ; a++ )
        {
        for ( b = 0 ; b < sep.Length() && s.GetChar(a+b) == sep.GetChar(b) ; b++ ) ;
        if ( b == sep.Length() )
           {
           r.Add ( n ) ;
           n = _T("") ;
           }
        else n += s.GetChar(a) ;
        }
    if ( !n.IsEmpty() ) r.Add ( n ) ; 
    }
    
wxString implode ( wxString sep , wxArrayString &r )
	{
	if ( r.GetCount() == 0 ) return _T("") ;
	wxString ret = r[0] ;
	for ( int a = 1 ; a < r.GetCount() ; a++ )
		ret += sep + r[a] ;
	return ret ;
	}
/*
char * txt ( char *item )
    {
    return (char*) myapp()->_text[item.MakeUpper()].c_str() ;
    }

char * txt ( wxString item )
    {
    return (char*) myapp()->_text[item.MakeUpper()].c_str() ;
    }
*/
wxString txt ( char *item )
    {
    return myapp()->_text[wxString(item,wxConvUTF8).MakeUpper()] ;
    }

wxString txt ( wxString item )
    {
    return myapp()->_text[item.MakeUpper()] ;
    }

// END GLOBAL FUNCTIONS


void MyApp::registerFileExtension ( wxString extension )
    {
#ifdef __WXMSW__    
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\."+extension);
    regKey.SetName(idName);    
  
    if ( !regKey.Exists() )
        {
        regKey.Create() ;
        regKey.SetValue ( _T("") , extension + _T("file") ) ;
        }    
  
    wxString s = _T("") , t = regKey ;
    s += _T("HKEY_CLASSES_ROOT\\") ;
    s += t ;
    s += _T("\\shell\\open\\command") ;
    regKey.SetName ( s ) ;

    if ( !regKey.Exists() ) regKey.Create () ;

    regKey.SetValue ( _T("") , _T("\"") + wxString ( myapp()->argv[0] ) + _T("\" \"%1\"") ) ;
#else
#endif
    }
    
void MyApp::registerProtocol ( wxString extension )
    {
#ifdef __WXMSW__    
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\"+extension);
    regKey.SetName(idName);    
  
    if ( !regKey.Exists() )
        {
        regKey.Create () ;
        regKey.SetValue ( _T("") , _T("URL: GENtle Protocol") ) ;
        regKey.SetValue ( _T("URL Protocol") , _T("") ) ;
        }    
  
    wxString s , t = regKey ;
    s += _T("HKEY_CLASSES_ROOT\\") ;
	 s += extension ;
    s += _T("\\shell\\open\\command") ;
    regKey.SetName ( s ) ;

    if ( !regKey.Exists() ) regKey.Create () ;

    regKey.SetValue ( _T("") , _T("\"") + wxString ( myapp()->argv[0] ) + _T("\" \"%1\"") ) ;
#else
#endif
    }

MyApp *theapp ; /**< \var theapp Pointer to the current application. */

MyApp *myapp ()
   {
   return theapp ;
   }


// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

/**	\fn MyApp::OnInit()
	\brief Initializes the application.
	
	* - Initializes variables
	* - Initializes handlers
	* - Checks if another program instance is already running
	* - Creates a new local database, if necessary
	* - Creates the frame
	* - Shows the splash screen
	* - Shows Tip of the Day (currently deactivated)
	* - Registers file extensions
*/
bool MyApp::OnInit()
{
	isoconv = new wxCSConv ( _T("iso-8859-1") ) ;
    wxConvCurrent = isoconv ;
	errout = NULL ;
    total_log_time = 0 ;
    total_log_counter = 0 ;
#ifdef MYLOG
    wxStartTimer() ;
#endif

    wxString s1 , s2 ;
    wxFileName::SplitPath ( argv[0] , &homedir , &s1 , &s2 ) ;

#ifdef __WXMAC__
	homedir = homedir.BeforeLast ( '/' ) ;
	homedir += _T("/Resources") ;
	
	wxApp::s_macAboutMenuItemId = MDI_ABOUT;
	wxApp::s_macPreferencesMenuItemId = PROGRAM_OPTIONS;
	wxApp::s_macExitMenuItemId = MDI_QUIT;
//	wxApp::s_macHelpMenuTitleName = "Help";
	
#endif

    wxInitAllImageHandlers() ;
    wxFileSystem::AddHandler ( new wxInternetFSHandler ) ;
    
    wxSetWorkingDirectory ( homedir ) ; // Setting home directory as working dir

	// Setting ncoils dir as an environment variable
	wxString ncoilsdir ;
	ncoilsdir = _T("COILSDIR=") ;
	ncoilsdir += homedir ;
	putenv ( (char*) ncoilsdir.mb_str() ) ;

    // Is an instance already running?
    const wxString name = wxString::Format ( _T("GENtle-%s") , wxGetUserId().c_str());
    m_checker = new wxSingleInstanceChecker (name);
    if ( m_checker->IsAnotherRunning() )
    {
        wxLogError(_T("Another program instance is already running, aborting."));
        return false;
    }
    theapp = this ;
    dbWarningIssued = false ;
    programVersion = 0 ; // This ensures that no old program version messes with a new database scheme

    wxFileSystem::AddHandler(new wxInternetFSHandler);
    
    // Create the main frame window
#ifdef __WXMSW__
    slash = _T("\\") ;
#else
    slash = _T("/") ;
#endif

#ifdef __WXMAC__
	bmpdir = homedir ;
#else
    bmpdir = homedir + slash + _T("bitmaps") ;
#endif
     
	 // Make sure local database exists
    wxString localdb , blankdb ;
    localdb = getLocalDBname() ;
    blankdb = homedir + slash + _T("blank.db") ;
    if ( !wxFileExists ( localdb ) && wxFileExists ( blankdb ) )
        wxCopyFile ( blankdb , localdb ) ;

    frame = new MyFrame((wxFrame *)NULL, -1, _T(""),
                        wxPoint(-1, -1), wxSize(500, 400),
                        wxDEFAULT_FRAME_STYLE );    
    frame->initme () ;
    if ( frame->dying ) return FALSE ;
    SetTopWindow(frame);

    if ( frame->showSplashScreen )
        {
        wxBitmap bitmap;
        wxString bmpfile = bmpdir + slash + _T("splash.bmp") ;
        if (bitmap.LoadFile(bmpfile, wxBITMAP_TYPE_BMP))
            {
            //wxSplashScreen* splash = 
			new wxSplashScreen(bitmap,
            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
            2500, NULL, -1, wxDefaultPosition, wxDefaultSize,
            wxSIMPLE_BORDER|wxSTAY_ON_TOP);
            }
        wxYield();
        }

    // Tips turned off until I can figure out how the hell
    // to get the state of the !"$%& show-again-checkbox
    bool showTip = frame->LS->getOption ( _T("SHOWTIP") , false ) ;
    if ( showTip )
        {
        int tip = frame->LS->getOption ( _T("NEXTTIP") , 0 ) ;
        wxString tipfile = _T("tips_") ;
        tipfile += frame->lang_string ;
        tipfile += _T(".txt") ;
        wxTipProvider *tipProvider = wxCreateFileTipProvider(tipfile, tip);
        showTip = wxShowTip(frame, tipProvider, showTip);
//        showTip = tipProvider->ShowTipsOnStartup() ;
        tip = tipProvider->GetCurrentTip() ;
//        frame->LS->setOption ( "SHOWTIP" , showTip ) ;
        frame->LS->setOption ( _T("NEXTTIP") , tip ) ;
        frame->LS->setOption ( _T("SHOWTIP") , showTip ) ;
        delete tipProvider;
        }
        
    if ( frame->doRegisterStuff )
    	{
        registerFileExtension ( _T("gb") ) ;
        registerFileExtension ( _T("genbank") ) ;
        registerFileExtension ( _T("gbxml") ) ;
        registerFileExtension ( _T("fasta") ) ;
        registerFileExtension ( _T("clone") ) ;
        registerFileExtension ( _T("abi") ) ;
        registerFileExtension ( _T("ab1") ) ;
        registerFileExtension ( _T("seq") ) ;
        registerFileExtension ( _T("gcg") ) ;
        registerFileExtension ( _T("codata") ) ;
        registerFileExtension ( _T("NBRF_PIR") ) ;
        registerFileExtension ( _T("swissprot") ) ;
        registerProtocol ( _T("gentle") ) ;
        }    

    return TRUE;
}

wxString MyApp::getLocalDBname ()
	{
#ifdef _-WXMAC__
	return wxGetHomeDir() + myapp()->slash + _T("local.db") ;
#else
	return myapp()->homedir + myapp()->slash + _T("local.db") ;
#endif
	}

wxString MyApp::get_GENtle_version ()
	{
 	return wxString::Format ( _T("%d.%d.%d") , GENTLE_VERSION_MAJOR ,
  											GENTLE_VERSION_MINOR , 
  											GENTLE_VERSION_SUB ) ;
	}    

/**	\fn MyApp::OnExit ()
	\brief Exits the application.
	
	* - Finished a log, if one is written
	* - deletes the wxSingleInstanceChecker
*/
int MyApp::OnExit ()
    {
#ifdef MYLOG
    logout->Write ( _T("Total log time : ") + wxString::Format ( _T("%d ms") , total_log_time ) + _T("\n") ) ;
    logout->Flush() ;
#endif
    delete m_checker;
    return 0;
    }
    
/**	\fn MyApp::getHTMLCommand ( wxString command )
	\brief Returns the command line to invoke the browser.
	\param command The URL/file.
*/
wxString MyApp::getHTMLCommand ( wxString command )
    {
    wxString ret ;
    ret = getFileFormatCommand ( command , _T("html") ) ;
    if ( ret.IsEmpty() ) ret = getFileFormatCommand ( command , _T("htm") ) ;
    if ( !ret.IsEmpty() ) return ret ;

// Fallback
#ifdef __WXMSW__    
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\.html");
    regKey.SetName(idName);    
    wxString s , t = regKey ;
    s += _T("HKEY_CLASSES_ROOT\\") ;
    s += t ;
    s += _T("\\shell\\open\\command") ;
    regKey.SetName ( s ) ;
    wxString q = regKey ;
    regKey.Close();
    q.Replace ( _T("-nohome") , _T("") ) ;
    if ( 0 == q.Replace ( wxString ( _T("%1") ) , wxString((char*)command.c_str()) ) )
        q += _T(" \"") + command + _T("\"") ;
    return q ;
#else
    return _T("") ;
#endif
    }

/**	\fn MyApp::getHTMLCommand ( wxString command )
	\brief Returns the command line to invoke the browser.
	\param type The file ending to find the application for.
	\param file The URL/file.
*/
wxString MyApp::getFileFormatCommand ( wxString type , wxString file )
	{
    wxFileType *ft = mtm.GetFileTypeFromExtension ( type ) ;
    if ( !ft ) return _T("") ;
    return ft->GetOpenCommand ( file ) ;
	}    
    
/**	\fn MyApp::getFileFormatApplication ( wxString type )
	\brief Returns the application associated with a file type. Windows only.
	\param type The file ending to find the application for.
*/
wxString MyApp::getFileFormatApplication ( wxString type )
    {
#ifdef __WXMSW__    
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\.");
    idName += type ;
    regKey.SetName(idName);    
    wxString s , t = regKey ;
    s += _T("HKEY_CLASSES_ROOT\\") ;
    s += t ;
    s += _T("\\shell\\open\\command") ;
    regKey.SetName ( s ) ;
    wxString q = regKey ;
    regKey.Close();
    q.Replace ( wxString ( _T("%1") ) , _T("") ) ;
    return q ;
#else
    return _T("") ;
#endif
    }

/**	\fn MyApp::do_my_ass ( bool b , wxString msg )
	\brief "My assertion" - little inside joke...
	\param b The condition given in the call. No assertion when b is FALSE.
	\param msg The message string to write into errout.
	
	The function writes the text of "msg" to the ERROR.txt file
	in case "b" is true. This is done to catch possible out-of-range errors.
	The function should not be used in releases, as the mere out-of-range check
	might significantly impact on performance. Called as "myass" (see main.h defs).
*/
void MyApp::do_my_ass ( bool b , wxString msg )
    {
    if ( b ) return ;
    if ( !errout ) errout = new wxFile ( _T("ERROR.txt") , wxFile::write ) ;
    errout->Write ( msg + _T("\n") ) ;
    errout->Flush() ;
    }
    
/**	\fn MyApp::do_my_log ( wxString function , wxString msg )
	\brief Logs events to a file.
	\param function The originating function name of the log event.
	\param msg The message string to write into logout.
	
	The function writes the text of "msg" to the LOG.txt file
	together with the "function" name. This is done to log events and
	variable values at certain points in the code. This should not
	be used in releases, as it <b>will</b> severely impact on
	performance. Called as "mylog" (see main.h defs).
*/
void MyApp::do_my_log ( wxString function , wxString msg )
    {
    if ( !logout ) logout = new wxFile ( _T("LOG.txt") , wxFile::write ) ;
    if ( total_log_counter > 5000 )
    	{
	    logout->Close () ;
	    logout->Open ( _T("LOG.txt") , wxFile::write ) ;
	    total_log_counter = 0 ;
    	}
    total_log_counter++ ;

    int i = wxGetElapsedTime() ;
    total_log_time += i ;
    logout->Write ( function + _T(" : ") + msg + _T(" (") + wxString::Format ( _T("%d ms") , i ) + _T(")\n") ) ;
    logout->Flush() ;
    }

/** \fn MyApp::init_txt ( wxString lang , wxString csv , wxHashString *target , int ln )
	\brief Initializes a hash table from a CSV file
	\param lang the column identifier (first row of the CSV file)
	\param csv the CSV file
	\param target the hash table; if NULL (default), the language table is created
	\param ln the default language number
*/
void MyApp::init_txt ( wxString lang , wxString csv , wxHashString *target , int ln )
    {
    if ( !target ) target = &_text ;
    wxTextFile in ( myapp()->homedir + _T("/") + csv) ;
    in.Open ( *isoconv ) ;
    unsigned char t[10000] ;
    bool firstline = true ;
    TGenBank dummy ;
    for ( int lc = 0 ; lc < in.GetLineCount() ; lc++ )
        {
        wxArrayString v ;
        strcpy ( (char*)t , in[lc].mb_str() ) ;
        if ( *t == 0 ) break ;
        unsigned char *c , *l ;
        bool quote = false ;
        for ( c = l = (unsigned char*) t ; *c ; c++ )
           {
           if ( *c == '\\' )
              {
              *c = ' ' ;
              c++ ;
              if ( *c == 't' ) *c = '\t' ;
              else if ( *c == '"' ) *c = '"' ;
              else if ( *c == 'n' ) *c = '\n' ;
              }
           else if ( *c == '"' ) quote = !quote ;
           else if ( *c == ',' && !quote )
              {
              *c = 0 ;
              v.Add ( dummy.trimQuotes(wxString((char*)l,wxConvUTF8)) ) ;
//              v.Add ( dummy.trimQuotes(CHAR2WX(l)));
              l = c+1 ;
              }
           }
        if ( l < c )
           {
           *(c-1) = 0 ;
           v.Add ( dummy.trimQuotes(wxString((char*)l,wxConvUTF8)) ) ;
//           v.Add ( dummy.trimQuotes(CHAR2WX(l)));
           }
        
        if ( firstline )
           {
           for ( int a = 0 ; a < v.GetCount() ; a++ )
              if ( v[a].Upper() == lang.Upper() )
                 ln = a ;
           }
        else if ( v.GetCount() > ln )
           {
           if ( v[ln].Find ( '\t' ) > -1 ) v[ln] += _T(" ") ;
           (*target)[v[0].MakeUpper()] = v[ln] ;
           }
        firstline = false ;
        }
    }
