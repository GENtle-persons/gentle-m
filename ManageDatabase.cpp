#include "ManageDatabase.h"

#define PM_LEFT 1
#define PM_RIGHT 2

BEGIN_EVENT_TABLE(TManageDatabaseDialog, wxDialog )
    EVT_BUTTON(MD_PM_NEW,TManageDatabaseDialog::pdOnNew)
    EVT_BUTTON(MD_PD_ADD,TManageDatabaseDialog::pdOnAdd)
    EVT_BUTTON(MD_PD_DEL,TManageDatabaseDialog::pdOnDel)
    EVT_BUTTON(MD_PD_DEFAULT,TManageDatabaseDialog::pdOnSetDefault)
    EVT_BUTTON(MD_PM_SAVE,TManageDatabaseDialog::pmOnSave)
    EVT_BUTTON(MD_PD_ADD_MYSQL,TManageDatabaseDialog::pmAddSQL)
    EVT_BUTTON(MD_PM_NEW_MYSQL,TManageDatabaseDialog::pmNewSQL)
    EVT_LISTBOX(MD_PD_DBLIST,TManageDatabaseDialog::pdOnDBchange)
    EVT_LIST_BEGIN_DRAG(MD_PM_LEFT,TManageDatabaseDialog::pmOnLeftListEvent)
    EVT_LIST_BEGIN_DRAG(MD_PM_RIGHT,TManageDatabaseDialog::pmOnRightListEvent)
    EVT_CHOICE(MD_PM_DD_L,TManageDatabaseDialog::pmOnLeftChoice)
    EVT_CHOICE(MD_PM_DD_R,TManageDatabaseDialog::pmOnRightChoice)
    
    EVT_CHECKBOX(MD_PM_FILTER_DNA,TManageDatabaseDialog::pmOnFilterDNA)
    EVT_CHECKBOX(MD_PM_FILTER_PROTEIN,TManageDatabaseDialog::pmOnFilterProtein)
    EVT_CHECKBOX(MD_PM_FILTER_PRIMER,TManageDatabaseDialog::pmOnFilterPrimer)
    EVT_CHECKBOX(MD_PM_TWOPANES,TManageDatabaseDialog::pmOnTwoPanes)
    EVT_CHECKBOX(MD_PM_FILTER_DESC,TManageDatabaseDialog::pmOnFilterDesc)
    EVT_CHECKBOX(MD_PM_FILTER_SEQ,TManageDatabaseDialog::pmOnFilterSeq)
    
    EVT_TEXT(MD_PM_FILTER,TManageDatabaseDialog::pmOnFilter)

    EVT_LIST_ITEM_SELECTED(MD_PM_LEFT,TManageDatabaseDialog::pmOnSelectLeft)
    EVT_LIST_ITEM_SELECTED(MD_PM_RIGHT,TManageDatabaseDialog::pmOnSelectRight)

    EVT_LIST_ITEM_ACTIVATED(MD_PM_LEFT,TManageDatabaseDialog::pmOnActivateLeft)
    EVT_LIST_ITEM_ACTIVATED(MD_PM_RIGHT,TManageDatabaseDialog::pmOnActivateRight)
    EVT_CHAR_HOOK(TManageDatabaseDialog::OnCharHook)
    EVT_LIST_ITEM_RIGHT_CLICK(MD_PM_LEFT,TManageDatabaseDialog::pmOnRightClickLeft)
    EVT_LIST_ITEM_RIGHT_CLICK(MD_PM_RIGHT,TManageDatabaseDialog::pmOnRightClickRight)
    EVT_MENU(MD_PM_OPEN, TManageDatabaseDialog::pmOnOpen)
    EVT_MENU(MD_PM_DEL, TManageDatabaseDialog::pmOnDelete)
    EVT_MENU(MD_PM_REN, TManageDatabaseDialog::pmOnRename)
END_EVENT_TABLE()


TManageDatabaseDialog::TManageDatabaseDialog ( wxWindow *parent , char *title , 
                            int mode , TVector *_v )
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 700 , 550 ) )
    {
    actionMode = mode ;
    il = NULL ;
    thetarget = NULL ;
    v = _v ;
    
    doLoad = ( mode & ACTION_MODE_LOAD ) != 0 ;
    doSave = ( mode & ACTION_MODE_SAVE ) != 0 ;
    isProject = ( mode & ACTION_MODE_PROJECT ) != 0 ;
    bool startup = ( mode & ACTION_MODE_STARTUP ) != 0 ;
    
    int w , h ;
#ifdef __WXMSW__
    GetClientSize ( &w , &h ) ;
#else // LINUX
    GetClientSize ( &w , &h ) ;
#endif

    wxClientDC dc ( (wxWindow*) this ) ;
    dc.GetTextExtent ( "X" , &bo , &th ) ;
    th = th * 3 / 2 ;
    bo = th / 4 ;
    wxBeginBusyCursor() ;
    nb = new wxNotebook ( (wxWindow*) this ,
                          -1 ,
                          wxPoint ( 0 , 0 ) ,
                          wxSize ( w , h ) ) ;

    pDatabases = new wxPanel ( nb , -1 ) ;
    pCopynMove = new wxPanel ( nb , -1 ) ;

    initDatabases () ;
    initCopynMove () ;

    if ( startup ) f_twopanes->SetValue ( 0 ) ;
    else if ( doLoad || doSave )
        {
        int tp = myapp()->frame->LS->getOption ( "TWOPANES" , 0 ) ;
        f_twopanes->SetValue ( tp ) ;
        }
    else  f_twopanes->SetValue ( 1 ) ;
    updateTwoLists () ;
    
    nb->SetSelection ( 1 ) ;
    
    Center () ;
    wxEndBusyCursor() ;
    }
    
TManageDatabaseDialog::~TManageDatabaseDialog ()
    {
    nb->DeleteAllPages() ;
    if ( il ) delete il ;
    }
    
void TManageDatabaseDialog::initCopynMove ()
    {
    wxPanel *p = pCopynMove ;

    nb->AddPage ( p , txt("t_management") ) ;
    int w , h ;
#ifdef __WXMSW__
    p->GetClientSize ( &w , &h ) ;
#else // LINUX 
    GetClientSize ( &w , &h ) ;
    w -= 20 ;
    h -= 40 ;
#endif
    if ( doSave ) h -= th ;

    wxRect r = ( new wxStaticText ( p , -1 , txt("t_filter") , wxPoint ( bo , bo ) ) ) -> GetRect() ;
    filter_txt = new wxTextCtrl ( p , MD_PM_FILTER , "" , 
                                    wxPoint ( bo+r.GetRight() , bo ) ,
                                    wxSize ( w*2/3 , th ) ) ;

    f_twopanes = new wxCheckBox ( p , MD_PM_TWOPANES , txt("t_twopanes") , 
                                        wxPoint ( bo , bo*2 + th ) ) ;

    
    int lbstyle = wxLC_LIST | wxLC_AUTOARRANGE | wxLC_SORT_ASCENDING ;
    pm_dd_l = new wxChoice ( p , MD_PM_DD_L , wxPoint ( bo , bo*2 + th*2 ) , wxSize ( w/2-bo*2 , th ) ) ;
    pm_dd_r = new wxChoice ( p , MD_PM_DD_R , wxPoint ( bo+w/2 , bo*2 + th*2 ) , wxSize ( w/2-bo*2 , th ) ) ;
    
    pm_left = new wxListCtrl ( p , MD_PM_LEFT , wxPoint ( bo , th*3+bo*2 ) ,
                                wxSize ( w/2-bo*2 , h-th*4-bo ) ,
                                lbstyle ) ;
    pm_right = new wxListCtrl ( p , MD_PM_RIGHT , wxPoint ( bo+w/2 , th*3+bo*2 ) ,
                                wxSize ( w/2-bo*2 , h-th*4-bo ) ,
                                lbstyle ) ;
    
    TMyDropTarget *rdt = new TMyDropTarget ( this , pm_right ) ;
    pm_right->SetDropTarget ( (wxDropTarget*) rdt ) ;
                                
    TMyDropTarget *ldt = new TMyDropTarget ( this , pm_left ) ;
    pm_left->SetDropTarget ( (wxDropTarget*) ldt ) ;


    wxBitmap bmp_helix ( myapp()->bmpdir+myapp()->slash+"Helix.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_project ( myapp()->bmpdir+myapp()->slash+"project.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_protein ( myapp()->bmpdir+myapp()->slash+"protein.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_primer ( myapp()->bmpdir+myapp()->slash+"primer.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_align ( myapp()->bmpdir+myapp()->slash+"alignment.bmp" , wxBITMAP_TYPE_BMP ) ;

    il = new wxImageList ( 21 , 15 ) ;
    il->Add ( bmp_helix ) ;
    il->Add ( bmp_project ) ;
    il->Add ( bmp_protein ) ;
    il->Add ( bmp_primer ) ;
    il->Add ( bmp_align ) ;
    
    pm_left->SetImageList ( il , wxIMAGE_LIST_SMALL ) ;
    pm_right->SetImageList ( il , wxIMAGE_LIST_SMALL ) ;
    
    if ( !isProject )
       {
       r = filter_txt->GetRect() ;
       f_dna = new wxCheckBox ( p , MD_PM_FILTER_DNA , txt("dna") , wxPoint ( r.GetRight() + bo , bo*2 ) ) ;
       r = f_dna->GetRect() ;
       f_prot = new wxCheckBox ( p , MD_PM_FILTER_PROTEIN , txt("protein") , wxPoint ( r.GetRight() + bo , bo*2 ) ) ;
       r = f_prot->GetRect() ;
       f_primer = new wxCheckBox ( p , MD_PM_FILTER_PRIMER , txt("primers") , wxPoint ( r.GetRight() + bo , bo*2 ) ) ;
       r = f_dna->GetRect() ;
       f_desc = new wxCheckBox ( p , MD_PM_FILTER_DESC , txt("desc") , wxPoint ( r.GetLeft() , bo*2 + th ) ) ;
       r = f_desc->GetRect() ;
       f_seq = new wxCheckBox ( p , MD_PM_FILTER_SEQ , txt("sequences") , wxPoint ( r.GetRight() + bo , bo*2 + th ) ) ;
       f_desc->SetValue ( 1 ) ;
       }
    

    if ( !doSave )
        {
        pm_init_lists () ;
        filter_txt->SetFocus() ;
        return ;
        }
        
    wxString name ;
    if ( isProject ) name = myapp()->frame->project_name ;
    else name = v->getName() ;
    h += th ;
    int w2 = w/5 ;
    pm_dd_save = new wxChoice ( p , MD_PM_SAVE_DB ,
                                    wxPoint ( bo , h - th*2 + bo*2 ) ,
                                    wxSize ( w2 , th ) ) ;
    pm_name = new wxTextCtrl ( p , MD_PM_EDIT , name , 
                                    wxPoint ( bo*2+w2 , h - th*2 + bo*2 ) ,
                                    wxSize ( w-w2*2 - bo*3 , th ) ) ;
    wxButton *sb = new wxButton ( p , MD_PM_SAVE , txt("b_store") , 
                    wxPoint ( w-w2 , h - th*2 + bo*2 ) ,
                    wxSize ( w2 - bo , th ) ) ;
    pm_init_lists () ;
    sb->SetDefault () ;
    pm_name->SetSelection ( -1 , -1 ) ;
    pm_name->SetFocus() ;
    }
    
void TManageDatabaseDialog::updateTwoLists ()
    {
    int w , h ;
#ifdef __WXMSW__
    pCopynMove->GetClientSize ( &w , &h ) ;
#else // LINUX 
    GetClientSize ( &w , &h ) ;
    w -= 20 ;
    h -= 40 ;
#endif
    if ( doSave ) h -= th ;
    if ( f_twopanes->GetValue() ) 
        {
        pm_left->SetSize ( wxSize ( w/2-bo*2 , h-th*4-bo ) ) ;
        pm_right->Show () ;
        pm_dd_r->Show () ;
        pm_list_items ( PM_RIGHT ) ;
        }
    else
        {
        pm_right->Hide () ;
        pm_dd_r->Hide () ;
        pm_left->SetSize ( wxSize ( w - bo*2 , h-th*4-bo ) ) ;
        }
    }
    
void TManageDatabaseDialog::pm_init_lists ()
    {
    int a ;
    pm_dd_l->Clear () ;
    pm_dd_r->Clear () ;
    if ( doSave )
        pm_dd_save->Clear () ;
    for ( a = 0 ; a < db_name.GetCount() ; a++ )
        {
        pm_dd_l->Append ( db_name[a] ) ;
        pm_dd_r->Append ( db_name[a] ) ;
        if ( doSave )
           pm_dd_save->Append ( db_name[a] ) ;
        }

    pm_dd_l->SetStringSelection ( txt("local_db") ) ;
    pm_dd_r->SetStringSelection ( defdb ) ;
    if ( doSave )
        {
        wxString db = defdb ;
        if ( isProject && !myapp()->frame->project_db.IsEmpty() )
           db = myapp()->frame->project_db ;
        else if ( !isProject )
           db = v->getDatabase() ;
        if ( db.IsEmpty() ) db = defdb ;
        pm_dd_save->SetStringSelection ( db ) ;
        }

    pm_list_items ( PM_LEFT ) ;
//    pm_list_items ( PM_RIGHT ) ;
    pm_left->SetFocus() ;
    }
    
TStorage *TManageDatabaseDialog::getTempDB ( wxString filename )
    {
    return myapp()->frame->getTempDB ( filename ) ;
    }
    
void TManageDatabaseDialog::pm_list_items ( int x )
    {
    int a ;
    wxChoice *c ;
    wxListCtrl *l ;
    if ( x == PM_LEFT )
        {
        c = pm_dd_l ;
        l = pm_left ;
        }
    else
        {
        c = pm_dd_r ;
        l = pm_right ;
        }
    if ( !l->IsShown() ) return ; // No need to load data for a list that isn't visible
    SetCursor ( *wxHOURGLASS_CURSOR ) ;
    wxString name = c->GetStringSelection() ;
    TStorage *st = getTempDB ( getFileName ( name ) ) ;
    
    l->DeleteAllItems () ;
    TSQLresult r ;
    if ( isProject )
        {
        wxString sql = "SELECT pr_name FROM project" ;
        r = st->getObject ( sql ) ;
        for ( a = 0 ; a < r.rows() ; a++ )
           {
           wxString s = r[a][r["pr_name"]] ;
           l->InsertItem ( a , s , 1 ) ;
           }        
        }
    else
        {
        wxString sql = "SELECT dna_name,dna_type FROM dna" ;
//        if ( !filter.IsEmpty() )
           {
           for ( a = 0 ; a < filter.length() ; a++ )
              if ( filter.GetChar(a) == '\"' ) filter.SetChar ( a , '\'' ) ;
           wxArrayString vf ;
           explode ( " " , filter , vf ) ;
           wxString sql2 , sql3 ;
           for ( a = 0 ; a < vf.GetCount() ; a++ )
              {
              if ( !vf[a].IsEmpty() )
                 {
                 if ( !sql2.IsEmpty() ) sql2 += " AND" ;
                 sql2 += " (dna_name LIKE \"%" + vf[a] + "%\"" ;
                 if ( f_desc->GetValue() )
                    sql2 += " OR dna_description LIKE \"%" + vf[a] + "%\"" ;
                 if ( f_seq->GetValue() )
                    sql2 += " OR dna_sequence LIKE \"%" + vf[a] + "%\"" ;
                 sql2 += ")" ;
                 }
              }
           if ( f_dna->GetValue() )
              {
              if ( !sql3.IsEmpty() ) sql3 += " OR " ;
              sql3 += "dna_type=0 OR dna_type=1 OR dna_type=2" ;
              }
           if ( f_prot->GetValue() )
              {
              if ( !sql3.IsEmpty() ) sql3 += " OR " ;
              sql3 += "dna_type=5" ;
              }
           if ( f_primer->GetValue() )
              {
              if ( !sql3.IsEmpty() ) sql3 += " OR " ;
              sql3 += "dna_type=3" ;
              }
           if ( false ) // Alignment
              {
              if ( !sql3.IsEmpty() ) sql3 += " OR " ;
              sql3 += "dna_type=4" ;
              }
           if ( !sql3.IsEmpty() )
              {
              if ( !sql2.IsEmpty() ) sql2 += " AND (" + sql3 + ")" ;
              else sql2 = " " + sql3 ;
              }
           if ( !sql2.IsEmpty() ) sql += " WHERE" + sql2 ;
           }
        r = st->getObject ( sql ) ;        
        for ( a = 0 ; a < r.rows() ; a++ )
           {
           wxString s = r[a][r["dna_name"]] ;
           if ( r[a][r["dna_type"]] == "5" ) l->InsertItem ( a , s , 2 ) ;
           else if ( r[a][r["dna_type"]] == "3" ) l->InsertItem ( a , s , 3 ) ;
           else if ( r[a][r["dna_type"]] == "4" ) l->InsertItem ( a , s , 4 ) ;
           else l->InsertItem ( a , s , 0 ) ;
           }
        }
    SetCursor ( *wxSTANDARD_CURSOR ) ;
    }
    
wxArrayInt TManageDatabaseDialog::getSelectedListItems ( wxListCtrl *l )
    {
    long item = -1 ;
    wxArrayInt vi ;
    while ( true )
        {
        item = l->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) ;
        if ( item == -1 ) break;
        vi.Add ( item ) ;
        }
    return vi ;
    }
    
void TManageDatabaseDialog::pmOnLeftListEvent ( wxListEvent &ev )
    {
    pmOnListEvent ( ev , pm_left , pm_right ) ;
    }
    
void TManageDatabaseDialog::pmOnRightListEvent ( wxListEvent &ev )
    {
    pmOnListEvent ( ev , pm_right , pm_left ) ;
    }

void TManageDatabaseDialog::pmOnListEvent ( wxListEvent &ev , wxListCtrl *source , wxListCtrl *target )
    {
    thetarget = target ;
    wxChoice *dds = pm_dd_l ;
    wxChoice *ddt = pm_dd_r ;
    if ( source == pm_right )
        {
        dds = pm_dd_r ;
        ddt = pm_dd_l ;
        }
    
    int a ;
    wxArrayInt vi ;
    wxArrayString vs ;
    vi = getSelectedListItems ( source ) ;
    for ( a = 0 ; a < vi.GetCount() ; a++ )
        vs.Add ( source->GetItemText ( vi[a] ) ) ;
    
    wxTextDataObject my_data(GENTLE_DRAG_DATA);
    wxDropSource dragSource( this );
	dragSource.SetData( my_data );
	wxDragResult result = dragSource.DoDragDrop( TRUE );
	if ( result != wxDragCopy && result != wxDragMove ) return ;

	wxString sdb = dds->GetStringSelection() ;
	wxString tdb = ddt->GetStringSelection() ;
	bool goOn = true ;
    for ( a = 0 ; goOn && a < vs.GetCount() ; a++ )
        {
        if ( result == wxDragCopy ) goOn = do_copy ( vs[a] , sdb , tdb ) ;
        if ( result == wxDragMove ) goOn = do_move ( vs[a] , sdb , tdb ) ;
        }
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;
    }

bool TManageDatabaseDialog::do_copy ( wxString name , wxString sdb , wxString tdb )
    {
    if ( isProject ) return false ;
    else return copyDNA ( name , sdb , tdb ) ;
    }
    
bool TManageDatabaseDialog::do_move ( wxString name , wxString sdb , wxString tdb )
    {
    if ( isProject ) return false ;
    else return moveDNA ( name , sdb , tdb ) ;
    }
    
void TManageDatabaseDialog::do_del  ( wxString name , wxString db )
    {
    if ( isProject ) return ;
    else delDNA ( name , db ) ;
    }


bool TManageDatabaseDialog::copyDNA ( wxString name , wxString sdb , wxString tdb )
    {
    if ( sdb == tdb ) return false ;
    SetCursor ( *wxHOURGLASS_CURSOR ) ;
    TStorage *source = getTempDB ( getFileName ( sdb ) ) ;
    TStorage *target = getTempDB ( getFileName ( tdb ) ) ;
    wxString sql , s , t ;

    int a ;
    TSQLresult r ;
    name = source->fixDNAname ( name ) ;
        
    // Already there?
    sql = "SELECT dna_name FROM dna WHERE dna_name=\""+name+"\"" ;
    r = target->getObject ( sql ) ;
    if ( r.rows() > 0 )
        {
        wxMessageDialog md ( this , txt("t_entry_exists_brief") , txt("msg_box") ) ;
        md.ShowModal() ;
        return false ;
        }
        
    // Copy DNA and items
    source->copySQLfields ( *target , "dna" , "dna_name=\""+name+"\"" ) ;
    source->copySQLfields ( *target , "dna_item" , "di_dna=\""+name+"\"" ) ;
    
    // Enzymes
    sql = "SELECT dna_restriction_enzymes FROM dna WHERE dna_name=\"" + name + "\"" ;
    r = source->getObject ( sql ) ;
    if ( r.rows() == 0 ) return false ;
    s = r[0][0] ;
    t = "" ;
    wxArrayString ve ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( s.GetChar(a) == ',' )
           {
           if ( !t.IsEmpty() ) ve.Add ( t ) ;
           t = "" ;
           }
        else t += s.GetChar(a) ;
        }
    if ( !t.IsEmpty() ) ve.Add ( t ) ;

    for ( a = 0 ; a < ve.GetCount() ; a++ )
        {
        sql = "SELECT e_name FROM enzyme WHERE e_name=\"" + ve[a] + "\"" ;
        r = target->getObject ( sql ) ;
        if ( r.rows() == 0 )
           source->copySQLfields ( *target , "enzyme" , "e_name=\"" + ve[a] + "\"" ) ;
        }
        
    SetCursor ( *wxSTANDARD_CURSOR ) ;
	return true ;
    }

bool TManageDatabaseDialog::moveDNA ( wxString name , wxString sdb , wxString tdb )
    {
    bool doit = copyDNA ( name , sdb , tdb ) ;
    if ( doit ) delDNA ( name , sdb ) ;
    return doit ;
    }

void TManageDatabaseDialog::delDNA ( wxString name , wxString db )
    {
    TStorage *source = getTempDB ( getFileName ( db ) ) ;
    name = source->fixDNAname ( name ) ;
    source->getObject ( "DELETE FROM dna WHERE dna_name=\"" + name + "\"" ) ;
    source->getObject ( "DELETE FROM dna_item WHERE di_dna=\"" + name + "\"" ) ;
    }

wxString TManageDatabaseDialog::getFileName ( wxString dbname )
    {
    int a ;
    for ( a = 0 ; a < db_name.GetCount() ; a++ )
        if ( db_name[a] == dbname )
           return db_file[a] ;
    return "" ;
    }
    
    
    
    
void TManageDatabaseDialog::initDatabases ()
    {
    wxPanel *p = pDatabases ;
    nb->AddPage ( p , txt("t_databases") ) ;

    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;
    wxFlexGridSizer *g1 = new wxFlexGridSizer ( 3 , 3 , 5 , 5 ) ;

    g1->Add ( new wxStaticText ( p , -1 , "" ) , 1 , wxEXPAND , 5 ) ;
    g1->Add ( new wxButton ( p , MD_PD_DEL , txt("b_del") ) , 1 , wxEXPAND , 5 ) ;
    g1->Add ( new wxButton ( p , MD_PD_DEFAULT , txt("b_set_default") ) , 1 , wxEXPAND , 5 ) ;

    g1->Add ( new wxStaticText ( p , -1 , txt("t_files") ) , 1 , wxEXPAND , 5 ) ;
    g1->Add ( new wxButton ( p , MD_PD_ADD , txt("b_add") ) , 1 , wxEXPAND , 5 ) ;
    g1->Add ( new wxButton ( p , MD_PM_NEW , txt("b_new") ) , 1 , wxEXPAND , 5 ) ;

    g1->Add ( new wxStaticText ( p , -1 , txt("t_mysql") ) , 1 , wxEXPAND , 5 ) ;
    g1->Add ( new wxButton ( p , MD_PD_ADD_MYSQL , txt("b_add") ) , 1 , wxEXPAND , 5 ) ;
//    g1->Add ( new wxButton ( p , MD_PM_NEW_MYSQL , txt("b_new") ) , 1 , wxEXPAND , 5 ) ;

    pd_db_name = new wxStaticText ( p , -1 , "" ) ;
    pd_db_file = new wxStaticText ( p , -1 , "" ) ;
    v1->Add ( new wxStaticText ( p , -1 , txt("t_databases") ) , 0 , wxEXPAND , 5 ) ;
    v1->Add ( new wxStaticText ( p , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v1->Add ( pd_db_name , 0 , wxEXPAND , 5 ) ;
    v1->Add ( pd_db_file , 0 , wxEXPAND , 5 ) ;
    v1->Add ( new wxStaticText ( p , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v1->Add ( g1 , 1 , wxEXPAND | wxALL , 5 ) ;
    
    pd_db = new wxListBox ( p , MD_PD_DBLIST ) ;
    h1->Add ( pd_db , 1 , wxEXPAND , 5 ) ;
    h1->Add ( v1 , 1 , wxEXPAND , 5 ) ;
    
    SetSizer ( h1 ) ;

    pd_loadList () ;
    pd_db->SetStringSelection ( txt("local_db") ) ;
    accessDB () ;
    }
    
void TManageDatabaseDialog::accessDB ()
    {
    int a ;
    wxString name , file ;
    name = pd_db->GetStringSelection() ;
    for ( a = 0 ; db_name[a] != name ; a++ ) ;
    file = db_file[a] ;
    pd_db_name->SetTitle ( wxString::Format ( txt("t_db_name") , db_name[a].c_str() ) ) ;
    pd_db_file->SetTitle ( wxString::Format ( txt("t_db_location") , db_file[a].c_str() ) ) ;
    }

void TManageDatabaseDialog::pd_loadList ()
    {
    defdb = myapp()->frame->LS->getDatabaseList ( db_name , db_file ) ;
    int a ;
    pd_db->Clear () ;
    for ( a = 0 ; a < db_name.GetCount() ; a++ )
        pd_db->Append ( db_name[a] ) ;
    }

// Event handlers

void TManageDatabaseDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TManageDatabaseDialog::OnOK ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }
    
void TManageDatabaseDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }

void TManageDatabaseDialog::pdOnDBchange ( wxCommandEvent &ev )
    {
    accessDB () ;
    }
    
void TManageDatabaseDialog::pdOnNew ( wxCommandEvent &ev )
    {
    wxString wildcard = "GENtle database (*.db)|*.db" ;
    wxFileDialog d ( this , txt("t_add_new_db") , "" , "" , wildcard , wxSAVE|wxOVERWRITE_PROMPT ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;

    wxString fn = d.GetPath() ;
    wxString blank = myapp()->homedir ;
    blank += "\\blank.db" ;
    
    bool b = wxCopyFile ( blank , fn , true ) ;
    if ( !b )
        {
        wxMessageBox ( fn , txt("t_db_already_exists") ) ;
        return ;
        }
    
    addDatabase ( fn ) ;
    }
    
void TManageDatabaseDialog::pdOnAdd ( wxCommandEvent &ev )
    {
    wxString wildcard = "GENtle database (*.db)|*.db" ;
    wxFileDialog d ( this , txt("t_choose_db") , "" , "" , wildcard , wxOPEN ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;
    
    wxString fn = d.GetPath() ;
    addDatabase ( fn ) ;
    }
    
void TManageDatabaseDialog::addDatabase ( wxString fn )
    {
    int a , b ;
    for ( a = 0 ; a < db_file.GetCount() && db_file[a] != fn ; a++ ) ;
    if ( a < db_file.GetCount() ) // Already in the list
        {
        wxMessageDialog md ( this , txt("t_db_in_use") ) ;
        md.ShowModal() ;
        return ;
        }
        
    wxString t ;
    if ( fn.GetChar(0) == ':' )
        {
        wxArrayString vv ;
        explode ( ":" , fn + ":" , vv ) ;
        t = vv[4] ;
        }
    else
        {
        for ( a = b = 0 ; a < fn.length() ; a++ )
            if ( fn.GetChar(a) == '\\' || fn.GetChar(a) == '/' )
               b = a ;
        t = fn.substr ( b+1 ) ;
        for ( a = b = 0 ; a < t.length() && b == 0 ; a++ )
            if ( t.GetChar(a) == '.' )
               b = a ;
        t = t.substr ( 0 , b ) ;
        }

    // Avoiding double names
    if ( t.GetChar(0) >= 'a' && t.GetChar(0) <= 'z' )
        t.SetChar ( 0 , t.GetChar(0) - 'a' + 'A' ) ; // Uppercase
    wxString s ;
    char z[100] ;
    b = 0 ;
    *z = 0 ;
    do {
        s = t + z ;
        b++ ;
        sprintf ( z , " %d" , b ) ;
        for ( a = 0 ; a < db_name.GetCount() && db_name[a] != s ; a++ ) ;
        } while ( a < db_name.GetCount() ) ;
    t = s ;
    
    // Saving in database
    TSQLresult r ;
    wxString sql = "INSERT INTO stuff (s_type,s_name,s_value)"
                 "VALUES (\"DATABASE\",\"" + t + "\",\"" + fn + "\")" ;
    r = myapp()->frame->LS->getObject ( sql ) ;
    pd_loadList () ;
    pd_db->SetStringSelection ( t ) ;
    pm_init_lists () ;
    }

void TManageDatabaseDialog::pdOnDel ( wxCommandEvent &ev )
    {
    wxString name = pd_db->GetStringSelection() ;
    if ( name == txt("local_db") || name.IsEmpty() )
        {
        wxMessageDialog md ( this , txt("t_cannot_del_local_db") ) ;
        md.ShowModal() ;
        return ;
        }
    
    int a ;
    for ( a = 0 ; a < db_name.GetCount() && db_name[a] != name ; a++ ) ;
    
    // Removing from database
    TSQLresult r ;
    wxString sql = "DELETE FROM stuff WHERE "
                 "s_type=\"DATABASE\" AND "
                 "s_name=\"" + name + "\" AND "
                 "s_value=\"" + db_file[a] + "\"" ;
    r = myapp()->frame->LS->getObject ( sql ) ;
    pd_loadList () ;
    pd_db->SetStringSelection ( txt("local_db") ) ;
    pm_init_lists () ;
    }
    
void TManageDatabaseDialog::pdOnSetDefault ( wxCommandEvent &ev )
    {
    wxString name = pd_db->GetStringSelection() ;
    if ( name.IsEmpty() ) return ;
    defdb = name ;
    
    
    wxString sql = "DELETE FROM stuff WHERE s_type=\"DEFAULT_DATABASE\"" ;
    myapp()->frame->LS->getObject ( sql ) ;
    
    sql = "INSERT INTO stuff (s_type,s_name,s_value)"
                 "VALUES (\"DEFAULT_DATABASE\",\"" + defdb + "\",\"" + defdb + "\")" ;
    myapp()->frame->LS->getObject ( sql ) ;                 
    }

void TManageDatabaseDialog::pmOnLeftChoice ( wxCommandEvent &ev )
    {
    pm_list_items ( PM_LEFT ) ;
    }

void TManageDatabaseDialog::pmOnRightChoice ( wxCommandEvent &ev )
    {
    pm_list_items ( PM_RIGHT ) ;
    }
    
void TManageDatabaseDialog::pmOnSelectLeft ( wxListEvent &ev )
    {
    pmOnSelect ( ev , pm_left ) ;
    }
    
void TManageDatabaseDialog::pmOnSelectRight ( wxListEvent &ev )
    {
    pmOnSelect ( ev , pm_right ) ;
    }
    
void TManageDatabaseDialog::pmOnSelect ( wxListEvent &ev , wxListCtrl *side )
    {
    if ( !doSave ) return ;
    int i = ev.GetIndex () ;
    wxString s = side->GetItemText(i) ;
    pm_name->SetValue ( s ) ;
    }

void TManageDatabaseDialog::pmOnActivateLeft ( wxListEvent &ev )
    {
    pmOnActivate ( ev , pm_left ) ;
    }
    
void TManageDatabaseDialog::pmOnActivateRight ( wxListEvent &ev )
    {
    pmOnActivate ( ev , pm_right ) ;
    }
    
void TManageDatabaseDialog::pmGetContext ( wxListCtrl *side )
    {
    wxArrayInt vi ;
    context_names.Clear () ;
    vi = getSelectedListItems ( side ) ;
    for ( int a = 0 ; a < vi.GetCount() ; a++ )
        context_names.Add ( side->GetItemText ( vi[a] ) ) ;
    
    }

void TManageDatabaseDialog::pmOnActivate ( wxListEvent &ev , wxListCtrl *side )
    {
    wxChoice *c = (side==pm_left)?pm_dd_l:pm_dd_r;
    int i = ev.GetIndex () ;
    wxString t = c->GetStringSelection() ;
    pmGetContext ( side ) ;
    pmOpenFiles ( context_names , t ) ;
    }
    
void TManageDatabaseDialog::pmOpenFiles ( wxArrayString &_names , wxString _db )
    {
    if ( _names.GetCount() == 0 ) return ;
    if ( doLoad || doSave )
        myapp()->frame->LS->setOption ( "TWOPANES" , f_twopanes->GetValue() ) ;
    wxBeginBusyCursor () ;
    myapp()->frame->Freeze () ;
    for ( int a = 0 ; a < _names.GetCount() ; a++ )
       do_load ( _names[a] , _db ) ;
    myapp()->frame->Thaw () ;
    wxEndBusyCursor () ;

    SetReturnCode ( wxID_OK ) ;
    EndModal ( true ) ;
    
    wxSizeEvent se ;
    myapp()->frame->OnSize ( se ) ;
    myapp()->frame->mainTree->Refresh () ;
    }

// --------------------------------

bool TManageDatabaseDialog::do_load ( wxString name , wxString db )
    {
    if ( isProject ) return do_load_project ( name , db ) ;
    else return do_load_DNA ( name , db ) ;
    }
    
bool TManageDatabaseDialog::do_load_project ( wxString name , wxString db )
    {
    int a ;
    wxString sql ;
    TStorage *tstorage = getTempDB ( getFileName ( db ) ) ;
    TSQLresult sr ;

    MyFrame *f = myapp()->frame ;
    int oc = f->children.GetCount() ;
    
    // Closing current windows
    wxCommandEvent dummy ;
    myapp()->frame->OnProjectClose ( dummy ) ;
    
    // Setting new project name, loading desc etc.
    name = fixQuotes ( name ) ;
    sql = "SELECT pr_desc FROM project WHERE pr_name=\""+name+"\"" ;
    sr = tstorage->getObject ( sql ) ;
    if ( sr.rows() == 0 ) return false ;
        
    myapp()->frame->project_name = name ;
    myapp()->frame->mainTree->SetItemText ( myapp()->frame->mainTree->treeroot , name ) ;
    myapp()->frame->project_db = db ;
    myapp()->frame->project_desc = sr[0][0] ;

    wxDialog dlg ( NULL , -1 , "Test" , wxDefaultPosition , wxSize ( 300 , 20 ) ) ;
    dlg.Show() ;

    myapp()->frame->SetFocus() ;
    wxBeginBusyCursor() ;
    myapp()->frame->Freeze () ;

    // Load associated DNA list
    sql = "SELECT * FROM project_dna WHERE pd_project=\""+name+"\"" ;
    sr = tstorage->getObject ( sql ) ;

    // Load DNA
    bool all = true ;
    for ( a = 0 ; a < sr.rows() ; a++ )
        {
        wxString dna_name = sr[a][sr["pd_dna"]] ;
        wxString dna_db = sr[a][sr["pd_database"]] ;
        wxString msg = wxString::Format ( txt("t_loading") , dna_name.c_str() ) ;
        dlg.SetTitle ( msg ) ;
        dlg.SetFocus() ;
        all &= do_load_DNA ( dna_name , dna_db ) ;
        }
    if ( !all )
        {
        wxMessageDialog md ( this , txt("t_not_all_files_loaded") ,
                                txt("msg_box") ) ;
        md.ShowModal() ;
        }
    
    myapp()->frame->Thaw () ;
    wxEndBusyCursor() ;
    return true ;
    }
    
bool TManageDatabaseDialog::do_load_DNA ( wxString name , wxString db )
    {
    int a ;
    wxString sql ;
    TStorage *tstorage = getTempDB ( getFileName ( db ) ) ;
    TSQLresult sr ;
    v = new TVector () ;

    if ( name.IsEmpty() ) return false ;

    // Loading vector
    sql = "SELECT * FROM dna WHERE dna_name=\"" + name + "\"" ;
    sr = tstorage->getObject ( sql ) ;
    if( sr.rows() == 0 ) return false ;
    v->setName ( sr[0][sr["dna_name"]] ) ;
    v->setDescription ( sr[0][sr["dna_description"]] ) ;
    v->setSequence ( sr[0][sr["dna_sequence"]] ) ;
    v->type = atoi ( sr[0][sr["dna_type"]].c_str() ) ;
    v->setStickyEnd ( true , true , sr[0][sr["dna_sticky_ul"]] ) ;
    v->setStickyEnd ( true , false , sr[0][sr["dna_sticky_ll"]] ) ;
    v->setStickyEnd ( false , true , sr[0][sr["dna_sticky_ur"]] ) ;
    v->setStickyEnd ( false , false , sr[0][sr["dna_sticky_lr"]] ) ;

    v->setParams ( sr[0][sr["dna_params"]] ) ;
    if ( sr[0][sr["dna_circular"]] == "1" ) v->setCircular ( true ) ;
    else v->setCircular ( false ) ;
    v->setDatabase ( db ) ;
    if ( v->type != TYPE_ALIGNMENT ) v->removeBlanksFromSequence () ;

    wxString s = sr[0][sr["dna_restriction_enzymes"]] , t = "" ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( s.GetChar(a) == ',' )
           {
           if ( !t.IsEmpty() )
              {
              if ( t.GetChar(0) == '*' ) v->proteases.Add ( t.substr ( 1 ) ) ;
              else v->re.Add ( tstorage->getRestrictionEnzyme ( t ) ) ;
              }
           t = "" ;
           }
        else t += s.GetChar(a) ;
        }
    if ( !t.IsEmpty() )
        {
        if ( t.GetChar(0) == '*' ) v->proteases.Add ( t.substr ( 1 ) ) ;
        else v->re.Add ( tstorage->getRestrictionEnzyme ( t ) ) ;
        }

    // Loading items
    sql = "SELECT * FROM dna_item WHERE di_dna=\"" + name + "\"" ;
    sr = tstorage->getObject ( sql ) ;
    for ( a = 0 ; a < sr.rows() ; a++ )
        {
        TVectorItem i ;
        i.name = sr[a][sr["di_name"]] ;
        i.desc = sr[a][sr["di_description"]] ;
        i.setType ( sr[a][sr["di_type"]].GetChar(0) ) ;
        i.from = atoi ( sr[a][sr["di_from"]].c_str() ) ;
        i.to = atoi ( sr[a][sr["di_to"]].c_str() ) ;
        i.setDirection ( atoi ( sr[a][sr["di_direction"]].c_str() ) ) ;
        i.explodeParams ( sr[a][sr["di_params"]] ) ;
        v->items.push_back ( i ) ;
        }
        
    // Sorting by size, largest first
    for ( a = 1 ; a < v->items.size() ; a++ )
        {
        if ( v->getItemLength ( a-1 ) < v->getItemLength ( a ) )
           {
           TVectorItem dummy = v->items[a] ;
           v->items[a] = v->items[a-1] ;
           v->items[a-1] = dummy ;
           a = 0 ;
           }
        }

    v->recalcvisual = true ;
    v->undo.clear() ;
    
    ChildBase *n = NULL ;
    if ( v->type == TYPE_AMINO_ACIDS )
        {
        wxString db1 = v->getDatabase() ;
        n = myapp()->frame->newAminoAcids ( v , v->getName() ) ;
        myass ( n , "Opening AA" ) ;
        n->vec->setDatabase ( db1 ) ;
        n->vec->setWindow ( n ) ;
        }
    else if ( v->type == TYPE_ALIGNMENT )
        {
        wxArrayString vs ;
        wxArrayChildBase vc ;
        TAlignment *ali = myapp()->frame->runAlignment ( vs , vc , v ) ;
        ali->name = name ;
        ali->database = db ;
        }
    else
        {
        v->recalculateCuts () ;
        wxString db1 = v->getDatabase() ;
        n = myapp()->frame->newFromVector ( v , v->type ) ;
        myass ( n , "Opening DNA" ) ;
        n->vec->setDatabase ( db1 ) ;
        }

    return true ;
    }
    
// REDUNDANT : TStorage::fixDNAname does the same
wxString TManageDatabaseDialog::fixQuotes ( wxString s )
    {
    int a ;
    for ( a = 0 ; a < s.length() ; a++ ) // Fixing quotes
        if ( s.GetChar(a) == '"' ) s.SetChar ( a , 39 ) ;
    return s ;
    }
    
bool TManageDatabaseDialog::doesNameExist ( wxString name , wxString dbname )
    {
    TStorage *storage = getTempDB ( getFileName ( dbname ) ) ;
    wxString sql ;
    TSQLresult sr ;
    wxString x = fixQuotes ( name ) ;
    if ( isProject )
        sql = "SELECT pr_name FROM project WHERE pr_name=\"" + x + "\"" ;
    else
        sql = "SELECT dna_sequence FROM dna WHERE dna_name=\"" + x + "\"" ;
    sr = storage->getObject ( sql ) ;
    if ( sr.content.size() == 0 ) return false ;
    
    wxString s , sc ;
    if ( isProject )
        {
        s = sr[0][sr["pr_name"]] ;
        sc = s ;
        }
    else
        {
        s = sr[0][sr["dna_sequence"]] ;
        sc = v->getSequence() ;
        }
    if ( s == sc )
        {
        wxMessageDialog md ( this , txt("t_entry_exists_but") , txt("msg_box") ,
                                wxYES|wxNO|wxYES_DEFAULT ) ;
        int r = md.ShowModal () ;
        if ( r == wxID_YES ) return false ;
        }
    else
        {
        wxMessageDialog md ( this , txt("t_entry_exists") , txt("msg_box") ) ;
        md.ShowModal () ;
        }
    
    return true ;
    }

void TManageDatabaseDialog::pmOnSave ( wxCommandEvent &ev )
    {
    if ( isProject ) do_save_project () ;
    else do_save_DNA () ;
    }
    
void TManageDatabaseDialog::do_save_project ()
    {
    wxString name , dbname ;
    name = pm_name->GetValue() ;
    dbname = pm_dd_save->GetStringSelection() ;
    if ( myapp()->frame->project_name != name &&
         doesNameExist ( name , dbname ) ) return ;
    TStorage *storage = getTempDB ( getFileName ( dbname ) ) ;

    // New name, or overwriting old one
    int a ;
    wxString sql , s1 , s2 ;
    wxString x = fixQuotes ( name ) ;

    // New default project
    myapp()->frame->project_db = dbname ;

    // Deleting old one, if any
    sql = "DELETE FROM project WHERE pr_name=\""+x+"\"" ;
    storage->getObject ( sql ) ;
    sql = "DELETE FROM project_dna WHERE pd_project=\""+x+"\"" ;
    storage->getObject ( sql ) ;
    
    // Storing new one
    s1 = s2 = "" ;
    storage->sqlAdd ( s1 , s2 , "pr_name" , x ) ;
    storage->sqlAdd ( s1 , s2 , "pr_desc" , "No description" ) ;
    sql = "INSERT INTO project ("+s1+") VALUES ("+s2+")" ;
    storage->getObject ( sql ) ;
    
    for ( a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
        {
        if ( myapp()->frame->children[a]->def == "dna" ||
             myapp()->frame->children[a]->def == "AminoAcids" ||
             myapp()->frame->children[a]->def == "alignment" )
           {
           ChildBase *c = myapp()->frame->children[a] ;
           wxString dna_name , dna_db ;
           if ( c->def == "alignment" )
              {
              TAlignment *ali = (TAlignment*) c ;
              dna_name = ali->name ;
              dna_db = ali->database ;
              }
           else
              {
              dna_name = c->vec->getName() ;
              dna_db = c->vec->getDatabase() ;
              }
           if ( !dna_db.IsEmpty() )
              {
              s1 = s2 = "" ;
              storage->sqlAdd ( s1 , s2 , "pd_project" , x ) ;
              storage->sqlAdd ( s1 , s2 , "pd_dna" , dna_name ) ;
              storage->sqlAdd ( s1 , s2 , "pd_database" , dna_db ) ;
              sql = "INSERT INTO project_dna ("+s1+") VALUES ("+s2+")" ;
              storage->getObject ( sql ) ;
              }
            else
               {
//               wxMessageDialog md ( this , "Not saved" , myapp()->frame->children[a]->def ) ;
//               md.ShowModal() ;
               }
           }
        }
    if ( myapp()->frame->project_name != x )
        {
        myapp()->frame->project_name = x ;
        myapp()->frame->mainTree->SetItemText ( myapp()->frame->mainTree->treeroot , x ) ;
        }

    SetReturnCode ( wxID_OK ) ;
    EndModal ( true ) ;
    }
    
void TManageDatabaseDialog::do_save_DNA ()
    {
    wxString name , dbname ;
    name = pm_name->GetValue() ;
    dbname = pm_dd_save->GetStringSelection() ;
    if ( doesNameExist ( name , dbname ) ) return ;
    TStorage *storage = getTempDB ( getFileName ( dbname ) ) ;

    // New name, or overwriting old one
    int a ;
    TSQLresult sr ;
    wxString sql , s1 , s2 ;
    wxString x = fixQuotes ( name ) ;
    v->setDatabase ( dbname ) ; // Now in that database
    
    // Deleting old one, if any
    sql = "DELETE FROM dna WHERE dna_name=\""+x+"\"" ;
    sr = storage->getObject ( sql ) ;
    sql = "DELETE FROM dna_item WHERE di_dna=\""+x+"\"" ;
    sr = storage->getObject ( sql ) ;
    
    // Storing new one
    wxString enzymes = "," ;
    
    // Restriction enzymes
    for ( a = 0 ; a < v->re.GetCount() ; a++ )
        enzymes += v->re[a]->name + "," ;
        
    // Proteases
    for ( a = 0 ; a < v->proteases.GetCount() ; a++ )
        enzymes += "*" + wxString ( v->proteases[a] ) + "," ;
        
    s1 = s2 = "" ;
    storage->sqlAdd ( s1 , s2 , "dna_name" , x ) ;
    storage->sqlAdd ( s1 , s2 , "dna_description" , v->getDescription() ) ;
    storage->sqlAdd ( s1 , s2 , "dna_type" , v->type ) ;
    storage->sqlAdd ( s1 , s2 , "dna_sequence" , v->getSequence() ) ;
    storage->sqlAdd ( s1 , s2 , "dna_sticky_ul" , v->getStickyEnd(true,true) ) ;
    storage->sqlAdd ( s1 , s2 , "dna_sticky_ll" , v->getStickyEnd(true,false) ) ;
    storage->sqlAdd ( s1 , s2 , "dna_sticky_ur" , v->getStickyEnd(false,true) ) ;
    storage->sqlAdd ( s1 , s2 , "dna_sticky_lr" , v->getStickyEnd(false,false) ) ;
    storage->sqlAdd ( s1 , s2 , "dna_circular" , v->isCircular() ) ;
    storage->sqlAdd ( s1 , s2 , "dna_restriction_enzymes" , enzymes ) ;
    storage->sqlAdd ( s1 , s2 , "dna_params" , fixQuotes ( v->getParams() ) ) ;
    sql = "INSERT INTO dna (" + s1 + ") VALUES (" + s2 + ")" ;
    sr = storage->getObject ( sql ) ;
    v->setName ( x ) ;
    
    // Inserting items
    wxString type = " " ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        s1 = s2 = "" ;
        type.SetChar ( 0 , v->items[a].getType() ) ;
        storage->sqlAdd ( s1 , s2 , "di_name" , v->items[a].name ) ;
        storage->sqlAdd ( s1 , s2 , "di_dna" , x ) ;
        storage->sqlAdd ( s1 , s2 , "di_description" , v->items[a].desc ) ;
        storage->sqlAdd ( s1 , s2 , "di_type" , type ) ;
        storage->sqlAdd ( s1 , s2 , "di_from" , v->items[a].from ) ;
        storage->sqlAdd ( s1 , s2 , "di_to" , v->items[a].to ) ;
        storage->sqlAdd ( s1 , s2 , "di_direction" , v->items[a].getDirection() ) ;
        storage->sqlAdd ( s1 , s2 , "di_params" , v->items[a].implodeParams() ) ;
        sql = "INSERT INTO dna_item (" + s1 + ") VALUES (" + s2 + ")" ;
        sr = storage->getObject ( sql ) ;
        }     

    v->undo.clear() ;
    wxFocusEvent fev ;
    MyChild *c = (MyChild*)GetParent() ;
    myass ( c , "Storage dialog has no parent" ) ;
    c->OnFocus(fev) ;
//    c->SetTitle ( c->vec->name ) ;
    if ( c->def == "dna" )
        {
        c->treeBox->initme() ;
        c->treeBox->SelectItem ( c->treeBox->vroot ) ;
        c->cSequence->arrange() ;
        c->cPlasmid->Refresh() ;
        myapp()->frame->mainTree->SetItemText ( c->inMainTree , c->vec->getName() ) ;
        }

    SetReturnCode ( wxID_OK ) ;
    EndModal ( true ) ;
    }
    
void TManageDatabaseDialog::pmOnRightClickLeft ( wxListEvent &ev )
    {
    pmOnRightClick ( ev , pm_left , pm_dd_l ) ;
    }
    
void TManageDatabaseDialog::pmOnRightClickRight ( wxListEvent &ev )
    {
    pmOnRightClick ( ev , pm_right , pm_dd_r ) ;
    }
    
void TManageDatabaseDialog::pmOnRightClick ( wxListEvent &ev , 
                                            wxListCtrl *lc , wxChoice *cc )
    {
    wxPoint p = ev.GetPoint() ;
    pmGetContext ( lc ) ;
    context_db = cc->GetStringSelection() ;
    wxMenu cm ;
    
    if ( isProject )
        {
        cm.Append ( MD_PM_OPEN , txt("t_open") ) ;
        if ( context_names.GetCount() == 1 )
           cm.Append ( MD_PM_REN , txt("t_rename") ) ;
        cm.Append ( MD_PM_DEL , txt("t_delete") ) ;
        }
    else
        {
        cm.Append ( MD_PM_OPEN , txt("t_open") ) ;
        if ( context_names.GetCount() == 1 )
           cm.Append ( MD_PM_REN , txt("t_rename") ) ;
        cm.Append ( MD_PM_DEL , txt("t_delete") ) ;
        }
    lc->PopupMenu ( &cm , p ) ;
    }
    
void TManageDatabaseDialog::pmOnRename ( wxCommandEvent &ev )
    {
    if ( context_names.GetCount() != 1 ) return ;
    wxString name = context_names[0] ;
    char t[1000] ;
    sprintf ( t , txt("t_rename_db_entry") , name.c_str() ) ;
    wxTextEntryDialog ted ( this , t , txt("t_rename") , name ) ;
    if ( ted.ShowModal() != wxID_OK ) return ;
    wxString newname = fixQuotes ( ted.GetValue() ) ;

    if ( isProject )
        {
        // Does the new name exist?
        TStorage *storage = getTempDB ( getFileName ( context_db ) ) ;
        TSQLresult sr ;
        wxString sql ;
        sql = "SELECT pr_name FROM project WHERE pr_name=\"" + newname + "\"" ;
        sr = storage->getObject ( sql ) ;
    
        if ( sr.rows() > 0 )
            {
            wxMessageDialog md ( this , txt("t_entry_exists_del_target") ,
                                    txt("t_entry_exists_brief") ,
                                    wxYES|wxNO ) ;
            if ( wxID_YES != md.ShowModal() ) return ;
            delProject ( newname , context_db ) ;
            }
        
        // Renaming
        sql = "UPDATE project SET pr_name=\"" + newname + "\" WHERE pr_name=\"" + name + "\"" ;
        storage->getObject ( sql ) ;
        sql = "UPDATE project_dna SET pd_project=\"" + newname + "\" WHERE pd_project=\"" + name + "\"" ;
        storage->getObject ( sql ) ;
        }
    else
        {
        // Does the new name exist?
        TStorage *storage = getTempDB ( getFileName ( context_db ) ) ;
        TSQLresult sr ;
        wxString sql ;
        sql = "SELECT dna_sequence FROM dna WHERE dna_name=\"" + newname + "\"" ;
        sr = storage->getObject ( sql ) ;
    
        if ( sr.rows() > 0 )
            {
            wxMessageDialog md ( this , txt("t_entry_exists_del_target") ,
                                    txt("t_entry_exists_brief") ,
                                    wxYES|wxNO ) ;
            if ( wxID_YES != md.ShowModal() ) return ;
            delDNA ( newname , context_db ) ;
            }
        
        // Renaming
        sql = "UPDATE dna SET dna_name=\"" + newname + "\" WHERE dna_name=\"" + name + "\"" ;
        storage->getObject ( sql ) ;
        sql = "UPDATE dna_item SET di_dna=\"" + newname + "\" WHERE di_dna=\"" + name + "\"" ;
        storage->getObject ( sql ) ;
        }
        
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;
    }
    
void TManageDatabaseDialog::delProject ( wxString name , wxString db )
    {
    wxString sql ;
    TStorage *storage = getTempDB ( getFileName ( db ) ) ;
    sql = "DELETE FROM project WHERE pr_name=\""+name+"\"" ;
    storage->getObject ( sql ) ;
    sql = "DELETE FROM project_dna WHERE pd_project=\""+name+"\"" ;
    storage->getObject ( sql ) ;    
    }
    
void TManageDatabaseDialog::pmOnOpen ( wxCommandEvent &ev )
    {
    if ( context_names.GetCount() == 0 ) return ;
    pmOpenFiles ( context_names , context_db ) ;
    }
    
void TManageDatabaseDialog::pmOnDelete ( wxCommandEvent &ev )
    {
    if ( context_names.GetCount() == 0 ) return ;
    if ( isProject )
        {
        wxMessageDialog md ( this , txt("t_deletion_warning_txt") ,
                                txt("t_deletion_warning") ,
                                wxYES|wxNO ) ;
        if ( wxID_YES != md.ShowModal() ) return ;
        for ( int a = 0 ; a < context_names.GetCount() ; a++ )
            delProject ( context_names[a] , context_db ) ;
        }
    else
        {
        wxMessageDialog md ( this , txt("t_deletion_warning_txt") ,
                                txt("t_deletion_warning") ,
                                wxYES|wxNO ) ;
        if ( wxID_YES != md.ShowModal() ) return ;
        for ( int a = 0 ; a < context_names.GetCount() ; a++ )
            delDNA ( context_names[a] , context_db ) ;
        }
        
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;
    }
    
void TManageDatabaseDialog::pmOnFilter ( wxCommandEvent &ev )
    {
    filter = filter_txt->GetValue() ;
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;    
    }
    

void TManageDatabaseDialog::pmOnFilterDNA ( wxCommandEvent &ev )
    {
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;    
    }
    
void TManageDatabaseDialog::pmOnFilterProtein ( wxCommandEvent &ev )
    {
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;    
    }

void TManageDatabaseDialog::pmOnFilterPrimer ( wxCommandEvent &ev )
    {
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;    
    }
    
void TManageDatabaseDialog::pmOnFilterDesc ( wxCommandEvent &ev )
    {
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;    
    }
    
void TManageDatabaseDialog::pmOnFilterSeq ( wxCommandEvent &ev )
    {
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;    
    }
    
void TManageDatabaseDialog::pmOnTwoPanes ( wxCommandEvent &ev )
    {
    updateTwoLists () ;
    }
    
void TManageDatabaseDialog::pmAddSQL ( wxCommandEvent &ev )
    {
    TMySQLDialog m ( this , "Add" ) ;
    if ( wxID_OK != m.ShowModal() ) return ;
    wxString fn ;
    fn = ":" + m.s->GetValue () ;
    fn += ":" + m.u->GetValue () ;
    fn += ":" + m.p->GetValue () ;
    fn += ":" + m.d->GetValue () ;
    addDatabase ( fn ) ;
    }

void TManageDatabaseDialog::pmNewSQL ( wxCommandEvent &ev )
    {
    }

// **************************************************************

TMyDropTarget::TMyDropTarget ( TManageDatabaseDialog *_d , wxListCtrl *_me )
    : wxTextDropTarget()
    {
    d = _d ;
    me = _me ;
    }

bool TMyDropTarget::OnDropText(int x, int y, const wxString &data)
    {
    return ( d->thetarget == me && data == GENTLE_DRAG_DATA ) ;
    }

// *****************************************************************

TMySQLDialog::TMySQLDialog ( wxWindow *parent , wxString title , wxString server ,
        wxString db , wxString user , wxString password )
        : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 200 , 180 ) )
   {
   wxBoxSizer *v = new wxBoxSizer ( wxVERTICAL ) ;
   wxFlexGridSizer *g = new wxFlexGridSizer ( 4 , 2 , 10 , 10 ) ;

   s = new wxTextCtrl ( this , -1 , server ) ;
   d = new wxTextCtrl ( this , -1 , db ) ;
   u = new wxTextCtrl ( this , -1 , user ) ;
   p = new wxTextCtrl ( this , -1 , password ) ;
   g->Add ( new wxStaticText ( this , -1 , "IP" ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( s , 1 , wxEXPAND , 5 ) ;
   g->Add ( new wxStaticText ( this , -1 , "DB" ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( d , 1 , wxEXPAND , 5 ) ;
   g->Add ( new wxStaticText ( this , -1 , "Name" ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( u , 1 , wxEXPAND , 5 ) ;
   g->Add ( new wxStaticText ( this , -1 , "PWD" ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( p , 1 , wxEXPAND , 5 ) ;
   
   v->Add ( g , 1 , wxEXPAND , 5 ) ;
   v->Add ( CreateButtonSizer ( wxOK | wxCANCEL ) , 0 , wxEXPAND , 5 ) ;
   
   SetSizer( v );
   Center () ;
   }


