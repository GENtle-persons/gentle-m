#include "ManageDatabase.h"
#include <wx/filefn.h> //wxCopyFile

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
    EVT_CHECKBOX(MD_PM_FILTER_ALIGN,TManageDatabaseDialog::pmOnFilterAlignments)
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


TManageDatabaseDialog::TManageDatabaseDialog ( wxWindow *parent , const wxString& title , const int mode , TVector * const _v )
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 700 , 550 ) )
    {
    wxPrintf( "D: TManageDatabaseDialog::TManageDatabaseDialog - start\n" ) ;
    pm_dd_l = pm_dd_r = NULL ;
    filter_txt = NULL ;
    pm_name = NULL ;
    actionMode = mode ;
    il = NULL ;
    thetarget = NULL ;
    v = _v ;

    doLoad = ( mode & ACTION_MODE_LOAD ) != 0 ;
    doSave = ( mode & ACTION_MODE_SAVE ) != 0 ;
    isProject = ( mode & ACTION_MODE_PROJECT ) != 0 ;
    startup = ( mode & ACTION_MODE_STARTUP ) != 0 ;
    justload = false ;
    silent_overwrite = false ;

    if ( isProject ) myapp()->frame->push_help ( _T("GENtle:Projects") ) ;
    else myapp()->frame->push_help ( _T("GENtle:Databases") ) ;

    int w , h ;
    GetClientSize ( &w , &h ) ;

    wxClientDC dc ( (wxWindow*) this ) ;
    dc.GetTextExtent ( "X" , &bo , &th ) ;
    th = th * 3 / 2 ;
    bo = th / 4 ;
    nb = new wxNotebook ( (wxWindow*) this , -1 , wxPoint ( 0 , 0 ) , wxSize ( w , h ) ) ;

    pDatabases = new wxPanel ( nb , -1 ) ;
    pCopynMove = new wxPanel ( nb , -1 ) ;

    wxBeginBusyCursor() ;
    initDatabases () ;
    initCopynMove () ;

    if ( startup )
        {
        f_twopanes->SetValue ( 0 ) ;
        }
    else if ( doLoad || doSave )
        {
        int tp = myapp()->frame->LS->getOption ( _T("TWOPANES") , 0 ) ;
        f_twopanes->SetValue ( tp ) ;
        }
    else
        {
        f_twopanes->SetValue ( 1 ) ;
        }
    updateTwoLists () ;

    nb->SetSelection ( 1 ) ;

    if ( !doSave && filter_txt ) filter_txt->SetFocus() ;
    else if ( pm_name ) pm_name->SetFocus() ;

    Center () ;
    wxEndBusyCursor() ;
    wxPrintf( "D: TManageDatabaseDialog::TManageDatabaseDialog - end\n" ) ;
    }

TManageDatabaseDialog::~TManageDatabaseDialog ()
    {
    wxPrintf( "D: TManageDatabaseDialog::~TManageDatabaseDialog - start\n" ) ;
    nb->DeleteAllPages() ;
    if ( il ) delete il ;
    wxPrintf( "D: TManageDatabaseDialog::~TManageDatabaseDialog - end\n" ) ;
    }

void TManageDatabaseDialog::initCopynMove ()
    {
    wxPrintf( "D: TManageDatabaseDialog::initCopynMove - start\n" ) ;
    v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;
    v2 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h4 = new wxBoxSizer ( wxHORIZONTAL ) ;

    wxPanel *p = pCopynMove ;

    nb->AddPage ( p , txt("t_management") ) ;

    filter_txt = new wxTextCtrl ( p , MD_PM_FILTER , "" ) ;

    f_twopanes = new wxCheckBox ( p , MD_PM_TWOPANES , txt("t_twopanes") ) ;

    int lbstyle = wxLC_LIST | wxLC_AUTOARRANGE | wxLC_SORT_ASCENDING ;
    pm_dd_l = new wxChoice ( p , MD_PM_DD_L ) ;
    pm_dd_r = new wxChoice ( p , MD_PM_DD_R ) ;

    pm_left  = new wxListCtrl ( p , MD_PM_LEFT ,  wxDefaultPosition , wxDefaultSize , lbstyle ) ;
    pm_right = new wxListCtrl ( p , MD_PM_RIGHT , wxDefaultPosition , wxDefaultSize , lbstyle ) ;

    TMyDropTarget *rdt = new TMyDropTarget ( this , pm_right ) ;
    pm_right->SetDropTarget ( (wxDropTarget*) rdt ) ;

    TMyDropTarget *ldt = new TMyDropTarget ( this , pm_left ) ;
    pm_left->SetDropTarget ( (wxDropTarget*) ldt ) ;

    wxBitmap bmp_helix ( myapp()->bmpdir.GetFullPath() + wxFileName::GetPathSeparator() + "Helix.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_project ( myapp()->bmpdir.GetFullPath() + wxFileName::GetPathSeparator() + "project.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_protein ( myapp()->bmpdir.GetFullPath() + wxFileName::GetPathSeparator() + "protein.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_primer ( myapp()->bmpdir.GetFullPath() + wxFileName::GetPathSeparator() + "primer.bmp" , wxBITMAP_TYPE_BMP ) ;
    wxBitmap bmp_align ( myapp()->bmpdir.GetFullPath() + wxFileName::GetPathSeparator() + "alignment.bmp" , wxBITMAP_TYPE_BMP ) ;

    il = new wxImageList ( 21 , 15 ) ;
    il->Add ( bmp_helix ) ;
    il->Add ( bmp_project ) ;
    il->Add ( bmp_protein ) ;
    il->Add ( bmp_primer ) ;
    il->Add ( bmp_align ) ;

    pm_left->SetImageList ( il , wxIMAGE_LIST_SMALL ) ;
    pm_right->SetImageList ( il , wxIMAGE_LIST_SMALL ) ;

    h1->Add (  new wxStaticText ( p , -1 , txt("t_filter") ) , 0 , wxEXPAND , 2 ) ;
    h1->Add ( filter_txt , 1 , wxEXPAND , 2 ) ;

    h2->Add ( f_twopanes , 0 , wxEXPAND , 2 ) ;

    if ( !isProject )
        {
        f_dna = new wxCheckBox ( p , MD_PM_FILTER_DNA , txt("dna") ) ;
        f_prot = new wxCheckBox ( p , MD_PM_FILTER_PROTEIN , txt("protein") ) ;
        f_primer = new wxCheckBox ( p , MD_PM_FILTER_PRIMER , txt("primers") ) ;
        f_align = new wxCheckBox ( p , MD_PM_FILTER_ALIGN , txt("alignments") ) ;
        f_desc = new wxCheckBox ( p , MD_PM_FILTER_DESC , txt("desc") ) ;
        f_seq = new wxCheckBox ( p , MD_PM_FILTER_SEQ , txt("sequences") ) ;
        f_desc->SetValue ( 1 ) ;

        h2->Add ( new wxStaticText ( p , -1 , "" ) , 1 , wxEXPAND , 2 ) ; // Dummy to shove the rest to the right
        h2->Add ( f_dna , 0 , wxEXPAND , 2 ) ;
        h2->Add ( f_prot , 0 , wxEXPAND , 2 ) ;
        h2->Add ( f_primer , 0 , wxEXPAND , 2 ) ;
        h2->Add ( f_align , 0 , wxEXPAND , 2 ) ;
        h2->Add ( f_desc , 0 , wxEXPAND , 2 ) ;
        h2->Add ( f_seq , 0 , wxEXPAND , 2 ) ;
        }

    v1->Add ( pm_dd_l , 0 , wxEXPAND , 2 ) ;
    v1->Add ( pm_left , 1 , wxEXPAND , 2 ) ;
    v2->Add ( pm_dd_r , 0 , wxEXPAND , 2 ) ;
    v2->Add ( pm_right , 1 , wxEXPAND , 2 ) ;
    h3->Add ( v1 , 1 , wxEXPAND , 2 ) ;
    h3->Add ( v2 , 1 , wxEXPAND , 2 ) ;

    v0->Add ( h1 , 0 , wxEXPAND , 2 ) ;
    v0->Add ( h2 , 0 , wxEXPAND , 2 ) ;
    v0->Add ( h3 , 1 , wxEXPAND , 2 ) ;

    if ( !doSave )
        {
        pm_init_lists () ;
        filter_txt->SetFocus() ;
        }
    else
        {
        wxString name ;
        if ( isProject ) name = myapp()->frame->project.name ;
        else name = v->getName() ;
        pm_dd_save = new wxChoice ( p , MD_PM_SAVE_DB ) ;
        pm_name = new wxTextCtrl ( p , MD_PM_EDIT , name ) ;
        wxButton *sb = new wxButton ( p , MD_PM_SAVE , txt("b_store") ) ;
        pm_init_lists () ;
        sb->SetDefault () ;
        pm_name->SetSelection ( -1 , -1 ) ;
        pm_name->SetFocus() ;
        h4->Add ( pm_dd_save , 0 , wxEXPAND , 2 ) ;
        h4->Add ( pm_name , 1 , wxEXPAND , 2 ) ;
        h4->Add ( sb , 0 , wxEXPAND , 2 ) ;
        v0->Add ( h4 , 0 , wxEXPAND , 2 ) ;
        }

    p->SetSizer ( v0 ) ;
    p->Layout () ;
    wxPrintf( "D: TManageDatabaseDialog::initCopynMove - end\n" ) ;
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
        v0->Show ( v2 , true , true ) ;
        v0->Layout () ;
        pm_list_items ( PM_RIGHT ) ;
        }
    else
        {
        v0->Show ( v2 , false , true ) ;
        v0->Layout () ;
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
        if ( isProject && !myapp()->frame->project.db.IsEmpty() )
            {
            db = myapp()->frame->project.db ;
            }
        else if ( !isProject )
            {
            db = v->getDatabase() ;
            }
        if ( db.IsEmpty() )
            {
            db = defdb ;
            }
        pm_dd_save->SetStringSelection ( db ) ;
        }

    pm_list_items ( PM_LEFT ) ;
//  pm_list_items ( PM_RIGHT ) ;
    pm_left->SetFocus() ;
    }

TStorage *TManageDatabaseDialog::getTempDB ( const wxString& filename ) const
    {
    return myapp()->frame->getTempDB ( filename ) ;
    }

void TManageDatabaseDialog::pm_list_items ( const int x )
    {
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
    wxString name = c ? c->GetStringSelection() : "" ;
    TStorage *st = getTempDB ( getFileName ( name ) ) ;

    l->DeleteAllItems () ;
    TSQLresult r ;
    if ( isProject )
        {
        wxString sql = "SELECT pr_name FROM project" ;
        r = st->getObject ( sql ) ;
        for ( int a = 0 ; a < r.rows() ; a++ )
            {
            wxString s = r[a][r["pr_name"]] ;
            l->InsertItem ( a , s , 1 ) ;
            }
        }
    else
        {
        wxString sql = "SELECT dna_name,dna_type FROM dna" ;

        // Filtering
        for ( int a = 0 ; a < filter.length() ; a++ )
            {
            if ( filter.GetChar(a) == '\"' )
                {
                filter.SetChar ( a , '\'' ) ;
                }
            }
        wxArrayString vf ;
        explode ( " " , filter , vf ) ;
        wxString sql2 , sql3 ;
        for ( int a = 0 ; a < vf.GetCount() ; a++ )
            {
            if ( !vf[a].IsEmpty() )
                {
                if ( !sql2.IsEmpty() )
                    {
                    sql2 += " AND" ;
                    }
                sql2 += " (dna_name LIKE \"%" + vf[a] + "%\"" ;
                if ( f_desc->GetValue() )
                    {
                    sql2 += " OR dna_description LIKE \"%" + vf[a] + "%\"" ;
                    }
                if ( f_seq->GetValue() )
                    {
                    sql2 += " OR dna_sequence LIKE \"%" + vf[a] + "%\"" ;
                    }
                sql2 += ")" ;
                }
            }
        if ( f_dna->GetValue() ) // DNA
            {
            if ( !sql3.IsEmpty() ) sql3 += " OR " ;
            sql3 += "dna_type=0 OR dna_type=1 OR dna_type=2" ;
            }
        if ( f_prot->GetValue() ) // Protein
            {
            if ( !sql3.IsEmpty() ) sql3 += " OR " ;
            sql3 += "dna_type=5" ;
            }
        if ( f_primer->GetValue() ) // Primer
            {
            if ( !sql3.IsEmpty() ) sql3 += " OR " ;
            sql3 += "dna_type=3" ;
            }
        if ( f_align->GetValue() ) // Alignments
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

        r = st->getObject ( sql ) ;

        for ( int a = 0 ; a < r.rows() ; a++ )
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

wxArrayInt TManageDatabaseDialog::getSelectedListItems ( const wxListCtrl * const l ) const
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

    wxArrayInt vi ;
    wxArrayString vs ;
    vi = getSelectedListItems ( source ) ;
    for ( int a = 0 ; a < vi.GetCount() ; a++ )
        {
        vs.Add ( source->GetItemText ( vi[a] ) ) ;
        }

    wxTextDataObject my_data(GENTLE_DRAG_DATA);
    wxDropSource dragSource( this );
    dragSource.SetData( my_data );
    wxDragResult result = dragSource.DoDragDrop( TRUE );
    if ( result != wxDragCopy && result != wxDragMove )
        {
        return ;
        }

    wxString sdb = dds->GetStringSelection() ;
    wxString tdb = ddt->GetStringSelection() ;
    bool goOn = true ;
    for ( int a = 0 ; goOn && a < vs.GetCount() ; a++ )
        {
        if ( result == wxDragCopy ) goOn = do_copy ( vs[a] , sdb , tdb ) ;
        if ( result == wxDragMove ) goOn = do_move ( vs[a] , sdb , tdb ) ;
        }
    pm_list_items ( PM_LEFT ) ;
    pm_list_items ( PM_RIGHT ) ;
    }

bool TManageDatabaseDialog::do_copy ( const wxString& name , const wxString& sdb , const wxString& tdb )
    {
    wxPrintf( "D: TManageDatabaseDialog::do_copy (%a, db, tdb)\n" ) ;
    if ( isProject ) return false ;
    else return copyDNA ( name , sdb , tdb ) ;
    }

bool TManageDatabaseDialog::do_move ( const wxString& name , const wxString& sdb , const wxString& tdb )
    {
    if ( isProject ) return false ;
    else return moveDNA ( name , sdb , tdb ) ;
    }

void TManageDatabaseDialog::do_del  ( const wxString& name , const wxString& db )
    {
    if ( isProject )
        {
        return ;
        }
    else
        {
        delDNA ( name , db ) ;
        }
    }


bool TManageDatabaseDialog::copyDNA ( const wxString& _name , const wxString& sdb , const wxString& tdb )
    {
    if ( sdb == tdb ) return false ;
    SetCursor ( *wxHOURGLASS_CURSOR ) ;
    TStorage *source = getTempDB ( getFileName ( sdb ) ) ;
    TStorage *target = getTempDB ( getFileName ( tdb ) ) ;
    const wxString name = source->fixDNAname ( _name ) ;

    TSQLresult r ;

    // Already there?
        {
        const wxString sql = "SELECT dna_name FROM dna WHERE dna_name=\"" + name + "\"" ;
        r = target->getObject ( sql ) ;
        if ( r.rows() > 0 )
            {
            SetCursor ( *wxSTANDARD_CURSOR ) ;
            wxMessageDialog md ( this , txt("t_entry_exists_brief") , txt("msg_box") ) ;
            md.ShowModal() ;
            return false ;
            }
        }
    // Copy DNA and items
    source->copySQLfields ( *target , "dna" , "dna_name=\"" + name + "\"" ) ;
    source->copySQLfields ( *target , "dna_item" , "di_dna=\"" + name + "\"" ) ;

    // Enzymes
        {
        const wxString sql = "SELECT dna_restriction_enzymes FROM dna WHERE dna_name=\"" + name + "\"" ;
        r = source->getObject ( sql ) ;
        if ( r.rows() == 0 )
            {
            SetCursor ( *wxSTANDARD_CURSOR ) ;
            wxPrintf("I: TManageDatabaseDialog::copyDNA: ret - No enzymes found for '%s'\n" , name ) ;
            return false ;
            }
        }

    const wxString s = r[0][0] ;
    wxString t = "" ;
    wxArrayString ve ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        if ( s.GetChar(a) == ',' )
            {
            if ( !t.IsEmpty() ) ve.Add ( t ) ;
            t = "" ;
            }
        else t += s.GetChar(a) ;
        }
    if ( !t.IsEmpty() ) ve.Add ( t ) ;

    for ( int a = 0 ; a < ve.GetCount() ; a++ )
        {
        const wxString sql = "SELECT e_name FROM enzyme WHERE e_name=\"" + ve[a] + "\"" ;
        r = target->getObject ( sql ) ;
        if ( r.rows() == 0 )
           source->copySQLfields ( *target , "enzyme" , "e_name=\"" + ve[a] + "\"" ) ;
        }

    SetCursor ( *wxSTANDARD_CURSOR ) ;
    return true ;
    }

bool TManageDatabaseDialog::moveDNA ( const wxString& name , const wxString& sdb , const wxString& tdb )
    {
    bool doit = copyDNA ( name , sdb , tdb ) ;
    if ( doit ) delDNA ( name , sdb ) ;
    return doit ;
    }

void TManageDatabaseDialog::delDNA ( const wxString& _n, const wxString& db )
    {
    TStorage * const source = getTempDB ( getFileName ( db ) ) ;
    const wxString name = source->fixDNAname ( _n ) ;
    source->getObject ( "DELETE FROM dna WHERE dna_name=\"" + name + "\"" ) ;
    source->getObject ( "DELETE FROM dna_item WHERE di_dna=\"" + name + "\"" ) ;
    }

wxString TManageDatabaseDialog::getFileName ( const wxString& dbname ) const
    {
    for ( int a = 0 ; a < db_name.GetCount() ; a++ )
        if ( db_name[a] == dbname )
           return db_file[a] ;
    wxPrintf("I: TManageDatabaseDialog::getFileName: Could not find database '%s'\n" , dbname ) ;
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
#ifdef USEMYSQL
    g1->Add ( new wxButton ( p , MD_PD_ADD_MYSQL , txt("b_add") ) , 1 , wxEXPAND , 5 ) ;
    g1->Add ( new wxButton ( p , MD_PM_NEW_MYSQL , txt("b_new") ) , 1 , wxEXPAND , 5 ) ;
#endif

    pd_db_name = new wxStaticText ( p , -1 , "" ) ;
    pd_db_file = new wxStaticText ( p , -1 , "" ) ;
    v1->Add ( new wxStaticText ( p , -1 , txt("t_databases") ) , 0 , wxEXPAND , 5 ) ;
    v1->Add ( new wxStaticText ( p , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v1->Add ( pd_db_name , 0 , wxEXPAND , 5 ) ;
    v1->Add ( pd_db_file , 0 , wxEXPAND , 5 ) ;
    v1->Add ( new wxStaticText ( p , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v1->Add ( g1 , 1 , wxEXPAND , 5 ) ;

    pd_db = new wxListBox ( p , MD_PD_DBLIST ) ;
    h1->Add ( pd_db , 1 , wxEXPAND , 5 ) ;
    h1->Add ( v1 , 1 , wxEXPAND , 5 ) ;

    pd_loadList () ;
    pd_db->SetStringSelection ( txt("local_db") ) ;
    accessDB () ;

    p->SetSizer ( h1 ) ;
    p->Layout ( ) ;

    }

void TManageDatabaseDialog::accessDB ()
    {
    wxString name = pd_db->GetStringSelection() ;

    int a = 0 ;
    while ( db_name[a] != name )
        {
        a++ ;
        }

    if ( db_name[a] != name )
        {
        wxPrintf("E: TManageDatabaseDialog::accessDB: db_name[a] != name = '%s'\n" , name ) ;
        exit( 1 ) ;
        }

    wxString file = db_file[a] ;
    pd_db_name->SetLabel ( wxString::Format ( txt("t_db_name") , db_name[a].c_str() ) ) ;
    pd_db_file->SetLabel ( wxString::Format ( txt("t_db_location") , db_file[a].c_str() ) ) ;
    }

void TManageDatabaseDialog::pd_loadList ()
    {
    defdb = myapp()->frame->LS->getDatabaseList ( db_name , db_file ) ;
    pd_db->Clear () ;
    for ( int a = 0 ; a < db_name.GetCount() ; a++ )
        pd_db->Append ( db_name[a] ) ;
    }

// Event handlers

void TManageDatabaseDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }

void TManageDatabaseDialog::OnOK ( wxCommandEvent &ev )
    {
    myapp()->frame->pop_help () ;
    EndModal ( wxID_OK ) ; //wxDialog::OnOK ( ev ) ;
    }

void TManageDatabaseDialog::OnCancel ( wxCommandEvent &ev )
    {
    myapp()->frame->pop_help () ;
    EndModal ( wxID_CANCEL ) ; //wxDialog::OnCancel ( ev ) ;
    }

void TManageDatabaseDialog::pdOnDBchange ( wxCommandEvent &ev )
    {
    accessDB () ;
    }

void TManageDatabaseDialog::pdOnNew ( wxCommandEvent &ev )
    {
    const wxString wildcard = "GENtle database (*.db)|*.db" ;
    wxFileDialog d ( this , txt("t_add_new_db") , "" , "" , wildcard , wxFD_SAVE|wxFD_OVERWRITE_PROMPT ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;

    const wxString fn = d.GetPath() ;
    const wxString blank = myapp()->homedir.GetFullPath() + wxFileName::GetPathSeparator() + "blank.db" ;

    const bool b = wxCopyFile ( blank , fn , true ) ;
    if ( !b )
        {
        wxMessageBox ( fn , txt("t_db_already_exists") ) ;
        return ;
        }

    addDatabase ( fn ) ;
    }

void TManageDatabaseDialog::pdOnAdd ( wxCommandEvent &ev )
    {
    const wxString wildcard = "GENtle database (*.db)|*.db" ;
    wxFileDialog d ( this , txt("t_choose_db") , "" , "" , wildcard , wxFD_OPEN ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;

    const wxString fn = d.GetPath() ;
    if ( !wxFileExists ( fn ) )
       {
       wxPrintf("I: TManageDatabaseDialog::pdOnAdd: File '%s' does not exist. Not adding database.\n" , fn ) ;
       return ;
       }
    addDatabase ( fn ) ;
    }

void TManageDatabaseDialog::addDatabase ( const wxString& fn )
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
        {
        t.SetChar ( 0 , t.GetChar(0) - 'a' + 'A' ) ; // Uppercase
        }
    wxString s = t ;
    b = 1 ;
    do {

        if ( b > 1 )
            {
            s = t + wxString::Format ( " (%d)" , b ) ;
            }
        b++ ;
        for ( a = 0 ; a < db_name.GetCount() && db_name[a] != s ; a++ ) ;

        } while ( a < db_name.GetCount() ) ;
    t = s ;

    // Saving in database
    const wxString sql = "INSERT INTO stuff (s_type,s_name,s_value) VALUES (\"DATABASE\",\"" + t + "\",\"" + fn + "\")" ;
    TSQLresult r = myapp()->frame->LS->getObject ( sql ) ;
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

    int namePos = 0 ;
    while ( namePos < db_name.GetCount() && db_name[namePos] != name )
        {
        namePos++ ;
        }
    if ( namePos >= db_name.GetCount() )
        {
        wxPrintf("E: TManageDatabaseDialog::pdOnDel: Could not find name '%s' among db_name entries.\n", name ) ;
        return ;
        }

    // Removing from database
    TSQLresult r ;
    wxString sql = "DELETE FROM stuff WHERE s_type=\"DATABASE\" AND s_name=\"" + name + "\" AND s_value=\"" + db_file[namePos] + "\"" ;
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

    sql = "INSERT INTO stuff (s_type,s_name,s_value) VALUES (\"DEFAULT_DATABASE\",\"" + defdb + "\",\"" + defdb + "\")" ;
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
    //int i = ev.GetIndex () ;
    wxString t = c->GetStringSelection() ;
    pmGetContext ( side ) ;
    pmOpenFiles ( context_names , t ) ;
    }

void TManageDatabaseDialog::pmOpenFiles ( const wxArrayString & _names , const wxString& _db )
    {
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - start - _names.Count()=%ld\n" , _names.Count() ) ;
    if ( _names.GetCount() == 0 )
        {
        wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - start-check - _names.GetCount() == 0 returning without ending modal.\n" ) ;
        return ;
        }
    myapp()->frame->pop_help () ;
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - A\n" ) ;
    if ( doLoad || doSave )
        {
        wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - A2 - doLoad || doSave\n" ) ;
        myapp()->frame->LS->setOption ( _T("TWOPANES") , f_twopanes->GetValue() ) ;
        }

    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - B1 - setting return code to wxID_OK\n" ) ;
    SetReturnCode ( wxID_OK ) ;
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - B2 - ending modal\n" ) ;
    EndModal ( true ) ;

    wxBeginBusyCursor () ;
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - C\n" ) ;
    //myapp()->frame->lockDisplay ( true ) ;
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - D\n" ) ;

    //wxProgressDialog pd ( txt("t_loading") , "" , _names.GetCount() , NULL , wxPD_ALL ) ;
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - E\n" ) ;
    for ( int a = 0 ; a < _names.GetCount() ; a++ )
        {
        /*
        wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - F - update progress box\n" ) ;
        if ( !pd.Update ( a , _names[a] ) )
            {
            wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - F - break\n" ) ;
            break ;
            }
        */
        wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - G - invoking do_load(%s,%s)\n" , _names[a] , _db ) ;
        do_load ( _names[a] , _db ) ;
        wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - H\n" ) ;
        }
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - I - EndBusyCursor\n" ) ;
    wxEndBusyCursor () ;

    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - J - UnlockDisplay\n" ) ;
    //myapp()->frame->lockDisplay ( false ) ;
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - K - MainTreeRefresh\n" ) ;
    myapp()->frame->mainTree->Refresh () ;
    wxPrintf( "D: TManageDatabaseDialog::pmOpenFiles - end\n" ) ;
    }

// --------------------------------

bool TManageDatabaseDialog::do_load ( const wxString& name , const wxString& db )
    {
    wxPrintf( "D: TManageDatabaseDialog::do_load (%s,%s)\n" , name, db ) ;
    if ( isProject ) return do_load_project ( name , db ) ;
    else return do_load_DNA ( name , db ) ;
    }

bool TManageDatabaseDialog::do_load_project ( const wxString& _n , const wxString& db )
    {
    int a ;
    wxString sql ;
    TStorage *tstorage = getTempDB ( getFileName ( db ) ) ;
    TSQLresult sr ;

    //MyFrame *f = myapp()->frame ;
    //int oc = f->children.GetCount() ;

    // Closing current windows
    wxCommandEvent dummy ;
    myapp()->frame->OnProjectClose ( dummy ) ;

    // Setting new project name, loading desc etc.
    wxString name = fixQuotes ( _n ) ;
    sql = "SELECT pr_desc FROM project WHERE pr_name=\"" + name + "\"" ;
    sr = tstorage->getObject ( sql ) ;
    if ( sr.rows() == 0 ) return false ;

    myapp()->frame->project.name = name ;
    myapp()->frame->mainTree->SetItemText ( myapp()->frame->mainTree->treeroot , name ) ;
    myapp()->frame->project.db = db ;
    myapp()->frame->project.desc = sr[0][0] ;

    myapp()->frame->SetFocus() ;
    wxBeginBusyCursor() ;
    myapp()->frame->lockDisplay ( true ) ;

    // Load associated DNA list
    sql = "SELECT * FROM project_dna WHERE pd_project=\"" + name + "\"" ;
    sr = tstorage->getObject ( sql ) ;

    // Load DNA
    bool all = true ;
    wxProgressDialog pd ( txt("t_loading") , "" , sr.rows() , NULL , wxPD_ALL ) ;
    for ( a = 0 ; a < sr.rows() ; a++ )
        {
        if ( !pd.Update ( a , sr[a][sr["pd_dna"]] ) )
            break ;
        all &= do_load_DNA ( sr[a][sr["pd_dna"]] , sr[a][sr["pd_database"]] ) ;
        }
    if ( a == sr.rows() ) pd.Update ( a ) ; // Close dialog

    myapp()->frame->lockDisplay ( false ) ;
    wxEndBusyCursor() ;

    if ( !all )
        {
        wxMessageDialog md ( this , txt("t_not_all_files_loaded") , txt("msg_box") ) ;
        md.ShowModal() ;
        }

    return true ;
    }

bool TManageDatabaseDialog::do_load_DNA ( const wxString& name , const wxString& db )
    {
    wxPrintf("D: TManageDatabaseDialog::do_load_DNA (name = %s, db = %s)\n", name, db ) ;

    TStorage *tstorage = getTempDB ( getFileName ( db ) ) ;
    v = new TVector () ;

    if ( name.IsEmpty() ) return false ;
    // Loading vector
    wxString sql = "SELECT * FROM dna WHERE dna_name=\"" ;
    sql += name ;
    sql += "\"" ;

    TSQLresult sr = tstorage->getObject ( sql ) ;
    if( sr.rows() == 0 )
        {
        wxMessageBox ( name , txt("t_could_not_load") ) ;
        return false ;
        }
    v->setName ( sr[0][sr["dna_name"]] ) ;
    v->setDescription ( sr[0][sr["dna_description"]] ) ;
    v->setSequence ( sr[0][sr["dna_sequence"]] ) ;
    v->setType ( atoi ( sr[0][sr["dna_type"]].mb_str() ) ) ;
    v->setStickyEnd ( true , true , sr[0][sr["dna_sticky_ul"]] ) ;
    v->setStickyEnd ( true , false , sr[0][sr["dna_sticky_ll"]] ) ;
    v->setStickyEnd ( false , true , sr[0][sr["dna_sticky_ur"]] ) ;
    v->setStickyEnd ( false , false , sr[0][sr["dna_sticky_lr"]] ) ;

    v->setParams ( sr[0][sr["dna_params"]] ) ;
    if ( sr[0][sr["dna_circular"]] == "1" ) v->setCircular ( true ) ;
    else v->setCircular ( false ) ;
    v->setDatabase ( db ) ;
    if ( v->getType() != TYPE_ALIGNMENT ) v->removeBlanksFromSequence () ;

    wxString s = sr[0][sr["dna_restriction_enzymes"]] ;
    wxString t = "" ;
    wxPrintf("D: Iterating over restrition enzymes (s=%s)\n", s ) ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        if ( s.GetChar(a) == ',' )
            {
            if ( !t.IsEmpty() )
                {
                if ( t.GetChar(0) == '*' )
                    {
                    v->proteases.Add ( t.substr ( 1 ) ) ;
                    }
                else
                    {
                    TRestrictionEnzyme *re = tstorage->getRestrictionEnzyme ( t ) ;
                    if ( re ) v->re.Add ( re ) ;
                    }
                }
            t = "" ;
            }
        else t += s.GetChar(a) ;
        }

    wxPrintf("D: t=%s\n",t);

    if ( !t.IsEmpty() )
        {
        wxPrintf("D: t is not empty: '%s'\n",t);
        if ( t.GetChar(0) == '*' ) v->proteases.Add ( t.substr ( 1 ) ) ;
        else v->re.Add ( tstorage->getRestrictionEnzyme ( t ) ) ;
        }
    else
        {
        wxPrintf("D: t is empty\n");
        }

    // Loading items
    sql = "SELECT * FROM dna_item WHERE di_dna=\"" + name + "\"" ;
    wxPrintf("D: %s\n", sql ) ;
    sr = tstorage->getObject ( sql ) ;
    for ( int a = 0 ; a < sr.rows() ; a++ )
        {
        TVectorItem i ;
        i.name = sr[a][sr["di_name"]] ;
        i.desc = sr[a][sr["di_description"]] ;
        wxPrintf( "D: %d of %d: %s : %s\n", a, sr.rows() , i.name, i.desc ) ;
        wxString typeString = sr[a][sr["di_type"]] ;
        wxPrintf( "D: di_type: '%s'\n", typeString ) ;
        if (typeString.length() > 0) {
            auto c = typeString[0].GetValue() ;
            i.setType ( c ) ;
        }
        else {
            i.setType ( "" ) ;
        }
        //wxPrintf( "D: type: %c\n", i.getType() ) ;
        i.from = atoi ( sr[a][sr["di_from"]].mb_str() ) ;
        i.to = atoi ( sr[a][sr["di_to"]].mb_str() ) ;
        wxPrintf( "D: from: %d, to %d\n", i.from, i.to ) ;
        i.setDirection ( atoi ( sr[a][sr["di_direction"]].mb_str() ) ) ;
        i.explodeParams ( sr[a][sr["di_params"]] ) ;
        v->items.push_back ( i ) ;
        }

    if ( justload )
        {
        wxPrintf( "D: do_load_DNA - return (justload)\n" ) ;
        return true ;
        }

    // Sorting by size, largest first
    wxPrintf("D: TManageDatabaseDialog::do_load_DNA: retrieved %ld elements \n", v->items.size() ) ;
    //wxPrintf( "D: TManageDatabaseDialog::do_load_DNA: retrieved elements \n" ) ;
    if ( v->items.size() < 100 ) // Don't do that for genomes!
        {
        for ( int a = 1 ; a < v->items.size() ; a++ )
            {
            if ( v->getItemLength ( a-1 ) < v->getItemLength ( a ) )
                {
                TVectorItem dummy = v->items[a] ;
                v->items[a] = v->items[a-1] ;
                v->items[a-1] = dummy ;
                a -= 2 ;
                if ( a < 0 )
                    {
                    a = 0 ;
                    }
                }
            }
        }

    wxPrintf( "D: TManageDatabaseDialog::do_load_DNA: mostly done\n" ) ;

    v->updateDisplay() ;
    v->undo.clear() ;

    ChildBase *n = NULL ;
    if ( v->getType() == TYPE_AMINO_ACIDS )
        {
        wxString db1 = v->getDatabase() ;
        n = myapp()->frame->newAminoAcids ( v , v->getName() ) ;
        myass ( n , "Error opening AA" ) ;
        n->vec->setDatabase ( db1 ) ;
        n->vec->setWindow ( n ) ;
        }
    else if ( v->getType() == TYPE_ALIGNMENT )
        {
        wxArrayString vs ;
        wxArrayChildBase vc ;
        TAlignment *ali = myapp()->frame->runAlignment ( vs , vc , v ) ;
        ali->name = name ;
        ali->database = db ;
        myapp()->frame->setActiveChild ( (ChildBase*) ali ) ;
        }
    else
        {
        v->recalculateCuts () ;
        wxString db1 = v->getDatabase() ;
        n = myapp()->frame->newFromVector ( v , v->getType() ) ;
        myass ( n , "Error opening DNA" ) ;
        n->vec->setDatabase ( db1 ) ;
        }

    wxPrintf( "D: TManageDatabaseDialog::do_load_DNA: returning\n" ) ;
    return true ;
    }

// REDUNDANT : TStorage::fixDNAname does the same
wxString TManageDatabaseDialog::fixQuotes ( const wxString& _s ) const
    {
    wxString s( _s ) ;
    for ( int a = 0 ; a < s.length() ; a++ ) // Fixing quotes
        if ( s.GetChar(a) == '"' ) s.SetChar ( a , 39 ) ;
    return s ;
    }

bool TManageDatabaseDialog::doesNameExist ( const wxString& name , const wxString& dbname )
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
        if ( silent_overwrite ) return false ;
        wxMessageDialog md ( this , txt("t_entry_exists_but") , name , wxYES|wxNO|wxYES_DEFAULT ) ;
        int r = md.ShowModal () ;
        if ( r == wxID_YES ) return false ;
        }
    else
        {
        wxMessageDialog md ( this , txt("t_entry_exists") , name ) ;
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
    if ( myapp()->frame->project.name != name &&
         doesNameExist ( name , dbname ) ) return ;
    TStorage *storage = getTempDB ( getFileName ( dbname ) ) ;

    // New name, or overwriting old one
    wxString sql , s1 , s2 ;
    wxString x = fixQuotes ( name ) ;

    // New default project
    myapp()->frame->project.db = dbname ;

    // Deleting old one, if any
    sql = "DELETE FROM project WHERE pr_name=\"" + x + "\"" ;
    storage->getObject ( sql ) ;
    sql = "DELETE FROM project_dna WHERE pd_project=\"" + x + "\"" ;
    storage->getObject ( sql ) ;

    // Storing new one
    s1 = s2 = "" ;
    storage->sqlAdd ( s1 , s2 , "pr_name" , x ) ;
    storage->sqlAdd ( s1 , s2 , "pr_desc" , _T("No description") ) ;
    sql = "INSERT INTO project (" + s1 + ") VALUES (" + s2 + ")" ;
    storage->getObject ( sql ) ;

    for ( int a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
        {
        if ( myapp()->frame->children[a]->def == _T("dna") ||
             myapp()->frame->children[a]->def == _T("AminoAcids") ||
             myapp()->frame->children[a]->def == _T("alignment") )
           {
           ChildBase *c = myapp()->frame->children[a] ;
           wxString dna_name , dna_db ;
           if ( c->def == _T("alignment") )
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
              sql = "INSERT INTO project_dna (" + s1 + ") VALUES (" + s2 + ")" ;
              storage->getObject ( sql ) ;
              }
            else
               {
//             wxMessageDialog md ( this , "Not saved" , myapp()->frame->children[a]->def ) ;
//             md.ShowModal() ;
               }
           }
        }
    if ( myapp()->frame->project.name != x )
        {
        myapp()->frame->project.name = x ;
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
    wxBeginBusyCursor() ;

    // New name, or overwriting old one
    TSQLresult sr ;
    wxString sql , s1 , s2 ;
    wxString x = fixQuotes ( name ) ;
    v->setDatabase ( dbname ) ; // Now in that database

    // Deleting old one, if any
    sql = "DELETE FROM dna WHERE dna_name=\"" + x + "\"" ;
    sr = storage->getObject ( sql ) ;
    sql = "DELETE FROM dna_item WHERE di_dna=\"" + x + "\"" ;
    sr = storage->getObject ( sql ) ;

    // Storing new one
    wxString enzymes = "," ;

    // Restriction enzymes
    for ( int a = 0 ; a < v->re.GetCount() ; a++ )
        enzymes += v->re[a]->getName() + "," ;

    // Proteases
    for ( int a = 0 ; a < v->proteases.GetCount() ; a++ )
        enzymes += "*" + wxString ( v->proteases[a] ) + "," ;

    s1 = s2 = "" ;
    storage->sqlAdd ( s1 , s2 , "dna_name" , x ) ;
    storage->sqlAdd ( s1 , s2 , "dna_description" , v->getDescription() ) ;
    storage->sqlAdd ( s1 , s2 , "dna_type" , v->getType() ) ;
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
    storage->startRecord () ;
    for ( int a = 0 ; a < v->items.size() ; a++ )
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
    storage->endRecord () ;

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

    wxEndBusyCursor() ;
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

void TManageDatabaseDialog::pmOnRightClick ( wxListEvent &ev , wxListCtrl *lc , wxChoice *cc )
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
//    char t[1000] ;
//    sprintf ( t , txt("t_rename_db_entry") , name.c_str() ) ;
    wxTextEntryDialog ted ( this , wxString::Format ( txt("t_rename_db_entry") , name.c_str() ) , txt("t_rename") , name ) ;
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
            wxMessageDialog md ( this , txt("t_entry_exists_del_target") , txt("t_entry_exists_brief") , wxYES|wxNO ) ;
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
            wxMessageDialog md ( this , txt("t_entry_exists_del_target") , txt("t_entry_exists_brief") , wxYES|wxNO ) ;
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

void TManageDatabaseDialog::delProject ( const wxString& name , const wxString& db )
    {
    wxString sql ;
    TStorage *storage = getTempDB ( getFileName ( db ) ) ;
    sql = "DELETE FROM project WHERE pr_name=\"" + name + "\"" ;
    storage->getObject ( sql ) ;
    sql = "DELETE FROM project_dna WHERE pd_project=\"" + name + "\"" ;
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
        wxMessageDialog md ( this , txt("t_deletion_warning_txt") , txt("t_deletion_warning") , wxYES|wxNO ) ;
        if ( wxID_YES != md.ShowModal() ) return ;
        for ( int a = 0 ; a < context_names.GetCount() ; a++ )
            delProject ( context_names[a] , context_db ) ;
        }
    else
        {
        wxMessageDialog md ( this , txt("t_deletion_warning_txt") , txt("t_deletion_warning") , wxYES|wxNO ) ;
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

void TManageDatabaseDialog::pmOnFilterAlignments ( wxCommandEvent &ev )
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
    TMySQLDialog m ( this , txt("t_add_sql") ) ;
    if ( wxID_OK != m.ShowModal() ) return ;
    wxString fn ;
    fn  = ":" + m.s->GetValue () ;
    fn += ":" + m.u->GetValue () ;
    fn += ":" + m.p->GetValue () ;
    fn += ":" + m.d->GetValue () ;
    addDatabase ( fn ) ;
    }

void TManageDatabaseDialog::pmNewSQL ( wxCommandEvent &ev )
    {
#ifdef USEMYSQL
    TMySQLDialog m ( this , txt("t_new_sql") ) ;
    if ( wxID_OK != m.ShowModal() ) return ;
    wxString fn = TStorage::createMySQLdb ( m.s->GetValue () , m.d->GetValue () , m.u->GetValue () , m.p->GetValue () ) ;

    addDatabase ( fn ) ;
#endif
    }

// **************************************************************

TMyDropTarget::TMyDropTarget ( TManageDatabaseDialog * const _d , wxListCtrl * const _me )
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

TMySQLDialog::TMySQLDialog ( wxWindow *parent , const wxString& title , const wxString& server , const wxString& db , const wxString& user , const wxString& password )
        : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 200 , 180 ) )
   {
   wxBoxSizer *v = new wxBoxSizer ( wxVERTICAL ) ;
   wxFlexGridSizer *g = new wxFlexGridSizer ( 4 , 2 , 10 , 10 ) ;

   s = new wxTextCtrl ( this , -1 , server ) ;
   d = new wxTextCtrl ( this , -1 , db ) ;
   u = new wxTextCtrl ( this , -1 , user ) ;
   p = new wxTextCtrl ( this , -1 , password ) ;
   g->Add ( new wxStaticText ( this , -1 , _T("IP") ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( s , 1 , wxEXPAND , 5 ) ;
   g->Add ( new wxStaticText ( this , -1 , _T("DB") ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( d , 1 , wxEXPAND , 5 ) ;
   g->Add ( new wxStaticText ( this , -1 , _T("Name") ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( u , 1 , wxEXPAND , 5 ) ;
   g->Add ( new wxStaticText ( this , -1 , _T("PWD") ) , 1 , wxEXPAND , 5 ) ;
   g->Add ( p , 1 , wxEXPAND , 5 ) ;

   v->Add ( g , 1 , wxEXPAND , 5 ) ;
   v->Add ( CreateButtonSizer ( wxOK | wxCANCEL ) , 0 , wxEXPAND , 5 ) ;

   SetSizer( v );
   Center () ;
   s->SetFocus() ;
   }


