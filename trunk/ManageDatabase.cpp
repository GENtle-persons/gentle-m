#include "ManageDatabase.h"

#define PM_LEFT 1
#define PM_RIGHT 2

BEGIN_EVENT_TABLE(TManageDatabaseDialog, wxDialog )
    EVT_BUTTON(MD_PM_NEW,TManageDatabaseDialog::pdOnNew)
    EVT_BUTTON(MD_PD_ADD,TManageDatabaseDialog::pdOnAdd)
    EVT_BUTTON(MD_PD_DEL,TManageDatabaseDialog::pdOnDel)
    EVT_BUTTON(MD_PD_DEFAULT,TManageDatabaseDialog::pdOnSetDefault)
    EVT_BUTTON(MD_PM_SAVE,TManageDatabaseDialog::pmOnSave)
    EVT_LISTBOX(MD_PD_DBLIST,TManageDatabaseDialog::pdOnDBchange)
    EVT_LIST_BEGIN_DRAG(MD_PM_LEFT,TManageDatabaseDialog::pmOnLeftListEvent)
    EVT_LIST_BEGIN_DRAG(MD_PM_RIGHT,TManageDatabaseDialog::pmOnRightListEvent)
    EVT_CHOICE(MD_PM_DD_L,TManageDatabaseDialog::pmOnLeftChoice)
    EVT_CHOICE(MD_PM_DD_R,TManageDatabaseDialog::pmOnRightChoice)
    
    EVT_CHECKBOX(MD_PM_FILTER_DNA,TManageDatabaseDialog::pmOnFilterDNA)
    EVT_CHECKBOX(MD_PM_FILTER_PROTEIN,TManageDatabaseDialog::pmOnFilterProtein)
    EVT_CHECKBOX(MD_PM_FILTER_PRIMER,TManageDatabaseDialog::pmOnFilterPrimer)

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
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 620 , 500 ) )
    {
    actionMode = mode ;
    f = myapp()->frame ;
    storage = NULL ;
    il = NULL ;
    thetarget = NULL ;
    v = _v ;
    
    doLoad = ( mode & ACTION_MODE_LOAD ) != 0 ;
    doSave = ( mode & ACTION_MODE_SAVE ) != 0 ;
    isProject = ( mode & ACTION_MODE_PROJECT ) != 0 ;
    
    int w , h ;
#ifdef __WXMSW__
    GetClientSize ( &w , &h ) ;
#else // LINUX
    w = 600 ;
    h = 480 ;
    GetClientSize ( &w , &h ) ;
#endif
    bo = 5 ;
    th = 22 ;
    
    nb = new wxNotebook ( (wxWindow*) this ,
                          -1 ,
                          wxPoint ( 0 , 0 ) ,
                          wxSize ( w , h ) ) ;

    pDatabases = new wxPanel ( nb , -1 ) ;
    pCopynMove = new wxPanel ( nb , -1 ) ;

    initDatabases () ;
    initCopynMove () ;
    
    nb->SetSelection ( 1 ) ;
    
    Center () ;
    }
    
TManageDatabaseDialog::~TManageDatabaseDialog ()
    {
    nb->DeleteAllPages() ;
    if ( storage ) delete storage ;
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

    
    int lbstyle = wxLC_LIST | wxLC_AUTOARRANGE | wxLC_SORT_ASCENDING ;
    pm_dd_l = new wxChoice ( p , MD_PM_DD_L , wxPoint ( bo , bo*2 + th ) , wxSize ( w/2-bo*2 , th ) ) ;
    pm_dd_r = new wxChoice ( p , MD_PM_DD_R , wxPoint ( bo+w/2 , bo*2 + th ) , wxSize ( w/2-bo*2 , th ) ) ;
    
    pm_left = new wxListCtrl ( p , MD_PM_LEFT , wxPoint ( bo , th*2+bo*2 ) ,
                                wxSize ( w/2-bo*2 , h-th*3-bo ) ,
                                lbstyle ) ;
    pm_right = new wxListCtrl ( p , MD_PM_RIGHT , wxPoint ( bo+w/2 , th*2+bo*2 ) ,
                                wxSize ( w/2-bo*2 , h-th*3-bo ) ,
                                lbstyle ) ;
    
    TMyDropTarget *rdt = new TMyDropTarget ( this , pm_right ) ;
    pm_right->SetDropTarget ( (wxDropTarget*) rdt ) ;
                                
    TMyDropTarget *ldt = new TMyDropTarget ( this , pm_left ) ;
    pm_left->SetDropTarget ( (wxDropTarget*) ldt ) ;



    wxBitmap bmp_helix ( myapp()->bmpdir+myapp()->slash+"Helix.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_project ( myapp()->bmpdir+myapp()->slash+"project.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_protein ( myapp()->bmpdir+myapp()->slash+"protein.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_primer ( myapp()->bmpdir+myapp()->slash+"primer.bmp" , wxBITMAP_TYPE_BMP ) ;

    il = new wxImageList ( 21 , 15 ) ;
    il->Add ( bmp_helix ) ;
    il->Add ( bmp_project ) ;
    il->Add ( bmp_protein ) ;
    il->Add ( bmp_primer ) ;
    
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
       }
    

    if ( !doSave )
        {
        pm_init_lists () ;
        filter_txt->SetFocus() ;
        return ;
        }
        
    string name ;
    if ( isProject ) name = myapp()->frame->project_name ;
    else name = v->name ;
    h += th ;
    int w2 = w/5 ;
    pm_dd_save = new wxChoice ( p , MD_PM_SAVE_DB ,
                                    wxPoint ( bo , h - th*2 + bo*2 ) ,
                                    wxSize ( w2 , th ) ) ;
    pm_name = new wxTextCtrl ( p , MD_PM_EDIT , name.c_str() , 
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
    
void TManageDatabaseDialog::pm_init_lists ()
    {
    int a ;
    pm_dd_l->Clear () ;
    pm_dd_r->Clear () ;
    if ( doSave )
        pm_dd_save->Clear () ;
    for ( a = 0 ; a < db_name.size() ; a++ )
        {
        pm_dd_l->Append ( db_name[a].c_str() ) ;
        pm_dd_r->Append ( db_name[a].c_str() ) ;
        if ( doSave )
           pm_dd_save->Append ( db_name[a].c_str() ) ;
        }

    pm_dd_l->SetStringSelection ( txt("local_db") ) ;
    pm_dd_r->SetStringSelection ( defdb.c_str() ) ;
    if ( doSave )
        {
        string db = defdb ;
        if ( isProject && myapp()->frame->project_db != "" )
           db = myapp()->frame->project_db ;
        pm_dd_save->SetStringSelection ( db.c_str() ) ;
        }

    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;
    pm_left->SetFocus() ;
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
    string name = c->GetStringSelection().c_str() ;
    TStorage st ( TEMP_STORAGE , getFileName ( name ) ) ;
    
    l->DeleteAllItems () ;
    TSQLresult r ;
    if ( isProject )
        {
        string sql = "SELECT pr_name FROM project" ;
        r = st.getObject ( sql ) ;        
        for ( a = 0 ; a < r.rows() ; a++ )
           {
           string s = r[a][r["pr_name"]] ;
           l->InsertItem ( a , s.c_str() , 1 ) ;
           }        
        }
    else
        {
        string sql = "SELECT dna_name,dna_type FROM dna" ;
//        if ( filter != "" )
           {
           for ( a = 0 ; a < filter.length() ; a++ )
              if ( filter[a] == '\"' ) filter[a] = '\'' ;
           vector <string> vf = explode ( " " , filter ) ;
           string sql2 , sql3 ;
           for ( a = 0 ; a < vf.size() ; a++ )
              {
              if ( vf[a] != "" )
                 {
                 if ( sql2 != "" ) sql2 += " AND" ;
                 sql2 += " dna_name LIKE \"%" + vf[a] + "%\"" ;
                 }
              }
           if ( f_dna->GetValue() )
              {
              if ( sql3 != "" ) sql3 += " OR " ;
              sql3 += "dna_type=0 OR dna_type=1 OR dna_type=2" ;
              }
           if ( f_prot->GetValue() )
              {
              if ( sql3 != "" ) sql3 += " OR " ;
              sql3 += "dna_type=5" ;
              }
           if ( f_primer->GetValue() )
              {
              if ( sql3 != "" ) sql3 += " OR " ;
              sql3 += "dna_type=3" ;
              }
           if ( sql3 != "" )
              {
              if ( sql2 != "" ) sql2 += " AND (" + sql3 + ")" ;
              else sql2 = " " + sql3 ;
              }
           if ( sql2 != "" ) sql += " WHERE" + sql2 ;
           }
        r = st.getObject ( sql ) ;        
        for ( a = 0 ; a < r.rows() ; a++ )
           {
           string s = r[a][r["dna_name"]] ;
           if ( r[a][r["dna_type"]] == "5" ) l->InsertItem ( a , s.c_str() , 2 ) ;
           else if ( r[a][r["dna_type"]] == "3" ) l->InsertItem ( a , s.c_str() , 3 ) ;
           else l->InsertItem ( a , s.c_str() , 0 ) ;
           }
        }
    }
    
vector <int> TManageDatabaseDialog::getSelectedListItems ( wxListCtrl *l )
    {
    long item = -1 ;
    vector <int> vi ;
    while ( true )
        {
        item = l->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) ;
        if ( item == -1 ) break;
        vi.push_back ( item ) ;
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
    vector <int> vi ;
    vector <string> vs ;
    vi = getSelectedListItems ( source ) ;
    for ( a = 0 ; a < vi.size() ; a++ )
        vs.push_back ( source->GetItemText ( vi[a] ).c_str() ) ;
    
    wxTextDataObject my_data(GENTLE_DRAG_DATA);
    wxDropSource dragSource( this );
	dragSource.SetData( my_data );
	wxDragResult result = dragSource.DoDragDrop( TRUE );
	if ( result != wxDragCopy && result != wxDragMove ) return ;

	string sdb = dds->GetStringSelection().c_str() ;
	string tdb = ddt->GetStringSelection().c_str() ;
	bool goOn = true ;
    for ( a = 0 ; goOn && a < vs.size() ; a++ )
        {
        if ( result == wxDragCopy ) goOn = do_copy ( vs[a] , sdb , tdb ) ;
        if ( result == wxDragMove ) goOn = do_move ( vs[a] , sdb , tdb ) ;
        }
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;
    }

bool TManageDatabaseDialog::do_copy ( string name , string sdb , string tdb )
    {
    if ( isProject ) return false ;
    else return copyDNA ( name , sdb , tdb ) ;
    }
    
bool TManageDatabaseDialog::do_move ( string name , string sdb , string tdb )
    {
    if ( isProject ) return false ;
    else return moveDNA ( name , sdb , tdb ) ;
    }
    
void TManageDatabaseDialog::do_del  ( string name , string db )
    {
    if ( isProject ) return ;
    else delDNA ( name , db ) ;
    }


bool TManageDatabaseDialog::copyDNA ( string name , string sdb , string tdb )
    {
    if ( sdb == tdb ) return false ;
    TStorage source ( TEMP_STORAGE , getFileName ( sdb ) ) ;
    TStorage target ( TEMP_STORAGE , getFileName ( tdb ) ) ;
    string sql , s , t ;

    int a ;
    TSQLresult r ;
    name = source.fixDNAname ( name ) ;
        
    // Already there?
    sql = "SELECT dna_name FROM dna WHERE dna_name=\""+name+"\"" ;
    r = target.getObject ( sql ) ;
    if ( r.rows() > 0 )
        {
        wxMessageDialog md ( this , txt("t_entry_exists_brief") , txt("msg_box") ) ;
        md.ShowModal() ;
        return false ;
        }
        
    // Copy DNA and items
    source.copySQLfields ( target , "dna" , "dna_name=\""+name+"\"" ) ;
    source.copySQLfields ( target , "dna_item" , "di_dna=\""+name+"\"" ) ;
    
    // Enzymes
    sql = "SELECT dna_restriction_enzymes FROM dna WHERE dna_name=\"" + name + "\"" ;
    r = source.getObject ( sql ) ;
    if ( r.rows() == 0 ) return false ;
    s = r[0][0] ;
    t = "" ;
    vector <string> ve ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( s[a] == ',' )
           {
           if ( t != "" ) ve.push_back ( t ) ;
           t = "" ;
           }
        else t += s[a] ;
        }
    if ( t != "" ) ve.push_back ( t ) ;

    for ( a = 0 ; a < ve.size() ; a++ )
        {
        sql = "SELECT e_name FROM enzyme WHERE e_name=\"" + ve[a] + "\"" ;
        r = target.getObject ( sql ) ;
        if ( r.rows() == 0 )
           source.copySQLfields ( target , "enzyme" , "e_name=\"" + ve[a] + "\"" ) ;
        }

	return true ;
    }

bool TManageDatabaseDialog::moveDNA ( string name , string sdb , string tdb )
    {
    bool doit = copyDNA ( name , sdb , tdb ) ;
    if ( doit ) delDNA ( name , sdb ) ;
    return doit ;
    }

void TManageDatabaseDialog::delDNA ( string name , string db )
    {
    TStorage source ( TEMP_STORAGE , getFileName ( db ) ) ;
    name = source.fixDNAname ( name ) ;
    source.getObject ( "DELETE FROM dna WHERE dna_name=\"" + name + "\"" ) ;
    source.getObject ( "DELETE FROM dna_item WHERE di_dna=\"" + name + "\"" ) ;
    }

string TManageDatabaseDialog::getFileName ( string dbname )
    {
    int a ;
    for ( a = 0 ; a < db_name.size() ; a++ )
        if ( db_name[a] == dbname )
           return db_file[a] ;
    return "" ;
    }
    
    
    
    
void TManageDatabaseDialog::initDatabases ()
    {
    wxPanel *p = pDatabases ;

    nb->AddPage ( p , txt("t_databases") ) ;
    int w , h ;
#ifdef __WXMSW__
    p->GetClientSize ( &w , &h ) ;
#else // LINUX
    GetClientSize ( &w , &h ) ;
    w -= 20 ;
    h -= 40 ;
#endif

    new wxStaticText ( p , -1 , txt("t_databases") , wxPoint ( bo , bo ) ) ;
    pd_db = new wxListBox ( p , MD_PD_DBLIST ,
                                wxPoint ( bo , th ) ,
                                wxSize ( w/3 , h - th ) ) ;
                                
    pd_db_name = new wxStaticText ( p , -1 , "" , 
                                    wxPoint ( w/3+bo , th*1 ) ,
                                    wxSize ( 500 , th ) ) ;
                                    
    pd_db_file = new wxStaticText ( p , -1 , "" ,
                                    wxPoint ( w/3+bo , th*2 ) ,
                                    wxSize ( 500 , th ) ) ;
                                    
    new wxButton ( p , MD_PD_ADD , txt("b_add") , 
                        wxPoint ( w/3+bo*2 , th*3 ) ,
                        wxSize ( w/5 , th ) ) ;
                        
    new wxButton ( p , MD_PD_DEL , txt("b_del") , 
                        wxPoint ( w/3+bo*3+w/5 , th*3 ) ,
                        wxSize ( w/5 , th ) ) ;

    new wxButton ( p , MD_PM_NEW , txt("b_new") , 
                        wxPoint ( w/3+bo*2 , th*4+bo ) ,
                        wxSize ( w/5 , th ) ) ;

    new wxButton ( p , MD_PD_DEFAULT , txt("b_set_default") , 
                        wxPoint ( w/3+bo*3+w/5 , th*4+bo ) ,
                        wxSize ( w/5 , th ) ) ;

    pd_loadList () ;
    pd_db->SetStringSelection ( defdb.c_str() ) ;
    accessDB () ;
    }
    
void TManageDatabaseDialog::accessDB ()
    {
    if ( storage ) delete storage ;
    int a ;
    string name , file ;
    name = pd_db->GetStringSelection().c_str() ;
    for ( a = 0 ; db_name[a] != name ; a++ ) ;
    file = db_file[a] ;
    storage = new TStorage ( TEMP_STORAGE , file ) ;
    storage->import() ;
    pd_db_name->SetTitle ( db_name[a].c_str() ) ;
    pd_db_file->SetTitle ( db_file[a].c_str() ) ;
    }

void TManageDatabaseDialog::pd_loadList ()
    {
    defdb = f->LS->getDatabaseList ( db_name , db_file ) ;
    int a ;
    pd_db->Clear () ;
    for ( a = 0 ; a < db_name.size() ; a++ )
        pd_db->Append ( db_name[a].c_str() ) ;
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

    string fn = d.GetPath().c_str() ;
    string blank = myapp()->homedir.c_str() ;
    blank += "\\blank.db" ;
    
    bool b = wxCopyFile ( blank.c_str() , fn.c_str() , true ) ;
    if ( !b )
        {
        wxMessageBox ( fn.c_str() , txt("t_db_already_exists") ) ;
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
    
    string fn = d.GetPath().c_str() ;
    addDatabase ( fn ) ;
    }
    
void TManageDatabaseDialog::addDatabase ( string fn )
    {
    int a , b ;
    for ( a = 0 ; a < db_file.size() && db_file[a] != fn ; a++ ) ;
    if ( a < db_file.size() ) // Already in the list
        {
        wxMessageDialog md ( this , txt("t_db_in_use") ) ;
        md.ShowModal() ;
        return ;
        }
    for ( a = b = 0 ; a < fn.length() ; a++ )
        if ( fn[a] == '\\' || fn[a] == '/' )
           b = a ;
    string t = fn.substr ( b+1 ) ;
    for ( a = b = 0 ; a < t.length() && b == 0 ; a++ )
        if ( t[a] == '.' )
           b = a ;
    t = t.substr ( 0 , b ) ;

    // Avoiding double names
    if ( t[0] >= 'a' && t[0] <= 'z' ) t[0] = t[0] - 'a' + 'A' ; // Uppercase
    string s ;
    char z[100] ;
    b = 0 ;
    *z = 0 ;
    do {
        s = t + z ;
        b++ ;
        sprintf ( z , " %d" , b ) ;
        for ( a = 0 ; a < db_name.size() && db_name[a] != s ; a++ ) ;
        } while ( a < db_name.size() ) ;
    t = s ;
    
    // Saving in database
    TSQLresult r ;
    string sql = "INSERT INTO stuff (s_type,s_name,s_value)"
                 "VALUES (\"DATABASE\",\"" + t + "\",\"" + fn + "\")" ;
    r = f->LS->getObject ( sql ) ;
    pd_loadList () ;
    pd_db->SetStringSelection ( t.c_str() ) ;
    pm_init_lists () ;
    }

void TManageDatabaseDialog::pdOnDel ( wxCommandEvent &ev )
    {
    string name = pd_db->GetStringSelection().c_str() ;
    if ( name == txt("local_db") || name == "" )
        {
        wxMessageDialog md ( this , txt("t_cannot_del_local_db") ) ;
        md.ShowModal() ;
        return ;
        }
    
    int a ;
    for ( a = 0 ; a < db_name.size() && db_name[a] != name ; a++ ) ;
    
    // Removing from database
    TSQLresult r ;
    string sql = "DELETE FROM stuff WHERE "
                 "s_type=\"DATABASE\" AND "
                 "s_name=\"" + name + "\" AND "
                 "s_value=\"" + db_file[a] + "\"" ;
    r = f->LS->getObject ( sql ) ;
    pd_loadList () ;
    pd_db->SetStringSelection ( txt("local_db") ) ;
    pm_init_lists () ;
    }
    
void TManageDatabaseDialog::pdOnSetDefault ( wxCommandEvent &ev )
    {
    string name = pd_db->GetStringSelection().c_str() ;
    if ( name == "" ) return ;
    defdb = name ;
    
    
    string sql = "DELETE FROM stuff WHERE s_type=\"DEFAULT_DATABASE\"" ;
    f->LS->getObject ( sql ) ;
    
    sql = "INSERT INTO stuff (s_type,s_name,s_value)"
                 "VALUES (\"DEFAULT_DATABASE\",\"" + defdb + "\",\"" + defdb + "\")" ;
    f->LS->getObject ( sql ) ;                 
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
    string s = side->GetItemText(i).c_str() ;
    pm_name->SetValue ( s.c_str() ) ;
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
    vector <int> vi ;
    while ( context_names.size() ) context_names.pop_back() ;
    vi = getSelectedListItems ( side ) ;
    for ( int a = 0 ; a < vi.size() ; a++ )
        context_names.push_back ( side->GetItemText ( vi[a] ).c_str() ) ;
    
    }

void TManageDatabaseDialog::pmOnActivate ( wxListEvent &ev , wxListCtrl *side )
    {
    wxChoice *c = (side==pm_left)?pm_dd_l:pm_dd_r;
    int i = ev.GetIndex () ;
    string t = c->GetStringSelection().c_str() ;
    pmGetContext ( side ) ;
    pmOpenFiles ( context_names , t ) ;
    }
    
void TManageDatabaseDialog::pmOpenFiles ( vector <string> &_names , string _db )
    {
    if ( _names.size() == 0 ) return ;
    for ( int a = 0 ; a < _names.size() ; a++ )
       do_load ( _names[a] , _db ) ;

    SetReturnCode ( wxID_OK ) ;
    EndModal ( true ) ;
    }

// --------------------------------

bool TManageDatabaseDialog::do_load ( string name , string db )
    {
    if ( isProject ) return do_load_project ( name , db ) ;
    else return do_load_DNA ( name , db ) ;
    }
    
bool TManageDatabaseDialog::do_load_project ( string name , string db )
    {
    int a ;
    string sql ;
    TStorage tstorage ( TEMP_STORAGE , getFileName ( db ) ) ;
    TSQLresult sr ;
    
    // Closing current windows
    wxCommandEvent dummy ;
    myapp()->frame->OnProjectClose ( dummy ) ;
    
    // Setting new project name, loading desc etc.
    name = fixQuotes ( name ) ;
    sql = "SELECT pr_desc FROM project WHERE pr_name=\""+name+"\"" ;
    sr = tstorage.getObject ( sql ) ;
    if ( sr.rows() == 0 ) return false ;
    myapp()->frame->project_name = name ;
    myapp()->frame->mainTree->SetItemText ( myapp()->frame->mainTree->treeroot , name.c_str() ) ;
    myapp()->frame->project_db = db ;
    myapp()->frame->project_desc = sr[0][0] ;
    
    
    // Load associated DNA list
    sql = "SELECT * FROM project_dna WHERE pd_project=\""+name+"\"" ;
    sr = tstorage.getObject ( sql ) ;
    
    // Load DNA
    bool all = true ;
    for ( a = 0 ; a < sr.rows() ; a++ )
        {
        string dna_name = sr[a][sr["pd_dna"]] ;
        string dna_db = sr[a][sr["pd_database"]] ;
        all &= do_load_DNA ( dna_name , dna_db ) ;
        }
    if ( !all )
        {
        wxMessageDialog md ( this , txt("t_not_all_files_loaded") ,
                                txt("msg_box") ) ;
        md.ShowModal() ;
        }
    
    return true ;
    }
    
bool TManageDatabaseDialog::do_load_DNA ( string name , string db )
    {
    int a ;
    string sql ;
    TStorage tstorage ( TEMP_STORAGE , getFileName ( db ) ) ;
    TSQLresult sr ;
    v = new TVector () ;

    if ( name == "" ) return false ;
    
    // Loading vector
    sql = "SELECT * FROM dna WHERE dna_name=\"" + name + "\"" ;
    sr = tstorage.getObject ( sql ) ;
    if( sr.rows() == 0 ) return false ;
    v->name = sr[0][sr["dna_name"]] ;
    v->desc = sr[0][sr["dna_description"]] ;
    v->sequence = sr[0][sr["dna_sequence"]] ;
    v->type = atoi ( sr[0][sr["dna_type"]].c_str() ) ;
    v->setStickyEnd ( true , true , sr[0][sr["dna_sticky_ul"]] ) ;
    v->setStickyEnd ( true , false , sr[0][sr["dna_sticky_ll"]] ) ;
    v->setStickyEnd ( false , true , sr[0][sr["dna_sticky_ur"]] ) ;
    v->setStickyEnd ( false , false , sr[0][sr["dna_sticky_lr"]] ) ;
/*    // To be deleted
    v->_ll = sr[0][sr["dna_sticky_ll"]] ;
    v->_ru = sr[0][sr["dna_sticky_ur"]] ;
    v->_rl = sr[0][sr["dna_sticky_lr"]] ;
*/
    v->setParams ( sr[0][sr["dna_params"]] ) ;
    if ( sr[0][sr["dna_circular"]] == "1" ) v->setCircular ( true ) ;
    else v->setCircular ( false ) ;
    v->setDatabase ( db ) ;
    v->removeBlanksFromSequence () ;

    string s = sr[0][sr["dna_restriction_enzymes"]] , t = "" ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( s[a] == ',' )
           {
           if ( t != "" )
              {
              if ( t[0] == '*' ) v->proteases.push_back ( t.substr ( 1 ) ) ;
              else v->re.push_back ( tstorage.getRestrictionEnzyme ( t ) ) ;
              }
           t = "" ;
           }
        else t += s[a] ;
        }
    if ( t != "" )
        {
        if ( t[0] == '*' ) v->proteases.push_back ( t.substr ( 1 ) ) ;
        else v->re.push_back ( tstorage.getRestrictionEnzyme ( t ) ) ;
        }

    // Loading items
    sql = "SELECT * FROM dna_item WHERE di_dna=\"" + name + "\"" ;
    sr = tstorage.getObject ( sql ) ;
    for ( a = 0 ; a < sr.rows() ; a++ )
        {
        TVectorItem i ;
        i.name = sr[a][sr["di_name"]].c_str() ;
        i.desc = sr[a][sr["di_description"]].c_str() ;
        i.type = sr[a][sr["di_type"]][0] ;
        i.from = atoi ( sr[a][sr["di_from"]].c_str() ) ;
        i.to = atoi ( sr[a][sr["di_to"]].c_str() ) ;
        i.direction = atoi ( sr[a][sr["di_direction"]].c_str() ) ;
        i.explodeParams ( sr[a][sr["di_params"]] ) ;
        v->items.push_back ( i ) ;
        }

    v->recalcvisual = true ;
    v->undo.clear() ;
    
    ChildBase *n = NULL ;
    if ( v->type == TYPE_AMINO_ACIDS )
        {
//        n = f->newAminoAcids ( v->sequence , v->name ) ;
        n = f->newAminoAcids ( v , v->name ) ;
        n->vec->setDatabase ( v->getDatabase() ) ;
        n->vec->setWindow ( n ) ;
        }
    else
        {
        v->recalculateCuts () ;
        n = f->newFromVector ( v , v->type ) ;
        }

    return true ;
    }
    
// REDUNDANT : TStorage::fixDNAname does the same
string TManageDatabaseDialog::fixQuotes ( string s )
    {
    int a ;
    for ( a = 0 ; a < s.length() ; a++ ) // Fixing quotes
        if ( s[a] == '"' ) s[a] = 39 ;
    return s ;
    }
    
bool TManageDatabaseDialog::doesNameExist ( string name , string dbname )
    {
    TStorage storage ( TEMP_STORAGE , getFileName ( dbname ) ) ;
    string sql ;
    TSQLresult sr ;
    string x = fixQuotes ( name ) ;
    if ( isProject )
        sql = "SELECT pr_name FROM project WHERE pr_name=\"" + x + "\"" ;
    else
        sql = "SELECT dna_sequence FROM dna WHERE dna_name=\"" + x + "\"" ;
    sr = storage.getObject ( sql ) ;
    if ( sr.content.size() == 0 ) return false ;
    
    string s , sc ;
    if ( isProject )
        {
        s = sr[0][sr["pr_name"]] ;
        sc = s ;
        }
    else
        {
        s = sr[0][sr["dna_sequence"]] ;
        sc = v->sequence ;
        }
    if ( s == sc )
        {
        wxMessageDialog md ( this , txt("t_entry_exists_but") , txt("msg_box") ,
                                wxYES|wxNO|wxNO_DEFAULT ) ;
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
    string name , dbname ;
    name = pm_name->GetValue().c_str() ;
    dbname = pm_dd_save->GetStringSelection().c_str() ;
    if ( myapp()->frame->project_name != name &&
         doesNameExist ( name , dbname ) ) return ;
    TStorage storage ( TEMP_STORAGE , getFileName ( dbname ) ) ;

    // New name, or overwriting old one
    int a ;
    string sql , s1 , s2 ;
    string x = fixQuotes ( name ) ;

    // New default project
    myapp()->frame->project_db = dbname ;

    // Deleting old one, if any
    sql = "DELETE FROM project WHERE pr_name=\""+x+"\"" ;
    storage.getObject ( sql ) ;
    sql = "DELETE FROM project_dna WHERE pd_project=\""+x+"\"" ;
    storage.getObject ( sql ) ;
    
    // Storing new one
    s1 = s2 = "" ;
    storage.sqlAdd ( s1 , s2 , "pr_name" , x ) ;
    storage.sqlAdd ( s1 , s2 , "pr_desc" , "No description" ) ;
    sql = "INSERT INTO project ("+s1+") VALUES ("+s2+")" ;
    storage.getObject ( sql ) ;
    
    for ( a = 0 ; a < myapp()->frame->children.size() ; a++ )
        {
        if ( myapp()->frame->children[a]->def == "dna" ||
             myapp()->frame->children[a]->def == "AminoAcids" )
           {
//           MyChild *c = (MyChild*) myapp()->frame->children[a] ;
           ChildBase *c = myapp()->frame->children[a] ;
           string dna_name = c->vec->name ;
           string dna_db = c->vec->getDatabase() ;
           if ( dna_db != "" )
              {
              s1 = s2 = "" ;
              storage.sqlAdd ( s1 , s2 , "pd_project" , x ) ;
              storage.sqlAdd ( s1 , s2 , "pd_dna" , dna_name ) ;
              storage.sqlAdd ( s1 , s2 , "pd_database" , dna_db ) ;
              sql = "INSERT INTO project_dna ("+s1+") VALUES ("+s2+")" ;
              storage.getObject ( sql ) ;
              }
            else
               {
               wxMessageDialog md ( this , "Not saved" , myapp()->frame->children[a]->def.c_str() ) ;
               md.ShowModal() ;
               }
           }
        }
    if ( myapp()->frame->project_name != x )
        {
        myapp()->frame->project_name = x ;
        myapp()->frame->mainTree->SetItemText ( myapp()->frame->mainTree->treeroot , x.c_str() ) ;
        }

    SetReturnCode ( wxID_OK ) ;
    EndModal ( true ) ;
    }
    
void TManageDatabaseDialog::do_save_DNA ()
    {
    string name , dbname ;
    name = pm_name->GetValue().c_str() ;
    dbname = pm_dd_save->GetStringSelection().c_str() ;
    if ( doesNameExist ( name , dbname ) ) return ;
    TStorage storage ( TEMP_STORAGE , getFileName ( dbname ) ) ;

    // New name, or overwriting old one
    int a ;
    TSQLresult sr ;
    string sql , s1 , s2 ;
    string x = fixQuotes ( name ) ;
    v->setDatabase ( dbname ) ; // Now in that database
    
    // Deleting old one, if any
    sql = "DELETE FROM dna WHERE dna_name=\""+x+"\"" ;
    sr = storage.getObject ( sql ) ;
    sql = "DELETE FROM dna_item WHERE di_dna=\""+x+"\"" ;
    sr = storage.getObject ( sql ) ;
    
    // Storing new one
    string enzymes = "," ;
    
    // Restriction enzymes
    for ( a = 0 ; a < v->re.size() ; a++ )
        enzymes += v->re[a]->name + "," ;
        
    // Proteases
    for ( a = 0 ; a < v->proteases.size() ; a++ )
        enzymes += "*" + v->proteases[a] + "," ;
        
    s1 = s2 = "" ;
    storage.sqlAdd ( s1 , s2 , "dna_name" , x ) ;
    storage.sqlAdd ( s1 , s2 , "dna_description" , v->desc ) ;
    storage.sqlAdd ( s1 , s2 , "dna_type" , v->type ) ;
    storage.sqlAdd ( s1 , s2 , "dna_sequence" , v->sequence ) ;
    storage.sqlAdd ( s1 , s2 , "dna_sticky_ul" , v->getStickyEnd(true,true) ) ;
    storage.sqlAdd ( s1 , s2 , "dna_sticky_ll" , v->getStickyEnd(true,false) ) ;
    storage.sqlAdd ( s1 , s2 , "dna_sticky_ur" , v->getStickyEnd(false,true) ) ;
    storage.sqlAdd ( s1 , s2 , "dna_sticky_lr" , v->getStickyEnd(false,false) ) ;
    storage.sqlAdd ( s1 , s2 , "dna_circular" , v->isCircular() ) ;
    storage.sqlAdd ( s1 , s2 , "dna_restriction_enzymes" , enzymes ) ;
    storage.sqlAdd ( s1 , s2 , "dna_params" , fixQuotes ( v->getParams() ) ) ;
    sql = "INSERT INTO dna (" + s1 + ") VALUES (" + s2 + ")" ;
    sr = storage.getObject ( sql ) ;
    v->name = x ;
    
    // Inserting items
    string type = " " ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        s1 = s2 = "" ;
        type[0] = v->items[a].type ;
        storage.sqlAdd ( s1 , s2 , "di_name" , v->items[a].name ) ;
        storage.sqlAdd ( s1 , s2 , "di_dna" , x ) ;
        storage.sqlAdd ( s1 , s2 , "di_description" , v->items[a].desc ) ;
        storage.sqlAdd ( s1 , s2 , "di_type" , type ) ;
        storage.sqlAdd ( s1 , s2 , "di_from" , v->items[a].from ) ;
        storage.sqlAdd ( s1 , s2 , "di_to" , v->items[a].to ) ;
        storage.sqlAdd ( s1 , s2 , "di_direction" , v->items[a].direction ) ;
        storage.sqlAdd ( s1 , s2 , "di_params" , v->items[a].implodeParams() ) ;
        sql = "INSERT INTO dna_item (" + s1 + ") VALUES (" + s2 + ")" ;
        sr = storage.getObject ( sql ) ;
        }     

    v->undo.clear() ;
    wxFocusEvent fev ;
    MyChild *c = (MyChild*)GetParent() ;
    c->OnFocus(fev) ;
//    c->SetTitle ( c->vec->name.c_str() ) ;
    if ( c->def == "dna" )
        {
        c->treeBox->initme() ;
        c->treeBox->SelectItem ( c->treeBox->vroot ) ;
        c->cSequence->arrange() ;
        c->cPlasmid->Refresh() ;
        myapp()->frame->mainTree->SetItemText ( c->inMainTree , c->vec->name.c_str() ) ;
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
    
/*    vector <int> vi ;
    while ( context_names.size() ) context_names.pop_back() ;
    vi = getSelectedListItems ( lc ) ;
    for ( int a = 0 ; a < vi.size() ; a++ )
        context_names.push_back ( lc->GetItemText ( vi[a] ).c_str() ) ;
    */
    pmGetContext ( lc ) ;

    context_db = cc->GetStringSelection().c_str() ;
    
    wxMenu cm ;
    
    if ( isProject )
        {
        cm.Append ( MD_PM_OPEN , txt("t_open") ) ;
        if ( context_names.size() == 1 )
           cm.Append ( MD_PM_REN , txt("t_rename") ) ;
        cm.Append ( MD_PM_DEL , txt("t_delete") ) ;
        }
    else
        {
        cm.Append ( MD_PM_OPEN , txt("t_open") ) ;
        if ( context_names.size() == 1 )
           cm.Append ( MD_PM_REN , txt("t_rename") ) ;
        cm.Append ( MD_PM_DEL , txt("t_delete") ) ;
        }
    lc->PopupMenu ( &cm , p ) ;
    }
    
void TManageDatabaseDialog::pmOnRename ( wxCommandEvent &ev )
    {
    if ( context_names.size() != 1 ) return ;
    string name = context_names[0] ;
    char t[1000] ;
    sprintf ( t , txt("t_rename_db_entry") , name.c_str() ) ;
    wxTextEntryDialog ted ( this , t , txt("t_rename") , name.c_str() ) ;
    if ( ted.ShowModal() != wxID_OK ) return ;
    string newname = fixQuotes ( ted.GetValue().c_str() ) ;

    if ( isProject )
        {
        // Does the new name exist?
        TStorage storage ( TEMP_STORAGE , getFileName ( context_db ) ) ;
        TSQLresult sr ;
        string sql ;
        sql = "SELECT pr_name FROM project WHERE pr_name=\"" + newname + "\"" ;
        sr = storage.getObject ( sql ) ;
    
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
        storage.getObject ( sql ) ;
        sql = "UPDATE project_dna SET pd_project=\"" + newname + "\" WHERE pd_project=\"" + name + "\"" ;
        storage.getObject ( sql ) ;
        }
    else
        {
        // Does the new name exist?
        TStorage storage ( TEMP_STORAGE , getFileName ( context_db ) ) ;
        TSQLresult sr ;
        string sql ;
        sql = "SELECT dna_sequence FROM dna WHERE dna_name=\"" + newname + "\"" ;
        sr = storage.getObject ( sql ) ;
    
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
        storage.getObject ( sql ) ;
        sql = "UPDATE dna_item SET di_dna=\"" + newname + "\" WHERE di_dna=\"" + name + "\"" ;
        storage.getObject ( sql ) ;
        }
        
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;
    }
    
void TManageDatabaseDialog::delProject ( string name , string db )
    {
    string sql ;
    TStorage storage ( TEMP_STORAGE , getFileName ( db ) ) ;
    sql = "DELETE FROM project WHERE pr_name=\""+name+"\"" ;
    storage.getObject ( sql ) ;
    sql = "DELETE FROM project_dna WHERE pd_project=\""+name+"\"" ;
    storage.getObject ( sql ) ;    
    }
    
void TManageDatabaseDialog::pmOnOpen ( wxCommandEvent &ev )
    {
    if ( context_names.size() == 0 ) return ;
    pmOpenFiles ( context_names , context_db ) ;
    }
    
void TManageDatabaseDialog::pmOnDelete ( wxCommandEvent &ev )
    {
    if ( context_names.size() == 0 ) return ;
    if ( isProject )
        {
        wxMessageDialog md ( this , txt("t_deletion_warning_txt") ,
                                txt("t_deletion_warning") ,
                                wxYES|wxNO ) ;
        if ( wxID_YES != md.ShowModal() ) return ;
        for ( int a = 0 ; a < context_names.size() ; a++ )
            delProject ( context_names[a] , context_db ) ;
        }
    else
        {
        wxMessageDialog md ( this , txt("t_deletion_warning_txt") ,
                                txt("t_deletion_warning") ,
                                wxYES|wxNO ) ;
        if ( wxID_YES != md.ShowModal() ) return ;
        for ( int a = 0 ; a < context_names.size() ; a++ )
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

