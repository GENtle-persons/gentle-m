#include "ExternalInterface.h"
#include "SendHTTP.h"
#include <wx/thread.h>

#define RETMAX 25

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
    EVT_BUTTON(ID_B_LAST, EIpanel::OnBlast)
    EVT_BUTTON(ID_B_NEXT, EIpanel::OnBnext)
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
    nb->AddPage ( new EIpanel ( nb , EI_BLAST ) , "BLAST" ) ;

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
    
    blast_thread = NULL ;

    up = new wxPanel ( this ) ;
    hlb = new EILB ( this , ID_HLB ) ;
    
    if ( mode == EI_NCBI ) init_ncbi() ;
    if ( mode == EI_BLAST ) init_blast() ;
    
    v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( up , 0 , wxEXPAND , 5 ) ;
    v0->Add ( hlb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
}
     
void EIpanel::init_blast()
{
    t1 = new wxTextCtrl ( up , ID_T1 , "" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
    b1 = new wxButton ( up , ID_B1 , txt("b_find") , wxDefaultPosition ) ;
    b2 = new wxButton ( up , ID_B2 , txt("t_open") , wxDefaultPosition ) ;
    c1 = new wxChoice ( up , ID_C1 ) ;
    c1->Append ( "Protein" ) ;
    c1->Append ( "Nucleotide" ) ;

    b_last = new wxButton ( up , ID_B_LAST , txt("b_last") , wxDefaultPosition ) ;
    b_next = new wxButton ( up , ID_B_NEXT , txt("b_next") , wxDefaultPosition ) ;
    b_last->Disable () ;
    b_next->Disable () ;

    v1 = new wxBoxSizer ( wxVERTICAL ) ;
    h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    h0->Add ( c1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( t1 , 1 , wxEXPAND , 5 ) ;
    h0->Add ( b1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b2 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_last , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_next , 0 , wxEXPAND , 5 ) ;

    st_msg = new wxStaticText ( up , -1 , "" ) ;

    v1->Add ( h0 , 0 , wxEXPAND , 0 ) ;
//    v1->Add ( h1 , 0 , wxEXPAND|wxTOP|wxBOTTOM , 3 ) ;
    v1->Add ( st_msg , 0 , wxEXPAND|wxTOP|wxBOTTOM , 3 ) ;
    up->SetSizer ( v1 ) ;

    c1->SetSelection ( 0 ) ;
    t1->SetValue ( "MSPILGYWKIKGLVQPTRLLLEYLEEKYEEHLYERDEGDKWRNKKFELGLEFPNLPYYIDGDVKLTQSMAIIRYIADKHNMLGGCPKERAEISMLEGAVLDIRYGVSRIAYSKDFETLKVDFLSKLPEMLKMFEDRLCHKTYLNGDHVTHPDFMLYDALDVVLYMDPMCLDAFPKLVCFKKRIEAIPQIDKYLKSSKYIAWPLQGWQATFGGGDHPPKSDLIEGRGIENLYFQGIPGNSS" ) ;
    t1->SetFocus() ;
}


#if !wxUSE_THREADS
    #error "This requires thread support!"
#endif // wxUSE_THREADS

class blastThread : public wxThread
{
public :
    blastThread ( EIpanel *panel , wxString seq ) : wxThread ()
	{
	    wxThread::Yield() ;

	    p = panel ;
	    // Put
/*	    url = "http://www.ncbi.nlm.nih.gov/blast/Blast.cgi?" ;
	    url += "CMD=Put" ;
	    url += "&QUERY=" + seq ;
	    url += "&DATABASE=nr" ;
	    url += "&PROGRAM=blastp" ;
	    url += "&HITLIST_SIZE=10" ;
	    
	    res = ex.getText ( url ) ;
	    
	    hs = parseQblast ( res ) ;
	    RID = hs["RID"] ;
	    RTOE = hs["RTOE"] ;
	    
	    // Get & wait
	    RTOE.ToLong ( &wait ) ;
	    url = "http://www.ncbi.nlm.nih.gov/blast/Blast.cgi?" ;
	    url += "CMD=Get" ;
	    url += "&RID=" + RID ;
	    url += "&FORMAT_TYPE=XML" ;	    
*/
	} ;

    virtual void *Entry ()
	{
#ifdef __WXMSW__
    wxString fn = "C:\\blast.html" ;
#else
    wxString fn = "/home/manske/blast.html" ;
#endif
	    
/*
	    do {
		while ( wait )
		{
		    wxMutexGuiEnter() ;
		    p->showMessage ( wxString::Format ( txt("t_blast_time") , wait ) ) ;
		    wxMutexGuiLeave() ;
		    wxThread::Sleep ( 1000 ) ; // Wait 1 sec
		    wait-- ;
		    if ( TestDestroy() ) wxThread::Exit() ;
		}
		wxMutexGuiEnter() ;
		res = ex.getText ( url ) ;
		wxMutexGuiLeave() ;
		hs = parseQblast ( res ) ;
		if ( hs["STATUS"].Upper() == "WAITING" ) wait = 10 ; // Wait another 10 seconds
		else wait = 0 ; // Done!
	    } while ( wait ) ;
	    wxFile out ( fn , wxFile::write ) ; out.Write ( res ) ; out.Close() ;
*/

	    // Dirty hack
	    wxMutexGuiEnter() ;
	    wxFile in ( fn , wxFile::read ) ; 
	    char *c = new char[in.Length()+5] ;
	    in.Read ( c , in.Length() ) ;
	    in.Close () ;
	    res = c ;
	    delete c ;
	    // End dirty hack

	    p->blast_res = res ;

	    wxMutexGuiLeave() ;

	    wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, ID_B1 );
	    wxPostEvent ( p , event ) ;

//	    wxMutexGuiEnter() ;
//	    p->process_blast2 () ;
//	    wxMutexGuiLeave() ;

	    wxThread::Exit() ;
//	    delete this ;
	    return NULL ;
	}
private :

    wxHashString parseQblast ( wxString res )
	{
	    wxHashString ret ;
	    wxString q1 = "QBlastInfoBegin" ;
	    wxString q2 = "QBlastInfoEnd" ;
	    
	    int i ;
	    i = res.First ( q1 ) ;
	    if ( i == -1 ) return ret ;
	    res = res.Mid ( i + q1.Length() ) ;
	    
	    i = res.First ( q2 ) ;
	    if ( i == -1 ) return ret ;
	    res = res.Left ( i - 1 ) ;
	    
	    while ( !res.IsEmpty() )
	    {
		wxString k , v ;
		k = res.BeforeFirst ( '\n' ) ;
		res = res.AfterFirst ( '\n' ) ;
		v = k.AfterFirst ( '=' ) ;
		k = k.BeforeFirst ( '=' ) ;
		v = v.Trim(true).Trim(false) ;
		k = k.Trim(true).Trim(false) ;
		if ( v.IsEmpty() || k.IsEmpty() ) continue ;
		k.MakeUpper() ;
		ret[k] = v ;
	    }
	    return ret ;
	}
    
    wxString res , url , RID , RTOE ;
    myExternal ex ;
    wxHashString hs ;
    long wait ;
    EIpanel *p ;
} ;

void EIpanel::process_blast() // This starts the thread
{
    if ( blast_thread )
    {
	process_blast2() ;
	return ;
    }

    blast_res.Empty() ;
    wxString seq = t1->GetValue() ;
    blast_thread = new blastThread ( this , seq ) ;
    if ( !blast_thread || wxTHREAD_NO_ERROR != blast_thread->Create() )
    {
	blast_thread = NULL ;
//	wxMessageBox ( txt("t_blast_failed") ) ;
	return ;
    }

    if ( wxTHREAD_NO_ERROR != blast_thread->Run() )
    {
	blast_thread = NULL ;
//	wxMessageBox ( txt("t_blast_failed") ) ;
	return ;
    }

    b1->Disable() ;
}

void EIpanel::process_blast2() // This is called upon termination of the thread
{
    if ( blast_thread ) blast_thread->Wait();
    blast_thread = NULL ;
    b1->Enable() ;

    TiXmlDocument blast_doc ;
    blast_doc.SetCondenseWhiteSpace(false);
    blast_doc.Parse ( blast_res.c_str() ) ;

    hlb->Clear () ;
    showMessage ( txt("t_blast_failed") ) ;
    TiXmlNode *x = blast_doc.FirstChild ( "BlastOutput" ) ;
    if ( !x ) return ;
    
    int w , h ;
    hlb->GetClientSize ( &w , &h ) ;
    w /= 8 ;
    
    wxString blast_version = valFC ( x->FirstChild ( "BlastOutput_version" ) ) ;
    showMessage ( wxString::Format ( txt("t_blast_results_by" ) , blast_version.c_str() ) ) ;

    x = x->FirstChild ( "BlastOutput_iterations" ) ;
    x = x->FirstChild ( "Iteration" ) ;
    x = x->FirstChild ( "Iteration_hits" ) ;
    
    int a = 0 ;
    for ( x = x->FirstChild ( "Hit" ) ; x ; x = x->NextSibling ( "Hit" ) )
    {
	wxString html ;
	wxString name = valFC ( x->FirstChild ( "Hit_def" ) ) ;
	wxString id = valFC ( x->FirstChild ( "Hit_id" ) ) ;

	TiXmlNode *h = x->FirstChild ( "Hit_hsps" ) ;
	h = h->FirstChild ( "Hsp" ) ;
	wxString evalue = valFC ( h->FirstChild ( "Hsp_evalue" ) ) ;
	if ( evalue.Find ( 'e' ) > -1 )
	{
	    wxString base = evalue.BeforeFirst ( 'e' ) + "&times;10" ;
	    wxString exp = "<font size=2>" + evalue.AfterFirst ( 'e' )  + "</font>" ;
	    
	    evalue = "<table border=0 cellpadding=0 cellspacing=0><tr><td rowspan=2 valign=bottom>E-Value=</td>" ;
	    evalue += "<td align=left valign=bottom><br>" + base + "</td>" ;
	    evalue += "<td align=right valign=top>" + exp + "</td>" ;
	    evalue += "</tr></table>" ;
	}
	else evalue = "E-Value=" + evalue ;
	
	wxString qseq = valFC ( h->FirstChild ( "Hsp_qseq" ) ) ;
	wxString mseq = valFC ( h->FirstChild ( "Hsp_midline" ) ) ;
	wxString hseq = valFC ( h->FirstChild ( "Hsp_hseq" ) ) ;

	html = "<table width=100%><tr>" ;
	html += "<td valign=top>" + name + "</td>" ;
	html += "<td align=right valign=top>" + evalue + "</td>" ;
	html += "</tr><tr>" ;
	html += "<td colspan=2><tt><font size=2>\n" ;
	html += blast_align ( qseq , mseq , hseq , w ) ;
	html += "</font></tt></td>" ;
	html += "</tr></table>" ;
	hlb->Set ( a++ , html , id ) ;
    }
    hlb->Update () ;
}

wxString EIpanel::blast_align ( wxString qseq , wxString mseq , wxString hseq , int cpl )
{
    wxString lead[3] ;
    lead[0] = txt("t_blast_qseq" ) ;
    lead[2] = txt("t_blast_hseq" ) ;
    while ( lead[0].Length() < lead[2].Length() ) lead[0] += " " ;
    while ( lead[0].Length() > lead[2].Length() ) lead[2] += " " ;
    lead[1].Append ( ' ' , lead[0].Length() ) ;

    cpl -= lead[0].Length() + 1 ;

    // Unify length
    int max = qseq.Length() ;
    max = max < mseq.Length() ? mseq.Length() : max ;
    max = max < hseq.Length() ? hseq.Length() : max ;
    while ( max % cpl ) max++ ;
    qseq.Append ( ' ' , max - qseq.Length() ) ;
    mseq.Append ( ' ' , max - mseq.Length() ) ;
    hseq.Append ( ' ' , max - hseq.Length() ) ;

    wxString ret ;
    while ( !qseq.IsEmpty() )
    {
//	ret += "<p>" ;
	ret += lead[0] + " " + qseq.Left ( cpl ) + "\n" ;
	ret += lead[1] + " " + mseq.Left ( cpl ) + "\n" ;
	ret += lead[2] + " " + hseq.Left ( cpl ) + "\n" ;
	qseq = qseq.Mid ( cpl ) ;
	mseq = mseq.Mid ( cpl ) ;
	hseq = hseq.Mid ( cpl ) ;
	if ( !qseq.IsEmpty() ) ret += "<p></p>\n" ;
    }
    ret.Replace ( " " , "&nbsp;" ) ;
    return ret ;
}

void EIpanel::execute_blast()
{
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
	
wxString EIpanel::valFC ( TiXmlNode *n )
	{
	if ( !n ) return "" ;
	if ( !n->FirstChild() ) return "" ;
	return val ( n->FirstChild() ) ;
	}
     
wxString EIpanel::val ( TiXmlNode *n )
	{
	if ( !n ) return "" ;
	if ( !n->Value() ) return "" ;
	return wxString ( n->Value() ) ;
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
    
    	myExternal ex ;
    	wxString res = ex.getText ( query ) ;
    	
    wxTheClipboard->Open(); wxTheClipboard->SetData( new wxTextDataObject(res) );    wxTheClipboard->Close();    	

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
	res_start = 0 ;
	b_next->Disable () ;
	b_last->Disable () ;
	process () ;
 	}    

void EIpanel::OnB2 ( wxCommandEvent& event )
	{
	wxBeginBusyCursor () ;
	if ( mode == EI_NCBI ) execute_ncbi() ;
	wxEndBusyCursor () ;
	}

void EIpanel::OnBlast ( wxCommandEvent& event )
	{
	if ( res_start == 0 ) return ;
	res_start -= RETMAX ;
	process () ;
	}
     
void EIpanel::OnBnext ( wxCommandEvent& event )
	{
	if ( res_start + RETMAX > res_count ) return ;
	res_start += RETMAX ;
	process () ;
	}
     
void EIpanel::process ()
	{
	wxBeginBusyCursor () ;
	hlb->Clear () ;
	if ( mode == EI_NCBI ) process_ncbi() ;
	if ( mode == EI_BLAST ) process_blast() ;
	hlb->Update () ;
	wxEndBusyCursor () ;
	}    
	
wxString EIpanel::num2html ( int num , int digits )
	{
	wxString s = wxString::Format ( "%d" , num ) ;
	while ( s.length() < digits ) s = " " + s ;
	s.Replace ( " " , "&nbsp;" ) ;
	return s ;
	}    
	
void EIpanel::showMessage ( wxString msg )
	{
	if ( !st_msg->IsShown() )
		{
		v1->Show ( st_msg , true ) ;
		v0->Layout () ;
		}		
	st_msg->SetLabel ( msg ) ;
	}
     
