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

typedef wxFrame MyFrameType ;

#ifdef MYTEST
class TTestSuite
	{
	public :
	TTestSuite () ;
	virtual void Step () ;

	private :
	virtual void pressKey ( ChildBase *ac ) ;
	virtual void action ( ChildBase *ac ) ;
	virtual void editMode ( ChildBase *ac ) ;

	virtual void vectorPressKey ( ChildBase *ac ) ;
	virtual void vectorAction ( ChildBase *ac ) ;

	virtual void mouseEvent ( ChildBase *ac ) ;
	
	virtual void aaAction ( ChildBase *ac ) ;
    int start ;
    int cnt ;
	} ;    
#endif

class TProject
	{
	public :
 	TProject () ;
	TEnzymeRules *enzyme_rules ;
    wxString name , desc , db ;
	} ;    

// Define a new frame
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
    
    virtual wxMenu *getFileMenu ( bool _save = false , bool _exp = false , bool _print = false ) ;
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
    
    virtual void addDefaultTools(wxToolBar* toolBar) ;
    virtual TCalculator *RunCalculator () ;
    virtual MyChild* newFromVector ( TVector *nv , int type = TYPE_VECTOR ) ;
    virtual TAminoAcids *newAminoAcids ( wxString aa , wxString title = "" ) ;
    virtual TAminoAcids *newAminoAcids ( TVector *nv , wxString title = "" ) ;
    virtual TABIviewer *newABI ( wxString filename , wxString title ) ;
    virtual MyChild *newCLONE ( TClone &clone ) ;
    virtual void newGB ( TGenBank &gb , wxString title = "" ) ;
    virtual void newPDB ( TPDB &pdb , wxString title = "" ) ;
    virtual void newXML ( TXMLfile &xml , wxString title = "" ) ;
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
    virtual void saveImage ( wxBitmap *bmp , wxString name = "" ) ;

    TMainTree *mainTree ;
    TStorage *LS ;
    bool dying , enhancedRefresh , showVectorTitle , showVectorLength , 
            loadLastProject , useMetafile , showSplashScreen , checkUpdate ,
            useCoolCanvas , useInternalHelp ;
    TProject project ;
    TEnzymeRules *global_enzyme_rules ;
    wxString lang_string ;
    wxArrayString lastCocktail ;
    wxArrayChildBase children ;
    wxHtmlEasyPrinting *html_ep ;
    vector <wxBitmap> bitmaps ;
    wxToolBar *tb_mychild ;

    private :
    wxSashLayoutWindow* m_topWindow;
    wxSashLayoutWindow* m_leftWindow1;
    wxSashLayoutWindow* m_leftWindow2;
    wxSashLayoutWindow* m_bottomWindow;
    wxArrayTStorage dbcache ;
    wxMiniFrame *miniFrame ;
    ChildBase *lastChild ;
    wxMenuBar *menu_bar ;
    int locked ;
    bool activating ;
#ifdef MYTEST
    TTestSuite *test_suite ;
#endif
    
    DECLARE_EVENT_TABLE()
};

class MyFrameDropTarget : public wxFileDropTarget
   {
   public :
   virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) ;
   } ;

#endif

