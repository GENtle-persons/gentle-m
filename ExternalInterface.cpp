#include "ExternalInterface.h"
#include "SendHTTP.h"

#define RETMAX 500

enum {
	ID_HLB = 6000,
	ID_T1,
	ID_T2,
	ID_T3,
	ID_T4,
	ID_B1,
	ID_B2,
	ID_C1,
	ID_C2,
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
	EVT_TEXT_ENTER(ID_T2, EIpanel::OnB1)
	EVT_TEXT_ENTER(ID_T3, EIpanel::OnB1)
	EVT_TEXT_ENTER(ID_T4, EIpanel::OnB1)
	EVT_CHOICE(ID_C1, EIpanel::OnC1)
    EVT_LISTBOX_DCLICK(ID_HLB, EIpanel::OnLboxDClick)
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
    
    nb->AddPage ( new EIpanel ( nb , EI_NCBI ) , "NCBI" ) ;

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
	if ( n % 3 == 0 ) bgcolor = "#FFFFFF" ;
	if ( n % 3 == 1 ) bgcolor = "#EEEEEE" ;
	if ( n % 3 == 2 ) bgcolor = "#DDDDDD" ;
	ret = "<table bgcolor='" + bgcolor + "' width='100%' cellspacing=0 cellpadding=0><tr><td>" + ret + "</td></tr></table>" ;
	return ret ;
	}
	
void EILB::Clear ()
	{
	DeselectAll () ;
	wxHtmlListBox::Clear () ;
	SetItemCount ( 0 ) ;
	was.Clear () ;
	data.Clear () ;
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
	hlb = new EILB ( this , ID_HLB ) ;
	
	if ( mode == EI_NCBI ) init_ncbi() ;

    v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( up , 0 , wxEXPAND , 5 ) ;
    v0->Add ( hlb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
//    v0->Fit ( this ) ;
	}
     
void EIpanel::init_ncbi()
	{
	t1 = new wxTextCtrl ( up , ID_T1 , "" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	b1 = new wxButton ( up , ID_B1 , txt("b_find") , wxDefaultPosition ) ;
	b2 = new wxButton ( up , ID_B2 , txt("t_open") , wxDefaultPosition ) ;
	c1 = new wxChoice ( up , ID_C1 ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Protein" ) ;
	c1->Append ( "PubMed" ) ;
	
	
/*
	structure, genome, pmc, omim, taxonomy, books, probeset,  domains, unists, cdd, snp, journals, unigene, popset
	c1->Append ( "" ) ;
*/

    v1 = new wxBoxSizer ( wxVERTICAL ) ;
    h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    h0->Add ( c1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( t1 , 1 , wxEXPAND , 5 ) ;
    h0->Add ( b1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b2 , 0 , wxEXPAND , 5 ) ;

	// PubMed-specific
	wxString off = "   " ;
    h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
	t2 = new wxTextCtrl ( up , ID_T2 , "" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	t3 = new wxTextCtrl ( up , ID_T3 , "" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	t4 = new wxTextCtrl ( up , ID_T4 , "" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	c2 = new wxChoice ( up , ID_C2 ) ;
	c2->Append ( "Author" ) ;
	c2->Append ( "Journal" ) ;
	c2->Append ( "Pub date" ) ;
	
	h1->Add ( new wxStaticText ( up , -1 , txt("author(s)") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( t2 , 1 , wxEXPAND , 5 ) ;
	h1->Add ( new wxStaticText ( up , -1 , off + txt("from") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( t3 , 1 , wxEXPAND , 5 ) ;
	h1->Add ( new wxStaticText ( up , -1 , off + txt("to") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( t4 , 1 , wxEXPAND , 5 ) ;
	h1->Add ( new wxStaticText ( up , -1 , off + txt("sort") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( c2 , 1 , wxEXPAND , 5 ) ;
	
    
    v1->Add ( h0 , 0 , wxEXPAND , 0 ) ;
    v1->Add ( h1 , 0 , wxEXPAND|wxTOP|wxBOTTOM , 3 ) ;
    up->SetSizer ( v1 ) ;
    v1->Fit ( up ) ;
    v1->Show ( h1 , false ) ; // Hide PubMed specifics

    c1->SetSelection ( 0 ) ;
    c2->SetSelection ( 2 ) ;
	t1->SetFocus() ;
    t1->SetSelection ( -1 , -1 ) ;
	}
	
void EIpanel::process_ncbi()
	{
	myExternal ex ;
	wxString database = c1->GetStringSelection() ;
	database = database.Lower() ;
	
	wxString search = t1->GetValue() ;
	wxString authors = t2->GetValue() ;
	wxString from = t3->GetValue() ;
	wxString to = t4->GetValue() ;
	wxString sort = c2->GetStringSelection().Lower() ;
	sort.Replace ( " " , "+" ) ;
	if ( database == "pubmed" && !authors.IsEmpty() )
		{
  		while ( !authors.IsEmpty() )
  			{
  			search = authors.BeforeFirst ( ',' ) + " [au] " + search ;
  			authors = authors.AfterFirst ( ',' ) ;
  			} 			
		}    
	search.Replace ( " " , "+" ) ;
	
	wxString query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?" ;
 	query += "db=" + database ;
	query += "&retmax=" + wxString::Format ( "%d" , RETMAX ) ;
    query += "&tool=GENtle" ;
    query += "&retmode=xml" ;
    if ( database == "pubmed" )
    	{
    	if ( !from.IsEmpty() ) query += "&mindate=" + from ;
    	if ( !to.IsEmpty() ) query += "&maxdate=" + to ;
    	query += "&sort=" + sort ;
    	}
    query += "&term=" + search ;
	query = wxURL::ConvertToValidURI ( query ) ;
	wxString res ;
	res = ex.getText ( query ) ; // The XML is now in res

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

	query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?" ;
 	query += "db=" + database ;
 	query += "&tool=GENtle&" ;
  	query += "id=" + res ;
  	query += "&retmode=xml" ;
	query += "&retmax=" + wxString::Format ( "%d" , RETMAX ) ;

	res = ex.getText ( query ) ; // The XML is now in res
    wxTheClipboard->Open(); wxTheClipboard->SetData( new wxTextDataObject(res) );    wxTheClipboard->Close();

    if ( res != "" ) doc.Parse ( res.c_str() ) ;
	x = doc.FirstChild ( "eSummaryResult" ) ;
    if ( res == "" || doc.Error() || !x )
    	{
	    hlb->Set ( 0 , txt("t_error") ) ;
	    return ;
    	}    
	
	a = 0 ;
	for ( x = x->FirstChild ( "DocSum" ) ; x ; x = x->NextSibling ( "DocSum" ) )
		{
		TiXmlNode *y = x ;
		wxString title , authors , source , pubdate , id ;
		id = x->FirstChild ( "Id" ) -> FirstChild() -> Value() ;
		for ( y = x->FirstChild ( "Item" ) ; y ; y = y->NextSibling ( "Item" ) )
			{
   			wxString at = y->ToElement()->Attribute ( "Name" ) ;
   			at = at.Upper() ;
   			if ( at == "TITLE" ) title = y->FirstChild()->Value() ;
   			else if ( at == "PUBDATE" ) pubdate = y->FirstChild()->Value() ;
   			else if ( at == "SOURCE" ) source = y->FirstChild()->Value() ;
   			else if ( at == "AUTHORLIST" ) 
   				{
			    TiXmlNode *z ;
			    for ( z = y->FirstChild ( "Item" ) ; z ; z = z->NextSibling ( "Item" ) )
			    	{
   	    			if ( wxString(z->ToElement()->Attribute("Name")).Upper() != "AUTHOR" ) continue ;
   	    			if ( !authors.IsEmpty() ) authors += ", " ;
   	    			authors += z->FirstChild()->Value() ;
			    	}    
   				}    
			}    
		wxString s ;
		if ( database == "nucleotide" ) s = title ;
		else if ( database == "protein" ) s = title ;
		else if ( database == "pubmed" )
			{
 			s = "<table width='100%'><tr>" ;
 			s += "<td colspan=3><b>" + title + "</b></td>" ;
 			s += "</tr><tr>" ;
 			s += "<td valign=top width='10%'>" + pubdate + "</td>" ;
 			s += "<td valign=top width='10%'>" + source + "</td>" ;
 			s += "<td valign=top>" + authors + "</td>" ;
 			s += "</tr></table>" ;
			}    
		hlb->Set ( a++ , s , id ) ;
		}    
	t1->SetFocus() ;
	}    

void EIpanel::execute_ncbi()
	{
	// Opening selected items
	int a ;
	wxString ids ;
	for ( a = 0 ; a < hlb->was.GetCount() ; a++ )
		{
		if ( !hlb->IsSelected ( a ) ) continue ;
		if ( hlb->data[a] == "" ) continue ;
		
		if ( !ids.IsEmpty() ) ids += "," ;
		ids += hlb->data[a] ;
		}		
		
	wxString database = c1->GetStringSelection() ;
	database = database.Lower() ;
	
	if ( database == "nucleotide" || database == "protein" )
		{
    	wxString query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?" ;
    	query += "db=" + database ;
    	query += "&tool=GENtle" ;
    	query += "&retmax=" + wxString::Format ( "%d" , RETMAX ) ;
    	query += "&id=" + ids ;
    	if ( database == "nucleotide" ) query += "&retmode=xml&rettype=gb" ;
      	else query += "&retmode=text&rettype=gp" ;
//      	else query += "&retmode=xml&rettype=gp" ; // One day, when the GenBankXML routine can read amino acids...
    
    	myExternal ex ;
    	wxString res = ex.getText ( query ) ;
//    wxTheClipboard->Open(); wxTheClipboard->SetData( new wxTextDataObject(res) );    wxTheClipboard->Close();    	
    
    	if ( database == "nucleotide" )
    		{
            TXMLfile xml ;
            xml.parse ( res.c_str() ) ;
            if ( xml.success() ) myapp()->frame->newXML ( xml ) ;
            }
        else
        	{
           TGenBank gb ;
           gb.paste ( res ) ;
           if ( gb.success ) myapp()->frame->newGB ( gb , "" ) ;
    	    }
        }    
    else if ( database == "pubmed" )
    	{
	    wxString query = "http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?" ;
	    query += "db=" + database ;
    	query += "&cmd=retrieve" ;
    	query += "&list_uids=" + ids ;
    	query += "&dopt=abstract" ;
    	wxExecute ( myapp()->getHTMLCommand ( query ) ) ;
    	}    
	}	    
	
void EIpanel::OnLboxDClick ( wxCommandEvent& event )
	{
	OnB2 ( event ) ;
	}    
	
void EIpanel::OnC1 ( wxCommandEvent& event )
	{
	if ( mode == EI_NCBI )
		{
		wxString database = c1->GetStringSelection() ;
		database = database.Lower() ;
		if ( database == "pubmed" ) 
			{
            v1->Show ( h1 , true ) ;
			}
		else
  			{
            v1->Show ( h1 , false ) ;
     		}        
   		v0->Layout() ;
		}    
	}
     
void EIpanel::OnB1 ( wxCommandEvent& event )
	{
	wxBeginBusyCursor () ;
	hlb->Clear () ;
	if ( mode == EI_NCBI ) process_ncbi() ;
//	hlb->Sort () ;
	hlb->Update () ;
	wxEndBusyCursor () ;
 	}    

void EIpanel::OnB2 ( wxCommandEvent& event )
	{
	wxBeginBusyCursor () ;
	if ( mode == EI_NCBI ) execute_ncbi() ;
	wxEndBusyCursor () ;
	}
     
