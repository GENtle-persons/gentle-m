#include "TUReadSeq.h"

TUReadSeq::TUReadSeq ( wxString _filename )
    {
    error = 0 ;
    filename = _filename ;
    getFormat () ;
    getSequenceNames() ;
    getSequences () ;
    }
    

void TUReadSeq::getFormat ()
    {
    if ( error != 0 ) return ;
    format = seqFileFormat ( filename , &skiplines , &error ) ;
    if ( format < kMinFormat || format > kMaxFormat ) error = 1 ;
    }
    

int TUReadSeq::getSequenceNames ()
    {
    if ( error != 0 ) return 0 ;
    char  *seqlist;   /* list of sequence names, newline separated, 0 terminated */

    seqlist = listSeqs( filename, skiplines, format, &numseqs, &error);
    wxString s = seqlist ;
    free( seqlist);
    seq_names = explode ( "\n" , s ) ;
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
        seq = readSeq( seqIndex, filename, skiplines, format,
                      &seqlen, &numseqs, &error, seqid);
        for ( char *c = seq ; *c ; c++ )
           {
           if ( *c >= 'a' && *c <= 'z' ) *c = *c - 'a' + 'A' ;
           }
        seq_names[seqIndex-1] = seqid ;
        seqs.Add ( seq ) ;
        free(seq);
        }
    }
    
int TUReadSeq::getSeqType ( wxString t )
    {
    short i = getseqtype ( t , t.length() ) ;
    if ( i == kAmino ) return TYPE_AMINO_ACIDS ;
    return TYPE_VECTOR ;
    }
    
void TUReadSeq::convert ( TGenBank &gb )
    {
    wxString s ;
    for ( int a = 0 ; a < seqs.GetCount() ; a++ )
        {
        s += "LOCUS       " + seq_names[a] + "\n" ;
        s += "TITLE       " + seq_names[a] + "\n" ;
        s += "ORIGIN\n" ;
        s += " " + seqs[a] + "\n" ;
        s += "//\n" ;
        }
    gb.paste ( s ) ;
    }
    
