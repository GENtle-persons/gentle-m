#include "AminoAcids.h"

BEGIN_EVENT_TABLE(TAminoAcids, MyChildBase)
    EVT_MENU(MDI_EDIT_MODE, TAminoAcids::OnEditMode)
    EVT_MENU(MDI_MARK_ALL, TAminoAcids::OnMarkAll)
    EVT_MENU(MDI_FILE_SAVE, TAminoAcids::OnFileSave)
    EVT_MENU(MDI_EXPORT, ChildBase::OnExport)
    EVT_MENU(MDI_FIND, TAminoAcids::OnFind)
    EVT_MENU(MDI_CUT, TAminoAcids::OnCut)
    EVT_MENU(MDI_COPY, TAminoAcids::OnCopy)
    EVT_MENU(AMINOACIDS_EDIT_NAME, TAminoAcids::OnEditName)
    EVT_MENU(SEQ_PRINT, TAminoAcids::OnPrint)
    EVT_MENU(MDI_AS_NEW_FEATURE, TAminoAcids::OnAsNewFeature)
    EVT_MENU(AMINOACIDS_BLAST_AA, TAminoAcids::OnBlastAA)
    EVT_MENU(AMINOACIDS_PHOTOMETER, TAminoAcids::OnPhotometer)
    EVT_MENU(SEQ_AA_BACKTRANSLATE,TAminoAcids::OnBacktranslate)

    EVT_MENU(MDI_UNDO, TAminoAcids::Undo)
    EVT_MENU(MDI_REDO, TAminoAcids::Redo)

    EVT_CHOICE(AA_IP, TAminoAcids::OnIP)
    EVT_CHECKBOX(ALIGN_HORIZ, TAminoAcids::OnHorizontal)
    EVT_LISTBOX(AA_LB, TAminoAcids::OnListBox)
    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_CLOSE(ChildBase::OnClose)

    // Dummies
    EVT_MENU(MDI_TOGGLE_FEATURES,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_PASTE,ChildBase::OnDummy)
    EVT_MENU(AA_NONE,TABIviewer::OnDummy)
    EVT_MENU(AA_KNOWN, TABIviewer::OnDummy)
    EVT_MENU(AA_ALL, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE, TABIviewer::OnDummy)
    EVT_MENU(AA_ONE, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_3, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M3, TABIviewer::OnDummy)
    
END_EVENT_TABLE()

TAminoAcids::TAminoAcids(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    vec = new TVector ( this ) ;
    def = "AminoAcids" ;
    vec->setName ( title ) ;
    from = -1 ;
    stat = NULL ;
    pc = NULL ;
    sc2 = NULL ;
    lastLBsel = "" ;
    miniDisplayOptions = MINI_DISPLAY_CONDENSED ;
    }
    
TAminoAcids::~TAminoAcids ()
    {
    if ( curDisplay ) delete curDisplay ;
    if ( vec ) delete vec ;
    }

void TAminoAcids::OnBacktranslate ( wxCommandEvent& event )
    {
    sc->OnBacktranslate ( event ) ;
    }    

void TAminoAcids::initme ()
    {
    int bo = 5 ;
    vec->undo.clear() ;

    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu ( true , true ) ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu ( false ) ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;


    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(MDI_UNDO, txt("m_undo") );
//    edit_menu->Append(MDI_REDO, txt("m_redo") );
    edit_menu->AppendSeparator();
    edit_menu->Append(MDI_MARK_ALL, txt("m_mark_all") );
    edit_menu->Append(MDI_CUT, txt("m_cut") );
    edit_menu->Append(MDI_COPY, txt("m_copy") );
//    edit_menu->Append(MDI_PASTE, txt("m_paste") );
    edit_menu->AppendSeparator();
    edit_menu->Append(MDI_FIND, txt("m_find") );
//    edit_menu->AppendSeparator();
//    edit_menu->Append(MDI_COPY_TO_NEW, txt("m_copy_to_new") );
//    edit_menu->Append(MDI_AS_NEW_FEATURE, txt("m_as_new_feature") );
//    edit_menu->Append(MDI_EXTRACT_AA, txt("m_extract_aa") );
    edit_menu->Append(AMINOACIDS_BLAST_AA, txt("m_blast_aa") ) ;
    edit_menu->Append(AMINOACIDS_PHOTOMETER, txt("m_aa_photometer") ) ;
    edit_menu->Append(SEQ_AA_BACKTRANSLATE, txt("m_aa_backtranslate") ) ;

    wxMenu *view_menu = new wxMenu;
    view_menu->Append(MDI_EDIT_MODE, txt("m_edit_mode") , "" , true );


//    wxMenu *action_menu = new wxMenu;
//    action_menu->Append(AM_SETUP, "SETUP" );

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(edit_menu, txt("m_edit") );
    menu_bar->Append(view_menu, txt("m_view") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );
    SetMenuBar(menu_bar);

    // Sequence Canvas
    sc = new SequenceCanvas ( this , wxPoint ( 0 , 0 ) , wxSize ( 100 , 100 ) ) ;
    cSequence = sc ; // For compatability with PlasmidCanvas
    sc->blankline = 1 ;
    sc->child = this ;
    sc->edit_id = "AA" ;
    sc->edit_valid = "ACDEFGHIKLMNPQRSTVWY|" ;

    int w , h ;
//    GetParent()->GetClientSize ( &w , &h ) ;
    myapp()->frame->GetClientSize ( &w , &h ) ;
    h = 100 ;
    
    Hide();
    
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    
    h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    v1 = new wxBoxSizer ( wxVERTICAL ) ;
    
    lb = new wxListBox ( this ,
                            AA_LB ,
                            wxDefaultPosition ,
                            wxSize ( 140 , 160 ) ) ;
                            
    lb->Append ( txt("t_data") ) ;
    lb->Append ( txt("desc") ) ;
    lb->Append ( txt("t_schema") ) ;
    lb->Append ( txt("t_aa_plot_mw") ) ;
    lb->Append ( txt("t_aa_plot_pi") ) ;
    lb->Append ( txt("t_aa_plot_hp") ) ;
    lb->Append ( txt("t_chou_fasman") ) ;

#ifdef __WXMSW__
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    myapp()->frame->InitToolBar(toolBar);
    toolBar->AddTool( MDI_TEXT_IMPORT , 
                myapp()->frame->bitmaps[0],
                txt("m_new_sequence") ) ;
    toolBar->AddTool( MDI_FILE_OPEN, 
                myapp()->frame->bitmaps[1],
            txt("m_open") , txt("m_opentxt") );
    toolBar->AddTool( MDI_FILE_SAVE, 
                myapp()->frame->bitmaps[2],
                txt("m_store_in_db") , 
                txt("m_txt_store_in_db"));
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_CUT, myapp()->frame->bitmaps[4] ) ;
    toolBar->AddTool( MDI_COPY, myapp()->frame->bitmaps[5] ) ;
    toolBar->AddTool( MDI_PASTE, myapp()->frame->bitmaps[6] ) ;
    toolBar->AddSeparator () ;
    wxCheckBox *mycb = new wxCheckBox ( toolBar , ALIGN_HORIZ , txt("t_horizontal") ) ;
    toolBar->AddControl ( mycb ) ;
    inlinePlot = new wxChoice ( toolBar , AA_IP ) ;
    toolBar->AddControl ( inlinePlot ) ;
    toolBar->Realize() ;
    inlinePlot->Append ( txt("t_no_plot") ) ;
    inlinePlot->Append ( txt("t_chou_fasman") ) ;
    inlinePlot->SetStringSelection ( txt("t_no_plot") ) ;
#endif


    curDisplay = new wxPanel ( this , -1 ) ;


    h1->Add ( lb   , 0 , wxEXPAND , 5 ) ;
    h1->Add ( curDisplay , 1 , wxEXPAND , 5 ) ;
    v1->Add ( h1 , 0 , wxEXPAND , 5 ) ;
    v1->Add ( sc , 1 , wxEXPAND , 5 ) ;
    
    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( v1 , 1 , wxEXPAND , 5 ) ;

    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;
    lb->SetStringSelection ( txt("t_data") ) ;
    
    wxCommandEvent event ;
    OnListBox ( event ) ;

    showSequence () ;
    showStat () ;
    updateUndoMenu () ;
    if ( myapp()->frame->isLocked() ) return ;
    Activate () ;
    sc->SetFocus() ;
    }
    
wxString TAminoAcids::getName ()
    {
    return vec->getName() ;
    }

void TAminoAcids::showStat ()
    {
    if ( !curDisplay ) return ;
    if ( curDisplay == desc ) desc->SetValue ( vec->getDescription() ) ;
    if ( curDisplay != stat ) return ;
    char t[10000] ;
    int noaa = 0 , piaa = 0 ;
    float pI = 0 ;
    float ex = 0 , abs = 0 ;
    int num[256] ;
    int a ;
    float mW = 0 ;
    for ( a = 0 ; a < 256 ; a++ ) num[a] = 0 ;
    for ( a = 0 ; a < vec->getSequenceLength() ; a++ )
        {
        unsigned char c = (unsigned char)vec->getSequenceChar ( a ) ;
        if ( c != '|' && c != ' ' ) noaa++ ;
        float ppi = vec->getAApi ( c ) ;
        if ( ppi != 0 )
           {
           pI += ppi ;
           piaa ++ ;
           }

        num[c]++ ;
        mW += vec->getAAmw ( c ) ;
        }
    if ( piaa > 0 ) pI /= piaa ;
    ex = num['W']*5500 + num['Y']*1490 + num['C']*125 ;
    if ( noaa > 0 ) abs = ex / noaa / 100 ;
    sprintf ( t , txt("aa_info") , noaa , mW , pI , ex , abs ) ;
    stat->SetValue ( t ) ;
    }

void TAminoAcids::showSequence ()
    {
    // Cleaning up
    CLEAR_DELETE ( sc->seq ) ;

    // Display
    SeqFeature *seqF = new SeqFeature ( sc ) ;
    sc->seq.Add ( seqF ) ;
    seqF->initFromTVector ( vec ) ;

    SeqAA *d = new SeqAA ( sc ) ;
    sc->seq.Add ( d ) ;
    d->primaryMode = true ;
    d->takesMouseActions = true ;
    d->initFromString ( vec->getSequence() ) ;
//    d->fixOffsets ( vec ) ;

    // Plot demo
    if ( inlinePlot->GetStringSelection() == txt("t_chou_fasman") )
        {
        SeqPlot *seqP = new SeqPlot ( sc ) ;
        sc->seq.Add ( seqP ) ;
        seqP->initFromTVector ( vec ) ;
        seqP->setLines ( 1 ) ;
        seqP->useChouFasman() ;
        seqP->takesMouseActions = false ;
        }
    
    seqF->aaa = d ;
        
    sc->arrange () ;
    sc->SilentRefresh () ;    

    wxCommandEvent event ;
    OnListBox ( event ) ;
    }
    
void TAminoAcids::OnEditMode(wxCommandEvent& event)
    {
    wxMenuBar *mb = GetMenuBar () ;
    wxMenuItem *mi = mb->FindItem ( MDI_EDIT_MODE ) ;
    if ( mi->IsChecked() ) sc->startEdit ( "AA" ) ;
    else sc->stopEdit() ;
    if ( sc->getEditMode() ) vec->undo.start ( txt("u_edit") ) ;
    else vec->undo.stop () ;
    }

void TAminoAcids::invokeVectorEditor ( wxString what , int num , bool forceUpdate )
    {
    vec->undo.start ( txt("u_vec_edit") ) ;
    TVectorEditor ve ( this , txt("t_vector_editor") , vec ) ;
    bool changed = vec->isChanged() ;
    vec->setChanged ( false ) ;

    ve.hideEnzym = true ;
    ve.hideEm () ;
    
    if ( what == "item" )
        ve.initialViewItem ( num ) ;

    int x = ve.ShowModal () ;
    ve.cleanup () ;
    if ( forceUpdate || vec->isChanged() )
        {
        myapp()->frame->mainTree->SetItemText ( inMainTree , getName() ) ;
        showName() ;
        showSequence () ;
        vec->undo.stop() ;
        }
    else vec->undo.abort() ;
    vec->setChanged ( changed | vec->isChanged() ) ;
    }
    
void TAminoAcids::OnEditName(wxCommandEvent& event)
    {
    invokeVectorEditor () ;
    }
    
void TAminoAcids::OnMarkAll(wxCommandEvent& event)
    {
    sc->mark ( "AA" , 1 , vec->getSequenceLength() ) ;
    }
    
void TAminoAcids::OnPrint(wxCommandEvent& event)
    {
    sc->OnPrint ( event ) ;
    }
    
void TAminoAcids::OnFileSave(wxCommandEvent& event)
    {
    TManageDatabaseDialog dbd ( this , txt("t_store") , ACTION_MODE_SAVE , vec ) ;
    dbd.ShowModal () ;
    }
    
void TAminoAcids::OnFind(wxCommandEvent& event)
    {
    FindSequenceDialog fsd ( this , txt("t_find") ) ;
    fsd.allowed_chars = sc->edit_valid ;
    fsd.ShowModal () ;
    }
    
void TAminoAcids::OnCut(wxCommandEvent& event)
    {
    vec->undo.start ( txt("u_cut") ) ;
    sc->OnCut ( event ) ;
    vec->undo.stop () ;
    }
    
void TAminoAcids::OnCopy(wxCommandEvent& event)
    {
    sc->OnCopy ( event ) ;
    }

void TAminoAcids::OnAsNewFeature(wxCommandEvent& event)
    {
    char t[1000] ;
    int from = sc->markedFrom() ;
    int to = sc->markedTo() ;
    if ( from == -1 ) return ;
    
    vec->undo.start ( txt("u_new_feature") ) ;

    TVectorItem nvi ;
    sprintf ( t , txt("t_new_item_title") , from , to ) ;
    nvi.name = t ;
    nvi.setDirection ( 1 ) ;
    nvi.setType ( VIT_MISC ) ;
    nvi.from = from ;
    nvi.to = to ;
    vec->setChanged () ;
    vec->items.push_back ( nvi ) ;
    invokeVectorEditor ( "item" , vec->items.size()-1 , true ) ;
    vec->undo.stop() ;
    }
    
void TAminoAcids::OnPhotometer(wxCommandEvent& event)
    {
    TCalculator *c = myapp()->frame->RunCalculator () ;
    wxString seq = vec->getSequence() ;
    float mW = 0 ;
    for ( int a = 0 ; a < vec->getSequenceLength() ; a++ )
        mW += vec->getAAmw ( vec->getSequenceChar ( a ) ) ;
    c->prot->SetCellValue ( 2 , 1 , wxString::Format("%d", seq.Replace("W","",true)) ) ;
    c->prot->SetCellValue ( 3 , 1 , wxString::Format("%d", seq.Replace("Y","",true)) ) ;
    c->prot->SetCellValue ( 4 , 1 , wxString::Format("%d", seq.Replace("C","",true)) ) ;
    c->prot->SetCellValue ( 5 , 1 , wxString::Format("%0.2f", mW) ) ;
    c->nb->SetSelection ( 2 ) ;
    }
    
void TAminoAcids::OnBlastAA(wxCommandEvent& event)
    {
    wxString seq ;
    int a ;
    if ( sc->markedFrom() < 0 ) seq = vec->getSequence() ;
    else seq = sc->getSelection() ;
    myapp()->frame->blast ( seq , "blastp" ) ;
    }

void TAminoAcids::Undo(wxCommandEvent& event)
    {
    if ( !vec->undo.canUndo() ) return ;
    vec->undo.pop() ;
//    initPanels () ;

    showSequence () ;
    showStat () ;
    updateUndoMenu () ;
    sc->SetFocus() ;
    }
    
void TAminoAcids::updateUndoMenu ()
    {
    wxString lm = vec->undo.getLastMessage() ;
    wxMenuBar *mb = GetMenuBar () ;
    if ( !mb ) return ;
    wxMenuItem *mi = mb->FindItem ( MDI_UNDO ) ;
    if ( !mi ) return ;
    bool canUndo ;
    if ( lm.IsEmpty() )
        {
        mi->SetText ( txt("u_no") ) ;
        mi->Enable ( false ) ;
	canUndo = false ;
        }
    else
        {
        mi->Enable ( true ) ;
        mi->SetText ( lm ) ;
	canUndo = true ;
        }
#ifdef __WXMSW__
    GetToolBar()->EnableTool ( MDI_UNDO , canUndo ) ;
#else
    // myapp()->frame->GetToolBar()->EnableTool ( MDI_UNDO , canUndo ) ;
#endif
    }
    
void TAminoAcids::Redo(wxCommandEvent& event)
    {
    
    }
    
void TAminoAcids::OnHorizontal ( wxCommandEvent& event )
    {
    sc->toggleHorizontal () ;
    sc->arrange () ;
    sc->SilentRefresh() ;    
    }
    
void TAminoAcids::OnIP ( wxCommandEvent& event )
    {
    showSequence () ;
    }
    
void TAminoAcids::OnListBox ( wxCommandEvent& event )
    {
    bool update = false ;
    wxString t = lb->GetStringSelection() ;
    if ( t == lastLBsel ) update = true ;
    else lastLBsel = t ;
    
    if ( !update )
        {
        if ( curDisplay )
            {
            h1->Remove ( curDisplay ) ;
            delete curDisplay ;
            }
        curDisplay = NULL ;
        desc = NULL ;
        stat = NULL ;
        sc2 = NULL ;
        }
    if ( t == txt("desc") )
        {
        if ( update ) desc->SetValue ( vec->getDescription() )  ;
        else
           {
           desc = new TURLtext ( this ,
                            URLTEXT_DUMMY ,
                            "" ,
                            wxDefaultPosition,
                            wxSize ( 250 , 90 ) ,
                            wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY ) ;
           h1->Add ( desc , 1 , wxEXPAND , 5 ) ;
           curDisplay = desc ;
           desc->SetValue ( vec->getDescription() ) ;
           }
        }
    else if ( t == txt("t_data") )
        {
        if ( !update ) 
           {
           stat = new wxTextCtrl ( this ,
                            -1 ,
                            "" ,
                            wxDefaultPosition,
                            wxSize ( 200 , 90 ) ,
                            wxTE_MULTILINE | wxTE_READONLY ) ;
           curDisplay = stat ;
           h1->Add ( stat , 1 , wxEXPAND , 5 ) ;
           }
        showStat () ;
        }
    else if ( t == txt("t_schema") )
        {
        vec->recalcvisual = true ;
        if ( update ) pc->Refresh() ;
        else
           {
           pc = new PlasmidCanvas ( this , wxDefaultPosition , wxSize ( 250 , 90 ) ) ;
           pc->setRootChild ( (MyChild*) this ) ;
           h1->Add ( pc , 1 , wxEXPAND , 5 ) ;
           curDisplay = pc ;
           }
        }
    else if ( t == txt("t_chou_fasman") ||
              t == txt("t_aa_plot_mw" ) ||
              t == txt("t_aa_plot_pi" ) ||
              t == txt("t_aa_plot_hp" ) )
        {
        if ( !update )
           {
           sc2 = new SequenceCanvas(this, wxDefaultPosition, wxSize(250, 90));
           sc2->blankline = 1 ;
           sc2->child = this ;
//           sc2->edit_id = "AA" ;
//           sc2->edit_valid = "ACDEFGHIKLMNPQRSTVWY|" ;
           h1->Add ( sc2 , 1 , wxEXPAND , 5 ) ;
           sc2->setMiniDisplay ( true );
           sc2->setHorizontal ( true ) ;
           curDisplay = sc2 ;        
           }
        else
           {
           CLEAR_DELETE ( sc2->seq ) ;
           }

        SeqPlot *seqP = new SeqPlot ( sc2 ) ;
        sc2->seq.Add ( seqP ) ;
        seqP->initFromTVector ( vec ) ;
        seqP->setLines ( 6 ) ;
        
        if ( t == txt("t_chou_fasman") ) seqP->useChouFasman() ;
        else if ( t == txt("t_aa_plot_mw") ) seqP->useMW () ;
        else if ( t == txt("t_aa_plot_pi") ) seqP->usePI () ;
        else if ( t == txt("t_aa_plot_hp") ) seqP->useHP () ;
        
        if ( update )
           {
           sc2->arrange() ;
           sc2->SilentRefresh() ;
           }
        }
    if ( !update )
        {
        h1->Layout() ;
        GetToolBar()->Refresh () ;
        }
    sc->SetFocus() ;
    }
    
void TAminoAcids::updateToolbar ()
    {
    }    
    
