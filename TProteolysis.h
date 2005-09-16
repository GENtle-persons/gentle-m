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
	
class TProteolysisSuggestion
	{
	public :
	wxArrayTProtease proteases ;
	wxString name , desc ;
	int grade ;
	} ;

bool operator < ( const TProteolysisSuggestion &c1 , const TProteolysisSuggestion &c2 ) ;
bool operator == ( const TProteolysisSuggestion &c1 , const TProteolysisSuggestion &c2 ) ;

class TProteolysis : public wxDialog
	{
	public :
	TProteolysis(TAminoAcids *_parent, const wxString& title ) ; ///< Constructor

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
	
	virtual void draw_gel ( wxDC &dc ) ;

	private :
	virtual void recalc () ;
	virtual void calc_cut_list () ;
	virtual void calc_fragment_list () ;
	virtual void calc_spearation () ;
	virtual void calc_spearation_sub ( int depth , wxArrayTProtease &prop , vector <TFragment> &tobe , int start = 0 ) ;
	virtual void show_gel () ;
	virtual void show_fragment_list () ;
	virtual double get_weight ( int from , int to ) ;
	virtual int get_y ( double y , int h , double min , double max ) ;
	virtual void determine_cuts ( wxArrayTProtease &prop , wxArrayTProteaseCut &apc ) ;
	virtual void remove_ignored_cuts ( wxArrayTProteaseCut &apc ) ;
	virtual void sort_cuts ( wxArrayTProteaseCut &apc ) ;
	virtual void add_final_piece ( wxArrayTProteaseCut &apc ) ;
	
	TAminoAcids *parent ;
	TVector *v ;
	TStorage *ls ;
	wxCheckListBox *proteases , *ignore , *cuts , *results , *sep_fragments ;
	wxListBox *sep_results ;
	wxRadioBox *sep_num_prot ;
	wxCheckBox *show_uncut , *create_fragments, *create_labels , *use_proteases ;
	TProteolysisGel *gel ;
	wxArrayTProteaseCut pc ;
	wxTextCtrl *sep_desc ;
	vector <TFragment> fragments ;
	wxRadioBox *sortresults ;
	vector <TProteolysisSuggestion> suggestions ;
	
	DECLARE_EVENT_TABLE()
	} ;


#endif
