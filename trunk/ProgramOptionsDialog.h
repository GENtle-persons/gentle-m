#ifndef _ProgramOptionsDialog_h_
#define _ProgramOptionsDialog_h_

#include "main.h"

class TVector ;

#define EST_GLOBAL 0
#define EST_PROJECT 1
#define EST_SINGLE 2

class TEnzymeSettingsTab : public wxPanel
	{
	public :
	TEnzymeSettingsTab ( wxWindow *parent = NULL , int _mode = EST_GLOBAL ) ;
    virtual void updateColorButton ( wxButton *b , wxColour &c ) ;
    virtual void updateGlobalEnzymes () ;
    virtual void OnEnzymeCheckbox ( wxCommandEvent &event ) ;
    virtual void OnButton1 ( wxCommandEvent &event ) ;
    virtual void OnButton2 ( wxCommandEvent &event ) ;
    virtual void OnButton3 ( wxCommandEvent &event ) ;
    virtual void updateColor ( wxColour &c ) ;

    wxCheckBox *useSettings ;
    wxFlexGridSizer *optionsSizer ;
    wxSpinCtrl *minCutoff , *maxCutoff ;
    wxCheckBox *useMinCutoff , *useMaxCutoff ;
    wxCheckBox *use_color_coding , *join_enzymes ;
    wxCheckBox *recog4 , *recog5 , *recog6 , *recog6p ;
    wxCheckBox *pattern3 , *pattern5 , *pattern_blunt ;
    wxCheckBox *met_dam , *met_dcm ;
    wxCheckBox *showgc ;
    wxChoice *default_group ;
    wxButton *bcol1 , *bcol2 , *bcol3 ;
    wxColour col1 , col2 , col3 ;
    int mode ;

    DECLARE_EVENT_TABLE()
	} ;    

class ProgramOptionsDialog : public wxDialog
    {
    public :
    ProgramOptionsDialog(wxWindow *parent, const wxString& title ) ;

    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;    

    wxNotebook *nb ;
    TEnzymeSettingsTab *globalEnzymesPanel ;
    wxPanel *globalSettingsPanel ;
    wxChoice *language ;
    wxCheckBox *enhancedDisplay , *vectorTitle , *vectorLength ,
                *loadLastProject , *useMetafile , *showSplashScreen ,
                *checkUpdate , *useInternalHelp , *doRegisterStuff ;
                
    
                
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
	virtual void setup_options ( TEnzymeSettingsTab *est ) ;
	virtual void lookup_options ( TEnzymeSettingsTab *est ) ;
	
	virtual wxString to_string () ;
	virtual void from_string ( wxString &s ) ;

	virtual void getVectorCuts ( TVector *v ) ;
	virtual wxColour *getColor ( int cuts ) ;
	
	bool useit ;
	int min_cutoff , max_cutoff ;
	bool use_min_cutoff , use_max_cutoff ;
	bool recog4 , recog5 , recog6 , recog_longer ;
	bool pattern3 , pattern5 , pattern_blunt ;
	bool showgc ;
	wxString default_group ;
	wxColour col1 , col2 , col3 ;
	bool join_enzymes , use_color_coding ;
	int methylation ;
	
	private :
	virtual wxColour scan_color ( wxString s ) ;
	} ;    

#endif
