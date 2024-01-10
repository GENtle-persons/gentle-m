#ifndef _PROTEOLYSIS_H_
#define _PROTEOLYSIS_H_

#include "main.h"

class TProtease ;

class TProteolysisGel : public wxScrolledWindow
    {
    public :
    TProteolysisGel(wxWindow* parent, wxWindowID id = -1 ) ;
    virtual void OnDraw(wxDC& dc) ; ///< Redraw
    virtual void OnEvent(wxMouseEvent& event); ///< Mouse event handler

    wxArrayInt screen , logical ;
    
    DECLARE_EVENT_TABLE()
    } ;

typedef vector <TProtease*> TProteaseArray ;
typedef vector <TProteaseCut*> TProteaseCutArray ;
    
class TProteolysisSuggestion
    {
    public :
    TProteaseArray proteases ;
    wxString name , desc ;
    int grade ;
    } ;

bool operator < ( const TProteolysisSuggestion &c1 , const TProteolysisSuggestion &c2 ) ;
bool operator == ( const TProteolysisSuggestion &c1 , const TProteolysisSuggestion &c2 ) ;

class TProteolysis : public wxDialog
    {
    public :
    TProteolysis(TAminoAcids *_parent, const wxString& title ) ; ///< Constructor
    virtual ~TProteolysis () ; ///< Destructor

    void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    void OnAll ( wxCommandEvent &ev ) ; ///< "All" button event handler
    void OnNone ( wxCommandEvent &ev ) ; ///< "None" button event handler
    void OnReport ( wxCommandEvent &ev ) ; ///< "Report" button event handler
    void OnProtease ( wxCommandEvent &ev ) ; ///< Protease list event handler
    void OnIgnore ( wxCommandEvent &ev ) ; ///< Ignore list event handler
    void OnSepFragments ( wxCommandEvent &ev ) ; ///< Separate fragments event handler
    void OnSepResults ( wxCommandEvent &ev ) ; ///< Separation results event handler
    void OnCuts ( wxCommandEvent &ev ) ; ///< Cut list event handler
    void OnShowGel ( wxCommandEvent &ev ) ; // Show/refresh gel
    void OnPartial ( wxCommandEvent &ev ) ; // Toggle partial digestion
    void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    void OnSortResults(wxCommandEvent& event) ; ///< Sort results event handler
    void OnNumProts(wxCommandEvent& event) ; ///< Number of proteases event handler
    void OnResults ( wxCommandEvent &ev ) ; ///< Fragment list checkbox changed
    
    void draw_gel ( wxDC &dc ) const ; ///< Draw the virtual gel
    void select_fragments ( const wxArrayInt &ai ) ;

    private :
    void recalc () ;
    void calc_cut_list () ;
    void calc_fragment_list () ;
    void calc_spearation () ;
    void calc_spearation_sub ( int depth , TProteaseArray &prop , vector <TFragment> &tobe , int start = 0 ) ;
    void show_gel () const ;
    void show_fragment_list () ;
    double get_weight ( int from , int to ) const ;
    int get_y ( const double y , const int h , const double min , const double max ) const ;
    void determine_cuts ( TProteaseArray &prop , TProteaseCutArray &apc ) ;
    void remove_ignored_cuts ( TProteaseCutArray &apc ) ;
    void sort_cuts ( TProteaseCutArray &apc ) ;
    void add_final_piece ( TProteaseCutArray &apc ) ;
    void find_cutting_proteases () ;
    
    TAminoAcids *parent ;
    TVector *v ;
    TStorage *ls ;
    wxCheckListBox *proteases , *ignore , *cuts , *results , *sep_fragments ;
    wxListBox *sep_results ;
    wxRadioBox *sep_num_prot ;
    wxCheckBox *show_uncut , *create_fragments, *create_labels , *use_proteases , *partial_digestion ;
    TProteolysisGel *gel ;
    TProteaseCutArray pc ;
    wxTextCtrl *sep_desc ;
    vector <TFragment> fragments ;
    wxRadioBox *sortresults ;
    vector <TProteolysisSuggestion> suggestions ;
    vector <TProtease*> pr_cache , cutting_proteases ;
    vector <TProteaseCutArray> pc_cache ;
    int max_dep ;
    
    DECLARE_EVENT_TABLE()
    } ;


#endif
