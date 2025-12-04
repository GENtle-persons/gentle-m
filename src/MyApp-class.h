#ifndef _MYAPP_CLASS_H_
#define _MYAPP_CLASS_H_

class MyFrame ;

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/app.h>
#include "mywx.h"

#define GENTLE_VERSION_MAJOR 1
#define GENTLE_VERSION_MINOR 9
#define GENTLE_VERSION_SUB 4


/** \class MyApp
    \brief Application class
*/
class MyApp : public wxApp
    {
    public:
    virtual bool OnInit();
    virtual int OnExit () ;
    virtual void init_txt ( const wxString& lang , const wxString& csv , wxHashString *target = NULL , int ln = 1 ) ;
    virtual void do_my_ass ( const bool b , const wxString& msg = _T("") ) ;
    virtual void do_my_log ( const wxString& function , const wxString& msg = _T("") ) ;
    virtual wxString getHTMLCommand ( const wxString& command ) ; ///< Returns the command line for running a browser
    virtual wxString getFileFormatApplication ( const wxString& type ) ; ///< Returns application associated with a file type
    virtual wxString getFileFormatCommand ( const wxString& type , const wxString& file ) ; ///< Returns the command line for running this file
    virtual wxString get_GENtle_version () const ; ///< Returns the GENtle version string
    virtual wxString getLocalDBname () ; ///< Returns the filename of the default local database
    virtual void launchBrowser ( const wxString& url ) ;

    MyFrame *frame; ///< The application frame
    wxMimeTypesManager mtm ; ///< The MIME types manager
    wxFileName homedir ; ///< Directory of the application
    wxFileName bmpdir ; ///< Directory of the bitmaps
    int programVersion ; ///< The database access program version
    int dbWarningIssued ; ///< Was a database warning issued?
    wxHashString _text ; ///< Contains the current GUI translation.
    wxHashString clp ; ///< Command line parameters
    wxCSConv *isoconv ; ///< UTF-8 conversion helper
    wxStopWatch sw ;

    private :
    virtual void registerFileExtension ( const wxString& extension ) ; ///< Registers a file extension to GENtle (windows only).
    virtual void registerProtocol ( const wxString& extension ) ; ///< Registers a protocol to GENtle (windows only).
    wxFile *errout ; ///< The ERROR.txt file handler for do_my_ass
    wxFile *logout ; ///< The LOG.txt file handler for do_my_log
    int total_log_time ; ///< The log timer for do_my_log
    int total_log_counter ; ///< The log counter for do_my_log
    wxSingleInstanceChecker *m_checker ; ///< Checks if another instance is running
    wxString theRealLocalDb ;
    };

    #endif