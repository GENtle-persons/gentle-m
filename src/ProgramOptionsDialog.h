/** \file
    \brief Contains the ProgramOptionsDialog class and its helper classes, TEnzymeSettingsTab (which is also used in TVectorEditor) and TEnzymeRules
*/
#ifndef _ProgramOptionsDialog_h_
#define _ProgramOptionsDialog_h_

#include "main.h"

class TVector ;

#define EST_GLOBAL 0
#define EST_PROJECT 1
#define EST_SINGLE 2

/** \brief The "tab" for global and per-sequence enzyme/methylation/etc. settings
*/ ;
class TEnzymeSettingsTab : public wxPanel
    {
    public :
    TEnzymeSettingsTab ( wxWindow *parent = NULL , const int _mode = EST_GLOBAL ) ; ///< Constructor
    virtual void updateColorButton ( wxButton *b , const wxColour &c ) ; ///< Updates the color of one of the color choice buttons
    virtual void updateGlobalEnzymes () ; ///< Update the global enzymes list
    virtual void updateColor ( wxColour &c ) ; ///< ???

    virtual void OnEnzymeCheckbox ( wxCommandEvent &event ) ; ///< Enyzme checkbox event handler
    virtual void OnButton1 ( wxCommandEvent &event ) ; ///< Button 1 (single cutter) event handler
    virtual void OnButton2 ( wxCommandEvent &event ) ; ///< Button 2 (double cutter) event handler
    virtual void OnButton3 ( wxCommandEvent &event ) ; ///< Button 3 (triple cutter) event handler

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

/** \brief The dialog containing program end enzyme global settings
*/
class ProgramOptionsDialog : public wxDialog
    {
    public :
    ProgramOptionsDialog(wxWindow *parent, const wxString& title ) ; ///< Constructor
    ~ProgramOptionsDialog () ; ///< Destructor

    virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    virtual void OnAACol ( wxCommandEvent &ev ) ; ///< Amino acid color event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    vector <int> translation_tables ;
    wxNotebook *nb ; ///< Pointer to the wxNotebook structure containing the tabs
    TEnzymeSettingsTab *globalEnzymesPanel ;
    wxPanel *globalSettingsPanel ;
    wxChoice *language ; ///< Pointer to the dropdown language list
    wxChoice *nonstandard_translation_table ;
    wxCheckBox *enhancedDisplay , *vectorTitle , *vectorLength ,
               *loadLastProject , *useMetafile , *showSplashScreen ,
               *checkUpdate , *useInternalHelp , *doRegisterStuff ,
               *showEnzymePos , *use_nonstandard_translation_table ,
               *showTips , *useTwoToolbars , *useOnlineHelp , *showToolTips ,
               *showLowercaseDNA;

    wxTextCtrl *proxyName , *proxyPort , *orfLength ;

    wxRadioBox *editFeatureMode ; ///< Pointer to the list of choices of how to treat edited items
    wxRadioBox *showStopCodon ; ///< Pointer to the list of choices of how to display stop codons
    wxColour aacol ; ///< The color in which to draw the amino acids in DNA mode

    private :
    virtual void initGlobalSettings () ; ///< Initialize "Global settings" tab
    virtual void initGlobalEnzymes () ; ///< Initialize "Global enzyme settings" tab
    int bo , lh ;

    DECLARE_EVENT_TABLE()
    } ;

/** \brief Stores, loads, stores, and compares enzyme settings
*/
class TEnzymeRules
    {
    public :
    TEnzymeRules () { init () ; } ///< Constructor
    virtual ~TEnzymeRules () {} ; ///< Destructor (empty)

    virtual void init () ; ///< Initialization
    virtual void load_global_settings () ; ///< Loads global settings from the database
    virtual void save_global_settings () const ; ///< Save global settings to the database
    virtual void setup_options ( TEnzymeSettingsTab * const est ) ; ///< Set options in the tab
    virtual void lookup_options ( TEnzymeSettingsTab * const est ) ; ///< Look up options from the tab
    virtual bool isEqual ( const TEnzymeRules &r ) const ; ///< Compare with another set of settings

    virtual wxString to_string () const ; ///< "Compress" to storable string
    virtual void from_string ( const wxString &s ) ; ///< "Decompress" from storage string

    virtual void getVectorCuts ( TVector * const v ) const ;
    virtual wxColour *getColor ( const int cuts ) ; ///< Returns a pointer to a wxColour structure with the correct color for the given number of cuts

    bool useit ;
    int min_cutoff ;
    int max_cutoff ; ///< limits the maximal number of cuts for an enzyme on a single sequence to be accepted
    bool use_min_cutoff ;
    bool use_max_cutoff ; ///< flag to enforce maximal number of cuts for an enzyme on a single sequence
    bool recog4 , recog5 , recog6 , recog_longer ;
    bool pattern3 , pattern5 , pattern_blunt ;
    bool showgc ;
    wxString default_group ;
    wxColour col1 , col2 , col3 ;
    bool join_enzymes , use_color_coding ;
    int methylation ;

    private :
    virtual wxColour scan_color ( const wxString& s ) const ; ///< Make color from string
    } ;

#endif
