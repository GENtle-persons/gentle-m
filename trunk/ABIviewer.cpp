#include "AminoAcids.h"

BEGIN_EVENT_TABLE(TABIviewer, MyChildBase)
    EVT_MENU(MDI_EDIT_MODE, TABIviewer::OnEditMode)
    EVT_MENU(MDI_MARK_ALL, TABIviewer::OnMarkAll)
    EVT_MENU(MDI_FILE_SAVE, TABIviewer::OnFileSave)
    EVT_MENU(MDI_EXPORT, TABIviewer::OnExport)
    EVT_MENU(MDI_FIND, TABIviewer::OnFind)
    EVT_MENU(MDI_COPY, TABIviewer::OnCopy)
    EVT_MENU(AMINOACIDS_EDIT_NAME, TABIviewer::OnEditName)
    EVT_MENU(MDI_COPY_TO_NEW,TABIviewer::OnCopyToNew)
    EVT_MENU(SEQ_PRINT, TABIviewer::OnSeqPrint)

    EVT_CHECKBOX(ABI_HELPLINES, TABIviewer::OnHelplines)
    EVT_CHECKBOX(ABI_INV_COMP, TABIviewer::OnInvCompl)
    EVT_SPINCTRL(ABI_WIDTH, TABIviewer::OnSpinWidth)
    EVT_SPINCTRL(ABI_HEIGHT, TABIviewer::OnSpinHeight)
    EVT_COMMAND_SCROLL(ABI_SLIDER, TABIviewer::OnZoom)

    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_CLOSE(TABIviewer::OnClose)
END_EVENT_TABLE()

TABIviewer::TABIviewer(MyFrame *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    vec = new TVector ( this ) ;
    def = "ABIviewer" ;
    vec->name = title.c_str() ;
    from = -1 ;
    vec->undo.clear () ;
    stat = NULL ;
    }
    
TABIviewer::~TABIviewer ()
    {
    if ( vec ) delete vec ;
    if ( stat ) delete stat ;
    }
    
void TABIviewer::OnZoom(wxScrollEvent& event)
    {
    int i = event.GetPosition() ;
    SeqABI *abi = (SeqABI*) sc->seq[0] ;
    abi->zoom = i ;
    sc->SilentRefresh() ;
    }

void TABIviewer::OnHelplines(wxCommandEvent& event)
    {
    bool state = aidLines->GetValue() ;
    SeqABI *abi = (SeqABI*) sc->seq[0] ;
    abi->showHelpLines = state ;
    sc->SilentRefresh () ;
    }
    
void TABIviewer::OnSpinWidth(wxSpinEvent& event)
    {
    int value = f_width->GetValue() ;
    SeqABI *abi = (SeqABI*) sc->seq[0] ;
    abi->screenScale = value ;
    sc->arrange () ;
    sc->SilentRefresh () ;
    }
    
void TABIviewer::OnSpinHeight(wxSpinEvent& event)
    {
    int value = f_height->GetValue() ;
    sc->blankline = value + 1 ;
    sc->arrange () ;
    sc->SilentRefresh () ;    
    }

void TABIviewer::OnClose(wxCloseEvent& event)
{
    if ( !caniclose ( event ) ) return ;
    
    // Removing the window from the main tree
    MyFrame *p = myapp()->frame ;
    p->mainTree->removeChild ( this ) ;
    p->SetTitle ( txt("gentle") ) ;
    SetTitle ( txt("gentle") ) ;
    p->removeChild ( this ) ;    
    event.Skip();
}
    
void TABIviewer::initme ()
    {
    int bo = 5 ;

    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(MDI_MARK_ALL, txt("m_mark_all") );
//    edit_menu->Append(MDI_CUT, txt("m_cut") );
    edit_menu->Append(MDI_COPY, txt("m_copy") );
//    edit_menu->Append(MDI_PASTE, txt("m_paste") );
//    edit_menu->AppendSeparator();
//    edit_menu->Append(MDI_FIND, txt("m_find") );
//    edit_menu->AppendSeparator();
    edit_menu->Append(MDI_COPY_TO_NEW, txt("m_copy_to_new") );
//    edit_menu->Append(MDI_AS_NEW_FEATURE, txt("m_as_new_feature") );
//    edit_menu->Append(MDI_EXTRACT_AA, txt("m_extract_aa") );

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

    // Sequence Canvas
    sc = new SequenceCanvas ( hs , wxPoint ( 0 , 0 ) , wxSize ( 100 , 100 ) ) ;
    sc->blankline = 0 ;
    sc->edit_id = "ABI" ;
    sc->edit_valid = "ATGCN" ;
    sc->forceOverwrite ( true ) ;
    sc->child = this ;

    // Upper panel
    Maximize ( true ) ;
    int w , h , th = 20 ;
    GetClientSize ( &w , &h ) ;
    up = new wxPanel ( hs , -1 , wxDefaultPosition , wxSize ( w , 90 ) ) ;
    up->GetClientSize ( &w , &h ) ;
    
    hs->SplitHorizontally ( up , sc ,h+bo ) ;
    hs->SetMinimumPaneSize ( h+bo ) ;
    
    // Toys
    wxRect r ;
    aidLines = new wxCheckBox ( up , ABI_HELPLINES , txt("t_aid_lines") , wxPoint ( bo , bo ) ) ;
    r = aidLines->GetRect() ;
    inv_compl = new wxCheckBox ( up , ABI_INV_COMP , txt("t_abi_inv_comp") , wxPoint ( r.GetRight()+bo , r.GetTop() ) ) ;
    f_height = new wxSpinCtrl ( up , ABI_HEIGHT , "5" , wxPoint ( bo , bo + th * 1 ) , wxSize ( 50 , -1 ) ) ;
    f_width = new wxSpinCtrl ( up , ABI_WIDTH , "2" , wxPoint ( bo , bo + th * 2 ) , wxSize ( 50 , -1 ) ) ;
    f_height->SetRange ( 1 , 20 ) ;
    f_width->SetRange ( 1 , 9 ) ;
    aidLines->SetValue ( true ) ;
    r = f_height->GetRect() ;
    new wxStaticText ( up , -1 , txt("t_scale_height") , wxPoint ( r.GetRight()+bo , r.GetTop()+2 ) ) ;
    r = f_width->GetRect() ;
    new wxStaticText ( up , -1 , txt("t_scale_width") , wxPoint ( r.GetRight()+bo , r.GetTop()+2 ) ) ;
    
    slider = new wxSlider ( up , ABI_SLIDER , 1 , 1 , 50 , 
                             wxPoint ( bo , r.GetBottom()+bo ) ,
                             wxSize ( 100 , th ) ,
                             wxSL_HORIZONTAL ) ;
    r = slider->GetRect() ;
    new wxStaticText ( up , -1 , txt("t_zoom") , wxPoint ( r.GetRight()+bo , r.GetTop()+2 ) ) ;
    r = inv_compl->GetRect() ;

    // Statistics box
    stat = new wxTextCtrl ( up ,
                            -1 ,
                            "" ,
                            wxPoint ( r.GetRight()+bo , 0 ) ,
                            wxSize ( w - (r.GetRight()+bo) , h ) ,
                            wxTE_MULTILINE | wxTE_READONLY ) ;

                               
#ifdef __WXMSW__ // LINUX
    // Toolbar
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    myapp()->frame->InitToolBar(toolBar);
    toolBar->AddTool( MDI_TEXT_IMPORT , 
                myapp()->frame->bitmaps[0] ,
                txt("m_new_sequence") ) ;
    toolBar->AddTool( MDI_FILE_OPEN, 
            myapp()->frame->bitmaps[1] ,
            txt("m_open") , txt("m_opentxt") );
    toolBar->AddTool( MDI_FILE_SAVE, 
                myapp()->frame->bitmaps[2] ,
                txt("m_store_in_db") , 
                txt("m_txt_store_in_db"));
    toolBar->AddSeparator() ;
//    toolBar->AddTool( MDI_CUT, myapp()->frame->bitmaps[4] ) ;
    toolBar->AddTool( MDI_COPY, myapp()->frame->bitmaps[5] ) ;
//    toolBar->AddTool( MDI_PASTE, myapp()->frame->bitmaps[6] ) ;
    toolBar->Realize() ;
#endif

    stat->SetFont ( *MYFONT ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ) ;

    showSequence () ;
    showStat () ;
    sc->SetFocus() ;
    myapp()->frame->setChild ( this ) ;
    }
    
string TABIviewer::getName ()
    {
    return vec->name ;
    }

string TABIviewer::getStat ()
    {
    SeqABI *abi = (SeqABI*) sc->seq[0] ;
    ABItype *at = abi->at ;
    char t[100] ;
    string comb = at->getRecordPascalString("CMBF",1) ;
    string pdmf = at->getRecordPascalString("PDMF",1) ;
    string smpl = at->getRecordPascalString("SMPL",1) ;
    string geln = at->getRecordPascalString("GELN",1) ;
    string gelp = at->getRecordPascalString("GELP",1) ;
    
    int lane = at->getRecordValue("LANE",1) >> 16 ;
    int rund1 = at->getRecordValue("RUND",1) ;
    int rund2 = at->getRecordValue("RUND",2) ;
    int runt1 = at->getRecordValue("RUNT",1) ;
    int runt2 = at->getRecordValue("RUNT",2) ;
    
    char start[100] , stop[100] ;
    sprintf ( start , txt("t_abi_start") ,
                        rund1 >> 16 ,
                        ( rund1 >> 8 ) & 255 ,
                        rund1 & 255 ,
                        ( runt1 >> 24 ) & 255 ,
                        ( runt1 >> 16 ) & 255 ,
                        ( runt1 >>  8 ) & 255 ) ;
    sprintf ( stop , txt("t_abi_stop") ,
                        rund2 >> 16 ,
                        ( rund2 >> 8 ) & 255 ,
                        rund2 & 255 ,
                        ( runt2 >> 24 ) & 255 ,
                        ( runt2 >> 16 ) & 255 ,
                        ( runt2 >>  8 ) & 255 ) ;
    
    int a ;
    char u[256] ;
    string bases ;
    for ( a = 0 ; a < 256 ; a++ ) u[a] = 0 ;
    for ( a = 0 ; a < abi->s.length() ; a++ ) u[abi->s[a]]++ ;
    for ( a = 0 ; a < 256 ; a++ )
       {
       if ( u[a] > 0 )
          {
          if ( bases != "" ) bases += ";  " ;
          sprintf ( t , "%c: %d" , (char) a , u[a] ) ;
          bases += t ;
          }
       }

    string r ;
    r += bases + "\n" ;
    r += txt("t_abi_sample") + smpl + "\n" ;
    sprintf ( t , txt("t_abi_lane") , lane ) ; r += t ;
    r += start ;
    r += stop ;
    if ( comb != "" ) r += txt("t_abi_comb") + comb + "\n" ;
    r += txt("t_abi_gel") + geln + "\n" ;
    r += txt("t_abi_file") + gelp + "\n" ;
    r += txt("t_abi_primer_mobility_correction") + pdmf ;
    
    return r ;
    }
    
void TABIviewer::showStat ()
    {
    string r = getStat () ;
    stat->SetValue ( r.c_str() ) ;
    }

void TABIviewer::showSequence ()
    {
    // Cleaning up
    while ( sc->seq.size() )
        {
        delete sc->seq[sc->seq.size()-1] ;
        sc->seq.pop_back () ;
        }
        
    sc->blankline = 6 ;

    // Display
    SeqABI *d = new SeqABI ( sc ) ;
    d->vec = vec ;
    d->initFromFile ( filename ) ;
    d->takesMouseActions = true ;
    sc->seq.push_back ( d ) ;
    
    sc->arrange () ;
    sc->Refresh () ;    
    showStat() ;
    }
    
void TABIviewer::OnEditMode(wxCommandEvent& event)
    {
    wxMenuBar *mb = GetMenuBar () ;
    wxMenuItem *mi = mb->FindItem ( MDI_EDIT_MODE ) ;
    wxString s ;
    if ( mi->IsChecked() )
        {
        sc->setEditMode ( true ) ;
        sc->findID("ABI")->s += " " ;
        vec->sequence += " " ;
        sc->arrange () ;
        if ( sc->_from == -1 ) sc->mark ( "ABI" , 1 , 1 , 2 ) ;
        else sc->mark ( "ABI" , sc->_from , sc->_from , 2 ) ;
        sc->SetFocus() ;
        }
    else
        {
        sc->setEditMode ( false ) ;
        sc->mark ( "ABI" , -1 , -1 ) ;
        vec->sequence.erase ( vec->sequence.length()-1 , 1 ) ;
        sc->findID("ABI")->s.erase ( sc->findID("ABI")->s.length()-1 , 1 ) ;
        sc->arrange () ;
        Refresh () ;
        }
    }

void TABIviewer::OnEditName(wxCommandEvent& event)
    {
    wxString nn = wxGetTextFromUser ( txt("t_edit_aa_name_txt") ,
                                      txt("t_edit_aa_name") , 
                                      vec->name.c_str() ) ;
    if ( nn == "" ) return ;
    if ( nn == vec->name.c_str() ) return ;
    
    vec->undo.start ( txt("u_title_change") ) ;
    vec->name = nn.c_str() ;
    myapp()->frame->mainTree->SetItemText ( inMainTree , nn ) ;
    vec->undo.stop() ;
    }
    
void TABIviewer::OnMarkAll(wxCommandEvent& event)
    {
    sc->mark ( "ABI" , 1 , vec->sequence.length() ) ;
    }
    
void TABIviewer::OnFileSave(wxCommandEvent& event)
    {
    TManageDatabaseDialog dbd ( this , txt("t_store") , ACTION_MODE_SAVE , vec ) ;
    dbd.ShowModal () ;
    }
    
void TABIviewer::OnExport(wxCommandEvent& event)
    {
    wxMessageDialog md ( this , "Not implemented ... yet!" ) ;
    md.ShowModal() ;
    }
    
void TABIviewer::OnFind(wxCommandEvent& event)
    {
    FindSequenceDialog fsd ( this , txt("t_find") ) ;
    fsd.allowed_chars = sc->edit_valid ;
    fsd.ShowModal () ;
    }
    
void TABIviewer::OnCopyToNew(wxCommandEvent& event)
    {
    TVector *nv = new TVector ;
    string s ;
    if ( sc->_from == -1 ) s = vec->sequence ; // All of it
    else s = sc->getSelection() ;
    nv->name = vec->name ;
    nv->sequence = s ;
    nv->desc = txt("t_abi_original") + getStat() ;
    myapp()->frame->newFromVector ( nv ) ;
    }

void TABIviewer::OnCopy(wxCommandEvent& event)
    {
    sc->OnCopy ( event ) ;
    }

void TABIviewer::OnInvCompl(wxCommandEvent& event)
    {
    bool state = inv_compl->GetValue() ;
    SeqABI *abi = (SeqABI*) sc->seq[0] ;
    abi->setInvCompl ( state ) ;
    sc->arrange () ;
    sc->SilentRefresh () ;
    }
    
void TABIviewer::OnSeqPrint(wxCommandEvent& event)
    {
    sc->OnPrint ( event ) ;    
    }
    
