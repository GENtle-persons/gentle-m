#ifndef _MYFRAME_H_
#define _MYFRAME_H_

#include "main.h"
#include <wx/utils.h>
#include <wx/dnd.h>

class ChildBase ;
class TStorage ;
class TMainTree ;
class TAminoAcids ;
class TABIviewer ;
class TClone ;
class TGenBank ;
class TFasta ;

// Define a new frame
class MyFrame : public wxMDIParentFrame
{
public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);
    ~MyFrame () ;

    void initme () ;
    void rememberLastProject () ;
    void fixMax () ;
    string check4update () ;
    void update2version ( string ver ) ;
    
    wxMenu *getFileMenu ( bool _save = false , bool _exp = false , bool _print = false ) ;
    wxMenu *getToolMenu ( bool _pcr = false ) ;
    wxMenu *getHelpMenu () ;
    
    void InitToolBar(wxToolBar* toolBar);

    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnProgramOptions(wxCommandEvent& event);
    void OnProjectSave(wxCommandEvent& event);
    void OnProjectLoad(wxCommandEvent& event);
    void OnProjectClose(wxCommandEvent& event);
    void OnImageViewer(wxCommandEvent& event);
    void OnCalculator(wxCommandEvent& event);

    void OnEnzymeEditor(wxCommandEvent& event);
    void OnAlignment(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnFileImport(wxCommandEvent& event ) ;
    void OnManageDatabase(wxCommandEvent& event ) ;
    void OnTextImport(wxCommandEvent& event ) ;
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    
    MyChild* newFromVector ( TVector *nv , int type = TYPE_VECTOR ) ;
    TAminoAcids *newAminoAcids ( string aa , string title = "" ) ;
    TAminoAcids *newAminoAcids ( TVector *nv , string title = "" ) ;
    TABIviewer *newABI ( string filename , string title ) ;
    MyChild *newCLONE ( TClone &clone ) ;
    MyChild *newGB ( TGenBank &gb ) ;
    MyChild *newFASTA ( TFasta &fasta ) ;
    void blast ( string seq , string prg ) ;
    void importFile ( string file , string path , int filter = -1 ) ;

    TMainTree *mainTree ;
    MyApp *app ;
    TStorage *LS ;
    vector <ChildBase*> children ;
    bool dying , enhancedRefresh , showVectorTitle , showVectorLength , 
            loadLastProject , useMetafile , showSplashScreen , checkUpdate ,
            useCoolCanvas ;
    string lang_string ;
    string project_name , project_desc , project_db ;
    vector <string> lastCocktail ;

    DECLARE_EVENT_TABLE()
};

class MyFrameDropTarget : public wxFileDropTarget
   {
   public :
   virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) ;
   MyFrame *f ;
   } ;

#endif

