#include "PrimerDesign.h"

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
    
    EVT_LIST_ITEM_ACTIVATED(PD_LC,TPrimerDesign::OnActivatePrimer)
    EVT_LIST_ITEM_SELECTED(PD_LC,TPrimerDesign::OnSelectPrimer)


/*    EVT_MENU(MDI_EDIT_MODE, TPrimerDesign::OnEditMode)
    EVT_MENU(MDI_MARK_ALL, TPrimerDesign::OnMarkAll)
    EVT_MENU(MDI_FILE_SAVE, TPrimerDesign::OnFileSave)
    EVT_MENU(MDI_EXPORT, TPrimerDesign::OnExport)
    EVT_MENU(MDI_FIND, TPrimerDesign::OnFind)
    EVT_MENU(MDI_CUT, TPrimerDesign::OnCut)
    EVT_MENU(AMINOACIDS_EDIT_NAME, TPrimerDesign::OnEditName)

    EVT_SET_FOCUS(ChildBase::OnFocus)*/

    EVT_MENU(MDI_COPY, TPrimerDesign::OnCopy)

    EVT_MENU(MDI_EDIT_MODE, TPrimerDesign::OnEditMode)
    EVT_CLOSE(ChildBase::OnClose)
END_EVENT_TABLE()

TPrimerDesign::TPrimerDesign(wxWindow *parent, 
                    wxString title,
                    TVector *_vec,
                    vector <TPrimer> pl ,
                    int _mut )
    : ChildBase(parent, title)
    {
    int a ;
    show_features = 1 ;
    spinTextEnabeled = false ;
    mut = _mut ;
    vec = new TVector ;
    vc = new TVector ;
    w = new TVector ;
    vec->setFromVector ( *_vec ) ;
    w->setFromVector ( *vec ) ;
    
    aa_state = AA_ALL ;
    aa_disp = AA_ONE ;
    for ( a = 0 ; a < vec->items.size() ; a++ )
        if ( vec->items[a].getRF() != 0 ) aa_state = AA_KNOWN ;
    
    primer = pl ;
    
    for ( a = 0 ; a < primer.size() ; a++ )
        {
        if ( primer[a].from >= vec->sequence.length() )
           {
           primer[a].from -= vec->sequence.length() ;
           primer[a].to -= vec->sequence.length() ;
           }
        }
    
    for ( a = 0 ; a < primer.size() ; a++ )
        primer[a].getSequenceFromVector ( vec ) ;
        
    def = "PrimerDesign" ;
    from = -1 ;
    stat = NULL ;
    lastPrimerActivated = -1 ;
    }
    
TPrimerDesign::~TPrimerDesign ()
    {
    if ( stat ) delete stat ;
    delete vc ;
    delete w ;
    }
    
void TPrimerDesign::guessOptNuc ()
    {
    int nuc = vec->sequence.length() ;
    
    if ( primer.size() == 2 )
        {
        if ( primer[0].overlap ( primer[1] ) ) // Mutagenesis?
           {
           }
        else // Probably just a piece
           {
           if ( primer[0].upper ) nuc = primer[1].from - primer[0].to ;
           else nuc = primer[1].to - primer[0].from ;
           if ( nuc < 0 ) nuc = vec->sequence.length() + nuc ;
           nuc++ ;
           }
        }
    
    spin->SetValue ( wxString::Format("%d",nuc) ) ;
    }

void TPrimerDesign::OnPrint ( wxCommandEvent &ev )
    {
    sc->OnPrint ( ev ) ;
    }
    
void TPrimerDesign::OnExportPrimer ( wxCommandEvent &ev )
    {
    if ( lastPrimerActivated == -1 ) return ;
    TVector *v = new TVector ;
    v->sequence = primer[lastPrimerActivated].get53sequence () ;
    v->name = vec->name ;
    if ( primer[lastPrimerActivated].upper ) v->name += "-5'" ;
    else v->name += "-3'" ;
    v->type = TYPE_PRIMER ;
    myapp()->frame->newFromVector ( v , TYPE_PRIMER ) ;
    }

void TPrimerDesign::OnImportPrimer ( wxCommandEvent &ev )
    {
    int a ;
    vector <ChildBase*> cbl ;
    
    for ( a = 0 ; a < myapp()->frame->children.size() ; a++ )
       if ( myapp()->frame->children[a]->vec->type == TYPE_PRIMER )
          cbl.push_back ( myapp()->frame->children[a] ) ;
    
    if ( cbl.size() == 0 )
       {
       wxMessageBox ( txt("t_load_primers_first") ) ;
       return ;
       }
    
    wxString *sl = new wxString[cbl.size()] ;
    for ( a = 0 ; a < cbl.size() ; a++ )
        sl[a] = cbl[a]->getName().c_str() ;

    TMyMultipleChoiceDialog scd ( this , 
                            txt("t_chose_primer_txt") , 
                            txt("t_chose_primer") ,
                            cbl.size() ,
                            sl
                            ) ;
                            
    if ( cbl.size() <= 2 ) scd.CheckAll () ;
    int res = scd.ShowModal() ;
    delete sl ;
    if ( res != wxID_OK ) return ;
    
    for ( a = 0 ; a < cbl.size() ; a++ )
       {
       if ( scd.IsChecked ( a ) )
          AddPrimer ( cbl[a]->vec->sequence ) ;
       }
    guessOptNuc () ;
    }
    
void TPrimerDesign::AddPrimer ( string s )
    {
    TPrimer best ;
    int a , bestVal = 0 ;
    for ( a = 0 ; a + s.length() < vec->sequence.length() ; a++ )
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
    string item = "PRIMER_UP" ;
    if ( sc->lastmarked != -1 ) item = sc->seq[sc->lastmarked]->whatsthis() ;
    sc->edit_id = item ;
//    sc->edit_valid = "AGCT " ;
    if ( !sc->getEditMode() )
        {
        sc->setEditMode ( true ) ;
        sc->arrange () ;
        if ( sc->_from == -1 )
           sc->mark ( item , 1 , 1 , 2 ) ;
        else
           sc->mark ( item , sc->_from , sc->_from , 2 ) ;
        sc->SetFocus() ;
        sc->Scroll ( 0 , sc->getBatchMark() ) ;
        }
    else
        {
        sc->mark ( item , -1 , -1 ) ;
        sc->setEditMode ( false ) ;
        sc->arrange () ;
        Refresh () ;
        }
    mi->Check ( sc->getEditMode() ) ;
    }
    
void TPrimerDesign::updatePrimersFromSequence ()
    {
    int a , b ;
    string s ;
    for ( a = 0 ; a < primer.size() ; a++ )
        {
        if ( primer[a].upper ) s = sc->seq[show_features]->s ;
        else s = sc->seq[4+show_features]->s ;
        
        TVector d ;
        d.sequence = s ;
        d.setCircular ( vec->isCircular() ) ;

        for ( b = primer[a].from-1 ; d.getNucleotide(b) == ' ' ; b++ ) ;
        while ( d.getNucleotide(b) != ' ' ) b-- ;
        primer[a].from = b+2 ;
        for ( b = primer[a].to-1 ; d.getNucleotide(b) == ' ' ; b-- ) ;
        while ( d.getNucleotide(b) != ' ' ) b++ ;
        primer[a].to = b ;
        
        primer[a].getSequenceFromVector ( &d , !primer[a].upper ) ;
        }
    updatePrimerStats () ;
    }

void TPrimerDesign::updatePrimerStats ()
    {
    char t[1000] ;
    int a ;
//    lb->Clear () ;
    lc->ClearAll () ;
    lc->InsertColumn ( 0 , txt("#") ) ;
    lc->InsertColumn ( 1 , txt("h_direction") ) ;
    lc->InsertColumn ( 2 , txt("length") ) ;
    lc->InsertColumn ( 3 , txt("h_tm") ) ;
    lc->InsertColumn ( 4 , txt("%GC") ) ;
    for ( a = 0 ; a < primer.size() ; a++ )
        {
        primer[a].annealingVector = vec ;
        primer[a].makeStats () ;
        primer[a].evaluate () ;
        char u[5] ;
        if ( primer[a].upper ) strcpy ( u , "-->" ) ;
        else strcpy ( u , "<--" ) ;
        int len = primer[a].to - primer[a].from + 1 ;
        
        sprintf ( t , "%d" , a+1 ) ;
        long l = lc->InsertItem ( a , t ) ;
        
        lc->SetItem ( l , 1 , u ) ;
        
        sprintf ( t , "%d" , len ) ;
        lc->SetItem ( l , 2 , t ) ;
        
        sprintf ( t , "%2.1f" , primer[a].getTm() ) ;
        lc->SetItem ( l , 3 , t ) ;
        
        sprintf ( t , "%2.1f" , primer[a].getGCcontents() ) ;
        lc->SetItem ( l , 4 , t ) ;
        
        }
    lc->SetColumnWidth ( 0 , wxLIST_AUTOSIZE_USEHEADER ) ;
    lc->SetColumnWidth ( 1 , wxLIST_AUTOSIZE_USEHEADER ) ;
    lc->SetColumnWidth ( 2 , wxLIST_AUTOSIZE_USEHEADER ) ;
    lc->SetColumnWidth ( 3 , wxLIST_AUTOSIZE_USEHEADER ) ;
    lc->SetColumnWidth ( 4 , wxLIST_AUTOSIZE_USEHEADER ) ;
    stat->SetValue ( "" ) ;
    lastPrimerActivated = -1 ;
    }


void TPrimerDesign::initme ()
    {
    int bo = 5 ;

    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

/*
    wxMenu *edit_menu = new wxMenu;
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
*/

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(PD_IMPORT, txt("b_import_primer") );
    edit_menu->Append(PD_EXPORT, txt("b_export_primer") );
    edit_menu->AppendSeparator();
//    edit_menu->Append(MDI_MARK_ALL, txt("m_mark_all") );
//    edit_menu->Append(MDI_CUT, txt("m_cut") );
    edit_menu->Append(MDI_COPY, txt("m_copy") );


    wxMenu *view_menu = new wxMenu;
/*    view_menu->Append(MDI_REFRESH, txt("m_refresh_picture") );
    view_menu->Append(MDI_CIRCULAR_LINEAR, txt("m_toggle_rc") );
    view_menu->Append(MDI_VIEW_MODE, txt("m_view_mode") , "" , true );*/
    
    wxMenu *mAA = new wxMenu ;
    view_menu->Append(MDI_EDIT_MODE, txt("m_edit_mode") , "" , true );
    view_menu->Append(MDI_TOGGLE_FEATURES, txt("m_display_features") , "" );
    view_menu->Append ( AA_MAIN , txt("m_aa_main") , mAA ) ;
    mAA->Append (AA_ALL,txt("m_aa_all"), "" , true );
    mAA->Append (AA_KNOWN,txt("m_aa_known"), "" , true );
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

    menu_bar->FindItem(aa_state)->Check ( true ) ;
    menu_bar->FindItem(aa_disp)->Check ( true ) ;

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);



    hs = new wxSplitterWindow ( this , SPLIT_AMINOACIDS ) ;

    // Sequence Canvas
    sc = new SequenceCanvas ( hs , wxPoint ( 0 , 0 ) , wxSize ( 100 , 100 ) ) ;
    sc->blankline = 1 ;
    sc->child = this ;
    sc->edit_id = "DNA" ;
    sc->edit_valid = "ACTG ." ;
    sc->forceOverwrite ( true ) ;
    
#ifdef __WXMSW__
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL |wxTB_DOCKABLE);
    myapp()->frame->InitToolBar(toolBar);
    toolBar->AddTool( MDI_TEXT_IMPORT , 
            myapp()->frame->bitmaps[0] ,
            txt("m_new_primer") ) ;
    toolBar->AddTool( MDI_FILE_OPEN, 
            myapp()->frame->bitmaps[1] ,
            txt("m_open") , txt("m_opentxt") );
    toolBar->AddTool( PD_IMPORT, 
            myapp()->frame->bitmaps[14] ,
            txt("b_import_primer") ) ;
    toolBar->AddTool( PD_EXPORT, 
            myapp()->frame->bitmaps[15] ,
            txt("b_export_primer") ) ;
    toolBar->AddSeparator() ;
    toolBar->AddTool( MDI_EDIT_MODE,
        myapp()->frame->bitmaps[13] ,
        myapp()->frame->bitmaps[13] ,
        TRUE, -1, -1, (wxObject *) NULL, txt("m_edit_mode") ) ;
    toolBar->AddTool( MDI_TOGGLE_FEATURES,
        myapp()->frame->bitmaps[10] ,
        myapp()->frame->bitmaps[10] ,
        TRUE, -1, -1, (wxObject *) NULL, txt("m_display_features") ) ;
    
    toolBar->AddSeparator() ;
    
    spin = new wxSpinCtrl ( toolBar , PCR_SPIN ) ;
    spin->SetRange ( 1 , vec->sequence.length() ) ;
    spin->SetValue ( wxString::Format("%d",vec->sequence.length()) ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_pcr_spin_1") ) ) ;
    toolBar->AddControl ( spin ) ;
    toolBar->AddControl ( new wxStaticText ( toolBar , -1 , txt("t_pcr_spin_2") ) ) ;
    spin->SetSize ( -1 , -1 , 50 , -1 , wxSIZE_USE_EXISTING ) ;
    toolBar->Realize() ;
#else
#endif

    guessOptNuc () ;

    int w , h ;
    Maximize () ;
#ifdef __WXMSW__
    GetClientSize ( &w , &h ) ;
#else
    GetParent()->GetClientSize( &w , &h ) ;
#endif

    // Upper panel
    up = new wxPanel ( hs , -1 , wxDefaultPosition , wxSize ( w , 100 ) ) ;

    up->GetClientSize ( &w , &h ) ;
    
    hs->SplitHorizontally ( up , sc ,h+bo ) ;
    hs->SetMinimumPaneSize ( h+bo ) ;
    
    lc = new wxListCtrl ( up , PD_LC , wxPoint ( 0 , 0 ) , wxSize ( w/3 , h ) , wxLC_REPORT|wxLC_SINGLE_SEL ) ;

    new wxButton ( up , PD_EDIT , txt("m_edit") , wxPoint ( w/3 + 5 , 20 ) , wxSize ( 60 , 20 ) ) ;
    new wxButton ( up , PD_DEL , txt("b_del") , wxPoint ( w/3 + 5 , 50 ) , wxSize ( 60 , 20 ) ) ;

    stat = new wxTextCtrl ( up ,
                            -1 ,
                            "" ,
                            wxPoint ( w/3 + 70 , 0 ) ,
                            wxSize ( w*2/3 - 70 , h ) ,
                            wxTE_MULTILINE | wxTE_READONLY ) ;
                            
    stat->SetFont ( *MYFONT ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ) ;

#ifdef __WXMSW__ // LINUX
    GetToolBar()->ToggleTool(MDI_TOGGLE_FEATURES,show_features);
#endif

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( hs , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;



    showSequence () ;
    updatePrimersFromSequence() ;
    sc->SetFocus() ;
    myapp()->frame->setChild ( this ) ;
    spinTextEnabeled = true ;
    }
    
void TPrimerDesign::OnEditPrimer ( wxCommandEvent &ev )
    {
    if ( lastPrimerActivated == -1 ) return ;
    if ( primer.size() == 0 ) return ;
    TPrimerDialog pd ( (wxWindow*) this , "" ) ;
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
/*    string s = cPlasmid->getSelection () ;
    if ( s == "" ) return ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s.c_str()) );
        wxTheClipboard->Close();
        }*/
    }

string TPrimerDesign::getName ()
    {
    return vec->name ;
    }

void TPrimerDesign::showSequence ()
    {
    // Cleaning up
    while ( sc->seq.size() )
        {
        delete sc->seq[sc->seq.size()-1] ;
        sc->seq.pop_back () ;
        }

    // Display
    int a , b ;
    sc->blankline = 0 ;
    vc->sequence = vec->transformSequence ( true , false ) ;
    vc->setCircular ( vec->isCircular() ) ;
    
    SeqFeature *seqF ;
    if ( show_features )
       {
       seqF = new SeqFeature ( sc ) ;
       seqF->initFromTVector ( vec ) ;
       }
    
    SeqPrimer *p1 = new SeqPrimer ( sc ) ;
    p1->initFromTVector ( vec ) ;
    p1->takesMouseActions = true ;
    p1->myname = "PRIMER_UP" ;
    p1->alternateName = "5'P" ;
    for ( a = 0 ; a < primer.size() ; a++ )
        if ( primer[a].upper )
           p1->addPrimer ( &primer[a] ) ;

    SeqDNA *s1 = new SeqDNA ( sc ) ;
    s1->initFromTVector ( vec ) ;
    s1->takesMouseActions = false ;
    
    SeqAA *a1 = new SeqAA ( sc ) ;
    a1->mode = aa_state ;
    a1->disp = aa_disp ;
    a1->initFromTVector ( vec ) ;
    a1->takesMouseActions = false ;

    SeqDNA *s2 = new SeqDNA ( sc ) ;
    s2->initFromTVector ( vc ) ;
    s2->showNumbers = false ;
    s2->alternateName = "3'" ;
    s2->takesMouseActions = false ;

    SeqPrimer *p2 = new SeqPrimer ( sc ) ;
    p2->initFromTVector ( vc ) ;
    p2->takesMouseActions = true ;
    p2->myname = "PRIMER_DOWN" ;
    p2->alternateName = "3'P" ;
    for ( a = 0 ; a < primer.size() ; a++ )
        if ( !primer[a].upper )
           p2->addPrimer ( &primer[a] ) ;

    if ( show_features ) sc->seq.push_back ( seqF ) ;
    sc->seq.push_back ( p1 ) ;
    sc->seq.push_back ( s1 ) ;
    sc->seq.push_back ( a1 ) ;
    sc->seq.push_back ( s2 ) ;
    sc->seq.push_back ( p2 ) ;
    updateResultSequence() ;
        
    sc->arrange () ;
    sc->Refresh ( false ) ;    
    updatePrimerStats () ;
    }
    
void TPrimerDesign::calculateResultSequence()
    {
    int a , b ;
    
    // Constructing result vector
    w->setFromVector ( *vec ) ;
    
    int nuc = spin->GetValue() ;
    for ( a = 0 ; a < w->sequence.length() ; a++ ) w->sequence[a] = ' ' ;
    TVector w2 ;
    w2.setFromVector ( *w ) ;
    
    for ( a = 0 ; a < primer.size() ; a++ )
        {
        TVector *wp = primer[a].upper ? w : &w2 ;
        int factor = primer[a].upper ? 1 : -1 ;
        int start = primer[a].upper ? primer[a].to : primer[a].from ;
        for ( b = 0 ; b < nuc ; b++ )
           {
           int pos = b * factor + start ;
           wp->setNucleotide ( pos , vec->getNucleotide ( pos ) ) ;
           }
        }
        
    for ( a = 0 ; a < w->sequence.length() ; a++ )
        w->sequence[a] = ( w->sequence[a] != ' ' && w2.sequence[a] != ' ' ) 
                                ? vec->sequence[a] : ' ' ;
       
    // Overwriting result sequence with primer sequences
    for ( a = 0 ; a < w->sequence.length() ; a++ )
        {
        char t = sc->seq[show_features]->s[a] ;
        if ( t == ' ' )
           {
           t = sc->seq[4+show_features]->s[a] ;
           if ( t != ' ' ) t = w->getComplement ( t ) ;
           }
        if ( t != ' ' ) w->setNucleotide ( a , t ) ;
        }
    
    w->recalculateCuts() ;
    }
    
void TPrimerDesign::updateResultSequence()
    {
    calculateResultSequence() ;
    
    SeqRestriction *r3 = new SeqRestriction ( sc ) ;
    r3->initFromTVector ( w ) ;
    r3->down = true ;
    r3->takesMouseActions = false ;

    SeqDNA *s3 = new SeqDNA ( sc ) ;
    s3->initFromTVector ( w ) ;
    s3->showNumbers = false ;
    s3->fontColor.Set ( 0 , 100 , 0 ) ;
    s3->takesMouseActions = false ;
    s3->alternateName = "RES" ;
    
    SeqAA *a3 = new SeqAA ( sc ) ;
    a3->mode = aa_state ;
    a3->disp = aa_disp ;
    a3->unknownAA = ' ' ;
    a3->initFromTVector ( w ) ;
    a3->takesMouseActions = false ;
    a3->showNumbers = false ;

    SeqDivider *div = new SeqDivider ( sc ) ;
    div->initFromTVector ( vec ) ;    

    sc->seq.push_back ( r3 ) ;
    sc->seq.push_back ( s3 ) ;
    sc->seq.push_back ( a3 ) ;
    sc->seq.push_back ( div ) ;
    }

void TPrimerDesign::OnAA_setit(int mode)
    {
    int oldto = sc->_to , oldfrom = sc->_from , lastmarked = sc->lastmarked ;
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
        sc->seq.push_back ( NULL ) ;
        for ( int a = sc->seq.size()-1 ; a >= 1 ; a-- )
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
        string s = sc->seq[lastmarked]->whatsthis() ;
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
    if ( to >= vec->sequence.length() ) to -= vec->sequence.length() ;
    string p ;
    if ( primer[i].upper ) p = "PRIMER_UP" ;
    else p = "PRIMER_DOWN" ;
    sc->mark ( p , from , to ) ;
    sc->Scroll ( 0 , sc->getBatchMark() ) ;
    sc->SetFocus () ;
    }

void TPrimerDesign::OnSelectPrimer ( wxListEvent& event)
    {
    lastPrimerActivated = event.GetIndex () ;
    string x = primer[lastPrimerActivated].report() ;
    stat->SetValue ( x.c_str() ) ;
    sc->SetFocus () ;
    }
    
void TPrimerDesign::OnActivatePrimer ( wxListEvent& event)
    {
    lastPrimerActivated = event.GetIndex () ;
    doShowPrimer ( lastPrimerActivated ) ;
    }

void TPrimerDesign::OnSilmut ( wxCommandEvent& event)
    {
    TSilmutDialog sd ( this , txt("t_silmut") ) ;
    sd.initme ( w , sc->_from , sc->_to ) ;
    if ( wxID_OK != sd.ShowModal () ) return ;
    string ns = sd.getSequence().c_str() ;
    if ( ns == "" ) return ;
    
    TVector z ;
    z.sequence = ns ;
    string nt = z.transformSequence ( true , false ) ;
    TRestrictionEnzyme *e = sd.getEnzyme() ;
    int a , b ;
    for ( a = 0 ; a < ns.length() ; a++ )
        {
        if ( ns[a] >= 'A' && ns[a] <= 'Z' )
           {
           b = a + sc->_from - 1 ;
           if ( sc->seq[show_features]->s[b] != ' ' ) sc->seq[show_features]->s[b] = ns[a] ;
           if ( sc->seq[4+show_features]->s[b] != ' ' ) sc->seq[4+show_features]->s[b] = nt[a] ;
           }
        }
    for ( a = 0 ; a < w->re.size() && w->re[a] != e ; a++ ) ;
    if ( a == w->re.size() ) w->re.push_back ( e ) ;
    updatePrimersFromSequence () ;
    showSequence () ;
    }
    
void TPrimerDesign::OnToggleFeatures ( wxCommandEvent &ev )
    {
    show_features = sc->findID("FEATURE")?0:1 ;
    showSequence () ;    
#ifdef __WXMSW__ // LINUX
    GetToolBar()->ToggleTool(MDI_TOGGLE_FEATURES,show_features);
#endif
    }
    
void TPrimerDesign::OnSpin(wxSpinEvent& event)
    {
    showSequence() ;
    }
    
void TPrimerDesign::OnSpinText(wxCommandEvent& event)
    {
    if ( spinTextEnabeled ) showSequence() ;
    }
    
