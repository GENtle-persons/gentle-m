#include "ProgramOptionsDialog.h"

BEGIN_EVENT_TABLE(ProgramOptionsDialog, wxDialog )
    EVT_BUTTON(POD_OK,ProgramOptionsDialog::OnOK)
    EVT_BUTTON(POD_CANCEL,ProgramOptionsDialog::OnCancel)
    EVT_CHAR_HOOK(ProgramOptionsDialog::OnCharHook)
    EVT_CHECKBOX(GES_USE,ProgramOptionsDialog::OnEnzymeCheckbox)
    EVT_CHECKBOX(GES_USE_MINCUTOFF,ProgramOptionsDialog::OnEnzymeCheckbox)
    EVT_CHECKBOX(GES_USE_MAXCUTOFF,ProgramOptionsDialog::OnEnzymeCheckbox)
    EVT_BUTTON(GES_COL_1,ProgramOptionsDialog::OnButton1)
    EVT_BUTTON(GES_COL_2,ProgramOptionsDialog::OnButton2)
    EVT_BUTTON(GES_COL_3,ProgramOptionsDialog::OnButton3)
END_EVENT_TABLE()


ProgramOptionsDialog::ProgramOptionsDialog(wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
    int w , h ;
    bo = 5 , lh = 22 ;
    GetClientSize ( &w , &h ) ;
    globalSettingsPanel = NULL ;
    nb = new wxNotebook ( (wxWindow*) this , -1 ,
                          wxPoint ( 0 , 0 ) ,
                          wxSize ( w , h - lh * 2 ) ) ;
    initGlobalEnzymes () ;
    initGlobalSettings () ;

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
	int w , h ;
    globalEnzymesPanel = new wxPanel ( nb , -1 ) ;
    nb->AddPage ( globalEnzymesPanel , txt("t_global_enzymes") ) ;    
    globalEnzymesPanel->GetClientSize ( &w , &h ) ;
    
    wxBoxSizer *vs = new wxBoxSizer ( wxVERTICAL ) ;
    optionsSizer = new wxFlexGridSizer ( 4 , 15 , 5 ) ;
    useSettings = new wxCheckBox ( globalEnzymesPanel , GES_USE , txt("t_use_global_enzyme_settings") ) ; 
    vs->Add ( useSettings , 0 , wxEXPAND , 5 ) ;
    vs->Add ( new wxStaticText ( globalEnzymesPanel , -1 , " " ) , 0 , wxEXPAND , 5 ) ;
    vs->Add ( optionsSizer , 1 , wxEXPAND , 5 ) ;
    
    // Min/max cutoff
    useMinCutoff = new wxCheckBox ( globalEnzymesPanel , GES_USE_MINCUTOFF , "Use min cutoff" ) ;
    minCutoff = new wxSpinCtrl ( globalEnzymesPanel , -1 , "1" ) ;
    useMaxCutoff = new wxCheckBox ( globalEnzymesPanel , GES_USE_MAXCUTOFF , "Use max cutoff" ) ;
    maxCutoff = new wxSpinCtrl ( globalEnzymesPanel , -1 , "3" ) ;
    
    // Length of recognition sequence
    recog4 = new wxCheckBox ( globalEnzymesPanel , -1 , "Sequence length 4" ) ;
    recog5 = new wxCheckBox ( globalEnzymesPanel , -1 , "Sequence length 5" ) ;
    recog6 = new wxCheckBox ( globalEnzymesPanel , -1 , "Sequence length 6" ) ;
    recog6p = new wxCheckBox ( globalEnzymesPanel , -1 , "Sequence length >6" ) ;

    // Overlaps
    pattern3 = new wxCheckBox ( globalEnzymesPanel , -1 , "3' overlap" ) ;
    pattern5 = new wxCheckBox ( globalEnzymesPanel , -1 , "5' overlap" ) ;
    pattern_blunt = new wxCheckBox ( globalEnzymesPanel , -1 , "blunt end" ) ;
    
    // Gropu list
    default_group = new wxChoice ( globalEnzymesPanel , -1 ) ;
    default_group->Append ( txt("All") ) ;
    wxArrayString vs2 ;
    myapp()->frame->LS->getEnzymeGroups ( vs2 ) ;
    for ( int i = 0 ; i < vs2.GetCount() ; i++ )
        default_group->Append ( vs2[i] ) ;
    default_group->SetStringSelection ( txt("All") ) ;
    
    // Colors
    bcol1 = new wxButton ( globalEnzymesPanel , GES_COL_1 , "Single cutter" ) ;
    bcol2 = new wxButton ( globalEnzymesPanel , GES_COL_2 , "Double cutter" ) ;
    bcol3 = new wxButton ( globalEnzymesPanel , GES_COL_3 , "Triple cutter" ) ;
    
    // Adding elements to gridsizer
    optionsSizer->Add ( useMinCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( minCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( useMaxCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( maxCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    optionsSizer->Add ( new wxStaticText ( globalEnzymesPanel , -1 , "Use enzymes that cause" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( pattern3 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( pattern5 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( pattern_blunt , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    optionsSizer->Add ( recog4 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( recog5 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( recog6 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( recog6p , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    optionsSizer->Add ( new wxStaticText ( globalEnzymesPanel , -1 , "Enzymes colors" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( bcol1 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( bcol2 , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( bcol3 , 1 , wxALIGN_CENTER_VERTICAL ) ;

    optionsSizer->Add ( new wxStaticText ( globalEnzymesPanel , -1 , "Default enzyme group" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( default_group , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( new wxStaticText ( globalEnzymesPanel , -1 , "" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( new wxStaticText ( globalEnzymesPanel , -1 , "" ) , 1 , wxALIGN_CENTER_VERTICAL ) ;
    
    // Do it!
    globalEnzymesPanel->SetSizer ( vs ) ;
    vs->Fit ( globalEnzymesPanel ) ;    
    globalEnzymesPanel->Show () ;
    updateGlobalEnzymes () ;
    }    
    
void ProgramOptionsDialog::updateGlobalEnzymes ()
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
	updateColorButton ( bcol1 , col1 ) ;
	updateColorButton ( bcol2 , col2 ) ;
	updateColorButton ( bcol3 , col3 ) ;
	}

void ProgramOptionsDialog::updateColorButton ( wxButton *b , wxColour &c )
	{
	b->SetForegroundColour ( c ) ;
	if ( ( c.Red() + c.Green() + c.Blue() ) / 3 < 100 ) b->SetBackgroundColour ( *wxWHITE ) ;
	else b->SetBackgroundColour ( *wxBLACK ) ;
	b->Enable ( useSettings->GetValue() ) ;
	}    
	
void ProgramOptionsDialog::OnEnzymeCheckbox ( wxCommandEvent &event )
	{
	updateGlobalEnzymes () ;
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


void ProgramOptionsDialog::OnButton1 ( wxCommandEvent &event )
	{
	updateColor ( col1 ) ;
	}    

void ProgramOptionsDialog::OnButton2 ( wxCommandEvent &event )
	{
	updateColor ( col2 ) ;
	}    

void ProgramOptionsDialog::OnButton3 ( wxCommandEvent &event )
	{
	updateColor ( col3 ) ;
	}    

void ProgramOptionsDialog::updateColor ( wxColour &c )
	{
	wxColour c2 = wxGetColourFromUser ( this , c ) ;
	if ( !c2.Ok() ) return ;
	c = c2 ;
	updateGlobalEnzymes () ;
	}    

//************************************************************************

void TEnzymeRules::init ()
	{
	useit = false ;
	min_cutoff = 1 ;
 	max_cutoff = 3 ;
	use_min_cutoff = true ;
 	use_max_cutoff = true ;
	use_display_cutoff = false ; // Unsupported
	use_recognition_sequence = true ; // Unsupported
	recog4 = true ;
	recog5 = true ;
	recog6 = true ;
 	recog_longer = true ;
	pattern3 = true ;
	pattern5  = false ;
	pattern_blunt = false ;
	default_group = txt("All") ;
	col1 = *wxRED ;
	col2 = *wxGREEN ;
	col3 = *wxBLUE ;
	}    

void TEnzymeRules::load_global_settings ()
	{
    wxString s = myapp()->frame->LS->getOption ( "GLOBALENZYMESETTINGS" , "" ) ;
    from_string ( s ) ;
	}    

void TEnzymeRules::save_global_settings ()
	{
    myapp()->frame->LS->setOption ( "GLOBALENZYMESETTINGS" , to_string() ) ;
	}    

void TEnzymeRules::setup_options ( ProgramOptionsDialog *pod )
	{
	pod->useSettings->SetValue ( useit ) ;
	pod->minCutoff->SetValue ( min_cutoff ) ;
	pod->maxCutoff->SetValue ( max_cutoff ) ;
	pod->useMinCutoff->SetValue ( use_min_cutoff ) ;
	pod->useMaxCutoff->SetValue ( use_max_cutoff ) ;
	pod->recog4->SetValue ( recog4 ) ;
	pod->recog5->SetValue ( recog5 ) ;
	pod->recog6->SetValue ( recog6 ) ;
	pod->recog6p->SetValue ( recog_longer ) ;
	pod->pattern3->SetValue ( pattern3 ) ;
	pod->pattern5->SetValue ( pattern5 ) ;
	pod->pattern_blunt->SetValue ( pattern_blunt ) ;
	if ( default_group != "" ) pod->default_group->SetStringSelection ( default_group ) ;
	pod->col1 = col1 ;
	pod->col2 = col2 ;
	pod->col3 = col3 ;
	pod->updateGlobalEnzymes () ;
	}    

void TEnzymeRules::lookup_options ( ProgramOptionsDialog *pod )
	{
	useit = pod->useSettings->GetValue() ;
	min_cutoff = pod->minCutoff->GetValue() ;
	max_cutoff = pod->maxCutoff->GetValue() ;
	use_min_cutoff = pod->useMinCutoff->GetValue() ;
	use_max_cutoff = pod->useMaxCutoff->GetValue() ;
	recog4 = pod->recog4->GetValue() ;
	recog5 = pod->recog5->GetValue() ;
	recog6 = pod->recog6->GetValue() ;
	recog_longer = pod->recog6p->GetValue() ;
	pattern3 = pod->pattern3->GetValue() ;
	pattern5 = pod->pattern5->GetValue() ;
	pattern_blunt = pod->pattern_blunt->GetValue() ;
	default_group = pod->default_group->GetStringSelection() ;
	col1 = pod->col1 ;
	col2 = pod->col2 ;
	col3 = pod->col3 ;
	}
     
wxString TEnzymeRules::to_string ()
	{
	wxString ret ;
	ret += wxString::Format ( "useit=%d\n" , useit ) ;
	ret += wxString::Format ( "min_cutoff=%d\n" , min_cutoff ) ;
	ret += wxString::Format ( "max_cutoff=%d\n" , max_cutoff ) ;
	ret += wxString::Format ( "use_min_cutoff=%d\n" , use_min_cutoff ) ;
	ret += wxString::Format ( "use_max_cutoff=%d\n" , use_max_cutoff ) ;
	ret += wxString::Format ( "use_display_cutoff=%d\n" , use_display_cutoff ) ;
	ret += wxString::Format ( "use_recognition_sequence=%d\n" , use_recognition_sequence ) ;
	ret += wxString::Format ( "recog4=%d\n" , recog4 ) ;
	ret += wxString::Format ( "recog5=%d\n" , recog5 ) ;
	ret += wxString::Format ( "recog6=%d\n" , recog6 ) ;
	ret += wxString::Format ( "recog_longer=%d\n" , recog_longer ) ;
	ret += wxString::Format ( "pattern3=%d\n" , pattern3 ) ;
	ret += wxString::Format ( "pattern5=%d\n" , pattern5 ) ;
	ret += wxString::Format ( "pattern_blunt=%d\n" , pattern_blunt ) ;
	ret += wxString::Format ( "default_group=%s\n" , default_group.c_str() ) ;
	ret += wxString::Format ( "col1=%d,%d,%d\n" , col1.Red() , col1.Green() , col1.Blue() ) ;
	ret += wxString::Format ( "col2=%d,%d,%d\n" , col2.Red() , col2.Green() , col2.Blue() ) ;
	ret += wxString::Format ( "col3=%d,%d,%d\n" , col3.Red() , col3.Green() , col3.Blue() ) ;
	return ret ;
	}    

void TEnzymeRules::from_string ( wxString &s )
	{
	init () ;
	wxArrayString as ;
	explode ( "\n" , s , as ) ;
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
	if ( !useit || cuts == 0 || cuts > 3 ) return wxBLACK ;
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
	    if ( keep && !pattern3 && e->overlap < 0 ) keep = false ; // ???
	    if ( keep && !pattern5 && e->overlap > 0 ) keep = false ; // ???
	    if ( keep && !pattern_blunt && e->overlap == 0 ) keep = false ;
	    if ( keep && !recog4 && e->sequence.length() == 4 ) keep = false ;
	    if ( keep && !recog5 && e->sequence.length() == 5 ) keep = false ;
	    if ( keep && !recog6 && e->sequence.length() == 6 ) keep = false ;
	    if ( keep && !recog_longer && e->sequence.length() > 6 ) keep = false ;
	    if ( keep ) ve.Add ( e ) ;
   		}    
   		
	// Add what has the correct number of cuts
	int max = 10000000 ;
	if ( use_max_cutoff ) max = max_cutoff ;
	for ( a = 0 ; a < ve.GetCount() ; a++ )
		{
  		vector <TRestrictionCut> vc ;
  		v->getCuts ( ve[a] , vc , false , max ) ;
  		if ( ( !use_min_cutoff || min_cutoff <= vc.size() ) &&
  			 ( !use_max_cutoff || max_cutoff >= vc.size() ) )
	       {
       	   for ( b = 0 ; b < vc.size() ; b++ ) v->rc.push_back ( vc[b] ) ;
	       }    
		}    
	}

