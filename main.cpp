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

#ifdef __WXMSW__
#include "wx/msw/registry.h"
#endif

using namespace std ;

IMPLEMENT_APP(MyApp)


// Some often needed stuff...

vector <string> explode ( string sep , string s )
    {
    int a , b , l = 0 ;
    vector <string> r ;
    if ( s != "" ) s += sep ;
    for ( a = 0 ; a + sep.length() <= s.length() ; a++ )
       {
       for ( b = 0 ; b < sep.length() && sep[b] == s[a+b] ; b++ ) ;
       if ( b == sep.length() )
          {
          string t ;
          for ( b = l ; b < a ; b++ ) t += s[b] ;
          r.push_back ( t ) ;
          l = a + sep.length() ;
          a = l ;
          }
       }
    return r ;
    }


// ===========================================================================
// implementation
// ===========================================================================

#include "Text.inc"

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------
#include <wx/splash.h>

MyApp *theapp ;

MyApp *myapp ()
   {
   return theapp ;
   }

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
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
    homedir = wxGetCwd() ;
    bmpdir = homedir + slash + "bitmaps" ;
    
    // Make sure local database exists
    wxString localdb , blankdb , hd = homedir.c_str() ;
    localdb = hd + slash + "local.db" ;
    blankdb = hd + slash + "blank.db" ;
    if ( !wxFileExists ( localdb ) && wxFileExists ( blankdb ) )
        wxCopyFile ( blankdb , localdb ) ;

    frame = new MyFrame((wxFrame *)NULL, -1, "",
                        wxPoint(-1, -1), wxSize(500, 400),
                        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
    
    frame->app = this ;
    frame->initme () ;
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

    return TRUE;
}

int MyApp::OnExit ()
    {
    delete m_checker;
    return 0;
    }
    
string MyApp::getHTMLCommand ( string command )
    {
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
    q.Replace ( wxString("%1") , wxString((char*)command.c_str()) ) ;
    return q.c_str() ;
#else
    return "" ;
#endif
    }
    
string MyApp::getFileFormatApplication ( string type )
    {
#ifdef __WXMSW__    
    wxRegKey regKey;
    wxString idName("HKEY_CLASSES_ROOT\\.");
    idName += type.c_str() ;
    regKey.SetName(idName);    
    wxString s = "" , t = regKey ;
    s += "HKEY_CLASSES_ROOT\\" ;
    s += t ;
    s += "\\shell\\open\\command" ;
    regKey.SetName ( s ) ;
    wxString q = regKey ;
    regKey.Close();
    q.Replace ( wxString("%1") , "" ) ;
    return q.c_str() ;
#else
    return "" ;
#endif
    }
    
// ****************************

wxToolBar *ChildBase::CreateToolBar ( int i )
{
#ifdef __WXMSW__
  return wxMDIChildFrame::CreateToolBar ( i , -1 , "" ) ;
#else
  wxToolBar *bar = new wxToolBar ( app->frame , -1 ) ;
  //  SetToolBar ( bar ) ;
  return bar ;
  //  return app->frame->GetToolBar () ;
#endif
}

void ChildBase::Maximize ( bool isit )
{
  wxMDIChildFrame::Maximize ( isit ) ;
  showName () ;
}

void ChildBase::showName ( string x )
    {
    if ( x == "" )
       {
       x = getName() ;
       if ( vec && vec->isChanged() ) x += "*" ;
       }
    if ( GetTitle().c_str() != x ) SetTitle ( x.c_str() ) ; 
    }
    
void ChildBase::OnFocus(wxFocusEvent& event)
    {
    MyFrame *f = (MyFrame*) GetParent() ;
    if ( f->dying ) return ;
    showName () ;
    f->mainTree->SelectItem ( inMainTree ) ;
    }

bool ChildBase::caniclose(wxCloseEvent& event)
    {
    if ( event.CanVeto() && vec && vec->isChanged() )
       {
       char t[1000] ;
       sprintf ( t , txt("t_close_anyway") , getName().c_str() ) ;

       int r = wxMessageBox ( t , txt("msg_box"),
                          wxICON_QUESTION | wxYES | wxNO | wxCANCEL ) ;
       
       if ( r == wxYES )
          {
          wxCommandEvent ev ;
          OnFileSave ( ev ) ;
          return true ;
          }
       else if ( r == wxCANCEL )
          {
          event.Veto();
          return false ;
          }
       }
    return true ;
    }

