#include "ExternalInterface.h"

void EIpanel::init_blast()
{
    RETMAX = 10 ;
    t1 = new wxTextCtrl ( up , ID_T1 , "" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
    b1 = new wxButton ( up , ID_B1 , txt("b_find") , wxDefaultPosition ) ;
    b2 = new wxButton ( up , ID_B2 , txt("t_open") , wxDefaultPosition ) ;
    c1 = new wxChoice ( up , ID_C1 ) ;
    c1->Append ( "Protein" ) ;
    c1->Append ( "Nucleotide" ) ;

    c2 = new wxChoice ( up , ID_C2 ) ;
    c2->Append ( "50" ) ;
    c2->Append ( "100" ) ;
    c2->Append ( "250" ) ;
    c2->Append ( "500" ) ;
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
    h0->Add ( c2 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_last , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_next , 0 , wxEXPAND , 5 ) ;

    st_msg = new wxStaticText ( up , -1 , "" ) ;

    v1->Add ( h0 , 0 , wxEXPAND , 0 ) ;
//    v1->Add ( h1 , 0 , wxEXPAND|wxTOP|wxBOTTOM , 3 ) ;
    v1->Add ( st_msg , 0 , wxEXPAND|wxTOP|wxBOTTOM , 3 ) ;
    up->SetSizer ( v1 ) ;

    c1->SetSelection ( 0 ) ;
    c2->SetSelection ( 0 ) ;
    t1->SetFocus() ;
}


#if !wxUSE_THREADS
    #error "This requires thread support!"
#endif // wxUSE_THREADS

//#define BLAST_TEST_MODE

class blastThread : public wxThread
{
public :
    blastThread ( EIpanel *panel , wxString seq ) : wxThread ()
	{
	    p = panel ;
#ifndef BLAST_TEST_MODE
	    // Put
	    url = "http://www.ncbi.nlm.nih.gov/blast/Blast.cgi?" ;
	    url += "CMD=Put" ;
	    url += "&QUERY=" + seq ;
	    url += "&DATABASE=nr" ;
	    if ( c1->GetSelection() == 0 ) url += "&PROGRAM=blastp" ;
	    if ( c1->GetSelection() == 1 ) url += "&PROGRAM=blastn" ;
	    url += "&HITLIST_SIZE=" + p->c2->GetStringSelection() ;
	    
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
#endif
	} ;

    virtual void OnExit() 
	{
	}

    virtual void *Entry ()
	{
#ifdef __WXMSW__
    wxString fn = "C:\\blast.html" ;
#else
    wxString fn = "/home/manske/blast.html" ;
#endif
	    
#ifndef BLAST_TEST_MODE
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
	    p->blast_res = res ;
	    wxFile out ( fn , wxFile::write ) ; out.Write ( res ) ; out.Close() ; // For testing
#else
	    // Dirty hack
	    wxMutexGuiEnter() ;
	    wxFile in ( fn , wxFile::read ) ; 
	    p->blast_res = "" ;
	    char c[10005] ;
	    while ( !in.Eof() )
	    {
		int l = in.Read ( c , 10000 ) ;
		if ( l >= 0 ) c[l] = 0 ;
		else *c = 0 ;
		p->blast_res += c ;
	    }
	    in.Close () ;
	    // End dirty hack

	    wxMutexGuiLeave() ;
#endif
	    wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, ID_B1 );
	    wxPostEvent ( p , event ) ;

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
    if ( !blast_res.IsEmpty() || blast_thread ) // If thread is running, or results are there...
    {
	process_blast2() ;
	return ;
    }

    // Start thread
    blast_res.Empty() ;
    wxString seq = t1->GetValue() ;
    blast_thread = new blastThread ( this , seq ) ;
    if ( !blast_thread || wxTHREAD_NO_ERROR != blast_thread->Create() )
    {
	blast_thread = NULL ;
	return ;
    }

    if ( wxTHREAD_NO_ERROR != blast_thread->Run() )
    {
	blast_thread = NULL ;
	return ;
    }

    b1->Disable() ;
}

void EIpanel::process_blast2() // This is called upon termination of the thread
{
    bool initial = false ;
    if ( blast_thread )
    {
	blast_thread = NULL ;
	b1->Enable() ;
	initial = true ;
	res_count = 0 ;
	res_start = 0 ;
    }

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

    x = x->FirstChild ( "BlastOutput_iterations" ) ;
    x = x->FirstChild ( "Iteration" ) ;
    x = x->FirstChild ( "Iteration_hits" ) ;
    
    int a = 0 ;
    for ( x = x->FirstChild ( "Hit" ) ; x ; x = x->NextSibling ( "Hit" ) , a++ )
    {

	if ( initial )
	{
	    res_count++ ;
	    if ( a >= RETMAX ) continue ;
	}
	else
	{
	    if ( a < res_start ) continue ;
	    if ( a >= res_start + RETMAX ) break ;
	}

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
	long qoff , hoff ;
	valFC ( h->FirstChild ( "Hsp_query-from" ) ) . ToLong ( &qoff ) ;
	valFC ( h->FirstChild ( "Hsp_hit-from" ) ) . ToLong ( &hoff ) ;

	html = "<table width=100%><tr>" ;
	html += "<td rowspan=2>" + wxString::Format ( "%d" , a+1 ) + "</td>" ;
	html += "<td valign=top width=100%>" + name + "</td>" ;
	html += "<td align=right valign=top>" + evalue + "</td>" ;
	html += "</tr><tr>" ;
	html += "<td colspan=2><tt><font size=2>\n" ;
	html += blast_align ( qseq , mseq , hseq , w , qoff , hoff ) ;
	html += "</font></tt></td>" ;
	html += "</tr></table>" ;
	hlb->Set ( a - res_start , html , id ) ;
    }
    hlb->Update () ;
//    wxMessageBox ( wxString::Format ( "%d" , res_count ) ) ;
    b_next->Enable ( res_start + RETMAX < res_count ) ;
    b_last->Enable ( res_start > 0 ) ;

    int max = res_start + RETMAX ;
    if ( max > res_count ) max = res_count ;
    showMessage ( wxString::Format ( txt("t_blast_results_by" ) , blast_version.c_str() , res_start+1 , max ) ) ;
}

wxString EIpanel::blast_align ( wxString qseq , wxString mseq , wxString hseq , int cpl , int qoff , int hoff )
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
	int a ;
	wxString z ;
	for ( a = 0 , z = qseq.Left ( cpl ) ; a < z.length() ; a++ ) if ( z.GetChar(a) >= 'A' && z.GetChar(a) <= 'Z' ) qoff++ ;
	for ( a = 0 , z = hseq.Left ( cpl ) ; a < z.length() ; a++ ) if ( z.GetChar(a) >= 'A' && z.GetChar(a) <= 'Z' ) hoff++ ;

	ret += lead[0] + " " + qseq.Left ( cpl ) + " " + wxString::Format ( "%5d" , qoff ) + "\n" ;
	ret += lead[1] + " " + mseq.Left ( cpl ) + "\n" ;
	ret += lead[2] + " " + hseq.Left ( cpl ) + " " + wxString::Format ( "%5d" , hoff ) + "\n" ;
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
    wxString database = c1->GetStringSelection() ;
    execute_ncbi_load ( database ) ; // Dummy
}
