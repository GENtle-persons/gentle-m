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
    void init ( SequenceCanvas * const sc = NULL ) ;
    void add_context_menu ( wxMenu *base ) ;
    void take_event ( wxCommandEvent& event ) ;
    void invalidate () ;

    private :
    void determine_marked_or_complete_sequence () ;
    wxString get_fasta_name ( const wxString& ret = _T("") ) const ;

    void do_phobius ( const wxString& additional = _T("") ) const ;
    void do_poly_phobius () const ;
    void do_motif_scan () const ;
    void do_pval_fpscan () const ;
    void do_elm () const ;
    void do_jpred () const ;
    void do_calcpi () const ;
    void do_gor () const ;
    void do_hnn () const ;

    void do_neb_cutter () const ;

    wxString get_fasta_sequences ( const bool clean_gaps , const bool for_clipboard ) const ;

    SequenceCanvas *canvas ;
    ChildBase *child ;
    wxString sequence ;
    } ;

#endif
