#include "ProgramOptionsDialog.h"

BEGIN_EVENT_TABLE(ProgramOptionsDialog, wxDialog )
    EVT_BUTTON(POD_OK,ProgramOptionsDialog::OnOK)
    EVT_BUTTON(POD_CANCEL,ProgramOptionsDialog::OnCancel)
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
    topSizer->Add ( new wxStaticText ( this , -1 , "" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    topSizer->Add ( new wxStaticText ( this , -1 , "" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    topSizer->Add ( useSettings , 1 , wxALIGN_CENTER_VERTICAL ) ;
    topSizer->Add ( join_enzymes , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    vs->Add ( topSizer , 0 , wxEXPAND , 5 ) ;
    vs->Add ( new wxStaticText ( this , -1 , " " ) , 0 , wxEXPAND , 5 ) ;
    vs->Add ( optionsSizer , 1 , wxEXPAND , 5 ) ;
    
    // Min/max cutoff
    useMinCutoff = new wxCheckBox ( this , GES_USE_MINCUTOFF , txt("t_ges_use_min_cutoff") ) ;
    minCutoff = new wxSpinCtrl ( this , -1 , "1"  , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , 30 ) ) ;
    useMaxCutoff = new wxCheckBox ( this , GES_USE_MAXCUTOFF , txt("t_ges_use_max_cutoff") ) ;
    maxCutoff = new wxSpinCtrl ( this , -1 , "3"  , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , 30 ) ) ;
    
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
    optionsSizer->Add ( new wxStaticText ( this , -1 , "" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( new wxStaticText ( this , -1 , "" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;

    optionsSizer->Add ( new wxStaticText ( this , -1 , txt("t_ges_met") ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( met_dam , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( met_dcm , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( new wxStaticText ( this , -1 , "" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;

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
    int w , h ;
    bo = 5 ;
    lh = 22 ;
    GetClientSize ( &w , &h ) ;
    globalSettingsPanel = NULL ;
    nb = new wxNotebook ( (wxWindow*) this , -1 ,
                          wxPoint ( 0 , 0 ) ,
                          wxSize ( w , h - lh * 2 ) ) ;
    initGlobalSettings () ;
    initGlobalEnzymes () ;

    wxButton *OK = new wxButton ( this , POD_OK , txt("b_ok") ,
                                wxPoint ( bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;
    new wxButton ( this , POD_CANCEL , txt("b_cancel") ,
                                wxPoint ( w*3/4 - bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;

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
	int w , h ;
    globalSettingsPanel = new wxPanel ( nb , -1 ) ;
    nb->AddPage ( globalSettingsPanel , txt("t_global_settings") ) ;    
    globalSettingsPanel->GetClientSize ( &w , &h ) ;
    
    wxStaticText *st ;
    st = new wxStaticText ( globalSettingsPanel , -1 , txt("t_language") , wxPoint ( bo , lh ) ) ;
    wxRect r1 = st->GetRect() ;
    
    int mw = r1.GetRight() ;
    mw += bo ;
    
    language = new wxChoice ( globalSettingsPanel , -1 , wxPoint ( mw , lh - bo ) ) ;
    enhancedDisplay = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_enhanced_display") , 
                        wxPoint ( bo , lh*3 ) ) ;
    vectorTitle = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_vector_title") , 
                        wxPoint ( bo , lh*4 ) ) ;
    vectorLength = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_vector_length") , 
                        wxPoint ( bo , lh*5 ) ) ;
    loadLastProject = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_load_last_project") , 
                        wxPoint ( bo , lh*6 ) ) ;
    useMetafile = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_use_metafile") , 
                        wxPoint ( bo , lh*7 ) ) ;
    showSplashScreen = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_show_splashscreen") , 
                        wxPoint ( bo , lh*8 ) ) ;
    checkUpdate = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_check4update") , 
                        wxPoint ( bo , lh*9 ) ) ;
    useInternalHelp = new wxCheckBox ( globalSettingsPanel , -1 , 
                        txt("t_useinternalhelp") , 
                        wxPoint ( bo , lh*10 ) ) ;
                        
    enhancedDisplay->SetValue ( myapp()->frame->enhancedRefresh ) ;
    vectorTitle->SetValue ( myapp()->frame->showVectorTitle ) ;
    vectorLength->SetValue ( myapp()->frame->showVectorLength ) ;
    loadLastProject->SetValue ( myapp()->frame->loadLastProject ) ;
    useMetafile->SetValue ( myapp()->frame->useMetafile ) ;
    showSplashScreen->SetValue ( myapp()->frame->showSplashScreen ) ;
    checkUpdate->SetValue ( myapp()->frame->checkUpdate ) ;
    useInternalHelp->SetValue ( myapp()->frame->useInternalHelp ) ;
    language->Append ( "en" ) ;
    language->Append ( "de" ) ;
    language->SetStringSelection ( myapp()->frame->lang_string ) ;
    }

void ProgramOptionsDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
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
    wxString s = myapp()->frame->LS->getOption ( "GLOBALENZYMESETTINGS" , "" ) ;
    from_string ( s ) ;
	}    

void TEnzymeRules::save_global_settings ()
	{
    myapp()->frame->LS->setOption ( "GLOBALENZYMESETTINGS" , to_string() ) ;
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
	if ( default_group != "" ) est->default_group->SetStringSelection ( default_group ) ;
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
     
wxString TEnzymeRules::to_string ()
	{
	wxString ret ;
	ret += wxString::Format ( "useit=%d\r" , useit ) ;
	ret += wxString::Format ( "min_cutoff=%d\r" , min_cutoff ) ;
	ret += wxString::Format ( "max_cutoff=%d\r" , max_cutoff ) ;
	ret += wxString::Format ( "use_min_cutoff=%d\r" , use_min_cutoff ) ;
	ret += wxString::Format ( "use_max_cutoff=%d\r" , use_max_cutoff ) ;
	ret += wxString::Format ( "recog4=%d\r" , recog4 ) ;
	ret += wxString::Format ( "recog5=%d\r" , recog5 ) ;
	ret += wxString::Format ( "recog6=%d\r" , recog6 ) ;
	ret += wxString::Format ( "recog_longer=%d\r" , recog_longer ) ;
	ret += wxString::Format ( "pattern3=%d\r" , pattern3 ) ;
	ret += wxString::Format ( "pattern5=%d\r" , pattern5 ) ;
	ret += wxString::Format ( "pattern_blunt=%d\r" , pattern_blunt ) ;
	ret += wxString::Format ( "default_group=%s\r" , default_group.c_str() ) ;
	ret += wxString::Format ( "col1=%d,%d,%d\r" , col1.Red() , col1.Green() , col1.Blue() ) ;
	ret += wxString::Format ( "col2=%d,%d,%d\r" , col2.Red() , col2.Green() , col2.Blue() ) ;
	ret += wxString::Format ( "col3=%d,%d,%d\r" , col3.Red() , col3.Green() , col3.Blue() ) ;
	ret += wxString::Format ( "join_enzymes=%d\r" , join_enzymes ) ;
	ret += wxString::Format ( "use_color_coding=%d\r" , use_color_coding ) ;
	ret += wxString::Format ( "methylation=%d\r" , methylation ) ;
	ret += wxString::Format ( "showgc=%d\r" , showgc ) ;
	return ret ;
	}    

void TEnzymeRules::from_string ( wxString &s )
	{
	init () ;
	wxArrayString as ;
	explode ( "\r" , s , as ) ;
	for ( int a = 0 ; a < as.GetCount() ; a++ )
		{
		if ( as[a] == "" ) continue ;
		wxString key = as[a].BeforeFirst ( '=' ) ;
		wxString val = as[a].AfterFirst ( '=' ) ;
		long l ;
		val.ToLong ( &l ) ;
		if ( key == "useit" ) useit = l ;
		else if ( key == "min_cutoff" ) min_cutoff = l ;
		else if ( key == "max_cutoff" ) max_cutoff = l ;
		else if ( key == "use_min_cutoff" ) use_min_cutoff = l ;
		else if ( key == "use_max_cutoff" ) use_max_cutoff = l ;
		else if ( key == "recog4" ) recog4 = l ;
		else if ( key == "recog5" ) recog5 = l ;
		else if ( key == "recog6" ) recog6 = l ;
		else if ( key == "recog_longer" ) recog_longer = l ;
		else if ( key == "pattern3" ) pattern3 = l ;
		else if ( key == "pattern5" ) pattern5 = l ;
		else if ( key == "pattern_blunt" ) pattern_blunt = l ;
		else if ( key == "default_group" ) default_group = val ;
		else if ( key == "col1" ) col1 = scan_color ( val ) ;
		else if ( key == "col2" ) col2 = scan_color ( val ) ;
		else if ( key == "col3" ) col3 = scan_color ( val ) ;
		else if ( key == "join_enzymes" ) join_enzymes = l ;
		else if ( key == "use_color_coding" ) use_color_coding = l ;
		else if ( key == "methylation" ) methylation = l ;
		else if ( key == "showgc" ) showgc = l ;
		}    
	}    

wxColour TEnzymeRules::scan_color ( wxString s )
	{
	wxString c1 = s.BeforeFirst ( ',' ) ;
	s = s.AfterFirst ( ',' ) ;
	wxString c2 = s.BeforeFirst ( ',' ) ;
	wxString c3 = s.AfterFirst ( ',' ) ;
	return wxColour ( atoi(c1.c_str()) , atoi(c2.c_str()) , atoi(c3.c_str()) ) ;
	}

// Here we go...

wxColour *TEnzymeRules::getColor ( int cuts )
	{
	if ( !use_color_coding || cuts == 0 || cuts > 3 ) return wxBLACK ;
	if ( cuts == 1 ) return &col1 ;
	if ( cuts == 2 ) return &col2 ;
	if ( cuts == 3 ) return &col3 ;
	}    

void TEnzymeRules::getVectorCuts ( TVector *v )
	{
    int a , b , c ;
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
 		    vs[i] = "" ;
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
	int max = 10000000 ;
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

