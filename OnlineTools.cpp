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
    if ( sequence.length() > 1 ) return ;

    // Nothing selected, try to get the whole sequence
    if ( !child ) return ;
    if ( !child->vec ) return ;
    sequence = child->vec->getSequence() ;
    }

void TOnlineTools::add_context_menu ( wxMenu *base )
    {
    if ( !canvas || !child ) return ;

    sequence = _T("") ;
    wxMenu *ret = NULL ;
    if ( child->def == _T("AminoAcids") )
       {
       ret = new wxMenu ;


       wxMenu *primary = new wxMenu ;
       primary->Append ( ONLINE_TOOLS_2ZIP , txt("m_ot_2zip") ) ;
       primary->Append ( ONLINE_TOOLS_SAPS , txt("m_ot_saps") ) ;

       wxMenu *topology = new wxMenu ;
       topology->Append ( ONLINE_TOOLS_PHOBIUS , txt("m_ot_phobius") ) ;
       topology->Append ( ONLINE_TOOLS_POLY_PHOBIUS , txt("m_ot_poly_phobius") ) ;
       topology->Append ( ONLINE_TOOLS_TARGETP , txt("m_ot_targetp") ) ;

       wxMenu *motif = new wxMenu ;
       motif->Append ( ONLINE_TOOLS_MOTIF_SCAN , txt("m_ot_motif_scan") ) ;
       motif->Append ( ONLINE_TOOLS_PVAL_FPSCAN , txt("m_ot_pval_fpscan") ) ;
       motif->Append ( ONLINE_TOOLS_ELM , txt("m_ot_elm") ) ;

       wxMenu *ssp = new wxMenu ;
       ssp->Append ( ONLINE_TOOLS_GOR , txt("m_ot_gor") ) ;
       ssp->Append ( ONLINE_TOOLS_HNN , txt("m_ot_hnn") ) ;
       ssp->Append ( ONLINE_TOOLS_JPRED , txt("m_ot_jpred") ) ;


       wxMenu *ptm = new wxMenu ;
       ptm->Append ( ONLINE_TOOLS_CALCPI , txt("m_ot_calcpi") ) ;
       ptm->Append ( ONLINE_TOOLS_MITOPROT , txt("m_ot_mitoprot") ) ;
       if ( child->vec->getSequenceChar(0) == 'G' )
          ptm->Append ( ONLINE_TOOLS_MYRISTOYLATOR , txt("m_ot_myristoylator") ) ;
       ptm->Append ( ONLINE_TOOLS_SULFINATOR , txt("m_ot_sulfinator" ) ) ;
       ptm->Append ( ONLINE_TOOLS_SUMOPLOT , txt("m_ot_sumoplot" ) ) ;
       ptm->Append ( ONLINE_TOOLS_PREPS , txt("m_ot_preps" ) ) ;
       ptm->Append ( ONLINE_TOOLS_DGPI , txt("m_ot_dgpi" ) ) ;

       ret->Append ( POPUP_DUMMY , txt("m_ot_primary") , primary ) ;
       ret->Append ( POPUP_DUMMY , txt("m_ot_ssp") , ssp ) ;
       ret->Append ( POPUP_DUMMY , txt("m_ot_topology") , topology ) ;
       ret->Append ( POPUP_DUMMY , txt("m_ot_motif") , motif ) ;
       ret->Append ( POPUP_DUMMY , txt("m_ot_ptm") , ptm ) ;
       }
    else if ( child->def == _T("dna") )
       {
       ret = new wxMenu ;
       ret->Append ( ONLINE_TOOLS_NEB_CUTTER , txt("m_ot_neb_cutter") ) ; // Partially functional
       ret->Append ( ONLINE_TOOLS_TRANSLATE , txt("m_ot_translate") ) ;
       if ( child->vec->isCircular() )
          ret->Append ( ONLINE_TOOLS_PLASMAPPER , txt("m_ot_plasmapper") ) ;
       }
    else if ( child->def == _T("alignment") )
       {
       ret = new wxMenu ;
       ret->Append ( ONLINE_TOOLS_NOMAD , txt("m_ot_nomad") ) ;
       ret->Append ( ONLINE_TOOLS_MULTALIN , txt("m_ot_multalin") ) ;
       ret->Append ( ONLINE_TOOLS_WEBLOGO , txt("m_ot_weblogo") ) ;
       }
    if ( ret ) base->Append ( POPUP_DUMMY , txt("m_online_tools") , ret ) ;
    }

void TOnlineTools::take_event ( wxCommandEvent& event )
    {
    if ( !child ) return ;
    if ( child->def != _T("alignment") && !child->vec ) return ;
    determine_marked_or_complete_sequence () ;
    if ( child->def != _T("alignment") && sequence.IsEmpty() ) return ;
    int id = event.GetId() ;

    wxString url , clipboard ;
    switch ( id )
        {
        case ONLINE_TOOLS_PHOBIUS : do_phobius (); break ;
        case ONLINE_TOOLS_POLY_PHOBIUS : do_poly_phobius () ; break ;
        case ONLINE_TOOLS_MOTIF_SCAN : do_motif_scan () ; break ;
        case ONLINE_TOOLS_PVAL_FPSCAN : do_pval_fpscan () ; break ;
        case ONLINE_TOOLS_ELM : do_elm () ; break ;
        case ONLINE_TOOLS_JPRED : do_jpred () ; break ;
        case ONLINE_TOOLS_CALCPI : do_calcpi () ; break ;
        case ONLINE_TOOLS_GOR : do_gor () ; break ;
        case ONLINE_TOOLS_HNN : do_hnn () ; break ;
        case ONLINE_TOOLS_NEB_CUTTER : do_neb_cutter () ; break ;
        case ONLINE_TOOLS_TRANSLATE :
             url = _T("http://www.expasy.org/cgi-bin/dna_aa?pre_text=") + sequence ;
             break ;
        case ONLINE_TOOLS_NOMAD :
             url = _T("http://www.expasy.org/tools/nomad.html") ;
             clipboard = get_fasta_sequences ( true , true ) ;
             break ;
        case ONLINE_TOOLS_MULTALIN :
             url = _T("http://prodes.toulouse.inra.fr/multalin/multalin.html") ;
             clipboard = get_fasta_sequences ( true , true ) ;
             break ;
        case ONLINE_TOOLS_WEBLOGO :
             url = _T("http://weblogo.berkeley.edu/logo.cgi?sequence=") ;
             url += get_fasta_sequences ( false , false ) ;
             break ;
        case ONLINE_TOOLS_MITOPROT :
             url = _T("http://ihg.gsf.de/cgi-bin/paolo/mitofilter?seq=") +
                   sequence + _T("&seqname=") + get_fasta_name() ;
             break ;
        case ONLINE_TOOLS_MYRISTOYLATOR :
             url = _T("http://www.expasy.org/cgi-bin/myristoylator/myristoylator.pl?protein=") + sequence ;
             break ;
        case ONLINE_TOOLS_SULFINATOR :
             url = _T("http://www.expasy.org/tools/sulfinator/") ;
             clipboard = sequence ;
             break ;
        case ONLINE_TOOLS_SUMOPLOT :
             url = _T("http://www.abgent.com/doc/sumoplot") ;
             clipboard = sequence ;
             break ;
        case ONLINE_TOOLS_TARGETP :
             url = _T("http://www.cbs.dtu.dk/services/TargetP/") ;
             clipboard = sequence ;
             break ;
        case ONLINE_TOOLS_2ZIP :
             url = _T("http://2zip.molgen.mpg.de/cgi-bin/2zip.pl?text=") + sequence ;
             break ;
        case ONLINE_TOOLS_SAPS :
             url = _T("http://www.isrec.isb-sib.ch/software/SAPS_form.html") ;
             clipboard = sequence ;
             break ;
        case ONLINE_TOOLS_PREPS :
             url = _T("http://mendel.imp.ac.at/sat/PrePS/index.html") ;
             clipboard = sequence ;
             break ;
        case ONLINE_TOOLS_DGPI :
             url = _T("http://129.194.185.165/dgpi/DGPI_demo_en.html") ;
             clipboard = sequence ;
             break ;
        case ONLINE_TOOLS_PLASMAPPER :
             url = _T("http://wishart.biology.ualberta.ca/PlasMapper/index.html") ;
             clipboard = _T(">") + get_fasta_name() + _T("\n") + sequence ;
             break ;
        } ;
    if ( url.IsEmpty() ) return ;

    if ( !clipboard.IsEmpty() )
       {
       if (!wxTheClipboard->Open()) return ;
       wxTheClipboard->SetData( new wxTextDataObject(clipboard) );
       wxTheClipboard->Close();
       wxMessageBox ( txt("t_ot_clipboard") ) ;
       }
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

wxString TOnlineTools::get_fasta_name ( wxString ret ) const
    {
    if ( ret.IsEmpty() )
       ret = child->vec->getName() ;
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

void TOnlineTools::do_calcpi ()
    {
    wxString url ;
    url = _T("http://scansite.mit.edu/cgi-bin/calcpi?sequence=") + sequence ;
    url += _T("&nphos=10") ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

void TOnlineTools::do_gor ()
    {
    wxString url ;
    url = _T("http://npsa-pbil.ibcp.fr/cgi-bin/secpred_gor4.pl?notice=") + sequence ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }

void TOnlineTools::do_hnn ()
    {
    wxString url ;
    url = _T("http://npsa-pbil.ibcp.fr/cgi-bin/secpred_hnn.pl?notice=") + sequence ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }



//__________________________________________ DNA

void TOnlineTools::do_neb_cutter ()
    {
    wxString url ;
    url = _T("http://tools.neb.com/NEBcutter2/enzcut.php?") ;
    url += _T("username=") + get_fasta_name() ;
    url += _T("&min_orf=100") ;
    url += _T("&circular=") ;
    url += child->vec->isLinear() ? _T("0") : _T("1") ;
    url += _T("&enz_suppl=1") ;
    url += _T("&sequence=") + sequence ;
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }


//__________________________________________ Alignments

wxString TOnlineTools::get_fasta_sequences ( const bool clean_gaps ,  const bool for_clipboard ) const
    {
    wxString nl = for_clipboard ? _T("\n") : _T("%0A") ;
    wxString fa = for_clipboard ? _T(">") : _T("%3E") ;
    wxString fasta ;
    TAlignment *ali = (TAlignment*) child ;
    for ( int a = 0 ; a < ali->lines.size() ; a++ )
        {
        if ( ali->lines[a].isIdentity ) continue ;
        wxString t = ali->lines[a].s ;
        if ( clean_gaps )
           t.Replace ( _T("-") , _T("") ) ;
        if ( !fasta.IsEmpty() ) fasta += nl ;
        fasta += fa + get_fasta_name ( ali->lines[a].name ) + nl ;
        fasta += t ;
        }
    return fasta ;
    }
