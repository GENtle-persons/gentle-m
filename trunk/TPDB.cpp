#include "TPDB.h"
#include <wx/textfile.h>

TPDB::TPDB ()
    {
    success = false ;
    }

void TPDB::load ( wxString s )
    {
    vs.Clear () ;
    wxTextFile tf ;
    tf.Open ( s ) ;
    while ( !tf.Eof() )
        vs.Add ( tf.GetNextLine() ) ;
    check4success () ;
    }

void TPDB::paste ( wxString s )
    {
    explode ( "\n" , s , vs ) ;
    check4success () ;
    }
    
int TPDB::seq ( char c )
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
    int a , b ;
    seqres.clear () ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        wxString six = vs[a].Left ( 6 ) ;
        wxString right = vs[a].Mid ( 6 ) ;
        right = right.Trim () ;
        right = right.Trim ( false ) ;
        six = six.Trim () ;
        if ( six == "TITLE" )
           {
           _name = right ;
           }    
        else if ( six == "REMARK" )
           {
           right = right.AfterFirst ( ' ' ) ;
           _desc += right + "\n" ;
           }    
        else if ( six == "SEQRES" )
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
           for ( b = 0 ; b < v2.GetCount() ; b++ )
              {
              wxString h = v2[b] ;
              h = h.Trim() ;
              char c = seqres[seq(chain)].v->three2one(h.Trim(false)) ;
              if ( c != ' ' ) seqres[seq(chain)].sequence += c ;
              }
           }
        else if ( six == "DBREF" )
           {
           wxString name = vs[a].Mid(7,4).Trim(false).Trim() ;
           wxString ncbi = vs[a].Mid(33,8).Trim(false).Trim() ;
           wxString gb = vs[a].Mid(42,12).Trim(false).Trim() ;
           char chain = vs[a].GetChar ( 12 ) ;
           seqres[seq(chain)].v->addDescription ( "PDB : http://www.rcsb.org/pdb/cgi/explore.cgi?pdbId=" + name + "\n" ) ;
           seqres[seq(chain)].v->addDescription ( "GenBank : http://www.ncbi.nlm.nih.gov/entrez/viewer.fcgi?db=protein&val=" + ncbi + "\n" ) ;
           seqres[seq(chain)].v->addDescription ( "NiceProt : http://expasy.org/cgi-bin/niceprot.pl?" + gb + "\n" ) ;
           seqres[seq(chain)].v->addDescription ( "SwissProt : http://expasy.org/cgi-bin/get-sprot-entry?" + ncbi + "\n" ) ;
           }    
        else if ( six == "HELIX" )
           {
           wxString name = "alpha-" + vs[a].Mid(11,4).Trim(false).Trim() ;
           wxString desc = vs[a].Mid ( 40 , 30 ) ;
           int from = atoi ( vs[a].Mid(21,4).c_str() ) ;
           int to = atoi ( vs[a].Mid(33,4).c_str() ) ;
           char chain = vs[a].GetChar ( 19 ) ;
           desc = "\n" + desc.Trim(false).Trim() ;
           desc = txt("t_seq_style_alpha") + desc ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( "SEQUENCE_STYLE" , "1" ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           }    
        else if ( six == "SHEET" )
           {
           wxString name = "beta-" + vs[a].Mid(11,4).Trim(false).Trim() ;
           wxString desc = txt("t_seq_style_beta") ;
           int from = atoi ( vs[a].Mid(22,4).c_str() ) ;
           int to = atoi ( vs[a].Mid(33,4).c_str() ) ;
           char chain = vs[a].GetChar ( 21 ) ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( "SEQUENCE_STYLE" , "2" ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           }    
        else if ( six == "TURN" )
           {
           wxString name = "turn-" + vs[a].Mid(11,4).Trim(false).Trim() ;
           wxString desc = vs[a].Mid ( 40 , 30 ) ;
           int from = atoi ( vs[a].Mid(20,4).c_str() ) ;
           int to = atoi ( vs[a].Mid(31,4).c_str() ) ;
           char chain = vs[a].GetChar ( 19 ) ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( "SEQUENCE_STYLE" , "0" ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           }    
        else if ( six == "SSBOND" )
           {
           wxString name = "SS" ;
           wxString desc = vs[a].Mid(11,4).Trim(false).Trim() ;
           int from = atoi ( vs[a].Mid(20,4).c_str() ) ;
           int to = from ;
           char chain = vs[a].GetChar ( 15 ) ;
           TVectorItem vi ( name , desc , from , to , VIT_MISC ) ;
           vi.setParam ( "SEQUENCE_STYLE" , "5" ) ;
           seqres[seq(chain)].v->items.push_back ( vi ) ;
           chain = vs[a].GetChar ( 29 ) ;
           desc = vs[a].Mid(25,4).Trim(false).Trim() ;
           from = atoi ( vs[a].Mid(31,4).c_str() ) ;
           to = from ;
           TVectorItem vi2 ( name , desc , from , to , VIT_MISC ) ;
           vi2.setParam ( "SEQUENCE_STYLE" , "5" ) ;
           seqres[seq(chain)].v->items.push_back ( vi2 ) ;
           }    
        }
        
    for ( a = 0 ; a < seqres.size() ; a++ )
        {
        seqres[a].v->setSequence ( seqres[a].sequence ) ;
        seqres[a].v->setName ( _name + " (" + wxString ( seqres[a].chain ) + ")" ) ;
        seqres[a].v->addDescription ( _desc ) ;
        }    
    }

    
void TPDB::check4success ()
    {
    success = false ;
    if ( vs.GetCount() == 0 ) return ;
    if ( vs[0].Left ( 6 ) != "HEADER" ) return ;
    success = true ;
    }
        
