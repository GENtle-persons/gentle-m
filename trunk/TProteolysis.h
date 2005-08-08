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

class TProteolysis : public wxDialog
	{
	public :
	TProteolysis(TAminoAcids *_parent, const wxString& title ) ; ///< Constructor

	virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
	virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
	virtual void OnAll ( wxCommandEvent &ev ) ; ///< "All" button event handler
	virtual void OnNone ( wxCommandEvent &ev ) ; ///< "None" button event handler
	virtual void OnProtease ( wxCommandEvent &ev ) ; ///< Protease list event handler
	virtual void OnIgnore ( wxCommandEvent &ev ) ; ///< Ignore list event handler
	virtual void OnCuts ( wxCommandEvent &ev ) ; ///< Cut list event handler
	virtual void OnShowGel ( wxCommandEvent &ev ) ; // Show/refresh gel
	virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
	
	virtual void draw_gel ( wxDC &dc ) ;

	private :
	virtual void recalc () ;
	virtual void calc_cut_list () ;
	virtual void calc_fragment_list () ;
	virtual void show_gel () ;
	virtual double get_weight ( int from , int to ) ;
	virtual int get_y ( double y , int h , double min , double max ) ;
	
	TAminoAcids *parent ;
	TVector *v ;
	TStorage *ls ;
	wxCheckListBox *proteases , *ignore , *cuts , *results ;
	wxCheckBox *show_uncut , *create_fragments, *create_labels , *use_proteases ;
	TProteolysisGel *gel ;
	wxArrayTProteaseCut pc ;
	vector <TFragment> fragments ;
	
	DECLARE_EVENT_TABLE()
	} ;


#endif
