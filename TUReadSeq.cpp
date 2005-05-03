/** \file
	\brief The TUReadSeq class
*/
#include "TUReadSeq.h"

TUReadSeq::TUReadSeq ( wxString _filename )
    {
    error = 0 ;
    filename = _filename ;
    getFormat () ;
    getSequenceNames() ;
    getSequences () ;
    if ( seqs.GetCount() == 0 ) error = 1 ;
    }
    

void TUReadSeq::getFormat ()
    {
    if ( error != 0 ) return ;
    format = seqFileFormat ( filename.mb_str() , &skiplines , &error ) ;
    if ( format < kMinFormat || format > kMaxFormat ) error = 1 ;
    }
    

int TUReadSeq::getSequenceNames ()
    {
    if ( error != 0 ) return 0 ;
    char  *seqlist;   /* list of sequence names, newline separated, 0 terminated */

    seqlist = listSeqs( filename.mb_str(), skiplines, format, &numseqs, &error);
    wxString s ( seqlist , *wxConvCurrent ) ;
    free( seqlist);
    explode ( _T("\n") , s , seq_names ) ;
    return seq_names.GetCount() ;
    }
    
void TUReadSeq::getSequences ()
    {
    if ( error != 0 ) return ;
    short seqIndex ;
    seqs.Clear () ;
    for ( seqIndex = 1 ; error == 0 && seqIndex <= seq_names.GetCount() ; seqIndex++ )
        {    
        long  seqlen;     /* length of seq */
        char  seqid[256]; /* sequence name */
        char  *seq;       /* sequence, 0 terminated, free when done */
        seqid[0] = 0 ;
        seq = readSeq( seqIndex, filename.mb_str(), skiplines, format,
                      &seqlen, &numseqs, &error, seqid);
        for ( char *c = seq ; *c ; c++ )
           {
           if ( *c >= 'a' && *c <= 'z' ) *c = *c - 'a' + 'A' ;
           }
        seq_names[seqIndex-1] = wxString ( seqid , *wxConvCurrent ) ;
        seqs.Add ( wxString ( seq , *wxConvCurrent) ) ;
        free(seq);
        }
    }
    
int TUReadSeq::getSeqType ( wxString t )
    {
    short i = getseqtype ( t.mb_str() , t.length() ) ;
    if ( i == kAmino ) return TYPE_AMINO_ACIDS ;
    return TYPE_VECTOR ;
    }
    
void TUReadSeq::convert ( TGenBank &gb )
    {
    wxString s ;
    for ( int a = 0 ; a < seqs.GetCount() ; a++ )
        {
        s += _T("LOCUS       ") + seq_names[a] + _T("\n") ;
        s += _T("TITLE       ") + seq_names[a] + _T("\n") ;
        s += _T("ORIGIN\n") ;
        s += _T(" ") + seqs[a] + _T("\n") ;
        s += _T("//\n") ;
        }
    gb.paste ( s ) ;
    }
    
