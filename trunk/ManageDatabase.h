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
#define ACTION_MODE_STARTUP 8

class TMyDropTarget : public wxTextDropTarget
    {
    public :
    TMyDropTarget ( TManageDatabaseDialog *_d , wxListCtrl *_me ) ;
    virtual bool OnDropText(int x, int y, const wxString &data) ;
    TManageDatabaseDialog *d ;
    wxListCtrl *me ;
    } ;

class TMySQLDialog : public wxDialog
    {
    public :
    TMySQLDialog ( wxWindow *parent , wxString title , wxString server = "" ,
        wxString db = "" , wxString user = "root" , wxString password = "" ) ;

    wxTextCtrl *s , *d , *u , *p ;
    } ;
    
class TManageDatabaseDialog : public wxDialog
    {
    public :
    TManageDatabaseDialog ( wxWindow *parent , char *title ,  
                        int mode = ACTION_MODE_MANAGE , TVector *_v = NULL ) ;
    virtual ~TManageDatabaseDialog () ;

//    private :
    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;

    virtual void pdOnNew ( wxCommandEvent &ev ) ;
    virtual void pdOnAdd ( wxCommandEvent &ev ) ;
    virtual void pdOnDel ( wxCommandEvent &ev ) ;
    virtual void pdOnDBchange ( wxCommandEvent &ev ) ;
    virtual void pdOnSetDefault ( wxCommandEvent &ev ) ;
    virtual void pmAddSQL ( wxCommandEvent &ev ) ;
    virtual void pmNewSQL ( wxCommandEvent &ev ) ;
    
    virtual void pmOnListEvent ( wxListEvent &ev , wxListCtrl *source , wxListCtrl *target ) ;
    virtual void pmOnLeftListEvent ( wxListEvent &ev ) ;
    virtual void pmOnRightListEvent ( wxListEvent &ev ) ;
    virtual void pmOnLeftChoice ( wxCommandEvent &ev ) ;
    virtual void pmOnRightChoice ( wxCommandEvent &ev ) ;
    virtual void pmOnSave ( wxCommandEvent &ev ) ;
    virtual void pmOnSelectLeft ( wxListEvent &ev ) ;
    virtual void pmOnSelectRight ( wxListEvent &ev ) ;
    virtual void pmOnSelect ( wxListEvent &ev , wxListCtrl *side ) ;
    virtual void pmOnActivateLeft ( wxListEvent &ev ) ;
    virtual void pmOnActivateRight ( wxListEvent &ev ) ;
    virtual void pmOnRightClickLeft ( wxListEvent &ev ) ;
    virtual void pmOnRightClickRight ( wxListEvent &ev ) ;
    virtual void pmOnRightClick ( wxListEvent &ev , wxListCtrl *lc , wxChoice *cc ) ;
    virtual void pmOnActivate ( wxListEvent &ev , wxListCtrl *side ) ;
    virtual void pmOnOpen ( wxCommandEvent &ev ) ;
    virtual void pmOnDelete ( wxCommandEvent &ev ) ;
    virtual void pmOnRename ( wxCommandEvent &ev ) ;
    virtual void pmOnFilter ( wxCommandEvent &ev ) ;
    virtual void pmOnFilterDNA ( wxCommandEvent &ev ) ;
    virtual void pmOnFilterProtein ( wxCommandEvent &ev ) ;
    virtual void pmOnFilterPrimer ( wxCommandEvent &ev ) ;
    virtual void pmOnFilterDesc ( wxCommandEvent &ev ) ;
    virtual void pmOnFilterSeq ( wxCommandEvent &ev ) ;
    virtual void pmOnTwoPanes ( wxCommandEvent &ev ) ;
    
    virtual bool do_copy ( string name , string sdb , string tdb ) ;
    virtual bool do_move ( string name , string sdb , string tdb ) ;
    virtual void do_del  ( string name , string db ) ;
    virtual void pmOpenFiles ( vector <string> &_names , string _db ) ;
    
    virtual bool copyDNA ( string name , string sdb , string tdb ) ;
    virtual bool moveDNA ( string name , string sdb , string tdb ) ;
    virtual void delDNA ( string name , string db ) ;

    virtual void delProject ( string name , string db ) ;
    
    virtual bool do_load ( string name , string db ) ;
    virtual bool do_load_DNA ( string name , string db ) ;
    virtual bool do_load_project ( string name , string db ) ;
    virtual void do_save_DNA () ;
    virtual void do_save_project () ;
    
    virtual void initDatabases () ;
    virtual void accessDB () ;
    virtual void pd_loadList () ;
    virtual void pm_init_lists () ;
    virtual void pm_list_items ( int x ) ;
    virtual bool doesNameExist ( string name , string dbname ) ;
    virtual void addDatabase ( string fn ) ;
    virtual string fixQuotes ( string s ) ;
    virtual string getFileName ( string dbname ) ;
    virtual vector <int> getSelectedListItems ( wxListCtrl *l ) ;
    virtual TStorage *getTempDB ( string filename ) ;
    
    virtual void initCopynMove () ;
    virtual void pmGetContext ( wxListCtrl *side ) ;
    virtual void updateTwoLists() ;

    vector <string> db_name , db_file ;
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
    wxCheckBox *f_dna , *f_prot , *f_primer , *f_twopanes ;
    wxCheckBox *f_desc , *f_seq ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif

