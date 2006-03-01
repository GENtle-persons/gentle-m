/** \file
	\brief Contains the TOnlineTools class
*/
#ifndef _ONLINE_TOOLS_H_
#define _ONLINE_TOOLS_H_

#include "main.h"

/** \class TOnlineTools
	\brief TOnlineTools class
*/
class TOnlineTools
    {
    public :
    TOnlineTools () ;
    void init ( SequenceCanvas *sc = NULL ) ;
    void add_context_menu ( wxMenu *base ) ;
    void take_event ( wxCommandEvent& event ) ;
    void invalidate () ;
    
    private :
    void determine_marked_or_complete_sequence () ;
    wxString get_fasta_name () ;
    
    void do_phobius ( wxString additional = _T("") ) ;
    void do_poly_phobius () ;
    void do_motif_scan () ;
    void do_pval_fpscan () ;
    void do_elm () ;
    void do_jpred () ;
    void do_neb_cutter () ;
    
    SequenceCanvas *canvas ;
    ChildBase *child ;
    wxString sequence ;
    } ;

#endif
