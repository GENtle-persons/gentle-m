#include "MyFrame.h"

#include "SendHTTP.h"


// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, MyFrameType)
    EVT_MENU(MDI_ABOUT, MyFrame::OnAbout)
    EVT_MENU(MDI_HELP, MyFrame::OnHelp)
    EVT_MENU(MDI_ENZYME_EDITOR, MyFrame::OnEnzymeEditor)
    EVT_MENU(MDI_ALIGNMENT, MyFrame::OnAlignment)
    EVT_MENU(MDI_IMAGE_VIEWER, MyFrame::OnImageViewer)
    EVT_MENU(MDI_EXTERNAL_INTERFACE, MyFrame::OnExternalInterface)
    EVT_MENU(MDI_CALCULATOR, MyFrame::OnCalculator)
    EVT_MENU(MDI_FILE_OPEN, MyFrame::OnFileOpen)
    EVT_MENU(MDI_FILE_IMPORT, MyFrame::OnFileImport)
    EVT_MENU(MDI_TEXT_IMPORT, MyFrame::OnTextImport)
    EVT_MENU(MDI_PROJECT_SAVE, MyFrame::OnProjectSave)
    EVT_MENU(MDI_PROJECT_LOAD, MyFrame::OnProjectLoad)
    EVT_MENU(MDI_PROJECT_CLOSE, MyFrame::OnProjectClose)
    EVT_MENU(MDI_QUIT, MyFrame::OnQuit)
    EVT_MENU(MDI_CLOSE, MyFrame::OnMDIClose)
    EVT_MENU(MDI_MANAGE_DATABASE, MyFrame::OnManageDatabase)
    EVT_MENU(PROGRAM_OPTIONS, MyFrame::OnProgramOptions)
    
    EVT_MENU(Y___, MyFrame::TestMenu)
    EVT_MENU(MDI_RESTRICTION, MyFrame::BollocksMenu)
    EVT_MENU(MDI_LIGATION, MyFrame::BollocksMenu)
    EVT_MENU(MDI_PRINT_REPORT, MyFrame::BollocksMenu)

    EVT_MENU(MDI_NEXT_WINDOW, MyFrame::BollocksMenu)
    EVT_MENU(MDI_PREV_WINDOW, MyFrame::BollocksMenu)

    EVT_MENU_RANGE(A___,Z___,MyFrame::RerouteMenu)


    EVT_SASH_DRAGGED_RANGE(FRAME_SASH_1, FRAME_SASH_TOP, MyFrame::OnSashDrag)
    
    EVT_CLOSE(MyFrame::OnClose)

    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

// Define my frame constructor
#define ACC_ENT 39
MyFrame::MyFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : MyFrameType(parent, id, title, pos, size, style)
{
    dying = false ;
    activating = false ;
    locked = 0 ;
#ifdef MYTEST
    test_suite = NULL ;
#endif

    // Accelerators
    wxAcceleratorEntry entries[ACC_ENT];
    entries[ 0].Set(wxACCEL_CTRL, (int) 'X', MDI_CUT);
    entries[ 1].Set(wxACCEL_CTRL, (int) 'C', MDI_COPY);
    entries[ 2].Set(wxACCEL_CTRL, (int) 'V', MDI_PASTE);
    entries[ 3].Set(wxACCEL_CTRL, (int) 'L', MDI_LIGATION);
    entries[ 4].Set(wxACCEL_CTRL, (int) 'A', MDI_MARK_ALL);
    entries[ 5].Set(wxACCEL_CTRL, (int) 'S', MDI_FILE_SAVE);
    entries[ 6].Set(wxACCEL_CTRL, (int) 'E', MDI_EXPORT);
    entries[ 7].Set(wxACCEL_NORMAL, WXK_F2, MDI_EDIT_MODE);
    entries[ 8].Set(wxACCEL_CTRL, (int) '0', AA_NONE);
    entries[ 9].Set(wxACCEL_CTRL, (int) '1', AA_THREE_1);
    entries[10].Set(wxACCEL_CTRL, (int) '2', AA_THREE_2);
    entries[11].Set(wxACCEL_CTRL, (int) '3', AA_THREE_3);
    entries[12].Set(wxACCEL_CTRL, (int) '4', AA_THREE_M1);
    entries[13].Set(wxACCEL_CTRL, (int) '5', AA_THREE_M2);
    entries[14].Set(wxACCEL_CTRL, (int) '6', AA_THREE_M3);
    entries[15].Set(wxACCEL_CTRL, (int) '7', AA_ALL);
    entries[16].Set(wxACCEL_CTRL, (int) '8', AA_KNOWN);
    entries[17].Set(wxACCEL_CTRL, (int) 'G', MDI_ALIGNMENT);
    entries[18].Set(wxACCEL_CTRL, (int) 'F', MDI_FIND);
    entries[19].Set(wxACCEL_NORMAL, WXK_F1, MDI_HELP);
    entries[20].Set(wxACCEL_CTRL, (int) 'Q', AA_ONE);
    entries[21].Set(wxACCEL_CTRL, (int) 'W', AA_THREE);
    entries[22].Set(wxACCEL_NORMAL, WXK_F11, MDI_PROJECT_LOAD);
    entries[23].Set(wxACCEL_NORMAL, WXK_F12, MDI_PROJECT_SAVE);
    entries[24].Set(wxACCEL_NORMAL, WXK_F4, MDI_ORFS);
    entries[25].Set(wxACCEL_NORMAL, WXK_F5, MDI_CIRCULAR_LINEAR);
    entries[26].Set(wxACCEL_NORMAL, WXK_F3, MDI_VIEW_MODE);
    entries[27].Set(wxACCEL_NORMAL, WXK_F6, MDI_TOGGLE_FEATURES);
    entries[28].Set(wxACCEL_NORMAL, WXK_F7, MDI_TOGGLE_RESTRICTION);
    entries[29].Set(wxACCEL_CTRL, (int) 'O', MDI_FILE_OPEN);
    entries[30].Set(wxACCEL_CTRL, (int) 'I', MDI_FILE_IMPORT);
    entries[31].Set(wxACCEL_CTRL, (int) 'Z', MDI_UNDO);
    entries[32].Set(wxACCEL_CTRL, (int) 'N', MDI_TEXT_IMPORT);
    entries[33].Set(wxACCEL_CTRL, (int) 'D', MDI_EXTERNAL_INTERFACE);
    entries[34].Set(wxACCEL_CTRL, WXK_F4, MDI_CLOSE);
    entries[35].Set(wxACCEL_CTRL, WXK_TAB, MDI_NEXT_WINDOW);
    entries[36].Set(wxACCEL_CTRL|wxACCEL_SHIFT, WXK_TAB, MDI_PREV_WINDOW);
    entries[37].Set(wxACCEL_NORMAL, WXK_F8, MDI_TOGGLE_IDNA);
    entries[38].Set(wxACCEL_CTRL|wxACCEL_SHIFT, WXK_F12, Y___);
    
    wxAcceleratorTable accel(ACC_ENT, entries);
    SetAcceleratorTable(accel);
    html_ep = NULL ;
    lastChild = NULL ;
    tb_mychild = NULL ;
}

MyFrame::~MyFrame ()
    {
    if ( html_ep ) delete html_ep ;
    rememberLastProject () ;
    CLEAR_DELETE ( dbcache ) ;
//    delete LS ;
    }

void MyFrame::initme ()
    {
#ifdef __WXGTK__
    SetFont ( *MYFONT ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ) ;    
#endif
  wxSashLayoutWindow* win ;
  m_leftWindow2 = m_topWindow = m_bottomWindow = NULL ;
/*  
  win = new wxSashLayoutWindow(this, FRAME_SASH_TOP,
                             wxDefaultPosition, wxSize(200, 30),
                             wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

  win->SetDefaultSize(wxSize(1000, 30));
  win->SetOrientation(wxLAYOUT_HORIZONTAL);
  win->SetAlignment(wxLAYOUT_TOP);
  win->SetBackgroundColour(wxColour(255, 0, 0));
  win->SetSashVisible(wxSASH_BOTTOM, TRUE);

  m_topWindow = win; 

  // A window like a statusbar
  win = new wxSashLayoutWindow(this, FRAME_SASH_BOTTOM,
                               wxDefaultPosition, wxSize(200, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  win->SetDefaultSize(wxSize(1000, 30));
  win->SetOrientation(wxLAYOUT_HORIZONTAL);
  win->SetAlignment(wxLAYOUT_BOTTOM);
  win->SetBackgroundColour(wxColour(0, 0, 255));
  win->SetSashVisible(wxSASH_TOP, TRUE);

  m_bottomWindow = win;*/

    // A window to the left of the client window
    // This contains the main tree
    win = new wxSashLayoutWindow(this, FRAME_SASH_1,
                               wxDefaultPosition, wxSize(200, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
    win->SetDefaultSize(wxSize(200, 1000));
    win->SetOrientation(wxLAYOUT_VERTICAL);
    win->SetAlignment(wxLAYOUT_LEFT);
//    win->SetBackgroundColour(wxColour(0, 255, 0));
    win->SetSashVisible(wxSASH_RIGHT, TRUE);
//    win->SetExtraBorderSize(10);
    mainTree = new TMainTree ( win , MAIN_TREE_DUMMY ) ;
    

  m_leftWindow1 = win;

  // Another window to the left of the client window
  win = new wxSashLayoutWindow(this, FRAME_SASH_2,
                               wxDefaultPosition, wxSize(500, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  win->SetDefaultSize(wxSize(500, 1000));
  win->SetOrientation(wxLAYOUT_VERTICAL);
  win->SetAlignment(wxLAYOUT_LEFT);
//  win->SetBackgroundColour(wxColour(0, 255, 255));
  win->SetSashVisible(wxSASH_RIGHT, TRUE);
  m_leftWindow2 = win;

    // Database access
    LS = new TStorage ( LOCAL_STORAGE ) ;
    LS->createDatabase() ;
//    LS->optimizeDatabase () ; // This will VACUUM sqlite databases

    // Synchronize enzymes in known databases
    // Turned of temporarily
//    LS->synchronize () ;
    
    // Import enzymes from local database
    LS->import() ;
    
    // Loading program options from database
    showVectorTitle = LS->getOption ( "SHOWVECTORTITLE" , true ) ;
    showVectorLength = LS->getOption ( "SHOWVECTORLENGTH" , true ) ;
    enhancedRefresh = LS->getOption ( "ENHANCEDREFRESH" , true ) ;
    loadLastProject = LS->getOption ( "LOADLASTPROJECT" , true ) ;
    useMetafile = LS->getOption ( "USEMETAFILE" , false ) ;
    showSplashScreen = LS->getOption ( "SHOWSPLASHSCREEN" , true ) ;
    checkUpdate = LS->getOption ( "CHECKUPDATE" , true ) ;
    lang_string = LS->getOption ( "LANGUAGE" , "en" ) ;
    useCoolCanvas = LS->getOption ( "USECOOLCANVAS" , false ) ; // Not saved yet
    useInternalHelp = LS->getOption ( "USEINTERNALHELP" , false ) ; // Not saved yet
    init_txt ( lang_string.c_str() ) ;
    global_enzyme_rules = new TEnzymeRules ;
    global_enzyme_rules->load_global_settings() ;

    // Set the DEBUGGING option in your local test database,
    // so you won't be bothered with updates anymore :-)
    if ( LS->getOption ( "DEBUGGING" , "" ) == "1" ) checkUpdate = false ;

#ifdef __WXMSW__
    if ( checkUpdate )
        {
        wxString cur_update = check4update () ;
        if ( !cur_update.IsEmpty() )
            {
            update2version ( cur_update ) ;
            return ;
            }
        }
#endif

    project.name = txt("project") ;
    SetTitle ( txt("gentle") ) ;
    mainTree->initme() ;
    
    // Give it an icon
#ifdef __WXMSW__
    SetIcon(wxIcon("GENtle.ico",wxBITMAP_TYPE_ICO));
#else
    SetIcon(wxIcon( mondrian_xpm ));
#endif
    
    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(MDI_FILE_OPEN, txt("m_open") , txt("m_opentxt") );
    file_menu->Append(MDI_FILE_IMPORT, txt("m_import") , txt("m_importtxt") );
    file_menu->Append(MDI_TEXT_IMPORT, txt("m_text_import") , txt("m_text_importtxt") );
    file_menu->AppendSeparator();
    file_menu->Append(MDI_PROJECT_LOAD, txt("m_project_open") , txt("m_project_opentxt") );    
    file_menu->Append(MDI_PROJECT_SAVE, txt("m_project_save") , txt("m_project_savetxt") );    
    file_menu->Append(MDI_PROJECT_CLOSE, txt("m_project_close") , txt("m_project_closetxt") );    
    file_menu->AppendSeparator();
    file_menu->Append(MDI_QUIT, txt("m_exit") , txt("m_exittxt") ) ;

    wxMenu *tool_menu = getToolMenu ( false ) ;
    wxMenu *help_menu = getHelpMenu () ;

    menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);
    CreateStatusBar();
    GetStatusBar()->SetFieldsCount ( 2 ) ;

#ifdef __WXGTK__
    Show(TRUE);
    int sw , sh ;
    wxDisplaySize ( &sw , &sh ) ;
    // SetSize ( sw/10 , sh/10 , sw*8/10 , sh*7/10 ) ;
    SetSize ( 0 , 0 , sw , sh*9/10 ) ;
#else
    Show(TRUE);
    Maximize() ;
#endif

    // Drag'n'drop    
    MyFrameDropTarget *dt = new MyFrameDropTarget ;
    SetDropTarget ( dt ) ;

    // Bitmap library
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"new.bmp", wxBITMAP_TYPE_BMP) ) ;  // 0
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"open.bmp", wxBITMAP_TYPE_BMP) ) ; // 1
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"save.bmp", wxBITMAP_TYPE_BMP) ) ; // 2 

    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"undo.bmp", wxBITMAP_TYPE_BMP) ) ; // 3 
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"cut.bmp", wxBITMAP_TYPE_BMP) ) ;  // 4
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"copy.bmp", wxBITMAP_TYPE_BMP) ) ; // 5
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"paste.bmp", wxBITMAP_TYPE_BMP) ) ;// 6

    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"plasmid_circular.bmp", wxBITMAP_TYPE_BMP) ) ; // 7
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"plasmid_linear.bmp", wxBITMAP_TYPE_BMP) ) ;   // 8

    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"orfs.bmp", wxBITMAP_TYPE_BMP) ) ; // 9
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"display_features.bmp", wxBITMAP_TYPE_BMP) ) ; // 10
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"display_restriction.bmp", wxBITMAP_TYPE_BMP) ) ; // 11
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"mode_view.bmp", wxBITMAP_TYPE_BMP) ) ; // 12
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"mode_edit.bmp", wxBITMAP_TYPE_BMP) ) ; // 13
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"primer_import.bmp", wxBITMAP_TYPE_BMP) ) ; // 14
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"primer_export.bmp", wxBITMAP_TYPE_BMP) ) ; // 15
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"print.bmp", wxBITMAP_TYPE_BMP) ) ; // 16

    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"align_tool.bmp", wxBITMAP_TYPE_BMP) ) ; // 17
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"image.bmp", wxBITMAP_TYPE_BMP) ) ; // 18
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"web.bmp", wxBITMAP_TYPE_BMP) ) ; // 19
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"calc_tool.bmp", wxBITMAP_TYPE_BMP) ) ; // 20
    bitmaps.push_back ( wxBitmap (myapp()->bmpdir+myapp()->slash+"settings_tool.bmp", wxBITMAP_TYPE_BMP) ) ; // 21

    html_ep = new wxHtmlEasyPrinting ( "" , this ) ;
    
    wxBeginBusyCursor() ;       
    lockDisplay ( true ) ;
       
    // Load last project?
//    loadLastProject = false ;
    if ( loadLastProject )
        {
        wxString sql , n , d ;
        TSQLresult r ;
        sql = "SELECT s_value FROM stuff WHERE s_type='LASTPROJECT' AND s_name='NAME'" ;
        r = LS->getObject ( sql ) ;
        if ( r.rows() > 0 ) n = r[0][0] ;
        sql = "SELECT s_value FROM stuff WHERE s_type='LASTPROJECT' AND s_name='DATABASE'" ;
        r = LS->getObject ( sql ) ;
        if ( r.rows() > 0 ) d = r[0][0] ;
        if ( !n.IsEmpty() && !d.IsEmpty() )
           {
           TManageDatabaseDialog mdb ( this , "dummy" , ACTION_MODE_STARTUP ) ;
           mdb.do_load_project ( n , d ) ;
           mainTree->Refresh () ;
           }
        }
        
    // Command line parameters?
    if ( myapp()->argc > 1 )
       {
       int a , b ;
       wxProgressDialog pd ( txt("t_loading") , "" , myapp()->argc-1 , NULL , wxPD_ALL ) ;
       for ( a = 1 ; a < myapp()->argc ; a++ )
          {
          wxString path = myapp()->argv[a] ;
          if ( !pd.Update ( a-1 , path ) ) break ;
          if ( path.Left(9).Upper() == "GENTLE://" )
          	{
      	    path = path.Mid ( 9 ) ;
      	    if ( path.GetChar ( path.length()-1 ) == '/' ) // Trailing '/'
      	    	path = path.Mid ( 0 , path.length() - 1 ) ;
 	    	wxString db = path.BeforeFirst ( ':' ) ;
 	    	wxString name = path.AfterFirst ( ':' ) ;
      	    TManageDatabaseDialog mdb ( this , "dummy" , ACTION_MODE_STARTUP ) ;
      	    if ( !mdb.do_load_DNA ( name , db ) ) wxMessageBox ( txt("t_not_all_files_imported") ) ;
            mainTree->Refresh () ;
          	}
          else
          	{
          	wxString file = path.AfterLast ( '/' ) ;
          	file = file.AfterLast ( '\\' ) ;
          	importFile ( file , path , -1 ) ;
          	wxSetWorkingDirectory ( myapp()->homedir ) ;
          	}   	
          }
       }
        
    lockDisplay ( false ) ;
    wxEndBusyCursor() ;
    Raise () ;
    
    SetSizeHints ( 600 , 400 ) ;
    Show(TRUE);
//    SetStatusText ( txt("t_update_warning") , 1 ) ;
    }
    
void MyFrame::OnClose(wxCloseEvent& event)
{
    bool canclose = true ;
    for ( int a = 0 ; canclose && a < children.GetCount() ; a++ )
        canclose = ! ( children[a]->vec && children[a]->vec->isChanged() ) ;
    if ( LS->getOption ( "DEBUGGING" , "" ) == "1" ) canclose = true ; // For debugging
    if ( !canclose )
        {
        if ( wxYES == wxMessageBox ( txt("t_some_changed") , txt("msg_box") , wxICON_QUESTION | wxYES | wxNO ) )
           canclose = true ;
        }
    if ( canclose )
        {    
        dying = true ;
        event.Skip();
        }    
    else event.Veto() ;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(txt("t_about_txt"), txt("t_about"));
}

void MyFrame::OnHelp(wxCommandEvent& WXUNUSED(event) )
{
    if ( useInternalHelp )
        {
        wxHtmlHelpController *hc = new wxHtmlHelpController ( wxHF_DEFAULT_STYLE|wxHF_OPEN_FILES ) ;
        wxString helpfile ;
        helpfile += myapp()->homedir ;
        helpfile += "\\help\\" ;
        helpfile += lang_string ;
        helpfile += ".hhp" ;
        hc->AddBook ( helpfile ) ;
        hc->DisplayContents () ;
        }
    else
        {
        #ifdef __WXMSW__
            wxString helpfile = "\"" ;
            helpfile += myapp()->homedir ;
            helpfile += "\\" ;
            helpfile += txt("f_help") ;
            helpfile += "\"" ;
            
            for ( int a = 0 ; a < helpfile.length() ; a++ )
               if ( helpfile.GetChar(a) == '/' ) helpfile.SetChar ( a , '\\' ) ;
            
            wxString command = myapp()->getHTMLCommand ( helpfile ) ;
            wxExecute ( command ) ;
        #else
        #endif
        }
}

ChildBase *MyFrame::GetActiveChild() //SDI
    {
    return lastChild ;
    }

void MyFrame::OnEnzymeEditor(wxCommandEvent& WXUNUSED(event) )
{   
    TVectorEditor ee ( this , txt("t_enzymeeditor") , NULL ) ;
    ee.ShowModal () ;
}

void MyFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event) )
{
	int i = children.GetCount() ;
    TManageDatabaseDialog dbd ( this , txt("t_open") , ACTION_MODE_LOAD ) ;
    dbd.ShowModal () ;
    if ( i != children.GetCount() ) setActiveChild ( children[children.GetCount()-1] ) ;
}

void MyFrame::OnTextImport(wxCommandEvent& WXUNUSED(event) )
{
    TextImportDialog d ( this , txt("m_text_importtxt") ) ;
    
    ChildBase *c = (ChildBase*) GetActiveChild() ;
    if ( c && c->def == "PrimerDesign" ) d.type->SetSelection ( 4 ) ;
    
    if ( d.ShowModal () != wxID_OK ) return ;

    int type = d.type->GetSelection () ;

    if ( type == 0 ) // DNA
        {
        TVector *v = new TVector ;
        v->setName ( d.sName ) ;
        v->setSequence ( d.sSequence ) ;
        v->setDescription ( txt("manually_entered_sequence") + wxString ( "\n" + wxGetUserName() ) ) ;
        newFromVector ( v ) ;
        }
    else if ( type == 1 ) // Amino Acids 
        {
        newAminoAcids ( d.sSequence , d.sName ) ;
        }
    else if ( type == 2 ) // GenBank
        {
        TGenBank gb ;
        gb.paste ( d.sequence->GetValue() ) ;
        newGB ( gb , d.sName ) ;
        }        
    else if ( type == 3 ) // XML
        {
        // Trying XML formats
        TXMLfile xml ;
        xml.parse ( d.sequence->GetValue().c_str() ) ;
        if ( xml.success() )
           {
           newXML ( xml , d.sName ) ;
           return ;
           }
        }
    else if ( type == 4 ) // Primer
        {
        TVector *v = new TVector ;
        v->setName ( d.sName ) ;
        v->setSequence ( d.sSequence ) ;
        v->type = TYPE_PRIMER ;
        v->setDescription ( txt("manually_entered_sequence") + wxString("\n") + wxGetUserName() ) ;
        newFromVector ( v , TYPE_PRIMER ) ;
        }
}

void MyFrame::saveImage ( wxBitmap *bmp , wxString name )
	{
    wxString wildcard ;
    wildcard += "PNG (*.png)|*.png" ;
    wildcard += "|TIF (*.tif)|*.tif" ;
    wildcard += "|Bitmap (*.bmp)|*.bmp" ;
    wildcard += "|JPEG (*.jpg)|*.jpg" ;
    
    wxString lastdir = LS->getOption ( "LAST_IMPORT_DIR" , "C:" ) ;
    wxFileDialog d ( this , txt("t_save_image") , lastdir , name , wildcard , wxSAVE|wxOVERWRITE_PROMPT ) ;
    if ( d.ShowModal() != wxID_OK ) return ;
    wxString filename = d.GetPath() ;
    
    name.Replace ( "*" , "" , TRUE ) ;
    name.Replace ( ":" , "_" , TRUE ) ;
    name.Replace ( "/" , "_" , TRUE ) ;
    name.Replace ( "\\" , "_" , TRUE ) ;

    wxBeginBusyCursor () ;
    wxString type = filename.AfterLast('.').Upper() ;
    if ( type == "BMP" ) bmp->SaveFile ( filename , wxBITMAP_TYPE_BMP ) ;
    if ( type == "PNG" ) bmp->SaveFile ( filename , wxBITMAP_TYPE_PNG ) ;
    if ( type == "JPG" ) bmp->SaveFile ( filename , wxBITMAP_TYPE_JPEG ) ;
    if ( type == "TIF" ) bmp->SaveFile ( filename , wxBITMAP_TYPE_TIF ) ;
    wxEndBusyCursor () ;
	}    

void MyFrame::OnFileImport(wxCommandEvent& WXUNUSED(event) )
{
    wxString wcAutomatic = txt("format_filter_auto") ;
    wxString wcClone = txt("format_filter_clone") ;
    wxString wcGenBank = txt("format_filter_genbank") ;
    wxString wcSequencing = txt("format_filter_fasta") ;
    wxString wcPlainTextDNA = txt("format_filter_plain_dna") ;
    wxString wcPlainTextAA = txt("format_filter_plain_aa") ;
    wxString wcABIformat = txt("format_filter_abi") ;
    wxString wildcard = wcAutomatic +
                        "|" + wcClone + 
                        "|" + wcGenBank + 
                        "|" + wcSequencing +
                        "|" + wcPlainTextDNA +
                        "|" + wcPlainTextAA +
                        "|" + wcABIformat ;
    wxString lastdir = LS->getOption ( "LAST_IMPORT_DIR" , "C:" ) ;
    wxFileDialog d ( this , txt("import_file") , lastdir , 
            "" , wildcard , wxOPEN | wxMULTIPLE ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;
    
    LS->setOption ( "LAST_IMPORT_DIR" , d.GetDirectory() ) ;

    int filter = d.GetFilterIndex () - 1 ;
    wxArrayString files , paths ;
    d.GetFilenames ( files ) ;
    d.GetPaths ( paths ) ;
    wxBeginBusyCursor();
    wxSafeYield() ;
    wxProgressDialog pd ( txt("t_loading") , "" , files.GetCount() , NULL , wxPD_ALL ) ;
//    lockDisplay ( true ) ;
    wxString unknown ;
    int a ;
    for ( a = 0 ; a < files.GetCount() ; a++ )
       {
       if ( !pd.Update ( a , files[a] ) ) break ;
       if ( !importFile ( files[a] , paths[a] , filter ) )
       		{
   		    if ( !unknown.IsEmpty() ) unknown += "\n " ;
            unknown += files[a] ;
            }    
       }
    if ( a == files.GetCount() ) pd.Update ( a ) ; // Hide progress dialog
    if ( !unknown.IsEmpty() )
    	wxMessageBox ( unknown , txt("t_unable_to_detect_file_type") ) ;
//    lockDisplay ( false ) ;
    SetFocus () ;
    wxEndBusyCursor();
    mainTree->Refresh () ;
    Refresh () ;
    activateChild ( getChildIndex ( lastChild ) ) ;
    }
    
bool MyFrame::importFile ( wxString file , wxString path , int filter )
    {
    // Trying GenBank format
    if ( filter == 1 || filter == -1 )
       {
       wxStartTimer () ;
       TGenBank gb ;
       gb.load ( path ) ;
       mylog ( "GenBank import" , "loaded" ) ;
       if ( gb.success )
          {
          newGB ( gb , file ) ;
          return true ;
          }
       }
       
    // Trying ABI format
    if ( filter == 5 || filter == -1 )
       {
        ABItype dummy ;
        dummy.parse ( path ) ;
        if ( dummy.vf.size() > 0 ) // Success
           {
           newABI ( path , file ) ;
           return true ;
           }
        }

    // Trying PDB format
    if ( filter == -1 )
       {
        TPDB pdb ;
        pdb.load ( path ) ;
        if ( pdb.success )
           {
           newPDB ( pdb , file ) ;
           return true ;
           }
       }
       
    // Trying XML formats
    if ( filter == -1 )
       {
        TXMLfile xml ;
        xml.load ( path ) ;
        if ( xml.success() )
           {
           newXML ( xml ) ;
           return true ;
           }
       }

    // Trying UReadSeq package
    if ( filter == 2 || filter == 3 || filter == 4 || filter == -1 )
       {
        TUReadSeq u ( path ) ;
        if ( u.error == 0 && u.seqs.GetCount() > 0 )
           {
           TGenBank gb ;
           u.convert ( gb ) ;
           newGB ( gb ) ;
           return true ;
           }
        }

    // Trying CLONE format
    if ( filter == 0 || filter == -1 )
       {
        TClone clone ;
        clone.LS = LS ;
        clone.load ( path ) ;
        if ( clone.success )
           {
           newCLONE ( clone ) ;
           return true ;
           }
       }

    return false ;
}

void MyFrame::newXML (  TXMLfile &xml , wxString title )
   {
   int n ;
   for ( n = 0 ; n < xml.countVectors() ; n++ )
       {
       TVector *nv = xml.getVector ( n ) ;
       short type = nv->type ;
       if ( !title.IsEmpty() ) nv->setName ( title ) ;
       nv->addDescription ( "\n" + wxGetUserName() ) ;
       if ( type == TYPE_AMINO_ACIDS )
          newAminoAcids ( nv , nv->getName() ) ;
       else
          newFromVector ( nv , type ) ;
       }
   }

MyChild *MyFrame::newCLONE ( TClone &clone )
    {
    MyChild *subframe = new MyChild(getCommonParent(), txt("imported_vector"),
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);
    setChild ( subframe ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    
    subframe->initme() ;
    int type = TYPE_VECTOR ;
    clone.remap ( subframe->vec ) ;
    subframe->vec->setDescription ( subframe->vec->getDescription() + "\n" + wxGetUserName() ) ;
    subframe->vec->setWindow ( subframe ) ;
    subframe->initPanels() ;
    mainTree->addChild(subframe,type) ;
    subframe->Maximize() ;
    }

void MyFrame::newPDB ( TPDB &pdb , wxString title )
    {
    pdb.remap () ;
    for ( int a = 0 ; a < pdb.seqres.size() ; a++ )
        {
        newAminoAcids ( pdb.seqres[a].v , pdb.seqres[a].v->getName() ) -> Activate () ;
        delete pdb.seqres[a].v ;
        }    
    }
        
void MyFrame::newGB ( TGenBank &gb , wxString title )
    {
    int n ;
    TVector *nv ;
    wxArrayTVector vv ;
    wxArrayChildBase vc ;
    wxArrayString vs ;
    bool alignment = false ;
    
    gb.vs.Clear () ;
    gb.vi.Clear () ;

    for ( n = 0 ; n < gb.vs_l.size() ; n++ )
        {
        nv = new TVector ;
        gb.remap ( nv , gb.vs_l[n] , gb.vi_l[n] ) ;
        gb.vs_l[n].Clear () ;
        gb.vi_l[n].Clear () ;

        mylog ( "GenBank import" , "remapped " + nv->getName() ) ;
        vs.Add ( nv->getSequence() ) ;
        vv.Add ( nv ) ;
        nv->setDescription ( nv->getDescription() + "\n" + wxGetUserName() ) ;
        if ( nv->getSequenceLength() != vv[0]->getSequenceLength() ) alignment = false ;
        else if ( nv->getSequence().find ( '-' ) != -1 ) alignment = true ;
        mylog ( "GenBank import" , "added " + nv->getName() ) ;
        }
    if ( gb.vs_l.size() == 1 ) alignment = false ;
    
    // Removing alignment artifacts from sequences
    for ( n = 0 ; n < vv.GetCount() ; n++ )
        vv[n]->removeAlignmentArtifacts () ;
    mylog ( "GenBank import" , "artifacts removed" ) ;
        
/*
	// Ask if GenBank alignment should be opened as such (not using this right now)
    if ( alignment )
        {
        if ( wxYES != wxMessageBox ( txt("t_possible_alignment") ,
                       title , wxYES_NO|wxICON_QUESTION ) )
           {
           alignment = false ;
           }        
        }*/
        
    wxBeginBusyCursor() ;
    for ( n = 0 ; n < gb.vs_l.size() ; n++ )
        {
        nv = vv[n] ;
        if ( gb.vs_l.size() == 1 && !title.IsEmpty() && nv->getName().IsEmpty() )
           nv->setName ( title ) ;
        short type = TUReadSeq::getSeqType ( nv->getSequence() ) ;
        if ( type == TYPE_AMINO_ACIDS )
           vc.Add ( newAminoAcids ( nv , nv->getName() ) ) ;
        else
           vc.Add ( newFromVector ( nv , type ) ) ;
        mylog ( "GenBank import" , "child added" ) ;
        }
    vv.Clear () ;
        
    if ( alignment )
        {
        runAlignment ( vs , vc ) ;
        }
        
    wxEndBusyCursor() ;
    }


MyChild* MyFrame::newFromVector ( TVector *nv , int type )
    {
    if ( !nv ) return NULL ;
    MyChild *subframe = new MyChild(getCommonParent(), "",
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);
    myass ( subframe , "MyFrame::newFromVector" ) ;
    setChild ( subframe ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme() ;
    delete subframe->vec ;
    subframe->vec = nv ;
    subframe->vec->setWindow ( subframe ) ;

    subframe->initPanels() ;
    mainTree->addChild(subframe,type) ;
    children.Last()->Activate () ;

    return subframe ;
    }


void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxLayoutAlgorithm layout;
    layout.LayoutFrame(this);
    if ( lastChild )
    	{
	    setActiveChild ( lastChild ) ;
//	    if ( lastChild->vec ) lastChild->vec->recalcvisual = true ;
//        lastChild->Activate() ;
        lastChild->Refresh() ;
        }    
    return ;
}


#define NOB 2
void MyFrame::InitToolBar(wxToolBar* toolBar)
{
}

void MyFrame::addDefaultTools(wxToolBar* toolBar)
	{
 	toolBar->AddSeparator() ;
 	toolBar->AddTool( MDI_ALIGNMENT, myapp()->frame->bitmaps[17] ) ;
 	toolBar->AddTool( MDI_IMAGE_VIEWER, myapp()->frame->bitmaps[18] ) ;
 	toolBar->AddTool( MDI_CALCULATOR, myapp()->frame->bitmaps[20] ) ;
 	toolBar->AddTool( PROGRAM_OPTIONS, myapp()->frame->bitmaps[21] ) ;
 	toolBar->AddTool( MDI_EXTERNAL_INTERFACE, myapp()->frame->bitmaps[19] ) ;
	}

TAlignment *MyFrame::runAlignment ( wxArrayString &vs , wxArrayChildBase &vc , TVector *nv )
    {
//    Thaw () ;
    TAlignment *subframe = new TAlignment ( getCommonParent() , "Alignment" ) ;
    setChild ( subframe ) ;
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;
    if ( locked == 0 ) subframe->Activate () ;

    if ( nv )
        {
        subframe->fromVector ( nv ) ;
        }
    else if ( vs.GetCount() == 0 )
        {
        wxCommandEvent ev ;
        subframe->OnSettings ( ev ) ;
        }
    else
        {
        subframe->prealigned ( vs , vc ) ;
        }

    if ( locked == 0 ) subframe->Show() ;
    
    mainTree->addChild ( subframe , TYPE_ALIGNMENT ) ;
    mainTree->Refresh () ;
    return subframe ;
    }

void MyFrame::OnAlignment(wxCommandEvent& event)
    {
    wxArrayString vs ; // Dummy
    wxArrayChildBase vc ; // Dummy
    runAlignment ( vs , vc ) ;
    }
    
void MyFrame::OnManageDatabase(wxCommandEvent& event)
    {
    TManageDatabaseDialog mdd ( this , txt("m_manage_dbtxt") ) ;
    int r = mdd.ShowModal () ;
    }

void MyFrame::OnProgramOptions(wxCommandEvent& event)
    {
    ProgramOptionsDialog pod ( this , txt("t_program_options" ) ) ;
    global_enzyme_rules->setup_options ( pod.globalEnzymesPanel ) ;
    int r = pod.ShowModal() ;
    if ( r != wxID_OK ) return ;

   	wxBeginBusyCursor() ;

    // retrieving options
    global_enzyme_rules->lookup_options ( pod.globalEnzymesPanel ) ;
    enhancedRefresh = pod.enhancedDisplay->GetValue() ;
    showVectorTitle = pod.vectorTitle->GetValue() ;
    showVectorLength = pod.vectorLength->GetValue() ;
    loadLastProject = pod.loadLastProject->GetValue() ;
    useMetafile = pod.useMetafile->GetValue() ;
    showSplashScreen = pod.showSplashScreen->GetValue() ;
    checkUpdate = pod.checkUpdate->GetValue() ;
    useInternalHelp = pod.useInternalHelp->GetValue() ;
    wxString lang = pod.language->GetStringSelection() ;
    if ( lang != lang_string )
        {
        wxMessageDialog md ( this , txt("t_effect_after_restart" ) ) ;
        md.ShowModal () ;
        }
    
    // Storing options
    LS->startRecord() ;
    LS->setOption ( "USEMETAFILE" , useMetafile ) ;
    LS->setOption ( "ENHANCEDREFRESH" , enhancedRefresh ) ;
    LS->setOption ( "LANGUAGE" , lang ) ;
    LS->setOption ( "SHOWVECTORTITLE" , showVectorTitle ) ;
    LS->setOption ( "SHOWVECTORLENGTH" , showVectorLength ) ;
    LS->setOption ( "LOADLASTPROJECT" , loadLastProject ) ;
    LS->setOption ( "SHOWSPLASHSCREEN" , showSplashScreen ) ;
    LS->setOption ( "CHECKUPDATE" , checkUpdate ) ;
    LS->setOption ( "USEINTERNALHELP" , useInternalHelp ) ;
    global_enzyme_rules->save_global_settings() ;
    LS->endRecord() ;
    for ( int a = 0 ; a < children.GetCount() ; a++ )
    	{
	    if ( !children[a]->vec ) continue ;
	    children[a]->vec->recalculateCuts() ;
	    children[a]->vec->recalcvisual = true ;
    	}    
   	if ( GetActiveChild() ) GetActiveChild()->Refresh () ;
   	wxEndBusyCursor() ;
    }

void MyFrame::OnProjectLoad(wxCommandEvent& event)
    {
    // Are all objects stored in a database?
    int a ;
    wxString notindb ;
    for ( a = 0 ; a < children.GetCount() ; a++ )
        {
        if ( children[a]->def == "dna" )
           {
           MyChild *c = (MyChild*) children[a] ;
           if ( c->vec->getDatabase().IsEmpty() )
              notindb += "\n" + c->getName() ;
           }
        }
    if ( !notindb.IsEmpty() )
        {
        notindb = txt("t_following_not_in_db_open") + 
                        notindb + txt("t_following_end_open") ;
        wxMessageDialog md ( this , notindb ,
                                txt("msg_box") , wxICON_EXCLAMATION|wxYES|wxNO ) ;
        if ( md.ShowModal() != wxID_YES ) return ;
        }
        
    // All are stored, or loading anyway
    TManageDatabaseDialog mdd ( this , txt("m_project_opentxt") , 
                            ACTION_MODE_LOAD|ACTION_MODE_PROJECT ) ;
    mdd.ShowModal () ;
    }

void MyFrame::OnProjectSave(wxCommandEvent& event)
    {
    // Are all objects stored in a database?
    int a ;
    wxString notindb ;
    for ( a = 0 ; a < children.GetCount() ; a++ )
        {
        if ( children[a]->def == "dna" )
           {
           MyChild *c = (MyChild*) children[a] ;
           if ( c->vec->getDatabase().IsEmpty() )
              notindb += "\n" + c->getName() ;
           }
        }
    if ( !notindb.IsEmpty() )
        {
        notindb = txt("t_following_not_in_db") + notindb + txt("t_following_end") ;
        wxMessageDialog md ( this , notindb ,
                                txt("msg_box") , wxICON_EXCLAMATION|wxYES|wxNO ) ;
        if ( md.ShowModal() != wxID_YES ) return ;
        }
        
    // All are stored, or saving anyway
    TManageDatabaseDialog mdd ( this , txt("m_project_savetxt") , 
                            ACTION_MODE_SAVE|ACTION_MODE_PROJECT ) ;
    mdd.ShowModal () ;
    }

void MyFrame::OnProjectClose(wxCommandEvent& event)
    {
    int a ;
    if ( tb_mychild )
    	{
    	tb_mychild->Close ( TRUE ) ;
    	delete tb_mychild ;
    	tb_mychild = NULL ;
     	}   	
    lastChild = NULL ;
    for ( a = 0 ; a < children.GetCount() ; a++ )
        {
//        children[a]->Close() ;
        delete children[a] ;
        }
    children.Clear() ;
    project.name = txt("project") ;
    project.desc = "" ;
    project.db = "" ;
    mainTree->initme() ;
    Refresh () ;
    }
        
void MyFrame::rememberLastProject ()
    {
    wxString sql ;
    char t[10000] ;

    sql = "DELETE FROM stuff WHERE s_type='LASTPROJECT'" ;
    LS->getObject ( sql ) ;

    sprintf ( t , "INSERT INTO stuff (s_type,s_name,s_value) VALUES "
                    "('LASTPROJECT','NAME','%s')" , project.name.c_str() ) ;
    LS->getObject ( t ) ;

    sprintf ( t , "INSERT INTO stuff (s_type,s_name,s_value) VALUES "
                    "('LASTPROJECT','DATABASE','%s')" , project.db.c_str() ) ;
    LS->getObject ( t ) ;
    }
    
TAminoAcids *MyFrame::newAminoAcids ( wxString aa , wxString title )
    {
    TVector nv ;
    nv.setSequence ( aa ) ;
    return newAminoAcids ( &nv , title ) ;
    }

TAminoAcids *MyFrame::newAminoAcids ( TVector *nv , wxString title )
    {
    int a ;
    if ( title.IsEmpty() ) title = "Surprise!" ;

    mylog ( "MyFrame::newAminoAcids" , "1" ) ;    
    TAminoAcids *subframe = new TAminoAcids ( (wxWindow*) getCommonParent() , title ) ;
    setChild ( subframe ) ;

    mylog ( "MyFrame::newAminoAcids" , "2" ) ;    
    subframe->vec->setFromVector ( *nv ) ;
    
    mylog ( "MyFrame::newAminoAcids" , "3" ) ;    
    wxString seq = subframe->vec->getSequence() ;
    seq.Replace ( "|" , "" , true ) ;
    subframe->vec->setSequence ( seq ) ;
    
    mylog ( "MyFrame::newAminoAcids" , "4" ) ;    
    subframe->vec->setWindow ( subframe ) ;
    subframe->vec->type = TYPE_AMINO_ACIDS ;
    subframe->vec->setName ( title ) ;
    subframe->vec->undo.clear() ;

    mylog ( "MyFrame::newAminoAcids" , "5" ) ;    
    for ( a = 0 ; a < nv->items.size() ; a++ )
        {
        nv->items[a].setType ( VIT_MISC ) ;
        nv->items[a].setDirection ( 1 ) ;
        }
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    mylog ( "MyFrame::newAminoAcids" , "6" ) ;    
    mainTree->addChild ( subframe , TYPE_AMINO_ACIDS ) ; 
    mylog ( "MyFrame::newAminoAcids" , "6a" ) ;    
    subframe->initme () ;
    mylog ( "MyFrame::newAminoAcids" , "7" ) ;    
    if ( locked != 0 ) return subframe ;
    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    subframe->Activate () ;
    children.Last()->Activate () ;
    mylog ( "MyFrame::newAminoAcids" , "8" ) ;    
    return subframe ;
    }

TABIviewer *MyFrame::newABI ( wxString filename , wxString title )
    {
    if ( title.IsEmpty() ) title = "Surprise!" ;
    TABIviewer *subframe = new TABIviewer ( getCommonParent() , title ) ;
    subframe->filename = filename ;
    subframe->vec->type = TYPE_SEQUENCE ;
    subframe->vec->addDescription ( "\n" + wxGetUserName() ) ;    

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    setChild ( subframe ) ;
    mainTree->addChild ( subframe , TYPE_SEQUENCE ) ;

    if ( LS->getOption ( "SHOWABIHORIZONTAL" , false ) )
        {
        subframe->sc->SilentRefresh () ;
        wxCheckBox *horiz = (wxCheckBox*) subframe->FindWindowById ( ALIGN_HORIZ ) ;
        myass ( horiz , "NULL" ) ;
        if ( horiz ) horiz->SetValue ( true ) ;
        wxCommandEvent event ;
        subframe->OnHorizontal(event) ;
        }    


    return subframe ;
    }

void MyFrame::blast ( wxString seq , wxString prg )
    {
	int a ;
	for ( a = 0 ; a < children.GetCount() && children[a]->getName() != "External Interface" ; a++ ) ;
	if ( a == children.GetCount() ) // Create new external interface
	{
	    wxCommandEvent event ;
	    OnExternalInterface ( event ) ;
	}
	activateChild ( a ) ;
	ExternalInterface *ei = (ExternalInterface*) children[a] ;
	ei->runBlast ( seq , prg ) ;
	return ;

// Obsolete
#ifdef __WXMSW__
    wxString unique = wxNow() ;
    unique.Replace ( ":" , "_" ) ;
    unique.Replace ( "/" , "_" ) ;
    unique.Replace ( " " , "_" ) ;
           
    wxString hd = myapp()->homedir ;
    wxString exe = hd + "\\blastcl3.exe" ; // WINDOWS-SPECIFIC!
    wxString ifile = hd + "\\blasts\\temp_" + unique + ".tmp" ;
    wxString ofile = hd + "\\blasts\\blast_results_" + unique + ".html" ;
    wxString bfile = hd + "\\blasts\\batch_" + unique + ".bat" ;
    
    wxString blast_dir = hd + "\\blasts" ;
    if ( !wxDir::Exists ( blast_dir ) )
       wxMkdir ( blast_dir ) ;
    
    // Writing temporary blast file
    wxFile out ( ifile , wxFile::write ) ;
    out.Write ( seq ) ;
    out.Close () ;
    
    wxFile batch ( bfile , wxFile::write ) ;
    batch.Write ( "@echo off\n" ) ;
    
    // Creating blast command line
    exe += " -p \"" + prg + "\"" ;
    exe += " -i \"" + ifile + "\"" ;
    exe += " -o \"" + ofile + "\"" ;
    exe += " -Tt" ; // Create HTML
    batch.Write ( exe + "\n" ) ;
    
    // Starting browser
    exe = "start " ;
    exe += myapp()->getHTMLCommand ( ofile ) ;
    batch.Write ( exe + "\n" ) ;

    // Delete temporary files
    batch.Write ( "del \"" + ifile + "\"\n" ) ;
    batch.Write ( "del \"" + bfile + "\"\n" ) ;

    // Run it
    batch.Close() ;
    wxExecute ( bfile ) ;
#endif
    }
    
void MyFrame::OnExternalInterface(wxCommandEvent& event)
    {
    ExternalInterface *subframe = new ExternalInterface ( getCommonParent() , txt("t_external_interface") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    }
    
void MyFrame::OnImageViewer(wxCommandEvent& event)
    {
    TImageDisplay *subframe = new TImageDisplay ( getCommonParent() , txt("t_image_viewer") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    }
    
void MyFrame::OnCalculator(wxCommandEvent& event)
    {
    RunCalculator () ;
    }    

TCalculator *MyFrame::RunCalculator ()
    {
    TCalculator *subframe = new TCalculator ( getCommonParent() , txt("t_calculator") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    return subframe ;
    }
    
wxMenu *MyFrame::getFileMenu ( bool _save , bool _exp , bool _print )
    {
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(MDI_FILE_OPEN, txt("m_open") , txt("m_opentxt") );
    if ( _save ) file_menu->Append(MDI_FILE_SAVE, txt("m_store_in_db") , txt("m_txt_store_in_db") ) ;
    file_menu->Append(MDI_FILE_IMPORT, txt("m_import") , txt("m_importtxt") );
    file_menu->Append(MDI_TEXT_IMPORT, txt("m_text_import") , txt("m_text_importtxt") );
    if ( _exp ) file_menu->Append(MDI_EXPORT, txt("m_export") , txt("m_export_txt") );
    file_menu->AppendSeparator();
    file_menu->Append(MDI_PROJECT_LOAD, txt("m_project_open") , txt("m_project_opentxt") );    
    file_menu->Append(MDI_PROJECT_SAVE, txt("m_project_save") , txt("m_project_savetxt") );    
    file_menu->Append(MDI_PROJECT_CLOSE, txt("m_project_close") , txt("m_project_closetxt") );    
    file_menu->AppendSeparator();
    if ( _print ) file_menu->Append(MDI_PRINT_IMAGE, txt("m_print_image") , txt("m_print_image_txt") );
    file_menu->Append(SEQ_PRINT, txt("m_print_sequence") ) ;
    file_menu->Append(MDI_PRINT_REPORT, txt("m_print_report") , txt("m_print_report_txt") );
    file_menu->AppendSeparator();
    file_menu->Append(MDI_QUIT, txt("m_exit") , txt("m_exittxt") ) ;
    return file_menu ;    
    }
    
wxMenu *MyFrame::getToolMenu ( bool _pcr )
    {
    wxMenu *tool_menu = new wxMenu;
    tool_menu->Append(MDI_ENZYME_EDITOR, txt("m_enzyme") ) ;
    tool_menu->Append(MDI_ALIGNMENT, txt("m_alignment") , txt("m_alignmenttxt") ) ;
    tool_menu->Append(MDI_RESTRICTION, txt("m_enzymeeditor") , txt("m_enzymeeditortxt") ) ;
    tool_menu->Append(MDI_LIGATION, txt("m_ligation") );
    if ( _pcr ) tool_menu->Append(MDI_RUN_PCR, txt("m_pcr") );
    tool_menu->Append(MDI_MANAGE_DATABASE, txt("m_manage_db") , txt("m_manage_dbtxt") ) ;
    tool_menu->Append(MDI_IMAGE_VIEWER, txt("m_image_viewer") , txt("m_image_viewer_txt") ) ;
    tool_menu->Append(MDI_EXTERNAL_INTERFACE, txt("m_external_interface") ) ;
    tool_menu->Append(MDI_CALCULATOR, txt("m_calculator") , txt("m_calculator_txt") ) ;
    tool_menu->Append(PROGRAM_OPTIONS, txt("m_options") , txt("m_options_txt") ) ;
    return tool_menu ;
    }
    
wxMenu *MyFrame::getHelpMenu ()
    {
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MDI_HELP, txt("m_help_content") ) ;
    help_menu->Append(MDI_ABOUT, txt("m_about") );
    return help_menu ;
    }
    
void MyFrame::setChild ( ChildBase *ch )
    {
    int a ;
    for ( a = 0 ; a < children.GetCount() && children[a] != ch ; a++ ) ;
    if ( a == children.GetCount() ) children.Add ( ch ) ;
    }
    
void MyFrame::removeChild ( ChildBase *ch )
    {
    int a ;
    for ( a = 0 ; a < children.GetCount() && children[a] != ch ; a++ ) ;
    if ( a == children.GetCount() ) return ;
    children[a]->Disable () ;
    children[a]->Hide() ;
//    children[a] = children[children.GetCount()-1] ;
//    children.pop_back () ;
    children.RemoveAt ( a ) ;
    lastChild = NULL ;
    activateChild ( 0 ) ;
    }

void MyFrame::activateChild ( int a )
    {
    if ( a >= children.GetCount() ) a = 0 ;
    if ( children.GetCount() )
       {
       children[a]->Activate () ;
       mainTree->EnsureVisible ( children[a]->inMainTree ) ;
       mainTree->SelectItem ( children[a]->inMainTree ) ;
       }
    else setActiveChild ( NULL ) ;
    wxSafeYield () ;
    }

void MyFrame::lockDisplay ( bool lock )
    {
    if ( lock )
        {
        if ( locked == 0 ) { mainTree->Freeze() ; Freeze() ; }
        locked++ ;
        }
    else
        {
        locked-- ;
        if ( locked == 0 )
           {
           mainTree->Thaw() ;
           Thaw() ;
           if ( !children.IsEmpty() )
              {
              if ( GetActiveChild() ) GetActiveChild()->Activate() ;
              else children.Last()->Activate () ;
              }
           }    
        }        
    }    

bool MyFrame::isLocked ()
    {
    return ( locked != 0 ) ;
    }    
        
bool MyFrame::isActivating ()
    {
    return activating ;
    }    
        
wxString MyFrame::check4update ()
    {
/*  // Deactivated due to strange error message on NT without RAS
    wxDialUpManager *dm = wxDialUpManager::Create() ;
    if ( dm && dm->IsOk() )
        {
        if ( !dm->IsOnline() && !dm->IsAlwaysOnline() )
           {
           delete dm ;
           return "" ; // Don't force the poor thing to dial for update
           }
        }
    delete dm ;*/
    
//    miniFrame->SetTitle ( txt("t_checking4update") ) ;
  
  bool error = true ;
  wxString text ;
  
#ifdef __WXMSW__
	myExternal ex ;
	text = ex.getText ( "http://gentle.magnusmanske.de/currentversion.txt" ) ;
	if ( text != "" ) error = false ;

#endif
    
    
/*    wxURL url ( "http://gentle-m.sf.net/currentversion.txt" );    
    wxInputStream *in_stream;
    in_stream = url.GetInputStream();
    wxString it , td ;
    while ( !in_stream->Eof() ) it += in_stream->GetC() ;
    if ( in_stream ) delete in_stream ;*/

/*
    wxHTTP http ;
    wxInputStream *in = http.GetInputStream ( "http://gentle.magnusmanske.de/currentversion.txt" );    
    wxString it ;
    if ( wxPROTO_NOERR == http.GetError() )
       {
        while ( !in->Eof() ) it += in->GetC() ;
        delete in ;
        wxString td = it ;
	    */
	    
    if ( !error )
    	{
	    wxString it = text ;
	    wxString td = text ;
        td = td.BeforeFirst ( '\n' ) ;
        td = td.BeforeFirst ( '\r' ) ;
//        td = td.BeforeFirst ( '\m' ) ;
        
        wxString lu = LS->getOption ( "LAST_UPDATE" , "" ) ;
        if ( lu.IsEmpty() ) // Assuming new installation of the latest version, so no update
           {
           lu = td ;
           LS->setOption ( "LAST_UPDATE" , lu ) ;
           }
           
        if ( td > lu )
           {
           wxString msg = it.AfterFirst ( '\n' ) ;
           wxMessageDialog md ( this , msg , txt("t_new_version" ) ,
                              wxOK | wxCANCEL | wxCENTRE | wxICON_INFORMATION ) ;
           if ( wxID_OK != md.ShowModal() )
              return "" ;
           
           return td ;
           }
        }
    else
    	{
//	    delete in ;
	    wxMessageBox ( "Error" ) ;
    	}    
    return "" ;
    }
    
void MyFrame::update2version ( wxString ver )
    {
    wxString do_run ;
    wxProgressDialog pd ( "Updating..." , "Downloading installer..." , 2 , NULL , 0 ) ;
    myExternal ex ;
    ex.pd = &pd ;
    do_run = myapp()->homedir + "\\GENtleSetup.exe" ;
    if ( ex.copyFile ( "http://gentle.magnusmanske.de/GENtleSetup.exe" , do_run ) > 0 )
    	{
        wxMessageBox ( "Couldn't read setup file" ) ;
        return ;
    	}    

    do_run = "\"" + do_run + "\" /S /D=\"" ;
    do_run += myapp()->homedir ;
    do_run += "\"" ;
    LS->setOption ( "LAST_UPDATE" , ver ) ;
	dying = true ;
    if ( 0 == wxExecute ( do_run , wxEXEC_ASYNC ) ) return ; // 0 means the process couldn't start :-(
    wxExit() ; // Hard exit
    }
    
void MyFrame::OnSashDrag(wxSashEvent& event)
    {
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId())
    {
        case FRAME_SASH_TOP:
        {
            m_topWindow->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
            break;
        }
        case FRAME_SASH_1:
        {
            m_leftWindow1->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
            break;
        }
        case FRAME_SASH_2:
        {
            m_leftWindow2->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
            break;
        }
        case FRAME_SASH_BOTTOM:
        {
            m_bottomWindow->SetDefaultSize(wxSize(1000, event.GetDragRect().height));
            break;
        }
    }
    wxLayoutAlgorithm layout;
    layout.LayoutFrame(this);

    // Leaves bits of itself behind sometimes
//    GetClientWindow()->Refresh();    
    }

TStorage *MyFrame::getTempDB ( wxString name )
    {
    int a ;
    for ( a = 0 ; a < dbcache.GetCount() && dbcache[a]->getDBname() != name ; a++ ) ;
    if ( a == dbcache.GetCount() ) 
        dbcache.Add ( new TStorage ( TEMP_STORAGE , name ) ) ;
    return dbcache[a] ;
    }
    
void MyFrame::setActiveChild ( ChildBase *c )
    {
    lastChild = c ;
    if ( !IsShown() ) return ;
    if ( locked != 0 ) return ;
    if ( activating ) return ;
    int a ;
    for ( a = 0 ; a < children.GetCount() ; a++ )
       {
       ChildBase *d = children[a] ;
       if ( d != c )
          {
          if ( d->IsShown() ) d->Hide () ;
          if ( d->IsEnabled() ) d->Disable () ;
          }
       }
    if ( children.GetCount() == 0 && GetMenuBar() != menu_bar ) SetMenuBar ( menu_bar ) ;
    if ( !c ) return ;
    activating = true ;
//    wxSafeYield() ;
    if ( !c->IsEnabled() ) c->Enable() ;
    if ( c->menubar && GetMenuBar() != c->menubar )
       {
       SetMenuBar ( c->menubar ) ;
       }
    wxSize s = c->GetParent()->GetClientSize() ;
    if ( c->GetSize() != s )
       {
       if ( c->vec ) c->vec->recalcvisual = true ;
       c->SetSize ( s ) ;
       }
    if ( mainTree && c->inMainTree.IsOk() && mainTree->GetSelection() != c->inMainTree )
        mainTree->SelectItem ( c->inMainTree ) ;
    if ( !c->IsShown() ) c->Show() ;
//    c->Refresh () ;
    wxSafeYield () ;
    activating = false ;
    }

wxWindow *MyFrame::getCommonParent()
    {
    return m_leftWindow2 ;
    }

void MyFrame::BollocksMenu(wxCommandEvent& event)
    {
    if ( !lastChild ) return ;
    if ( event.GetId() == MDI_NEXT_WINDOW ||
         event.GetId() == MDI_PREV_WINDOW )
       {
       int a = getChildIndex ( lastChild ) ;
       if ( event.GetId() == MDI_NEXT_WINDOW ) a++ ;
       else a-- ;
       if ( a < 0 ) a = children.GetCount()-1 ;
       if ( a >= children.GetCount() ) a = 0 ;
       if ( a >= 0 && a <= children.GetCount() )
          {
          mainTree->EnsureVisible ( children[a]->inMainTree ) ;
          mainTree->SelectItem ( children[a]->inMainTree ) ;
          }
       return ;
       }
    if ( lastChild->def != "dna" ) return ;
    lastChild->ProcessEvent ( event ) ;
    }
    
void MyFrame::RerouteMenu(wxCommandEvent& event) 
    {
    bool b = false ;
    if ( lastChild ) b = lastChild->ProcessEvent ( event ) ;
//    if ( !b ) ProcessEvent ( event ) ;
    }
    
int MyFrame::getChildIndex ( ChildBase *c )
    {
    int a ;
    for ( a = 0 ; a < children.GetCount() && children[a] != c ; a++ ) ;
    return a ;
    }

void MyFrame::OnMDIClose(wxCommandEvent& event)
    {
    if ( lastChild ) lastChild->Close ( TRUE ) ;
    }
    
TVirtualGel *MyFrame::useGel ( wxString type )
	{
	int a ;
	TVirtualGel *gel ;
	for ( a = 0 ; a < children.GetCount() ; a++ )
		{
 		if ( children[a]->def != "GEL" ) continue ;
 		gel = (TVirtualGel*) children[a] ;
 		if ( gel->type == type ) return gel ;
		}    
		
	gel = new TVirtualGel ( getCommonParent() , "GEL " + type ) ;
	gel->type = type ;

    // Give it an icon
#ifdef __WXMSW__
    gel->SetIcon(wxIcon("chrt_icn"));
#else
    gel->SetIcon(wxIcon( mondrian_xpm ));
#endif

    gel->initme () ;

    gel->Show() ;
    gel->Maximize() ;
    gel->showName() ;
    
    mainTree->addChild ( gel , TYPE_MISC ) ;
    setChild ( gel ) ;
	return gel ;
	}    

void MyFrame::TestMenu(wxCommandEvent& event)
    {
#ifdef MYTEST
    if ( test_suite == NULL ) test_suite = new TTestSuite ;
    test_suite->Step () ;
    wxCommandEvent ev ( wxEVT_COMMAND_MENU_SELECTED , Y___ ) ;
    wxPostEvent ( this , ev ) ;
#endif
    }
    	
//******************************************************************* TTestSuite

#ifdef MYTEST

TTestSuite::TTestSuite ()
	{
    srand ( time(NULL) );
    cnt = 0 ;
	}    

void TTestSuite::pressKey ( ChildBase *ac )
	{
	if ( ac->def == "dna" ) vectorPressKey ( ac ) ;
	if ( ac->def == "AminoAcids" ) vectorPressKey ( ac ) ;
	if ( ac->def == "PrimerDesign" ) vectorPressKey ( ac ) ;
	}    
	
void TTestSuite::action ( ChildBase *ac )
	{
	if ( ac->def == "dna" ) vectorAction ( ac ) ;
	if ( ac->def == "AminoAcids" ) aaAction ( ac ) ;
	}

void TTestSuite::editMode ( ChildBase *ac )
	{
	wxCommandEvent event ;
    mylog ( "Testsuite:Edit mode" , "" ) ;
    if ( ac->def == "dna" ) ((MyChild*)ac)->OnEditMode(event) ;
    if ( ac->def == "PrimerDesign" ) ((TPrimerDesign*)ac)->OnEditMode(event) ;
    if ( ac->def == "AminoAcids" )
    	{
        ((TAminoAcids*)ac)->OnEditMode(event) ;
        }    
	}
          
void TTestSuite::vectorPressKey ( ChildBase *ac )
	{
//    if ( !ac->cSequence ) { mylog ("D'oh!",""); return ;}
    wxKeyEvent ev ( wxEVT_CHAR_HOOK ) ;
    ev.m_altDown = false ;
    ev.m_controlDown = rand() % 2 ;
    ev.m_metaDown = false ;
    ev.m_shiftDown = rand() % 2 ;
    int r = rand () % 40 ;
    if ( r > 14 ) r = 9 + r % 6 ;
    wxString msg ;
    ev.m_keyCode = 'A' ; // Default
    if ( r == 0 ) { ev.m_keyCode = 'A' ; msg = "A" ; }
    if ( r == 1 ) { ev.m_keyCode = 'C' ; msg = "C" ; }
    if ( r == 2 ) { ev.m_keyCode = 'T' ; msg = "G" ; }
    if ( r == 3 ) { ev.m_keyCode = 'G' ; msg = "T" ; }
    if ( r == 4 ) { ev.m_keyCode = WXK_BACK ; msg = "BACK" ; }
    if ( r == 5 ) { ev.m_keyCode = WXK_DELETE ; msg = "DEL" ; }
    if ( r == 6 ) { ev.m_keyCode = WXK_HOME ; msg = "HOME" ; }
    if ( r == 7 ) { ev.m_keyCode = WXK_END ; msg = "END" ; }
    if ( r == 8 ) { ev.m_keyCode = WXK_INSERT ; msg = "INSERT" ; }
    if ( r == 9 ) { ev.m_keyCode = WXK_RIGHT ; msg = "RIGHT" ; }
    if ( r == 10 ) { ev.m_keyCode = WXK_LEFT ; msg = "LEFT" ; }
    if ( r == 11 ) { ev.m_keyCode = WXK_UP ; msg = "UP" ; }
    if ( r == 12 ) { ev.m_keyCode = WXK_DOWN ; msg = "DOWN" ; }
    if ( r == 13 ) { ev.m_keyCode = WXK_PRIOR ; msg = "PRIOR" ; }
    if ( r == 14 ) { ev.m_keyCode = WXK_NEXT ; msg = "NEXT" ; }
    mylog ( "Testsuite:Key" , wxString::Format ( "%s" , msg.c_str() ) ) ;
    if ( ac->def == "PrimerDesign" ) ((TPrimerDesign*)ac)->sc->OnCharHook(ev) ;
    else ac->cSequence->OnCharHook(ev) ;
	}
     
void TTestSuite::vectorAction ( ChildBase *ac )
	{
    MyChild *c = (MyChild*) ac ;
    wxCommandEvent ev ;
    int r = rand() % 24 ;
    mylog ( "Testsuite:Message" , wxString::Format ( "%d" , r ) ) ;
    switch ( r )
		{
	    case  0 : c->OnAA_none ( ev ) ; break ;
	    case  1 : c->OnAA_known ( ev ) ; break ;
	    case  2 : c->OnAA_all ( ev ) ; break ;
	    case  3 : c->OnAA_three ( ev ) ; break ;
	    case  4 : c->OnAA_one ( ev ) ; break ;
	    case  5 : c->OnAA_three_1 ( ev ) ; break ;
	    case  6 : c->OnAA_three_2 ( ev ) ; break ;
	    case  7 : c->OnAA_three_3 ( ev ) ; break ;
	    case  8 : c->OnAA_three_M1 ( ev ) ; break ;
	    case  9 : c->OnAA_three_M2 ( ev ) ; break ;
	    case 10 : c->OnAA_three_M3 ( ev ) ; break ;
	    case 11 : c->OnCircularLinear ( ev ) ; break ;
	    case 12 : c->OnMarkAll ( ev ) ; break ;
	    case 13 : c->OnCut ( ev ) ; break ;
	    case 14 : c->OnCopy ( ev ) ; break ;
//     	    case 15 : c->OnPaste ( ev ) ; break ; // Might paste *real* clipboard;-)
	    case 16 : c->OnCopyToNew ( ev ) ; break ;
	    case 17 : c->OnViewMode ( ev ) ; break ;
	    case 18 : c->OnORFs ( ev ) ; break ;
	    case 19 : c->OnToggleFeatures ( ev ) ; break ;
	    case 20 : c->OnToggleRestriction ( ev ) ; break ;
	    case 21 : c->OnToggleIDNA ( ev ) ; break ;
	    case 22 : c->Undo ( ev ) ; break ;
	    case 23 : if ( c->cSequence ) c->OnRunPCR ( ev ) ; break ;
	    }    
	}	
     
void TTestSuite::aaAction ( ChildBase *ac )
	{
    wxCommandEvent ev ;
	TAminoAcids *aa = (TAminoAcids*) ac ;
	
	int r = rand() % 15 ;
	if ( r == 0 )
		{
		int num = aa->inlinePlot->GetCount() ;
		num = rand() % num ;
		mylog ( "TTestSuite::aaAction" , wxString::Format ( "Inline plot to %d" , num ) ) ;
		aa->inlinePlot->SetSelection ( num ) ;
		aa->OnIP ( ev ) ;
		}
	else if ( r == 1 ) { mylog ( "TTestSuite::aaAction" , "OnHorizontal" ) ; aa->OnHorizontal ( ev ) ; }
	else if ( r == 2 ) { mylog ( "TTestSuite::aaAction" , "MarkAll" ) ; aa->OnMarkAll ( ev ) ; }
	else if ( r == 3 ) { mylog ( "TTestSuite::aaAction" , "Cut" ) ; aa->OnCut ( ev ) ; }
//	else if ( r == 4 ) aa->OnPhotometer ( ev ) ; // Works, too many boring calculators ;-)
	else
 		{    
 		int num = aa->lb->GetCount() ;
 		num = rand() % num ;
 		mylog ( "TTestSuite::aaAction" , wxString::Format ( "ListBox to %d" , num ) ) ; 
 		aa->lb->SetSelection ( num , true ) ;
    	aa->OnListBox ( ev ) ;
     	}   	
	}	
     
void TTestSuite::mouseEvent ( ChildBase *ac )
	{
	SequenceCanvas *can = ac->cSequence ;
	if ( !can && ac->def == "PrimerDesign" ) can = ((TPrimerDesign*)ac)->sc ;
	if ( !can ) return ;
	
	wxArrayInt vi ;
	vi.Add ( wxEVT_ENTER_WINDOW ) ;
	vi.Add ( wxEVT_LEAVE_WINDOW ) ;
	vi.Add ( wxEVT_LEFT_DOWN ) ;
	vi.Add ( wxEVT_LEFT_UP ) ;
//	vi.Add ( wxEVT_LEFT_DCLICK ) ;
	vi.Add ( wxEVT_MIDDLE_DOWN ) ;
	vi.Add ( wxEVT_MIDDLE_UP ) ;
	vi.Add ( wxEVT_MIDDLE_DCLICK ) ;
//	vi.Add ( wxEVT_RIGHT_DOWN ) ;
	vi.Add ( wxEVT_RIGHT_UP ) ;
	vi.Add ( wxEVT_RIGHT_DCLICK ) ;
	vi.Add ( wxEVT_MOTION ) ;
	vi.Add ( wxEVT_MOUSEWHEEL ) ;

	int et = vi[rand()%vi.GetCount()] ;
	
	wxMouseEvent event ( et ) ;
	int w , h ;
	can->GetClientSize ( &w , &h ) ;
	event.m_x = rand() % w ;
	event.m_y = rand() % h ;
	can->OnEvent ( event ) ;
	}	
     
void TTestSuite::Step()
    {
    if ( cnt == 0 ) start = wxGetLocalTime() ;
    MyFrame *f = myapp()->frame ;

    ChildBase *ac = f->GetActiveChild () ;
    if ( !ac ) return ; // No child open
    
    cnt++ ;
    if ( cnt > 1000000 )
    	{
	    wxMessageBox ( "1000K" ) ;
	    wxExit() ;
    	}    
    int r = rand() ;
    r %= 10 ;
    
    wxString x ;
    x = ac->getName() ;
    x += " [" + ac->def + "] " ;
    x += wxString::Format ( " [%ds, #%d]" , wxGetLocalTime()-start , cnt ) ;
    if ( ac->vec ) x += wxString::Format ( ", seq : %d" , ac->vec->getSequenceLength() ) ;
    if ( ac->cSequence ) x += wxString::Format ( ", EditMode : %d" , ac->cSequence->getEditMode() ) ;
    if ( ac->cSequence ) x += wxString::Format ( ", Overwrite : %d" , ac->cSequence->doOverwrite() ) ;
    if ( ac->cSequence ) x += wxString::Format ( ", Mark %d-%d" , ac->cSequence->markedFrom() , ac->cSequence->markedTo() ) ;
    x += wxString::Format ( ", r=%d" , r ) ;
    mylog ( "Testsuite:Status" , x ) ;
    
    if ( r < 6 ) pressKey ( ac ) ;
   	else if ( r == 6 ) mouseEvent ( ac ) ;
   	else if ( r == 7 ) editMode ( ac ) ;
   	else if ( r == 8 )
   		{
	    r = rand() % f->children.GetCount() ;
	    mylog ( "Testsuite:Activate" , wxString::Format ( "%d" , r ) ) ;
	    f->activateChild ( r ) ;
   		}    
   	else if ( r == 9 ) action ( ac ) ;
    }            

#endif
                
// DROP TARGET

bool MyFrameDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
    int a , b ;
    for ( a = 0 ; a < filenames.GetCount() ; a++ )
       {
       wxString path = filenames[a] ;
       for ( b = path.length() - 1 ; path.GetChar(b) != '\\' && path.GetChar(b) != '/' ; b-- ) ;
       wxString file = path.substr ( b+1 ) ;
       myapp()->frame->importFile ( file , path , -1 ) ;
       }
    return true ;
    }
        
// Project

TProject::TProject ()
	{
	enzyme_rules = NULL ;
	}
    
