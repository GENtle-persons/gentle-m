#ifndef _ProgramOptionsDialog_h_
#define _ProgramOptionsDialog_h_

#include "main.h"
    
class ProgramOptionsDialog : public wxDialog
    {
    public :
    ProgramOptionsDialog(wxWindow *parent, const wxString& title ) ;

    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;

    virtual void updateGlobalEnzymes () ;
    virtual void OnEnzymeCheckbox ( wxCommandEvent &event ) ;
    

    wxNotebook *nb ;
    
    wxPanel *globalSettingsPanel ;
    wxChoice *language ;
    wxCheckBox *enhancedDisplay , *vectorTitle , *vectorLength ,
                *loadLastProject , *useMetafile , *showSplashScreen ,
                *checkUpdate , *useInternalHelp ;
                
    wxPanel *globalEnzymesPanel ;
    wxCheckBox *useSettings ;
    wxFlexGridSizer *optionsSizer ;
    wxSpinCtrl *minCutoff , *maxCutoff ;
    wxCheckBox *useMinCutoff , *useMaxCutoff ;
                
    private :
    virtual void initGlobalSettings () ;
    virtual void initGlobalEnzymes () ;
    int bo , lh ;
    
    DECLARE_EVENT_TABLE()
    } ;

class TEnzymeRules
	{
	public :
	TEnzymeRules () { init () ; }
	virtual void init () ;
	virtual void load_global_settings () ;
	virtual void save_global_settings () ;
	virtual void setup_options ( ProgramOptionsDialog *pod ) ;
	virtual void lookup_options ( ProgramOptionsDialog *pod ) ;
	bool useit ;
	int min_cutoff , max_cutoff ;
	bool use_min_cutoff , use_max_cutoff ;
	bool use_display_cutoff ;
	bool use_recognition_sequence ;
	bool recog4 , recog5 , recog6 , recog_longer ;
	bool pattern3 , pattern5 , pattern_blunt ;
	wxString default_group ;
	wxColour col_single , col_double , col_triple ;
	
	private :
	virtual wxString to_string () ;
	virtual void from_string ( wxString &s ) ;
	} ;    

#endif
