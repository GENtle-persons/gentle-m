/** \file
	\brief Contains the ExternalInterface and its helper classes
*/
#ifndef _EXTERNAL_INTERFACE_H_
#define _EXTERNAL_INTERFACE_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>
#include <wx/htmllbox.h>
#include "SendHTTP.h"
#include <wx/thread.h>

enum {
	ID_HLB = 6000,
	ID_T1,
	ID_T2,
	ID_T3,
	ID_T4,
	ID_B1,
	ID_B2,
	ID_B3,
	ID_B_LAST,
	ID_B_NEXT,
	ID_C1,
	ID_C2,
	} ;	

enum {
	EI_NCBI = 1,
	EI_BLAST,
	} ;	
	
class blastThread ;

/// The External Interface List Box class, a specialized wxHtmlListBox
class EILB : public wxHtmlListBox
	{
	public :
	EILB ( wxWindow *parent , int id = wxID_ANY ) ; ///< Constructor
	virtual wxString OnGetItem(size_t n) const ; ///< Get the item string
	virtual void Clear () ; ///< Clear the list box
	virtual void Sort () ; ///< Sort the list box
	virtual void Update () ; ///< Update the list box
	virtual void Set ( int id , wxString s , wxString t = _T("") ) ; ///< Set an entry
	
	wxArrayString was , data ;
	} ;

/// The External Interface panel class
class EIpanel : public wxPanel
{
    public :
	EIpanel ( wxWindow *parent , int _mode ) ; ///< Constructor

//    private :
	virtual void process () ; ///< Runs the query, as process_blast or process_ncbi
    virtual wxString num2html ( int num , int digits ) ; ///< Returns a HTML-formatted number
    void showMessage ( wxString msg ) ; ///< Displays a message beneath the search controls

    virtual void init_blast() ; ///< Initialized BLAST interface
    virtual void process_blast() ; ///< Processes BLAST command, starts thread
    virtual void process_blast2() ; ///< Processes thread results
    virtual void execute_blast_b3() ; ///< Opens the associated link
    virtual void execute_blast() ; ///< Opens returned BLAST entry
    virtual wxString blast_align ( wxString qseq , wxString mseq , wxString hseq , int cpl , int qoff , int hoff ) ;

    virtual void init_ncbi() ; ///< Initializes NCBI interface
    virtual void process_ncbi() ; ///< Processes NCBI request
    virtual void execute_ncbi() ; ///< Opens returned sequence entry
    virtual void execute_ncbi_b3() ; ///< Opens the associated link

    virtual void execute_ncbi_load ( wxString database ) ; ///< Load a sequence from NCBI

    virtual void OnB1 ( wxCommandEvent& WXUNUSED(event) ) ; ///< Button 1 handler
    virtual void OnB2 ( wxCommandEvent& WXUNUSED(event) ) ; ///< Button 2 handler
    virtual void OnB3 ( wxCommandEvent& WXUNUSED(event) ) ; ///< Button 3 handler
    virtual void OnBlast ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Run BLAST" event handler
    virtual void OnBnext ( wxCommandEvent& WXUNUSED(event) ) ; ///< "Next" event handler
    virtual void OnC1 ( wxCommandEvent& WXUNUSED(event) ) ; ///< Choice box 1 event handler
    virtual void OnLboxDClick ( wxCommandEvent& WXUNUSED(event) ) ; ///< List box double click event handler

    virtual wxString val ( TiXmlNode *n ) ; ///< Return safe value
    virtual wxString valFC ( TiXmlNode *n ) ; ///< Return value of FirstChild

    int mode ;
    wxPanel *up ;
    EILB *hlb ;
    wxBoxSizer *v0 , *v1 , *h0 , *h1 ;
    wxTextCtrl *t1 , *t2 , *t3 , *t4 ;
    wxStaticText *st_msg ;
    wxButton *b1 , *b2 , *b3 , *b_last , *b_next ;
    wxChoice *c1 , *c2 ;
    long res_count , res_start , RETMAX ;

    // BLAST-specific
    blastThread *blast_thread ;
    wxString blast_res ;


    DECLARE_EVENT_TABLE()
	} ;

/**	\brief The External Interface ChildBase class
*/
class ExternalInterface : public ChildBase
    {
    public :
    ExternalInterface(wxWindow *parent, const wxString& title) ; ///< Constructor
    ~ExternalInterface () ; ///< Destructor

    void initme () ; ///< Initialization
    virtual wxString getName () ; ///< Returns the class name

    virtual void runBlast ( wxString seq , wxString prg ) ; ///< Directly runs a BLAST query

    wxNotebook *nb ; ///< Pointer to the wxNotebook class containing one or more EIpanel

    DECLARE_EVENT_TABLE()
    } ;

#endif
