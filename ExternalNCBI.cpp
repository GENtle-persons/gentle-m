#include "ExternalInterface.h"

void EIpanel::init_ncbi()
	{
	    RETMAX = 25 ;
	t1 = new wxTextCtrl ( up , ID_T1 , "" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	b1 = new wxButton ( up , ID_B1 , txt("b_find") , wxDefaultPosition ) ;
	b2 = new wxButton ( up , ID_B2 , txt("t_open") , wxDefaultPosition ) ;
	c1 = new wxChoice ( up , ID_C1 ) ;
	c1->Append ( "Nucleotide" ) ;
	c1->Append ( "Protein" ) ;
	c1->Append ( "PubMed" ) ;

	b_last = new wxButton ( up , ID_B_LAST , txt("b_last") , wxDefaultPosition ) ;
	b_next = new wxButton ( up , ID_B_NEXT , txt("b_next") , wxDefaultPosition ) ;
	b_last->Disable () ;
	b_next->Disable () ;
	
	
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
    h0->Add ( b_last , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_next , 0 , wxEXPAND , 5 ) ;

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
	
    st_msg = new wxStaticText ( up , -1 , "" ) ;
    
    v1->Add ( h0 , 0 , wxEXPAND , 0 ) ;
    v1->Add ( h1 , 0 , wxEXPAND|wxTOP|wxBOTTOM , 3 ) ;
    v1->Add ( st_msg , 0 , wxEXPAND|wxTOP|wxBOTTOM , 3 ) ;
    up->SetSizer ( v1 ) ;
    v1->Fit ( up ) ;
    v1->Show ( h1 , false ) ; // Hide PubMed specifics
    v1->Show ( st_msg , false ) ; // Hide message

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
	
	// Invoking ESearch
	wxString query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?" ;
 	query += "db=" + database ;
	query += "&retstart=" + wxString::Format ( "%d" , res_start ) ;
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
	    showMessage ( txt("t_error") ) ;
	    return ;
    	}    
	
	// Extracting items from XML
	res_count = 0 ;
	wxArrayString ids ;
	TiXmlNode *x = doc.FirstChild ( "eSearchResult" ) ;
	if ( x ) 
		{
		TiXmlNode *y ;
		y = x->FirstChild ( "Count" ) ;
		if ( y ) valFC(y).ToLong ( &res_count ) ;
		y = x->FirstChild ( "ResStart" ) ;
		if ( y ) valFC(y).ToLong ( &res_start ) ;
		x = x->FirstChild ( "IdList" ) ;
		if ( x )
			{
   			for ( x = x->FirstChild ( "Id" ) ; x ; x = x->NextSibling ( "Id" ) )
   				ids.Add ( valFC ( x ) ) ;
			}    
		}    
//	wxMessageBox ( wxString::Format ( "%d of %d" , res_count , res_start ) ) ;
	
	if ( ids.IsEmpty() )
		{
 		showMessage ( "The search returned no results." ) ;
 		return ;
		}    
	
	int a ;
	res = "" ;
	for ( a = 0 ; a < ids.GetCount() ; a++ )
		{
  		if ( !res.IsEmpty() ) res += "," ;
		res += ids[a] ;
		}		
		
	// Invoking ESummary
	query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?" ;
 	query += "db=" + database ;
 	query += "&tool=GENtle&" ;
  	query += "id=" + res ;
  	query += "&retmode=xml" ;
	query += "&retmax=" + wxString::Format ( "%d" , RETMAX ) ;

	res = ex.getText ( query ) ; // The XML is now in res
//    wxTheClipboard->Open(); wxTheClipboard->SetData( new wxTextDataObject(res) );    wxTheClipboard->Close();

    if ( res != "" ) doc.Parse ( res.c_str() ) ;
	x = doc.FirstChild ( "eSummaryResult" ) ;
    if ( res == "" || doc.Error() || !x )
    	{
	    showMessage ( txt("t_error") ) ;
	    return ;
    	}    
	
	// Parsing ESummary XML
	a = 0 ;
	for ( x = x->FirstChild ( "DocSum" ) ; x ; x = x->NextSibling ( "DocSum" ) )
		{
		TiXmlNode *y = x ;
		wxString title , authors , source , pubdate , id , caption ;
		id = valFC ( x->FirstChild ( "Id" ) ) ;
		for ( y = x->FirstChild ( "Item" ) ; y ; y = y->NextSibling ( "Item" ) )
			{
   			wxString at = y->ToElement()->Attribute ( "Name" ) ;
   			at = at.Upper() ;
   			if ( at == "TITLE" ) title = valFC ( y ) ;
   			else if ( at == "PUBDATE" ) pubdate = valFC ( y ) ;
   			else if ( at == "SOURCE" ) source = valFC ( y ) ;
   			else if ( at == "CAPTION" ) caption = valFC ( y ) ;
   			else if ( at == "AUTHORLIST" ) 
   				{
			    TiXmlNode *z ;
			    for ( z = y->FirstChild ( "Item" ) ; z ; z = z->NextSibling ( "Item" ) )
			    	{
   	    			if ( wxString(z->ToElement()->Attribute("Name")).Upper() != "AUTHOR" ) continue ;
   	    			if ( !authors.IsEmpty() ) authors += ", " ;
   	    			authors += valFC ( z ) ;
			    	}    
   				}    
			}    
		wxString s ;
		if ( database == "nucleotide" || database == "protein" )
  			{
 			s = "<table width='100%'><tr>" ;
 			s += "<td align=right valign=center width='50px'>" ;
 			s += wxString::Format ( "%d" , a + res_start + 1 ) ;
 			s += "</td>" ;
 			s += "<td width='10%'>" + caption + "</td>" ;
 			s += "<td>" + title + "</td>" ;
 			s += "</tr></table>" ;
          	}
		else if ( database == "pubmed" )
			{
 			s = "<table width='100%'><tr>" ;
 			s += "<td rowspan=2 align=right valign=top width='50px'>" ;
 			s += wxString::Format ( "%d" , a + res_start + 1 ) ;
 			s += "</td>" ;
 			s += "<td colspan=3><b>" + title + "</b></td>" ;
 			s += "</tr><tr>" ;
 			s += "<td valign=top width='10%'>" + pubdate + "</td>" ;
 			s += "<td valign=top width='10%'>" + source + "</td>" ;
 			s += "<td valign=top>" + authors + "</td>" ;
 			s += "</tr></table>" ;
			}    
		hlb->Set ( a++ , s , id ) ;
		}    
		
	int res_to = res_start + RETMAX ;
	if ( res_to > res_count ) res_to = res_count ;
	wxString msg = wxString::Format ( txt("t_ext_show_res") , res_start + 1 , res_to , res_count ) ;
	msg += " (" + wxString ( txt("t_data_by_ncbi") ) + ")" ;
	showMessage ( msg  ) ;
	b_next->Enable ( res_start + RETMAX <= res_count ) ;
	b_last->Enable ( res_start > 0 ) ;
	t1->SetFocus() ;
	}    

void EIpanel::execute_ncbi()
	{
	    wxString database = c1->GetStringSelection() ;
	    execute_ncbi_load ( database ) ;
	}

void EIpanel::execute_ncbi_load ( wxString database )
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
		
	database = database.Lower() ;
	
	if ( database == "nucleotide" || database == "protein" )
	{
	    wxString query = "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?" ;
	    query += "db=" + database ;
	    query += "&tool=GENtle" ;
//	    query += "&retmax=" + wxString::Format ( "%d" , RETMAX ) ;
	    query += "&id=" + ids ;
	    if ( database == "nucleotide" ) query += "&retmode=xml&rettype=gb" ;
	    else query += "&retmode=text&rettype=gp" ;
	    
	    myExternal ex ;
	    wxString res = ex.getText ( query ) ;
	    
//    wxTheClipboard->Open(); wxTheClipboard->SetData( new wxTextDataObject(res) );    wxTheClipboard->Close();    	

	    if ( database == "nucleotide" )
	    {
		if ( res.Left ( 5 ) == "LOCUS" )
		{
		    TGenBank gb ;
		    gb.paste ( res ) ;
//	wxMessageBox ( "GB" , wxString::Format ( "%d" , gb.success ) ) ;
		    if ( gb.success ) myapp()->frame->newGB ( gb ) ;
		}
		else
		{
		    TXMLfile xml ;
		    xml.parse ( res ) ;
//	wxMessageBox ( res , wxString::Format ( "%d" , xml.success() ) ) ;
		    if ( xml.success() ) myapp()->frame->newXML ( xml ) ;
		}
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
