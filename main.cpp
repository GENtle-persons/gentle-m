/** \file main.cpp
    \brief Contains global functions, includes, and the MyApp class
 */


/**    \mainpage GENtle - the source code documentation

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

// ===
// declarations
// ===

// ---
// headers
// ---

// For compilers that support precompilation, includes "wx/wx.h".


#include "main.h"
#include <wx/tipdlg.h>
#include <wx/splash.h>
#include <wx/filesys.h>
#include <wx/file.h>
#include <wx/filefn.h> // wxCopyFile
#include <wx/filename.h> // wxFileExists

#ifdef __WXMSW__
#include "wx/msw/registry.h"
#endif


using namespace std ;

IMPLEMENT_APP(MyApp)

void MyApp::registerFileExtension ( const wxString& extension )
    {
#ifdef __WXMSW__
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\."+extension);
    regKey.SetName(idName);

    if ( !regKey.Exists() )
        {
        regKey.Create() ;
        regKey.SetValue ( "" , extension + "file" ) ;
        }

    wxString s = "" , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += t ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;

    if ( !regKey.Exists() ) regKey.Create () ;

    regKey.SetValue ( "" , "\"" + wxString ( myapp()->argv[0] ) + "\" \"%1\"")  ;
#else
#endif
    }

void MyApp::registerProtocol ( const wxString& extension )
    {
#ifdef __WXMSW__
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\"+extension);
    regKey.SetName(idName);

    if ( !regKey.Exists() )
        {
        regKey.Create () ;
        regKey.SetValue ( "" , "URL: GENtle Protocol" ) ;
        regKey.SetValue ( "URL Protocol" , "" ) ;
        }

    wxString s , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += extension ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;

    if ( !regKey.Exists() ) regKey.Create () ;

    regKey.SetValue ( "" , "\"" + wxString ( myapp()->argv[0] ) + "\" \"%1\"" ) ;
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

/** \fn MyApp::OnInit()
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
    // Handling command-line arguments
    for(int i=1; i<argc; i++)
        {
        if (0==strcmp("--version",argv[i]) || 0==strcmp("-v",argv[i]) )
            {
            wxPrintf("%s\n",get_GENtle_version());
            exit(0);
            }
        else if (0==strcmp("--help",argv[i]) || 0==strcmp("-h",argv[i]))
            {
            wxPrintf("%s","Please see the GENtle man page for help.\n");
            exit(0);
            }
        else
            {
            wxPrintf("%s","Unknown option '%s'.\n",argv[i]);
            exit(1);
            }
        }

    isoconv = new wxCSConv ( "iso-8859-1" ) ;
    wxConvCurrent = isoconv ;
    errout = NULL ;
    total_log_time = 0 ;
    total_log_counter = 0 ;
#ifdef MYLOG
    wxStartTimer() ;
#endif

    wxString tmp;
    if ( wxGetEnv ( "GENTLE_HOMEDIR" , &tmp ) )
        {
        homedir.AssignDir( tmp );
        wxPrintf( "I: GENtle's homedir variable set by eviroment: '%s'\n", homedir.GetFullPath() ) ;
        }
#ifdef HOMEDIR
    else if ( wxDirExists ( HOMEDIR ) )
        {
        wxPrintf( "I: GENtle's homedir assigned to configuration parameter '%s' since that dir is indeed existing.\n", HOMEDIR ) ;
            homedir.AssignDir( HOMEDIR ) ;
        }
    else
        {
        wxPrintf( "W: GENtle's homedir could not be assigned to configuration parameter '%s' since that dir was not found.\n", HOMEDIR ) ;
#else
    else
        {
#endif
        wxString h, s1 , s2 ;
        wxFileName::SplitPath ( argv[0] , &h, &s1 , &s2 ) ;
        if ( h.IsEmpty() )
            {
            homedir.AssignCwd() ;
            }
        else
            {
            homedir.AssignDir(h) ;
            }

//FIXME: Unfortunate - why differ from Windows?
#ifdef __WXMAC__
        if (wxNOT_FOUND != homedir.GetFullPath().Find( wxFileName::GetPathSeparator() ))
            {
            homedir.AssignDir(homedir.GetFullPath().BeforeLast ( '/' ) ) ;
            }
            //FIXME: Why?
            //homedir += "/Resources" ;
#endif
        wxPrintf( "I: GENtle's homedir variable self-determined: '%s'\n", homedir.GetFullPath() ) ;
        }

    homedir.MakeAbsolute() ;
    wxPrintf( "I: GENtle's homedir as absolute path: '%s'\n", homedir.GetFullPath() ) ;

#ifdef __WXMAC__
    wxApp::s_macAboutMenuItemId = MDI_ABOUT;
    wxApp::s_macPreferencesMenuItemId = PROGRAM_OPTIONS;
    wxApp::s_macExitMenuItemId = MDI_QUIT;
//    wxApp::s_macHelpMenuTitleName = "Help";

#endif


    wxInitAllImageHandlers() ;
    wxFileSystem::AddHandler ( new wxInternetFSHandler ) ;

    // Caveat: Because of this change of the working directory,
    //         all prior paths need to be absolute or relative to homedir.
    wxSetWorkingDirectory ( homedir.GetFullPath() ) ; // Setting home directory as working dir

    // Setting ncoils dir as an environment variable
    wxString ncoilsdir ;
    ncoilsdir = "COILSDIR=" ;
    ncoilsdir += homedir.GetFullPath() ;

#ifdef __WXMAC__
    if ( wxGetEnv ( "COILSDIR" , NULL ) ) wxUnsetEnv ( "COILSDIR" ) ;
    wxSetEnv ( "COILSDIR" , homedir.GetFullPath() ) ;
#else
    setenv ( "COILSDIR" , homedir.GetFullPath().c_str() , 1 ) ;
#endif

    // Is an instance already running?
    const wxString name = wxString::Format ( "GENtle-%s" , wxGetUserId().c_str());
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

    bmpdir.AssignDir(homedir.GetFullPath() + wxFileName::GetPathSeparator() + "bitmaps") ;
    wxPrintf("D: Assigned bmpdir to '%s'\n",bmpdir.GetFullPath());
    if ( !wxDirExists ( bmpdir.GetFullPath() ) )
        {
            wxPrintf("E: Could not find bitmap directory expected at '%s'.\n",bmpdir.GetFullPath());
        }

    // Make sure local database exists
    wxString localdb , blankdb ;
    localdb = getLocalDBname() ;
    blankdb = homedir.GetFullPath() + wxFileName::GetPathSeparator() + "blank.db" ;
    wxLogNull logNo; // Suppress error message
    if ( !wxFileExists ( localdb ) && wxFileExists ( blankdb ) )
        {
        wxCopyFile ( blankdb , localdb ) ;
        }

    // Check is local.db exists and is writable
    bool local_ok = true ;
    if ( !wxFileExists ( localdb ) )
            local_ok = false ;
    else
        {
        wxFile test ( localdb , wxFile::write_append ) ;
        if ( !test.IsOpened() )
            local_ok = false ;
        }

    if ( !local_ok )
        {
        theRealLocalDb = wxGetHomeDir() + wxFileName::GetPathSeparator()  + "local.db" ;
        localdb = theRealLocalDb ;
        if ( !wxFileExists ( localdb ) )
            {
            wxCopyFile ( blankdb , localdb ) ;
            }
        else local_ok = true ;
        }

    frame = new MyFrame((wxFrame *)NULL, -1, "",
                        wxPoint(-1, -1), wxSize(500, 400),
                        wxDEFAULT_FRAME_STYLE );
    frame->initme () ;
    if ( frame->dying ) return FALSE ;
    SetTopWindow(frame);

    if ( clp[_T("no-splash-screen")] == "1" )
        frame->showSplashScreen = false ;

    if ( !local_ok )
        wxMessageBox ( txt("t_local_db_warning") ) ;

    if ( frame->showSplashScreen )
        {
        wxBitmap bitmap;
        wxString bmpfile = bmpdir.GetFullPath() + wxFileName::GetPathSeparator() + "splash.bmp" ;
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
    bool showTip = frame->LS->getOption ( _T("SHOWTIP") , true ) ;

    if ( clp["no-tips"] == "1" )
        showTip = false ;

    if ( showTip )
        {
        int tip = frame->LS->getOption ( _T("NEXTTIP") , 0 ) ;
        wxString tipfile = "tips_" ;
        tipfile += frame->lang_string ;
        tipfile += ".txt" ;
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
        registerFileExtension ( "gb") ;
        registerFileExtension ( "genbank") ;
        registerFileExtension ( "gbxml") ;
        registerFileExtension ( "fasta") ;
        registerFileExtension ( "clone") ;
        registerFileExtension ( "abi") ;
        registerFileExtension ( "ab1") ;
        registerFileExtension ( "seq") ;
        registerFileExtension ( "gcg") ;
        registerFileExtension ( "codata") ;
        registerFileExtension ( "NBRF_PIR") ;
        registerFileExtension ( "swissprot") ;
        registerProtocol ( "gentle") ;
        }

    return TRUE;
    }

wxString MyApp::getLocalDBname () /* not const */
    {
    if ( !theRealLocalDb.IsEmpty() ) return theRealLocalDb ;
#ifdef __WXMAC__
    theRealLocalDb = wxGetHomeDir() + wxFileName::GetPathSeparator() + "local.db" ;
#else
    theRealLocalDb = myapp()->homedir.GetFullPath() + wxFileName::GetPathSeparator() + "local.db" ;
#endif
    return theRealLocalDb ;
    }

wxString MyApp::get_GENtle_version () const
    {
    return wxString::Format ( "%d.%d.%d" , GENTLE_VERSION_MAJOR , GENTLE_VERSION_MINOR , GENTLE_VERSION_SUB ) ;
    }

/**    \fn MyApp::OnExit ()
    \brief Exits the application.

    * - Finished a log, if one is written
    * - deletes the wxSingleInstanceChecker
*/
int MyApp::OnExit ()
    {
#ifdef MYLOG
    logout->Write ( _T("Total log time : ") + wxString::Format ( "%d ms" , total_log_time ) + "\n" ) ;
    logout->Flush() ;
#endif
    delete m_checker;
    return wxApp::OnExit () ;
//  return 0;
    }

void MyApp::launchBrowser ( const wxString& url )
    {
//  wxMessageBox ( url ) ;
//  if ( wxLaunchDefaultBrowser ( url ) ) return ;
    wxString command = myapp()->getHTMLCommand ( url ) ;
    wxExecute ( command ) ;
    }

/** \fn MyApp::getHTMLCommand ( wxString command )
    \brief Returns the command line to invoke the browser.
    \param command The URL/file.
*/
wxString MyApp::getHTMLCommand ( const wxString& command )
    {
#ifdef __WXMAC__
    return "open " + command ;
#endif
    wxString ret = getFileFormatCommand ( command , "html" ) ;
    if ( ret.IsEmpty() ) ret = getFileFormatCommand ( command , "htm" ) ;
    if ( !ret.IsEmpty() ) return ret ;

// Fallback
#ifdef __WXMSW__
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\.html");
    regKey.SetName(idName);
    wxString s , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += t ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;
    wxString q = regKey ;
    regKey.Close();
    q.Replace ( "-nohome" , "" ) ;
    if ( 0 == q.Replace ( wxString ( "%1" ) , command ) )
        q += " \"" + command + "\"" ;
    return q ;
#else
    return "" ;
#endif
    }

/** \fn MyApp::getFileFormatCommand ( wxString command )
    \brief Returns the command line to invoke the application.
    \param type The file ending to find the application for.
    \param file The URL/file.
*/
wxString MyApp::getFileFormatCommand ( const wxString& type , const wxString& file )
    {
    wxFileType *ft = mtm.GetFileTypeFromExtension ( type ) ;
    if ( !ft ) return "" ;
    return ft->GetOpenCommand ( file ) ;
    }

/** \fn MyApp::getFileFormatApplication ( wxString type )
    \brief Returns the application associated with a file type. Windows only.
    \param type The file ending to find the application for.
*/
wxString MyApp::getFileFormatApplication ( const wxString& type )
    {
#ifdef __WXMSW__
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\.");
    idName += type ;
    regKey.SetName(idName);
    wxString s , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += t ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;
    wxString q = regKey ;
    regKey.Close();
    q.Replace ( wxString ( "%1" ) , "" ) ;
    return q ;
#else
    return "" ;
#endif
    }

/** \fn MyApp::do_my_ass ( bool b , wxString msg )
    \brief "My assertion" - little inside joke...
    \param b The condition given in the call. No assertion when b is FALSE.
    \param msg The message string to write into errout.

    The function writes the text of "msg" to the ERROR.txt file
    in case "b" is true. This is done to catch possible out-of-range errors.
    The function should not be used in releases, as the mere out-of-range check
    might significantly impact on performance. Called as "myass" (see main.h defs).
*/
void MyApp::do_my_ass ( const bool b , const wxString& msg )
    {
    if ( b ) return ;
    if ( !errout ) errout = new wxFile ( "ERROR.txt" , wxFile::write ) ;
    errout->Write ( msg + "\n" ) ;
    errout->Flush() ;
    }

/** \fn MyApp::do_my_log ( wxString function , wxString msg )
    \brief Logs events to a file.
    \param function The originating function name of the log event.
    \param msg The message string to write into logout.

    The function writes the text of "msg" to the LOG.txt file
    together with the "function" name. This is done to log events and
    variable values at certain points in the code. This should not
    be used in releases, as it <b>will</b> severely impact on
    performance. Called as "mylog" (see main.h defs).
*/
void MyApp::do_my_log ( const wxString& function , const wxString& msg )
    {
    if ( !logout ) logout = new wxFile ( "LOG.txt" , wxFile::write ) ;
    if ( total_log_counter > 5000 )
        {
        logout->Close () ;
        logout->Open ( "LOG.txt" , wxFile::write ) ;
        total_log_counter = 0 ;
        }
    total_log_counter++ ;

    int i = sw.Time() ;
    total_log_time += i ;
    logout->Write ( function + " : " + msg + " (" + wxString::Format ( "%d ms" , i ) + ")\n" ) ;
    logout->Flush() ;
    }

/** \fn MyApp::init_txt ( wxString lang , wxString csv , wxHashString *target , int ln )
    \brief Initializes a hash table from a CSV file
    \param lang the column identifier (first row of the CSV file)
    \param csv the CSV file
    \param target the hash table; if NULL (default), the language table is created
    \param ln the default language number
*/
void MyApp::init_txt ( const wxString& lang , const wxString& csv , wxHashString *target , int ln )
    {
    if ( !target ) target = &_text ;
    wxTextFile in ( myapp()->homedir.GetFullPath() + wxFileName::GetPathSeparator() + csv) ;
    in.Open ( wxConvUTF8 ) ;
//  in.Open ( *isoconv ) ;
//  unsigned char t[10000] ;
    bool firstline = true ;
    TGenBank dummy ;

/*
#ifdef __WXGTK__
    wxMBConv *conv = &wxConvUTF8 ;
#else
    wxMBConv *conv = isoconv ;
#endif
*/

    for ( int lc = 0 ; lc < in.GetLineCount() ; lc++ )
        {
        wxString s = in[lc] ;
        if ( s.IsEmpty() ) continue ; // Blank line
        wxArrayString v ;
        s.Replace ( "\\t" , "\t" ) ;
        s.Replace ( "\\n" , "\n" ) ;
        s = s.Mid ( 1 ) ; // Get rid of initial "
        while ( 1 )
            {
            int f = s.Find ( "\",\"" ) ;
            if ( f == -1 )
                {
                s = s.Left ( s.length()-1 ) ; // Get rid of final "
                v.Add ( s ) ;
                break ;
                }
            else
                {
                v.Add ( s.Left ( f ) ) ;
                s = s.Mid ( f + 3 ) ;
                }
            }

        // Get key (firstline) or value
        if ( firstline )
            {
            if ( frame->language_list.IsEmpty() )
                {
                frame->language_list = v ;
                frame->language_list.RemoveAt ( 0 ) ;
                }
            for ( int a = 0 ; a < v.GetCount() ; a++ )
                if ( v[a].Upper() == lang.Upper() )
                    ln = a ;
            }
        else if ( v.GetCount() > ln )
            {
            if ( v[ln].Find ( '\t' ) > -1 ) v[ln] += " " ;
            (*target)[v[0].MakeUpper()] = v[ln] ;
            }
        firstline = false ;
        }
    }
