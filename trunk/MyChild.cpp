#include "MyChild.h"
#include <wx/datetime.h>
#include <wx/printdlg.h>

BEGIN_EVENT_TABLE(MyChild, MyChildBase)
    EVT_MENU(AA_NONE, MyChild::OnAA_none)
    EVT_MENU(AA_KNOWN, MyChild::OnAA_known)
    EVT_MENU(AA_ALL, MyChild::OnAA_all)
    EVT_MENU(AA_THREE, MyChild::OnAA_three)
    EVT_MENU(AA_ONE, MyChild::OnAA_one)
    EVT_MENU(AA_THREE_1, MyChild::OnAA_three_1)
    EVT_MENU(AA_THREE_2, MyChild::OnAA_three_2)
    EVT_MENU(AA_THREE_3, MyChild::OnAA_three_3)
    EVT_MENU(AA_THREE_M1, MyChild::OnAA_three_M1)
    EVT_MENU(AA_THREE_M2, MyChild::OnAA_three_M2)
    EVT_MENU(AA_THREE_M3, MyChild::OnAA_three_M3)

    EVT_MENU(MDI_UNDO, MyChild::Undo)
//    EVT_MENU(MDI_REDO, MyChild::Redo)

    EVT_MENU(MDI_CHILD_QUIT, MyChild::OnQuit)
    EVT_MENU(MDI_MARK_ALL, MyChild::OnMarkAll)
    EVT_MENU(MDI_EXTRACT_AA, MyChild::OnExtractAA)
    EVT_MENU(MDI_CIRCULAR_LINEAR, MyChild::OnCircularLinear)
    EVT_MENU(MDI_LIGATION, MyChild::OnLigation)
    EVT_MENU(MDI_FILE_SAVE, MyChild::OnFileSave)
    EVT_MENU(MDI_CUT, MyChild::OnCut)
    EVT_MENU(MDI_COPY, MyChild::OnCopy)
    EVT_MENU(MDI_PASTE, MyChild::OnPaste)
    EVT_MENU(MDI_EXPORT, ChildBase::OnExport)
    EVT_MENU(MDI_FIND, MyChild::OnFind)
    EVT_MENU(MDI_COPY_TO_NEW, MyChild::OnCopyToNew)
    EVT_MENU(MDI_EDIT_MODE, MyChild::OnEditMode)
    EVT_MENU(MDI_VIEW_MODE, MyChild::OnViewMode)
    EVT_MENU(MDI_AS_NEW_FEATURE, MyChild::OnAsNewFeature)
    EVT_MENU(MDI_TRANSFORM_SEQUENCE, MyChild::OnTransformSequence)
    EVT_MENU(MDI_PRINT_IMAGE, MyChild::OnPrintImage)
    EVT_MENU(MDI_RESTRICTION,MyChild::OnRestriction)
    EVT_MENU(MDI_HELP, MyChild::OnHelp)
    EVT_MENU(MDI_ORFS, MyChild::OnORFs)
    EVT_MENU(MDI_EDIT_ORFS, MyChild::OnEditORFs)
    EVT_MENU(SEQ_PRINT, MyChild::OnSeqPrint)
    EVT_MENU(MDI_PRINT_REPORT,MyChild::OnPrintReport)
    EVT_MENU(MDI_TOGGLE_FEATURES,MyChild::OnToggleFeatures)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,MyChild::OnToggleRestriction)
    EVT_MENU(MDI_TOGGLE_IDNA,MyChild::OnToggleIDNA)
    EVT_MENU(MDI_RUN_PCR,MyChild::OnRunPCR)

    EVT_CHOICE(PC_ZOOM,MyChild::OnZoom)
    EVT_UPDATE_UI(MDI_REFRESH, MyChild::OnUpdateRefresh)
//    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_CLOSE(ChildBase::OnClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MySplitter, wxSplitterWindow)
    EVT_SPLITTER_SASH_POS_CHANGED(SPLIT_1,MySplitter::OnChanged)
    EVT_SPLITTER_SASH_POS_CHANGED(SPLIT_2,MySplitter::OnChanged)
    EVT_SPLITTER_SASH_POS_CHANGED(SPLIT_3,MySplitter::OnChanged)
END_EVENT_TABLE()


// ---------------------------------------------------------------------------
// MyChild
// ---------------------------------------------------------------------------

void MyChild::OnFileSave(wxCommandEvent& WXUNUSED(event) )
    {
    TManageDatabaseDialog dbd ( this , txt("t_store") , ACTION_MODE_SAVE , vec ) ;
    dbd.ShowModal () ;
    }

MyChild::MyChild(wxWindow *parent, const wxString& title,
                 const wxPoint& pos, const wxSize& size,
                 const long style)
       : ChildBase(parent, title)//, pos, size, style)
{
    cPlasmid = (PlasmidCanvas *) NULL;
    cSequence = (SequenceCanvas *) NULL;
    vec = (TVector *) NULL ;
    def = "dna" ;
    aa_state = AA_ALL ;
    aa_disp = AA_ONE ;
    showORFs = false ;
    orf_mode = 63 ;
    viewMode = false ;
    aa_offset = 1 ;
}

MyChild::~MyChild()
{
    delete swl ;
    delete swu ;
    delete sw ;
}

void MyChild::OnRunPCR(wxCommandEvent& event)
    {
    if ( !cPlasmid ) return ;
    cPlasmid->OnRunPCR ( event ) ;
    }

void MyChild::OnToggleRestriction(wxCommandEvent& event)
    {
    TMarkMem mm ( cSequence ) ;
    if ( cSequence->findID ( "RESTRICTION" ) )
       { // Turn off
       delete cSequence->seq[cSequence->seq.GetCount()-1] ;
       cSequence->seq.RemoveAt(cSequence->seq.GetCount()-1) ;
       }
    else
       {
       SeqRestriction *seqR = new SeqRestriction ( cSequence ) ;
       cSequence->seq.Add ( seqR ) ;
       seqR->initFromTVector ( vec ) ;    
       }
    updateSequenceCanvas () ;
#ifdef __WXMSW__ // LINUX
    GetToolBar()->ToggleTool(MDI_TOGGLE_RESTRICTION,cSequence->findID("RESTRICTION"));
#endif
    mm.remark () ;
    }
    
void MyChild::OnToggleIDNA(wxCommandEvent& event)
    {
    TMarkMem mm ( cSequence ) ;
    SeqBasic *idna = cSequence->findID ( "IDNA" ) ;
    if ( idna )
        {
        int a ;
        for ( a = 0 ; cSequence->seq[a] != idna ; a++ ) ;
        for ( a++ ; a < cSequence->seq.GetCount() ; a++ )
           cSequence->seq[a-1] = cSequence->seq[a] ;
        cSequence->seq.RemoveAt ( cSequence->seq.GetCount()-1 ) ;
        delete idna ;
        }
    else
        {
        SeqDNA *seqi = new SeqDNA ( cSequence ) ;
        cSequence->seq.Add ( seqi ) ;
        int a ;
        for ( a = cSequence->seq.GetCount()-1 ; cSequence->seq[a-1]->whatsthis() != "DNA" ; a-- )
           {
           idna = cSequence->seq[a-1] ;
           cSequence->seq[a-1] = cSequence->seq[a] ;
           cSequence->seq[a] = idna ;
           }
        seqi->invers = true ;
        seqi->initFromTVector ( vec ) ;
        }
    updateSequenceCanvas () ;
    mm.remark () ;
    }

void MyChild::OnToggleFeatures(wxCommandEvent& event)
    {
    int a ;
    cSequence->lastmarked += cSequence->findID ( "FEATURE" ) ? -1 : 1 ;
    TMarkMem mm ( cSequence ) ;
    if ( cSequence->findID ( "FEATURE" ) )
       { // Turn off
       aa_offset = 0 ;
       delete cSequence->seq[0] ;
       for ( a = 1 ; a < cSequence->seq.GetCount() ; a++ )
          cSequence->seq[a-1] = cSequence->seq[a] ;
       cSequence->seq.RemoveAt ( cSequence->seq.GetCount()-1 ) ;
       }
    else
       { // Turn on
       aa_offset = 1 ;
       cSequence->seq.Add ( 0 ) ;
       for ( a = cSequence->seq.GetCount()-1 ; a > 0 ; a-- )
          cSequence->seq[a] = cSequence->seq[a-1] ;
       SeqFeature *seqF = new SeqFeature ( cSequence ) ;
       cSequence->seq[0] = seqF ;
       seqF->initFromTVector ( vec ) ;
       }
    updateSequenceCanvas () ;
#ifdef __WXMSW__ // LINUX
    GetToolBar()->ToggleTool(MDI_TOGGLE_FEATURES,cSequence->findID("FEATURE"));
#endif
    mm.remark () ;
    }

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyChild::OnUpdateRefresh(wxUpdateUIEvent& event)
{
    event.Enable( cPlasmid );
}


void MyChild::OnActivate(wxActivateEvent& event)
{
    ChildBase::Activate () ;
    if ( event.GetActive() && cSequence ) 
       cSequence->SetFocus();
}


void MyChild::OnCircularLinear(wxCommandEvent& event)
    {
    if ( vec->hasStickyEnds() )
        {
#ifdef __WXMSW__ // LINUX
        GetToolBar()->ToggleTool(MDI_CIRCULAR_LINEAR,vec->isCircular());
#endif
        return ;
        }
    for ( int a = 0 ; a < vec->items.size() ; a++ ) vec->items[a].r1 = -1 ;
    vec->setCircular ( vec->isLinear() ) ;
    vec->recalcvisual = true ;
    vec->recalculateCuts() ;
    treeBox->Refresh() ;
    cPlasmid->Refresh() ;
    cSequence->Refresh() ;
#ifdef __WXMSW__ // LINUX
    GetToolBar()->ToggleTool(MDI_CIRCULAR_LINEAR,vec->isCircular());
#endif
    }

void MyChild::initme ()
    {
    // Make a menubar
    wxMenu *file_menu = myapp()->frame->getFileMenu ( true , true , true ) ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu ( true ) ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(MDI_UNDO, txt("m_undo") );
//    edit_menu->Append(MDI_REDO, txt("m_redo") );
    edit_menu->AppendSeparator();
    edit_menu->Append(MDI_MARK_ALL, txt("m_mark_all") );
    edit_menu->Append(MDI_CUT, txt("m_cut") );
    edit_menu->Append(MDI_COPY, txt("m_copy") );
    edit_menu->Append(MDI_PASTE, txt("m_paste") );
    edit_menu->AppendSeparator();
    edit_menu->Append(MDI_FIND, txt("m_find") );
    edit_menu->AppendSeparator();
    edit_menu->Append(MDI_COPY_TO_NEW, txt("m_copy_to_new") );
    edit_menu->Append(MDI_AS_NEW_FEATURE, txt("m_as_new_feature") );
    edit_menu->Append(MDI_EXTRACT_AA, txt("m_extract_aa") );
    edit_menu->Append(MDI_TRANSFORM_SEQUENCE, txt("t_transform_sequence") );
    edit_menu->Append(MDI_ORFS, txt("m_orfs"), txt("m_orfs_txt") );
    edit_menu->Append(MDI_EDIT_ORFS, txt("m_edit_orfs") );

    wxMenu *view_menu = new wxMenu;
//    view_menu->Append(MDI_REFRESH, txt("m_refresh_picture") );
    view_menu->Append(MDI_CIRCULAR_LINEAR, txt("m_toggle_rc") );
    view_menu->Append(MDI_TOGGLE_FEATURES, txt("m_display_features") , "" );
    view_menu->Append(MDI_TOGGLE_RESTRICTION, txt("m_display_restriction") , "" );
    view_menu->Append(MDI_TOGGLE_IDNA, txt("m_display_idna") , "" );
    view_menu->AppendSeparator();
    view_menu->Append(MDI_EDIT_MODE, txt("m_edit_mode") , "" , true );
    view_menu->Append(MDI_VIEW_MODE, txt("m_view_mode") , "" , true );
    
    wxMenu *mAA = new wxMenu ;
    view_menu->Append ( AA_MAIN , txt("m_aa_main") , mAA ) ;
    mAA->Append (AA_NONE,txt("m_aa_none") , "" , true );
    mAA->Append (AA_KNOWN,txt("m_aa_known"), "" , true );
    mAA->Append (AA_ALL,txt("m_aa_all"), "" , true );
    mAA->Append (AA_THREE_1,txt("m_aa_1"), "" , true );
    mAA->Append (AA_THREE_2,txt("m_aa_2"), "" , true );
    mAA->Append (AA_THREE_3,txt("m_aa_3"), "" , true );
    mAA->Append (AA_THREE_M1,txt("m_aa_m1"), "" , true );
    mAA->Append (AA_THREE_M2,txt("m_aa_m2"), "" , true );
    mAA->Append (AA_THREE_M3,txt("m_aa_m3"), "" , true );
    mAA->AppendSeparator();
    mAA->Append (AA_ONE,txt("m_aa_one"), "" , true );
    mAA->Append (AA_THREE,txt("m_aa_three"), "" , true );

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(edit_menu, txt("m_edit") );
    menu_bar->Append(view_menu, txt("m_view") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    myass ( menu_bar->FindItem(aa_state) , "MyChild::initme_1" ) ;
    menu_bar->FindItem(aa_state)->Check ( true ) ;
    myass ( menu_bar->FindItem(aa_disp) , "MyChild::initme_1" ) ;
    menu_bar->FindItem(aa_disp)->Check ( true ) ;

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);
    
    // Vector
    vec = new TVector ;
    vec->init () ;

    // Canvas
    int width, height;
    GetParent()->GetClientSize(&width, &height);

    sw = new MySplitter ( this , SPLIT_1 , this ) ;
    swu = new MySplitter ( sw , SPLIT_2 , this ) ;
    swl = new MySplitter ( swu , SPLIT_3 , this ) ;

    cPlasmid = new PlasmidCanvas(swu, wxPoint(0, 0), wxSize(width*2/3, height/2));   
    cPlasmid->setRootChild ( this ) ;
    
    cSequence = new SequenceCanvas(sw, wxPoint(0, 0), wxSize(width, height/2));
    cSequence->p = this ;
    cSequence->child = this ;

    propBox = new TURLtext(swl, URLTEXT_DUMMY, "",
                                wxDefaultPosition, wxSize ( width/3 , 0 ) ,
                                wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY );
                               
    treeBox = new TVectorTree((ChildBase*)swl, TREE_DUMMY ) ;
    treeBox->textWindow = propBox ;
    treeBox->p = this ;

    sw->SplitHorizontally ( swu , cSequence , height/2 ) ;
    swu->SplitVertically ( swl , cPlasmid , width/4 ) ;
    swl->SplitHorizontally ( treeBox , propBox , height/3 ) ;
    
    // Give it scrollbars
    cSequence->SetScrollbars(0, 20, 0, 50);

    // TOOLBAR 

    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);    
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
    toolBar->AddTool( MDI_UNDO, myapp()->frame->bitmaps[3] ) ;
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_CUT, myapp()->frame->bitmaps[4] ) ;
    toolBar->AddTool( MDI_COPY, myapp()->frame->bitmaps[5] ) ;
    toolBar->AddTool( MDI_PASTE, myapp()->frame->bitmaps[6] ) ;
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_CIRCULAR_LINEAR,
        myapp()->frame->bitmaps[7],
        myapp()->frame->bitmaps[8],
        TRUE, -1, -1, (wxObject *) NULL, txt("m_toggle_rc") ) ;
    toolBar->AddTool( MDI_ORFS,
        myapp()->frame->bitmaps[9],
        myapp()->frame->bitmaps[9],
        TRUE, -1, -1, (wxObject *) NULL, txt("m_orfs") ) ;
    toolBar->AddTool( MDI_TOGGLE_FEATURES,
        myapp()->frame->bitmaps[10],
        myapp()->frame->bitmaps[10],
        TRUE, -1, -1, (wxObject *) NULL, txt("m_display_features") ) ;
    toolBar->AddTool( MDI_TOGGLE_RESTRICTION,
        myapp()->frame->bitmaps[11],
        myapp()->frame->bitmaps[11],
        TRUE, -1, -1, (wxObject *) NULL, txt("m_display_restriction") ) ;        
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_VIEW_MODE,
        myapp()->frame->bitmaps[12],
        myapp()->frame->bitmaps[12],
        TRUE, -1, -1, (wxObject *) NULL, txt("m_view_mode") ) ;
    toolBar->AddTool( MDI_EDIT_MODE,
        myapp()->frame->bitmaps[13],
        myapp()->frame->bitmaps[13],
        TRUE, -1, -1, (wxObject *) NULL, txt("m_edit_mode") ) ;
    toolBar->AddSeparator() ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_zoom") ) ) ;
    zoom_cb = new wxChoice ( toolBar , PC_ZOOM , wxDefaultPosition , wxSize ( 60 , -1 ) ) ;
    zoom_cb->Append ( "100%" ) ;
    zoom_cb->Append ( "200%" ) ;
    zoom_cb->Append ( "300%" ) ;
    zoom_cb->Append ( "400%" ) ;
    zoom_cb->Append ( "800%" ) ;
    zoom_cb->Append ( "1600%" ) ;
    zoom_cb->SetSelection ( 0 ) ;
    toolBar->AddControl ( zoom_cb ) ;
    toolBar->Realize() ;    

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( sw , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

    updateUndoMenu () ;
    }

void MyChild::OnLigation(wxCommandEvent& event)
    {
    TLigationDialog ld ( this , txt("t_ligation") ) ;
    long l ;
    for ( l = 0 ; l < myapp()->frame->children.GetCount() ; l++ )
        {
        MyChild *p = (MyChild*) myapp()->frame->children[l] ;
        if ( p->def == "dna" && !p->vec->isCircular() )
           {
           ld.vv.Add ( p->vec ) ;
           }
        }
    
    ld.init () ;
    ld.ShowModal() ;
    if ( !ld.doLigate ) return ;
    for ( l = 0 ; l < ld.ligates.size() ; l++ )
        {
        if ( !ld.ligates[l].getSequence().IsEmpty() )
           {
           TVector *v = new TVector ;
           v->setFromVector ( ld.ligates[l] ) ;
           for ( int a = 0 ; a < v->items.size() ; a++ )
              v->items[a].r1 = -1 ; // Resetting item radius to "recalc"
           myapp()->frame->newFromVector ( v ) ;
           }
        }
    myapp()->frame->lastCocktail.Clear () ;
    }
    
wxString MyChild::getName ()
    {
    return vec->getName() ;
    }
    
void MyChild::OnCut(wxCommandEvent& event)
    {
    wxString s = cPlasmid->getSelection () ;
    if ( s.IsEmpty() ) return ;
    if (wxTheClipboard->Open())
        {
        vec->undo.start ( txt("u_cut") ) ;
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();

        myass ( false , "!1" ) ;
        vec->doRemove ( cPlasmid->getMarkFrom() , cPlasmid->getMarkTo() ) ;
        myass ( false , "!2" ) ;
        cPlasmid->setMarkFrom ( -1 ) ;
        
        myass ( false , "!3" ) ;
        for ( int a = 0 ; a < cSequence->seq.GetCount() ; a++ )
           cSequence->seq[a]->initFromTVector ( vec ) ;

        myass ( false , "!4" ) ;
        cSequence->arrange() ;
        treeBox->initme() ;
        treeBox->Refresh() ;
        cPlasmid->Refresh() ;
        cSequence->Refresh() ;
        vec->undo.stop() ;
        myass ( false , "!!" ) ;
        }
    }
    
void MyChild::OnCopy(wxCommandEvent& event)
    {
    wxString s = cPlasmid->getSelection () ;
    if ( s.IsEmpty() ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }
    }
    
void MyChild::OnPaste(wxCommandEvent& event)
    {
    cSequence->OnPaste ( event ) ;
    }

void MyChild::OnCopyToNew(wxCommandEvent& event)
    {
    int from , to ;
    cPlasmid->getMark ( from , to ) ;
    if ( from == -1 ) return ;
    myapp()->frame->newFromVector ( vec->newFromMark ( from , to ) ) ;
    }
    
void MyChild::OnAsNewFeature(wxCommandEvent& event)
    {
    char t[1000] ;
    int from , to ;
    cPlasmid->getMark ( from , to ) ;
    if ( from == -1 ) return ;
    
    vec->undo.start ( txt("u_new_feature") ) ;

    if ( to > vec->getSequenceLength() )
        to -= vec->getSequenceLength() ;

    TVectorItem nvi ;
    sprintf ( t , txt("t_new_item_title") , from , to ) ;
    nvi.name = t ;
    nvi.direction = 1 ;
    nvi.type = VIT_GENE ;
    nvi.from = from ;
    nvi.to = to ;
    vec->setChanged () ;
    vec->items.push_back ( nvi ) ;
    cPlasmid->invokeVectorEditor ( "item" , vec->items.size()-1 , true ) ;
    vec->recalcvisual = true ;
    cPlasmid->Refresh () ;
    updateSequenceCanvas () ;
    vec->undo.stop () ;
    }

void MyChild::OnViewMode(wxCommandEvent& event)
    {
    wxMenuBar *mb = GetMenuBar () ;
    myass ( mb , "MyChild::OnViewMode_1" ) ;
    wxMenuItem *mi = mb->FindItem ( MDI_VIEW_MODE ) ;
    myass ( mb , "MyChild::OnViewMode_2" ) ;
    wxString s ;
    if ( cSequence->getEditMode() ) OnEditMode ( event ) ;
    if ( !viewMode )
        {
        sp1 = sw->GetSashPosition () ;
        sp2 = swu->GetSashPosition () ;
        sw->Unsplit ( cSequence ) ;
        swu->Unsplit ( swl ) ;
        viewMode = true ;
        cPlasmid->SetFocus() ;
        OnZoom ( event ) ;
        }
    else
        {
        cPlasmid->setMarkFrom ( -1 ) ;
        cSequence->setEditMode ( false ) ;
        cSequence->arrange () ;
        vec->recalcvisual = true ;
        sw->SplitHorizontally ( swu , cSequence ) ;
        swu->SplitVertically ( swl , cPlasmid ) ;
        sw->SetSashPosition ( sp1 , true ) ;
        swu->SetSashPosition ( sp2 , true ) ;
        swl->Show ( TRUE ) ;
        cSequence->Show ( TRUE ) ;
        viewMode = false ;
        OnZoom ( event ) ;
        Refresh () ;
        cSequence->SetFocus() ;
        }
    mi->Check ( viewMode ) ;
#ifdef __WXMSW__ // LINUX
    myass ( GetToolBar() , "MyChild::OnViewMode_3" ) ;
    GetToolBar()->ToggleTool(MDI_VIEW_MODE,viewMode);
#endif
    }
    
void MyChild::OnEditMode(wxCommandEvent& event)
    {
    wxMenuBar *mb = GetMenuBar () ;
    myass ( mb , "MyChild::OnEditMode_1" ) ;
    wxMenuItem *mi = mb->FindItem ( MDI_EDIT_MODE ) ;
    myass ( mi , "MyChild::OnEditMode_2" ) ;
    wxString s ;
    if ( viewMode ) OnViewMode ( event ) ;
    
    if ( !cSequence->getEditMode() )
        {
        sp1 = sw->GetSashPosition () ;
        sp2 = swl->GetSashPosition () ;
        sw->Unsplit ( swu ) ;
        cSequence->startEdit ( "DNA" ) ;
        cSequence->Scroll ( 0 , cSequence->getBatchMark() ) ;
        }
    else
        {
        vec->recalcvisual = true ;        
        cSequence->stopEdit() ;
        sw->SplitHorizontally ( swu , cSequence ) ;
        sw->SetSashPosition ( sp1 , true ) ;
        swl->SetSashPosition ( sp2 , true ) ;
        swu->Show ( TRUE ) ;
        }
    mi->Check ( cSequence->getEditMode() ) ;
    if ( cSequence->getEditMode() ) vec->undo.start ( txt("u_edit") ) ;
    else vec->undo.stop () ;
#ifdef __WXMSW__ // LINUX
    myass ( GetToolBar() , "MyChild::OnEditMode_3" ) ;
    GetToolBar()->ToggleTool(MDI_EDIT_MODE,cSequence->getEditMode());
#endif
    }

void MyChild::initPanels ()
    {
    wxStartTimer() ;    
    SeqFeature *seqF = new SeqFeature ( cSequence ) ;
    SeqDNA *seq = new SeqDNA ( cSequence ) ;
    SeqRestriction *seqR = new SeqRestriction ( cSequence ) ;
    SeqAA *seqAA = new SeqAA ( cSequence ) ;
    cSequence->seq.Clear () ;
//    cSequence->seq.Add ( seqF ) ;
//    cSequence->seq.Add ( seqAA ) ;
    cSequence->seq.Add ( seq ) ;
//    cSequence->seq.Add ( seqR ) ;
    seqF->aaa = seqAA ;
//    seqF->initFromTVector ( vec ) ;
    seq->initFromTVector ( vec ) ;    
//    seqR->initFromTVector ( vec ) ;    
//    seqAA->initFromTVector ( vec ) ;    
    seqAA->showNumbers = false ;

    wxMessageBox ( wxString::Format ( "2nd : %d ms" , wxGetElapsedTime() ) ) ;
    treeBox->initme () ;

    int a , b = AA_ALL ;
    for ( a = 0 ; b == AA_ALL && vec && a < vec->items.size() ; a++ )
        if ( vec->items[a].getRF() != 0 )
           b = AA_KNOWN ;

//    if ( aa_state != b ) OnAA_setit ( b ) ;
    
#ifdef __WXMSW__ // LINUX
    GetToolBar()->ToggleTool(MDI_CIRCULAR_LINEAR,vec->isCircular());
    GetToolBar()->ToggleTool(MDI_TOGGLE_FEATURES,cSequence->findID("FEATURE"));
    GetToolBar()->ToggleTool(MDI_TOGGLE_RESTRICTION,cSequence->findID("RESTRICTION"));
#endif

    if ( myapp()->frame->isLocked() ) return ;
    Show() ;
    SetFocus () ;
    cSequence->SetFocus() ;
    wxFocusEvent fev ;
    OnFocus(fev) ;

    Activate () ;
    }

void MyChild::OnHelp(wxCommandEvent& event)
    {
    myapp()->frame->OnHelp ( event ) ;
    }

void MyChild::updateSequenceCanvas ( bool remember )
    {
    myass ( cSequence , "MyChild::updateSequenceCanvas" ) ;
    TMarkMem mm ( cSequence ) ;
    int dummy , oldscrollpos ;
    cSequence->GetViewStart ( &dummy , &oldscrollpos ) ;
    int old_mode = aa_state ;
    
    if ( cSequence->findID ( "FEATURE" ) )
       cSequence->findID("FEATURE")->initFromTVector ( vec ) ;
    if ( old_mode != AA_NONE )
        {
        wxCommandEvent dummy ;
        OnAA_none ( dummy ) ;
        OnAA_setit ( old_mode ) ;
        }
    else
        {
        cSequence->arrange () ;
        if ( remember ) cSequence->SilentRefresh () ;
        else cSequence->Refresh () ;
        }
        
    if ( remember )
       {
       mm.remark () ;
       cSequence->Scroll ( -1 , oldscrollpos ) ;
       }
    cSequence->SetFocus () ;
    }
            
// The Amino Acid Display Orgy (TM)
    
void MyChild::OnAA_none(wxCommandEvent& event)
    {
    if ( aa_state == AA_NONE ) return ;
    cSequence->lastmarked-- ;
    TMarkMem mm ( cSequence ) ;
    wxMenuBar *mb = GetMenuBar () ;
    wxMenuItem *mi = mb->FindItem ( aa_state ) ;
    mi->Check ( false ) ;
    aa_state = AA_NONE ;
    mi = mb->FindItem ( aa_state ) ;
    mi->Check () ;
    myass ( aa_offset < cSequence->seq.GetCount() , "MyChild::OnAA_none" ) ;
    delete cSequence->seq[aa_offset] ;
    for ( int a = aa_offset+1 ; a < cSequence->seq.GetCount() ; a++ )
        cSequence->seq[a-1] = cSequence->seq[a] ;
    cSequence->seq.RemoveAt ( cSequence->seq.GetCount()-1 ) ;
    cSequence->arrange () ;
    cSequence->Refresh () ;
    mm.remark () ;
    }
    
void MyChild::OnAA_setit(int mode)
    {
    bool wasZero = aa_state == AA_NONE ;
    int oldscrollpos , dummy , oldmarkfrom , oldmarkto , oldmarkwhat ;
    oldmarkfrom = cSequence->markedFrom() ;
    oldmarkto = cSequence->markedTo() ;
    oldmarkwhat = cSequence->lastmarked ;
    cSequence->GetViewStart ( &dummy , &oldscrollpos ) ;
    if ( aa_state == mode ) return ;
    if ( aa_state == AA_NONE && (mode == AA_THREE || mode == AA_ONE ) ) return ;
    if ( aa_disp == mode && (mode == AA_THREE || mode == AA_ONE ) ) return ;
    wxMenuBar *mb = GetMenuBar () ;
    wxMenuItem *mi ;
    if ( mode == AA_ALL && aa_disp == AA_THREE )
        {
        aa_disp = AA_ONE ;
        mb->FindItem(AA_THREE)->Check ( false ) ;
        mb->FindItem(AA_ONE)->Check () ;
        }
    if ( mode == AA_THREE || mode == AA_ONE ) mi = mb->FindItem ( aa_disp ) ;
    else mi = mb->FindItem ( aa_state ) ;
    mi->Check ( false ) ;
    if ( aa_state == AA_NONE )
        {
        cSequence->seq.Add ( NULL ) ;
        for ( int a = cSequence->seq.GetCount()-1 ; a >= 1+aa_offset ; a-- )
           cSequence->seq[a] = cSequence->seq[a-1] ;
        SeqAA *seqAA = new SeqAA ( cSequence ) ;
        cSequence->seq[aa_offset] = seqAA ;
        seqAA->initFromTVector ( vec ) ;    
        seqAA->showNumbers = false ;
        cSequence->lastmarked++ ;
        }
    TMarkMem mm ( cSequence ) ;
    if ( mode == AA_THREE || mode == AA_ONE ) aa_disp = mode ;
    else aa_state = mode ;
    mi = mb->FindItem ( aa_state ) ;
    mi->Check () ;
    SeqAA *seqAA = (SeqAA*) cSequence->seq[aa_offset] ;
    seqAA->mode = aa_state ;
    seqAA->disp = aa_disp ;
    seqAA->initFromTVector ( vec ) ;
    seqAA->showNumbers = false ;
    
    for ( int u = 0 ; u < cSequence->seq.GetCount() ; u++ )
        {
        if ( cSequence->seq[u]->whatsthis() == "FEATURE" )
           {
           SeqFeature *feat = (SeqFeature*) cSequence->seq[u] ;
           feat->aaa = seqAA ;
           }
        }
    
    cSequence->arrange () ;
    cSequence->Refresh ( false ) ;
    if ( !wasZero && aa_state != AA_NONE )
        cSequence->Scroll ( -1 , oldscrollpos ) ;
    if ( !wasZero && !cSequence->getEditMode() && oldmarkfrom != -1 && oldmarkwhat >= 0 )
        {
        wxString s = cSequence->seq[oldmarkwhat]->whatsthis() ;
        cSequence->mark ( s , oldmarkfrom , oldmarkto ) ;
        }
    mm.remark () ;
    }

void MyChild::OnAA_known(wxCommandEvent& event)
    {    OnAA_setit ( AA_KNOWN ) ;    }
    
void MyChild::OnAA_all(wxCommandEvent& event)
    {    OnAA_setit ( AA_ALL ) ;    }
    
void MyChild::OnAA_three_1(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_1 ) ;    }
    
void MyChild::OnAA_three_2(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_2 ) ;    }
    
void MyChild::OnAA_three_3(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_3 ) ;    }
    
void MyChild::OnAA_three_M1(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_M1 ) ;    }
    
void MyChild::OnAA_three_M2(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_M2 ) ;    }
    
void MyChild::OnAA_three_M3(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_M3 ) ;    }
    
void MyChild::OnAA_three(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE ) ;    }
    
void MyChild::OnAA_one(wxCommandEvent& event)
    {    OnAA_setit ( AA_ONE ) ;    }
    
void MyChild::OnPrintImage(wxCommandEvent& event)
    {
    cPlasmid->print() ;
    }
    
void MyChild::OnFind(wxCommandEvent& event)
    {
    FindSequenceDialog fsd ( this , txt("t_find") ) ;
    fsd.ShowModal () ;
    }
    
void MyChild::OnMarkAll(wxCommandEvent& event)
    {
    cSequence->mark ( "DNA" , 1 , vec->getSequenceLength() ) ;
    Refresh () ;
    }

wxString MyChild::doExtractAA ( bool coding )
    {
    if ( aa_state == AA_NONE || aa_state == AA_ALL )
        {
        wxMessageDialog md ( this , txt("t_decide_aa_schema") , txt("msg_box") ) ;
        md.ShowModal () ;
        return "" ;
        }
        
    if ( aa_state == AA_KNOWN )
        {
        wxMessageDialog md ( this , txt("t_attention_aa") , txt("msg_box") ) ;
        md.ShowModal () ;
        }
        
    if ( cPlasmid->getMarkFrom() == -1 ) // No selection = all
        {
        wxCommandEvent event ;
        OnMarkAll ( event ) ;
        }
    
    SeqAA aa ( NULL ) ;
    aa.mode = aa_state ;
    aa.disp = AA_ONE ;
    aa.initFromTVector ( vec ) ;
    
    wxString z = aa.s , t ;
    int a ;
    int from , to ;
    cPlasmid->getMark ( from , to ) ;
    if ( to <= z.length() )
        {
        z = z.substr ( from-1 , to-from ) ;
        }
    else
        {
        z = z.substr ( from-1 ) + z.substr ( 0 , to-z.length() ) ;
        }
        
    for ( a = 0 ; a < z.length() ; a++ )
        {
        if ( z.GetChar(a) != ' ' && z.GetChar(a) != '|' )
           {
           if ( aa_state != AA_THREE_M1 &&
                aa_state != AA_THREE_M2 &&
                aa_state != AA_THREE_M3 )
              t += z.GetChar(a) ;
           else
              t = z.GetChar(a) + t ;
           }
        }
    return t ;
    }
    
void MyChild::OnExtractAA(wxCommandEvent& event)
    {
    wxString seq = doExtractAA () ;
    if ( seq.IsEmpty() ) return ;
    char tt[1000] ;
    sprintf ( tt , txt("t_aa_from_vec") , vec->getName().c_str() ) ;
    myapp()->frame->newAminoAcids ( seq , tt ) ;
    }
        
void MyChild::OnRestriction(wxCommandEvent& event)
    {
    MyFrame *f = myapp()->frame ; //(MyFrame*) GetParent() ;
    TRestrictionEditor ed ( f , "" , wxPoint(-1,-1) , wxSize(600,400) , 
               wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL);
//    ed.pre = s ;
    ed.cocktail = vec->cocktail ;
    ed.remoteCocktail = &vec->cocktail ;
    ed.initme ( vec ) ;
    if ( ed.ShowModal () == wxID_OK )
       {
       vec->cocktail = ed.cocktail ;
       vec->doAction() ;
       }
    }
    
void MyChild::OnTransformSequence(wxCommandEvent& event)
    {
    TransformSequenceDialog tsd ( this , txt("t_transform_sequence") ) ;
    if ( tsd.ShowModal() != wxID_OK ) return ;
    
    int a ;
    TVector *v = vec ;
    bool inNewVector = tsd.new_item->GetValue() ;
    if ( inNewVector ) // Creating new vector, if needed
        {
        v = new TVector ;
        *v = *vec ;
        }
    else // Just transform the old one
        {
        vec->undo.start( txt("u_transform") ) ;
        }
    
    // Transforming DNA
    v->setSequence ( v->transformSequence ( tsd.complement->GetValue() ,
                                         tsd.invert->GetValue() ) ) ;

    // Transforming items
    if ( tsd.invert->GetValue() )
        {
        int l = v->getSequenceLength() ;
        for ( a = 0 ; a < v->items.size() ; a++ )
           {
           v->items[a].direction *= -1 ;
           int from = v->items[a].from ;
           int to = v->items[a].to ;
           
           from = l - from + 1 ;
           to = l - to + 1 ;
           
           v->items[a].from = to ;
           v->items[a].to = from ;
           }
        }
          
    // Display
    if ( inNewVector )
        {
        v->addName ( "*" ) ;
        v->recalcvisual = true ;
        v->recalculateCuts() ;
        MyChild *c = myapp()->frame->newFromVector(v) ;
        }
    else
        {
        v->recalcvisual = true ;
        v->recalculateCuts() ;
        vec->undo.stop() ;
        cSequence->findID("DNA")->initFromTVector ( v ) ;
        if ( aa_state != AA_NONE )
           {
           int old_mode = aa_state ;
           wxCommandEvent dummy ;
           OnAA_none ( dummy ) ;
           OnAA_setit ( old_mode ) ;
           }
        cSequence->arrange () ;
        cSequence->Refresh() ;
        cPlasmid->Refresh() ;
        }
    }

void MyChild::OnEditORFs(wxCommandEvent& event)
    {
    wxString *vs = new wxString[6] ;
    vs[0] = txt("t_orf_1") ;
    vs[1] = txt("t_orf_2") ;
    vs[2] = txt("t_orf_3") ;
    vs[3] = txt("t_orf_1c") ;
    vs[4] = txt("t_orf_2c") ;
    vs[5] = txt("t_orf_3c") ;
    TMyMultipleChoiceDialog mcd ( this ,
                                  txt("t_orf_msg") ,
                                  txt("msg_box") ,
                                  6 ,
                                  vs ) ;

    mcd.Check ( 0 , ( orf_mode &  1 ) > 0 ) ;
    mcd.Check ( 1 , ( orf_mode &  2 ) > 0 ) ;
    mcd.Check ( 2 , ( orf_mode &  4 ) > 0 ) ;
    mcd.Check ( 3 , ( orf_mode &  8 ) > 0 ) ;
    mcd.Check ( 4 , ( orf_mode & 16 ) > 0 ) ;
    mcd.Check ( 5 , ( orf_mode & 32 ) > 0 ) ;

    if ( mcd.ShowModal() != wxID_OK ) return ;
    orf_mode = 0 ;
    if ( mcd.IsChecked ( 0 ) ) orf_mode +=  1 ;
    if ( mcd.IsChecked ( 1 ) ) orf_mode +=  2 ;
    if ( mcd.IsChecked ( 2 ) ) orf_mode +=  4 ;
    if ( mcd.IsChecked ( 3 ) ) orf_mode +=  8 ;
    if ( mcd.IsChecked ( 4 ) ) orf_mode += 16 ;
    if ( mcd.IsChecked ( 5 ) ) orf_mode += 32 ;
    showORFs = false ;
    OnORFs ( event ) ;
    }
    
void MyChild::OnORFs(wxCommandEvent& event)
    {
    vec->ClearORFs() ;
    showORFs = !showORFs ;
    if ( showORFs )
       {
       if ( ( orf_mode &  1 ) > 0 ) vec->addORFs ( 1 ) ;
       if ( ( orf_mode &  2 ) > 0 ) vec->addORFs ( 2 ) ;
       if ( ( orf_mode &  4 ) > 0 ) vec->addORFs ( 3 ) ;
       if ( ( orf_mode &  8 ) > 0 ) vec->addORFs ( -1 ) ;
       if ( ( orf_mode & 16 ) > 0 ) vec->addORFs ( -2 ) ;
       if ( ( orf_mode & 32 ) > 0 ) vec->addORFs ( -3 ) ;
       }
#ifdef __WXMSW__ // LINUX
    GetToolBar()->ToggleTool(MDI_ORFS,showORFs);
#endif
    cPlasmid->Refresh() ;
    }
    
void MyChild::OnSeqPrint(wxCommandEvent& event)
    {
    cSequence->OnPrint ( event ) ;
    }
        
void MyChild::OnPrintReport(wxCommandEvent& event)
    {
    wxPrintDialog pd ( this ) ;
    int r = pd.ShowModal () ;
    if ( r != wxID_OK ) return ;

    bool svt = myapp()->frame->showVectorTitle ;
    bool svl = myapp()->frame->showVectorLength ;
    myapp()->frame->showVectorTitle = true ;
    myapp()->frame->showVectorLength = true ;

    int w , h ;
    wxDC *pdc = pd.GetPrintDC () ;
    pdc->GetSize ( &w , &h ) ;
    pdc->StartDoc ( vec->getName() ) ;
    pdc->StartPage () ;
    
    // Plasmid canvas
    pdc->SetDeviceOrigin ( 0 , -h/4 ) ;
    cPlasmid->setPrinting ( true ) ;
    cPlasmid->OnDraw ( *pdc ) ;
    cPlasmid->setPrinting ( false ) ;
    
    // Feature list
    pdc->SetDeviceOrigin ( 0 , h/2 ) ;
    int cw , ch ;
    wxFont *font = MYFONT ( w/80 , wxMODERN , wxNORMAL , wxNORMAL ) ; 
    wxFont *sfont = MYFONT ( w/120 , wxMODERN , wxNORMAL , wxNORMAL ) ; 
    wxFont *bfont = MYFONT ( w/40 , wxMODERN , wxNORMAL , wxNORMAL ) ; 
    pdc->SetFont ( *font ) ;
    pdc->GetTextExtent ( "A" , &cw , &ch ) ;
    
    int a , b ;
    int x , y ;
    int x0 = w / 20 ;
    int x1 = x0 + w / 5 ;
    int x2 = x1 + w / 12 ;
    int x3 = x2 + w / 12 ;
    int x4 = x3 + w / 12 ;
    int x5 = x4 + w / 4 ;
    char t[10000] ;
    
    y = 0 ;
    pdc->DrawText ( txt("name") , x0 , y ) ;
    pdc->DrawText ( txt("from") , x1 , y ) ;
    pdc->DrawText ( txt("to") , x2 , y ) ;
    pdc->DrawText ( txt("length") , x3 , y ) ;
    pdc->DrawText ( txt("type") , x4 , y ) ;
    pdc->DrawText ( txt("direction") , x5 , y ) ;
    y += ch / 2 ;
    pdc->DrawLine ( x0 , y+ch*6/10 , w - x0 , y+ch*6/10 ) ;
    pdc->DrawLine ( x0 , y+ch*8/10 , w - x0 , y+ch*8/10 ) ;
    
    for ( a = 0 ; a < vec->items.size() ; a++ )
        {
        y += ch ;
        pdc->DrawText ( vec->items[a].name , x0 , y ) ;
        sprintf ( t , "%d" , vec->items[a].from ) ; pdc->DrawText ( t , x1 , y ) ;
        sprintf ( t , "%d" , vec->items[a].to ) ; pdc->DrawText ( t , x2 , y ) ;
        
        int len = vec->items[a].to - vec->items[a].from + 1 ;
        if ( vec->items[a].to < vec->items[a].from )
           len += vec->getSequenceLength() ;
        sprintf ( t , "%d" , len ) ; pdc->DrawText ( t , x3 , y ) ;
        
        sprintf ( t , "itemtype%d" , vec->items[a].type ) ;
        pdc->DrawText ( txt(t) , x4 , y ) ;
        if ( vec->items[a].direction > 0 ) pdc->DrawText ( txt("cw") , x5 , y ) ;
        else pdc->DrawText ( txt("ccw") , x5 , y ) ;
        
        if ( !vec->items[a].desc.IsEmpty() )
           {
           wxArrayString vs ;
           wxString s ;
           strcpy ( t , vec->items[a].desc ) ;

           int cnt = 0 , ls = 0 ;
           for ( b = 0 ; t[b] ; b++ )
              {
              cnt++ ;
              if ( cnt > w / cw )
                 {
                 t[ls] = '\n' ;
                 cnt = b - ls ;
                 }
              else if ( t[b] == ' ' ) ls = b ;
              }

           for ( b = 0 ; t[b] ; b++ )
              {
              if ( t[b] > 15 ) s += t[b] ;
              else
                 {
                 vs.Add ( s ) ;
                 s = "" ;
                 }
              }
           if ( !s.IsEmpty() ) vs.Add ( s ) ;
           int dx , dy ;
           y += ch ;
           pdc->SetFont ( *sfont ) ;
           for ( b = 0 ; b < vs.GetCount() ; b++ )
              {
              pdc->GetTextExtent ( vs[b] , &dx , &dy ) ;
              pdc->DrawText ( vs[b] , x0 + cw * 3 , y ) ;
              y += dy ;
              }
           y -= ch ;
           pdc->SetFont ( *font ) ;
           }
        
        pdc->DrawLine ( x0 , y + ch + 2 , w - x0 , y + ch + 2 ) ;
        y += 5 ;
        }
    y += ch * 2 ;
    pdc->DrawText ( vec->getDescription() , x0 , y ) ;

    // Name, date
    int tw , th ;
    pdc->SetDeviceOrigin ( 0 , 0 ) ;
    wxDateTime now = wxDateTime::Now();
    wxString printtime = now.Format("%c", wxDateTime::Local) ;
    pdc->GetTextExtent ( printtime , &tw , &th ) ;
    pdc->DrawText ( printtime , w - x0 - tw , 0 ) ;
    pdc->SetFont ( *bfont ) ;
    pdc->DrawText ( vec->getName() , x0 , 0 ) ;
    
    pdc->EndPage () ;
   
    pdc->EndDoc () ;
    
    myapp()->frame->showVectorTitle = svt ;
    myapp()->frame->showVectorLength = svl ;
    }
    

void MyChild::OnZoom ( wxCommandEvent &ev )
    {
    wxString s = zoom_cb->GetStringSelection() ;
    int i = atoi ( s.c_str() ) ;
    cPlasmid->setZoom ( i ) ;
    }

void MyChild::Undo(wxCommandEvent& event)
    {
    if ( !vec->undo.canUndo() ) return ;
    vec->undo.pop() ;
    initPanels () ;

    updateSequenceCanvas ( false ) ;
    cPlasmid->Refresh() ;
    treeBox->Refresh() ;
    updateUndoMenu () ;
    }
    
void MyChild::updateUndoMenu ()
    {
    if ( !vec ) return ;
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
#ifdef __WXMSW __
    GetToolBar()->EnableTool ( MDI_UNDO , canUndo ) ;
#else
    //    myapp()->frame->GetToolBar()->EnableTool ( MDI_UNDO , canUndo ) ;
#endif
    }
    
void MyChild::Redo(wxCommandEvent& event)
    {
    
    }
    
// *************************

MySplitter::MySplitter ( wxWindow *win , int id , MyChild *child )
    : wxSplitterWindow ( win , id , wxDefaultPosition , wxDefaultSize , wxSP_3DSASH )
    {
    c = child ;
    }

void MySplitter::OnChanged ( wxSplitterEvent &ev )
    {
    myass ( c , "MySplitter::OnChanged" ) ;
    int np = ev.GetSashPosition() ;
    if ( np < GetMinimumPaneSize() ) 
        {
        return ;
        }
    
    if ( this == c->sw )
        {
        int op = GetSashPosition() ;
        int lp = c->swl->GetSashPosition() ;
        lp = lp * np / op ;
        c->swl->SetSashPosition ( lp ) ;
        }
    else if ( this == c->swl ) return ;
    
    SetSashPosition ( np ) ;
    c->cPlasmid->Refresh();
    
    if ( this == c->swu ) return ;

    c->cSequence->Refresh() ;
    }

