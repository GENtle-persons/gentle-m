#ifndef _MAIN_H_
#define _MAIN_H_

typedef unsigned int uint ;

// Place a comment in front of the following lines when compiling releases

// Turn error logging on/off
//#define MYDEBUG

// Turn logging on/off
//#define MYLOG

// Turn testing (log and error log) on/off
#define MYTEST

#ifdef MYTEST

#ifndef MYLOG
#define MYLOG
#endif

#ifndef MYDEBUG
#define MYDEBUG
#endif

#endif // MYTEST

#include "wx/wxprec.h"

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

#define MYPEN(_a) (wxThePenList->FindOrCreatePen ( _a , 1 , wxSOLID ) )
#define MYBRUSH(_a) (wxTheBrushList->FindOrCreateBrush ( _a , wxSOLID ) )
#define MYFONT(_a,_b,_c,_d) (wxTheFontList->FindOrCreateFont(_a,_b,_c,_d) )

// Don't you *EVER* turn this on again!!!!
//#define TIXML_USE_STL
#include "tinyxml/tinyxml.h"

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

#define wxPD_ALL (wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME)

#define TYPE_VECTOR 0
#define TYPE_FRAGMENT 1
#define TYPE_SEQUENCE 2
#define TYPE_PRIMER 3
#define TYPE_ALIGNMENT 4
#define TYPE_AMINO_ACIDS 5
#define TYPE_MISC 6

#define FEAT_NONE 0
#define FEAT_ALPHA 1
#define FEAT_BETA 2
#define FEAT_MUT 3
#define FEAT_NOLINE 4
#define FEAT_VLINE 5

#define DAM_METHYLATION 1
#define DCM_METHYLATION 2
#define ALL_METHYLATION_ENZYMES ( DAM_METHYLATION + DCM_METHYLATION )

class MyFrame ;

// Define a new application
class MyApp : public wxApp
    {
    public:
    bool OnInit();
    int OnExit () ;
    wxString getHTMLCommand ( wxString command ) ;
    wxString getFileFormatApplication ( wxString type ) ;
    MyFrame *frame;
    wxMimeTypesManager mtm ;
    wxString homedir , bmpdir , slash ;
    wxSingleInstanceChecker *m_checker ;
    int programVersion ;
    int dbWarningIssued ;
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

#ifndef _wxArrayTVector
#define _wxArrayTVector
WX_DEFINE_ARRAY(TVector *, wxArrayTVector);
#endif

#define CLEAR_DELETE(__x) { while ( !__x.IsEmpty() ) { if ( __x[0] ) { delete __x[0] ; } __x.RemoveAt ( 0 ) ; } }
#define FILLSTRING(__x,__c,__l) { __x = wxString ( __c , __l ) ; }


#include "enums.h"
#include "TVector.h"
#include "TUReadSeq.h"
#include "ChildBase.h"
#include "ABItype.h"
#include "MiscDialogs.h"
#include "TStorage.h"
#include "TPrimer.h"
#include "TIMGreader.h"
#include "PlasmidCanvas.h"
#include "SequenceCanvas.h"
#include "MyFrame.h"
#include "MyChild.h"
#include "TRestrictionEditor.h"
#include "TClone.h"
#include "GenBank.h"
#include "TPDB.h"
#include "TXMLfile.h"
#include "TVectorTree.h"
#include "TMainTree.h"
#include "RestrictionEnzymes.h"
#include "TImageDisplay.h"
#include "TVirtualGel.h"
#include "TCalculator.h"
#include "Alignment.h"
#include "AminoAcids.h"
#include "ABIviewer.h"
#include "ExternalInterface.h"
#include "TItemEditDialog.h"
#include "PrimerDesign.h"

#define PI 3.14
#define IUPAC_A 1
#define IUPAC_C 2
#define IUPAC_G 4
#define IUPAC_T 8

#define MINI_DISPLAY_ORIGINAL 0
#define MINI_DISPLAY_REAL 1
#define MINI_DISPLAY_CONDENSED 2

#include "ureadseq.h"


// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

#ifdef MYDEBUG
void myass ( bool b , wxString msg = "" ) ;
#else
#define myass(a,b)
#endif
 
#ifdef MYLOG
void mylog ( wxString function , wxString msg = "" ) ;
#else
#define mylog(a,b)
#endif

void wxStringInsert ( wxString &s , int from , wxString t ) ;
void explode ( wxString sep , wxString s , wxArrayString &r ) ;
wxString implode ( wxString sep , wxArrayString &r ) ;
char* txt ( wxString item ) ;
void init_txt ( wxString lang ) ;
MyApp *myapp () ;
int cmpint(int *first, int *second) ;
int cmpre(TRestrictionEnzyme *first, TRestrictionEnzyme *second) ;

#endif
// _MAIN_H__

