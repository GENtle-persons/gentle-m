/** \file
	\brief Contains the TABIviewer class members
*/
#include "AminoAcids.h"

BEGIN_EVENT_TABLE(TABIviewer, MyChildBase)
    EVT_MENU(MDI_EDIT_MODE, TABIviewer::OnEditMode)
    EVT_MENU(MDI_MARK_ALL, TABIviewer::OnMarkAll)
    EVT_MENU(MDI_FILE_SAVE, TABIviewer::OnFileSave)
    EVT_MENU(MDI_EXPORT, ChildBase::OnExport)
    EVT_MENU(MDI_FIND, TABIviewer::OnFind)
    EVT_MENU(MDI_COPY, TABIviewer::OnCopy)
    EVT_MENU(AMINOACIDS_EDIT_NAME, TABIviewer::OnEditName)
    EVT_MENU(MDI_COPY_TO_NEW,TABIviewer::OnCopyToNew)
    EVT_MENU(SEQ_PRINT, TABIviewer::OnSeqPrint)
    EVT_MENU(MDI_SPEAK, TABIviewer::OnSpeak)

    EVT_CHECKBOX(ABI_HELPLINES, TABIviewer::OnHelplines)
    EVT_CHECKBOX(ABI_INV_COMP, TABIviewer::OnInvCompl)
    EVT_SPINCTRL(ABI_WIDTH, TABIviewer::OnSpinWidth)
    EVT_SPINCTRL(ABI_HEIGHT, TABIviewer::OnSpinHeight)
    EVT_COMMAND_SCROLL(ABI_SLIDER, TABIviewer::OnZoom)
    EVT_CHECKBOX(ALIGN_HORIZ, TABIviewer::OnHorizontal)

    EVT_SET_FOCUS(ChildBase::OnFocus)
    EVT_CLOSE(ChildBase::OnClose)

    // Dummies
    EVT_MENU(MDI_TOGGLE_FEATURES,TABIviewer::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,TABIviewer::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,TABIviewer::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,TABIviewer::OnDummy)
    EVT_MENU(MDI_ORFS,TABIviewer::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,TABIviewer::OnDummy)
    EVT_MENU(MDI_UNDO,TABIviewer::OnDummy)
    EVT_MENU(MDI_CUT,TABIviewer::OnDummy)
    EVT_MENU(MDI_PASTE,TABIviewer::OnDummy)
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

TABIviewer::TABIviewer(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    vec = new TVector ( this ) ;
    def = _T("ABIviewer") ;
    vec->setName ( title ) ;
    from = -1 ;
    vec->undo.clear () ;
    stat = NULL ;
    }
    
TABIviewer::~TABIviewer ()
    {
    if ( vec ) delete vec ;
    if ( stat ) delete stat ;
    }
    
void TABIviewer::OnHorizontal(wxCommandEvent& event)
    {
    if ( sc->charheight == 0 )
        {
        return ;
        }    
    if ( sc->isHorizontal() )
        {
        f_height->SetValue ( oldh ) ;
        }
    else
        {
        oldh = f_height->GetValue () ;
        wxSize s = sc->MyGetClientSize()  ;
        myass ( false , wxString::Format ( _T("%d %d") , s.GetHeight() , sc->charheight ) ) ;
        int newh = ( s.GetHeight() / sc->charheight - 2 ) / 2 ;
        f_height->SetValue ( newh ) ;
        }        
    sc->toggleHorizontal () ;
    myapp()->frame->LS->setOption ( _T("SHOWABIHORIZONTAL") , sc->isHorizontal() ) ;
    spinHeight() ;
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
    
void TABIviewer::spinHeight()
    {
    int value = f_height->GetValue() ;
    sc->blankline = value + 1 ;
    sc->arrange () ;
    sc->SilentRefresh () ;    
    sc->SetFocus () ;
    }    
    
void TABIviewer::OnSpinHeight(wxSpinEvent& event)
    {
    spinHeight() ;
    }

void TABIviewer::initme ()
    {

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
    edit_menu->Append(MDI_FIND, txt("m_find") );
//    edit_menu->AppendSeparator();
    edit_menu->Append(MDI_COPY_TO_NEW, txt("m_copy_to_new") );
//    edit_menu->Append(MDI_AS_NEW_FEATURE, txt("m_as_new_feature") );
//    edit_menu->Append(MDI_EXTRACT_AA, txt("m_extract_aa") );

    wxMenu *view_menu = new wxMenu;
    view_menu->Append(MDI_EDIT_MODE, txt("m_edit_mode") , _T("") , true );
    view_menu->Append(MDI_SPEAK, txt("m_speak") , _T("") );

//    wxMenu *action_menu = new wxMenu;
//    action_menu->Append(AM_SETUP, "SETUP" );

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(edit_menu, txt("m_edit") );
    menu_bar->Append(view_menu, txt("m_view") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );
    SetMenuBar(menu_bar);

    wxCheckBox *horiz = NULL ;

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
    toolBar->AddTool( SEQ_PRINT, 
                myapp()->frame->bitmaps[16],
                txt("m_print_sequence") ) ;
    toolBar->AddSeparator() ;
//    toolBar->AddTool( MDI_CUT, myapp()->frame->bitmaps[4] ) ;
    toolBar->AddTool( MDI_COPY, myapp()->frame->bitmaps[5] ) ;
//    toolBar->AddTool( MDI_PASTE, myapp()->frame->bitmaps[6] ) ;
    toolBar->AddSeparator() ;
    horiz = new wxCheckBox ( toolBar , ALIGN_HORIZ , txt("t_horizontal") ) ;
    toolBar->AddControl ( horiz ) ;
    myapp()->frame->addDefaultTools ( toolBar ) ;
    toolBar->Realize() ;
    toolbar = toolBar ;

    // Sequence Canvas
    sc = new SequenceCanvas ( this , wxDefaultPosition , wxDefaultSize ) ;
    sc->blankline = 0 ;
    sc->edit_id = _T("ABI") ;
    sc->edit_valid = _T("ATGCN") ;
    sc->forceOverwrite ( true ) ;
    sc->child = this ;

    Maximize ( true ) ;

    // Toys
    aidLines = new wxCheckBox ( this , ABI_HELPLINES , txt("t_aid_lines") );
    inv_compl = new wxCheckBox ( this , ABI_INV_COMP , txt("t_abi_inv_comp") );
    f_height = new wxSpinCtrl ( this , ABI_HEIGHT , _T("5") );
    f_width = new wxSpinCtrl ( this , ABI_WIDTH , _T("2") );
    f_height->SetRange ( 1 , 50 ) ;
    f_width->SetRange ( 1 , 9 ) ;
    aidLines->SetValue ( true ) ;

    
    slider = new wxSlider ( this , ABI_SLIDER , 1 , 1 , 50 , 
                             wxDefaultPosition ,
#ifdef __WXMSW
									  wxDefaultSize,
#else
                             wxSize ( 100 , 20 ) ,
#endif
                             wxSL_HORIZONTAL ) ;

    // Statistics box
    stat = new wxTextCtrl ( this ,
                            -1 ,
                            _T("Test") ,
                            wxDefaultPosition ,
                            wxDefaultSize ,
                            wxTE_MULTILINE | wxTE_READONLY ) ;
    stat->SetFont ( *MYFONT ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ) ;

    wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h4 = new wxBoxSizer ( wxHORIZONTAL ) ;
    
    h1->Add ( aidLines , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;
    h1->Add ( inv_compl , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;
    h2->Add ( f_height , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;
    h2->Add ( new wxStaticText ( this , -1 , txt("t_scale_height") ) , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;
    h3->Add ( f_width , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;
    h3->Add ( new wxStaticText ( this , -1 , txt("t_scale_width") ) , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;    
    h4->Add ( slider , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;
    h4->Add ( new wxStaticText ( this , -1 , txt("t_zoom") ) , 0 , wxEXPAND|wxLEFT|wxRIGHT , 5 ) ;

	v1->Add ( h1 , 1 ) ;
	v1->Add ( h2 , 1 ) ;
	v1->Add ( h3 , 1 ) ;
	v1->Add ( h4 , 1 ) ;

    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
	h0->Add ( v1 , 1 , wxTOP|wxBOTTOM|wxEXPAND , 5 ) ;
	h0->Add ( stat , 4 , wxEXPAND ) ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( toolbar , 0 , wxBOTTOM , 5 ) ;
    v0->Add ( h0 , 0 , wxBOTTOM|wxEXPAND , 5 ) ;
    v0->Add ( sc , 1 , wxEXPAND ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;


    showSequence () ;
    showStat () ;
    myapp()->frame->setChild ( this ) ;    
    sc->SetFocus() ;
    }
    
wxString TABIviewer::getName ()
    {
    return vec->getName() ;
    }

wxString TABIviewer::getStat ()
    {
    SeqABI *abi = (SeqABI*) sc->seq[0] ;
    ABItype *at = abi->at ;
    wxString comb = at->getRecordPascalString ( _T("CMBF") , 1 ) ;
    wxString pdmf = at->getRecordPascalString ( _T("PDMF") , 1 ) ;
    wxString smpl = at->getRecordPascalString ( _T("SMPL") , 1 ) ;
    wxString geln = at->getRecordPascalString ( _T("GELN") , 1 ) ;
    wxString gelp = at->getRecordPascalString ( _T("GELP") , 1 ) ;
    
    int lane  = at->getRecordValue ( _T("LANE") , 1 ) >> 16 ;
    int rund1 = at->getRecordValue ( _T("RUND") , 1 ) ;
    int rund2 = at->getRecordValue ( _T("RUND") , 2 ) ;
    int runt1 = at->getRecordValue ( _T("RUNT") , 1 ) ;
    int runt2 = at->getRecordValue ( _T("RUNT") , 2 ) ;
    
    wxString start , stop ;
    start = wxString::Format ( txt("t_abi_start") ,
                        rund1 >> 16 ,
                        ( rund1 >> 8 ) & 255 ,
                        rund1 & 255 ,
                        ( runt1 >> 24 ) & 255 ,
                        ( runt1 >> 16 ) & 255 ,
                        ( runt1 >>  8 ) & 255 ) ;
    stop = wxString::Format ( txt("t_abi_stop") ,
                        rund2 >> 16 ,
                        ( rund2 >> 8 ) & 255 ,
                        rund2 & 255 ,
                        ( runt2 >> 24 ) & 255 ,
                        ( runt2 >> 16 ) & 255 ,
                        ( runt2 >>  8 ) & 255 ) ;
    
    unsigned int a ;
    char u[256] ;
    wxString bases ;
    for ( a = 0 ; a < 256 ; a++ ) u[a] = 0 ;
    for ( a = 0 ; a < abi->s.length() ; a++ ) u[abi->s.GetChar(a)]++ ;
    for ( a = 0 ; a < 256 ; a++ )
       {
       if ( u[a] > 0 )
          {
          if ( !bases.IsEmpty() ) bases += _T(";  ") ;
          bases += wxString::Format ( _T("%c: %d") , (char) a , u[a] ) ;
          }
       }

    wxString r ;
    r += bases + _T("\n") ;
    r += txt("t_abi_sample") + smpl + _T("\n") ;
    r += wxString::Format ( txt("t_abi_lane") , lane ) ;
    r += start ;
    r += stop ;
    if ( !comb.IsEmpty() ) r += txt("t_abi_comb") + comb + _T("\n") ;
    r += txt("t_abi_gel") + geln + _T("\n") ;
    r += txt("t_abi_file") + gelp + _T("\n") ;
    r += txt("t_abi_primer_mobility_correction") + pdmf ;
    
    return r ;
    }
    
void TABIviewer::showStat ()
    {
    wxString r = getStat () ;
    stat->SetValue ( r ) ;
    }

void TABIviewer::showSequence ()
    {
    // Cleaning up
    CLEAR_DELETE ( sc->seq ) ;
        
    sc->blankline = 6 ;

    // Display
    SeqABI *d = new SeqABI ( sc ) ;
    d->vec = vec ;
    d->initFromFile ( filename ) ;
    d->takesMouseActions = true ;
    sc->seq.Add ( d ) ;
    
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
        sc->findID ( _T("ABI") )->s += _T(" ") ;
        vec->addToSequence ( _T(" ") ) ;
        sc->arrange () ;
        if ( sc->markedFrom() == -1 ) sc->mark ( _T("ABI") , 1 , 1 , 2 ) ;
        else sc->mark ( _T("ABI") , sc->markedFrom() , sc->markedFrom() , 2 ) ;
        sc->SetFocus() ;
        }
    else
        {
        sc->setEditMode ( false ) ;
        sc->unmark() ;
//        sc->mark ( "ABI" , -1 , -1 ) ;
        vec->eraseSequence ( vec->getSequenceLength()-1 , 1 ) ;
        sc->findID ( _T("ABI") )->s.erase ( sc->findID ( _T("ABI") )->s.length()-1 , 1 ) ;
        sc->arrange () ;
        Refresh () ;
        }
    }

void TABIviewer::OnEditName(wxCommandEvent& event)
    {
    wxString nn = wxGetTextFromUser ( txt("t_edit_aa_name_txt") ,
                                      txt("t_edit_aa_name") , 
                                      vec->getName() ) ;
    if ( nn.IsEmpty() ) return ;
    if ( nn == vec->getName() ) return ;
    
    vec->undo.start ( txt("u_title_change") ) ;
    vec->setName ( nn ) ;
    myapp()->frame->mainTree->SetItemText ( inMainTree , nn ) ;
    vec->undo.stop() ;
    }
    
void TABIviewer::OnMarkAll(wxCommandEvent& event)
    {
    if ( sc->getEditMode() ) return ;
    sc->mark ( _T("ABI") , 1 , vec->getSequenceLength() ) ;
    }
    
void TABIviewer::OnFileSave(wxCommandEvent& event)
    {
    TManageDatabaseDialog dbd ( this , txt("t_store") , ACTION_MODE_SAVE , vec ) ;
    dbd.ShowModal () ;
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
    wxString s ;
    bool unmark = false ;
    if ( sc->markedFrom() == -1 ) // All of it
    	{
	    OnMarkAll ( event ) ;
	    unmark = true ;
    	}    
    s = sc->getSelection() ;
    if ( unmark ) sc->unmark() ;
    nv->setName ( vec->getName() ) ;
    nv->setSequence ( s ) ;
    nv->setDescription ( txt("t_abi_original") + getStat() ) ;
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
    
void TABIviewer::OnSpeak(wxCommandEvent& event)
	{
    wxString s = sc->getSelection() ;
    if ( s.IsEmpty() ) s = vec->getSequence() ; // Nothing selected, read whole sequence
	TSpeakDialog sd ( this , txt("t_speak") , s ) ;
	sd.ShowModal() ;
	}
