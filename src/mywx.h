#ifndef _MYWX_H_
#define _MYWX_H_


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

/** \brief Style for Process Dialog */
#define wxPD_ALL (wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME)

#include "ChildBase.h"
WX_DEFINE_ARRAY(ChildBase *, wxArrayChildBase);
//#include "RestrictionEnzymes.h"
class TRestrictionEnzyme ;
WX_DEFINE_ARRAY(TRestrictionEnzyme *, wxArrayTRestrictionEnzyme);
// Forward declaration of TStorage to avoid circular dependency
class TStorage;
WX_DEFINE_ARRAY(TStorage *, wxArrayTStorage);
//include "TProteolysis.h"
class TProtease;
WX_DEFINE_ARRAY(TProtease *,wxArrayTProtease);
//#include "TVector.h"
class TVectorItem ;
WX_DEFINE_ARRAY(TVectorItem *,wxArrayTVectorItem);
class SeqBasic ;
WX_DEFINE_ARRAY(SeqBasic *,wxArraySeqBasic);
class TProteaseCut;
WX_DEFINE_ARRAY(TProteaseCut *,wxArrayTProteaseCut);

#ifndef _wxArrayTVector
    #define _wxArrayTVector
    WX_DEFINE_ARRAY(TVector *, wxArrayTVector);
#endif

/** \brief Clear wxArray of pointers and delete the not-NULL-pointer objects */
#define CLEAR_DELETE(__x) { while ( !__x.IsEmpty() ) { if ( __x[0] ) { delete __x[0] ; __x[0] = NULL ; } __x.RemoveAt ( 0 ) ; } }

/** \brief Create a wxString of __l times the __c char */
#define FILLSTRING(__x,__c,__l) { __x = wxString ( __c , __l ) ; }

WX_DECLARE_STRING_HASH_MAP( wxString, wxHashString );

/** \def MYPEN(_a)
    \brief Creates or reuses a solid pen with size 1. _a is of type wxColour
*/
#define MYPEN(_a) (wxThePenList->FindOrCreatePen ( _a , 1 , wxPENSTYLE_SOLID ) )

/** \def MYBRUSH(_a)
    \brief Creates or reuses a solid brush. _a is of type wxColour
*/
#define MYBRUSH(_a) (wxTheBrushList->FindOrCreateBrush ( _a , wxBRUSHSTYLE_SOLID ) )


/** \brief Chop wxString "s" into parts "r" separated by "sep" */
void explode ( const wxString& sep , const wxString& s , wxArrayString &r ) ;

/** \brief Join wxStrings in "r" while putting "sep"s between them */
wxString implode ( const wxString& sep , wxArrayString &r ) ;

#define CHAR2WX(__c) wxString((char*)l,wxConvUTF8)

#endif

