/** \file
	\brief Contains classes MyFrame, TProject, MyFrameDropTarget, and TTestSuite (only when compiled with MYTEST)
*/
#ifndef _MYFRAME_H_
#define _MYFRAME_H_

#include "main.h"
#include <wx/utils.h>
#include <wx/dnd.h>
#include <wx/docview.h>
#include <wx/html/htmprint.h>
#include <wx/minifram.h>

class ChildBase ;
class TStorage ;
class TMainTree ;
class TAminoAcids ;
class TABIviewer ;
class TClone ;
class TGenBank ;
class TXMLfile ;
class TCalculator ;
class TPDB ;
class TEnzymeRules ;
class TVirtualGel ;
class TGraph ;
class TPhyloTree ;

typedef wxFrame MyFrameType ;

#define FILE_MENU_SAVE 1
#define FILE_MENU_EXPORT 2
#define FILE_MENU_PRINT 4
#define FILE_MENU_PRINT_RESTRICTIONS 8


#ifdef MYTEST
/** \class TTestSuite
	\brief Test suite class
*/
class TTestSuite
	{
	public :
	TTestSuite () ; ///< \brief Constructor
	virtual void Step () ; ///< \brief Performs a (random) step

	private :
	virtual void pressKey ( ChildBase *ac ) ; ///< \brief Simulates a random key pressed
	virtual void action ( ChildBase *ac ) ; ///< \brief Simulates a random action
	virtual void editMode ( ChildBase *ac ) ; ///< \brief Toggles edit mode

	virtual void vectorPressKey ( ChildBase *ac ) ; ///< \brief Simulates a random key pressed
	virtual void vectorAction ( ChildBase *ac ) ; ///< \brief Simulates a random action

	virtual void mouseEvent ( ChildBase *ac ) ; ///< \brief Simulates a mouse event
	
	virtual void aaAction ( ChildBase *ac ) ; ///< \brief Simulates a random action
    int start ; ///< \brief Internal start value
    int cnt ; ///< \brief Internal counter
	} ;    
#endif

/** \class TProject
	\brief Project class
*/
class TProject
	{
	public :
 	TProject () ; ///< Constructor
	virtual ~TProject () {} ; ///< Dummy destructor

    virtual TEnzymeRules *getEnzymeRules () ; ///< Returns the restriction enzyme display rules for the project
    virtual void setEnzymeRules ( TEnzymeRules *er ) ; ///< Sets the restriction enzyme display rules for the project

    wxString name ;  ///< \brief The project name
    wxString desc ;  ///< \brief The project description
    wxString db ;    ///< \brief The name of the database the project is stored in
    
    private :
	TEnzymeRules *enzyme_rules ; ///< \brief The enzyme rules of the project
	} ;    

/** \class MyFrame
	\brief Frame class
	
	This class contains the actually displayed frame, as well as several
	often-used functions.
*/
class MyFrame : public MyFrameType
{
public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);
    virtual ~MyFrame () ;

    virtual void initme () ;
    virtual void rememberLastProject () ;
    virtual wxString check4update () ;
    virtual void update2version ( wxString ver ) ;
    
    virtual wxMenu *getFileMenu ( int options = 0 ) ;
    virtual wxMenu *getToolMenu ( bool _pcr = false ) ;
    virtual wxMenu *getHelpMenu () ;
    
    virtual void InitToolBar(wxToolBar* toolBar);

    virtual void OnSize(wxSizeEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnHelp(wxCommandEvent& event);
    virtual void OnProgramOptions(wxCommandEvent& event);
    virtual void OnProjectSave(wxCommandEvent& event);
    virtual void OnProjectLoad(wxCommandEvent& event);
    virtual void OnProjectClose(wxCommandEvent& event);
    virtual void OnImageViewer(wxCommandEvent& event);
    virtual void OnExternalInterface(wxCommandEvent& event);
    virtual void OnCalculator(wxCommandEvent& event);
    virtual void OnGraph(wxCommandEvent& event);
    
    virtual void RerouteMenu(wxCommandEvent& event);
    virtual void BollocksMenu(wxCommandEvent& event);
    virtual void TestMenu(wxCommandEvent& event);
    virtual void OnMDIClose(wxCommandEvent& event);

    virtual void OnEnzymeEditor(wxCommandEvent& event);
    virtual void OnAlignment(wxCommandEvent& event);
    virtual void OnFileOpen(wxCommandEvent& event);
    virtual void OnFileImport(wxCommandEvent& event ) ;
    virtual void OnManageDatabase(wxCommandEvent& event ) ;
    virtual void OnTextImport(wxCommandEvent& event ) ;
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnSashDrag(wxSashEvent& event);
    
    virtual void addTool ( wxToolBar* toolBar , int id ) ;
    virtual void addDefaultTools ( wxToolBar* toolBar ) ;
    virtual void addCCPFTools(wxToolBar* toolBar, bool findbutton = false ) ;
    virtual void setDummyToolbar ( ChildBase *c ) ;
    virtual TCalculator *RunCalculator () ;
    virtual TGraph *RunGraph () ;
    virtual MyChild* newFromVector ( TVector *nv , int type = TYPE_VECTOR ) ;
    virtual TAminoAcids *newAminoAcids ( wxString aa , wxString title = _T("") ) ;
    virtual TAminoAcids *newAminoAcids ( TVector *nv , wxString title = _T("") ) ;
    virtual TABIviewer *newABI ( wxString filename , wxString title ) ;
    virtual MyChild *newCLONE ( TClone &clone ) ;
    virtual void newGB ( TGenBank &gb , wxString title = _T("") ) ;
    virtual void newPDB ( TPDB &pdb , wxString title = _T("") ) ;
    virtual void newXML ( TXMLfile &xml , wxString title = _T("") ) ;
    virtual TPhyloTree *newPhyloTree ( wxString title = _T("") ) ;
    virtual void blast ( wxString seq , wxString prg ) ;
    virtual bool importFile ( wxString file , wxString path , int filter = -1 ) ;
    virtual void setChild ( ChildBase *ch ) ;
    virtual void removeChild ( ChildBase *ch ) ;
    virtual TStorage *getTempDB ( wxString name ) ;
    virtual TAlignment *runAlignment ( wxArrayString &vs , wxArrayChildBase &vc , TVector *nv = NULL ) ;
    virtual ChildBase *GetActiveChild() ;
    virtual void setActiveChild ( ChildBase *c ) ;
    virtual wxWindow *getCommonParent() ;
    virtual int getChildIndex ( ChildBase *c ) ;
    virtual void activateChild ( int a ) ;
    virtual void lockDisplay ( bool lock = true ) ;
    virtual bool isLocked () ;
    virtual bool isActivating () ;
    virtual TVirtualGel *useGel ( wxString type ) ;
    virtual void saveImage ( wxBitmap *bmp , wxString name = _T("") ) ;

    TMainTree *mainTree ; ///< \brief The main tree (for sequences etc.)
    TStorage *LS ; ///< \brief Pointer to the local TStorage database structure
    bool dying , enhancedRefresh , showVectorTitle , showVectorLength , 
            loadLastProject , useMetafile , showSplashScreen , checkUpdate ,
            useCoolCanvas , useInternalHelp , doRegisterStuff , showEnzymePos ;
    wxColour aa_color ;
    int editFeatureMode , showStopCodon ;
    TProject project ; ///< \brief The current project
    TEnzymeRules *global_enzyme_rules ; ///< \brief The global rules for which enzymes to use/display
    wxString lang_string ; ///< \brief The user interface language name
    wxArrayString lastCocktail ; ///< \brief Enzymes of the last restriction cocktail
    wxArrayChildBase children ; ///< \brief List of pointers to all child windows
    wxHtmlEasyPrinting *html_ep ; ///< \brief HTML printer wrapper for TCalculator
    vector <wxBitmap> bitmaps ; ///< \brief The bitmaps in the user interface
    wxToolBar *tb_mychild ; ///< \brief The shared toolbar (?)
    char stopcodon ;
    int nonstandard_translation_table ;
    wxArrayString dna_marker ;

    private :
    wxSashLayoutWindow* m_topWindow; ///< \brief Frame layout, top window (unused)
    wxSashLayoutWindow* m_leftWindow1; ///< \brief Frame layout, left window
    wxSashLayoutWindow* m_leftWindow2; ///< \brief Frame layout, right window
    wxSashLayoutWindow* m_bottomWindow; ///< \brief Frame layout, bottom window (unused)
    wxArrayTStorage dbcache ; ///< \brief Cache of databases for faster access
    ChildBase *lastChild ; ///< \brief Last child shown
    wxMenuBar *menu_bar ; ///< \brief The current menu bar
	wxToolBar *mainToolBar ; ///< The main toolbar
    int locked ; ///< \brief State of display locking
    bool activating ; ///< \brief In child activation?
#ifdef MYTEST
    TTestSuite *test_suite ; ///< \brief The Test suite
#endif
    
    DECLARE_EVENT_TABLE()
};

/** \class MyFrameDropTarget
	\brief The class that handles drag'n'drop events for MyFrame
*/
class MyFrameDropTarget : public wxFileDropTarget
   {
   public :
   virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) ;
   } ;

#endif

