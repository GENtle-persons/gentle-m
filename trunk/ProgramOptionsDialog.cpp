/** \file
	\brief Contains the members of the ProgramOptionsDialog class and its helper classes, TEnzymeSettingsTab (which is also used in TVectorEditor) and TEnzymeRules
*/
#include "ProgramOptionsDialog.h"

BEGIN_EVENT_TABLE(ProgramOptionsDialog, wxDialog )
    EVT_BUTTON(POD_OK,ProgramOptionsDialog::OnOK)
    EVT_BUTTON(POD_CANCEL,ProgramOptionsDialog::OnCancel)
    EVT_BUTTON(POD_AA_COL,ProgramOptionsDialog::OnAACol)
    EVT_CHAR_HOOK(ProgramOptionsDialog::OnCharHook)
END_EVENT_TABLE()
    
BEGIN_EVENT_TABLE(TEnzymeSettingsTab, wxPanel )
    EVT_CHECKBOX(GES_USE,TEnzymeSettingsTab::OnEnzymeCheckbox)
    EVT_CHECKBOX(GES_USE_MINCUTOFF,TEnzymeSettingsTab::OnEnzymeCheckbox)
    EVT_CHECKBOX(GES_USE_MAXCUTOFF,TEnzymeSettingsTab::OnEnzymeCheckbox)
    EVT_CHECKBOX(GES_USE_COLOR_CODING,TEnzymeSettingsTab::OnEnzymeCheckbox)
    EVT_BUTTON(GES_COL_1,TEnzymeSettingsTab::OnButton1)
    EVT_BUTTON(GES_COL_2,TEnzymeSettingsTab::OnButton2)
    EVT_BUTTON(GES_COL_3,TEnzymeSettingsTab::OnButton3)
END_EVENT_TABLE()


TEnzymeSettingsTab::TEnzymeSettingsTab ( wxWindow *parent , int _mode )
	: wxPanel ( parent )
	{
	mode = _mode ;
	int w , h ;
    GetClientSize ( &w , &h ) ;
    
    wxBoxSizer *vs = new wxBoxSizer ( wxVERTICAL ) ;
    wxFlexGridSizer *topSizer = new wxFlexGridSizer ( 2 , 15 , 5 ) ;
    optionsSizer = new wxFlexGridSizer ( 4 , 15 , 5 ) ;    
    
    // Top
    wxString t = txt("t_use_global_enzyme_settings") ; // EST_GLOBAL
    if ( mode == EST_PROJECT ) t = txt("t_use_project_enzyme_settings") ;
    if ( mode == EST_SINGLE ) t = txt("t_use_single_enzyme_settings") ;
    useSettings = new wxCheckBox ( this , GES_USE , t ) ; 
    join_enzymes = new wxCheckBox ( this , -1 , txt("t_ges_join_enzymes") ) ;    
    topSizer->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    topSizer->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    topSizer->Add ( useSettings , 1 , wxALIGN_CENTER_VERTICAL ) ;
    topSizer->Add ( join_enzymes , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    vs->Add ( topSizer , 0 , wxEXPAND , 5 ) ;
    vs->Add ( new wxStaticText ( this , -1 , _T(" ") ) , 0 , wxEXPAND , 5 ) ;
    vs->Add ( optionsSizer , 1 , wxEXPAND , 5 ) ;
    
    // Min/max cutoff
    useMinCutoff = new wxCheckBox ( this , GES_USE_MINCUTOFF , txt("t_ges_use_min_cutoff") ) ;
    minCutoff = new wxSpinCtrl ( this , -1 , _T("1")  , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , 30 ) ) ;
    useMaxCutoff = new wxCheckBox ( this , GES_USE_MAXCUTOFF , txt("t_ges_use_max_cutoff") ) ;
    maxCutoff = new wxSpinCtrl ( this , -1 , _T("3")  , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , 30 ) ) ;
    
    // Length of recognition sequence
    recog4 = new wxCheckBox ( this , -1 , txt("t_ges_seqlen4") ) ;
    recog5 = new wxCheckBox ( this , -1 , txt("t_ges_seqlen5") ) ;
    recog6 = new wxCheckBox ( this , -1 , txt("t_ges_seqlen6") ) ;
    recog6p = new wxCheckBox ( this , -1 , txt("t_ges_seqlen6p") ) ;

    // Overlaps
    pattern3 = new wxCheckBox ( this , -1 , txt("t_ges_overlap3") ) ;
    pattern5 = new wxCheckBox ( this , -1 , txt("t_ges_overlap5") ) ;
    pattern_blunt = new wxCheckBox ( this , -1 , txt("t_ges_overlap_blunt") ) ;
    
    // Group list
    default_group = new wxChoice ( this , -1 ) ;
    default_group->Append ( txt("All") ) ;
    wxArrayString vs2 ;
    myapp()->frame->LS->getEnzymeGroups ( vs2 ) ;
    for ( int i = 0 ; i < vs2.GetCount() ; i++ )
        default_group->Append ( vs2[i] ) ;
    default_group->SetStringSelection ( txt("All") ) ;
    
    // Colors
    use_color_coding = new wxCheckBox ( this , GES_USE_COLOR_CODING , txt("t_ges_use_color_coding") ) ;
    bcol1 = new wxButton ( this , GES_COL_1 , txt("t_ges_cut1") ) ;
    bcol2 = new wxButton ( this , GES_COL_2 , txt("t_ges_cut2") ) ;
    bcol3 = new wxButton ( this , GES_COL_3 , txt("t_ges_cut3") ) ;
    
    // Methylation
    met_dam = new wxCheckBox ( this , -1 , txt("t_ges_met_dam") ) ;
    met_dcm = new wxCheckBox ( this , -1 , txt("t_ges_met_dcm") ) ;
    
    // GC%
    showgc = new wxCheckBox ( this , -1 , txt("t_ges_showgc") ) ;
    
    // Adding elements to gridsizer
    optionsSizer->Add ( use_color_coding , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( bcol1 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( bcol2 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( bcol3 , 1 , wxALIGN_CENTER_VERTICAL ) ;

    optionsSizer->Add ( useMinCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( minCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( useMaxCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( maxCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    optionsSizer->Add ( new wxStaticText ( this , -1 , txt("t_ges_overlaps") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( pattern3 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( pattern5 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( pattern_blunt , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    optionsSizer->Add ( recog4 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( recog5 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( recog6 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( recog6p , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    optionsSizer->Add ( new wxStaticText ( this , -1 , txt("t_ges_enzyme_group") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( default_group , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;

    optionsSizer->Add ( new wxStaticText ( this , -1 , txt("t_ges_met") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( met_dam , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( met_dcm , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;

    optionsSizer->Add ( showgc , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    // Do it!
    this->SetSizer ( vs ) ;
    vs->Fit ( this ) ;    
    this->Show () ;
    updateGlobalEnzymes () ;
	vs->Fit ( this ) ;
	}    

void TEnzymeSettingsTab::updateGlobalEnzymes ()
	{
	bool use = useSettings->GetValue() ;
	useMinCutoff->Enable ( use ) ;
	useMaxCutoff->Enable ( use ) ;
	minCutoff->Enable ( useMinCutoff->GetValue() && use ) ;
	maxCutoff->Enable ( useMaxCutoff->GetValue() && use ) ;
	recog4->Enable ( use ) ;
	recog5->Enable ( use ) ;
	recog6->Enable ( use ) ;
	recog6p->Enable ( use ) ;
	pattern3->Enable ( use ) ;
	pattern5->Enable ( use ) ;
	pattern_blunt->Enable ( use ) ;
	default_group->Enable ( use ) ;
	if ( mode != EST_GLOBAL )
		{
		join_enzymes->Enable ( use ) ;
		use_color_coding->Enable ( use ) ;
		}    
	updateColorButton ( bcol1 , col1 ) ;
	updateColorButton ( bcol2 , col2 ) ;
	updateColorButton ( bcol3 , col3 ) ;
	}

void TEnzymeSettingsTab::updateColorButton ( wxButton *b , wxColour &c )
	{
	b->SetForegroundColour ( c ) ;
	if ( ( c.Red() + c.Green() + c.Blue() ) / 3 < 230 ) b->SetBackgroundColour ( *wxWHITE ) ;
	else b->SetBackgroundColour ( *wxBLACK ) ;
	if ( mode == EST_GLOBAL ) b->Enable ( use_color_coding->GetValue() ) ;
 	else b->Enable ( use_color_coding->GetValue() && useSettings->GetValue() ) ;
	}    
	
void TEnzymeSettingsTab::OnEnzymeCheckbox ( wxCommandEvent &event )
	{
	updateGlobalEnzymes () ;
	}    

void TEnzymeSettingsTab::OnButton1 ( wxCommandEvent &event )
	{
	updateColor ( col1 ) ;
	}    

void TEnzymeSettingsTab::OnButton2 ( wxCommandEvent &event )
	{
	updateColor ( col2 ) ;
	}    

void TEnzymeSettingsTab::OnButton3 ( wxCommandEvent &event )
	{
	updateColor ( col3 ) ;
	}    

void TEnzymeSettingsTab::updateColor ( wxColour &c )
	{
	wxColour c2 = wxGetColourFromUser ( this , c ) ;
	if ( !c2.Ok() ) return ;
	c = c2 ;
	updateGlobalEnzymes () ;
	}    
    

// ****************************************

ProgramOptionsDialog::ProgramOptionsDialog(wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 620 , 450 ) )
    {
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
	
    globalSettingsPanel = NULL ;
    nb = new wxNotebook ( (wxWindow*) this , -1 ) ;
	wxNotebookSizer *nbs = new wxNotebookSizer ( nb ) ;

    initGlobalSettings () ;
    initGlobalEnzymes () ;

    wxButton *OK = new wxButton ( this , POD_OK , txt("b_ok") ) ;
    wxButton *CANCEL = new wxButton ( this , POD_CANCEL , txt("b_cancel") ) ;

	h0->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , 0 ) ;
	h0->Add ( OK , 1 , 0 ) ;
	h0->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , 0 ) ;
	h0->Add ( CANCEL , 1 , 0 ) ;
	h0->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , 0 ) ;

	v0->Add ( nbs , 1 , wxALL|wxEXPAND , 5 ) ;
	v0->Add ( h0 , 0 , wxALL|wxEXPAND , 5 ) ;

	SetSizer ( v0 ) ;
	v0->Fit ( this ) ;

    Center () ;
    OK->SetDefault () ;
    OK->SetFocus() ;
    }
    
void ProgramOptionsDialog::initGlobalEnzymes ()
	{
    globalEnzymesPanel = new TEnzymeSettingsTab ( nb ) ;
    nb->AddPage ( globalEnzymesPanel , txt("t_global_enzymes") ) ;    
    }    
    
void ProgramOptionsDialog::initGlobalSettings ()
	{
    globalSettingsPanel = new wxPanel ( nb , -1 ) ;
    nb->AddPage ( globalSettingsPanel , txt("t_global_settings") ) ;    

    language = new wxChoice ( globalSettingsPanel , -1 ) ;
    enhancedDisplay = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_enhanced_display") ) ; 
    vectorTitle = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_vector_title") ) ;
    vectorLength = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_vector_length") ) ;
    loadLastProject = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_load_last_project") ) ;
    useMetafile = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_use_metafile") ) ;
    showSplashScreen = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_show_splashscreen") ) ;
    checkUpdate = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_check4update") ) ;
    useInternalHelp = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_useinternalhelp") ) ; 
    doRegisterStuff = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_doregisterstuff") ) ; 
    showEnzymePos = new wxCheckBox ( globalSettingsPanel , -1 ,
                        txt("t_showenzymepos") ) ;
    showTips = new wxCheckBox ( globalSettingsPanel , -1 ,
                        txt("t_show_tips") ) ;
    use_nonstandard_translation_table = new wxCheckBox ( globalSettingsPanel , -1 ,
                        txt("t_use_nonstandard_translation_table") ) ;
    nonstandard_translation_table = new wxChoice ( globalSettingsPanel , -1 ) ;
                        
                        
    wxString efm[3] ;
    efm[0] = txt("t_editfeaturemode_1") ;
    efm[1] = txt("t_editfeaturemode_2") ;
    efm[2] = txt("t_editfeaturemode_3") ;
    editFeatureMode = new wxRadioBox ( globalSettingsPanel , -1 , 
                        txt("t_editfeaturemode") , 
                        wxDefaultPosition , 
                        wxDefaultSize ,
                        3 , efm , 1 , wxRA_SPECIFY_ROWS ) ;
    editFeatureMode->SetSelection ( myapp()->frame->editFeatureMode ) ;

    wxString ssc[2] ;
    ssc[0] = _T("|") ;
    ssc[1] = _T("*") ;
    showStopCodon = new wxRadioBox ( globalSettingsPanel , -1 , 
                        txt("t_show_stop_codon") , 
                        wxDefaultPosition , 
                        wxDefaultSize ,
                        2 , ssc , 1 , wxRA_SPECIFY_ROWS ) ;
    showStopCodon->SetSelection ( myapp()->frame->showStopCodon ) ;
    wxButton *b_aacol = new wxButton ( globalSettingsPanel , POD_AA_COL , txt("t_aa_col") ) ;
                        
    enhancedDisplay->SetValue ( myapp()->frame->enhancedRefresh ) ;
    vectorTitle->SetValue ( myapp()->frame->showVectorTitle ) ;
    vectorLength->SetValue ( myapp()->frame->showVectorLength ) ;
    loadLastProject->SetValue ( myapp()->frame->loadLastProject ) ;
    useMetafile->SetValue ( myapp()->frame->useMetafile ) ;
    showSplashScreen->SetValue ( myapp()->frame->showSplashScreen ) ;
    checkUpdate->SetValue ( myapp()->frame->checkUpdate ) ;
    useInternalHelp->SetValue ( myapp()->frame->useInternalHelp ) ;
    doRegisterStuff->SetValue ( myapp()->frame->doRegisterStuff ) ;
    showEnzymePos->SetValue ( myapp()->frame->showEnzymePos ) ;
    showTips->SetValue ( myapp()->frame->LS->getOption ( _T("SHOWTIP") , true ) ) ;
    use_nonstandard_translation_table->SetValue ( myapp()->frame->nonstandard_translation_table != -1 ) ;
    language->Append ( _T("en") ) ;
    language->Append ( _T("de") ) ;
    language->SetStringSelection ( myapp()->frame->lang_string ) ;
    aacol = myapp()->frame->aa_color ;
    
    TVector dummy ;
    int tt_mark = 0 ;
    for ( int a = 0 ; a < dummy.countCodonTables() ; a++ )
    	{
		if ( a != 1 && dummy.getCodonTableName(a) == dummy.getCodonTableName(1) ) continue ;
		translation_tables.push_back ( a ) ;
    	nonstandard_translation_table->Append ( dummy.getCodonTableName(a) ) ;
    	if ( myapp()->frame->nonstandard_translation_table == a )
    		tt_mark = translation_tables.size()-1 ;
		}
    if ( myapp()->frame->nonstandard_translation_table != -1 )
    	nonstandard_translation_table->SetSelection ( tt_mark ) ;
    else nonstandard_translation_table->SetSelection ( 0 ) ;

    wxBoxSizer *v = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    h->Add ( new wxStaticText ( globalSettingsPanel , -1 , txt("t_language") ) , 0 , wxEXPAND|wxALL , 5 ) ;
    h->Add ( language , 0 , wxEXPAND|wxALL , 5 ) ;

    h1->Add ( showStopCodon , 1 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( b_aacol, 0 , wxEXPAND|wxALL , 5 ) ;

	 h2->Add ( use_nonstandard_translation_table , 0 , wxEXPAND|wxALL , 5 ) ; 
	 h2->Add ( nonstandard_translation_table , 0 , wxEXPAND|wxALL , 5 ) ; 
	 
    v->Add ( h , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( enhancedDisplay , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( vectorTitle , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( vectorLength , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( loadLastProject , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( useMetafile , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( showSplashScreen , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( checkUpdate , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( useInternalHelp , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( doRegisterStuff , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( showEnzymePos , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( showTips , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( editFeatureMode , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( h1 , 0 , wxEXPAND|wxALL , 5 ) ;
    v->Add ( h2 , 0 , wxEXPAND|wxALL , 5 ) ;

	 globalSettingsPanel->SetSizer ( v ) ;
	 v->Fit ( globalSettingsPanel ) ;
    }

void ProgramOptionsDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void ProgramOptionsDialog::OnAACol ( wxCommandEvent &ev )
	{
	wxColour n = wxGetColourFromUser ( this , aacol ) ;
	if ( !n.Ok() ) return ; // Not OK
	aacol = n ;
	}

void ProgramOptionsDialog::OnOK ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }

void ProgramOptionsDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }


//************************************************************************

void TEnzymeRules::init () // Default settings
	{
	useit = true ;
	min_cutoff = 1 ;
 	max_cutoff = 3 ;
	use_min_cutoff = true ;
 	use_max_cutoff = true ;
	recog4 = false ;
	recog5 = false ;
	recog6 = true ;
 	recog_longer = false ;
	pattern3 = false ;
	pattern5  = true ;
	pattern_blunt = false ;
	default_group = txt("All") ;
	col1.Set ( 200 , 0 , 0 ) ;
	col2.Set ( 0 , 200 , 0 ) ;
	col3.Set ( 0 , 0 , 200 ) ;
	join_enzymes = true ;
	use_color_coding = true ;
	methylation = 0 ;
	showgc = false ;
	}    

void TEnzymeRules::load_global_settings ()
	{
	init () ;
    wxString s = myapp()->frame->LS->getOption ( _T("GLOBALENZYMESETTINGS") , _T("") ) ;
    from_string ( s ) ;
	}    

void TEnzymeRules::save_global_settings ()
	{
    myapp()->frame->LS->setOption ( _T("GLOBALENZYMESETTINGS") , to_string() ) ;
	}    

void TEnzymeRules::setup_options ( TEnzymeSettingsTab *est )
	{
	est->useSettings->SetValue ( useit ) ;
	est->minCutoff->SetValue ( min_cutoff ) ;
	est->maxCutoff->SetValue ( max_cutoff ) ;
	est->useMinCutoff->SetValue ( use_min_cutoff ) ;
	est->useMaxCutoff->SetValue ( use_max_cutoff ) ;
	est->recog4->SetValue ( recog4 ) ;
	est->recog5->SetValue ( recog5 ) ;
	est->recog6->SetValue ( recog6 ) ;
	est->recog6p->SetValue ( recog_longer ) ;
	est->pattern3->SetValue ( pattern3 ) ;
	est->pattern5->SetValue ( pattern5 ) ;
	est->pattern_blunt->SetValue ( pattern_blunt ) ;
	if ( default_group != _T("") ) est->default_group->SetStringSelection ( default_group ) ;
	est->col1 = col1 ;
	est->col2 = col2 ;
	est->col3 = col3 ;
	est->join_enzymes->SetValue ( join_enzymes ) ;
	est->use_color_coding->SetValue ( use_color_coding ) ;
	est->met_dam->SetValue ( ( methylation & DAM_METHYLATION ) > 0 ) ;
	est->met_dcm->SetValue ( ( methylation & DCM_METHYLATION ) > 0 ) ;
	est->showgc->SetValue ( showgc ) ;
	est->updateGlobalEnzymes () ;
	}    

void TEnzymeRules::lookup_options ( TEnzymeSettingsTab *est )
	{
	useit = est->useSettings->GetValue() ;
	min_cutoff = est->minCutoff->GetValue() ;
	max_cutoff = est->maxCutoff->GetValue() ;
	use_min_cutoff = est->useMinCutoff->GetValue() ;
	use_max_cutoff = est->useMaxCutoff->GetValue() ;
	recog4 = est->recog4->GetValue() ;
	recog5 = est->recog5->GetValue() ;
	recog6 = est->recog6->GetValue() ;
	recog_longer = est->recog6p->GetValue() ;
	pattern3 = est->pattern3->GetValue() ;
	pattern5 = est->pattern5->GetValue() ;
	pattern_blunt = est->pattern_blunt->GetValue() ;
	default_group = est->default_group->GetStringSelection() ;
	col1 = est->col1 ;
	col2 = est->col2 ;
	col3 = est->col3 ;
	join_enzymes = est->join_enzymes->GetValue() ;
	use_color_coding = est->use_color_coding->GetValue() ;
	showgc = est->showgc->GetValue() ;
	
	methylation = 0 ;
	if ( est->met_dam->GetValue() ) methylation += DAM_METHYLATION ;
	if ( est->met_dcm->GetValue() ) methylation += DCM_METHYLATION ;
	}
	
bool TEnzymeRules::isEqual ( TEnzymeRules &r )
	{
 	return to_string() == r.to_string() ;
	}    
     
wxString TEnzymeRules::to_string ()
	{
	wxString ret ;
	ret += wxString::Format ( _T("useit=%d\r") , useit ) ;
	ret += wxString::Format ( _T("min_cutoff=%d\r") , min_cutoff ) ;
	ret += wxString::Format ( _T("max_cutoff=%d\r") , max_cutoff ) ;
	ret += wxString::Format ( _T("use_min_cutoff=%d\r") , use_min_cutoff ) ;
	ret += wxString::Format ( _T("use_max_cutoff=%d\r") , use_max_cutoff ) ;
	ret += wxString::Format ( _T("recog4=%d\r") , recog4 ) ;
	ret += wxString::Format ( _T("recog5=%d\r") , recog5 ) ;
	ret += wxString::Format ( _T("recog6=%d\r") , recog6 ) ;
	ret += wxString::Format ( _T("recog_longer=%d\r") , recog_longer ) ;
	ret += wxString::Format ( _T("pattern3=%d\r") , pattern3 ) ;
	ret += wxString::Format ( _T("pattern5=%d\r") , pattern5 ) ;
	ret += wxString::Format ( _T("pattern_blunt=%d\r") , pattern_blunt ) ;
	ret += wxString::Format ( _T("default_group=%s\r") , default_group.c_str() ) ;
	ret += wxString::Format ( _T("col1=%d,%d,%d\r") , col1.Red() , col1.Green() , col1.Blue() ) ;
	ret += wxString::Format ( _T("col2=%d,%d,%d\r") , col2.Red() , col2.Green() , col2.Blue() ) ;
	ret += wxString::Format ( _T("col3=%d,%d,%d\r") , col3.Red() , col3.Green() , col3.Blue() ) ;
	ret += wxString::Format ( _T("join_enzymes=%d\r") , join_enzymes ) ;
	ret += wxString::Format ( _T("use_color_coding=%d\r") , use_color_coding ) ;
	ret += wxString::Format ( _T("methylation=%d\r") , methylation ) ;
	ret += wxString::Format ( _T("showgc=%d\r") , showgc ) ;
	return ret ;
	}    

void TEnzymeRules::from_string ( wxString &s )
	{
	init () ;
	wxArrayString as ;
	explode ( _T("\r") , s , as ) ;
	for ( int a = 0 ; a < as.GetCount() ; a++ )
		{
		if ( as[a] == _T("") ) continue ;
		wxString key = as[a].BeforeFirst ( '=' ) ;
		wxString val = as[a].AfterFirst ( '=' ) ;
		long l ;
		val.ToLong ( &l ) ;
		if ( key == _T("useit") ) useit = l ;
		else if ( key == _T("min_cutoff") ) min_cutoff = l ;
		else if ( key == _T("max_cutoff") ) max_cutoff = l ;
		else if ( key == _T("use_min_cutoff") ) use_min_cutoff = l ;
		else if ( key == _T("use_max_cutoff") ) use_max_cutoff = l ;
		else if ( key == _T("recog4") ) recog4 = l ;
		else if ( key == _T("recog5") ) recog5 = l ;
		else if ( key == _T("recog6") ) recog6 = l ;
		else if ( key == _T("recog_longer") ) recog_longer = l ;
		else if ( key == _T("pattern3") ) pattern3 = l ;
		else if ( key == _T("pattern5") ) pattern5 = l ;
		else if ( key == _T("pattern_blunt") ) pattern_blunt = l ;
		else if ( key == _T("default_group") ) default_group = val ;
		else if ( key == _T("col1") ) col1 = scan_color ( val ) ;
		else if ( key == _T("col2") ) col2 = scan_color ( val ) ;
		else if ( key == _T("col3") ) col3 = scan_color ( val ) ;
		else if ( key == _T("join_enzymes") ) join_enzymes = l ;
		else if ( key == _T("use_color_coding") ) use_color_coding = l ;
		else if ( key == _T("methylation") ) methylation = l ;
		else if ( key == _T("showgc") ) showgc = l ;
		}    
	}    

wxColour TEnzymeRules::scan_color ( wxString s )
	{
	wxString c1 = s.BeforeFirst ( ',' ) ;
	s = s.AfterFirst ( ',' ) ;
	wxString c2 = s.BeforeFirst ( ',' ) ;
	wxString c3 = s.AfterFirst ( ',' ) ;
	return wxColour ( atoi(c1.mb_str()) , atoi(c2.mb_str()) , atoi(c3.mb_str()) ) ;
	}

// Here we go...

wxColour *TEnzymeRules::getColor ( int cuts )
	{
	if ( !use_color_coding || cuts == 0 || cuts > 3 ) return wxBLACK ;
	if ( cuts == 1 ) return &col1 ;
	if ( cuts == 2 ) return &col2 ;
	if ( cuts == 3 ) return &col3 ;
	return &col1 ; // Dummy
	}    

void TEnzymeRules::getVectorCuts ( TVector *v )
{
    int a , b ;
    for ( a = 0 ; a < v->re.GetCount() ; a++ )
	v->getCuts ( v->re[a] , v->rc , false ) ;
    if ( !useit ) return ;

    // Getting the default list of enzymes
    wxArrayTRestrictionEnzyme ve ;
    wxArrayString vs ;
    myapp()->frame->LS->getEnzymesInGroup ( default_group , vs ) ;
    ve.Alloc ( vs.GetCount() ) ;
    
    // Removing the ones we already did because they were manually selected
    for ( a = 0 ; a < v->re.GetCount() ; a++ )
    {
	if ( v->re[a]->name.IsEmpty() ) continue ; // Avoid endless loop
	int i = vs.Index ( v->re[a]->name ) ;
	while ( i != wxNOT_FOUND )
	{
	    vs[i] = _T("") ;
	    i = vs.Index ( v->re[a]->name ) ;
	}    
    }    

    // Eliminating from properties
    for ( a = 0 ; a < vs.GetCount() ; a++ )
    {
	if ( vs[a].IsEmpty() ) continue ; // Was removed earlier
	TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( vs[a] ) ;
	bool keep = true ;
	if ( keep && !pattern3 && e->overlap < 0 ) keep = false ;
	if ( keep && !pattern5 && e->overlap > 0 ) keep = false ;
	if ( keep && !pattern_blunt && e->overlap == 0 ) keep = false ;
	if ( keep && !recog4 && e->sequence.length() == 4 ) keep = false ;
	if ( keep && !recog5 && e->sequence.length() == 5 ) keep = false ;
	if ( keep && !recog6 && e->sequence.length() == 6 ) keep = false ;
	if ( keep && !recog_longer && e->sequence.length() > 6 ) keep = false ;
	if ( keep ) ve.Add ( e ) ;
    }    
    
    // Add what has the correct number of cuts
    v->re2.Clear () ; // The list of used enzymes that were *not* added manually
    int max = 10000000 ; // Ridiculously high number
    if ( use_max_cutoff ) max = max_cutoff ;
    for ( a = 0 ; a < ve.GetCount() ; a++ )
    {
	vector <TRestrictionCut> vc ;
	v->getCuts ( ve[a] , vc , false , max ) ;
	if ( ( !use_min_cutoff || min_cutoff <= vc.size() ) &&
	     ( !use_max_cutoff || max_cutoff >= vc.size() ) )
	{
	    v->re2.Add ( ve[a] ) ;
	    for ( b = 0 ; b < vc.size() ; b++ ) v->rc.push_back ( vc[b] ) ;
	}    
    }    
}

