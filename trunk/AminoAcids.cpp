#include "AminoAcids.h"

BEGIN_EVENT_TABLE(TAminoAcids, wxMDIChildFrame)
    EVT_MENU(MDI_EDIT_MODE, TAminoAcids::OnEditMode)
    EVT_MENU(MDI_MARK_ALL, TAminoAcids::OnMarkAll)
    EVT_MENU(MDI_FILE_SAVE, TAminoAcids::OnFileSave)
    EVT_MENU(MDI_EXPORT, TAminoAcids::OnExport)
    EVT_MENU(MDI_FIND, TAminoAcids::OnFind)
    EVT_MENU(MDI_CUT, TAminoAcids::OnCut)
    EVT_MENU(MDI_COPY, TAminoAcids::OnCopy)
    EVT_MENU(AMINOACIDS_EDIT_NAME, TAminoAcids::OnEditName)
    EVT_MENU(SEQ_PRINT, TAminoAcids::OnPrint)
    EVT_MENU(MDI_AS_NEW_FEATURE, TAminoAcids::OnAsNewFeature)
    EVT_MENU(AMINOACIDS_BLAST_AA, TAminoAcids::OnBlastAA)

    EVT_MENU(MDI_UNDO, TAminoAcids::Undo)
    EVT_MENU(MDI_REDO, TAminoAcids::Redo)

    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_CLOSE(TAminoAcids::OnClose)
END_EVENT_TABLE()

TAminoAcids::TAminoAcids(wxMDIParentFrame *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    vec = new TVector ( this ) ;
    def = "AminoAcids" ;
    vec->name = title.c_str() ;
    from = -1 ;
    vec->undo.clear() ;
    stat = NULL ;
    
    app = ((MyFrame*) parent)->app ;
    }
    
TAminoAcids::~TAminoAcids ()
    {
    if ( vec ) delete vec ;
    if ( stat ) delete stat ;
//    if ( app ) app->my_children.DeleteObject(this);    
    }


void TAminoAcids::OnClose(wxCloseEvent& event)
{
    if ( !caniclose ( event ) ) return ;
    
    // Removing the window from the main tree
    MyFrame *p = (MyFrame*)GetParent();
    p->mainTree->removeChild ( this ) ;
    p->SetTitle ( txt("gentle") ) ;
    SetTitle ( txt("gentle") ) ;
    
    // Removing from frame children list
    int a ;
    for ( a = 0 ; a < p->children.size() && p->children[a] != this ; a++ ) ;
    if ( a < p->children.size() )
        {
        p->children[a] = p->children[p->children.size()-1] ;
        p->children.pop_back () ;
        }

    event.Skip();
}
    
void TAminoAcids::initme ( MyApp *_app )
    {
    app = _app ;
    int bo = 5 ;

    // Menus
    wxMenu *file_menu = app->frame->getFileMenu ( true , true ) ;
    wxMenu *tool_menu = app->frame->getToolMenu ( true ) ;
    wxMenu *help_menu = app->frame->getHelpMenu () ;


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



    hs = new wxSplitterWindow ( this , SPLIT_AMINOACIDS ) ;
    vs = new wxSplitterWindow ( hs , -1 ) ;

    // Sequence Canvas
    sc = new SequenceCanvas ( hs , wxPoint ( 0 , 0 ) , wxSize ( 100 , 100 ) ) ;
    sc->blankline = 1 ;
    sc->app = app ;
    sc->aa = this ;
    sc->child = this ;
    sc->edit_id = "AA" ;
    sc->edit_valid = "ACDEFGHIKLMNPQRSTVWY|" ;

    int h = 100 ;
    
    stat = new wxTextCtrl ( vs ,
                            -1 ,
                            "" ,
                            wxDefaultPosition,
                            wxDefaultSize ,
                            wxTE_MULTILINE | wxTE_READONLY ) ;

    desc = new TURLtext ( vs ,
                            URLTEXT_DUMMY ,
                            vec->desc.c_str() ,
                            wxDefaultPosition,
                            wxDefaultSize ,
                            wxTE_MULTILINE | wxTE_READONLY ) ;

    hs->SplitHorizontally ( vs , sc ,h+bo ) ;
    vs->SplitVertically ( stat , desc ) ;
    hs->SetMinimumPaneSize ( h+bo ) ;
    
                            
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    app->frame->InitToolBar(toolBar);
    toolBar->AddTool( MDI_TEXT_IMPORT , 
                wxBitmap (app->bmpdir+"\\new.bmp", wxBITMAP_TYPE_BMP),
                txt("m_new_sequence") ) ;
    toolBar->AddTool( MDI_FILE_OPEN, 
            wxBitmap (app->bmpdir+"\\open.bmp", wxBITMAP_TYPE_BMP), 
            txt("m_open") , txt("m_opentxt") );
    toolBar->AddTool( MDI_FILE_SAVE, 
                wxBitmap (app->bmpdir+"\\save.bmp", wxBITMAP_TYPE_BMP),
                txt("m_store_in_db") , 
                txt("m_txt_store_in_db"));
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_CUT,
        wxBitmap (app->bmpdir+"\\cut.bmp", wxBITMAP_TYPE_BMP)) ;
    toolBar->AddTool( MDI_COPY,
        wxBitmap (app->bmpdir+"\\copy.bmp", wxBITMAP_TYPE_BMP)) ;
    toolBar->AddTool( MDI_PASTE,
        wxBitmap (app->bmpdir+"\\paste.bmp", wxBITMAP_TYPE_BMP)) ;
    toolBar->Realize() ;

    showSequence () ;
    showStat () ;
    updateUndoMenu () ;
    sc->SetFocus() ;
    }
    
string TAminoAcids::getName ()
    {
    return vec->name ;
    }

void TAminoAcids::showStat ()
    {
    char t[10000] ;
    int noaa = 0 , piaa = 0 ;
    float pI = 0 ;
    float mW = 0 ;
    float ex = 0 , abs = 0 ;
    int num[256] ;
    int a ;
    for ( a = 0 ; a < 256 ; a++ ) num[a] = 0 ;
    for ( a = 0 ; a < vec->sequence.length() ; a++ )
        {
        unsigned char c = (unsigned char)vec->sequence[a] ;
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
    desc->SetValue ( vec->desc.c_str() ) ;
    }

void TAminoAcids::showSequence ()
    {
    // Cleaning up
    while ( sc->seq.size() )
        {
        delete sc->seq[sc->seq.size()-1] ;
        sc->seq.pop_back () ;
        }

    // Display
    SeqFeature *seqF = new SeqFeature ( sc ) ;
    sc->seq.push_back ( seqF ) ;
    seqF->initFromTVector ( vec ) ;


    SeqAA *d = new SeqAA ( sc ) ;
    sc->seq.push_back ( d ) ;
    d->primaryMode = true ;
    d->takesMouseActions = true ;
    d->initFromString ( vec->sequence ) ;
    d->fixOffsets ( vec ) ;
    
    seqF->aaa = d ;
        
    sc->arrange () ;
    sc->Refresh () ;    
    showStat() ;
    }
    
void TAminoAcids::OnEditMode(wxCommandEvent& event)
    {
    wxMenuBar *mb = GetMenuBar () ;
    wxMenuItem *mi = mb->FindItem ( MDI_EDIT_MODE ) ;
    wxString s ;
    if ( mi->IsChecked() )
        {
        sc->editMode = true ;
        sc->findID("AA")->s += " " ;
        vec->sequence += " " ;
        sc->arrange () ;
        if ( sc->_from == -1 ) sc->mark ( "AA" , 1 , 1 , 2 ) ;
        else sc->mark ( "AA" , sc->_from , sc->_from , 2 ) ;
        sc->SetFocus() ;
        }
    else
        {
        sc->editMode = false ;
        sc->mark ( "AA" , -1 , -1 ) ;
        vec->sequence.erase ( vec->sequence.length()-1 , 1 ) ;
        sc->findID("AA")->s.erase ( sc->findID("AA")->s.length()-1 , 1 ) ;
        sc->arrange () ;
        Refresh () ;
        }
    if ( sc->editMode ) vec->undo.start ( txt("u_edit") ) ;
    else vec->undo.stop () ;
    }

void TAminoAcids::invokeVectorEditor ( string what , int num , bool forceUpdate )
    {
    vec->undo.start ( txt("u_vec_edit") ) ;
    TVectorEditor ve ( this , txt("t_vector_editor") , vec , app ) ;
    bool changed = vec->isChanged() ;
    string on = vec->name ;
    vec->setChanged ( false ) ;

    ve.hideEnzym = true ;
    ve.hideEm () ;
    
    if ( what == "item" )
        ve.initialViewItem ( num ) ;

    int x = ve.ShowModal () ;
    ve.cleanup () ;
    if ( forceUpdate || vec->isChanged() )
        {
        app->frame->mainTree->SetItemText ( inMainTree , getName().c_str() ) ;
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
    sc->mark ( "AA" , 1 , vec->sequence.length() ) ;
    }
    
void TAminoAcids::OnPrint(wxCommandEvent& event)
    {
    sc->OnPrint ( event ) ;
    }
    
void TAminoAcids::OnFileSave(wxCommandEvent& event)
    {
    TManageDatabaseDialog dbd ( this , txt("t_store") , app , ACTION_MODE_SAVE , vec ) ;
    dbd.ShowModal () ;
    }
    
void TAminoAcids::OnExport(wxCommandEvent& event)
    {
    wxMessageDialog md ( this , txt("t_not_yet_implemented") ) ;
    md.ShowModal() ;
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
    int from = sc->_from ;
    int to = sc->_to ;
    if ( from == -1 ) return ;
    
    vec->undo.start ( txt("u_new_feature") ) ;

    TVectorItem nvi ;
    sprintf ( t , txt("t_new_item_title") , from , to ) ;
    nvi.name = t ;
    nvi.direction = 1 ;
    nvi.type = VIT_MISC ;
    nvi.from = from ;
    nvi.to = to ;
    vec->setChanged () ;
    vec->items.push_back ( nvi ) ;
    invokeVectorEditor ( "item" , vec->items.size()-1 , true ) ;
    vec->undo.stop() ;
    }
    
void TAminoAcids::OnBlastAA(wxCommandEvent& event)
    {
    string seq ;
    int a ;
    if ( sc->_from < 0 ) seq = vec->sequence ;
    else
        {
        for ( a = sc->_from-1 ; a < sc->_to ; a++ ) seq += vec->sequence[a] ;
        }
    app->frame->blast ( seq , "blastp" ) ;
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
    if ( lm == "" )
        {
        mi->SetText ( txt("u_no") ) ;
        mi->Enable ( false ) ;
        }
    else
        {
        mi->Enable ( true ) ;
        mi->SetText ( lm ) ;
        }
    }
    
void TAminoAcids::Redo(wxCommandEvent& event)
    {
    
    }
    
