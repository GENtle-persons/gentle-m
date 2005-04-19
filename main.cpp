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
           n = "" ;
           }
        else n += s.GetChar(a) ;
        }
    if ( !n.IsEmpty() ) r.Add ( n ) ; 
    }
    
wxString implode ( wxString sep , wxArrayString &r )
	{
	if ( r.GetCount() == 0 ) return "" ;
	wxString ret = r[0] ;
	for ( int a = 1 ; a < r.GetCount() ; a++ )
		ret += sep + r[a] ;
	return ret ;
	}
	
char * txt ( wxString item )
    {
    return (char*) myapp()->_text[item.MakeUpper()].c_str() ;
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
        regKey.SetValue ( "" , extension + "file" ) ;
        }    
  
    wxString s = "" , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += t ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;

    if ( !regKey.Exists() ) regKey.Create () ;

    regKey.SetValue ( "" , "\"" + wxString ( myapp()->argv[0] ) + "\" \"%1\"" ) ;
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
        regKey.SetValue ( "" , "URL: GENtle Protocol" ) ;
        regKey.SetValue ( "URL Protocol" , "" ) ;
        }    
  
    wxString s = "" , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
//    s += t ;
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
    errout = NULL ;
    total_log_time = 0 ;
    total_log_counter = 0 ;
#ifdef MYLOG
    wxStartTimer() ;
#endif

    wxString s1 , s2 ;
    wxFileName::SplitPath ( argv[0] , &homedir , &s1 , &s2 ) ;
    wxInitAllImageHandlers() ;
    wxFileSystem::AddHandler ( new wxInternetFSHandler ) ;
    
    wxSetWorkingDirectory ( homedir ) ;
    
    // Is an instance already running?
    const wxString name = wxString::Format("GENtle-%s", wxGetUserId().c_str());
    m_checker = new wxSingleInstanceChecker (name);
    if ( m_checker->IsAnotherRunning() )
    {
        wxLogError(_("Another program instance is already running, aborting."));
        return false;
    }
    theapp = this ;
    dbWarningIssued = false ;
    programVersion = 0 ; // This ensures that no old program version messes with a new database scheme
    
    wxFileSystem::AddHandler(new wxInternetFSHandler);
    
    // Create the main frame window
#ifdef __WXMSW__
    slash = "\\" ;
#else
    slash = "/" ;
#endif
    bmpdir = homedir + slash + "bitmaps" ;
    
    // Make sure local database exists
    wxString localdb , blankdb , hd = homedir ;
    localdb = hd + slash + "local.db" ;
    blankdb = hd + slash + "blank.db" ;
    if ( !wxFileExists ( localdb ) && wxFileExists ( blankdb ) )
        wxCopyFile ( blankdb , localdb ) ;

    frame = new MyFrame((wxFrame *)NULL, -1, "",
                        wxPoint(-1, -1), wxSize(500, 400),
                        wxDEFAULT_FRAME_STYLE );    
    frame->initme () ;
    if ( frame->dying ) return FALSE ;
    SetTopWindow(frame);

    if ( frame->showSplashScreen )
        {
        wxBitmap bitmap;
        wxString bmpfile = bmpdir + slash + "splash.bmp" ;
        if (bitmap.LoadFile(bmpfile, wxBITMAP_TYPE_BMP))
            {
            wxSplashScreen* splash = new wxSplashScreen(bitmap,
            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
            2500, NULL, -1, wxDefaultPosition, wxDefaultSize,
            wxSIMPLE_BORDER|wxSTAY_ON_TOP);
            }
        wxYield();
        }
        
    // Tips turned off until I can figure out how the hell
    // to get the state of the !"§$%& show-again-checkbox
    bool showTip = frame->LS->getOption ( "SHOWTIP" , false ) ;
    if ( showTip )
        {
        int tip = frame->LS->getOption ( "NEXTTIP" , 0 ) ;
        wxString tipfile = "tips_" ;
        tipfile += frame->lang_string ;
        tipfile += ".txt" ;
        wxTipProvider *tipProvider = wxCreateFileTipProvider(tipfile, tip);
        showTip = wxShowTip(frame, tipProvider, showTip);
//        showTip = tipProvider->ShowTipsOnStartup() ;
        tip = tipProvider->GetCurrentTip() ;
//        frame->LS->setOption ( "SHOWTIP" , showTip ) ;
        frame->LS->setOption ( "NEXTTIP" , tip ) ;
        frame->LS->setOption ( "SHOWTIP" , showTip ) ;
        delete tipProvider;
        }
        
    if ( frame->doRegisterStuff )
    	{
        registerFileExtension ( "gb" ) ;
        registerFileExtension ( "genbank" ) ;
        registerFileExtension ( "gbxml" ) ;
        registerFileExtension ( "fasta" ) ;
        registerFileExtension ( "clone" ) ;
        registerFileExtension ( "abi" ) ;
        registerFileExtension ( "ab1" ) ;
        registerFileExtension ( "seq" ) ;
        registerFileExtension ( "gcg" ) ;
        registerFileExtension ( "codata" ) ;
        registerFileExtension ( "NBRF_PIR" ) ;
        registerFileExtension ( "swissprot" ) ;
        registerProtocol ( "gentle" ) ;
        }    

    return TRUE;
}

wxString MyApp::get_GENtle_version ()
	{
 	return wxString::Format ( "%d.%d.%d" , GENTLE_VERSION_MAJOR , 
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
    logout->Write ( "Total log time : " + wxString::Format ( "%d ms" , total_log_time ) + "\n" ) ;
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
    ret = getFileFormatCommand ( command , "html" ) ;
    if ( ret.IsEmpty() ) ret = getFileFormatCommand ( command , "htm" ) ;
    if ( !ret.IsEmpty() ) return ret ;

// Fallback
#ifdef __WXMSW__    
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\.html");
    regKey.SetName(idName);    
    wxString s = "" , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += t ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;
    wxString q = regKey ;
    regKey.Close();
    q.Replace ( "-nohome" , "" ) ;
    if ( 0 == q.Replace ( wxString("%1") , wxString((char*)command.c_str()) ) )
        q += " \"" + command + "\"" ;
    return q ;
#else
    return "" ;
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
    if ( !ft ) return "" ;
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
    wxString s = "" , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += t ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;
    wxString q = regKey ;
    regKey.Close();
    q.Replace ( wxString("%1") , "" ) ;
    return q ;
#else
    return "" ;
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
    if ( !errout ) errout = new wxFile ( "ERROR.txt" , wxFile::write ) ;
    errout->Write ( msg + "\n" ) ;
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
    if ( !logout ) logout = new wxFile ( "LOG.txt" , wxFile::write ) ;
    if ( total_log_counter > 5000 )
    	{
	    logout->Close () ;
	    logout->Open ( "LOG.txt" , wxFile::write ) ;
	    total_log_counter = 0 ;
    	}
    total_log_counter++ ;

    int i = wxGetElapsedTime() ;
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
void MyApp::init_txt ( wxString lang , wxString csv , wxHashString *target , int ln )
    {
    if ( !target ) target = &_text ;
    wxTextFile in ( myapp()->homedir + "/" + csv) ;
    in.Open () ;
    unsigned char t[10000] ;
    bool firstline = true ;
    TGenBank dummy ;
    for ( int lc = 0 ; lc < in.GetLineCount() ; lc++ )
        {
        wxArrayString v ;
        strcpy ( (char*)t , in[lc].c_str() ) ;
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
              v.Add ( dummy.trimQuotes(l) ) ;
              l = c+1 ;
              }
           }
        if ( l < c )
           {
           *(c-1) = 0 ;
           v.Add ( dummy.trimQuotes(l) ) ;
           }
        
        if ( firstline )
           {
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

