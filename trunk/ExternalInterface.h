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

class EILB : public wxHtmlListBox
	{
	public :
	EILB ( wxWindow *parent , int id = wxID_ANY ) ;
	virtual wxString OnGetItem(size_t n) const ;
	virtual void Clear () ;
	virtual void Sort () ;
	virtual void Update () ;
	virtual void Set ( int id , wxString s , wxString t = "" ) ;
	
	wxArrayString was , data ;
	} ;    

class EIpanel : public wxPanel
{
    public :
	EIpanel ( wxWindow *parent , int _mode ) ;
    
//    private :
	virtual void process () ;
    virtual wxString num2html ( int num , int digits ) ;
    void showMessage ( wxString msg ) ;

    virtual void init_blast() ;
    virtual void process_blast() ;
    virtual void process_blast2() ;
    virtual void execute_blast() ;
    virtual wxString blast_align ( wxString qseq , wxString mseq , wxString hseq , int cpl ) ;
    
    virtual void init_ncbi() ;
    virtual void process_ncbi() ;
    virtual void execute_ncbi() ;

    virtual void execute_ncbi_load ( wxString database ) ;
    
    virtual void OnB1 ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnB2 ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnBlast ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnBnext ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnC1 ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnLboxDClick ( wxCommandEvent& WXUNUSED(event) ) ;
    
    virtual wxString val ( TiXmlNode *n ) ; // Safe value
    virtual wxString valFC ( TiXmlNode *n ) ; // Value of FirstChild
    
    int mode ;
    wxPanel *up ;
    EILB *hlb ;
    wxBoxSizer *v0 , *v1 , *h0 , *h1 ;
    wxTextCtrl *t1 , *t2 , *t3 , *t4 ;
    wxStaticText *st_msg ;
    wxButton *b1 , *b2 , *b_last , *b_next ;
    wxChoice *c1 , *c2 ;
    long res_count , res_start , RETMAX ;
    
    // BLAST-specific
    blastThread *blast_thread ;
    wxString blast_res ;

    
    DECLARE_EVENT_TABLE()
	} ;    

class ExternalInterface : public ChildBase
    {
    public :
    ExternalInterface(wxWindow *parent, const wxString& title) ;
    ~ExternalInterface () ;
    
    void initme () ;
    virtual wxString getName () ;

    wxNotebook *nb ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
