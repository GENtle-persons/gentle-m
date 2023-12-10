/** \file
    \brief TPDB class, and its tseqres helper class
*/
#include "TPDB.h"
#include <wx/textfile.h>

TPDB::TPDB ()
    {
    success = false ;
    }

void TPDB::load ( const wxString& s )
    {
    vs.Clear () ;
    wxTextFile tf ;
    tf.Open ( s , *(myapp()->isoconv) ) ;
    vs.Add ( tf.GetFirstLine() ) ;
    while ( !tf.Eof() )
        vs.Add ( tf.GetNextLine() ) ;
    check4success () ;
    }

void TPDB::paste ( const wxString& s )
    {
    explode ( _T("\n") , s , vs ) ;
    check4success () ;
    }

int TPDB::seq ( const char c )
    {
    for ( int a = 0 ; a < seqres.size() ; a++ )
        if ( seqres[a].chain == c )
           return a ;
    seqres.push_back ( tseqres() ) ;
    seqres[seqres.size()-1].chain = c ;
    seqres[seqres.size()-1].v = new TVector ;
    return seqres.size()-1 ;
    }

void TPDB::remap ()
    {
    seqres.clear () ;
    for ( int a = 0 ; a < vs.GetCount() ; a++ )
        {
        wxString six = vs[a].Left ( 6 ) ;
        wxString right = vs[a].Mid ( 6 ) ;
        right = right.Trim () ;
        right = right.Trim ( false ) ;
        six = six.Trim () ;
        if ( six == _T("TITLE") )
           {
           _name = right ;
           }
        else if ( six == _T("REMARK") )
           {
           right = right.AfterFirst ( ' ' ) ;
           _desc += right + _T("\n") ;
           }
        else if ( six == _T("SEQRES") )
           {
           char chain = vs[a].GetChar ( 11 ) ;
           wxArrayString v2 ;
           right = vs[a].Mid ( 19 ) ;
           right = right.Trim () ;
           while ( !right.IsEmpty() )
              {
              v2.Add ( right.BeforeFirst ( ' ' ) ) ;
              right = right.AfterFirst ( ' ' ) ;
              }
           for ( int b = 0 ; b < v2.GetCount() ; b++ )
              {
              wxString h = v2[b] ;
              h = h.Trim() ;
              char c = seqres[seq(chain)].v->three2one(h.Trim(false)) ;
              if ( c != ' ' ) seqres[seq(chain)].sequence += c ;
              }
           }
        else if ( six == _T("DBREF") )
           {
           wxString name = vs[a].Mid(7,4).Trim(false).Trim() ;
           wxString ncbi = vs[a].Mid(33,8).Trim(false).Trim() ;
           wxString gb = vs[a].Mid(42,12).Trim(false).Trim() ;
           char chain = vs[a].GetChar ( 12 ) ;
           seqres[seq(chain)].v->addDescription ( _T("PDB : http://www.rcsb.org/pdb/cgi/explore.cgi?pdbId=") + name + _T("\n") ) ;
           seqres[seq(chain)].v->addDescription ( _T("GenBank : http://www.ncbi.nlm.nih.gov/entrez/viewer.fcgi?db=protein&val=") + ncbi + _T("\n") ) ;
           seqres[seq(chain)].v->addDescription ( _T("NiceProt : http://expasy.org/cgi-bin/niceprot.pl?") + gb + _T("\n") ) ;
           seqres[seq(chain)].v->addDescription ( _T("SwissProt : http://expasy.org/cgi-bin/get-sprot-entry?") + ncbi + _T("\n") ) ;
           }
        else if ( six == _T("HELIX") )
           {
           wxString name = _T("alpha-") + vs[a].Mid(11,4).Trim(false).Trim() ;
           wxString desc = vs[a].Mid ( 40 , 30 ) ;
           int from = atoi ( vs[a].Mid(21,4).mb_str() ) ;
           int to = atoi ( vs[a].Mid(33,4).mb_str() ) ;
           char chain = vs[a].GetChar ( 19 ) ;
           desc = _T("\n") + desc.Trim(false).Trim() ;
           desc = txt("t_seq_style_alpha") + desc ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( _T("SEQUENCE_STYLE") , _T("1") ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           }
        else if ( six == _T("SHEET") )
           {
           wxString name = _T("beta-") + vs[a].Mid(11,4).Trim(false).Trim() ;
           wxString desc = txt("t_seq_style_beta") ;
           int from = atoi ( vs[a].Mid(22,4).mb_str() ) ;
           int to = atoi ( vs[a].Mid(33,4).mb_str() ) ;
           char chain = vs[a].GetChar ( 21 ) ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( _T("SEQUENCE_STYLE") , _T("2") ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           }
        else if ( six == _T("TURN") )
           {
           wxString name = _T("turn-") + vs[a].Mid(11,4).Trim(false).Trim() ;
           wxString desc = vs[a].Mid ( 40 , 30 ) ;
           int from = atoi ( vs[a].Mid(20,4).mb_str() ) ;
           int to = atoi ( vs[a].Mid(31,4).mb_str() ) ;
           char chain = vs[a].GetChar ( 19 ) ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( _T("SEQUENCE_STYLE") , _T("0") ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           }
        else if ( six == _T("SSBOND") )
           {
           wxString name = _T("SS") ;
           wxString desc = vs[a].Mid(11,4).Trim(false).Trim() ;
           int from = atoi ( vs[a].Mid(20,4).mb_str() ) ;
           int to = from ;
           char chain = vs[a].GetChar ( 15 ) ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( _T("SEQUENCE_STYLE") , _T("5") ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           chain = vs[a].GetChar ( 29 ) ;
           desc = vs[a].Mid(25,4).Trim(false).Trim() ;
           from = atoi ( vs[a].Mid(31,4).mb_str() ) ;
           to = from ;
           TVectorItem vi2 ( name , desc , from , to , VIT_MISC ) ;
           vi2.setParam ( _T("SEQUENCE_STYLE") , _T("5") ) ;
           seqres[seq(chain)].v->items.push_back ( vi2 ) ;
           }
        else if ( six == _T("SLTBRG") || six == _T("LINK") )
           {
           wxString name = _T("PDB_") + six ;
           name = txt(name);
           wxString desc = vs[a] ;
           int id1 = atoi ( vs[a].Mid(22,4).mb_str() ) ;
           int id2 = atoi ( vs[a].Mid(52,4).mb_str() ) ;
           wxString name2 = vs[a].Mid(47,3) ;
           char chain1 = (char) vs[a].GetChar ( 21 ) ;
           char chain2 = (char) vs[a].GetChar ( 51 ) ;
           name += _T(" ") + name2 + wxString::Format ( _T("%d %c") , id2 , chain2 ) ;
           TVectorItem vi1 ( name , desc , id1 , id1 , VIT_MISC ) ;
           vi1.setParam ( _T("SEQUENCE_STYLE") , _T("5") ) ;
           seqres[seq(chain1)].v->items.push_back ( vi1 ) ;
           }
        }

    for ( int a = 0 ; a < seqres.size() ; a++ )
        {
        seqres[a].v->setSequence ( seqres[a].sequence ) ;
        seqres[a].v->setName ( _name + _T(" (") + wxString::Format ( _T("%d") , seqres[a].chain ) + _T(")") ) ;
//      seqres[a].v->setName ( _name + _T(" (") + wxString ( (char*)seqres[a].chain , *wxConvCurrent ) + _T(")") ) ;
        seqres[a].v->addDescription ( _desc ) ;
        }
    }


void TPDB::check4success ()
    {
    success = false ;
    if ( vs.GetCount() == 0 ) return ;
    if ( vs[0].Left ( 6 ) != _T("HEADER") ) return ;
    success = true ;
    }

