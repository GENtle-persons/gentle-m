#ifndef _ProgramOptionsDialog_h_
#define _ProgramOptionsDialog_h_

#include "main.h"

class TVector ;
    
class ProgramOptionsDialog : public wxDialog
    {
    public :
    ProgramOptionsDialog(wxWindow *parent, const wxString& title ) ;

    virtual void OnOK ( wxCommandEvent &ev ) ;
    virtual void OnCancel ( wxCommandEvent &ev ) ;
    virtual void OnCharHook(wxKeyEvent& event) ;

    virtual void updateGlobalEnzymes () ;
    virtual void OnEnzymeCheckbox ( wxCommandEvent &event ) ;
    virtual void OnButton1 ( wxCommandEvent &event ) ;
    virtual void OnButton2 ( wxCommandEvent &event ) ;
    virtual void OnButton3 ( wxCommandEvent &event ) ;
    virtual void updateColor ( wxColour &c ) ;
    

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
    wxCheckBox *recog4 , *recog5 , *recog6 , *recog6p ;
    wxCheckBox *pattern3 , *pattern5 , *pattern_blunt ;
    wxChoice *default_group ;
    wxButton *bcol1 , *bcol2 , *bcol3 ;
    wxColour col1 , col2 , col3 ;
    
                
    private :
    virtual void initGlobalSettings () ;
    virtual void initGlobalEnzymes () ;
    virtual void updateColorButton ( wxButton *b , wxColour &c ) ;
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
	
	virtual void getVectorCuts ( TVector *v ) ;
	virtual wxColour *getColor ( int cuts ) ;
	
	bool useit ;
	int min_cutoff , max_cutoff ;
	bool use_min_cutoff , use_max_cutoff ;
	bool use_display_cutoff ;
	bool use_recognition_sequence ;
	bool recog4 , recog5 , recog6 , recog_longer ;
	bool pattern3 , pattern5 , pattern_blunt ;
	wxString default_group ;
	wxColour col1 , col2 , col3 ;
	
	private :
	virtual wxString to_string () ;
	virtual void from_string ( wxString &s ) ;
	virtual wxColour scan_color ( wxString s ) ;
	} ;    

#endif
