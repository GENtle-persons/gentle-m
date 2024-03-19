/** \file
    \brief Contains the EIpanel methods and the blastThread helper class
*/
#include "ExternalInterface.h"

void EIpanel::init_blast()
    {
    RETMAX = 10 ;
    t1 = new wxTextCtrl ( up , ID_T1 , _T("") , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
    b1 = new wxButton ( up , ID_B1 , txt("b_find") , wxDefaultPosition ) ;
    b2 = new wxButton ( up , ID_B2 , txt("t_open") , wxDefaultPosition ) ;
    b3 = new wxButton ( up , ID_B3 , txt("b_open_link") , wxDefaultPosition ) ;
    c1 = new wxChoice ( up , ID_C1 ) ;
    c1->Append ( _T("Protein") ) ;
    c1->Append ( _T("Nucleotide") ) ;

    c2 = new wxChoice ( up , ID_C2 ) ;
    c2->Append ( _T("50") ) ;
    c2->Append ( _T("100") ) ;
    c2->Append ( _T("250") ) ;
    c2->Append ( _T("500") ) ;
    b_last = new wxButton ( up , ID_B_LAST , txt("b_last") , wxDefaultPosition ) ;
    b_next = new wxButton ( up , ID_B_NEXT , txt("b_next") , wxDefaultPosition ) ;
    b_last->Disable () ;
    b_next->Disable () ;
    b3->Disable () ;

    v1 = new wxBoxSizer ( wxVERTICAL ) ;
    h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    h0->Add ( c1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( t1 , 1 , wxEXPAND , 5 ) ;
    h0->Add ( b1 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b2 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b3 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( c2 , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_last , 0 , wxEXPAND , 5 ) ;
    h0->Add ( b_next , 0 , wxEXPAND , 5 ) ;

    st_msg = new wxStaticText ( up , -1 , _T("") ) ;

    v1->Add ( h0 , 0 , wxEXPAND , 0 ) ;
//  v1->Add ( h1 , 0 , wxEXPAND , 3 ) ;
    v1->Add ( st_msg , 0 , wxEXPAND , 3 ) ;
    up->SetSizer ( v1 ) ;

    c1->SetSelection ( 0 ) ;
    c2->SetSelection ( 0 ) ;
    t1->SetFocus() ;
    }


#if !wxUSE_THREADS
    #error "This requires thread support!"
#endif // wxUSE_THREADS

/** \brief A wxThreadHelper class to run BLAST queries in the background
*/
class blastThread : public wxThreadHelper
{
public :
    /// Constructor
    blastThread ( EIpanel *panel , wxString seq ) : wxThreadHelper()
        {
        p = panel ;
        seq.Replace ( _T("|") , _T("") ) ; // Amino acid search fix

        // Put
        url = _T("https://www.ncbi.nlm.nih.gov/blast/Blast.cgi?") ;
        url += _T("CMD=Put") ;
        url += _T("&QUERY=") + seq ;
        url += _T("&DATABASE=nr") ;
        if ( p->c1->GetSelection() == 0 ) url += _T("&PROGRAM=blastp") ;
        if ( p->c1->GetSelection() == 1 ) url += _T("&PROGRAM=blastn") ;
        url += _T("&HITLIST_SIZE=") + p->c2->GetStringSelection() ;

        res = p->ExecuteHttpsQuery ( url ) ;

        hs = parseQblast ( res ) ;
        RID = hs[_T("RID")] ;
        RTOE = hs[_T("RTOE")] ;

        // Prepare URL
        RTOE.ToLong ( &wait ) ;
        url = _T("https://www.ncbi.nlm.nih.gov/blast/Blast.cgi?") ;
        url += _T("CMD=Get") ;
        url += _T("&RID=") + RID ;
        url += _T("&FORMAT_TYPE=XML") ;
        } ;

    /// This display the time left, and (re-)checks online if the query is done
    virtual void *Entry ()
        {
        // Get & wait
        do {
            while ( wait )
                {
#ifndef __WXMAC__
                wxMutexGuiEnter() ;
                p->showMessage ( wxString::Format ( txt("t_blast_time") , wait ) ) ;
                wxMutexGuiLeave() ;
#endif
                wxThread::Sleep ( 1000 ) ; // Wait 1 sec
                wait-- ;
                if ( GetThread()->TestDestroy() ) // Currently, there's no way to interrupt this!
                    {
                    // MISSING : Set search button to "on" again
                    return NULL ;
                }
            }
//          cout << res << endl << "----\n\n" ;
            wxMutexGuiEnter() ;
            res = p->ExecuteHttpsQuery ( url ) ;
            wxMutexGuiLeave() ;

            hs = parseQblast ( res ) ;
            if ( hs[_T("STATUS")].Upper() == _T("WAITING") ) wait = 5 ; // Wait another 5 seconds
            else wait = 0 ; // Done!
        } while ( wait ) ;

        // Set result and display
        p->blast_res = res ;
        wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, ID_B1 );
        wxPostEvent ( p , event ) ;
        return NULL ;
        }
private :

    /// I'll be damned if I remember what this is for!
    wxHashString parseQblast ( wxString res )
        {
        wxHashString ret ;
        wxString q1 = _T("QBlastInfoBegin") ;
        wxString q2 = _T("QBlastInfoEnd") ;

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

/// This starts the thread
void EIpanel::process_blast()
    {
    mylog ( "blast1" , "1" ) ;
    if ( !blast_res.IsEmpty() || blast_thread ) // If thread is running, or results are there...
    {
    mylog ( "blast1" , "1a" ) ;
    process_blast2() ;
    return ;
    }
    mylog ( "blast1" , "2" ) ;

    // Start thread
    blast_res.Empty() ;
    wxString seq = t1->GetValue() ;
    blast_thread = new blastThread ( this , seq ) ;
    if ( !blast_thread || wxTHREAD_NO_ERROR != blast_thread->CreateThread() )
    {
    blast_thread = NULL ;
    return ;
    }
    mylog ( "blast1" , "3" ) ;

//    blast_thread->SetPriority ( 10 ) ; // Quite low

    if ( wxTHREAD_NO_ERROR != blast_thread->GetThread()->Run() )
    {
    blast_thread = NULL ;
    return ;
    }
    mylog ( "blast1" , "4" ) ;

    b1->Disable() ;
    mylog ( "blast1" , "5" ) ;
    }

/// This is called upon termination of the thread
void EIpanel::process_blast2()
    {
    bool initial = false ;
    mylog ( "blast2" , "1" ) ;
    if ( blast_thread )
        {
#ifdef __WXMSW__
//    blast_thread->Wait() ;
//    blast_thread->Delete() ;
#endif
        blast_thread = NULL ;
        b1->Enable() ;
        initial = true ;
        res_count = 0 ;
        res_start = 0 ;
        }
    mylog ( "blast2" , "2" ) ;

    TiXmlDocument blast_doc ;
    blast_doc.SetCondenseWhiteSpace(false);
    blast_doc.Parse ( blast_res.mb_str() ) ;

    mylog ( "blast2" , "3" ) ;
    hlb->Clear () ;
    showMessage ( txt("t_blast_failed") ) ;
    TiXmlNode *x = blast_doc.FirstChild ( "BlastOutput" ) ;
    if ( !x ) return ;

    mylog ( "blast2" , "4" ) ;
    int w , h ;
    hlb->GetClientSize ( &w , &h ) ;
    w /= 8 ;

    wxString blast_version = valFC ( x->FirstChild ( "BlastOutput_version" ) ) ;

    x = x->FirstChild ( "BlastOutput_iterations" ) ;
    x = x->FirstChild ( "Iteration" ) ;
    x = x->FirstChild ( "Iteration_hits" ) ;

    mylog ( "blast2" , "5" ) ;
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

        name = name.BeforeFirst ( '>' ) ;

        TiXmlNode *h = x->FirstChild ( "Hit_hsps" ) ;
        h = h->FirstChild ( "Hsp" ) ;
        wxString evalue = valFC ( h->FirstChild ( "Hsp_evalue" ) ) ;
        if ( evalue.Find ( 'e' ) > -1 )
            {
            wxString base = evalue.BeforeFirst ( 'e' ) + _T("&times;10") ;
            wxString exp = _T("<font size=2>") + evalue.AfterFirst ( 'e' )  + _T("</font>") ;

            evalue = _T("<table border=0 cellpadding=0 cellspacing=0><tr><td rowspan=2 valign=bottom>E-Value=</td>") ;
            evalue += _T("<td align=left valign=bottom><br>") + base + _T("</td>") ;
            evalue += _T("<td align=right valign=top>") + exp + _T("</td>") ;
            evalue += _T("</tr></table>") ;
            }
        else evalue = _T("E-Value=") + evalue ;

        wxString qseq = valFC ( h->FirstChild ( "Hsp_qseq" ) ) ;
        wxString mseq = valFC ( h->FirstChild ( "Hsp_midline" ) ) ;
        wxString hseq = valFC ( h->FirstChild ( "Hsp_hseq" ) ) ;
        long qoff , hoff ;
        valFC ( h->FirstChild ( "Hsp_query-from" ) ) . ToLong ( &qoff ) ;
        valFC ( h->FirstChild ( "Hsp_hit-from" ) ) . ToLong ( &hoff ) ;

        html = _T("<table width=100%><tr>") ;
        html += _T("<td rowspan=2>") + wxString::Format ( _T("%d") , a+1 ) + _T("</td>") ;
        html += _T("<td valign=top width=100%>") + name + _T("</td>") ;
        html += _T("<td align=right valign=top>") + evalue + _T("</td>") ;
        html += _T("</tr><tr>") ;
        html += _T("<td colspan=2><tt><font size=2>\n") ;
        html += blast_align ( qseq , mseq , hseq , w , qoff , hoff ) ;
        html += _T("</font></tt></td>") ;
        html += _T("</tr></table>") ;
        hlb->Set ( a - res_start , html , id ) ;
        }

    mylog ( "blast2" , "6" ) ;
    hlb->Update () ;
    mylog ( "blast2" , "7" ) ;
//  wxMessageBox ( wxString::Format ( "%d" , res_count ) ) ;
    b_next->Enable ( res_start + RETMAX < res_count ) ;
    b_last->Enable ( res_start > 0 ) ;
    b3->Enable ( true ) ;

    int max = res_start + RETMAX ;
    if ( max > res_count ) max = res_count ;
    showMessage ( wxString::Format ( txt("t_blast_results_by" ) , blast_version.c_str() , res_start+1 , max , res_count ) ) ;
    wxWakeUpIdle();
    mylog ( "blast2" , "8" ) ;
    }

wxString EIpanel::blast_align ( const wxString& _qseq , const wxString& _mseq , const wxString& _hseq , const int _cpl , const int _qoff , const int _hoff )
    {
    wxString lead[3] ;
    wxString mseq = _mseq ;
    wxString hseq = _hseq ;
    wxString qseq = _qseq ;

    int hoff ( _hoff ) ;
    int qoff ( _qoff ) ;
    int cpl ( _cpl ) ;

    lead[0] = txt("t_blast_qseq" ) ;
    lead[2] = txt("t_blast_hseq" ) ;
    while ( lead[0].Length() < lead[2].Length() ) lead[0] += _T(" ") ;
    while ( lead[0].Length() > lead[2].Length() ) lead[2] += _T(" ") ;
    lead[1].Append ( ' ' , lead[0].Length() ) ;

    cpl -= lead[0].Length() + 1 ;
    qoff-- ;
    hoff-- ;

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
//      ret += "<p>" ;
        wxString z ;
        int a ;
        for ( a = 0 , z = qseq.Left ( cpl ) ; a < z.length() ; a++ ) if ( z.GetChar(a) >= 'A' && z.GetChar(a) <= 'Z' ) qoff++ ;
        for ( a = 0 , z = hseq.Left ( cpl ) ; a < z.length() ; a++ ) if ( z.GetChar(a) >= 'A' && z.GetChar(a) <= 'Z' ) hoff++ ;

        ret += lead[0] + _T(" ") + qseq.Left ( cpl ) + _T(" ") + wxString::Format ( _T("%5d") , qoff ) + _T("\n") ;
        ret += lead[1] + _T(" ") + mseq.Left ( cpl ) + _T("\n") ;
        ret += lead[2] + _T(" ") + hseq.Left ( cpl ) + _T(" ") + wxString::Format ( _T("%5d") , hoff ) + _T("\n") ;
        qseq = qseq.Mid ( cpl ) ;
        mseq = mseq.Mid ( cpl ) ;
        hseq = hseq.Mid ( cpl ) ;
        if ( !qseq.IsEmpty() ) ret += _T("<p></p>\n") ;
        }
    ret.Replace ( _T(" ") , _T("&nbsp;") ) ;
    return ret ;
    }

void EIpanel::execute_blast()
    {
    wxString database = c1->GetStringSelection() ;
    execute_ncbi_load ( database ) ; // Dummy
    }

void EIpanel::execute_blast_b3()
    {
    wxString database = c1->GetStringSelection().Lower() ;
    for ( int i = 0 ; i < hlb->data.GetCount() ; i++ )
        {
        if ( !hlb->IsSelected ( i ) ) continue ;
        wxString s = hlb->data[i] ;

        int a = s.Find ( _T("gi|") ) ;
        if ( a == -1 ) return ;
        s = s.Mid ( a + 3 ) ;
        s = s.BeforeFirst ( '|' ) ;
        s = _T("https://www.ncbi.nlm.nih.gov/entrez/viewer.fcgi?cmd=Retrieve&db=") + database + _T("&list_uids=") + s + _T("&dopt=GenPept") ;
        s = myapp()->getHTMLCommand ( s ) ;
        wxExecute ( s ) ;
        }
    }
