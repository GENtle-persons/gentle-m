#ifndef _PROTEOLYSIS_H_
#define _PROTEOLYSIS_H_

#include "main.h"

class TProtease ;

class TProteolysisGel : public wxScrolledWindow
	{
	public :
	TProteolysisGel(wxWindow* parent, wxWindowID id = -1 ) ;
	virtual void OnDraw(wxDC& dc) ;
	
//	DECLARE_EVENT_TABLE()
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
	void OnProtease ( wxCommandEvent &ev ) ; ///< Protease list event handler
	void OnIgnore ( wxCommandEvent &ev ) ; ///< Ignore list event handler
	void OnSepFragments ( wxCommandEvent &ev ) ; ///< Separate fragments event handler
	void OnSepResults ( wxCommandEvent &ev ) ; ///< Separation results event handler
	void OnCuts ( wxCommandEvent &ev ) ; ///< Cut list event handler
	void OnShowGel ( wxCommandEvent &ev ) ; // Show/refresh gel
	void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
	void OnSortResults(wxCommandEvent& event) ; ///< Sort results event handler
	void OnNumProts(wxCommandEvent& event) ; ///< Number of proteases event handler
	
	virtual void draw_gel ( wxDC &dc ) ; ///< Draw the virtual gel

	private :
	virtual void recalc () ;
	virtual void calc_cut_list () ;
	virtual void calc_fragment_list () ;
	virtual void calc_spearation () ;
	virtual void calc_spearation_sub ( int depth , TProteaseArray &prop , vector <TFragment> &tobe , int start = 0 ) ;
	virtual void show_gel () ;
	virtual void show_fragment_list () ;
	virtual double get_weight ( int from , int to ) ;
	virtual int get_y ( double y , int h , double min , double max ) ;
	virtual void determine_cuts ( TProteaseArray &prop , TProteaseCutArray &apc ) ;
	virtual void remove_ignored_cuts ( TProteaseCutArray &apc ) ;
	virtual void sort_cuts ( TProteaseCutArray &apc ) ;
	virtual void add_final_piece ( TProteaseCutArray &apc ) ;
	virtual void find_cutting_proteases () ;
	
	TAminoAcids *parent ;
	TVector *v ;
	TStorage *ls ;
	wxCheckListBox *proteases , *ignore , *cuts , *results , *sep_fragments ;
	wxListBox *sep_results ;
	wxRadioBox *sep_num_prot ;
	wxCheckBox *show_uncut , *create_fragments, *create_labels , *use_proteases ;
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
