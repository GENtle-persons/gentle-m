#include "MyFrame.h"
#include <wx/textfile.h>
#include <wx/dir.h>
#include <wx/html/helpctrl.h>

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, MyFrameType)
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
    EVT_MENU(MDI_CLOSE, MyFrame::OnMDIClose)
    EVT_MENU(MDI_MANAGE_DATABASE, MyFrame::OnManageDatabase)
    EVT_MENU(PROGRAM_OPTIONS, MyFrame::OnProgramOptions)
    
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
#define ACC_ENT 37
MyFrame::MyFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : MyFrameType(parent, id, title, pos, size, style)
{
    dying = false ;
    locked = 0 ;

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
    entries[33].Set(wxACCEL_CTRL, WXK_F4, MDI_CLOSE);
    entries[34].Set(wxACCEL_CTRL, WXK_TAB, MDI_NEXT_WINDOW);
    entries[35].Set(wxACCEL_CTRL|wxACCEL_SHIFT, WXK_TAB, MDI_PREV_WINDOW);
    entries[36].Set(wxACCEL_NORMAL, WXK_F8, MDI_TOGGLE_IDNA);
    
    wxAcceleratorTable accel(ACC_ENT, entries);
    SetAcceleratorTable(accel);
    html_ep = NULL ;
    lastChild = NULL ;
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

    // Set the DEBUGGING option in your local test database,
    // so you won't be bothered with updates anymore :-)
    if ( LS->getOption ( "DEBUGGING" , "" ) == "1" ) checkUpdate = false ;

//    miniFrame = new wxMiniFrame ( this , -1 , "" , wxDefaultPosition , wxSize ( 250 , 20 ) , wxTINY_CAPTION_HORIZ ) ;
//    miniFrame->Center() ;
//    miniFrame->Show() ;

#ifdef __WXMSW__
    if ( checkUpdate )
        {
        wxString cur_update = check4update () ;
        if ( !cur_update.IsEmpty() )
            {
            update2version ( cur_update ) ;
//            miniFrame->Close() ;
//            delete miniFrame ;
            return ;
            }
        }
#endif

//    miniFrame->Close() ;
//    delete miniFrame ;
        
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
/*
#ifdef __WXGTK_______
    // Toolbar
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    InitToolBar(toolBar);
    toolBar->AddTool( MDI_TEXT_IMPORT , 
                bitmaps[0] ,
                txt("m_new_sequence") ) ;  
    toolBar->AddTool( MDI_FILE_OPEN, 
                bitmaps[1] ,
                txt("m_open") , txt("m_opentxt") );
    toolBar->AddTool( MDI_FILE_SAVE, 
                bitmaps[2] ,
                txt("m_store_in_db") , 
                txt("m_txt_store_in_db"));
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_UNDO, bitmaps[3] ) ;
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_CUT, bitmaps[4] ) ;
    toolBar->AddTool( MDI_COPY, bitmaps[5] ) ;
    toolBar->AddTool( MDI_PASTE, bitmaps[6] ) ;
    toolBar->AddSeparator() ;
    toolBar->Realize() ;    
#endif
*/
    html_ep = new wxHtmlEasyPrinting ( "" , this ) ;
    
    wxBeginBusyCursor() ;       
       
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
       for ( a = 1 ; a < myapp()->argc ; a++ )
          {
          wxString path = myapp()->argv[a] ;
          wxString file = path.AfterLast ( '/' ) ;
          file = file.AfterLast ( '\\' ) ;
          importFile ( file , path , -1 ) ;
          wxSetWorkingDirectory ( myapp()->homedir ) ;
          }
       }
        
    wxEndBusyCursor() ;
    

       
    SetSizeHints ( 600 , 400 ) ;
    Show(TRUE);
    if ( children.GetCount() ) children.Last()->Activate () ;
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
    TManageDatabaseDialog dbd ( this , txt("t_open") , ACTION_MODE_LOAD ) ;
    dbd.ShowModal () ;
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
    wxDialog dlg ( NULL , -1 , "Test" , wxDefaultPosition , wxSize ( 300 , 20 ) ) ;
    if ( files.GetCount() > 1 ) dlg.Show() ;
    lockDisplay ( true ) ;
    for ( int a = 0 ; a < files.GetCount() ; a++ )
       {
       wxString msg = wxString::Format ( txt("t_loading") , files[a].c_str() ) ;
       dlg.SetTitle ( msg ) ;
       dlg.SetFocus() ;
       importFile ( files[a] , paths[a] , filter ) ;
       }
    lockDisplay ( false ) ;
    SetFocus () ;
    wxEndBusyCursor();
    mainTree->Refresh () ;
    Refresh () ;
    activateChild ( getChildIndex ( lastChild ) ) ;
    }
    
void MyFrame::importFile ( wxString file , wxString path , int filter )
    {
    // Trying ABI format
    if ( filter == 5 || filter == -1 )
       {
        ABItype dummy ;
        dummy.parse ( path ) ;
        if ( dummy.vf.size() > 0 ) // Success
           {
           newABI ( path , file ) ;
           return ;
           }
        }

    // Trying GenBank format
    if ( filter == 1 || filter == -1 )
       {
       TGenBank gb ;
       gb.load ( path ) ;
       if ( gb.success )
          {
          newGB ( gb , file ) ;
          return ;
          }
       }
       
    // Trying PDB format
    if ( filter == 1 || filter == -1 )
       {
        TPDB pdb ;
        pdb.load ( path ) ;
        if ( pdb.success )
           {
           newPDB ( pdb , file ) ;
           return ;
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
           return ;
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
           return ;
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
           return ;
           }
       }

    wxMessageBox ( txt("t_unable_to_detect_file_type") , file ) ;
    return ;
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
        newAminoAcids ( pdb.seqres[a].v , pdb.seqres[a].v->getName() ) ;
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
    for ( n = 0 ; n < gb.vs_l.size() ; n++ )
        {
        gb.vs = gb.vs_l[n] ;
        gb.vi = gb.vi_l[n] ;
        nv = new TVector ;
        gb.remap ( nv ) ;
        vs.Add ( nv->getSequence() ) ;
        vv.Add ( nv ) ;
        nv->setDescription ( nv->getDescription() + "\n" + wxGetUserName() ) ;
        if ( nv->getSequenceLength() != vv[0]->getSequenceLength() ) alignment = false ;
        else if ( nv->getSequence().find ( '-' ) != -1 ) alignment = true ;
        }
    if ( gb.vs_l.size() == 1 ) alignment = false ;
        
    // Removing alignment artifacts from sequences
    for ( n = 0 ; n < vv.GetCount() ; n++ )
        {
        for ( int m = 0 ; m < vv[n]->getSequenceLength() ; m++ )
           {
           if ( vv[n]->getSequenceChar(m) == '-' )
              {
              vv[n]->eraseSequence ( m , 1 ) ;
              m-- ;
              }
           }
        }
        
    if ( alignment )
        {
        if ( wxYES != wxMessageBox ( txt("t_possible_alignment") ,
                       title , wxYES_NO|wxICON_QUESTION ) )
           {
           alignment = false ;
           }        
        }
        
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
    if ( lastChild ) lastChild->Maximize() ;
    return ;
}


#define NOB 2
void MyFrame::InitToolBar(wxToolBar* toolBar)
{
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
    useInternalHelp = pod.useInternalHelp->GetValue() ;
    wxString lang = pod.language->GetStringSelection() ;
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
    LS->setOption ( "USEINTERNALHELP" , useInternalHelp ) ;
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
    for ( a = 0 ; a < children.GetCount() ; a++ )
        {
//        children[a]->Close() ;
        delete children[a] ;
        }
    children.Clear() ;
    project_name = txt("project") ;
    project_desc = "" ;
    project_db = "" ;
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
                    "('LASTPROJECT','NAME','%s')" , project_name.c_str() ) ;
    LS->getObject ( t ) ;

    sprintf ( t , "INSERT INTO stuff (s_type,s_name,s_value) VALUES "
                    "('LASTPROJECT','DATABASE','%s')" , project_db.c_str() ) ;
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
    
    TAminoAcids *subframe = new TAminoAcids ( (wxWindow*) getCommonParent() , title ) ;
    setChild ( subframe ) ;

    subframe->vec->setFromVector ( *nv ) ;
    
    wxString seq = subframe->vec->getSequence() ;
    seq.Replace ( "|" , "" , true ) ;
    subframe->vec->setSequence ( seq ) ;
    
    subframe->vec->setWindow ( subframe ) ;
    subframe->vec->type = TYPE_AMINO_ACIDS ;
    subframe->vec->setName ( title ) ;
    subframe->vec->undo.clear() ;
    
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

    subframe->initme () ;
    mainTree->addChild ( subframe , TYPE_AMINO_ACIDS ) ;
    if ( locked != 0 ) return subframe ;
    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;
    subframe->Activate () ;
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
    
    mainTree->addChild ( subframe , TYPE_SEQUENCE ) ;
    setChild ( subframe ) ;

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
    }

void MyFrame::lockDisplay ( bool lock )
    {
    if ( lock )
        {
        if ( locked == 0 ) mainTree->Freeze() ;
        locked++ ;
        }
    else
        {
        locked-- ;
        if ( locked == 0 )
           {
           mainTree->Thaw() ;
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
    return "" ;
    }
    
void MyFrame::update2version ( wxString ver )
    {
    wxString do_run ;
    wxFileSystem fs ;
    wxFSFile *f = fs.OpenFile ( "http://gentle.magnusmanske.de/GENtleSetup.exe" ) ;
    if ( f )
       {
/*       TStandbyDialog sd ( this , "Standby" , "Downloading installer..." ) ;
       sd.Show(TRUE) ;
       sd.Refresh() ;*/
//       miniFrame->SetTitle ( txt("t_downloading_new_version") ) ;
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
       wxFile out ( do_run , wxFile::write ) ;
       out.Write ( uc , uv.size() ) ;
       out.Close () ;
       delete uc ;
       do_run = "\"" + do_run + "\" /S /D=\"" ;
       do_run += myapp()->homedir ;
       do_run += "\"" ;
//       sd.Close () ;
       }
    else
       {
       wxMessageBox ( "Couldn't read setup file" ) ;
       return ;
       }
       
    if ( do_run.IsEmpty() ) return ;    
    
    LS->setOption ( "LAST_UPDATE" , ver ) ;
    wxExecute ( do_run , wxEXEC_ASYNC ) ;
    SetFocus () ;
    showSplashScreen = false ;
    dying = true ;
//    Close() ;
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

TStorage *MyFrame::getTempDB ( wxString filename )
    {
    int a ;
    for ( a = 0 ; a < dbcache.GetCount() && dbcache[a]->getDBname() != filename ; a++ ) ;
    if ( a == dbcache.GetCount() ) 
        dbcache.Add ( new TStorage ( TEMP_STORAGE , filename ) ) ;
    return dbcache[a] ;
    }
    
void MyFrame::setActiveChild ( ChildBase *c )
    {
    lastChild = c ;
    if ( !IsShown() ) return ;
    if ( locked != 0 ) return ;
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
    if ( !c->IsEnabled() ) c->Enable() ;
    if ( !c->IsShown() ) c->Show() ;
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
    if ( c->inMainTree.IsOk() && mainTree->GetSelection() != c->inMainTree )
        mainTree->SelectItem ( c->inMainTree ) ;
    c->Refresh () ;
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
        
