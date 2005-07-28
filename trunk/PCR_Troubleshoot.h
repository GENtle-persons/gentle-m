#ifndef _PCRTROUBLESHOOT_H_
#define _PCRTROUBLESHOOT_H_

#include "main.h"

class TPrimerDesign ;
class TPrimer ;

class TSantaLucia
	{
	public :
	TSantaLucia ( wxString s , double uFE , double dH , double dS )
		{
		seq = s ;
		unifiedFE = uFE ;
		deltaH = dH ;
		deltaS = dS ;
		} ;
	wxString seq ; // "AB/CD"
	double unifiedFE ; // Free energy (deltaG), unified
	double deltaH ;
	double deltaS ;
	} ;

class PCR_troubleshoot_dialog : public wxDialog
	{
	public :
	PCR_troubleshoot_dialog(TPrimerDesign *_parent, const wxString& title ) ; ///< Constructor
	
	virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
	virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
	virtual void OnList ( wxCommandEvent &ev ) ; ///< List event handler
	virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
	
	private :
	virtual void scan () ;
	virtual void scan_hairpin ( TPrimer &p , int nr ) ;
	virtual void scan_dimer ( TPrimer &p1 , TPrimer &p2 , int nr1 , int nr2 ) ;
	virtual void scan_length ( TPrimer &p , int nr ) ;
	virtual void scan_GCcontent ( TPrimer &p , int nr ) ;
	virtual void scan_GCclamp ( TPrimer &p , int nr ) ;
	virtual void scan_end_stability ( TPrimer &p , int nr ) ;
	virtual void scan_Runs ( TPrimer &p , int nr , int length ) ;
	virtual void scan_specificity ( TPrimer &p , int nr ) ;
	virtual void scan_melting_temperature ( TPrimer &p , int nr ) ;
	
	virtual void add_warning ( TPrimer &p , int nr , wxString head , wxString tail ) ;
	virtual void add_error ( TPrimer &p , int nr , wxString head , wxString tail ) ;
	virtual void show_item ( int n ) ;
	virtual wxString invert ( wxString s ) ;
	virtual wxString trim_both ( wxString s1 , wxString s2 , wxString s3 ) ;
	virtual wxArrayString get_matrix ( wxString s1 , wxString s2 ) ;
	virtual wxArrayString get_matches ( wxArrayString &m , int min , int allowed_gaps = 0 ) ;
	virtual void fillSantaLucia () ;
	
	virtual wxChar opp ( wxChar c ) ;
	wxString get_dimer_connections ( wxString s1 , wxString s2 ) ;
	virtual double deltaG0_dimer ( wxString s1 , wxString s2 ) ;
	virtual double deltaG0 ( TPrimer &p ) ;
	virtual double deltaH0 ( TPrimer &p ) ;
	virtual double deltaS0 ( TPrimer &p ) ;
	virtual int getSLindex ( wxString s ) ;
	
	static vector <TSantaLucia> santa_lucia ;
	TPrimerDesign *parent ;
	wxTextCtrl *text ;
	wxListBox *list ;
	wxArrayString l_title , l_text ;
	wxString nl ; // New line in display text
	
	DECLARE_EVENT_TABLE()
	} ;

#endif
