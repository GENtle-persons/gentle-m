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

#ifdef __WXMSW__
#include "wx/msw/registry.h"
#endif

using namespace std ;

IMPLEMENT_APP(MyApp)


// Some often needed stuff...

#ifdef MYDEBUG
wxFile errout ( "ERROR.txt" , wxFile::write ) ;

void myass ( bool b , wxString msg )
    {
    if ( b ) return ;
    errout.Write ( msg + "\n" ) ;
    errout.Flush() ;
    }
#endif

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

wxArrayString explode ( wxString sep , wxString s )
    {
    int a , b ;
    wxArrayString r ;
    wxString n ;
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
    return r ;
    }


// ===========================================================================
// implementation
// ===========================================================================

// Text
WX_DECLARE_STRING_HASH_MAP( wxString, wxHashString );

wxHashString _text ;

void init_txt ( wxString lang )
    {
    wxTextFile in ( myapp()->homedir + "\\variables.csv" ) ;
    in.Open () ;
    char t[10000] ;
    bool firstline = true ;
    int ln = 1 ; // English is default
    TGenBank dummy ;
    for ( int lc = 0 ; lc < in.GetLineCount() ; lc++ )
        {
        wxArrayString v ;
        strcpy ( t , in[lc].c_str() ) ;
        if ( *t == 0 ) break ;
        char *c , *l ;
        bool quote = false ;
        for ( c = l = t ; *c ; c++ )
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
              if ( v[a] == lang )
                 ln = a ;
           }
        else
           {
           if ( v[ln].Find ( '\t' ) > -1 ) v[ln] += " " ;
           _text[v[0].MakeUpper()] = v[ln] ;
           }
        firstline = false ;
        }
    }

char * txt ( wxString item )
    {
    return (char*) _text[item.MakeUpper()].c_str() ;
    }



// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

MyApp *theapp ;

MyApp *myapp ()
   {
   return theapp ;
   }

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
    wxString s1 , s2 ;
    wxFileName::SplitPath ( argv[0] , &homedir , &s1 , &s2 ) ;
    
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
        wxShowTip(frame, tipProvider);
//        showTip = tipProvider->ShowTipsOnStartup() ;
        tip = tipProvider->GetCurrentTip() ;
//        frame->LS->setOption ( "SHOWTIP" , showTip ) ;
        frame->LS->setOption ( "NEXTTIP" , tip ) ;
        delete tipProvider;
        }

    return TRUE;
}

int MyApp::OnExit ()
    {
    delete m_checker;
    return 0;
    }
    
wxString MyApp::getHTMLCommand ( wxString command )
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
    q.Replace ( "-nohome" , "" ) ;
    if ( 0 == q.Replace ( wxString("%1") , wxString((char*)command.c_str()) ) )
        q += " \"" + command + "\"" ;
    return q ;
#else
    return "" ;
#endif
    }
    
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
    
// ****************************

ChildBase::ChildBase ()
    {
    def = "" ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }
    
ChildBase::ChildBase(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
        : MyChildBase((MyFrameType*)parent, -1, pos, size, style)
    {
    def = "" ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }
        
ChildBase::ChildBase(wxWindow *parent, const wxString& title)
        : MyChildBase((MyFrameType*)parent, -1)
    {
    def = "" ;
    vec = NULL ;
    cSequence = NULL ;
    toolbar = NULL ;
    menubar = NULL ;
    }


wxToolBar *ChildBase::CreateToolBar ( int i )
{
#ifdef __WXMSW__
  return ChildBase::CreateToolBar ( i , -1 , "" ) ;
#else
  wxToolBar *bar = new wxToolBar ( myapp()->frame , -1 ) ;
  //  SetToolBar ( bar ) ;
  return bar ;
  //  return myapp()->frame->GetToolBar () ;
#endif
}

void ChildBase::Maximize ( bool isit )
{
//  MyChildBase::Maximize ( isit ) ;
  Activate () ;
//  SetSize ( GetParent()->GetClientSize() ) ;
//  showName () ;
}

void ChildBase::showName ( wxString x )
    {
    if ( x.IsEmpty() )
       {
       x = getName() ;
       if ( vec && vec->isChanged() ) x += "*" ;
       }
    if ( myapp()->frame->GetTitle() != x )
        myapp()->frame->SetTitle ( x ) ;
    }
    
void ChildBase::OnFocus(wxFocusEvent& event)
    {
    myass ( myapp() , "Oh no! No application defined!" ) ;
    myass ( myapp()->frame , "Oh no! No frame defined!" ) ;
    if ( myapp()->frame->dying ) return ;
    
    Activate () ;
    showName ( ) ;
    myapp()->frame->mainTree->SelectItem ( inMainTree ) ;
    SetFocus () ;
    if ( cSequence ) cSequence->SetFocus() ;
    }

bool ChildBase::caniclose(wxCloseEvent& event)
    {
    if ( /*event.CanVeto() && */vec && vec->isChanged() )
       {
       int r = wxMessageBox ( wxString::Format ( txt("t_close_anyway") , getName().c_str() ) , 
                            txt("msg_box"), wxICON_QUESTION | wxYES | wxNO | wxCANCEL ) ;
       
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

void ChildBase::updateUndoMenu () 
    {
    }
    
wxToolBar *ChildBase::CreateToolBar ( int i , int j , wxString s )
    {
    toolbar = new wxToolBar ( this , -1 ) ;
    return toolbar ;
    }
    
void ChildBase::SetMenuBar ( wxMenuBar *menu_bar )
    {
    menubar = menu_bar ;
    }
    
wxMenuBar *ChildBase::GetMenuBar ()
    {
    return menubar ;
    }
    
void ChildBase::Activate ()
    {
    myapp()->frame->setActiveChild ( this ) ;
    showName () ;
    SetFocus();
    if ( cSequence ) cSequence->SetFocus() ;
    }
    
wxToolBar *ChildBase::GetToolBar ()
    {
    return toolbar ;
    }
    
void ChildBase::SetIcon ( wxIcon icon )
    {
    }
    
void ChildBase::OnClose(wxCloseEvent& event)
{
    if ( !caniclose ( event ) )
        {
        event.Veto() ;
        return ;
        }
    
    myass ( this , "ChildBase::OnClose" ) ;
    myapp()->frame->mainTree->removeChild ( this ) ;
    myapp()->frame->SetTitle ( txt("gentle") ) ;
    SetTitle ( txt("gentle") ) ;
    event.Skip();
    myapp()->frame->removeChild ( this ) ;
}

