#include "OnlineTools.h"

TOnlineTools::TOnlineTools ()
    {
    invalidate () ;
    }

void TOnlineTools::invalidate ()
    {
    init ( NULL ) ;
    }

void TOnlineTools::init ( SequenceCanvas *sc )
    {
    canvas = sc ;
    if ( sc ) child = sc->child ;
    else child = NULL ;
    }

void TOnlineTools::determine_marked_or_complete_sequence ()
    {
    sequence = _T("") ;
    if ( !canvas || !child ) return ;
    sequence = canvas->getSelection() ;
    if ( !sequence.IsEmpty() ) return ;
    
    // Nothing selected, try to get the whole sequence
    if ( !child ) return ;
    if ( !child->vec ) return ;
    sequence = child->vec->getSequence() ;
    }

void TOnlineTools::add_context_menu ( wxMenu *base )
    {
    if ( !canvas || !child ) return ;
    
    sequence = _T("") ;
    wxMenu *ret = new wxMenu ;
    base->Append ( POPUP_DUMMY , txt("m_online_tools") , ret ) ;
    if ( child->def == _T("AminoAcids") )
       {
       ret->Append ( ONLINE_TOOLS_PHOBIUS , txt("m_ot_phobius") ) ;
       ret->Append ( ONLINE_TOOLS_POLY_PHOBIUS , txt("m_ot_poly_phobius") ) ;
       ret->Append ( ONLINE_TOOLS_MOTIF_SCAN , txt("m_ot_motif_scan") ) ;
       ret->Append ( ONLINE_TOOLS_PVAL_FPSCAN , txt("m_ot_pval_fpscan") ) ;
       ret->Append ( ONLINE_TOOLS_ELM , txt("m_ot_elm") ) ;
       ret->Append ( ONLINE_TOOLS_JPRED , txt("m_ot_jpred") ) ;
       }
    else if ( child->def == _T("dna") )
       {
       ret->Append ( ONLINE_TOOLS_NEB_CUTTER , txt("m_neb_cutter") ) ;
       }
    }

void TOnlineTools::take_event ( wxCommandEvent& event )
    {
    if ( !canvas || !child ) return ;
    if ( !child->vec ) return ;
    determine_marked_or_complete_sequence () ;
    if ( sequence.IsEmpty() ) return ;
    
    int id = event.GetId() ;
    switch ( id )
        {
        case ONLINE_TOOLS_PHOBIUS : do_phobius (); break ;
        case ONLINE_TOOLS_POLY_PHOBIUS : do_poly_phobius () ; break ;
        case ONLINE_TOOLS_MOTIF_SCAN : do_motif_scan () ; break ;
        case ONLINE_TOOLS_PVAL_FPSCAN : do_pval_fpscan () ; break ;
        case ONLINE_TOOLS_ELM : do_elm () ; break ;
        case ONLINE_TOOLS_JPRED : do_jpred () ; break ;
        case ONLINE_TOOLS_NEB_CUTTER : do_neb_cutter () ; break ;
        } ;
    }

wxString TOnlineTools::get_fasta_name ()
    {
    wxString ret = child->vec->getName() ;
    ret.Replace ( _T(" ") , _T("_") ) ;
    return ret ;
    }

//__________________________________________ Amino acids

void TOnlineTools::do_phobius ( wxString additional )
    {
    wxString url ;
    url = _T("http://phobius.cgb.ki.se/cgi-bin/predict.pl?protseq=") ;
    url += _T("%3E") + get_fasta_name() + _T("%0A") ;
    url += sequence ;
    url += _T("&informat=blast&format=plp") + additional ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

void TOnlineTools::do_poly_phobius ()
    {
    do_phobius ( _T("&poly=Y") ) ;
    }

void TOnlineTools::do_motif_scan ()
    {
    wxString url ;
    url = _T("http://myhits.isb-sib.ch/cgi-bin/motif_scan?text=") + sequence ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

void TOnlineTools::do_jpred ()
    {
    wxString url ;
    url = _T("http://www.compbio.dundee.ac.uk/~www-jpred/cgi-bin/jpred_form?seq=") ;
    url += sequence + _T("&input=seq&pdb=1") ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

void TOnlineTools::do_elm ()
    {
    wxString url ;
    url = _T("http://elm.eu.org/basicELM/cgimodel.py?sequence=") ;
    url += _T("%3E") + get_fasta_name() + _T("%0A") + sequence ;
    url += _T("&userSpecies=9606&userCC=1&fun=Submit") ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

void TOnlineTools::do_pval_fpscan ()
    {
    wxString url ;
    url = _T("http://umber.sbs.man.ac.uk/cgi-bin/dbbrowser/fingerPRINTScan/muppet/FPScan_fam.cgi?textbox=") + sequence ;
    url += _T("&threshold=0.0001") ;
    url += _T("&database=38_0") ;
    url += _T("&matrix=blos62") ;
    url += _T("&Dthreshold=10") ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

//__________________________________________ DNA

void TOnlineTools::do_neb_cutter ()
    {
    wxString url ;
    url = _T("http://tools.neb.com/NEBcutter2/enzcut.php?") ;
    url += _T("username=") + get_fasta_name() ;
    url += _T("&min_orf=100") ;
    url += _T("&circular=") + _T( child->vec->isLinear() ? "0" : "1" ) ;
    url += _T("&enz_suppl=1") ;
    url += _T("&sequence=") + sequence ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;

    }
