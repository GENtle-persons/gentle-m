/** \file
	\brief Contains the TAminoAcids module class methods
*/
#include "AminoAcids.h"

BEGIN_EVENT_TABLE(TAminoAcids, MyChildBase)
    EVT_MENU(MDI_EDIT_MODE, TAminoAcids::OnEditMode)
    EVT_MENU(MDI_MARK_ALL, TAminoAcids::OnMarkAll)
    EVT_MENU(MDI_FILE_SAVE, TAminoAcids::OnFileSave)
    EVT_MENU(MDI_EXPORT, ChildBase::OnExport)
    EVT_MENU(MDI_FIND, TAminoAcids::OnFind)
    EVT_MENU(MDI_CUT, TAminoAcids::OnCut)
    EVT_MENU(MDI_COPY, TAminoAcids::OnCopy)
    EVT_MENU(MDI_PASTE,TAminoAcids::OnPaste)
    EVT_MENU(AMINOACIDS_EDIT_NAME, TAminoAcids::OnEditName)
    EVT_MENU(SEQ_PRINT, TAminoAcids::OnPrint)
    EVT_MENU(MDI_AS_NEW_FEATURE, TAminoAcids::OnAsNewFeature)
    EVT_MENU(AMINOACIDS_BLAST_AA, TAminoAcids::OnBlastAA)
    EVT_MENU(AMINOACIDS_PHOTOMETER, TAminoAcids::OnPhotometer)
    EVT_MENU(SEQ_AA_BACKTRANSLATE,TAminoAcids::OnBacktranslate)
    EVT_MENU(MDI_AA_IPC,TAminoAcids::OnIPC)

    EVT_MENU(MDI_UNDO, TAminoAcids::Undo)
    EVT_MENU(MDI_REDO, TAminoAcids::Redo)
    EVT_MENU(MDI_SPEAK, TAminoAcids::OnSpeak)
    EVT_MENU(AA_PROTEOLYSIS, TAminoAcids::OnProteolysis)

    EVT_CHOICE(AA_FONTSIZE,TAminoAcids::OnFontsize)
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
    EVT_MENU(AA_NONE,ChildBase::OnDummy)
    EVT_MENU(AA_KNOWN, ChildBase::OnDummy)
    EVT_MENU(AA_ALL, ChildBase::OnDummy)
    EVT_MENU(AA_THREE, ChildBase::OnDummy)
    EVT_MENU(AA_ONE, ChildBase::OnDummy)
    EVT_MENU(MDI_AUTO_ANNOTATE, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_1, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_2, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_3, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M1, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M2, ChildBase::OnDummy)
    EVT_MENU(AA_THREE_M3, ChildBase::OnDummy)
    
END_EVENT_TABLE()

TAminoAcids::TAminoAcids(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    vec = new TVector ( this ) ;
    def = _T("AminoAcids") ;
    vec->setName ( title ) ;
    from = -1 ;
    stat = NULL ;
    pc = NULL ;
    sc2 = NULL ;
    lastLBsel = _T("") ;
    miniDisplayOptions = MINI_DISPLAY_CONDENSED ;
    allow_print = allow_save = allow_find = true ;
    }

TAminoAcids::~TAminoAcids ()
    {
    if ( curDisplay ) delete curDisplay ;
    if ( vec ) delete vec ;
    }

void TAminoAcids::OnFontsize ( wxCommandEvent& event )
    {
	long l ;
	wxString s = fontsize->GetStringSelection() ;
	s.ToLong ( &l ) ;
	sc->set_font_size ( (int) l ) ;
	showSequence () ;
	}

void TAminoAcids::OnIPC ( wxCommandEvent& event )
    {    
    TIPCDialog ipcd ( this , txt("t_aa_ipc") , vec->getSequenceLength() ) ;
    if ( wxID_OK != ipcd.ShowModal() ) return ;
        
    wxString filename = wxFileName::CreateTempFileName ( _T("ipc") ) ;

   	wxBeginBusyCursor() ;
   	TIPC ipc ;
//   	wxStartTimer () ;
		int r = ipc.ipc_main2 ( filename.mb_str() , vec->getSequence().mb_str() , 3000 ) ;
   	wxEndBusyCursor() ;
   	if ( r != 0 ) { return ; } // ERROR
//   	wxMessageBox ( wxString::Format ( "%d" , wxGetElapsedTime() ) ) ;
   	
   	TGraph *g = myapp()->frame->RunGraph() ;
   	g->gd->setupIPCfile ( filename ) ;
   	g->gd->AutoScale () ;
   	g->gd->UpdateDisplay () ;
   	
   	wxRemoveFile ( filename ) ;
   	wxRemoveFile ( filename + _T(".gnu") ) ;
    }
    
void TAminoAcids::OnBacktranslate ( wxCommandEvent& event )
    {
    sc->OnBacktranslate ( event ) ;
    }    

void TAminoAcids::initme ()
    {
    mylog ( "TAminoAcids::initme" , "1" ) ;
    vec->undo.clear() ;
    mylog ( "TAminoAcids::initme" , "2" ) ;
    

    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu ( FILE_MENU_SAVE|FILE_MENU_EXPORT ) ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu ( false ) ;
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
//    edit_menu->AppendSeparator();
//    edit_menu->Append(MDI_COPY_TO_NEW, txt("m_copy_to_new") );
//    edit_menu->Append(MDI_AS_NEW_FEATURE, txt("m_as_new_feature") );
//    edit_menu->Append(MDI_EXTRACT_AA, txt("m_extract_aa") );
    edit_menu->Append(AMINOACIDS_BLAST_AA, txt("m_blast_aa") ) ;
    edit_menu->Append(AMINOACIDS_PHOTOMETER, txt("m_aa_photometer") ) ;
    edit_menu->Append(SEQ_AA_BACKTRANSLATE, txt("m_aa_backtranslate") ) ;
    edit_menu->Append(MDI_AA_IPC, txt("m_aa_ipc") ) ;
    edit_menu->Append(AA_PROTEOLYSIS, txt("m_aa_proteolysis") ) ;

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
    mylog ( "TAminoAcids::initme" , "3" ) ;

    // Sequence Canvas
    sc = new SequenceCanvas ( this , wxPoint ( 0 , 0 ) , wxSize ( 100 , 100 ) ) ;
    cSequence = sc ; // For compatability with PlasmidCanvas
    sc->blankline = 1 ;
    sc->child = this ;
    sc->edit_id = _T("AA") ;
    sc->edit_valid = _T("ACDEFGHIKLMNPQRSTVWY|") ;
    mylog ( "TAminoAcids::initme" , "4" ) ;

    int w , h ;
//    GetParent()->GetClientSize ( &w , &h ) ;
    myapp()->frame->GetClientSize ( &w , &h ) ;
    h = 100 ;
    
    Hide();
    
    mylog ( "TAminoAcids::initme" , "5" ) ;
    v0 = new wxBoxSizer ( wxVERTICAL ) ;
    
    h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    v1 = new wxBoxSizer ( wxVERTICAL ) ;
    
#ifdef __WXMSW__
	int lbwidth = 130 ;
#else
	int lbwidth = -1 ;
#endif
    
    lb = new wxListBox ( this ,
			 AA_LB ,
			 wxDefaultPosition ,
			 wxSize ( lbwidth , 170 ) ) ;
                            
    lb->Append ( txt("t_data") ) ;
    lb->Append ( txt("desc") ) ;
    lb->Append ( txt("t_schema") ) ;
    lb->Append ( txt("t_aa_plot_mw") ) ;
    lb->Append ( txt("t_aa_plot_pi") ) ;
    lb->Append ( txt("t_aa_plot_hp") ) ;
    lb->Append ( txt("t_chou_fasman") ) ;
    lb->Append ( txt("t_ncoils") ) ;

    mylog ( "TAminoAcids::initme" , "6" ) ;

    toolbar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    myapp()->frame->InitToolBar(toolbar);
    toolbar->Reparent ( this ) ;
	myapp()->frame->addTool ( toolbar , MDI_TEXT_IMPORT ) ;
	myapp()->frame->addTool ( toolbar , MDI_FILE_OPEN ) ;
    myapp()->frame->addTool ( toolbar , MDI_FILE_SAVE ) ;
    myapp()->frame->addTool ( toolbar , SEQ_PRINT ) ;
    myapp()->frame->addCCPFTools ( toolbar , true ) ;
    wxCheckBox *mycb = new wxCheckBox ( toolbar , ALIGN_HORIZ , txt("t_horizontal") ) ;
    toolbar->AddControl ( mycb ) ;
    inlinePlot = new wxChoice ( toolbar , AA_IP ) ;
    toolbar->AddControl ( inlinePlot ) ;
    fontsize = myapp()->frame->AddFontsizeTool ( toolbar , AA_FONTSIZE ) ;
    myapp()->frame->addDefaultTools ( toolbar ) ;
    toolbar->Realize() ;
    inlinePlot->Append ( txt("t_no_plot") ) ;
    inlinePlot->Append ( txt("t_chou_fasman") ) ;
    inlinePlot->Append ( txt("t_ncoils") ) ;
    inlinePlot->Append ( txt("t_aa_structure") ) ;
    inlinePlot->SetStringSelection ( txt("t_no_plot") ) ;

    mylog ( "TAminoAcids::initme" , "7" ) ;
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
    mylog ( "TAminoAcids::initme" , "8" ) ;
    
    mylog ( "TAminoAcids::initme" , "9" ) ;

    showSequence () ;
    mylog ( "TAminoAcids::initme" , "9a" ) ;
    showStat () ;
    mylog ( "TAminoAcids::initme" , "9b" ) ;
    updateUndoMenu () ;
    mylog ( "TAminoAcids::initme" , "10" ) ;
    handleListBox ( txt("t_data") ) ;
    if ( myapp()->frame->isLocked() ) return ;
    mylog ( "TAminoAcids::initme" , "11a" ) ;
    Activate () ;
    sc->SetFocus() ;
    mylog ( "TAminoAcids::initme" , "11b" ) ;
    }
    
const wxString TAminoAcids::getName () const
    {
    return vec->getName() ;
    }

void TAminoAcids::showStat ()
    {
    if ( !curDisplay ) return ;
    if ( curDisplay == desc ) desc->SetValue ( vec->getDescription() ) ;
    if ( curDisplay != stat ) return ;
    wxString t ;
    int noaa = 0 , piaa = 0 ;
    float pI = 0 ;
    float ext = 0 , abs = 0 ;
    float ext2 = 0 , abs2 = 0 ;
    int num[256] ;
    int a , b ;
    float mW = 18 ; // OH and H at the ends
    
    // Key values
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
    ext = num[(unsigned int)'W']*5500 + num[(unsigned int)'Y']*1490 + (num[(unsigned int)'C']*125)/2 ;
    ext2 = num[(unsigned int)'W']*5500 + num[(unsigned int)'Y']*1490  ;
    if ( mW > 0 )
       {
       abs = ext / mW ;
       abs2 = ext2 / mW ;
       }
    t = txt("aa_info") ;
    t += txt("aa_info1a") + txt("aa_info2a") + _T("\n") + txt("aa_info1a") + txt("aa_info2b") ;
    t.Replace ( _T("%f") , _T("%9.2f") ) ;
    t = wxString::Format ( t , noaa , mW , pI , ext , abs , ext2 , abs2 ) ;
    
    // Amino acid count
    wxString t2 ;
    b = 0 ;
    for ( a = 'A' ; a < 'Z' ; a++ )
    	{
	    if ( num[a] == 0 ) continue ;
	    b++ ;
	    TAAProp p = vec->getAAprop ( a ) ;
	    t2 += a ;
	    t2 += _T("/") ;
	    t2 += p.tla ;
	    t2 += wxString::Format ( _T("%4d") , num[a] ) ;
	    if ( b % 4 == 0 && a+1 != 'Z' ) t2 += _T("\n") ;
	    else if ( a+1 != 'Z' ) t2 += _T("      ") ;
    	}
	
    t += _T("\n") + wxString::Format ( txt("aa_info2") , num[(unsigned int)'D']+num[(unsigned int)'E'] , num[(unsigned int)'R']+num[(unsigned int)'K'] ) ;
    t += _T("\n\n") + t2 + _T("\n\n") ;
   	
    // Atomic composition & hydrophobicity
    int carbon = 0 , hydrogen = 2 , nitrogen = 0 , oxygen = 1 , sulfur = 0 ;
    float hydropathicity = 0 ;
    for ( a = 0 ; a < vec->getSequenceLength() ; a++ )
        {
        unsigned char c = (unsigned char)vec->getSequenceChar ( a ) ;
        TAAProp p = vec->getAAprop ( c ) ;
        carbon += p.carbon ;
        hydrogen += p.hydrogen ;
        nitrogen += p.nitrogen ;
        oxygen += p.oxygen ;
        sulfur += p.sulfur ;
        hydropathicity += p.hp_kd ;
        }
    int atom_total = carbon + hydrogen + nitrogen + oxygen + sulfur ;   	
   	t += wxString::Format ( txt("aa_info3") , atom_total , carbon , hydrogen , nitrogen , oxygen , sulfur ) ;
   	
   	// Half-life
   	if ( vec->getSequenceLength() > 0 )
   		{
	    unsigned char c = (unsigned char)vec->getSequenceChar ( 0 ) ;
   		TAAProp p = vec->getAAprop ( c ) ;
   		wxString hl_mammal = p.get_halflife_text ( p.hl_mammal ) ;
   		wxString hl_yeast = p.get_halflife_text ( p.hl_yeast ) ;
   		wxString hl_ecoli = p.get_halflife_text ( p.hl_ecoli ) ;
   		t += _T("\n\n") + wxString::Format ( txt("aa_info4") ,
     						hl_mammal.c_str() , 
     						hl_yeast.c_str() ,
     						hl_ecoli.c_str() ) ;

        hydropathicity /= vec->getSequenceLength() ;
        t += _T("\n") + wxString::Format ( txt("aa_info5") , hydropathicity ) ;
   		}  		
    
    stat->SetValue ( t ) ;
    stat->SetFont ( *MYFONT ( MYFONTSIZE , wxFONTFAMILY_MODERN , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;
    }

void TAminoAcids::showSequence ()
    {
    mylog ( "TAminoAcids::showSequence" , "1" ) ;
    // Cleaning up
    CLEAR_DELETE ( sc->seq ) ;
    mylog ( "TAminoAcids::showSequence" , "2" ) ;

    // Display
    SeqFeature *seqF = new SeqFeature ( sc ) ;
    sc->seq.Add ( seqF ) ;
    seqF->initFromTVector ( vec ) ;
    mylog ( "TAminoAcids::showSequence" , "3" ) ;

    SeqAA *d = new SeqAA ( sc ) ;
    sc->seq.Add ( d ) ;
    d->primaryMode = true ;
    d->takesMouseActions = true ;
    d->initFromString ( vec->getSequence() ) ;
//    d->fixOffsets ( vec ) ;
    mylog ( "TAminoAcids::showSequence" , "4" ) ;

    // Plot demo
	if ( inlinePlot->GetStringSelection() == txt("t_chou_fasman") )
		{
		mylog ( "TAminoAcids::showSequence" , "CHOU_FASMAN 1" ) ;
		SeqPlot *seqP = new SeqPlot ( sc ) ;
		mylog ( "TAminoAcids::showSequence" , "CHOU_FASMAN 2" ) ;
		sc->seq.Add ( seqP ) ;
		mylog ( "TAminoAcids::showSequence" , "CHOU_FASMAN 3" ) ;
		seqP->initFromTVector ( vec ) ;
		mylog ( "TAminoAcids::showSequence" , "CHOU_FASMAN 4" ) ;
		seqP->setLines ( 1 ) ;
		mylog ( "TAminoAcids::showSequence" , "CHOU_FASMAN 5" ) ;
		seqP->useChouFasman() ;
		mylog ( "TAminoAcids::showSequence" , "CHOU_FASMAN 6" ) ;
		seqP->takesMouseActions = false ;
		mylog ( "TAminoAcids::showSequence" , "CHOU_FASMAN 7" ) ;
		}
	else if ( inlinePlot->GetStringSelection() == txt("t_ncoils") )
		{
		mylog ( "TAminoAcids::showSequence" , "NCOILS 1" ) ;
		SeqPlot *seqP = new SeqPlot ( sc ) ;
		mylog ( "TAminoAcids::showSequence" , "NCOILS 2" ) ;
		sc->seq.Add ( seqP ) ;
		mylog ( "TAminoAcids::showSequence" , "NCOILS 3" ) ;
		seqP->initFromTVector ( vec ) ;
		mylog ( "TAminoAcids::showSequence" , "NCOILS 4" ) ;
		seqP->setLines ( 1 ) ;
		mylog ( "TAminoAcids::showSequence" , "NCOILS 5" ) ;
		seqP->useNcoils() ;
		mylog ( "TAminoAcids::showSequence" , "NCOILS 6" ) ;
		seqP->takesMouseActions = false ;
		mylog ( "TAminoAcids::showSequence" , "NCOILS 7" ) ;
		}
	else if ( inlinePlot->GetStringSelection() == txt("t_aa_structure") )
		{
		SeqAAstructure *seqS = new SeqAAstructure ( sc , d ) ;
		sc->seq.Add ( seqS ) ;
		}
    mylog ( "TAminoAcids::showSequence" , "5" ) ;
    
    seqF->aaa = d ;
        
    mylog ( "TAminoAcids::showSequence" , "6" ) ;
    sc->arrange () ;
    mylog ( "TAminoAcids::showSequence" , "7" ) ;
    sc->SilentRefresh () ;    
    mylog ( "TAminoAcids::showSequence" , "8" ) ;

    mylog ( "TAminoAcids::showSequence" , "9" ) ;
    }
    
void TAminoAcids::OnEditMode(wxCommandEvent& event)
    {
	if ( sc->getEditMode() )
 		{
        sc->stopEdit() ;
        vec->undo.stop () ;
        }    
	else
 		{
   		vec->undo.start ( txt("u_edit") ) ;
        sc->startEdit ( _T("AA") ) ;
        }    
    }

void TAminoAcids::invokeVectorEditor ( const wxString& what , const int num , const bool forceUpdate )
	{
	vec->undo.start ( txt("u_vec_edit") ) ;
	TVectorEditor ve ( this , txt("t_vector_editor") , vec ) ;
	bool changed = vec->isChanged() ;
	vec->setChanged ( false ) ;
	
	ve.hideEnzym = true ;
	ve.hideEm () ;
	
	if ( what == _T("item") )
		ve.initialViewItem ( num ) ;
	
	ve.ShowModal () ;
	ve.cleanup () ;
	if ( forceUpdate || vec->isChanged() )
		{
		myapp()->frame->mainTree->SetItemText ( inMainTree , getName() ) ;
		showName() ;
		showSequence () ;
		vec->undo.stop() ;
		if ( pc ) pc->Refresh () ;
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
    if ( sc->getEditMode() ) return ;
    sc->mark ( _T("AA") , 1 , vec->getSequenceLength() ) ;
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

void TAminoAcids::OnPaste(wxCommandEvent& event)
    {
    sc->OnPaste ( event ) ;
    }

void TAminoAcids::OnAsNewFeature(wxCommandEvent& event)
    {
//    char t[1000] ;
    int from = sc->markedFrom() ;
    int to = sc->markedTo() ;
    if ( from == -1 ) return ;
    
    vec->undo.start ( txt("u_new_feature") ) ;

    TVectorItem nvi ;
//    sprintf ( t , txt("t_new_item_title") , from , to ) ;
    nvi.name = wxString::Format ( txt("t_new_item_title") , from , to ) ;
    nvi.setDirection ( 1 ) ;
    nvi.setType ( VIT_MISC ) ;
    nvi.from = from ;
    nvi.to = to ;
    vec->setChanged () ;
    vec->items.push_back ( nvi ) ;
    invokeVectorEditor ( _T("item") , vec->items.size()-1 , true ) ;
    vec->undo.stop() ;
    }
    
void TAminoAcids::OnPhotometer(wxCommandEvent& event)
    {
    TCalculator *c = myapp()->frame->RunCalculator () ;
    wxString seq = vec->getSequence() ;
    float mW = 0 ;
    for ( int a = 0 ; a < vec->getSequenceLength() ; a++ )
        mW += vec->getAAmw ( vec->getSequenceChar ( a ) ) ;
    c->prot->SetCellValue ( 2 , 1 , wxString::Format ( _T("%d") , (int)seq.Replace ( _T("W") , _T("") , true ) ) ) ;
    c->prot->SetCellValue ( 3 , 1 , wxString::Format ( _T("%d") , (int)seq.Replace ( _T("Y") , _T("") , true ) ) ) ;
    c->prot->SetCellValue ( 4 , 1 , wxString::Format ( _T("%d") , (int)seq.Replace ( _T("C") , _T("") , true ) ) ) ;
    c->prot->SetCellValue ( 5 , 1 , wxString::Format ( _T("%0.2f") , mW) ) ;
    c->nb->SetSelection ( 2 ) ;
    }
    
void TAminoAcids::OnBlastAA(wxCommandEvent& event)
    {
    wxString seq ;
    if ( sc->markedFrom() < 0 ) seq = vec->getSequence() ;
    else seq = sc->getSelection() ;
    myapp()->frame->blast ( seq , _T("blastp") ) ;
    }

void TAminoAcids::Undo(wxCommandEvent& event)
    {
    if ( !vec->undo.canUndo() ) return ;
    if ( sc->getEditMode() ) return ;
    sc->unmark() ;
    vec->undo.pop() ;
//    initPanels () ;

    showSequence () ;
    showStat () ;
    updateUndoMenu () ;
    sc->SetFocus() ;
    }
    
bool TAminoAcids::HasUndoData ()        
    {
    if ( !vec ) return false ;
    if ( !allow_undo ) return false ;
    wxString lm = vec->undo.getLastMessage() ;
    if ( lm.IsEmpty() ) return false ;
    return true ;
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
        mi->SetItemLabel ( txt("u_no") ) ;
        mi->Enable ( false ) ;
	canUndo = false ;
        }
    else
        {
        mi->Enable ( true ) ;
        mi->SetItemLabel ( lm ) ;
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
	 mylog ( "TAminoAcids::OnHorizontal" , "BEGIN" ) ;
    sc->toggleHorizontal () ;
    showSequence () ;
//    sc->arrange () ;
//    sc->SilentRefresh() ;    
	 mylog ( "TAminoAcids::OnHorizontal" , "END" ) ;
    }
    
void TAminoAcids::OnIP ( wxCommandEvent& event )
    {
    if ( sc->getEditMode() ) sc->stopEdit() ;
    showSequence () ;
    }
    
void TAminoAcids::OnListBox ( wxCommandEvent& event )
    {
    handleListBox ( lb->GetStringSelection() ) ;
    }

void TAminoAcids::handleListBox ( const wxString& t )
	{
	mylog ( "TAminoAcids::handleListBox" , "1" ) ;
	bool update = false ;
	if ( t == lastLBsel ) update = true ;
	else lastLBsel = t ;
	
	if ( !update )
		{
		if ( curDisplay )
			{
			h1->Detach ( curDisplay ) ;
			delete curDisplay ;
			}
		curDisplay = NULL ;
		desc = NULL ;
		stat = NULL ;
		sc2 = NULL ;
		}
	
	mylog ( "TAminoAcids::handleListBox" , wxString::Format ( _T("2 : %s") , t.mb_str() ) ) ;
	if ( t == txt("desc") )
		{
		if ( update ) desc->SetValue ( vec->getDescription() )  ;
		else
			{
			desc = new TURLtext ( this ,
			URLTEXT_DUMMY ,
			_T("") ,
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
			_T("") ,
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
		vec->updateDisplay() ;
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
	t == txt("t_ncoils" ) ||
	t == txt("t_aa_plot_mw" ) ||
	t == txt("t_aa_plot_pi" ) ||
	t == txt("t_aa_plot_hp" ) )
		{
		mylog ( "TAminoAcids::handleListBox" , "2a" ) ;
		if ( !update )
			{
			mylog ( "TAminoAcids::handleListBox" , "2a1" ) ;
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
			mylog ( "TAminoAcids::handleListBox" , "2a2" ) ;
			CLEAR_DELETE ( sc2->seq ) ;
			}
		
		mylog ( "TAminoAcids::handleListBox" , "2b" ) ;
		SeqPlot *seqP = new SeqPlot ( sc2 ) ;
		mylog ( "TAminoAcids::handleListBox" , "2c" ) ;
		sc2->seq.Add ( seqP ) ;
		mylog ( "TAminoAcids::handleListBox" , "2d" ) ;
		seqP->initFromTVector ( vec ) ;
		mylog ( "TAminoAcids::handleListBox" , "2e" ) ;
		seqP->setLines ( 6 ) ;
		mylog ( "TAminoAcids::handleListBox" , "2f" ) ;
		
		if ( t == txt("t_chou_fasman") ) seqP->useChouFasman() ;
		else if ( t == txt("t_ncoils") ) seqP->useNcoils () ;
		else if ( t == txt("t_aa_plot_mw") ) seqP->useMW () ;
		else if ( t == txt("t_aa_plot_pi") ) seqP->usePI () ;
		else if ( t == txt("t_aa_plot_hp") ) seqP->useHP () ;
		
		mylog ( "TAminoAcids::handleListBox" , "2g" ) ;
		if ( update )
			{
			mylog ( "TAminoAcids::handleListBox" , "2g1" ) ;
			sc2->arrange() ;
			mylog ( "TAminoAcids::handleListBox" , "2g2" ) ;
			sc2->SilentRefresh() ;
			}
		mylog ( "TAminoAcids::handleListBox" , "2h" ) ;
		}
	mylog ( "TAminoAcids::handleListBox" , "3" ) ;

	if ( !update )
		{
//        GetToolBar()->Refresh () ;
		h1->Layout() ;
		lb->Refresh () ;
		}
	sc->SetFocus() ;
	mylog ( "TAminoAcids::handleListBox" , "4" ) ;
	}
    
void TAminoAcids::updateToolbar ()
    {
    }    
    
void TAminoAcids::OnSpeak(wxCommandEvent& event)
	{
    wxString s = sc->getSelection() ;
    if ( s.IsEmpty() ) s = vec->getSequence() ; // Nothing selected, read whole sequence
	TSpeakDialog sd ( this , txt("t_speak") , s ) ;
	sd.ShowModal() ;
	}
