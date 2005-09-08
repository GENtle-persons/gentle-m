/** \file
	\brief Contains the EIpanel functions for NCBI
*/
#include "ExternalInterface.h"

void EIpanel::init_ncbi()
	{
	    RETMAX = 25 ;
	t1 = new wxTextCtrl ( up , ID_T1 , _T("") , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	b1 = new wxButton ( up , ID_B1 , txt("b_find") , wxDefaultPosition ) ;
	b2 = new wxButton ( up , ID_B2 , txt("t_open") , wxDefaultPosition ) ;
   b3 = new wxButton ( up , ID_B3 , txt("b_open_link") , wxDefaultPosition ) ;
	c1 = new wxChoice ( up , ID_C1 ) ;
	c1->Append ( _T("Nucleotide") ) ;
	c1->Append ( _T("Protein") ) ;
	c1->Append ( _T("PubMed") ) ;

	b_last = new wxButton ( up , ID_B_LAST , txt("b_last") , wxDefaultPosition ) ;
	b_next = new wxButton ( up , ID_B_NEXT , txt("b_next") , wxDefaultPosition ) ;
	b_last->Disable () ;
	b_next->Disable () ;
	b3->Disable () ;
	
	
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
    h0->Add ( b3 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_last , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_next , 0 , wxEXPAND , 5 ) ;

	// PubMed-specific
	wxString off = _T("   ") ;
    h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
	t2 = new wxTextCtrl ( up , ID_T2 , _T("") , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	t3 = new wxTextCtrl ( up , ID_T3 , _T("") , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	t4 = new wxTextCtrl ( up , ID_T4 , _T("") , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	c2 = new wxChoice ( up , ID_C2 ) ;
	c2->Append ( _T("Author") ) ;
	c2->Append ( _T("Journal") ) ;
	c2->Append ( _T("Pub date") ) ;
	
	h1->Add ( new wxStaticText ( up , -1 , txt("author(s)") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( t2 , 1 , wxEXPAND , 5 ) ;
	h1->Add ( new wxStaticText ( up , -1 , off + txt("from") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( t3 , 1 , wxEXPAND , 5 ) ;
	h1->Add ( new wxStaticText ( up , -1 , off + txt("to") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( t4 , 1 , wxEXPAND , 5 ) ;
	h1->Add ( new wxStaticText ( up , -1 , off + txt("sort") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( c2 , 1 , wxEXPAND , 5 ) ;
	
    st_msg = new wxStaticText ( up , -1 , _T("") ) ;
    
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
	sort.Replace ( _T(" ") , _T("+") ) ;
	if ( database == _T("pubmed") && !authors.IsEmpty() )
		{
  		while ( !authors.IsEmpty() )
  			{
  			search = authors.BeforeFirst ( ',' ) + _T(" [au] ") + search ;
  			authors = authors.AfterFirst ( ',' ) ;
  			} 			
		}    
	search.Replace ( _T(" ") , _T("+") ) ;
	
	// Invoking ESearch
	wxString query = _T("http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?") ;
 	query += _T("db=") + database ;
	query += _T("&retstart=") + wxString::Format ( _T("%d") , res_start ) ;
	query += _T("&retmax=") + wxString::Format ( _T("%d") , RETMAX ) ;
    query += _T("&tool=GENtle") ;
    query += _T("&retmode=xml") ;
    if ( database == _T("pubmed") )
    	{
    	if ( !from.IsEmpty() ) query += _T("&mindate=") + from ;
    	if ( !to.IsEmpty() ) query += _T("&maxdate=") + to ;
    	query += _T("&sort=") + sort ;
    	}
    query += _T("&term=") + search ;
//	query = wxURL::ConvertToValidURI ( query ) ;//!!!!!!!!!!!!!

//	wxURI uri ( query ) ;
//	query = uri.BuildUnescapedURI () ;

	wxString res ;
	res = ex.getText ( query ) ; // The XML is now in res

    TiXmlDocument doc ;
    doc.Parse ( res.mb_str() ) ;
    if ( res == _T("") || doc.Error() )
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
//	wxMessageBox ( wxString::Format ( _T("%d of %d") , res_count , res_start ) ) ;
	
	if ( ids.IsEmpty() )
		{
 		showMessage ( _T("The search returned no results.") ) ;
 		return ;
		}    
	
	int a ;
	res = _T("") ;
	for ( a = 0 ; a < ids.GetCount() ; a++ )
		{
  		if ( !res.IsEmpty() ) res += _T(",") ;
		res += ids[a] ;
		}		
		
	// Invoking ESummary
	query = _T("http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?") ;
 	query += _T("db=") + database ;
 	query += _T("&tool=GENtle&") ;
  	query += _T("id=") + res ;
  	query += _T("&retmode=xml") ;
	query += _T("&retmax=") + wxString::Format ( _T("%d") , RETMAX ) ;

	res = ex.getText ( query ) ; // The XML is now in res
//cout << res.mb_str() << endl ;
//    wxTheClipboard->Open(); wxTheClipboard->SetData( new wxTextDataObject(res) );    wxTheClipboard->Close();

    if ( res != _T("") ) doc.Parse ( res.mb_str() ) ;
	x = doc.FirstChild ( "eSummaryResult" ) ;
    if ( res == _T("") || doc.Error() || !x )
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
   			wxString at ( y->ToElement()->Attribute ( "Name" ) , wxConvUTF8 ) ;
   			at = at.Upper() ;
   			if ( at == _T("TITLE") ) title = valFC ( y ) ;
   			else if ( at == _T("PUBDATE") ) pubdate = valFC ( y ) ;
   			else if ( at == _T("SOURCE") ) source = valFC ( y ) ;
   			else if ( at == _T("CAPTION") ) caption = valFC ( y ) ;
   			else if ( at == _T("AUTHORLIST") )
   				{
			    TiXmlNode *z ;
			    for ( z = y->FirstChild ( "Item" ) ; z ; z = z->NextSibling ( "Item" ) )
			    	{
   	    			if ( wxString(z->ToElement()->Attribute ( "Name" ),wxConvUTF8).Upper() != _T("AUTHOR") ) continue ;
   	    			if ( !authors.IsEmpty() ) authors += _T(", ") ;
   	    			authors += valFC ( z ) ;
			    	}    
   				}    
			}    
		wxString s ;
		if ( database == _T("nucleotide") || database == _T("protein") )
  			{
 			s = _T("<table width='100%'><tr>") ;
 			s += _T("<td align=right valign=center width='50px'>") ;
 			s += wxString::Format ( _T("%d") , a + res_start + 1 ) ;
 			s += _T("</td>") ;
 			s += _T("<td width='10%'>") + caption + _T("</td>") ;
 			s += _T("<td>") + title + _T("</td>") ;
 			s += _T("</tr></table>") ;
          	}
		else if ( database == _T("pubmed") )
			{
 			s = _T("<table width='100%'><tr>") ;
 			s += _T("<td rowspan=2 align=right valign=top width='50px'>") ;
 			s += wxString::Format ( _T("%d") , a + res_start + 1 ) ;
 			s += _T("</td>") ;
 			s += _T("<td colspan=3><b>") + title + _T("</b></td>") ; // !!!!!!!!!!!!!!!!!!!!
 			s += _T("</tr><tr>") ;
 			s += _T("<td valign=top width='10%'>") + pubdate + _T("</td>") ;
 			s += _T("<td valign=top width='10%'>") + source + _T("</td>") ;
 			s += _T("<td valign=top>") + authors + _T("</td>") ;
 			s += _T("</tr></table>") ;
			}    
		hlb->Set ( a++ , s , id ) ;
		}    
		
	int res_to = res_start + RETMAX ;
	if ( res_to > res_count ) res_to = res_count ;
	wxString msg = wxString::Format ( txt("t_ext_show_res") , res_start + 1 , res_to , res_count ) ;
	msg += _T(" (") + wxString ( txt("t_data_by_ncbi") ) + _T(")") ;
	showMessage ( msg  ) ;
	b_next->Enable ( res_start + RETMAX <= res_count ) ;
	b_last->Enable ( res_start > 0 ) ;
   b3->Enable ( true ) ;
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
		if ( hlb->data[a] == _T("") ) continue ;
		
		if ( !ids.IsEmpty() ) ids += _T(",") ;
		ids += hlb->data[a] ;
		}		
	
	database = database.Lower() ;
	
	if ( database == _T("nucleotide") || database == _T("protein") ) // Requesting sequence
		{
		wxString query = _T("http://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?") ;
		query += _T("db=") + database ;
		query += _T("&tool=GENtle") ;
		//	    query += "&retmax=" + wxString::Format ( "%d" , RETMAX ) ;
		query += _T("&id=") + ids ;
		if ( database == _T("nucleotide") ) query += _T("&retmode=xml&rettype=gb") ;
		else query += _T("&retmode=text&rettype=gp") ;
		
		myExternal ex ;
		wxString res = ex.getText ( query ) ;
		
		//    wxTheClipboard->Open(); wxTheClipboard->SetData( new wxTextDataObject(res) );    wxTheClipboard->Close();    	
		
		if ( database == _T("nucleotide") )
			{
			if ( res.Left ( 5 ) == _T("LOCUS") )
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
			if ( gb.success ) 
				myapp()->frame->newGB ( gb , _T("") ) ;
			}
		}    
	else if ( database == _T("pubmed") ) // Requesting paper
		{
		wxString query = _T("http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?") ;
		query += _T("db=") + database ;
		query += _T("&cmd=retrieve") ;
		query += _T("&list_uids=") + ids ;
		query += _T("&dopt=abstract") ;
		wxExecute ( myapp()->getHTMLCommand ( query ) ) ;
		}    
	}	    

void EIpanel::execute_ncbi_b3()
	{
	wxString script , database = c1->GetStringSelection().Lower() ;
	if ( database == _T("pubmed") )
		{
		database += _T("&dopt=Abstract&cmd=Retrieve") ;
		script = "query" ;
		}
	else script = "viewer" ;
	for ( int i = 0 ; i < hlb->data.GetCount() ; i++ )
		{
		if ( !hlb->IsSelected ( i ) ) continue ;
		wxString s = hlb->data[i] ;
		
		s = s.BeforeFirst ( '|' ) ;
		s = _T("http://www.ncbi.nlm.nih.gov/entrez/") + script + _T(".fcgi?db=") + database + _T("&list_uids=") + s ;
		s = myapp()->getHTMLCommand ( s ) ;
		wxExecute ( s ) ;
		}
	}
