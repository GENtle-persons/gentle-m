#ifndef _MYFRAME_H_
#define _MYFRAME_H_

#include "main.h"
#include <wx/utils.h>
#include <wx/dnd.h>
#include <wx/docview.h>

class ChildBase ;
class TStorage ;
class TMainTree ;
class TAminoAcids ;
class TABIviewer ;
class TClone ;
class TGenBank ;
class TFasta ;

typedef wxMDIParentFrame MyFrameType ;

// Define a new frame
class MyFrame : public MyFrameType
{
public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);
    virtual ~MyFrame () ;

    virtual void initme () ;
    virtual void rememberLastProject () ;
    virtual void fixMax () ;
    virtual string check4update () ;
    virtual void update2version ( string ver ) ;
    
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
    virtual void OnCalculator(wxCommandEvent& event);

    virtual void OnEnzymeEditor(wxCommandEvent& event);
    virtual void OnAlignment(wxCommandEvent& event);
    virtual void OnFileOpen(wxCommandEvent& event);
    virtual void OnFileImport(wxCommandEvent& event ) ;
    virtual void OnManageDatabase(wxCommandEvent& event ) ;
    virtual void OnTextImport(wxCommandEvent& event ) ;
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnSashDrag(wxSashEvent& event);
    
    virtual MyChild* newFromVector ( TVector *nv , int type = TYPE_VECTOR ) ;
    virtual TAminoAcids *newAminoAcids ( string aa , string title = "" ) ;
    virtual TAminoAcids *newAminoAcids ( TVector *nv , string title = "" ) ;
    virtual TABIviewer *newABI ( string filename , string title ) ;
    virtual MyChild *newCLONE ( TClone &clone ) ;
    virtual MyChild *newGB ( TGenBank &gb ) ;
    virtual MyChild *newFASTA ( TFasta &fasta ) ;
    virtual void blast ( string seq , string prg ) ;
    virtual void importFile ( string file , string path , int filter = -1 ) ;
    virtual void setChild ( ChildBase *ch ) ;
    virtual void removeChild ( ChildBase *ch ) ;


    TMainTree *mainTree ;
    TStorage *LS ;
    bool dying , enhancedRefresh , showVectorTitle , showVectorLength , 
            loadLastProject , useMetafile , showSplashScreen , checkUpdate ,
            useCoolCanvas ;
    string lang_string ;
    string project_name , project_desc , project_db ;
    vector <string> lastCocktail ;
    vector <ChildBase*> children ;

    private :
    wxSashLayoutWindow* m_topWindow;
    wxSashLayoutWindow* m_leftWindow1;
    wxSashLayoutWindow* m_leftWindow2;
    wxSashLayoutWindow* m_bottomWindow;
    
    DECLARE_EVENT_TABLE()
};

class MyFrameDropTarget : public wxFileDropTarget
   {
   public :
   virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) ;
   } ;

#endif

