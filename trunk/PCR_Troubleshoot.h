/** \file
	\brief Contains the FindSequenceDialog class
*/
#ifndef _PCRTROUBLESHOOT_H_
#define _PCRTROUBLESHOOT_H_

#include "main.h"

class TPrimerDesign ;
class TPrimer ;

/**	\brief The class implementing a "Santa Lucia" free energy set
*/
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

/**	\brief The class implementing the PCR troubleshooting dialog
*/
class PCR_troubleshoot_dialog : public wxDialog
	{
	public :
	PCR_troubleshoot_dialog(TPrimerDesign *_parent, const wxString& title ) ; ///< Constructor
	
	virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
	virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
	virtual void OnList ( wxCommandEvent &ev ) ; ///< List event handler
	virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
	
	private :
	virtual void scan () ; ///< Performs various scans
	virtual void scan_hairpin ( TPrimer &p , int nr ) ; ///< Scans for hairpins
	virtual void scan_dimer ( TPrimer &p1 , TPrimer &p2 , int nr1 , int nr2 ) ; ///< Scans for (self-)dimers
	virtual void scan_length ( TPrimer &p , int nr ) ; ///< Checks primer length
	virtual void scan_GCcontent ( TPrimer &p , int nr ) ; ///< Checks primer GC contents
	virtual void scan_GCclamp ( TPrimer &p , int nr ) ; ///< Checks for GC clamp
	virtual void scan_end_stability ( TPrimer &p , int nr ) ; ///< Checks primer end stability
	virtual void scan_Runs ( TPrimer &p , int nr , int length ) ;
	virtual void scan_specificity ( TPrimer &p , int nr ) ; ///< Checks primer specificity
	virtual void scan_melting_temperature ( TPrimer &p , int nr ) ; ///< Checks melting temperature
	
	virtual void add_warning ( TPrimer &p , int nr , wxString head , wxString tail ) ; ///< Adds a warning to the list
	virtual void add_error ( TPrimer &p , int nr , wxString head , wxString tail ) ; ///< Adds an error to the list
	virtual void show_item ( int n ) ;
	virtual wxString invert ( wxString s ) ; ///< Inverts a (DNA) sequence
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
