#include "ProgramOptionsDialog.h"

BEGIN_EVENT_TABLE(ProgramOptionsDialog, wxDialog )
    EVT_BUTTON(POD_OK,ProgramOptionsDialog::OnOK)
    EVT_BUTTON(POD_CANCEL,ProgramOptionsDialog::OnCancel)
    EVT_CHAR_HOOK(ProgramOptionsDialog::OnCharHook)
    EVT_CHECKBOX(GES_USE,ProgramOptionsDialog::OnEnzymeCheckbox)
    EVT_CHECKBOX(GES_USE_MINCUTOFF,ProgramOptionsDialog::OnEnzymeCheckbox)
    EVT_CHECKBOX(GES_USE_MAXCUTOFF,ProgramOptionsDialog::OnEnzymeCheckbox)
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
    optionsSizer = new wxFlexGridSizer ( 2 ) ;
    useSettings = new wxCheckBox ( globalEnzymesPanel , GES_USE , txt("t_use_global_enzyme_settings") ) ; 
    vs->Add ( useSettings , 1 , wxEXPAND , 5 ) ;
    vs->Add ( optionsSizer , 1 , wxEXPAND , 5 ) ;
    
    useMinCutoff = new wxCheckBox ( globalEnzymesPanel , GES_USE_MINCUTOFF , "Use min cutoff" ) ;
    minCutoff = new wxSpinCtrl ( globalEnzymesPanel , -1 , "1" ) ;
    useMaxCutoff = new wxCheckBox ( globalEnzymesPanel , GES_USE_MAXCUTOFF , "Use max cutoff" ) ;
    maxCutoff = new wxSpinCtrl ( globalEnzymesPanel , -1 , "3" ) ;
    
    optionsSizer->Add ( useMinCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( minCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( useMaxCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    optionsSizer->Add ( maxCutoff , 1 , wxALIGN_CENTER_VERTICAL ) ;
    

    globalEnzymesPanel->SetSizer ( vs ) ;
    vs->Fit ( globalEnzymesPanel ) ;    
    globalEnzymesPanel->Show () ;
    updateGlobalEnzymes () ;
    }    
    
void ProgramOptionsDialog::updateGlobalEnzymes ()
	{
	useMinCutoff->Enable ( useSettings->GetValue() ) ;
	useMaxCutoff->Enable ( useSettings->GetValue() ) ;
	minCutoff->Enable ( useMinCutoff->GetValue() && useSettings->GetValue() ) ;
	maxCutoff->Enable ( useMaxCutoff->GetValue() && useSettings->GetValue() ) ;
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

//************************************************************************

void TEnzymeRules::init ()
	{
	useit = false ;
	min_cutoff = 1 ;
 	max_cutoff = 3 ;
	use_min_cutoff = false ;
 	use_max_cutoff = false ;
	use_display_cutoff = false ;
	use_recognition_sequence = false ;
	recog4 = false ;
	recog5 = false ;
	recog6 = false ;
 	recog_longer = false ;
	pattern3 = false ;
	pattern5  = false ;
	pattern_blunt = false ;
	default_group = "" ;
	col_single = *wxBLACK ;
	col_double = *wxBLACK;
	col_triple = *wxBLACK ;
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
	pod->updateGlobalEnzymes () ;
	}    

void TEnzymeRules::lookup_options ( ProgramOptionsDialog *pod )
	{
	useit = pod->useSettings->GetValue() ;
	min_cutoff = pod->minCutoff->GetValue() ;
	max_cutoff = pod->maxCutoff->GetValue() ;
	use_min_cutoff = pod->useMinCutoff->GetValue() ;
	use_max_cutoff = pod->useMaxCutoff->GetValue() ;
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
	ret += wxString::Format ( "col_single=%d,%d,%d\n" , col_single.Red() , col_single.Green() , col_single.Blue() ) ;
	ret += wxString::Format ( "col_double=%d,%d,%d\n" , col_double.Red() , col_double.Green() , col_double.Blue() ) ;
	ret += wxString::Format ( "col_triple=%d,%d,%d\n" , col_triple.Red() , col_triple.Green() , col_triple.Blue() ) ;
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
		}    
	}    

