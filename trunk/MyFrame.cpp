#include "MyFrame.h"
#include <wx/textfile.h>
#include <wx/dir.h>

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(MDI_ABOUT, MyFrame::OnAbout)
    EVT_MENU(MDI_HELP, MyFrame::OnHelp)
    EVT_MENU(MDI_ENZYME_EDITOR, MyFrame::OnEnzymeEditor)
    EVT_MENU(MDI_ALIGNMENT, MyFrame::OnAlignment)
    EVT_MENU(MDI_IMAGE_VIEWER, MyFrame::OnImageViewer)
    EVT_MENU(MDI_CALCULATOR, MyFrame::OnCalculator)
    EVT_MENU(MDI_FILE_OPEN, MyFrame::OnFileOpen)
    EVT_MENU(MDI_FILE_IMPORT, MyFrame::OnFileImport)
    EVT_MENU(MDI_TEXT_IMPORT, MyFrame::OnTextImport)
    EVT_MENU(MDI_PROJECT_SAVE, MyFrame::OnProjectSave)
    EVT_MENU(MDI_PROJECT_LOAD, MyFrame::OnProjectLoad)
    EVT_MENU(MDI_PROJECT_CLOSE, MyFrame::OnProjectClose)
    EVT_MENU(MDI_QUIT, MyFrame::OnQuit)
    EVT_MENU(MDI_MANAGE_DATABASE, MyFrame::OnManageDatabase)
    EVT_MENU(PROGRAM_OPTIONS, MyFrame::OnProgramOptions)

    EVT_SASH_DRAGGED_RANGE(FRAME_SASH_TOP, FRAME_SASH_BOTTOM, MyFrame::OnSashDrag)
    
    EVT_CLOSE(MyFrame::OnClose)

    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

// Define my frame constructor
#define ACC_ENT 32
MyFrame::MyFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : wxMDIParentFrame(parent, id, title, pos, size, style)
{
    dying = false ;

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
    
    wxAcceleratorTable accel(ACC_ENT, entries);
    SetAcceleratorTable(accel);

}

MyFrame::~MyFrame ()
    {
    rememberLastProject () ;
//    delete LS ;
    }

void MyFrame::initme ()
    {
  wxSashLayoutWindow* win ;
  m_leftWindow2 = m_topWindow = m_bottomWindow = NULL ;
/*      win = new wxSashLayoutWindow(this, FRAME_SASH_TOP,
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
/*  win = new wxSashLayoutWindow(this, FRAME_SASH_2,
                               wxDefaultPosition, wxSize(200, 30),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  win->SetDefaultSize(wxSize(120, 1000));
  win->SetOrientation(wxLAYOUT_VERTICAL);
  win->SetAlignment(wxLAYOUT_LEFT);
  win->SetBackgroundColour(wxColour(0, 255, 255));
  win->SetSashVisible(wxSASH_RIGHT, TRUE);

  m_leftWindow2 = win;*/

    // Database access
    LS = new TStorage ( LOCAL_STORAGE ) ;
    LS->createDatabase() ;

    // Synchronize enzymes in known databases
    LS->synchronize () ;
    
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
    init_txt ( lang_string ) ;

    if ( LS->getOption ( "DEBUGGING" , "" ) == "1" ) checkUpdate = false ;

#ifdef __WXMSW__
    if ( checkUpdate )
        {
        string cur_update = check4update () ;
        if ( cur_update != "" )
            {
            update2version ( cur_update ) ;
            return ;
            }
        }
#endif
        
    project_name = txt("project") ;
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

    wxMenuBar *menu_bar = new wxMenuBar;

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
#ifdef __WXGTK__
    // Toolbar
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    InitToolBar(toolBar);

    toolBar->AddTool( MDI_TEXT_IMPORT , 
                wxBitmap (myapp()->bmpdir+myapp()->slash+"new.bmp", wxBITMAP_TYPE_BMP),
                txt("m_new_sequence") ) ;  
    toolBar->AddTool( MDI_FILE_OPEN, 
            wxBitmap (myapp()->bmpdir+myapp()->slash+"open.bmp", wxBITMAP_TYPE_BMP), 
            txt("m_open") , txt("m_opentxt") );
    toolBar->AddTool( MDI_FILE_SAVE, 
                wxBitmap (myapp()->bmpdir+myapp()->slash+"save.bmp", wxBITMAP_TYPE_BMP),
                txt("m_store_in_db") , 
                txt("m_txt_store_in_db"));
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_UNDO,
        wxBitmap (myapp()->bmpdir+myapp()->slash+"undo.bmp", wxBITMAP_TYPE_BMP)) ;
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_CUT,
        wxBitmap (myapp()->bmpdir+myapp()->slash+"cut.bmp", wxBITMAP_TYPE_BMP)) ;
    toolBar->AddTool( MDI_COPY,
        wxBitmap (myapp()->bmpdir+myapp()->slash+"copy.bmp", wxBITMAP_TYPE_BMP)) ;
    toolBar->AddTool( MDI_PASTE,
        wxBitmap (myapp()->bmpdir+myapp()->slash+"paste.bmp", wxBITMAP_TYPE_BMP)) ;
    toolBar->AddSeparator() ;

//    toolBar->AddSeparator() ;
        
    toolBar->Realize() ;    
#endif

    // Load last project?
    if ( loadLastProject )
        {
        string sql , n , d ;
        TSQLresult r ;
        sql = "SELECT s_value FROM stuff WHERE s_type='LASTPROJECT' AND s_name='NAME'" ;
        r = LS->getObject ( sql ) ;
        if ( r.rows() > 0 ) n = r[0][0] ;
        sql = "SELECT s_value FROM stuff WHERE s_type='LASTPROJECT' AND s_name='DATABASE'" ;
        r = LS->getObject ( sql ) ;
        if ( r.rows() > 0 ) d = r[0][0] ;
        if ( n != "" && d != "" )
           {
           TManageDatabaseDialog mdb ( this , "dummy" ) ;
           mdb.do_load_project ( n , d ) ;
           }
        }

    // Command line parameters? BUGGY!!!!
    if ( myapp()->argc > 1 )
       {
       int a , b ;
       for ( a = 1 ; a <= myapp()->argc ; a++ )
          {
          string path = myapp()->argv[a] ;
          for ( b = path.length() - 1 ; path[b] != '\\' && path[b] != '/' ; b-- ) ;
          string file = path.substr ( b+1 ) ;
          importFile ( file , path , -1 ) ;
          }
       }

    Show(TRUE);
    }

void MyFrame::OnClose(wxCloseEvent& event)
{
    dying = true ;
    event.Skip();
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
#ifdef __WXMSW__
    string helpfile = "\"" ;
    helpfile += myapp()->homedir ;
    helpfile += "\\" ;
    helpfile += txt("f_help") ;
    helpfile += "\"" ;
    
    for ( int a = 0 ; a < helpfile.length() ; a++ )
       if ( helpfile[a] == '/' ) helpfile[a] = '\\' ;
    
    string command = myapp()->getHTMLCommand ( helpfile ) ;
    wxExecute ( command.c_str() ) ;
#else
#endif
}

void MyFrame::OnEnzymeEditor(wxCommandEvent& WXUNUSED(event) )
{   
    TVectorEditor ee ( this , txt("t_enzymeeditor") , NULL ) ;
    ee.ShowModal () ;
}

void MyFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event) )
{
    TManageDatabaseDialog dbd ( this , txt("t_open") , ACTION_MODE_LOAD ) ;
    dbd.ShowModal () ;
}

void MyFrame::OnTextImport(wxCommandEvent& WXUNUSED(event) )
{
    TextImportDialog d ( this , txt("m_text_importtxt") ) ;
    
    ChildBase *c = (ChildBase*) GetActiveChild() ;
    if ( c && c->def == "PrimerDesign" ) d.type->SetSelection ( 3 ) ;
    
    if ( d.ShowModal () != wxID_OK ) return ;

    int type = d.type->GetSelection () ;

    if ( type == 0 ) // DNA
        {
        TVector *v = new TVector ;
        v->name = d.sName ;
        v->sequence = d.sSequence ;
        v->desc = txt("manually_entered_sequence") ;
        newFromVector ( v ) ;
        }
    else if ( type == 1 ) // Amino Acids 
        {
        newAminoAcids ( d.sSequence , d.sName ) ;
        }
    else if ( type == 2 ) // GenBank
        {
        TVector *v = new TVector ;
        TGenBank gb ;
        gb.paste ( d.sequence->GetValue().c_str() ) ;
        gb.remap ( v ) ;
        if ( d.sName != "" ) v->name = d.sName ;
        newFromVector ( v ) ;
        }        
    else if ( type == 3 ) // Primer
        {
        TVector *v = new TVector ;
        v->name = d.sName ;
        v->sequence = d.sSequence ;
        v->type = TYPE_PRIMER ;
        v->desc = txt("manually_entered_sequence") ;
        newFromVector ( v , TYPE_PRIMER ) ;
        }
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
    string lastdir = LS->getOption ( "LAST_IMPORT_DIR" , "C:" ) ;
    wxFileDialog d ( this , txt("import_file") , lastdir.c_str() , 
            "" , wildcard , wxOPEN ) ;
    int x = d.ShowModal() ;
    if ( x != wxID_OK ) return ;
    
    LS->setOption ( "LAST_IMPORT_DIR" , d.GetDirectory().c_str() ) ;

    int filter = d.GetFilterIndex () - 1 ;
    importFile ( d.GetFilename().c_str() ,
                 d.GetPath().c_str() ,
                 filter ) ;
    }
    
void MyFrame::importFile ( string file , string path , int filter )
    {

    if ( filter == -1 ) // Automatic detection
        {
        
        // Trying ABI format
        ABItype dummy ;
        dummy.parse ( path ) ;
        if ( dummy.vf.size() > 0 ) // Success
           {
           newABI ( path , file ) ;
           return ;
           }

        // Trying GenBank format
        TGenBank gb ;
        gb.load ( path ) ;
        if ( gb.success )
           {
           newGB ( gb ) ;
           return ;
           }

        // Trying CLONE format
        TClone clone ;
        clone.LS = LS ;
        clone.load ( path ) ;
        if ( clone.success )
           {
           newCLONE ( clone ) ;
           return ;
           }

        // Trying FASTA format
        TFasta seq ;
        seq.load (path ) ;
        if ( seq.success )
           {
           newFASTA ( seq ) ;
//           seq.remap ( subframe->vec ) ;
//           type = TYPE_SEQUENCE ;
           return ;
           }


        wxMessageBox ( txt("t_unable_to_detect_file_type") ) ;
                         
        return ;
        }

    MyChild *subframe = new MyChild(myapp()->frame, txt("imported_vector"),
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme() ;

    int type = TYPE_VECTOR ;
    
    if ( filter == 0 )
        {
        // Loading Clone file    
        TClone clone ;
        clone.LS = LS ;
        clone.load ( path ) ;
        clone.remap ( subframe->vec ) ;
        }
    else if ( filter == 1 )
        {
        // Loading GenBank file
        TGenBank gb ;
        gb.load ( path ) ;
        gb.remap ( subframe->vec ) ;
        }
    else if ( filter == 2 )
        {
        // Loading results from sequencing
        TFasta seq ;
        seq.load ( path ) ;
        seq.remap ( subframe->vec ) ;
        type = TYPE_SEQUENCE ;
        }
    else if ( filter == 3 || filter == 4 ) // Plain text DNA/AA
        {
        string sequence ;
        wxTextFile tf ( path.c_str() ) ;
        tf.Open () ;
        for ( sequence = tf.GetFirstLine() ; !tf.Eof() ; sequence += tf.GetNextLine() ) ;
        tf.Close () ;
        
        if ( filter == 3 )
           {
           subframe->vec->name = file ;
           subframe->vec->sequence = sequence ;
           subframe->vec->desc = txt("manually_entered_sequence") ;
           }
        else if ( filter == 4 )
           {
           subframe->vec->setWindow ( subframe ) ;
           subframe->vec->undo.clear() ;
           mainTree->addChild(subframe,type) ;
           subframe->Close() ;
           delete subframe ;
           newAminoAcids ( sequence , file ) ;
           return ;
           }
        }
    else if ( filter == 5 ) // ABI
        {
        subframe->vec->setWindow ( subframe ) ;
        subframe->vec->undo.clear() ;
        mainTree->addChild(subframe,type) ;
        subframe->Close() ;
        delete subframe ;
        newABI ( path , file ) ;
        return ;
        }

    subframe->vec->setWindow ( subframe ) ;
    subframe->initPanels() ;
    mainTree->addChild(subframe,type) ;
    subframe->Maximize() ;
}


MyChild *MyFrame::newCLONE ( TClone &clone )
    {
    MyChild *subframe = new MyChild(myapp()->frame, txt("imported_vector"),
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    
    subframe->initme() ;
    int type = TYPE_VECTOR ;
    clone.remap ( subframe->vec ) ;
    subframe->vec->setWindow ( subframe ) ;
    subframe->initPanels() ;
    mainTree->addChild(subframe,type) ;
    subframe->Maximize() ;
    }

MyChild *MyFrame::newFASTA ( TFasta &fasta )
    {
    MyChild *subframe = new MyChild(myapp()->frame, txt("imported_vector"),
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    
    subframe->initme() ;
    int type = TYPE_VECTOR ;
    fasta.remap ( subframe->vec ) ;
    subframe->vec->setWindow ( subframe ) ;
    subframe->initPanels() ;
    mainTree->addChild(subframe,type) ;
    subframe->Maximize() ;
    }

MyChild *MyFrame::newGB ( TGenBank &gb )
    {
    MyChild *subframe = new MyChild(myapp()->frame, txt("imported_vector"),
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    
    subframe->initme() ;
    int type = TYPE_VECTOR ;
    gb.remap ( subframe->vec ) ;
    subframe->vec->setWindow ( subframe ) ;
    subframe->initPanels() ;
    mainTree->addChild(subframe,type) ;
    subframe->Maximize() ;
    }


MyChild* MyFrame::newFromVector ( TVector *nv , int type )
    {
    MyChild *subframe = new MyChild(this, "",
                                    wxPoint(-1, -1), wxSize(-1, -1),
                                    wxDEFAULT_FRAME_STYLE);
//    myapp()->my_children.Append(subframe);
    

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
    subframe->Maximize() ;
    return subframe ;
    }


void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
    return ;
    int w, h;
    GetClientSize(&w, &h);
    
    mainTree->SetSize(0, 0, 200, h);
    GetClientWindow()->SetSize(200, 0, w - 200, h);
}


#define NOB 2
void MyFrame::InitToolBar(wxToolBar* toolBar)
{
/*
#ifdef __WXMSW__
    toolBar->AddTool( MDI_FILE_OPEN, 
            wxBitmap (myapp()->bmpdir+"\\open.bmp", wxBITMAP_TYPE_BMP), 
            txt("m_open") , txt("m_opentxt") );
#else
    wxBitmap* bitmaps[NOB];
    bitmaps[0] = new wxBitmap( new_xpm );
    bitmaps[1] = new wxBitmap( open_xpm );
    bitmaps[2] = new wxBitmap( save_xpm );
    bitmaps[3] = new wxBitmap( copy_xpm );
    bitmaps[4] = new wxBitmap( cut_xpm );
    bitmaps[5] = new wxBitmap( paste_xpm );
    bitmaps[6] = new wxBitmap( print_xpm );
    bitmaps[7] = new wxBitmap( help_xpm );
    int i;
    for (i = 0; i < NOB; i++)
        delete bitmaps[i];
#endif
*/
/*
#ifdef __WXMSW__
    int width = 24;
#else
    int width = 16;
#endif
    int currentX = 5;
*/
}

void MyFrame::OnAlignment(wxCommandEvent& event)
    {
    TAlignment *subframe = new TAlignment ( myapp()->frame , "Alignment" ) ;
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    
    mainTree->addChild ( subframe , TYPE_ALIGNMENT ) ;
    setChild ( subframe ) ;
    }
    
void MyFrame::OnManageDatabase(wxCommandEvent& event)
    {
    TManageDatabaseDialog mdd ( this , txt("m_manage_dbtxt") ) ;
    int r = mdd.ShowModal () ;
    }

void MyFrame::OnProgramOptions(wxCommandEvent& event)
    {
    ProgramOptionsDialog pod ( this , txt("t_program_options" ) ) ;
    int r = pod.ShowModal() ;
    if ( r != wxID_OK ) return ;

    // retrieving options
    enhancedRefresh = pod.enhancedDisplay->GetValue() ;
    showVectorTitle = pod.vectorTitle->GetValue() ;
    showVectorLength = pod.vectorLength->GetValue() ;
    loadLastProject = pod.loadLastProject->GetValue() ;
    useMetafile = pod.useMetafile->GetValue() ;
    showSplashScreen = pod.showSplashScreen->GetValue() ;
    checkUpdate = pod.checkUpdate->GetValue() ;
    string lang = pod.language->GetStringSelection().c_str() ;
    if ( lang != lang_string )
        {
        wxMessageDialog md ( this , txt("t_effect_after_restart" ) ) ;
        md.ShowModal () ;
        }
    
    // Storing options
    LS->setOption ( "USEMETAFILE" , useMetafile ) ;
    LS->setOption ( "ENHANCEDREFRESH" , enhancedRefresh ) ;
    LS->setOption ( "LANGUAGE" , lang ) ;
    LS->setOption ( "SHOWVECTORTITLE" , showVectorTitle ) ;
    LS->setOption ( "SHOWVECTORLENGTH" , showVectorLength ) ;
    LS->setOption ( "LOADLASTPROJECT" , loadLastProject ) ;
    LS->setOption ( "SHOWSPLASHSCREEN" , showSplashScreen ) ;
    LS->setOption ( "CHECKUPDATE" , checkUpdate ) ;
    }

void MyFrame::OnProjectLoad(wxCommandEvent& event)
    {
    // Are all objects stored in a database?
    int a ;
    string notindb ;
    for ( a = 0 ; a < children.size() ; a++ )
        {
        if ( children[a]->def == "dna" )
           {
           MyChild *c = (MyChild*) children[a] ;
           if ( c->vec->getDatabase() == "" )
              notindb += "\n" + c->getName() ;
           }
        }
    if ( notindb != "" )
        {
        notindb = txt("t_following_not_in_db_open") + 
                        notindb + txt("t_following_end_open") ;
        wxMessageDialog md ( this , notindb.c_str() ,
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
    string notindb ;
    for ( a = 0 ; a < children.size() ; a++ )
        {
        if ( children[a]->def == "dna" )
           {
           MyChild *c = (MyChild*) children[a] ;
           if ( c->vec->getDatabase() == "" )
              notindb += "\n" + c->getName() ;
           }
        }
    if ( notindb != "" )
        {
        notindb = txt("t_following_not_in_db") + notindb + txt("t_following_end") ;
        wxMessageDialog md ( this , notindb.c_str() ,
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
    for ( a = 0 ; a < children.size() ; a++ )
        {
//        children[a]->Close() ;
        delete children[a] ;
        }
    children.clear() ;
    project_name = txt("project") ;
    project_desc = "" ;
    project_db = "" ;
    mainTree->initme() ;
    Refresh () ;
    }
        
void MyFrame::rememberLastProject ()
    {
    string sql ;
    char t[10000] ;

    sql = "DELETE FROM stuff WHERE s_type='LASTPROJECT'" ;
    LS->getObject ( sql ) ;

    sprintf ( t , "INSERT INTO stuff (s_type,s_name,s_value) VALUES "
                    "('LASTPROJECT','NAME','%s')" , project_name.c_str() ) ;
    LS->getObject ( t ) ;

    sprintf ( t , "INSERT INTO stuff (s_type,s_name,s_value) VALUES "
                    "('LASTPROJECT','DATABASE','%s')" , project_db.c_str() ) ;
    LS->getObject ( t ) ;
    }
    
TAminoAcids *MyFrame::newAminoAcids ( string aa , string title )
    {
    if ( title == "" ) title = "Surprise!" ;
    TAminoAcids *subframe = new TAminoAcids ( this , title.c_str() ) ;
    subframe->vec->sequence = aa ;
    subframe->vec->type = TYPE_AMINO_ACIDS ;

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
    
//    mainTree->addChild ( subframe , TYPE_AMINO_ACIDS ) ;
    setChild ( subframe ) ;

    return subframe ;
    }

TAminoAcids *MyFrame::newAminoAcids ( TVector *nv , string title )
    {
    int a ;
    if ( title == "" ) title = "Surprise!" ;
    TAminoAcids *subframe = new TAminoAcids ( this , title.c_str() ) ;
    
//    subframe->vec->sequence = aa ;
    subframe->vec->setFromVector ( *nv ) ;
    subframe->vec->setWindow ( subframe ) ;
    subframe->vec->type = TYPE_AMINO_ACIDS ;
    subframe->vec->name = title.c_str() ;
    subframe->vec->undo.clear() ;
    
    for ( a = 0 ; a < nv->items.size() ; a++ )
        {
        nv->items[a].type = VIT_MISC ;
        nv->items[a].direction = 1 ;
        }

//    myapp()->my_children.Append(subframe);
    
    
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
    
    mainTree->addChild ( subframe , TYPE_AMINO_ACIDS ) ;
    setChild ( subframe ) ;

    return subframe ;
    }

TABIviewer *MyFrame::newABI ( string filename , string title )
    {
    if ( title == "" ) title = "Surprise!" ;
    TABIviewer *subframe = new TABIviewer ( this , title.c_str() ) ;
    subframe->filename = filename ;
    subframe->vec->type = TYPE_SEQUENCE ;

//    myapp()->my_children.Append(subframe);
    
    
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
    
    mainTree->addChild ( subframe , TYPE_SEQUENCE ) ;
    setChild ( subframe ) ;

    return subframe ;
    }

void MyFrame::blast ( string seq , string prg )
    {
#ifdef __WXMSW__
    string unique = wxNow().c_str() ;
    for ( int a = 0 ; a < unique.length() ; a++ )
        if ( unique[a] == ':' || 
             unique[a] == '/' ||
             unique[a] == ' ' )
           unique[a] = '_' ;
           
    string hd = myapp()->homedir.c_str() ;
    string exe = hd + "\\blastcl3.exe" ; // WINDOWS-SPECIFIC!
    string ifile = hd + "\\blasts\\temp_" + unique + ".tmp" ;
    string ofile = hd + "\\blasts\\blast_results_" + unique + ".html" ;
    string bfile = hd + "\\blasts\\batch_" + unique + ".bat" ;
    
    wxString blast_dir = string ( hd + "\\blasts" ).c_str() ;
    if ( !wxDir::Exists ( blast_dir ) )
       wxMkdir ( blast_dir ) ;
    
    // Writing temporary blast file
    ofstream out ( ifile.c_str() , ios::out ) ;
    out << seq ;
    out.close() ;
    
    ofstream batch ( bfile.c_str() , ios::out ) ;
    batch << "@echo off" << endl ;
    
    // Creating blast command line
    exe += " -p \"" + prg + "\"" ;
    exe += " -i \"" + ifile + "\"" ;
    exe += " -o \"" + ofile + "\"" ;
    exe += " -Tt" ; // Create HTML
    batch << exe << endl ;
    
    // Starting browser
    exe = "start " ;
    exe += myapp()->getHTMLCommand ( ofile ) ;
//    iexplore.exe \"" + ofile + "\"" ; // WINDOWS-SPECIFIC!
    batch << exe << endl ;

    // Delete temporary files
    batch << "del \"" << ifile << "\"" << endl ;
    batch << "del \"" << bfile << "\"" << endl ;

    // Run it
    batch.close() ;
    wxExecute ( bfile.c_str() ) ;
#endif
    }
    
void MyFrame::fixMax ()
    {
    int a ;
    bool max = false ;
    for ( a = 0 ; a < children.size() ; a++ )
        {
        if ( !children[a]->IsMaximized() )
           {
           children[a]->Maximize() ;
           max = true ;
           }
        }
    }
    
void MyFrame::OnImageViewer(wxCommandEvent& event)
    {
    TImageDisplay *subframe = new TImageDisplay ( this , txt("t_image_viewer") ) ;

//    myapp()->my_children.Append(subframe);
    
    
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
    TCalculator *subframe = new TCalculator ( this , txt("t_calculator") ) ;

//    myapp()->my_children.Append(subframe);
    
    
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
    tool_menu->Append(MDI_ENZYME_EDITOR, txt("m_enzymeeditor") , txt("m_enzymeeditortxt") ) ;
    tool_menu->Append(MDI_ALIGNMENT, txt("m_alignment") , txt("m_alignmenttxt") ) ;
    tool_menu->Append(MDI_RESTRICTION, txt("m_restriction") , txt("m_restrictiontxt") ) ;
    tool_menu->Append(MDI_LIGATION, txt("m_ligation") );
    if ( _pcr ) tool_menu->Append(MDI_RUN_PCR, txt("m_pcr") );
    tool_menu->Append(MDI_MANAGE_DATABASE, txt("m_manage_db") , txt("m_manage_dbtxt") ) ;
    tool_menu->Append(MDI_IMAGE_VIEWER, txt("m_image_viewer") , txt("m_image_viewer_txt") ) ;
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
    for ( a = 0 ; a < children.size() && children[a] != ch ; a++ ) ;
    if ( a == children.size() ) children.push_back ( ch ) ;
    }
    
void MyFrame::removeChild ( ChildBase *ch )
    {
    int a ;
    for ( a = 0 ; a < children.size() && children[a] != ch ; a++ ) ;
    if ( a == children.size() ) return ;
    children[a] = children[children.size()-1] ;
    children.pop_back () ;
    }
    
string MyFrame::check4update ()
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
    
    wxString it ;
    wxFileSystem fs ;
    wxFSFile *f = fs.OpenFile ( "http://gentle.magnusmanske.de/currentversion.txt" ) ;
    if ( f )
        {
        wxInputStream *in = f->GetStream () ;
        while ( !in->Eof() ) it += in->GetC() ;
        delete in ;
        
        wxString td = it ;
        td = td.BeforeFirst ( '\n' ) ;
        td = td.BeforeFirst ( '\r' ) ;
        td = td.BeforeFirst ( '\m' ) ;
        
        string lu = LS->getOption ( "LAST_UPDATE" , "" ) ;
        if ( lu == "" ) // Assuming new installation of the latest version, so no update
           {
           lu = td.c_str() ;
           LS->setOption ( "LAST_UPDATE" , lu ) ;
           }
        
        if ( string ( td.c_str() ) > lu )
           {
           wxString msg = it.AfterFirst ( '\n' ) ;
           wxMessageDialog md ( this , msg , txt("t_new_version" ) ,
                              wxOK | wxCANCEL | wxCENTRE | wxICON_INFORMATION ) ;
           if ( wxID_OK != md.ShowModal() )
              return "" ;
           
           return td.c_str() ;
           }
        }
    return "" ;
    }
    
void MyFrame::update2version ( string ver )
    {

    string do_run ;
    wxFileSystem fs ;
    wxFSFile *f = fs.OpenFile ( "http://gentle.magnusmanske.de/GENtleSetup.exe" ) ;
    if ( f )
       {
/*       TStandbyDialog sd ( this , "Standby" , "Downloading installer..." ) ;
       sd.Show(TRUE) ;
       sd.Refresh() ;*/
       unsigned char tmp[10000] ;
       wxInputStream *in = f->GetStream () ;
       vector <unsigned char> uv ;
       while ( !in->Eof() )
          {
          in->Read ( tmp , 10000 ) ;
          int max = in->LastRead() ;
          for ( int w = 0 ; w < max ; w++ ) uv.push_back ( tmp[w] ) ;
          }
       delete in ;
       unsigned char *uc = new unsigned char [ uv.size() ] ;
       for ( int u = 0 ; u < uv.size() ; u++ ) uc[u] = uv[u] ; // __WXMSW__
       do_run = myapp()->homedir + "\\GENtleSetup.exe" ;
       wxFile out ( do_run.c_str() , wxFile::write ) ;
       out.Write ( uc , uv.size() ) ;
       out.Close () ;
       delete uc ;
//       sd.Close () ;
       }
    else
       {
       wxMessageBox ( "Couldn't read setup file" ) ;
       return ;
       }
       
    if ( do_run == "" ) return ;    
    
    LS->setOption ( "LAST_UPDATE" , ver ) ;
    wxExecute ( do_run.c_str() , wxEXEC_ASYNC ) ;
    SetFocus () ;
    Close() ;
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
    layout.LayoutMDIFrame(this);

    // Leaves bits of itself behind sometimes
    GetClientWindow()->Refresh();    
    }
    
// DROP TARGET

bool MyFrameDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
    int a , b ;
    for ( a = 0 ; a < filenames.GetCount() ; a++ )
       {
       string path = filenames[a].c_str() ;
       for ( b = path.length() - 1 ; path[b] != '\\' && path[b] != '/' ; b-- ) ;
       string file = path.substr ( b+1 ) ;
       myapp()->frame->importFile ( file , path , -1 ) ;
       }
//    wxMessageBox ( filenames[0] ) ;
    return true ;
    }