/** \file
	\brief Contains the TManageDatabaseDialog class and its helper classes, TMyDropTarget and TMySQLDialog
*/
#ifndef _MANAGE_DATABASE_H_
#define _MANAGE_DATABASE_H_

#include "main.h"
#include <wx/dnd.h>

class TStorage ;
class TManageDatabaseDialog ;

#define GENTLE_DRAG_DATA _T("GENTLE_DRAG_DATA")
#define ACTION_MODE_MANAGE 0
#define ACTION_MODE_SAVE 1
#define ACTION_MODE_LOAD 2
#define ACTION_MODE_PROJECT 4
#define ACTION_MODE_STARTUP 8

/**	\brief Handles drag'n'drop events for items dragged between the two database lists in TManageDatabaseDialog
*/
class TMyDropTarget : public wxTextDropTarget
    {
    public :
    TMyDropTarget ( TManageDatabaseDialog *_d , wxListCtrl *_me ) ; ///< Constructor
    virtual bool OnDropText(int x, int y, const wxString &data) ; ///< Drop event handler
    TManageDatabaseDialog *d ; ///< Pointer to the calling structure
    wxListCtrl *me ; ///< Pointer to the list control in question
    } ;

/** \brief Creates the dialog for entering parameters to add a MySQL database
*/
class TMySQLDialog : public wxDialog
    {
    public :
    TMySQLDialog ( wxWindow *parent , wxString title , wxString server = _T("") ,
        wxString db = _T("") , wxString user = _T("root") , wxString password = _T("") ) ; ///< Constructor

    wxTextCtrl *s , *d , *u , *p ;
    } ;

/** \brief The database interface dialog; opening, storing, searching sequences
*/
class TManageDatabaseDialog : public wxDialog
    {
    public :
    TManageDatabaseDialog ( wxWindow *parent , wxString title ,
                        int mode = ACTION_MODE_MANAGE , TVector *_v = NULL ) ; ///< Constructor
    virtual ~TManageDatabaseDialog () ; ///< Destructor

    virtual void OnOK ( wxCommandEvent &ev ) ; ///< "OK button" event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< "Cancel button" event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    virtual void pdOnNew ( wxCommandEvent &ev ) ; ///< "New sqlite database button" event handler
    virtual void pdOnAdd ( wxCommandEvent &ev ) ; ///< "Add sqlite database button" event handler
    virtual void pdOnDel ( wxCommandEvent &ev ) ; ///< "Delete database button" event handler
    virtual void pdOnDBchange ( wxCommandEvent &ev ) ; ///< "Change database" event handler
    virtual void pdOnSetDefault ( wxCommandEvent &ev ) ; ///< "Set as default database button" event handler
    virtual void pmAddSQL ( wxCommandEvent &ev ) ; ///< "Add MySQL database button" event handler
    virtual void pmNewSQL ( wxCommandEvent &ev ) ; ///< "New MySQL database button" event handler
    
    virtual void pmOnListEvent ( wxListEvent &ev , wxListCtrl *source , wxListCtrl *target ) ; ///< "Drag/drop" event handler
    virtual void pmOnLeftListEvent ( wxListEvent &ev ) ; ///< "Event in left dropdown list" event handler
    virtual void pmOnRightListEvent ( wxListEvent &ev ) ; ///< "Event in right dropdown list" event handler
    virtual void pmOnLeftChoice ( wxCommandEvent &ev ) ; ///< "Selection in left dropdown list" event handler
    virtual void pmOnRightChoice ( wxCommandEvent &ev ) ; ///< "Selection in right dropdown list" event handler
    virtual void pmOnSave ( wxCommandEvent &ev ) ; ///< "Save button" event handler
    virtual void pmOnSelectLeft ( wxListEvent &ev ) ; ///< "Selection in left list" event handler
    virtual void pmOnSelectRight ( wxListEvent &ev ) ; ///< "Selection in right list" event handler
    virtual void pmOnSelect ( wxListEvent &ev , wxListCtrl *side ) ; ///< Selection in a list - universal "event handler"
    virtual void pmOnActivateLeft ( wxListEvent &ev ) ; ///< "Left list activated" event handler
    virtual void pmOnActivateRight ( wxListEvent &ev ) ; ///< "Right list activated" event handler
    virtual void pmOnRightClickLeft ( wxListEvent &ev ) ; ///< "Context menu for left list" event handler
    virtual void pmOnRightClickRight ( wxListEvent &ev ) ; ///< "Context menu for right list" event handler
    virtual void pmOnRightClick ( wxListEvent &ev , wxListCtrl *lc , wxChoice *cc ) ; ///< "Right click" universal event handler
    
    virtual void pmOnActivate ( wxListEvent &ev , wxListCtrl *side ) ; ///< activation event handler
    virtual void pmOnOpen ( wxCommandEvent &ev ) ; ///< "Open item" event handler
    virtual void pmOnDelete ( wxCommandEvent &ev ) ; ///< "Delete item" event handler
    virtual void pmOnRename ( wxCommandEvent &ev ) ; ///< "Rename item" event handler
    virtual void pmOnFilter ( wxCommandEvent &ev ) ; ///< "Filter has changed" event handler
    virtual void pmOnFilterDNA ( wxCommandEvent &ev ) ; ///< "Show DNA checkbox" event handler
    virtual void pmOnFilterProtein ( wxCommandEvent &ev ) ; ///< "Show protein checkbox" event handler
    virtual void pmOnFilterPrimer ( wxCommandEvent &ev ) ; ///< "Show primer checkbox" event handler
    virtual void pmOnFilterAlignments ( wxCommandEvent &ev ) ; ///< "Show alignments checkbox" event handler
    virtual void pmOnFilterDesc ( wxCommandEvent &ev ) ; ///< "Search description checkbox" event handler
    virtual void pmOnFilterSeq ( wxCommandEvent &ev ) ; ///< "Search sequence checkbox" event handler
    virtual void pmOnTwoPanes ( wxCommandEvent &ev ) ; ///< "Toggle one/two lists" event handler
    
    virtual bool do_copy ( wxString name , wxString sdb , wxString tdb ) ; ///< Copy item "name" from source to target database
    virtual bool do_move ( wxString name , wxString sdb , wxString tdb ) ; ///< Move item "name" from source to target database
    virtual void do_del  ( wxString name , wxString db ) ; ///< Delete item "name" from the database
    virtual void pmOpenFiles ( wxArrayString &_names , wxString _db ) ; ///< Open the selected files
    
    virtual bool copyDNA ( wxString name , wxString sdb , wxString tdb ) ; ///< Copy item "name" from source to target database
    virtual bool moveDNA ( wxString name , wxString sdb , wxString tdb ) ; ///< Move item "name" from source to target database
    virtual void delDNA ( wxString name , wxString db ) ; ///< Delete item "name" from the database

    virtual void delProject ( wxString name , wxString db ) ; ///< Remove a project from the database
    
    virtual bool do_load ( wxString name , wxString db ) ; ///< Load an entry from the database
    virtual bool do_load_DNA ( wxString name , wxString db ) ; ///< Load a sequence from the database
    virtual bool do_load_project ( wxString name , wxString db ) ; ///< Load a project from the database
    virtual void do_save_DNA () ; ///< Save a sequence to the database
    virtual void do_save_project () ; ///< Save a project to the database
    
    virtual void initDatabases () ; ///< Initialize internal database list
    virtual void accessDB () ; ///< Access the database
    virtual void pd_loadList () ; ///< Loads the database
    virtual void pm_init_lists () ; ///< Initializes the lists
    virtual void pm_list_items ( int x ) ; ///< Initializes list items
    virtual bool doesNameExist ( wxString name , wxString dbname ) ; ///< Does an item "name" exist in that database?
    virtual void addDatabase ( wxString fn ) ; ///< Add a database to the list
    virtual wxString fixQuotes ( wxString s ) ; ///< Fix quotes for storing in the database
    virtual wxString getFileName ( wxString dbname ) ; ///< Returns the filename of the database
    virtual wxArrayInt getSelectedListItems ( wxListCtrl *l ) ; ///< Returns the numbers of the selected items in the given list
    virtual TStorage *getTempDB ( wxString filename ) ; ///< Returns a pointer to termorarily accessed database
    
    virtual void initCopynMove () ; ///< Prepare the open/store/search tab
    virtual void pmGetContext ( wxListCtrl *side ) ; ///< ???
    virtual void updateTwoLists() ; ///< Update both database lists

    wxArrayString db_name , db_file ;
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
    wxString defdb , context_db ;
    wxArrayString context_names ;
    int actionMode ;
    TVector *v ;
    bool justload , silent_overwrite ;
    
    private:
    bool doSave , doLoad , isProject , startup ;
    wxString filter ;
    wxTextCtrl *filter_txt ;
    wxCheckBox *f_dna , *f_prot , *f_primer , *f_align , *f_twopanes ;
    wxCheckBox *f_desc , *f_seq ;
	wxBoxSizer *v0 , *v2 ; // Sizers needed for second (right) list
    
    DECLARE_EVENT_TABLE()
    } ;

#endif

