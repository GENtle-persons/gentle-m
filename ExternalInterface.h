#ifndef _EXTERNAL_INTERFACE_H_
#define _EXTERNAL_INTERFACE_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>
#include <wx/htmllbox.h>

enum {
	EI_PUBMED_NUCLEOTIDE = 1,
	} ;	
	
class EILB : public wxHtmlListBox
	{
	public :
	EILB ( wxWindow *parent , int id = wxID_ANY ) ;
	virtual wxString OnGetItem(size_t n) const ;
	
	wxArrayString was ;
	} ;    

class EIpanel : public wxPanel
	{
	public :
	EIpanel ( wxWindow *parent , int _mode ) ;
	
	virtual void init_pubmed_nucleotide() ;
	
	virtual void process_pubmed_nucleotide() ;

    virtual void OnOK ( wxCommandEvent& WXUNUSED(event) ) ;
	
	int mode ;
	wxPanel *up ;
	EILB *hlb ;
	wxTextCtrl *t1 ;
	wxButton *b1 ;

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
