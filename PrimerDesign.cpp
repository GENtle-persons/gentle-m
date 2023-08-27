/** \file
	\brief Contains the methods of the TPrimerDesign module class
*/
#include "PrimerDesign.h"
#include "PCR_Troubleshoot.h"

BEGIN_EVENT_TABLE(TPrimerDesign, MyChildBase)
    EVT_MENU(AA_ALL, TPrimerDesign::OnAA_all)
    EVT_MENU(AA_THREE, TPrimerDesign::OnAA_three)
    EVT_MENU(AA_ONE, TPrimerDesign::OnAA_one)
    EVT_MENU(AA_THREE_1, TPrimerDesign::OnAA_three_1)
    EVT_MENU(AA_THREE_2, TPrimerDesign::OnAA_three_2)
    EVT_MENU(AA_THREE_3, TPrimerDesign::OnAA_three_3)
    EVT_MENU(AA_THREE_M1, TPrimerDesign::OnAA_three_M1)
    EVT_MENU(AA_THREE_M2, TPrimerDesign::OnAA_three_M2)
    EVT_MENU(AA_THREE_M3, TPrimerDesign::OnAA_three_M3)
    EVT_MENU(AA_KNOWN, TPrimerDesign::OnAA_known)

    EVT_BUTTON(PD_EDIT,TPrimerDesign::OnEditPrimer)
    EVT_BUTTON(PD_DEL,TPrimerDesign::OnDeletePrimer)
    EVT_SPINCTRL(PCR_SPIN, TPrimerDesign::OnSpin)
    EVT_TEXT(PCR_SPIN,TPrimerDesign::OnSpinText)

    EVT_MENU(PD_SILMUT,TPrimerDesign::OnSilmut)
    EVT_MENU(MDI_TOGGLE_FEATURES,TPrimerDesign::OnToggleFeatures)
    EVT_MENU(PD_EXPORT,TPrimerDesign::OnExportPrimer)
    EVT_MENU(PD_IMPORT,TPrimerDesign::OnImportPrimer)
    EVT_MENU(SEQ_PRINT,TPrimerDesign::OnPrint)
    EVT_MENU(SEQ_INSERT_RESTRICTION_SITE_LEFT,TPrimerDesign::OnInsertRestrictionSiteLeft)
    EVT_MENU(SEQ_INSERT_RESTRICTION_SITE_RIGHT,TPrimerDesign::OnInsertRestrictionSiteRight)
    EVT_MENU(PD_TROUBLESHOOT,TPrimerDesign::OnTroubleshoot)

    EVT_LIST_ITEM_ACTIVATED(PD_LC,TPrimerDesign::OnActivatePrimer)
    EVT_LIST_ITEM_SELECTED(PD_LC,TPrimerDesign::OnSelectPrimer)
    EVT_CHOICE(AA_FONTSIZE,TPrimerDesign::OnFontsize)

    EVT_CHECKBOX(ALIGN_HORIZ, TPrimerDesign::OnHorizontal)

    EVT_MENU(MDI_FIND, TPrimerDesign::OnFind)
    EVT_MENU(MDI_COPY, TPrimerDesign::OnCopy)
    EVT_MENU(MDI_PASTE,TPrimerDesign::OnPaste)

    EVT_MENU(MDI_EDIT_MODE, TPrimerDesign::OnEditMode)
    EVT_CLOSE(ChildBase::OnClose)

    // Dummies
    EVT_MENU(AA_NONE, TPrimerDesign::ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_UNDO,ChildBase::OnDummy)
    EVT_MENU(MDI_CUT,ChildBase::OnDummy)
    EVT_MENU(MDI_EXPORT,ChildBase::OnDummy)
//    EVT_MENU(MDI_COPY,ChildBase::OnDummy)
    EVT_MENU(MDI_EDIT_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_MARK_ALL,ChildBase::OnDummy)
    EVT_MENU(MDI_FILE_SAVE,ChildBase::OnDummy)

END_EVENT_TABLE()

TPrimerDesign::TPrimerDesign(wxWindow *parent,
                    wxString title,
                    TVector *_vec,
                    vector <TPrimer> pl ,
                    int _mut )
    : ChildBase(parent, title)
    {
    int a ;
    updating = false ;
    show_features = _vec->getGenomeMode() ? 0 : 1 ;
    spinTextEnabeled = false ;
    mut = _mut ;
    vec = new TVector ;
    inverse_template_vector = new TVector ;
    resulting_sequence_vector = new TVector ;
    vec->setFromVector ( *_vec ) ;
    resulting_sequence_vector->setFromVector ( *vec ) ;

    aa_state = AA_ALL ;
    aa_disp = AA_ONE ;
    for ( a = 0 ; a < vec->items.size() ; a++ )
        if ( vec->items[a].getRF() != 0 ) aa_state = AA_KNOWN ;

    primer = pl ;

    for ( a = 0 ; a < primer.size() ; a++ )
        {
        if ( primer[a].from >= vec->getSequenceLength() )
           {
           primer[a].from -= vec->getSequenceLength() ;
           primer[a].to -= vec->getSequenceLength() ;
           }
        }

    for ( a = 0 ; a < primer.size() ; a++ )
        primer[a].getSequenceFromVector ( vec ) ;

    def = _T("PrimerDesign") ;
    from = -1 ;
    stat = NULL ;
    lastPrimerActivated = -1 ;
    allow_print = true ;
    }

TPrimerDesign::~TPrimerDesign ()
    {
    if ( stat ) delete stat ;
    delete inverse_template_vector ;
    delete resulting_sequence_vector ;
    }

void TPrimerDesign::OnFontsize ( wxCommandEvent& event )
    {
	long l ;
	wxString s = fontsize->GetStringSelection() ;
	s.ToLong ( &l ) ;
	sc->set_font_size ( (int) l ) ;
    showSequence () ;
	}

void TPrimerDesign::guessOptNuc ()
    {
    int nuc = vec->getSequenceLength() ;

    if ( primer.size() == 2 )
        {
        if ( primer[0].overlap ( primer[1] ) ) // Mutagenesis?
           {
           }
        else // Probably just a piece
           {
           if ( primer[0].upper ) nuc = primer[1].from - primer[0].to ;
           else nuc = primer[1].to - primer[0].from ;
           if ( nuc < 0 ) nuc = vec->getSequenceLength() + nuc ;
           nuc++ ;
           }
        }

    spin->SetValue ( wxString::Format ( _T("%d") , nuc ) ) ;
    }

void TPrimerDesign::OnPrint ( wxCommandEvent &ev )
    {
    sc->OnPrint ( ev ) ;
    }

void TPrimerDesign::OnExportPrimer ( wxCommandEvent &ev )
    {
    if ( lastPrimerActivated == -1 ) return ;
    TVector *v = new TVector ;
    v->setSequence ( primer[lastPrimerActivated].get53sequence () ) ;
    v->setName ( vec->getName() ) ;
    if ( primer[lastPrimerActivated].upper ) v->addName ( _T("-5'") ) ;
    else v->addName ( _T("-3'") ) ;
    v->setType ( TYPE_PRIMER ) ;
    v->setDescription ( primer[lastPrimerActivated].report() ) ;
    myapp()->frame->newFromVector ( v , TYPE_PRIMER ) ;
    }

void TPrimerDesign::OnImportPrimer ( wxCommandEvent &ev )
    {
    int a ;
    wxArrayChildBase cbl ;

    for ( a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
       if ( myapp()->frame->children[a] &&
       		myapp()->frame->children[a]->vec &&
         	myapp()->frame->children[a]->vec->getType() == TYPE_PRIMER )
          cbl.Add ( myapp()->frame->children[a] ) ;

    if ( cbl.GetCount() == 0 )
       {
       wxMessageBox ( txt("t_load_primers_first") ) ;
       return ;
       }

    wxString *sl = new wxString[cbl.GetCount()] ;
    for ( a = 0 ; a < cbl.GetCount() ; a++ )
        sl[a] = cbl[a]->getName() ;

    TMyMultipleChoiceDialog scd ( this ,
                            txt("t_chose_primer_txt") ,
                            txt("t_chose_primer") ,
                            cbl.GetCount() ,
                            sl
                            ) ;

    if ( cbl.GetCount() <= 2 ) scd.CheckAll () ;
    int res = scd.ShowModal() ;
    delete [] sl ;
    if ( res != wxID_OK ) return ;

    for ( a = 0 ; a < cbl.GetCount() ; a++ )
       {
       if ( scd.IsChecked ( a ) )
          AddPrimer ( cbl[a]->vec->getSequence() , cbl[a]->vec->getName() ) ;
       }
    lc->SetColumnWidth ( 5 , wxLIST_AUTOSIZE_USEHEADER ) ;
    guessOptNuc () ;
    }

void TPrimerDesign::AddPrimer ( const wxString& s , const wxString& pname )
    {
    TPrimer best ;
    int a , bestVal = 0 ;
    for ( a = 0 ; a < vec->getSequenceLength() ; a++ )
        {
        TPrimer pu ( a , a + s.length() - 1 , true ) ;
        TPrimer pl ( a , a + s.length() - 1 , false ) ;
        pu.sequence = s ; // 5'-primer
        pl.sequence = pu.get35sequence() ; // 3'-primer

        if ( pu.checkFit ( vec ) > bestVal )
           {
           bestVal = pu.checkFit ( vec ) ;
           best = pu ;
           }

        if ( pl.checkFit ( vec ) > bestVal )
           {
           bestVal = pl.checkFit ( vec ) ;
           best = pl ;
           }

        }

    best.setName ( pname ) ;
    primer.push_back ( best ) ;
    showSequence () ;
    updatePrimersFromSequence () ;
    doShowPrimer ( primer.size() - 1 ) ;
    }

void TPrimerDesign::OnEditMode(wxCommandEvent& event)
    {
    wxMenuBar *mb = GetMenuBar () ;
    wxMenuItem *mi = mb->FindItem ( MDI_EDIT_MODE ) ;
    wxString s ;
    wxString item = _T("PRIMER_UP") ;
    if ( sc->lastmarked != -1 ) item = sc->seq[sc->lastmarked]->whatsthis() ;
    sc->edit_id = item ;

    if ( sc->getEditMode() ) sc->stopEdit() ;
    else sc->startEdit ( item ) ;
    /*
//    sc->edit_valid = "AGCT " ;
    if ( !sc->getEditMode() )
        {
        sc->setEditMode ( true ) ;
        sc->arrange () ;
        if ( sc->markedFrom() == -1 )
           sc->mark ( item , 1 , 1 , 2 ) ;
        else
           sc->mark ( item , sc->markedFrom() , sc->markedFrom() , 2 ) ;
        sc->SetFocus() ;
        sc->Scroll ( 0 , sc->getBatchMark() ) ;
        }
    else
        {
        sc->mark ( item , -1 , -1 ) ;
        sc->setEditMode ( false ) ;
        sc->arrange () ;
        Refresh () ;
        }*/
    mi->Check ( sc->getEditMode() ) ;
    }

void TPrimerDesign::updatePrimersFromSequence ()
    {
    int a , b ;
    wxString s ;
    updating = true ;
    for ( a = 0 ; a < primer.size() ; a++ )
        {
        if ( primer[a].upper ) s = sc->seq[show_features]->s ;
        else s = sc->seq[4+show_features]->s ;

        TVector d ;
        d.setSequence ( s ) ;
        d.setCircular ( vec->isCircular() ) ;

        for ( b = primer[a].from-1 ; b <= primer[a].to-1 && d.getNucleotide(b) == ' ' ; b++ ) ;
        if ( b == primer[a].to ) // Primer has been deleted
           {
           for ( b = a+1 ; b < primer.size() ; b++ )
              primer[b-1] = primer[b] ;
           primer.pop_back() ;
           a-- ;
           continue ;
           }

        for ( b = primer[a].from-1 ; d.getNucleotide(b) == ' ' ; b++ ) ;
        while ( d.getNucleotide(b) != ' ' ) b-- ;
        primer[a].from = b+2 ;
        for ( b = primer[a].to-1 ; d.getNucleotide(b) == ' ' ; b-- ) ;
        while ( d.getNucleotide(b) != ' ' ) b++ ;
        primer[a].to = b ;

        primer[a].getSequenceFromVector ( &d , !primer[a].upper ) ;
        }
    updatePrimerStats () ;
    guessOptNuc() ;
    updating = false ;
    }

void TPrimerDesign::updatePrimerStats ()
    {
    int a ;
    lc->ClearAll () ;
    lc->InsertColumn ( 0 , txt("#") ) ;
    lc->InsertColumn ( 1 , txt("h_direction") ) ;
    lc->InsertColumn ( 2 , txt("length") ) ;
    lc->InsertColumn ( 3 , txt("h_tm") ) ;
    lc->InsertColumn ( 4 , txt("%GC") ) ;
    lc->InsertColumn ( 5 , txt("name") ) ;
    lc->InsertColumn ( 6 , txt("h_position") ) ;
    for ( a = 0 ; a < primer.size() ; a++ )
        {
        primer[a].annealingVector = vec ;
        primer[a].makeStats () ;
        primer[a].evaluate () ;
		wxString u ;
        if ( primer[a].upper ) u = _T("5'->3'") ; //strcpy ( u , "-->" ) ;
        else u = _T("3'->5'") ; //strcpy ( u , "<--" ) ;
        int len = primer[a].to - primer[a].from + 1 ;

        long l = lc->InsertItem ( a , wxString::Format ( _T("%d") , a+1 ) ) ;

        lc->SetItem ( l , 1 , u ) ;
        lc->SetItem ( l , 2 , wxString::Format ( _T("%d") , len ) ) ;
        lc->SetItem ( l , 3 , wxString::Format ( _T("%2.1f") , primer[a].getTm() ) ) ;
        lc->SetItem ( l , 4 , wxString::Format ( _T("%2.1f") , primer[a].getGCcontents() ) ) ;
        lc->SetItem ( l , 5 , primer[a].getName() ) ;
        lc->SetItem ( l , 6 , wxString::Format ( _T("%d") , primer[a].from ) ) ;
        }
    for ( a = 0 ; a <= 6 ; a++ )
    	lc->SetColumnWidth ( a , wxLIST_AUTOSIZE_USEHEADER ) ;
    stat->SetValue ( _T("") ) ;
    lastPrimerActivated = -1 ;
    }


void TPrimerDesign::initme ()
    {

    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(PD_IMPORT, txt("b_import_primer") );
    edit_menu->Append(PD_EXPORT, txt("b_export_primer") );
    edit_menu->AppendSeparator();
//    edit_menu->Append(MDI_MARK_ALL, txt("m_mark_all") );
//    edit_menu->Append(MDI_CUT, txt("m_cut") );
    edit_menu->Append(MDI_COPY, txt("m_copy") );
    edit_menu->Append ( SEQ_INSERT_RESTRICTION_SITE_LEFT , txt("m_insert_restiction_site_left") ) ;
    edit_menu->Append ( SEQ_INSERT_RESTRICTION_SITE_RIGHT , txt("m_insert_restiction_site_right") ) ;
    edit_menu->Append ( PD_TROUBLESHOOT , txt("m_troubleshoot") ) ;

    wxMenu *view_menu = new wxMenu;
/*    view_menu->Append(MDI_REFRESH, txt("m_refresh_picture") );
    view_menu->Append(MDI_CIRCULAR_LINEAR, txt("m_toggle_rc") );
    view_menu->Append(MDI_VIEW_MODE, txt("m_view_mode") , "" , true );*/

    wxMenu *mAA = new wxMenu ;
    view_menu->Append(MDI_EDIT_MODE, txt("m_edit_mode") , _T("") , true );
    view_menu->Append(MDI_TOGGLE_FEATURES, txt("m_display_features") , _T("") );
    view_menu->Append ( AA_MAIN , txt("m_aa_main") , mAA ) ;
    mAA->Append (AA_ALL,txt("m_aa_all"), _T("") , true );
    mAA->Append (AA_KNOWN,txt("m_aa_known"), _T("") , true );
    mAA->Append (AA_THREE_1,txt("m_aa_1"), _T("") , true );
    mAA->Append (AA_THREE_2,txt("m_aa_2"), _T("") , true );
    mAA->Append (AA_THREE_3,txt("m_aa_3"), _T("") , true );
    mAA->Append (AA_THREE_M1,txt("m_aa_m1"), _T("") , true );
    mAA->Append (AA_THREE_M2,txt("m_aa_m2"), _T("") , true );
    mAA->Append (AA_THREE_M3,txt("m_aa_m3"), _T("") , true );
    mAA->AppendSeparator();
    mAA->Append (AA_ONE,txt("m_aa_one"), _T("") , true );
    mAA->Append (AA_THREE,txt("m_aa_three"), _T("") , true );

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(edit_menu, txt("m_edit") );
    menu_bar->Append(view_menu, txt("m_view") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    menu_bar->FindItem(aa_state)->Check ( true ) ;
    menu_bar->FindItem(aa_disp)->Check ( true ) ;

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);



//    hs = new wxSplitterWindow ( this , SPLIT_AMINOACIDS ) ;

    // Sequence Canvas
    sc = new SequenceCanvas ( this , wxDefaultPosition , wxDefaultSize ) ;//wxPoint ( 0 , 0 ) , wxSize ( 1000 , 1000 ) ) ;
    sc->blankline = 1 ;
    sc->child = this ;
    sc->edit_id = _T("DNA") ;
    sc->edit_valid = _T("ACTG .") ;
    sc->forceOverwrite ( true ) ;

    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    myapp()->frame->InitToolBar(toolBar);
	myapp()->frame->addTool ( toolBar , MDI_TEXT_IMPORT ) ;
	myapp()->frame->addTool ( toolBar , MDI_FILE_OPEN ) ;
	myapp()->frame->addTool ( toolBar , SEQ_PRINT ) ;
    if ( !myapp()->frame->mainToolBar ) toolBar->AddSeparator() ;
    toolBar->AddTool( PD_IMPORT,
            wxEmptyString ,
            myapp()->frame->bitmaps[14] ,
            txt("b_import_primer") ) ;
    toolBar->AddTool( PD_EXPORT,
            wxEmptyString ,
            myapp()->frame->bitmaps[15] ,
            txt("b_export_primer") ) ;
    toolBar->AddSeparator() ;
	myapp()->frame->addTool ( toolBar , MDI_COPY ) ;
    toolBar->AddTool( MDI_EDIT_MODE,
        wxEmptyString ,
        myapp()->frame->bitmaps[13] ,
        myapp()->frame->bitmaps[13] ,
        wxITEM_CHECK, txt("m_edit_mode") ) ;
    toolBar->AddTool( MDI_TOGGLE_FEATURES,
        wxEmptyString ,
        myapp()->frame->bitmaps[10] ,
        myapp()->frame->bitmaps[10] ,
        wxITEM_CHECK, txt("m_display_features") ) ;
    toolBar->AddSeparator() ;

    spin = new wxSpinCtrl ( toolBar , PCR_SPIN , _T("") , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , -1 ) ) ;
    spin->SetRange ( 1 , vec->getSequenceLength() ) ;
    spin->SetValue ( wxString::Format ( _T("%d") , vec->getSequenceLength() ) ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_pcr_spin_1") ) ) ;
    toolBar->AddControl ( spin ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_pcr_spin_2") ) ) ;

#ifndef __WXMSW__
	spin->SetSize ( -1 , -1 , 90 , -1 , wxSIZE_USE_EXISTING ) ;
#endif

    toolBar->AddSeparator () ;
	fontsize = myapp()->frame->AddFontsizeTool ( toolBar , AA_FONTSIZE ) ;
    wxCheckBox *mycb = new wxCheckBox ( toolBar , ALIGN_HORIZ , txt("t_horizontal") ) ;
    toolBar->AddControl ( mycb ) ;
    myapp()->frame->addDefaultTools ( toolBar ) ;

    toolBar->Realize() ;
    toolbar = toolBar ;

    guessOptNuc () ;

    int w , h ;
    Maximize () ;
    GetClientSize ( &w , &h ) ;

    // Upper panel
    v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;

    GetClientSize ( &w , &h ) ;
    lc = new wxListCtrl ( this , PD_LC , wxDefaultPosition , wxDefaultSize , wxLC_REPORT|wxLC_SINGLE_SEL ) ;


    v1->Add ( new wxButton ( this , PD_EDIT , txt("m_edit") , wxPoint ( w/3 + 5 , 20 ) ) , 0 , wxALL , 2 ) ;
    v1->Add ( new wxButton ( this , PD_DEL , txt("b_del") , wxPoint ( w/3 + 5 , 50 ) ) , 0 , wxALL , 2 ) ;

    stat = new wxTextCtrl ( this ,
                            -1 ,
                            _T("") ,
                            wxDefaultPosition,//wxPoint ( w/3 + 70 , 0 ) ,
                            wxDefaultSize,//wxSize ( w*2/3 - 70 , h ) ,
                            wxTE_MULTILINE | wxTE_READONLY ) ;

    stat->SetFont ( *MYFONT ( MYFONTSIZE , wxFONTFAMILY_MODERN , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;

    GetToolBar()->ToggleTool(MDI_TOGGLE_FEATURES,show_features);

    h0->Add ( lc , 1 , wxEXPAND , 2 ) ;
    h0->Add ( v1 , 0 , wxEXPAND , 2 ) ;
    h0->Add ( stat , 1 , wxEXPAND , 2 ) ;

    v0->Add ( toolbar , 0 , wxEXPAND , 2 ) ;
    v0->Add ( h0 , 0 , wxEXPAND , 2 ) ;
    v0->Add ( sc , 1 , wxEXPAND , 2 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

    showSequence () ;
    updatePrimersFromSequence() ;
    myapp()->frame->setChild ( this ) ;
    spinTextEnabeled = true ;
    sc->SetFocus() ;
    for ( int a = 0 ; a < lc->GetColumnCount() ; a++ )
	    lc->SetColumnWidth ( a , wxLIST_AUTOSIZE_USEHEADER ) ;
    }

void TPrimerDesign::OnEditPrimer ( wxCommandEvent &ev )
    {
    if ( lastPrimerActivated == -1 ) return ;
    if ( primer.size() == 0 ) return ;
    TPrimerDialog pd ( (wxWindow*) this , _T("") ) ;
    pd.initme ( &primer[lastPrimerActivated] , this ) ;
    if ( wxID_OK != pd.ShowModal() ) return ;
    primer[lastPrimerActivated] = *(pd.cp) ;
    showSequence () ;
    }

void TPrimerDesign::OnDeletePrimer ( wxCommandEvent &ev )
    {
    if ( lastPrimerActivated == -1 ) return ;
    if ( primer.size() == 0 ) return ;
    for ( int a = lastPrimerActivated ; a + 1 < primer.size() ; a++ )
       primer[a] = primer[a+1] ;
    primer.pop_back () ;
    lastPrimerActivated = -1 ;
    showSequence () ;
    updatePrimersFromSequence () ;
    }

void TPrimerDesign::OnCopy(wxCommandEvent& event)
    {
    sc->OnCopy ( event ) ;
    }

void TPrimerDesign::OnPaste (wxCommandEvent& event)
    {
    sc->OnPaste ( event ) ;
    }


wxString TPrimerDesign::getName () const
    {
    return vec->getName() ;
    }

void TPrimerDesign::showSequence ()
    {
    // Cleaning up
    CLEAR_DELETE ( sc->seq ) ;

    // Display
    int a ;
    sc->blankline = 0 ;
    inverse_template_vector->setSequence ( vec->transformSequence ( true , false ) ) ;
    inverse_template_vector->setCircular ( vec->isCircular() ) ;

    // Features
    SeqFeature *seqF = NULL ;
    if ( show_features )
       {
       seqF = new SeqFeature ( sc ) ;
       seqF->initFromTVector ( vec ) ;
       }

    // Upper primer
    SeqPrimer *p1 = new SeqPrimer ( sc ) ;
    p1->initFromTVector ( vec ) ;
    p1->takesMouseActions = true ;
    p1->myname = _T("PRIMER_UP") ;
    p1->alternateName = txt("t_primer_up") ;
    for ( a = 0 ; a < primer.size() ; a++ )
        if ( primer[a].upper )
           p1->addPrimer ( &primer[a] ) ;

	// Upper template sequence
    SeqDNA *s1 = new SeqDNA ( sc ) ;
    s1->initFromTVector ( vec ) ;
    s1->takesMouseActions = false ;

    // Amino acid sequence of template
    SeqAA *a1 = new SeqAA ( sc ) ;
    a1->mode = aa_state ;
    a1->disp = aa_disp ;
    a1->initFromTVector ( vec ) ;
    a1->takesMouseActions = false ;

	// Lower template sequence
    SeqDNA *s2 = new SeqDNA ( sc ) ;
    s2->initFromTVector ( inverse_template_vector ) ;
    s2->showNumbers = false ;
    s2->alternateName = _T("3'") ;
    s2->takesMouseActions = false ;

	// Lower primer
    SeqPrimer *p2 = new SeqPrimer ( sc ) ;
    p2->initFromTVector ( inverse_template_vector ) ;
    p2->takesMouseActions = true ;
    p2->myname = _T("PRIMER_DOWN") ;
    p2->alternateName = txt("t_primer_down") ;
    for ( a = 0 ; a < primer.size() ; a++ )
        if ( !primer[a].upper )
           p2->addPrimer ( &primer[a] ) ;

    if ( show_features ) sc->seq.Add ( seqF ) ;
    sc->seq.Add ( p1 ) ;
    sc->seq.Add ( s1 ) ;
    sc->seq.Add ( a1 ) ;
    sc->seq.Add ( s2 ) ;
    sc->seq.Add ( p2 ) ;
    updateResultSequence() ;

    sc->arrange () ;
    sc->Refresh ( false ) ;
    updatePrimerStats () ;
    }

void TPrimerDesign::calculateResultSequence()
    {
    int a , b ;

    // Constructing result vector
    resulting_sequence_vector->setFromVector ( *vec ) ;

    int nuc = spin->GetValue() ;
    for ( a = 0 ; a < resulting_sequence_vector->getSequenceLength() ; a++ ) resulting_sequence_vector->alterSequence ( a , ' ' ) ;
    TVector w2 ;
    w2.setFromVector ( *resulting_sequence_vector ) ;

    for ( a = 0 ; a < primer.size() ; a++ )
        {
        TVector *wp = primer[a].upper ? resulting_sequence_vector : &w2 ;
        int factor = primer[a].upper ? 1 : -1 ;
        int start = primer[a].upper ? primer[a].to : primer[a].from ;
        for ( b = 0 ; b < nuc ; b++ )
           {
           int pos = b * factor + start ;
           wp->setNucleotide ( pos , vec->getNucleotide ( pos ) ) ;
           }
        }

    for ( a = 0 ; a < resulting_sequence_vector->getSequenceLength() ; a++ )
        {
        if ( resulting_sequence_vector->getSequenceChar(a) != ' ' && w2.getSequenceChar(a) != ' ' )
           resulting_sequence_vector->alterSequence ( a , vec->getSequenceChar(a) ) ;
        else resulting_sequence_vector->alterSequence ( a , ' ' ) ;
        }

    // Overwriting result sequence with primer sequences
    for ( a = 0 ; a < resulting_sequence_vector->getSequenceLength() ; a++ )
        {
        char t = sc->seq[show_features]->s.GetChar(a) ;
        if ( t == ' ' )
           {
           t = sc->seq[4+show_features]->s.GetChar(a) ;
           if ( t != ' ' ) t = resulting_sequence_vector->getComplement ( t ) ;
           }
        if ( t != ' ' ) resulting_sequence_vector->setNucleotide ( a , t ) ;
        }

    resulting_sequence_vector->recalculateCuts() ;
    }

void TPrimerDesign::updateResultSequence()
    {
    calculateResultSequence() ;

    while ( sc && sc->seq.GetCount() > 5 + show_features )
        {
        delete sc->seq[sc->seq.GetCount()-1] ;
        sc->seq.RemoveAt ( sc->seq.GetCount()-1 ) ;
        }

    SeqRestriction *r3 = new SeqRestriction ( sc ) ;
    SeqDNA *s3 = new SeqDNA ( sc ) ;
    SeqAA *a3 = new SeqAA ( sc ) ;

	// Restriction of resulting sequence
    r3->initFromTVector ( resulting_sequence_vector ) ;
    r3->down = true ;
    r3->takesMouseActions = false ;

	// Resulting sequence
    s3->initFromTVector ( resulting_sequence_vector ) ;
    s3->showNumbers = false ;
    s3->fontColor.Set ( 0 , 100 , 0 ) ;
    s3->takesMouseActions = false ;
    s3->alternateName = _T("RES") ;

    // Resulting amino acids
    a3->mode = aa_state ;
    a3->disp = aa_disp ;
    a3->unknownAA = ' ' ;
    a3->initFromTVector ( resulting_sequence_vector ) ;
    a3->takesMouseActions = false ;
    a3->showNumbers = false ;
    a3->show_diff_to = (SeqAA*) sc->seq[show_features?3:2] ;

    sc->seq.Add ( r3 ) ;
    sc->seq.Add ( s3 ) ;
    sc->seq.Add ( a3 ) ;

    if ( !sc->isHorizontal() )
        {
        SeqDivider *div = new SeqDivider ( sc ) ;
        div->initFromTVector ( vec ) ;
        sc->seq.Add ( div ) ;
        }
    }

void TPrimerDesign::OnAA_setit(int mode)
    {
    int oldto = sc->markedTo() , oldfrom = sc->markedFrom() , lastmarked = sc->lastmarked ;
    bool wasZero = aa_state == AA_NONE ;
    int oldscrollpos , dummy ;
    sc->GetViewStart ( &dummy , &oldscrollpos ) ;
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
    if ( mode == AA_THREE || mode == AA_ONE )
        mi = mb->FindItem ( aa_disp ) ;
    else
        mi = mb->FindItem ( aa_state ) ;
    mi->Check ( false ) ;
    if ( aa_state == AA_NONE )
        {
        sc->seq.Add ( NULL ) ;
        for ( int a = sc->seq.GetCount()-1 ; a >= 1 ; a-- )
           sc->seq[a] = sc->seq[a-1] ;
        SeqAA *seqAA = new SeqAA ( sc ) ;
        sc->seq[0] = seqAA ;
        seqAA->initFromTVector ( vec ) ;
        }
    if ( mode == AA_THREE || mode == AA_ONE )
        aa_disp = mode ;
    else
        aa_state = mode ;
    mi = mb->FindItem ( aa_state ) ;
    mi->Check () ;
/*    SeqAA *seqAA = (SeqAA*) sc->seq[0] ;
    seqAA->mode = aa_state ;
    seqAA->disp = aa_disp ;
    seqAA->initFromTVector ( vec ) ;
    sc->arrange () ;
    sc->Refresh () ;*/
    showSequence () ;
    if ( !wasZero && aa_state != AA_NONE )
        sc->Scroll ( -1 , oldscrollpos ) ;
    if ( sc->getEditMode() && lastmarked != -1 )
        sc->mark ( sc->seq[lastmarked]->whatsthis() , oldfrom , oldfrom , 2 ) ;
    else if ( !sc->getEditMode() && lastmarked != -1 )
        {
        wxString s = sc->seq[lastmarked]->whatsthis() ;
        sc->mark ( s , oldfrom , oldto ) ;
        }
    }

void TPrimerDesign::OnAA_known(wxCommandEvent& event)
    {    OnAA_setit ( AA_KNOWN ) ;    }

void TPrimerDesign::OnAA_all(wxCommandEvent& event)
    {    OnAA_setit ( AA_ALL ) ;    }

void TPrimerDesign::OnAA_three_1(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_1 ) ;    }

void TPrimerDesign::OnAA_three_2(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_2 ) ;    }

void TPrimerDesign::OnAA_three_3(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_3 ) ;    }

void TPrimerDesign::OnAA_three_M1(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_M1 ) ;    }

void TPrimerDesign::OnAA_three_M2(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_M2 ) ;    }

void TPrimerDesign::OnAA_three_M3(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE_M3 ) ;    }

void TPrimerDesign::OnAA_three(wxCommandEvent& event)
    {    OnAA_setit ( AA_THREE ) ;    }

void TPrimerDesign::OnAA_one(wxCommandEvent& event)
    {    OnAA_setit ( AA_ONE ) ;    }


void TPrimerDesign::doShowPrimer ( int i )
    {
    int from = primer[i].from ;
    int to = primer[i].to ;
    if ( to >= vec->getSequenceLength() ) to -= vec->getSequenceLength() ;
    wxString p ;
    if ( primer[i].upper ) p = _T("PRIMER_UP") ;
    else p = _T("PRIMER_DOWN") ;
    sc->mark ( p , from , to ) ;
    sc->ensureVisible(sc->markedFrom()) ;
//    sc->Scroll ( 0 , sc->getBatchMark() ) ;
    sc->SetFocus () ;
    }

void TPrimerDesign::OnSelectPrimer ( wxListEvent& event)
    {
    lastPrimerActivated = event.GetIndex () ;
    wxString x = primer[lastPrimerActivated].report() ;
    stat->SetValue ( x ) ;
    sc->SetFocus () ;
    }

void TPrimerDesign::OnActivatePrimer ( wxListEvent& event)
    {
    lastPrimerActivated = event.GetIndex () ;
    doShowPrimer ( lastPrimerActivated ) ;
    }

void TPrimerDesign::OnSilmut ( wxCommandEvent& event)
    {
	if ( !vec || !resulting_sequence_vector ) return ; // Paranoia

	// Construct new vector containing just the resulting sequence
	wxString resulting_sequence ;
	int from = sc->markedFrom() ;
	int to = sc->markedTo() ;
	int rsvl = resulting_sequence_vector->getSequenceLength() ;
	wxString dummy = resulting_sequence_vector->getSequence() ;
	dummy.Replace ( _T(" ") , _T("") ) ;

	TVector *temp_result = sc->getPCR_DNA_vector () ;
//	if ( aa_disp != AA_KNOWN ) temp_result->items.clear () ;

	int old_aa_disp = aa_disp ;
	int old_aa_state = aa_state ;

	int new_dir = 0 ;
	switch ( aa_state )
		{
		case AA_THREE_1  : new_dir =  1 ; break ;
		case AA_THREE_2  : new_dir =  2 ; break ;
		case AA_THREE_3  : new_dir =  3 ; break ;
		case AA_THREE_M1 : new_dir = -1 ; break ;
		case AA_THREE_M2 : new_dir = -2 ; break ;
		case AA_THREE_M3 : new_dir = -3 ; break ;
		}

	if ( vec->getSequenceLength() != dummy.length() )
		{
		int move_to_left = 0 ;
		if ( resulting_sequence_vector->getSequence().GetChar(0) == ' ' )
			{
			resulting_sequence = resulting_sequence_vector->getSequence().Trim(false) ;
			move_to_left = rsvl - resulting_sequence.length () ;
			from -= move_to_left ;
			to -= move_to_left ;
			}
		else
			{
			resulting_sequence = resulting_sequence_vector->getSequence() ;
			wxString part1 = resulting_sequence.AfterLast(' ') ;
			move_to_left = rsvl - part1.length () ;
			from -= move_to_left ;
			to -= move_to_left ;
			if ( from < 1 || to < 1 ) { from += rsvl ; to += rsvl ; }
			}
		if ( new_dir != 0 )
			{
			move_to_left %= 3 ;
			if ( move_to_left == 1 )
				{
				switch ( new_dir )
					{
					case  1 : new_dir =  3 ; break ;
					case  2 : new_dir =  1 ; break ;
					case  3 : new_dir =  2 ; break ;
					case -1 : new_dir = -3 ; break ;
					case -2 : new_dir = -1 ; break ;
					case -3 : new_dir = -2 ; break ;
					}
				}
			else if ( move_to_left == 2 )
				{
				switch ( new_dir )
					{
					case  1 : new_dir =  2 ; break ;
					case  2 : new_dir =  3 ; break ;
					case  3 : new_dir =  1 ; break ;
					case -1 : new_dir = -2 ; break ;
					case -2 : new_dir = -3 ; break ;
					case -3 : new_dir = -1 ; break ;
					}
				}
			}
		}
	if ( from < 0 ) { to += -from ; from = 0 ; }

	if ( new_dir != 0 ) aa_disp = AA_ONE ;
	switch ( new_dir )
		{
		case  1 : aa_state = AA_THREE_1 ; break ;
		case  2 : aa_state = AA_THREE_2 ; break ;
		case  3 : aa_state = AA_THREE_3 ; break ;
		case -1 : aa_state = AA_THREE_M1 ; break ;
		case -2 : aa_state = AA_THREE_M2 ; break ;
		case -3 : aa_state = AA_THREE_M3 ; break ;
		}


	// Run silmut dialog
    TSilmutDialog sd ( this , txt("t_silmut") ) ;
    sd.initme ( temp_result , from , to ) ;
    int dialog_result = sd.ShowModal () ;

    aa_state = old_aa_state ;
    aa_disp = old_aa_disp ;

    if ( wxID_OK != dialog_result ) return ; //Canceled
    wxString ns = sd.getSequence() ;
    if ( ns.IsEmpty() ) return ;

    // Change sequence to mutated version
    TVector z ;
    z.setSequence ( ns ) ;
    wxString nt = z.transformSequence ( true , false ) ;
    TRestrictionEnzyme *e = sd.getEnzyme() ;
    int a , b ;
    for ( a = 0 ; a < ns.length() ; a++ )
        {
        if ( ns.GetChar(a) >= 'A' && ns.GetChar(a) <= 'Z' )
           {
           b = a + sc->markedFrom() - 1 ;
           if ( sc->seq[show_features]->s.GetChar(b) != ' ' ) sc->seq[show_features]->s.SetChar(b,ns.GetChar(a)) ;
           if ( sc->seq[4+show_features]->s.GetChar(b) != ' ' ) sc->seq[4+show_features]->s.SetChar(b,nt.GetChar(a)) ;
           }
        }

    // Add new restriction enzyme and refresh display
	resulting_sequence_vector->addRestrictionEnzyme ( e ) ;
	vec->addRestrictionEnzyme ( e ) ;
	if ( inverse_template_vector ) inverse_template_vector->addRestrictionEnzyme ( e ) ;
    updatePrimersFromSequence () ;
    showSequence () ;
    }

void TPrimerDesign::OnToggleFeatures ( wxCommandEvent &ev )
    {
    show_features = sc->findID ( _T("FEATURE") ) ? 0 : 1 ;
    showSequence () ;
    GetToolBar()->ToggleTool(MDI_TOGGLE_FEATURES,show_features);
    }

void TPrimerDesign::OnSpin(wxSpinEvent& event)
    {
    updateResultSequence() ;
    sc->arrange () ;
    sc->Refresh ( false ) ;
    }

void TPrimerDesign::OnSpinText(wxCommandEvent& event)
    {
    if ( spinTextEnabeled )
        {
        updateResultSequence() ;
        sc->arrange () ;
        sc->Refresh ( false ) ;
        }
    }

void TPrimerDesign::OnHorizontal ( wxCommandEvent& event )
    {
    sc->toggleHorizontal () ;
    if ( sc->isHorizontal() )
       {
       delete sc->seq[sc->seq.GetCount()-1] ;
       sc->seq.RemoveAt ( sc->seq.GetCount()-1 ) ;
       }
    else
       {
       SeqDivider *div = new SeqDivider ( sc ) ;
       div->initFromTVector ( vec ) ;
       sc->seq.Add ( div ) ;
       }
    sc->arrange () ;
    sc->SilentRefresh() ;
    sc->SetFocus () ;
    }

void TPrimerDesign::OnFind(wxCommandEvent& event)
    {
    FindSequenceDialog fsd ( this , txt("t_find") ) ;
    fsd.ShowModal () ;
    }

void TPrimerDesign::OnInsertRestrictionSiteLeft(wxCommandEvent& event)
    {
    sc->OnInsertRestrictionSiteLeft ( event ) ;
    }

void TPrimerDesign::OnInsertRestrictionSiteRight(wxCommandEvent& event)
    {
    sc->OnInsertRestrictionSiteRight ( event ) ;
    }

