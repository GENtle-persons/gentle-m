#ifndef _MANAGE_DATABASE_H_
#define _MANAGE_DATABASE_H_

#include "main.h"
#include <wx/dnd.h>

class TStorage ;
class TManageDatabaseDialog ;

#define GENTLE_DRAG_DATA "GENTLE_DRAG_DATA"
#define ACTION_MODE_MANAGE 0
#define ACTION_MODE_SAVE 1
#define ACTION_MODE_LOAD 2
#define ACTION_MODE_PROJECT 4

class TMyDropTarget : public wxTextDropTarget
    {
    public :
    TMyDropTarget ( TManageDatabaseDialog *_d , wxListCtrl *_me ) ;
    virtual bool OnDropText(int x, int y, const wxString &data) ;
    TManageDatabaseDialog *d ;
    wxListCtrl *me ;
    } ;

class TManageDatabaseDialog : public wxDialog
    {
    public :
    TManageDatabaseDialog ( wxWindow *parent , char *title ,  
                        int mode = ACTION_MODE_MANAGE , TVector *_v = NULL ) ;
    virtual ~TManageDatabaseDialog () ;

//    private :
    void OnOK ( wxCommandEvent &ev ) ;
    void OnCancel ( wxCommandEvent &ev ) ;
    void OnCharHook(wxKeyEvent& event) ;

    void pdOnNew ( wxCommandEvent &ev ) ;
    void pdOnAdd ( wxCommandEvent &ev ) ;
    void pdOnDel ( wxCommandEvent &ev ) ;
    void pdOnDBchange ( wxCommandEvent &ev ) ;
    void pdOnSetDefault ( wxCommandEvent &ev ) ;
    
    void pmOnListEvent ( wxListEvent &ev , wxListCtrl *source , wxListCtrl *target ) ;
    void pmOnLeftListEvent ( wxListEvent &ev ) ;
    void pmOnRightListEvent ( wxListEvent &ev ) ;
    void pmOnLeftChoice ( wxCommandEvent &ev ) ;
    void pmOnRightChoice ( wxCommandEvent &ev ) ;
    void pmOnSave ( wxCommandEvent &ev ) ;
    void pmOnSelectLeft ( wxListEvent &ev ) ;
    void pmOnSelectRight ( wxListEvent &ev ) ;
    void pmOnSelect ( wxListEvent &ev , wxListCtrl *side ) ;
    void pmOnActivateLeft ( wxListEvent &ev ) ;
    void pmOnActivateRight ( wxListEvent &ev ) ;
    void pmOnRightClickLeft ( wxListEvent &ev ) ;
    void pmOnRightClickRight ( wxListEvent &ev ) ;
    void pmOnRightClick ( wxListEvent &ev , wxListCtrl *lc , wxChoice *cc ) ;
    void pmOnActivate ( wxListEvent &ev , wxListCtrl *side ) ;
    void pmOnOpen ( wxCommandEvent &ev ) ;
    void pmOnDelete ( wxCommandEvent &ev ) ;
    void pmOnRename ( wxCommandEvent &ev ) ;
    void pmOnFilter ( wxCommandEvent &ev ) ;
    void pmOnFilterDNA ( wxCommandEvent &ev ) ;
    void pmOnFilterProtein ( wxCommandEvent &ev ) ;
    void pmOnFilterPrimer ( wxCommandEvent &ev ) ;
    
    bool do_copy ( string name , string sdb , string tdb ) ;
    bool do_move ( string name , string sdb , string tdb ) ;
    void do_del  ( string name , string db ) ;
    void pmOpenFiles ( vector <string> &_names , string _db ) ;
    
    bool copyDNA ( string name , string sdb , string tdb ) ;
    bool moveDNA ( string name , string sdb , string tdb ) ;
    void delDNA ( string name , string db ) ;

    void delProject ( string name , string db ) ;
    
    bool do_load ( string name , string db ) ;
    bool do_load_DNA ( string name , string db ) ;
    bool do_load_project ( string name , string db ) ;
    void do_save_DNA () ;
    void do_save_project () ;
    
    void initDatabases () ;
    void accessDB () ;
    void pd_loadList () ;
    void pm_init_lists () ;
    void pm_list_items ( int x ) ;
    bool doesNameExist ( string name , string dbname ) ;
    void addDatabase ( string fn ) ;
    string fixQuotes ( string s ) ;
    string getFileName ( string dbname ) ;
    vector <int> getSelectedListItems ( wxListCtrl *l ) ;
    
    void initCopynMove () ;
    void pmGetContext ( wxListCtrl *side ) ;

    vector <string> db_name , db_file ;
    MyFrame *f ;
    TStorage *storage ;
    wxNotebook *nb ;
    wxPanel *pDatabases , *pCopynMove ;
    wxStaticText *pd_db_name , *pd_db_file ;
    wxListBox *pd_db ;
    
    public :
    wxListCtrl *pm_left , *pm_right , *thetarget ;
    wxChoice *pm_dd_l , *pm_dd_r , *pm_dd_save ;
    wxTextCtrl *pm_name ;
    wxImageList *il ;
    int bo , th ;
    string defdb , context_db ;
    vector <string> context_names ;
    int actionMode ;
    TVector *v ;
    
    private:
    bool doSave , doLoad , isProject ;
    string filter ;
    wxTextCtrl *filter_txt ;
    wxCheckBox *f_dna , *f_prot , *f_primer ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif

