#ifndef _EXTERNAL_INTERFACE_H_
#define _EXTERNAL_INTERFACE_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>
#include <wx/htmllbox.h>

enum {
	EI_PUBMED = 1,
	} ;	
	
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
	
	virtual void init_pubmed() ;
	
	virtual void process_pubmed() ;
	virtual void execute_pubmed() ;

    virtual void OnB1 ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnB2 ( wxCommandEvent& WXUNUSED(event) ) ;
    virtual void OnC1 ( wxCommandEvent& WXUNUSED(event) ) ;
	
	int mode ;
	wxPanel *up ;
	EILB *hlb ;
	wxTextCtrl *t1 ;
	wxButton *b1 , *b2 ;
	wxChoice *c1 ;

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
