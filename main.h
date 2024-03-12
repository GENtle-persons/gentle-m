/** \file
    \brief Contains defines, global functions, includes, and the MyApp class
*/
#ifndef _MAIN_H_
#define _MAIN_H_

//#define __DEBIAN__

#define GENTLE_VERSION_MAJOR 1
#define GENTLE_VERSION_MINOR 9
#define GENTLE_VERSION_SUB 4

typedef unsigned int uint ;

/** \fn myass ( bool b , wxString msg )
    \brief "My assertion" - little inside joke...
    \param b The condition given in the call. No assertion when b is FALSE.
    \param msg The message string to write into errout.
*/

/** \def MYDEBUG
    \brief Turns on output of the myass function; should not be used for releases
*/

/** \def MYLOG
    \brief Turns on output of logging; should not be used for releases
*/

/** \def MYTEST
    \brief Turns on both MYDEBUG and MYLOG; should not be used for releases
*/

//#define MYDEBUG
//#define MYLOG
//#define MYTEST

#ifdef MYTEST
    #ifndef MYLOG
        #define MYLOG
    #endif // MYLOG
    #ifndef MYDEBUG
        #define MYDEBUG
    #endif // MYDEBUG
#endif // MYTEST

/**    \def MYSPINBOXSIZE
    \brief Width of spin box, for windows only
*/
#ifdef __WXMSW__
    #define MYSPINBOXSIZE 80
#else
    #define MYSPINBOXSIZE -1
#endif

#include "wx/wxprec.h"

/**    \def MYFONTSIZE
    \brief Main tree font size
*/
#ifdef __WXMAC__
    #define MYFONTSIZE 10
#else
    #define MYFONTSIZE 8
#endif

/**    \def MYSPINBOXSIZE
    \brief List box style, for GTK only
*/
#ifdef __WXGTK__
    #define MYLISTBORDER wxSIMPLE_BORDER
#else
    #define MYLISTBORDER 0
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/notebook.h"
#endif

#include <wx/toolbar.h>
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/file.h>
#include <wx/dialup.h>
#include <wx/utils.h>
#include <wx/numdlg.h>
#include <wx/timer.h>
#include <wx/filename.h>

#ifdef _UNICODE
    #define wxUSE_UNICODE 1
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
    #include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/paste.xpm"
    #include "bitmaps/print.xpm"
    #include "bitmaps/help.xpm"
#endif

#include <math.h>
#include <vector>

using namespace std ;

/** \def MYPEN(_a)
    \brief Creates or reuses a solid pen with size 1. _a is of type wxColour
*/

/** \def MYBRUSH(_a)
    \brief Creates or reuses a solid brush. _a is of type wxColour
*/

/** \def MYFONT(_a,_b,_c,_d)
    \brief Creates or reuses a font. Parameters are the same as wxCreateFont
*/

#define MYPEN(_a) (wxThePenList->FindOrCreatePen ( _a , 1 , wxPENSTYLE_SOLID ) )
#define MYBRUSH(_a) (wxTheBrushList->FindOrCreateBrush ( _a , wxBRUSHSTYLE_SOLID ) )
#define MYFONT(_a,_b,_c,_d) (wxTheFontList->FindOrCreateFont(_a,_b,_c,_d) )

// Don't you *EVER* turn this on again!!!!
//#define TIXML_USE_STL

#include <tinyxml.h>

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/snglinst.h>
#include <wx/laywin.h>
#include <wx/progdlg.h>
#include <wx/mimetype.h>
#include <wx/textfile.h>
#include <wx/dir.h>
#include <wx/html/helpctrl.h>
#include <wx/url.h>
#include <wx/protocol/http.h>
WX_DECLARE_STRING_HASH_MAP( wxString, wxHashString );

/** \brief Style for Process Dialog */
#define wxPD_ALL (wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME)

#define TYPE_VECTOR 0 /**< \brief Vector type */
#define TYPE_FRAGMENT 1 /**< \brief Fragment type */
#define TYPE_SEQUENCE 2 /**< \brief Sequence type */
#define TYPE_PRIMER 3 /**< \brief Primer type */
#define TYPE_ALIGNMENT 4 /**< \brief Alignment type */
#define TYPE_AMINO_ACIDS 5 /**< \brief Amino acid type */
#define TYPE_MISC 6 /**< \brief Misc type */

#define FEAT_NONE 0  /**< \brief Do not draw feature  */
#define FEAT_ALPHA 1  /**< \brief Draw feature as alpha helix */
#define FEAT_BETA 2  /**< \brief Draw feature as beta sheet */
#define FEAT_MUT 3  /**< \brief Draw feature as mutation */
#define FEAT_NOLINE 4  /**< \brief Draw feature without line  */
#define FEAT_VLINE 5  /**< \brief Draw feature as vertical line */

#define DAM_METHYLATION 1 /**< \brief Show DAM methylation */
#define DCM_METHYLATION 2 /**< \brief Show DCM methylation */
#define ALL_METHYLATION_ENZYMES ( DAM_METHYLATION + DCM_METHYLATION ) /**< \brief Show all methylations */

class MyFrame ;

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

class TUndo ;
class ChildBase ;
class TVector ;
class TRestrictionEnzyme ;
class TStorage ;
class TProtease ;
class TVectorItem ;
class SeqBasic ;
class TProteaseCut ;

WX_DEFINE_ARRAY(ChildBase *, wxArrayChildBase);
WX_DEFINE_ARRAY(TRestrictionEnzyme *, wxArrayTRestrictionEnzyme);
WX_DEFINE_ARRAY(TStorage *, wxArrayTStorage);
WX_DEFINE_ARRAY(TProtease *,wxArrayTProtease);
WX_DEFINE_ARRAY(TVectorItem *,wxArrayTVectorItem);
WX_DEFINE_ARRAY(SeqBasic *,wxArraySeqBasic);
WX_DEFINE_ARRAY(TProteaseCut *,wxArrayTProteaseCut);
WX_DECLARE_OBJARRAY(float, wxArrayFloat);

#ifndef _wxArrayTVector
    #define _wxArrayTVector
    WX_DEFINE_ARRAY(TVector *, wxArrayTVector);
#endif

/** \brief Clear wxArray of pointers and delete the not-NULL-pointer objects */
#define CLEAR_DELETE(__x) { while ( !__x.IsEmpty() ) { if ( __x[0] ) { delete __x[0] ; __x[0] = NULL ; } __x.RemoveAt ( 0 ) ; } }

/** \brief Create a wxString of __l times the __c char */
#define FILLSTRING(__x,__c,__l) { __x = wxString ( __c , __l ) ; }


#include "enums.h"
#include "TVector.h"
#include "TUReadSeq.h"
#include "TPhylip.h"
#include "ipc/ipc.h"
#include "ncoils.h"
#include "CGview.h"
#include "AutoAnnotate.h"
#include "ChildBase.h"
#include "OnlineTools.h"
#include "TSequencerData.h"
#include "ABItype.h"
#include "SCFtype.h"
#include "CloningAssistant.h"
#include "MiscDialogs.h"
#include "TStorage.h"
#include "TPrimer.h"
#include "TIMGreader.h"
#include "PlasmidCanvas.h"
#include "SequenceCanvas.h"
#include "TDotPlot.h"
#include "TStoreAllDialog.h"
#include "TSequencingAssistantDialog.h"
#include "MyFrame.h"
#include "MyChild.h"
#include "TRestrictionEditor.h"
#include "TClone.h"
#include "GenBank.h"
#include "TPDB.h"
#include "CloneManager.h"
#include "TXMLfile.h"
#include "TVectorTree.h"
#include "TMainTree.h"
#include "RestrictionEnzymes.h"
#include "TImageDisplay.h"
#include "TVirtualGel.h"
//#include "TRestrictionIdentifier.h"
#include "TCalculator.h"
#include "TGraph.h"
#include "TPhyloTree.h"
#include "Alignment.h"
#include "AminoAcids.h"
#include "ABIviewer.h"
#include "ExternalInterface.h"
#include "TItemEditDialog.h"
#include "PrimerDesign.h"

#define PI 3.14 /**< \brief Just PI */
#define IUPAC_A 1 /**< \brief IUPAC bit for A */
#define IUPAC_C 2 /**< \brief IUPAC bit for C */
#define IUPAC_G 4 /**< \brief IUPAC bit for G */
#define IUPAC_T 8 /**< \brief IUPAC bit for T */

#define MINI_DISPLAY_ORIGINAL 0 /**< \brief Display mini-canvas at original width */
#define MINI_DISPLAY_REAL 1 /**< \brief Display mini-canvas at real width */
#define MINI_DISPLAY_CONDENSED 2 /**< \brief Display mini-canvas as condensed width */

#include "ureadseq.h"


// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

#ifdef MYDEBUG
    #define myass(a,b) myapp()->do_my_ass(a,b)
#else
    #define myass(a,b)
#endif

#ifdef MYLOG
    #define mylog(a,b) myapp()->do_my_log(a,b)
#else
    #define mylog(a,b)
#endif

#define CHAR2WX(__c) wxString((char*)l,wxConvUTF8)

/** \brief Chop wxString "s" into parts "r" separated by "sep" */
void explode ( const wxString& sep , const wxString& s , wxArrayString &r ) ;

/** \brief Join wxStrings in "r" while putting "sep"s between them */
wxString implode ( const wxString& sep , wxArrayString &r ) ;
/*
//  \brief Returns the current language version of the "item" /
const char * txt ( const wxString item ) ;

//  \brief Returns the current language version of the "item" /
const char* txt ( const char * const item ) ;
*/
/** \brief Returns the current language version of the "item" */
const wxString txt ( wxString item ) ;

/** \brief Returns the current language version of the "item" */
const wxString txt ( const char * const item ) ;

/** \brief Returns a pointer to the application */
MyApp *myapp () ;

/** \brief Compares the values of two int pointers */
int cmpint(/*const*/ int * const first, /*const*/ int * const second) ; // no const to match SCMPFUNC

/** \brief Compares the values of two TRestrictionEnzyme pointers */
int cmpre(const TRestrictionEnzyme * const first, const TRestrictionEnzyme * const second) ;

#endif
// _MAIN_H__
