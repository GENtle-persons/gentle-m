#include "ExternalInterface.h"
#include "SendHTTP.h"

enum {
	ID_T1 = 6000,
	ID_B1,
	ID_B2,
	ID_C1,
	} ;	
   	

BEGIN_EVENT_TABLE(ExternalInterface, MyChildBase)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)

    // Dummies
    EVT_MENU(MDI_TOGGLE_FEATURES,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_UNDO,ChildBase::OnDummy)
    EVT_MENU(MDI_CUT,ChildBase::OnDummy)
    EVT_MENU(MDI_COPY,ChildBase::OnDummy)
    EVT_MENU(MDI_PASTE,ChildBase::OnDummy)
    EVT_MENU(MDI_EDIT_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_EXPORT,ChildBase::OnDummy)
    EVT_MENU(MDI_MARK_ALL,ChildBase::OnDummy)
    EVT_MENU(MDI_FILE_SAVE,ChildBase::OnDummy)
    EVT_MENU(MDI_FIND,ChildBase::OnDummy)
    EVT_MENU(AA_NONE,TABIviewer::OnDummy)
    EVT_MENU(AA_KNOWN, TABIviewer::OnDummy)
    EVT_MENU(AA_ALL, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE, TABIviewer::OnDummy)
    EVT_MENU(AA_ONE, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_3, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M3, TABIviewer::OnDummy)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(EIpanel, wxPanel)
	EVT_BUTTON(ID_B1, EIpanel::OnB1)
	EVT_BUTTON(ID_B2, EIpanel::OnB2)
	EVT_TEXT_ENTER(ID_T1, EIpanel::OnB1)
	EVT_CHOICE(ID_C1, EIpanel::OnC1)
END_EVENT_TABLE()


ExternalInterface::ExternalInterface(wxWindow *parent, const wxString& title) 
    : ChildBase(parent, title)
    {
    def = "EXTERNAL" ;
    }

ExternalInterface::~ExternalInterface ()
    {
    }
    
void ExternalInterface::initme ()
    {
    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    SetMenuBar(menu_bar);

    nb = new wxNotebook ( this , -1 ) ;
    
    nb->AddPage ( new EIpanel ( nb , EI_PUBMED ) , "Test" ) ;

/*
    rl = new wxSplitterWindow ( this , -1 ) ;
    right = new TMyImagePanel ( rl , IV_IMAGE ) ;
    right->imdi = this ;
    
    wxSplitterWindow *ud ;
    ud = new wxSplitterWindow ( rl , -1 ) ;

    rl->SplitVertically ( ud , right , 300 ) ;
    
    lb = new wxListBox ( ud , IV_LIST , wxDefaultPosition , wxDefaultSize ,
                            0 , NULL , wxLB_SORT ) ;
    bu = new wxButton ( ud , IV_BUTTON , "" ) ;
    
    ud->SplitHorizontally ( bu , lb , 20 ) ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
//    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( rl , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;
    
    wxString s_dir = myapp()->frame->LS->getOption ( "IMGDIR" , wxGetCwd() ) ;    
    ShowDir ( s_dir ) ;
*/

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
//    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( nb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

    myapp()->frame->setChild ( this ) ;
    Maximize () ;
    Activate () ;
    }

wxString ExternalInterface::getName ()
    {
    return "External Interface" ;
    }
    
    
// *****************************************************************************

EILB::EILB ( wxWindow *parent , int id )
	: wxHtmlListBox ( parent , id , wxDefaultPosition , wxDefaultSize , wxLB_MULTIPLE )
	{
	SetItemCount ( 0 ) ;
	}    

wxString EILB::OnGetItem(size_t n) const
	{
	wxString ret = was[n] ;
	wxString bgcolor ;
	if ( n & 1 ) bgcolor = "#FFFFFF" ;
	else bgcolor = "#EEEEEE" ;
	ret = "<table bgcolor='" + bgcolor + "' width='100%' cellspacing=0 cellpadding=0><tr><td>" + ret + "</td></tr></table>" ;
	return ret ;
	}
	
void EILB::Clear ()
	{
	SetItemCount ( 0 ) ;
	was.Clear () ;
	data.Clear () ;
//	wxHtmlListBox::Clear () ;
	}    
     
void EILB::Set ( int id , wxString s , wxString t )
	{
	while ( was.GetCount() <= id ) was.Add ( "" ) ;
	while ( data.GetCount() <= id ) data.Add ( "" ) ;
	was[id] = s ;
	data[id] = t ;
	}
	
void EILB::Sort()
	{
	int a ;
	for ( a = 1 ; a < was.GetCount() ; a++ )
		{
		if ( was[a-1] <= was[a] ) continue ;
		wxString tmp ;
		tmp = was[a] ; was[a] = was[a-1] ; was[a-1] = tmp ;
		tmp = data[a] ; data[a] = data[a-1] ; data[a-1] = tmp ;
		a = 0 ;
		}    
	}
     
void EILB::Update()
	{
	SetItemCount ( was.GetCount() ) ;
	Refresh () ;
	}    
     
// *****************************************************************************

EIpanel::EIpanel ( wxWindow *parent , int _mode )
	: wxPanel ( parent )
	{
	mode = _mode ;
	
	up = new wxPanel ( this ) ;
	hlb = new EILB ( this ) ;
	
	if ( mode == EI_PUBMED ) init_pubmed() ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( up , 0 , wxEXPAND , 5 ) ;
    v0->Add ( hlb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
//    v0->Fit ( this ) ;
	}
     
void EIpanel::init_pubmed()
	{
	t1 = new wxTextCtrl ( up , ID_T1 , "pGEX 3x" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	b1 = new wxButton ( up , ID_B1 , txt("b_find") , wxDefaultPosition ) ;
	b2 = new wxButton ( up , ID_B2 , txt("t_open") , wxDefaultPosition ) ;
	c1 = new wxChoice ( up , ID_C1 ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Protein" ) ;
	c1->Append ( "Pubmed" ) ;
/*
	structure, genome, pmc, omim, taxonomy, books, probeset,  domains, unists, cdd, snp, journals, unigene, popset
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Nucleotide" ) ;
*/

    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    h0->Add ( c1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( t1 , 1 , wxEXPAND , 5 ) ;
    h0->Add ( b1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b2 , 0 , wxEXPAND , 5 ) ;
    up->SetSizer ( h0 ) ;
    h0->Fit ( up ) ;

    c1->SetSelection ( 0 ) ;
	t1->SetFocus() ;
	}
	
void EIpanel::process_pubmed()
	{
	myExternal ex ;
	wxString database = c1->GetStringSelection() ;
	database = database.Lower() ;
	wxString search = t1->GetValue() ;
	search.Replace ( " " , "+" ) ;
	wxString query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=" + database + "&retmode=xml&term=" ;
	query += search ;
	wxString res ;
	res = ex.getText ( query ) ; // The XML is now in res
	hlb->Clear () ;

    TiXmlDocument doc ;
    doc.Parse ( res.c_str() ) ;
    if ( res == "" || doc.Error() )
    	{
	    hlb->Set ( 0 , txt("t_error") ) ;
	    return ;
    	}    
	
	wxArrayString ids ;
	TiXmlNode *x = doc.FirstChild ( "eSearchResult" ) ;
	if ( x ) 
		{
		x = x->FirstChild ( "IdList" ) ;
		if ( x )
			{
   			for ( x = x->FirstChild ( "Id" ) ; x ; x = x->NextSibling ( "Id" ) )
   				ids.Add ( x->FirstChild()->Value() ) ;
			}    
		}    
	
	if ( ids.IsEmpty() )
		{
 		hlb->Set ( 0 , "The search returned no results." ) ;
 		return ;
		}    
	
	int a ;
	res = "" ;
	for ( a = 0 ; a < ids.GetCount() ; a++ )
		{
  		if ( !res.IsEmpty() ) res += "," ;
		res += ids[a] ;
		}		

	query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?db=" + database + "&id=" + res + "&retmode=xml" ;
	res = ex.getText ( query ) ; // The XML is now in res
    doc.Parse ( res.c_str() ) ;
    if ( res == "" || doc.Error() )
    	{
	    hlb->Set ( 0 , txt("t_error") ) ;
	    return ;
    	}    
	
	x = doc.FirstChild ( "eSummaryResult" ) ;
	a = 0 ;
	for ( x = x->FirstChild ( "DocSum" ) ; x ; x = x->NextSibling ( "DocSum" ) )
		{
		TiXmlNode *y = x ;
		wxString s1 , s2 ;
		s2 = x->FirstChild ( "Id" ) -> FirstChild() -> Value() ;
		for ( y = x->FirstChild ( "Item" ) ; y ; y = y->NextSibling ( "Item" ) )
			{
   			wxString at = y->ToElement()->Attribute ( "Name" ) ;
   			at = at.Upper() ;
   			if ( at == "TITLE" )
   				s1 = y->FirstChild()->Value() ;
			}    
		hlb->Set ( a++ , s1 , s2 ) ;
		}    
	}    

void EIpanel::execute_pubmed()
	{
	// Opening selected items
	int a ;
	wxString ids ;
	for ( a = 0 ; a < hlb->was.GetCount() ; a++ )
		{
		if ( !hlb->IsSelected ( a ) ) continue ;
		
		if ( !ids.IsEmpty() ) ids += "," ;
		ids += hlb->data[a] ;
		}		
		
	wxString database = c1->GetStringSelection() ;
	database = database.Lower() ;
	wxString query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?" ;
	query += "db=" + database ;
	query += "&id=" + ids ; //hlb->data[a] ;
	query += "&retmode=xml&rettype=gb" ;

	myExternal ex ;
	wxString res = ex.getText ( query ) ;

    TXMLfile xml ;
    xml.parse ( res.c_str() ) ;
    if ( xml.success() ) myapp()->frame->newXML ( xml ) ;
	}	    
	
void EIpanel::OnC1 ( wxCommandEvent& event )
	{
	}
     
void EIpanel::OnB2 ( wxCommandEvent& event )
	{
	wxBeginBusyCursor () ;
	if ( mode == EI_PUBMED ) execute_pubmed() ;
	wxEndBusyCursor () ;
	}
     
void EIpanel::OnB1 ( wxCommandEvent& event )
	{
	wxBeginBusyCursor () ;
	if ( mode == EI_PUBMED ) process_pubmed() ;
//	hlb->Sort () ;
	hlb->Update () ;
	wxEndBusyCursor () ;
 	}    

