#include "MyFrame.h"
#include "SendHTTP.h"
#include <wx/sysopt.h>

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
    EVT_MENU(MDI_GRAPH, MyFrame::OnGraph)
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

/// \brief Number of wxAcceleratorEntry entries
#define ACC_ENT 41

/** \brief Constructor

	* - Initializes variables
	* - Sets the accelerator keys
*/
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

#ifndef __WXMAC__
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
    entries[20].Set(wxACCEL_CTRL, (int) 'Y', AA_ONE);
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
//    entries[38].Set(wxACCEL_CTRL, WXK_F12, MDI_GRAPH);
    entries[39].Set(wxACCEL_CTRL, WXK_F1, MDI_ABOUT);
    entries[40].Set(wxACCEL_NORMAL, WXK_F9, MDI_AUTO_ANNOTATE);
    
    wxAcceleratorTable accel(ACC_ENT, entries);
    SetAcceleratorTable(accel);
#endif
    html_ep = NULL ;
    lastChild = NULL ;
    tb_mychild = NULL ;

    // Load DNA markers
    wxTextFile tf ( _T("marker.txt") ) ;
    tf.Open () ;
    for ( unsigned int a = 0 ; a < tf.GetLineCount() ; a++ )
    {
     wxString s = tf.GetLine ( a ) ;
     s = s.Trim().Trim(false) ;
     if ( s.IsEmpty() ) continue ; // Skip blank lines
     if ( s.Left ( 1 ) == _T("#") ) continue ; // Skip comments
     dna_marker.Add ( s ) ;
    }
}

/** \brief Destructor
	
	* - Stores the last project used, so it can be reopened on the next program start
	* - Clears memory
*/
MyFrame::~MyFrame ()
    {
    if ( html_ep ) delete html_ep ;
    rememberLastProject () ;
    CLEAR_DELETE ( dbcache ) ;
//    delete LS ;
    }

/** \brief Initializes the frame

	* - Creates frame layout
	* - Accesses local database and imports enzyme list
	* - Sets options
	* - Loads language file
	* - Checks for updates online (Windows only)
	* - Menus
	* - Initialized drop target
	* - Loads bitmaps for user interface
	* - Loads last project
	* - Parses command line paramaters
*/
void MyFrame::initme ()
    {
#ifdef __WXGTK__
    SetFont ( *MYFONT ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ) ;    
#endif

	int borders = wxNO_BORDER ;

  	wxSashLayoutWindow* win ;
  	m_leftWindow2 = m_topWindow = m_bottomWindow = NULL ;

    // A window to the left of the client window
    // This contains the main tree
    win = new wxSashLayoutWindow(this, FRAME_SASH_1,
                               wxDefaultPosition, wxSize(200, 30),
                               borders | wxSW_3D | wxCLIP_CHILDREN);
    win->SetDefaultSize(wxSize(200, 1000));
    win->SetOrientation(wxLAYOUT_VERTICAL);
    win->SetAlignment(wxLAYOUT_LEFT);
    win->SetSashVisible(wxSASH_RIGHT, TRUE);
    mainTree = new TMainTree ( win , MAIN_TREE_DUMMY ) ;
    

    m_leftWindow1 = win;

    // The client window
    win = new wxSashLayoutWindow(this, FRAME_SASH_2,
                               wxDefaultPosition, wxSize(500, 30),
                               borders | wxSW_3D | wxCLIP_CHILDREN);
    win->SetDefaultSize(wxSize(500, 1000));
    win->SetOrientation(wxLAYOUT_VERTICAL);
    win->SetAlignment(wxLAYOUT_LEFT);
    win->SetSashVisible(wxSASH_RIGHT, TRUE);
    m_leftWindow2 = win;
	
    // Database access
    LS = new TStorage ( LOCAL_STORAGE ) ;
    LS->createDatabase() ;
    LS->syncEnzymes () ;
//    LS->optimizeDatabase () ; // This will VACUUM sqlite databases

    // Synchronize enzymes in known databases
    // Turned of temporarily
//    LS->synchronize () ;
    
    // Import enzymes from local database
    LS->import() ;

#ifndef MISER_MODE
    bool default_update = false ;
#else
    bool default_update = true ;
#endif

#ifdef __WXMAC__
    default_update = true ;
#endif

    // Loading program options from database
    showVectorTitle = LS->getOption ( _T("SHOWVECTORTITLE") , true ) ;
    showVectorLength = LS->getOption ( _T("SHOWVECTORLENGTH") , true ) ;
    enhancedRefresh = LS->getOption ( _T("ENHANCEDREFRESH") , true ) ;
    loadLastProject = LS->getOption ( _T("LOADLASTPROJECT") , true ) ;
    useMetafile = LS->getOption ( _T("USEMETAFILE") , false ) ;
    showSplashScreen = LS->getOption ( _T("SHOWSPLASHSCREEN") , true ) ;
    checkUpdate = LS->getOption ( _T("CHECKUPDATE") , default_update ) ;
    lang_string = LS->getOption ( _T("LANGUAGE") , _T("en") ) ;
    doRegisterStuff = LS->getOption ( _T("REGISTERSTUFF") , true ) ;
    editFeatureMode = LS->getOption ( _T("EDITFEATUREMODE") , 0 ) ;
    showStopCodon = LS->getOption ( _T("SHOWSTOPCODON") , 0 ) ;
    useCoolCanvas = LS->getOption ( _T("USECOOLCANVAS") , false ) ; // Ignored
    useInternalHelp = LS->getOption ( _T("USEINTERNALHELP") , false ) ;
    showEnzymePos = LS->getOption ( _T("SHOWENZYMEPOS") , true ) ;
    nonstandard_translation_table = LS->getOption ( _T("nonstandard_translation_table") , -1 ) ;
//#ifdef __WXMAC__
	useTwoToolbars = LS->getOption ( _T("use_two_toolbars") , true ) ;
/*#else
	useTwoToolbars = LS->getOption ( _T("use_two_toolbars") , false ) ;
#endif*/
    
    int aa_red = LS->getOption ( _T("AA_RED") , wxLIGHT_GREY->Red() ) ;
    int aa_green = LS->getOption ( _T("AA_GREEN") , wxLIGHT_GREY->Green() ) ;
    int aa_blue = LS->getOption ( _T("AA_BLUE") , wxLIGHT_GREY->Blue() ) ;
    aa_color.Set ( aa_red , aa_green , aa_blue ) ;

    stopcodon = showStopCodon == 0 ? '|' : '*' ;
	
	myapp()->init_txt ( lang_string.c_str() , _T("variables.csv") ) ;
	
	
	
    global_enzyme_rules = new TEnzymeRules ;
    global_enzyme_rules->load_global_settings() ;

    // Set the DEBUGGING option in your local test database,
    // so you won't be bothered with updates anymore :-)
    if ( LS->getOption ( _T("DEBUGGING") , _T("") ) == _T("1") ) checkUpdate = false ;

#ifndef __WXMSW__
	#ifndef __WXMAC__
		checkUpdate = false ;
	#endif
#endif

    // Check for update online
//#ifdef __WXMSW__
    if ( checkUpdate )
        {
        wxString cur_update = check4update () ;
        if ( !cur_update.IsEmpty() )
            {
            update2version ( cur_update ) ;
            return ;
            }
        }
//#endif

    project.name = txt("project") ;
    SetTitle ( txt("gentle") ) ;
    mainTree->initme() ;
    
    // Give it an icon
#ifdef __WXMSW__
    SetIcon(wxIcon("GENtle.ico",wxBITMAP_TYPE_ICO));
#elif __WXMAC__
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

    // Drag'n'drop    
    MyFrameDropTarget *dt = new MyFrameDropTarget ;
    SetDropTarget ( dt ) ;

    // Bitmap library
    wxString icondir = myapp()->bmpdir + myapp()->slash ;
    bitmaps.push_back ( wxBitmap (icondir + _T("document-new.png") , wxBITMAP_TYPE_PNG) ) ;  // 0
    bitmaps.push_back ( wxBitmap (icondir + _T("document-open.png") , wxBITMAP_TYPE_PNG) ) ; // 1
    bitmaps.push_back ( wxBitmap (icondir + _T("document-save.png") , wxBITMAP_TYPE_PNG) ) ; // 2
    bitmaps.push_back ( wxBitmap (icondir + _T("edit-undo.png") , wxBITMAP_TYPE_PNG) ) ; // 3
    bitmaps.push_back ( wxBitmap (icondir + _T("edit-cut.png") , wxBITMAP_TYPE_PNG) ) ;  // 4
    bitmaps.push_back ( wxBitmap (icondir + _T("edit-copy.png") , wxBITMAP_TYPE_PNG) ) ; // 5
    bitmaps.push_back ( wxBitmap (icondir + _T("edit-paste.png") , wxBITMAP_TYPE_PNG) ) ;// 6
    bitmaps.push_back ( wxBitmap (icondir + _T("display_circular_linear.png") , wxBITMAP_TYPE_PNG) ) ; // 7
    bitmaps.push_back ( wxBitmap (icondir + _T("display_circular_linear.png") , wxBITMAP_TYPE_PNG) ) ;   // 8
    bitmaps.push_back ( wxBitmap (icondir + _T("display_orfs.png") , wxBITMAP_TYPE_PNG) ) ; // 9
    bitmaps.push_back ( wxBitmap (icondir + _T("display_features.png") , wxBITMAP_TYPE_PNG) ) ; // 10
    bitmaps.push_back ( wxBitmap (icondir + _T("restriction_new.png") , wxBITMAP_TYPE_PNG) ) ; // 11
    bitmaps.push_back ( wxBitmap (icondir + _T("view_new.png") , wxBITMAP_TYPE_PNG) ) ; // 12
    bitmaps.push_back ( wxBitmap (icondir + _T("accessories-text-editor.png") , wxBITMAP_TYPE_PNG) ) ; // 13
    bitmaps.push_back ( wxBitmap (icondir + _T("go-down.png") , wxBITMAP_TYPE_PNG) ) ; // 14
    bitmaps.push_back ( wxBitmap (icondir + _T("go-up.png") , wxBITMAP_TYPE_PNG) ) ; // 15
    bitmaps.push_back ( wxBitmap (icondir + _T("document-print.png") , wxBITMAP_TYPE_PNG) ) ; // 16
    bitmaps.push_back ( wxBitmap (icondir + _T("alignments.png") , wxBITMAP_TYPE_PNG) ) ; // 17
    bitmaps.push_back ( wxBitmap (icondir + _T("image_viewer.png") , wxBITMAP_TYPE_PNG) ) ; // 18
    bitmaps.push_back ( wxBitmap (icondir + _T("applications-internet.png") , wxBITMAP_TYPE_PNG) ) ; // 19
    bitmaps.push_back ( wxBitmap (icondir + _T("accessories-calculator.png") , wxBITMAP_TYPE_PNG) ) ; // 20
    bitmaps.push_back ( wxBitmap (icondir + _T("preferences-desktop.png") , wxBITMAP_TYPE_PNG) ) ; // 21
    bitmaps.push_back ( wxBitmap (icondir + _T("edit-find.png") , wxBITMAP_TYPE_PNG) ) ; // 22

    bitmaps.push_back ( to_grey ( bitmaps[ 4] ) ) ; // 23 - GREY Cut
    bitmaps.push_back ( to_grey ( bitmaps[ 5] ) ) ; // 24 - GREY Copy
    bitmaps.push_back ( to_grey ( bitmaps[ 6] ) ) ; // 25 - GREY Paste
    bitmaps.push_back ( to_grey ( bitmaps[22] ) ) ; // 26 - GREY Find
    bitmaps.push_back ( to_grey ( bitmaps[ 2] ) ) ; // 27 - GREY Save
    bitmaps.push_back ( to_grey ( bitmaps[ 3] ) ) ; // 28 - GREY Undo
    bitmaps.push_back ( to_grey ( bitmaps[16] ) ) ; // 29 - GREY Print sequence


	if ( useTwoToolbars )
		{
		mainToolBar = CreateToolBar ( wxTB_HORIZONTAL ) ;
		InitToolBar ( mainToolBar ) ;
		addTool ( mainToolBar , MDI_TEXT_IMPORT ) ;
		addTool ( mainToolBar , MDI_FILE_IMPORT ) ;
		addTool ( mainToolBar , MDI_FILE_OPEN ) ;
		addTool ( mainToolBar , MDI_FILE_SAVE ) ;
		addTool ( mainToolBar , SEQ_PRINT ) ;
		addTool ( mainToolBar , MDI_UNDO ) ;
		addCCPFTools ( mainToolBar , true ) ;
		addDefaultTools ( mainToolBar ) ;
		mainToolBar->Realize() ;
		updateCCP ( NULL ) ;
		}
	else
		{
		mainToolBar = NULL ;
		}

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


    html_ep = new wxHtmlEasyPrinting ( _T("") , this ) ;
    
    wxBeginBusyCursor() ;
#ifndef MISER_MODE
    lockDisplay ( true ) ;
#endif
       
    // Load last project?
//    loadLastProject = false ;
    if ( loadLastProject )
        {
        wxString sql , n , d ;
        TSQLresult r ;
        sql = _T("SELECT s_value FROM stuff WHERE s_type='LASTPROJECT' AND s_name='NAME'") ;
        r = LS->getObject ( sql ) ;
        if ( r.rows() > 0 ) n = r[0][0] ;
        sql = _T("SELECT s_value FROM stuff WHERE s_type='LASTPROJECT' AND s_name='DATABASE'") ;
        r = LS->getObject ( sql ) ;
        if ( r.rows() > 0 ) d = r[0][0] ;
        if ( !n.IsEmpty() && !d.IsEmpty() )
           {
           TManageDatabaseDialog mdb ( this , _T("dummy") , ACTION_MODE_STARTUP ) ;
           mdb.do_load_project ( n , d ) ;
           mainTree->Refresh () ;
           }
        }
        
    // Command line parameters?
    if ( myapp()->argc > 1 )
       {
       int a ;
       wxProgressDialog pd ( txt("t_loading") , _T("") , myapp()->argc-1 , NULL , wxPD_ALL ) ;
       for ( a = 1 ; a < myapp()->argc ; a++ )
          {
          wxString path = myapp()->argv[a] ;
          if ( !pd.Update ( a-1 , path ) ) break ;
          if ( path.Left(9).Upper() == _T("GENTLE://") )
          	{
      	    path = path.Mid ( 9 ) ;
      	    if ( path.GetChar ( path.length()-1 ) == '/' ) // Trailing '/'
      	    	path = path.Mid ( 0 , path.length() - 1 ) ;
 	    	wxString db = path.BeforeFirst ( ':' ) ;
 	    	wxString name = path.AfterFirst ( ':' ) ;
      	    TManageDatabaseDialog mdb ( this , _T("dummy") , ACTION_MODE_STARTUP ) ;
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

        
#ifndef MISER_MODE
    lockDisplay ( false ) ;
#endif
    wxEndBusyCursor() ;
    Raise () ;
    mainTree->Refresh () ;
    
    SetSizeHints ( 600 , 400 ) ;
    Show(TRUE);
    m_leftWindow2->SetFocus () ;
//    SetStatusText ( txt("t_update_warning") , 1 ) ;

    }

wxBitmap MyFrame::to_grey ( wxBitmap &bmp1 )
     {
#ifndef __WXMSW__
     return wxNullBitmap ;
#endif
     wxImage i = bmp1.ConvertToImage () ;
     for ( int x = 0 ; x < i.GetWidth() ; x++ )
         {
         for ( int y = 0 ; y < i.GetHeight() ; y++ )
             {
             //if ( i.GetAplha ( x , y ) < 255 ) continue ;
             int z = ( i.GetRed(x,y) + i.GetGreen(x,y) + i.GetBlue(x,y) ) / 3 ;
             z /= 2 ;
             i.SetRGB ( x , y , z , z , z ) ;
             }
         }
     return wxBitmap ( i ) ;
     }


/** \brief Handles close event

	Scans through all the children; if any one is changed, ask the user the
	darn "are you sure" question, unless in debugging mode
*/
void MyFrame::OnClose(wxCloseEvent& event)
{
    bool canclose = true ;
    for ( unsigned int a = 0 ; canclose && a < children.GetCount() ; a++ )
        canclose = ! ( children[a]->vec && children[a]->vec->isChanged() ) ;
    if ( LS->getOption ( _T("DEBUGGING") , _T("") ) == _T("1") ) canclose = true ; // For debugging
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

/** \brief Handles the Quit command, closes
*/
void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

/** \brief Displays "About" text
*/
void MyFrame::OnAbout(wxCommandEvent& event )
{
	wxString text = txt("t_about_txt") ;
    (void)wxMessageBox(text + _T("\n\nV ") + myapp()->get_GENtle_version(), txt("t_about") );
}

/** \brief Invokes help
*/
void MyFrame::OnHelp(wxCommandEvent& event )
{
    if ( useInternalHelp )
        {
        wxHtmlHelpController *hc = new wxHtmlHelpController ( wxHF_DEFAULT_STYLE|wxHF_OPEN_FILES ) ;
        wxString helpfile ;
        helpfile += myapp()->homedir ;
        helpfile += _T("\\help\\") ;
        helpfile += lang_string ;
        helpfile += _T(".hhp") ;
        hc->AddBook ( helpfile ) ;
        hc->DisplayContents () ;
        }
    else
        {
		#ifdef __WXMAC__
            wxString helpfile = _T("\"") ;
            helpfile += myapp()->homedir ;
            helpfile += _T("/") ;
            helpfile += txt("f_help").AfterFirst ( '/' ) ;
            helpfile += _T("\"") ;

            wxString command = myapp()->getHTMLCommand ( helpfile ) ;
            wxExecute ( command ) ;
		#endif

        #ifdef __WXMSW__
            wxString helpfile = _T("\"") ;
            helpfile += myapp()->homedir ;
            helpfile += _T("\\") ;
            helpfile += txt("f_help") ;
            helpfile += _T("\"") ;
            
            for ( int a = 0 ; a < helpfile.length() ; a++ )
               if ( helpfile.GetChar(a) == '/' ) helpfile.SetChar ( a , '\\' ) ;
            
            wxString command = myapp()->getHTMLCommand ( helpfile ) ;
            wxExecute ( command ) ;
        #endif
        }
}

/** \brief Returns pointer to last (active) child
*/
ChildBase *MyFrame::GetActiveChild() //SDI
    {
    return lastChild ;
    }

/** \brief Invokes the enzyme editor
*/
void MyFrame::OnEnzymeEditor(wxCommandEvent& event )
{   
    TVectorEditor ee ( this , txt("t_enzymeeditor") , NULL ) ;
    ee.ShowModal () ;
}

/** \brief Invokes the "open from database" dialog
*/
void MyFrame::OnFileOpen(wxCommandEvent& event )
{
	unsigned int i = children.GetCount() ;
    TManageDatabaseDialog dbd ( this , txt("t_open") , ACTION_MODE_LOAD ) ;
    dbd.ShowModal () ;
    if ( i != children.GetCount() ) setActiveChild ( children[children.GetCount()-1] ) ;
    if ( GetActiveChild() ) GetActiveChild()->SetFocus() ;
}

/** \brief Invokes the "enter sequence manually" dialog and calls the appropriate creation function
*/
void MyFrame::OnTextImport(wxCommandEvent& event )
{
    TextImportDialog d ( this , txt("m_text_importtxt") ) ;
    
    ChildBase *c = (ChildBase*) GetActiveChild() ;
    if ( c && c->def == _T("PrimerDesign") ) d.type->SetSelection ( 4 ) ;
    
    if ( d.ShowModal () != wxID_OK ) return ;

    int type = d.type->GetSelection () ;

    if ( type == 0 ) // DNA
        {
        TVector *v = new TVector ;
        v->setName ( d.sName ) ;
        v->setSequence ( d.sSequence ) ;
        v->setDescription ( txt("manually_entered_sequence") + wxString ( _T("\n") + wxGetUserName() ) ) ;
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
        v->setType ( TYPE_PRIMER ) ;
        v->setDescription ( txt("manually_entered_sequence") + _T("\n") + wxGetUserName() ) ;
        newFromVector ( v , TYPE_PRIMER ) ;
        }
}

/** \brief Saves an image (actually, a wxBitmap) to a file, with "save as" dialog
*/
void MyFrame::saveImage ( wxBitmap *bmp , wxString name )
	{
    wxString wildcard ;
    wildcard += _T("PNG (*.png)|*.png") ;
//    wildcard += _T("|TIF (*.tif)|*.tif") ;
    wildcard += _T("|Bitmap (*.bmp)|*.bmp") ;
    wildcard += _T("|JPEG (*.jpg)|*.jpg") ;
    
    wxString lastdir = LS->getOption ( _T("LAST_IMPORT_DIR") , _T("C:") ) ;
    wxFileDialog d ( this , txt("t_save_image") , lastdir , name , wildcard , wxSAVE|wxOVERWRITE_PROMPT ) ;
    if ( d.ShowModal() != wxID_OK ) return ;
    wxString filename = d.GetPath() ;
    
    name.Replace ( _T("*") , _T("") , TRUE ) ;
    name.Replace ( _T(":") , _T("_") , TRUE ) ;
    name.Replace ( _T("/") , _T("_") , TRUE ) ;
    name.Replace ( _T("\\") , _T("_") , TRUE ) ;

    wxBeginBusyCursor () ;
    wxString type = filename.AfterLast('.').Upper() ;
    if ( type == _T("BMP") ) bmp->SaveFile ( filename , wxBITMAP_TYPE_BMP ) ;
    if ( type == _T("PNG") ) bmp->SaveFile ( filename , wxBITMAP_TYPE_PNG ) ;
    if ( type == _T("JPG") ) bmp->SaveFile ( filename , wxBITMAP_TYPE_JPEG ) ;
    if ( type == _T("TIF") ) bmp->SaveFile ( filename , wxBITMAP_TYPE_TIF ) ;
    wxEndBusyCursor () ;
	}    

/** \brief Handles the "import from file" dialog, then calls MyFrame::importFile 
*/
void MyFrame::OnFileImport(wxCommandEvent& event )
{
    wxString wcAutomatic = txt("format_filter_auto") ;
    wxString wcClone = txt("format_filter_clone") ;
    wxString wcGenBank = txt("format_filter_genbank") ;
    wxString wcSequencing = txt("format_filter_fasta") ;
    wxString wcPlainTextDNA = txt("format_filter_plain_dna") ;
    wxString wcPlainTextAA = txt("format_filter_plain_aa") ;
    wxString wcABIformat = txt("format_filter_abi") ;
    wxString wcCSVformat = txt("format_filter_csv") ;
    wxString wildcard = wcAutomatic +
                        _T("|") + wcClone +
                        _T("|") + wcGenBank +
                        _T("|") + wcSequencing +
                        _T("|") + wcPlainTextDNA +
                        _T("|") + wcPlainTextAA +
                        _T("|") + wcABIformat +
                        _T("|") + wcCSVformat ;
    wxString lastdir = LS->getOption ( _T("LAST_IMPORT_DIR") , _T("C:") ) ;
    wxFileDialog d ( this , txt("import_file") , lastdir , 
            _T("") , wildcard , wxOPEN | wxMULTIPLE ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;
    
    LS->setOption ( _T("LAST_IMPORT_DIR") , d.GetDirectory() ) ;

    int filter = d.GetFilterIndex () - 1 ;
    wxArrayString files , paths ;
    d.GetFilenames ( files ) ;
    d.GetPaths ( paths ) ;
    wxBeginBusyCursor();
    wxSafeYield() ;
    wxProgressDialog pd ( txt("t_loading") , _T("") , files.GetCount() , NULL , wxPD_ALL ) ;
//    lockDisplay ( true ) ;
    wxString unknown ;
    unsigned int a ;
    for ( a = 0 ; a < files.GetCount() ; a++ )
       {
       if ( !pd.Update ( a , files[a] ) ) break ;
       if ( !importFile ( files[a] , paths[a] , filter ) )
       		{
   		    if ( !unknown.IsEmpty() ) unknown += _T("\n ") ;
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
    
/** \brief Imports a file

	If a "filter" number is given, only the matching file type is tried.
	If "filter" is not given (-1), it tries to detect the type
	by opening it with all import modules until there is a match.
*/
bool MyFrame::importFile ( wxString file , wxString path , int filter )
    {
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

    // Trying spectra
    if ( filter == 6 || filter == -1 )
    	{
	    wxEndBusyCursor();
        TGraph *g = RunGraph() ;
        if ( filter == -1 && g->gd->tryall ( path ) == _T("") )
        	{
        	wxBeginBusyCursor();
   		    g->Close () ;
        	}    
        else if ( g->gd->SetupFromFile ( path ) )
        	{
        	wxBeginBusyCursor();
           	g->gd->AutoScale () ;
           	g->gd->UpdateDisplay () ;
           	wxEndBusyCursor();
           	if ( filter == 6 ) return true ;
           	}   	
       	else
       		{
        	wxBeginBusyCursor();
   		    g->Close () ;
   		    wxEndBusyCursor();
   		    if ( filter == 6 ) return false ;
       		}    
    	}    
        
    // Trying UReadSeq package
    if ( filter == 2 || filter == 3 || filter == 4 || filter == -1 )
       {
        TUReadSeq u ( path ) ;
//        if ( u.error == 0 && u.seqs.GetCount() > 0 )
        if ( u.error == 0 )
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

/** \brief Creates a new module with a phylogenetic tree
*/
TPhyloTree *MyFrame::newPhyloTree ( wxString title )
	{
	TPhyloTree *subframe = new TPhyloTree(getCommonParent(), txt("t_phylogenetic_tree")) ;
	
	setChild ( subframe ) ;
	
	// Give it an icon
#ifdef __WXMSW__
	subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
	subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif
	
	
	subframe->initme() ;
	mainTree->addChild(subframe,TYPE_MISC) ;
	subframe->Maximize() ;
	return subframe ;
	}

/** \brief Creates a new entry from an XML file (GenBankXML; DNA or amino acids)
*/
void MyFrame::newXML (  TXMLfile &xml , wxString title )
   {
   int n ;
   for ( n = 0 ; n < xml.countVectors() ; n++ )
       {
       TVector *nv = xml.getVector ( n ) ;
       short type = nv->getType() ;
       if ( !title.IsEmpty() ) nv->setName ( title ) ;
       nv->addDescription ( _T("\n") + wxGetUserName() ) ;
       if ( type == TYPE_AMINO_ACIDS )
          newAminoAcids ( nv , nv->getName() ) ;
       else
          newFromVector ( nv , type ) ;
       }
   }

/** \brief Creates a new entry from a CLONE import
*/
MyChild *MyFrame::newCLONE ( TClone &clone )
    {
    MyChild *subframe = new MyChild(getCommonParent(), txt("imported_vector"),
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);
    setChild ( subframe ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    
    subframe->initme() ;
    int type = TYPE_VECTOR ;
    clone.remap ( subframe->vec ) ;
    subframe->vec->setDescription ( subframe->vec->getDescription() + _T("\n") + wxGetUserName() ) ;
    subframe->vec->setWindow ( subframe ) ;
    subframe->initPanels() ;
    mainTree->addChild(subframe,type) ;
    subframe->Maximize() ;
	return subframe ;
    }

/** \brief Creates a new entry from a PDB import
*/
void MyFrame::newPDB ( TPDB &pdb , wxString title )
    {
    pdb.remap () ;
    for ( int a = 0 ; a < pdb.seqres.size() ; a++ )
        {
        newAminoAcids ( pdb.seqres[a].v , pdb.seqres[a].v->getName() ) -> Activate () ;
        delete pdb.seqres[a].v ;
        }    
    }
        
/** \brief Creates a new entry from a GenBank import

	This function can actually open multiple sequences from one and the same
	GenBank structure. This is used, for example, when opening alignments.
*/
void MyFrame::newGB ( TGenBank &gb , wxString title )
    {
    int n ;
    TVector *nv ;
    wxArrayTVector vv ;
    wxArrayChildBase vc ;
    wxArrayString vs ;
    bool alignment = false ;
//    wxStartTimer () ;
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
        nv->setDescription ( nv->getDescription() + _T("\n") + wxGetUserName() ) ;
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
//    wxMessageBox ( wxString::Format ( "%dms" , wxGetElapsedTime() ) ) ;
    wxEndBusyCursor() ;
    }


/** \brief Creates a new DNA entry from a TVector structure

	This is a very important function. Virtually all import/open/enter
	functions call this one to create the actual window containing
	that data.
*/
MyChild* MyFrame::newFromVector ( TVector *nv , int type )
    {
    if ( !nv ) return NULL ;
    MyChild *subframe = new MyChild(getCommonParent(), _T(""),
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);
    myass ( subframe , "MyFrame::newFromVector" ) ;
    setChild ( subframe ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
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


/** \brief Handles OnSize events
*/
void MyFrame::OnSize(wxSizeEvent& event)
{
    wxLayoutAlgorithm layout;
    layout.LayoutFrame(this);
/*
    if ( lastChild )
    	{
	    setActiveChild ( lastChild ) ;
        lastChild->Refresh() ;
        }    
*/
}


/** \brief Sets the icon size for the toolbar
*/
void MyFrame::InitToolBar(wxToolBar* toolBar)
{
	wxSize ns ;
	if ( useTwoToolbars )
		{
#ifdef __WXMAC__
		if ( toolBar == mainToolBar ) ns = wxSize ( 48 , 48 ) ;
		else ns = wxSize ( 36 , 36 ) ;
#else
		ns = wxSize ( 22 , 22 ) ;
#endif
		}
	else
		{
#ifdef __WXMAC__
		ns = wxSize ( 32 , 32 ) ;
#else
		ns = wxSize ( 22 , 22 ) ;
#endif
		}
		
     toolBar->SetToolBitmapSize ( ns ) ;
}


/** \brief Adds a tool to a given toolbar
*/
void MyFrame::addTool ( wxToolBar* toolBar , int id )
	{
	if ( mainToolBar && toolBar != mainToolBar ) return ;

	if ( id == MDI_TEXT_IMPORT )
		toolBar->AddTool( MDI_TEXT_IMPORT ,
                    bitmaps[0],
                    txt("m_new_sequence") ) ;  
	else if ( id == MDI_FILE_IMPORT )
		toolBar->AddTool( MDI_FILE_IMPORT, 
                    bitmaps[14],
                txt("m_importtxt") );
	else if ( id == MDI_FILE_OPEN )
		toolBar->AddTool( MDI_FILE_OPEN, 
                    bitmaps[1],
                txt("m_opentxt") );
	else if ( id == MDI_FILE_OPEN )
		toolBar->AddTool( MDI_FILE_OPEN, 
                    bitmaps[1],
                txt("m_opentxt") );
	else if ( id == MDI_FILE_SAVE )
        toolBar->AddTool( MDI_FILE_SAVE, 
                    txt("m_txt_store_in_db"),
                    myapp()->frame->bitmaps[2],
                    myapp()->frame->bitmaps[27] ) ;
	else if ( id == SEQ_PRINT )
        toolBar->AddTool( SEQ_PRINT, 
                    txt("m_print_sequence"),
                    myapp()->frame->bitmaps[16],
                    myapp()->frame->bitmaps[29] ) ;
	else if ( id == MDI_UNDO )
	    {
        toolBar->AddSeparator() ;
        toolBar->AddTool( MDI_UNDO, txt("m_undo_text"),
                          myapp()->frame->bitmaps[3],
                          myapp()->frame->bitmaps[28] ) ;
        }
	}

/** \brief Adds default tools to a given toolbar
*/
void MyFrame::addDefaultTools(wxToolBar* toolBar)
	{
	if ( mainToolBar && toolBar != mainToolBar ) return ;
 	toolBar->AddSeparator() ;
 	toolBar->AddTool( MDI_ALIGNMENT, myapp()->frame->bitmaps[17] , txt("m_alignment_text") ) ;
 	toolBar->AddTool( MDI_IMAGE_VIEWER, myapp()->frame->bitmaps[18] , txt("m_imageviewer_text") ) ;
 	toolBar->AddTool( MDI_CALCULATOR, myapp()->frame->bitmaps[20] , txt("m_calculator_text") ) ;
 	toolBar->AddTool( PROGRAM_OPTIONS, myapp()->frame->bitmaps[21] , txt("m_options_text") ) ;
 	toolBar->AddTool( MDI_EXTERNAL_INTERFACE, myapp()->frame->bitmaps[19] , txt("m_external_text") ) ;
   }

/** \brief Adds cut,copy,paste (and find) tools to a given toolbar
*/
void MyFrame::addCCPFTools(wxToolBar* toolBar, bool findbutton )
	{
	if ( mainToolBar && toolBar != mainToolBar ) return ;
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_CUT, txt("m_cut_text") , myapp()->frame->bitmaps[4] , myapp()->frame->bitmaps[23] ) ;
    toolBar->AddTool( MDI_COPY, txt("m_copy_text") , myapp()->frame->bitmaps[5] , myapp()->frame->bitmaps[24] ) ;
    toolBar->AddTool( MDI_PASTE, txt("m_paste_text") , myapp()->frame->bitmaps[6] , myapp()->frame->bitmaps[25] ) ;
    if ( findbutton )
       toolBar->AddTool( MDI_FIND, txt("m_find_text") , myapp()->frame->bitmaps[22] , myapp()->frame->bitmaps[26] ) ;
    if ( !mainToolBar ) toolBar->AddSeparator () ;
   }

/**	\brief Updates cut, copy, paste, find icons
*/
void MyFrame::updateCCP ( ChildBase *c )
	{
    if ( !mainToolBar && !c ) return ;
    wxToolBar *tb = mainToolBar ? mainToolBar : c->toolbar ;
	tb->EnableTool ( MDI_FILE_SAVE , c ? c->allow_save : false ) ;
	tb->EnableTool ( MDI_UNDO , c ? c->allow_undo : false ) ;
	tb->EnableTool ( SEQ_PRINT , c ? c->allow_print : false ) ;
	tb->EnableTool ( MDI_CUT , c ? c->allow_cut : false ) ;
	tb->EnableTool ( MDI_COPY , c ? c->allow_copy : false ) ;
	tb->EnableTool ( MDI_PASTE , c ? c->allow_paste : false ) ;
	tb->EnableTool ( MDI_FIND , c ? c->allow_find : false ) ;
	}

/** \brief Generates a basic tooblar
*/
void MyFrame::setDummyToolbar ( ChildBase *c )
	{
    wxToolBar *toolBar = c->CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    toolBar->Reparent ( c ) ;
    c->toolbar = toolBar ;
    myapp()->frame->InitToolBar(toolBar);
	addTool ( toolBar , MDI_TEXT_IMPORT ) ;
	addTool ( toolBar , MDI_FILE_OPEN ) ;
    c->toolbar = toolBar ;
    toolBar->Realize() ;
   }

/** \brief Creates a new alignment window
*/
TAlignment *MyFrame::runAlignment ( wxArrayString &vs , wxArrayChildBase &vc , TVector *nv )
    {
//    Thaw () ;
    TAlignment *subframe = new TAlignment ( getCommonParent() , _T("Alignment") ) ;
    setChild ( subframe ) ;
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
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
    activateChild ( children.GetCount()-1 ) ;
    return subframe ;
    }

/** \brief Handles the Alignment menu event, calls MyFrame::runAlignment
*/
void MyFrame::OnAlignment(wxCommandEvent& event)
    {
    wxArrayString vs ; // Dummy
    wxArrayChildBase vc ; // Dummy
    runAlignment ( vs , vc ) ;
    }
    
/** \brief Handles the Manage Database menu event
*/
void MyFrame::OnManageDatabase(wxCommandEvent& event)
    {
    TManageDatabaseDialog mdd ( this , txt("m_manage_dbtxt") ) ;
	mdd.ShowModal () ;
    }

/** \brief Handles the Program Options menu event, and stores the options in the local database
*/
void MyFrame::OnProgramOptions(wxCommandEvent& event)
    {
    ProgramOptionsDialog pod ( this , txt("t_program_options" ) ) ;
    global_enzyme_rules->setup_options ( pod.globalEnzymesPanel ) ;
    int r = pod.ShowModal() ;
    if ( r != wxID_OK ) return ;

   	wxBeginBusyCursor() ;

	bool useTwoToolbarsBefore = useTwoToolbars ;

    // retrieving options
//    TEnzymeRules oo == *global_enzyme_rules ;
    global_enzyme_rules->lookup_options ( pod.globalEnzymesPanel ) ;
    enhancedRefresh = pod.enhancedDisplay->GetValue() ;
    showVectorTitle = pod.vectorTitle->GetValue() ;
    showVectorLength = pod.vectorLength->GetValue() ;
    loadLastProject = pod.loadLastProject->GetValue() ;
    useMetafile = pod.useMetafile->GetValue() ;
    showSplashScreen = pod.showSplashScreen->GetValue() ;
    checkUpdate = pod.checkUpdate->GetValue() ;
    useInternalHelp = pod.useInternalHelp->GetValue() ;
    doRegisterStuff = pod.doRegisterStuff->GetValue() ;
    editFeatureMode = pod.editFeatureMode->GetSelection() ;
    showStopCodon = pod.showStopCodon->GetSelection() ;    
    showEnzymePos = pod.showEnzymePos->GetValue() ;
	useTwoToolbars = pod.useTwoToolbars->GetValue() ;
    if ( pod.use_nonstandard_translation_table->GetValue() )
    	nonstandard_translation_table = pod.translation_tables[pod.nonstandard_translation_table->GetSelection()] ;
    else nonstandard_translation_table = -1 ;
    aa_color = pod.aacol ;
    stopcodon = showStopCodon == 0 ? '|' : '*' ;
    wxString lang = pod.language->GetStringSelection() ;
    if ( lang != lang_string || useTwoToolbars != useTwoToolbarsBefore )
        {
        wxMessageDialog md ( this , txt("t_effect_after_restart" ) ) ;
        md.ShowModal () ;
        }
    
    // Storing options
    LS->startRecord() ;
    LS->setOption ( _T("USEMETAFILE") , useMetafile ) ;
    LS->setOption ( _T("ENHANCEDREFRESH") , enhancedRefresh ) ;
    LS->setOption ( _T("LANGUAGE") , lang ) ;
    LS->setOption ( _T("SHOWVECTORTITLE") , showVectorTitle ) ;
    LS->setOption ( _T("SHOWVECTORLENGTH") , showVectorLength ) ;
    LS->setOption ( _T("LOADLASTPROJECT") , loadLastProject ) ;
    LS->setOption ( _T("SHOWSPLASHSCREEN") , showSplashScreen ) ;
    LS->setOption ( _T("CHECKUPDATE") , checkUpdate ) ;
    LS->setOption ( _T("USEINTERNALHELP") , useInternalHelp ) ;
    LS->setOption ( _T("REGISTERSTUFF") , doRegisterStuff ) ;
    LS->setOption ( _T("EDITFEATUREMODE") , editFeatureMode ) ;
    LS->setOption ( _T("SHOWSTOPCODON") , showStopCodon ) ;
    LS->setOption ( _T("SHOWENZYMEPOS") , showEnzymePos ) ;
    LS->setOption ( _T("AA_RED") , aa_color.Red() ) ;
    LS->setOption ( _T("AA_GREEN") , aa_color.Green() ) ;
    LS->setOption ( _T("AA_BLUE") , aa_color.Blue() ) ;
    LS->setOption ( _T("nonstandard_translation_table") , nonstandard_translation_table ) ;
	LS->setOption ( _T("use_two_toolbars") , useTwoToolbars ) ;
    LS->setOption ( _T("SHOWTIP") , pod.showTips->GetValue() ) ;
    global_enzyme_rules->save_global_settings() ; //!!!!!!! fix this!
    LS->endRecord() ;
    for ( int a = 0 ; a < children.GetCount() ; a++ )
    	{
	    if ( !children[a]->vec ) continue ;
	    children[a]->vec->recalculateCuts() ;
//	    children[a]->vec->updateDisplay() ;
		 children[a]->EnforceRefesh () ;
    	}    
//   	if ( GetActiveChild() ) GetActiveChild()->EnforceRefesh () ;
   	wxEndBusyCursor() ;
    }

/** \brief Handles the Load Project menu event
*/
void MyFrame::OnProjectLoad(wxCommandEvent& event)
    {
    // Are all objects stored in a database?
    int a ;
    wxString notindb ;
    for ( a = 0 ; a < children.GetCount() ; a++ )
        {
        if ( children[a]->def == _T("dna") )
           {
           MyChild *c = (MyChild*) children[a] ;
           if ( c->vec->getDatabase().IsEmpty() )
              notindb += _T("\n") + c->getName() ;
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

/** \brief Handles the Save Project menu event
*/
void MyFrame::OnProjectSave(wxCommandEvent& event)
    {
    // Are all objects stored in a database?
    int a ;
    wxString notindb ;
    for ( a = 0 ; a < children.GetCount() ; a++ )
        {
        if ( children[a]->def == _T("dna") )
           {
           MyChild *c = (MyChild*) children[a] ;
           if ( c->vec->getDatabase().IsEmpty() )
              notindb += _T("\n") + c->getName() ;
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

/** \brief Handles the Close Project menu event
*/
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
    project.desc = _T("") ;
    project.db = _T("") ;
    mainTree->initme() ;
    Refresh () ;
    }
        
/** \brief Stores the last open project for reopening on program start
*/
void MyFrame::rememberLastProject ()
    {
    wxString sql ;

    sql = _T("DELETE FROM stuff WHERE s_type='LASTPROJECT'") ;
    LS->getObject ( sql ) ;
	 LS->getObject ( wxString::Format ( _T("INSERT INTO stuff (s_type,s_name,s_value) VALUES ('LASTPROJECT','NAME','%s')") , project.name.c_str() ) ) ;
	 LS->getObject ( wxString::Format ( _T("INSERT INTO stuff (s_type,s_name,s_value) VALUES ('LASTPROJECT','DATABASE','%s')") , project.db.c_str() ) ) ;
    }
    
/** \brief Creates a new amino acid entry from a string
*/
TAminoAcids *MyFrame::newAminoAcids ( wxString aa , wxString title )
    {
    TVector nv ;
    nv.setSequence ( aa ) ;
    return newAminoAcids ( &nv , title ) ;
    }

/** \brief Creates a new amino acid entry from a TVector structure

	This function basically does the same as MyFrame::newFromVector,
	but for amino acid sequences instead of DNA
*/
TAminoAcids *MyFrame::newAminoAcids ( TVector *nv , wxString title )
    {
    int a ;
    if ( title.IsEmpty() ) title = _T("Surprise!") ;

    mylog ( "MyFrame::newAminoAcids" , "1" ) ;    
    TAminoAcids *subframe = new TAminoAcids ( (wxWindow*) getCommonParent() , title ) ;
    setChild ( subframe ) ;

    mylog ( "MyFrame::newAminoAcids" , "2" ) ;    
    subframe->vec->setFromVector ( *nv ) ;
    
    mylog ( "MyFrame::newAminoAcids" , "3" ) ;    
    wxString seq = subframe->vec->getSequence() ;
    seq.Replace ( _T("|") , _T("") , true ) ;
    subframe->vec->setSequence ( seq ) ;
    
    mylog ( "MyFrame::newAminoAcids" , "4" ) ;    
    subframe->vec->setWindow ( subframe ) ;
    subframe->vec->setType ( TYPE_AMINO_ACIDS ) ;
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
#elif __WXMAC__
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

/** \brief Creates a new sequencing entry from an ABI file
*/
TABIviewer *MyFrame::newABI ( wxString filename , wxString title )
    {
    if ( title.IsEmpty() ) title = _T("Surprise!") ;
    TABIviewer *subframe = new TABIviewer ( getCommonParent() , title ) ;
    subframe->filename = filename ;
    subframe->vec->setType ( TYPE_SEQUENCE ) ;
    subframe->vec->addDescription ( _T("\n") + wxGetUserName() ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    setChild ( subframe ) ;
    mainTree->addChild ( subframe , TYPE_SEQUENCE ) ;

    if ( LS->getOption ( _T("SHOWABIHORIZONTAL") , false ) )
        {
        subframe->sc->SilentRefresh () ;
        wxCheckBox *horiz = (wxCheckBox*) subframe->FindWindowById ( ALIGN_HORIZ ) ;
        myass ( horiz , _T("NULL") ) ;
        if ( horiz ) horiz->SetValue ( true ) ;
        wxCommandEvent event ;
        subframe->OnHorizontal(event) ;
        }    


    return subframe ;
    }

/** \brief Runs a blast search by invoking MyFrame::OnExternalInterface
*/
void MyFrame::blast ( wxString seq , wxString prg )
    {
	int a ;
	for ( a = 0 ; a < children.GetCount() && children[a]->getName() != _T("External Interface") ; a++ ) ;
	if ( a == children.GetCount() ) // Create new external interface
	{
	    wxCommandEvent event ;
	    OnExternalInterface ( event ) ;
	}
	activateChild ( a ) ;
	ExternalInterface *ei = (ExternalInterface*) children[a] ;
	ei->runBlast ( seq , prg ) ;
	return ;

/*
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
*/
    }
    
/** \brief Creates a new external interface for NCBI/BLAST searching
*/
void MyFrame::OnExternalInterface(wxCommandEvent& event)
    {
    ExternalInterface *subframe = new ExternalInterface ( getCommonParent() , txt("t_external_interface") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    activateChild ( children.GetCount()-1 ) ;
    }
    
/** \brief Invokes the image viewer module
*/
void MyFrame::OnImageViewer(wxCommandEvent& event)
    {
    TImageDisplay *subframe = new TImageDisplay ( getCommonParent() , txt("t_image_viewer") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    activateChild ( children.GetCount()-1 ) ;
    }
    
/** \brief Handles the Calculator menu event by calling MyFrame::RunCalculator
*/
void MyFrame::OnCalculator(wxCommandEvent& event)
    {
    RunCalculator () ;
    }    

/** \brief Handles the Graph menu event by calling MyFrame::RunGraph
*/
void MyFrame::OnGraph(wxCommandEvent& event)
    {
	wxString seq = _T("MSPILGYWKIKGLVQPTRLLLEYLEEKYEEHLYERDEGDKWRNKKFELGLEFPNLPYYIDGDVKLTQSMAIIRYIADKHNMLGGCPKERAEISMLEGAVLDIRYGVSRIAYSKDFETLKVDFLSKLPEMLKMFEDRLCHKTYLNGDHVTHPDFMLYDALDVVLYMDPMCLDAFPKLVCFKKRIEAIPQIDKYLKSSKYIAWPLQGWQATFGGGDHPPKSDLIEGRGIPGNSS") ; // GST
//	char *s2 = new char[seq.length()+5] ;
//	strcpy ( s2 , seq.mb_str() ) ;

	wxString ret ;
	wxArrayFloat af[3] ;
	for ( int a = 0 ; a < 3 ; a++ )
		{
 		int b = a==0?14:(a==1?21:28) ;
		string s = ncoils_function ( seq.mb_str() , b ) ;
		wxString t ( s.c_str() , wxConvUTF8 ) ;
		wxArrayString ta ;
		explode ( _T("\n") , t , ta ) ;
		for ( b = 0 ; b < seq.length() ; b++ )
			{
  			if ( b >= ta.GetCount() ) break ;
  			t = ta[b].Mid ( 18 ) ;
  			double prob ;
  			t.ToDouble ( &prob ) ;
  			af[a].Add ( (float) prob ) ;
  			ret += wxString::Format ( _T("%1.4f, ") , (float) prob ) ;
			}
        ret += _T("\n") ;
		}	
     wxMessageBox ( ret ) ;
	
/*    TGraph *g = RunGraph() ;
    g->gd->SetupDummy() ;
   	g->gd->AutoScale () ;
   	g->gd->UpdateDisplay () ;*/
    }    

/** \brief Invokes the calculator module
*/
TCalculator *MyFrame::RunCalculator ()
    {
    TCalculator *subframe = new TCalculator ( getCommonParent() , txt("t_calculator") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    activateChild ( children.GetCount()-1 ) ;
    return subframe ;
    }
    
/** \brief Invokes the graph module
*/
TGraph *MyFrame::RunGraph ()
    {
    TGraph *subframe = new TGraph ( getCommonParent() , txt("t_graph") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    
    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    activateChild ( children.GetCount()-1 ) ;
    return subframe ;
    }
	
/** \brief Returns the standard File menu, with variations
	\param _save Include "Save" menu option
	\param _exp Include "Export" menu option
	\param _print Include "Print" menu option
*/
wxMenu *MyFrame::getFileMenu ( int options )
    {
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(MDI_FILE_OPEN, txt("m_open") , txt("m_opentxt") );
    if ( 0 < ( options & FILE_MENU_SAVE )  ) file_menu->Append(MDI_FILE_SAVE, txt("m_store_in_db") , txt("m_txt_store_in_db") ) ;
    file_menu->Append(MDI_FILE_IMPORT, txt("m_import") , txt("m_importtxt") );
    file_menu->Append(MDI_TEXT_IMPORT, txt("m_text_import") , txt("m_text_importtxt") );
    if ( 0 < ( options & FILE_MENU_EXPORT ) ) file_menu->Append(MDI_EXPORT, txt("m_export") , txt("m_export_txt") );
    file_menu->AppendSeparator();
    file_menu->Append(MDI_PROJECT_LOAD, txt("m_project_open") , txt("m_project_opentxt") );    
    file_menu->Append(MDI_PROJECT_SAVE, txt("m_project_save") , txt("m_project_savetxt") );    
    file_menu->Append(MDI_PROJECT_CLOSE, txt("m_project_close") , txt("m_project_closetxt") );    
    file_menu->AppendSeparator();
    if ( 0 < ( options & FILE_MENU_PRINT ) ) file_menu->Append(MDI_PRINT_IMAGE, txt("m_print_image") , txt("m_print_image_txt") );
    if ( 0 < ( options & FILE_MENU_PRINT_RESTRICTIONS ) ) file_menu->Append(MDI_PRINT_RESTRICTION_LIST, txt("m_print_restrictions") , txt("m_print_restrictions_txt") );
    file_menu->Append(SEQ_PRINT, txt("m_print_sequence") ) ;
    file_menu->Append(MDI_PRINT_REPORT, txt("m_print_report") , txt("m_print_report_txt") );
    file_menu->AppendSeparator();
    file_menu->Append(MDI_QUIT, txt("m_exit") , txt("m_exittxt") ) ;
    return file_menu ;    
    }
    
/** \brief Returns the standard Tools menu, with variations
	\param _pcr Include PCR menu option
*/
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
//    tool_menu->Append(MDI_GRAPH, txt("m_graph") , txt("m_graph_txt") ) ; // TESTING, Deactivated in release
    tool_menu->Append(PROGRAM_OPTIONS, txt("m_options") , txt("m_options_txt") ) ;
    return tool_menu ;
    }
    
/** \brief Returns the standard Help menu
*/
wxMenu *MyFrame::getHelpMenu ()
    {
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MDI_HELP, txt("m_help_content") ) ;
    help_menu->Append(MDI_ABOUT, txt("m_about") );
    return help_menu ;
    }
    
/** \brief Adds a child, if it is not already in the children list
	\param ch Pointer to child
*/
void MyFrame::setChild ( ChildBase *ch )
    {
    int a ;
    for ( a = 0 ; a < children.GetCount() && children[a] != ch ; a++ ) ;
    if ( a == children.GetCount() ) children.Add ( ch ) ;
    }
    
/** \brief Safely removes a child from the children list
	\param ch Pointer to child
*/
void MyFrame::removeChild ( ChildBase *ch )
    {
    unsigned int a ;
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

/** \brief Activates a child (brings to front, makes visible, etc.)
	\param a Number of child in children list
*/
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

/** \brief Locks/unlocks display, counts (un)lock requests
	\param lock Lock if true, unlock if not
*/
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

/** \brief Is display locked?
*/
bool MyFrame::isLocked ()
    {
    return ( locked != 0 ) ;
    }    
        
/** \brief Is a child activation sequence running?
*/
bool MyFrame::isActivating ()
    {
    return activating ;
    }    
        
/** \brief Is a new version available online?

	This function is currently a mess, due to bugs in wxWidgets,
	but it basically works...
*/
wxString MyFrame::check4update ()
    {
  bool error = true ;
  wxString text ;
  
	myExternal ex ;
	
#ifdef __WXMSW__
	text = ex.getText ( _T("http://gentle.magnusmanske.de/currentversion.txt") ) ;
#endif

#ifdef __WXMAC__
	text = ex.getText ( _T("http://gentle.magnusmanske.de/mac_currentversion.txt") ) ;
#endif

	if ( text != _T("") ) error = false ;

    if ( !error )
    	{
	    wxString it = text ;
	    wxString td = text ;
        td = td.BeforeFirst ( '\n' ) ;
        td = td.BeforeFirst ( '\r' ) ;
//        td = td.BeforeFirst ( '\m' ) ;
        
        wxString lu = LS->getOption ( _T("LAST_UPDATE") , _T("") ) ;
        if ( lu.IsEmpty() ) // Assuming new installation of the latest version, so no update
           {
           lu = td ;
           LS->setOption ( _T("LAST_UPDATE") , lu ) ;
           }
           
        if ( td > lu )
           {
           wxString msg = it.AfterFirst ( '\n' ) ;
           msg += _T("\n(") + wxString ( txt("t_you_use_version") ) + myapp()->get_GENtle_version() + _T(")") ;
           wxMessageDialog md ( this , msg , txt("t_new_version" ) ,
                              wxOK | wxCANCEL | wxCENTRE | wxICON_INFORMATION ) ;
           if ( wxID_OK != md.ShowModal() )
              return _T("") ;
           
           return td ;
           }
        }
    else
    	{
//	    delete in ;
	    wxMessageBox ( _T("Error") ) ;
    	}    
    return _T("") ;
    }
    
/** \brief Download the new version of GENtle
	\param ver New version
*/
void MyFrame::update2version ( wxString ver )
    {
    wxString do_run ;
    wxProgressDialog pd ( _T("Updating...") , _T("Downloading installer...") , 2 , NULL , 0 ) ;
    myExternal ex ;
    ex.pd = &pd ;

	wxString sourcefile , localfile ;

#ifdef __WXMSW__
    localfile = myapp()->homedir + _T("\\GENtleSetup.exe") ;
	sourcefile = _T("http://gentle.magnusmanske.de/GENtleSetup.exe") ;
#endif

#ifdef __WXMAC__
	localfile = wxString(wxGetUserHome()) + _T("/Desktop/GENtle.pkg.zip") ;
	sourcefile = _T("http://gentle.magnusmanske.de/GENtle.pkg.zip") ;
#endif

    if ( ex.copyFile ( sourcefile , localfile ) > 0 )
    	{
        wxMessageBox ( _T("Couldn't read setup file") ) ;
        return ;
    	}    

#ifdef __WXMSW__
    do_run = _T("\"") + localfile + _T("\" /D=\"") ;
    do_run += myapp()->homedir ;
    do_run += _T("\"") ;
    LS->setOption ( _T("LAST_UPDATE") , ver ) ;
	dying = true ;
    if ( 0 == wxExecute ( do_run , wxEXEC_ASYNC ) ) return ; // 0 means the process couldn't start :-(
    wxExit() ; // Hard exit
#endif

#ifdef __WXMAC__
	do_run = _T("unzip -o ") + localfile + _T(" -d ") + wxGetUserHome() + _T("/Desktop") ;
	wxExecute ( do_run , wxEXEC_SYNC ) ;
	wxRemoveFile ( localfile ) ;
	localfile = wxString(wxGetUserHome()) + _T("/Desktop/GENtle_package.pkg") ;
	do_run = _T("open ") + localfile ;
    LS->setOption ( _T("LAST_UPDATE") , ver ) ;
	dying = true ;
	wxExecute ( do_run , wxEXEC_ASYNC ) ;
    Destroy() ;
#endif
    }
    
/** \brief Handles SashDrag events (sash between the main tree and the current child
*/
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
	Refresh () ;

    // Leaves bits of itself behind sometimes
//    GetClientWindow()->Refresh();    
    }

/** \brief Creates a temporary database, or uses one from the cache
	\param name Name of the database to use
*/
TStorage *MyFrame::getTempDB ( wxString name )
    {
    unsigned int a ;
    for ( a = 0 ; a < dbcache.GetCount() && dbcache[a]->getDBname() != name ; a++ ) ;
    if ( a == dbcache.GetCount() ) 
        dbcache.Add ( new TStorage ( TEMP_STORAGE , name ) ) ;
    return dbcache[a] ;
    }
    
/** \brief Sets the active child
	\param c Pointer to the child
*/
void MyFrame::setActiveChild ( ChildBase *c )
    {
    lastChild = c ;
    if ( !IsShown() ) return ;
    if ( locked != 0 ) return ;
    if ( activating ) return ;
    unsigned int a ;
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
		c->SetMyMenuBar () ;
       //SetMenuBar ( c->menubar ) ;
    wxSize s = c->GetParent()->GetClientSize() ;
    if ( c->GetSize() != s )
       {
       if ( c->vec ) c->vec->updateDisplay() ;
       c->SetSize ( s ) ;
       }
    if ( mainTree && c->inMainTree.IsOk() && mainTree->GetSelection() != c->inMainTree )
        mainTree->SelectItem ( c->inMainTree ) ;
    if ( !c->IsShown() ) c->Show() ;
//    c->Refresh () ;
    wxSafeYield () ;
    activating = false ;
    }

/** \brief Returns the base window for all children
*/
wxWindow *MyFrame::getCommonParent()
    {
    return m_leftWindow2 ;
    }

/** \brief Bug-patch function to catch illegal events
*/
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
    if ( lastChild->def != _T("dna") ) return ;
    lastChild->ProcessEvent ( event ) ;
    }
    
/** \brief Reroutes menu events from the frame to the active child
*/
void MyFrame::RerouteMenu(wxCommandEvent& event) 
    {
    bool b = false ;
    if ( lastChild ) b = lastChild->ProcessEvent ( event ) ;
//    if ( !b ) ProcessEvent ( event ) ;
    }
    
/** \brief Gets the number of a child in the children list
	\param c Pointer to child
*/
int MyFrame::getChildIndex ( ChildBase *c )
    {
    int a ;
    for ( a = 0 ; a < children.GetCount() && children[a] != c ; a++ ) ;
    return a ;
    }


/** \brief Closes MDI child via (Crtl-F4)
*/
void MyFrame::OnMDIClose(wxCommandEvent& event)
    {
    if ( lastChild ) lastChild->Close ( TRUE ) ;
    }
    
/** \brief Creates/reuses a virtual gel
*/
TVirtualGel *MyFrame::useGel ( wxString type )
	{
	int a ;
	TVirtualGel *gel ;
	for ( a = 0 ; a < children.GetCount() ; a++ )
		{
 		if ( children[a]->def != _T("GEL") ) continue ;
 		gel = (TVirtualGel*) children[a] ;
 		if ( gel->type == type ) return gel ;
		}    
		
	gel = new TVirtualGel ( getCommonParent() , _T("GEL ") + type ) ;
	gel->type = type ;

    // Give it an icon
#ifdef __WXMSW__
    gel->SetIcon(wxIcon("chrt_icn"));
#elif __WXMAC__
#else
    gel->SetIcon(wxIcon( mondrian_xpm ));
#endif

    gel->initme () ;

    gel->Show() ;
    gel->Maximize() ;
    gel->showName() ;
    
    mainTree->addChild ( gel , TYPE_MISC ) ;
    setChild ( gel ) ;
    activateChild ( children.GetCount()-1 ) ;
	return gel ;
	}    

/** \brief Handles the menu request for running the test suite
*/
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
	if ( ac->def == _T("dna") ) vectorPressKey ( ac ) ;
	if ( ac->def == _T("AminoAcids") ) vectorPressKey ( ac ) ;
	if ( ac->def == _T("PrimerDesign") ) vectorPressKey ( ac ) ;
	}    
	
void TTestSuite::action ( ChildBase *ac )
	{
	if ( ac->def == _T("dna") ) vectorAction ( ac ) ;
	if ( ac->def == _T("AminoAcids") ) aaAction ( ac ) ;
	}

void TTestSuite::editMode ( ChildBase *ac )
	{
	wxCommandEvent event ;
    mylog ( "Testsuite:Edit mode" , "" ) ;
    if ( ac->def == _T("dna") ) ((MyChild*)ac)->OnEditMode(event) ;
    if ( ac->def == _T("PrimerDesign") ) ((TPrimerDesign*)ac)->OnEditMode(event) ;
    if ( ac->def == _T("AminoAcids") )
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
    if ( r == 0 ) { ev.m_keyCode = 'A' ; msg = _T("A") ; }
    if ( r == 1 ) { ev.m_keyCode = 'C' ; msg = _T("C") ; }
    if ( r == 2 ) { ev.m_keyCode = 'T' ; msg = _T("G") ; }
    if ( r == 3 ) { ev.m_keyCode = 'G' ; msg = _T("T") ; }
    if ( r == 4 ) { ev.m_keyCode = WXK_BACK ; msg = _T("BACK") ; }
    if ( r == 5 ) { ev.m_keyCode = WXK_DELETE ; msg = _T("DEL") ; }
    if ( r == 6 ) { ev.m_keyCode = WXK_HOME ; msg = _T("HOME") ; }
    if ( r == 7 ) { ev.m_keyCode = WXK_END ; msg = _T("END") ; }
    if ( r == 8 ) { ev.m_keyCode = WXK_INSERT ; msg = _T("INSERT") ; }
    if ( r == 9 ) { ev.m_keyCode = WXK_RIGHT ; msg = _T("RIGHT") ; }
    if ( r == 10 ) { ev.m_keyCode = WXK_LEFT ; msg = _T("LEFT") ; }
    if ( r == 11 ) { ev.m_keyCode = WXK_UP ; msg = _T("UP") ; }
    if ( r == 12 ) { ev.m_keyCode = WXK_DOWN ; msg = _T("DOWN") ; }
    if ( r == 13 ) { ev.m_keyCode = WXK_PRIOR ; msg = _T("PRIOR") ; }
    if ( r == 14 ) { ev.m_keyCode = WXK_NEXT ; msg = _T("NEXT") ; }
    mylog ( "Testsuite:Key" , wxString::Format ( "%s" , msg.c_str() ) ) ;
    if ( ac->def == _T("PrimerDesign") ) ((TPrimerDesign*)ac)->sc->OnCharHook(ev) ;
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
	if ( !can && ac->def == _T("PrimerDesign") ) can = ((TPrimerDesign*)ac)->sc ;
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
	    wxMessageBox ( _T("1000K") ) ;
	    wxExit() ;
    	}    
    int r = rand() ;
    r %= 10 ;
    
    wxString x ;
    x = ac->getName() ;
    x += _T(" [") + ac->def + _T("] ") ;
    x += wxString::Format ( _T(" [%ds, #%d]") , wxGetLocalTime()-start , cnt ) ;
    if ( ac->vec ) x += wxString::Format ( _T(", seq : %d") , ac->vec->getSequenceLength() ) ;
    if ( ac->cSequence ) x += wxString::Format ( _T(", EditMode : %d") , ac->cSequence->getEditMode() ) ;
    if ( ac->cSequence ) x += wxString::Format ( _T(", Overwrite : %d") , ac->cSequence->doOverwrite() ) ;
    if ( ac->cSequence ) x += wxString::Format ( _T(", Mark %d-%d") , ac->cSequence->markedFrom() , ac->cSequence->markedTo() ) ;
    x += wxString::Format ( _T(", r=%d") , r ) ;
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

/** \brief Handles dropped file name(s)
*/
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

/** \brief Constructor
*/
TProject::TProject ()
	{
	enzyme_rules = NULL ;
	}

TEnzymeRules *TProject::getEnzymeRules () { return enzyme_rules ; }
void TProject::setEnzymeRules ( TEnzymeRules *er ) { enzyme_rules = er ; }
    
