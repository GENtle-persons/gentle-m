/** \file
    \brief Contains the FindSequenceDialog class
*/
#ifndef _PCRTROUBLESHOOT_H_
#define _PCRTROUBLESHOOT_H_

#include "main.h"

class TPrimerDesign ;
class TPrimer ;

/** \brief The class implementing a "Santa Lucia" free energy set
*/
class TSantaLucia
    {
    public :
    TSantaLucia ( const wxString& s , const double& uFE , const double& dH , const double& dS )
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

/**    \brief The class implementing the PCR troubleshooting dialog
*/
class PCR_troubleshoot_dialog : public wxDialog
    {
    public :
    PCR_troubleshoot_dialog(TPrimerDesign *_parent, const wxString& title ) ; ///< Constructor
    ~PCR_troubleshoot_dialog () ; ///< Destructor

    virtual void OnOK ( wxCommandEvent &ev ) ; ///< OK button event handler
    virtual void OnCancel ( wxCommandEvent &ev ) ; ///< Cancel button event handler
    virtual void OnList ( wxCommandEvent &ev ) ; ///< List event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    private :
    virtual void scan () ; ///< Performs various scans
    virtual void scan_hairpin ( const TPrimer &p , const int nr ) ; ///< Scans for hairpins
    virtual void scan_dimer ( const TPrimer &p1 , const TPrimer &p2 , const int nr1 , const int nr2 ) ; ///< Scans for (self-)dimers
    virtual void scan_length ( const TPrimer &p , const int nr ) ; ///< Checks primer length
    virtual void scan_GCcontent ( const TPrimer &p , const int nr ) ; ///< Checks primer GC contents
    virtual void scan_GCclamp ( TPrimer &p , int nr ) ; ///< Checks for GC clamp
    virtual void scan_end_stability ( const TPrimer &p , const int nr ) ; ///< Checks primer end stability
    virtual void scan_Runs ( const TPrimer &p , const int nr , const int length ) ;
    virtual void scan_specificity ( const TPrimer &p , const int nr ) ; ///< Checks primer specificity
    virtual void scan_melting_temperature ( const TPrimer &p , const int nr ) ; ///< Checks melting temperature

    virtual void add_warning ( const TPrimer &p , const int nr , const wxString& head , const wxString& tail ) ; ///< Adds a warning to the list
    virtual void add_error ( const TPrimer &p , const int nr , const wxString& head , const wxString& tail ) ; ///< Adds an error to the list
    virtual void show_item ( const int n ) ;
    virtual wxString invert ( const wxString& s ) const ; ///< Inverts a (DNA) sequence
    virtual wxString trim_both ( const wxString& s1 , const wxString& s2 , const wxString& s3 ) ;
    virtual wxArrayString get_matrix ( const wxString& s1 , const wxString& s2 ) const ;
    // dead code (Comment by Steffen)
    // virtual wxArrayString get_matches ( wxArrayString &m , int min , int allowed_gaps = 0 ) ;
    virtual void fillSantaLucia () ;

    virtual wxChar opp ( const wxChar& c ) ;
    wxString get_dimer_connections ( const wxString& s1 , const wxString& s2 ) ;
    virtual double deltaG0_dimer ( const wxString& s1 , const wxString& s2 ) ;
    virtual double deltaG0 ( const TPrimer &p ) const ;
    virtual double deltaH0 ( const TPrimer &p ) const ;
    virtual double deltaS0 ( const TPrimer &p ) const ;
    virtual int getSLindex ( const wxString& s ) const ;

    static vector <TSantaLucia> santa_lucia ;
    TPrimerDesign *parent ;
    wxTextCtrl *text ;
    wxListBox *list ;
    wxArrayString l_title , l_text ;
    wxString nl ; // New line in display text

    DECLARE_EVENT_TABLE()
    } ;

#endif
