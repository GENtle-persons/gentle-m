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
    format = seqFileFormat ( filename.c_str() , &skiplines , &error ) ;
    if ( format < kMinFormat || format > kMaxFormat ) error = 1 ;
    }
    

int TUReadSeq::getSequenceNames ()
    {
    if ( error != 0 ) return 0 ;
    char  *seqlist;   /* list of sequence names, newline separated, 0 terminated */

    seqlist = listSeqs( filename.c_str(), skiplines, format, &numseqs, &error);
    string s = seqlist ;
    free( seqlist);
    seq_names = explode ( "\n" , s ) ;
    return seq_names.size() ;
    }
    
void TUReadSeq::getSequences ()
    {
    if ( error != 0 ) return ;
    short seqIndex ;
    seqs.clear () ;
    for ( seqIndex = 1 ; error == 0 && seqIndex <= seq_names.size() ; seqIndex++ )
        {    
        long  seqlen;     /* length of seq */
        char  seqid[256]; /* sequence name */
        char  *seq;       /* sequence, 0 terminated, free when done */
        seqid[0] = 0 ;
        seq = readSeq( seqIndex, filename.c_str(), skiplines, format,
                      &seqlen, &numseqs, &error, seqid);
        for ( char *c = seq ; *c ; c++ )
           {
           if ( *c >= 'a' && *c <= 'z' ) *c = *c - 'a' + 'A' ;
           }
        seq_names[seqIndex-1] = seqid ;
        seqs.push_back ( string ( seq ) ) ;
        free(seq);
        }
    }
    
int TUReadSeq::getSeqType ( string t )
    {
    short i = getseqtype ( t.c_str() , t.length() ) ;
    if ( i == kAmino ) return TYPE_AMINO_ACIDS ;
    return TYPE_VECTOR ;
    }
    
void TUReadSeq::convert ( TGenBank &gb )
    {
    string s ;
    for ( int a = 0 ; a < seqs.size() ; a++ )
        {
        s += "LOCUS       " + seq_names[a] + "\n" ;
        s += "TITLE       " + seq_names[a] + "\n" ;
        s += "ORIGIN\n" ;
        s += " " + seqs[a] + "\n" ;
        s += "//\n" ;
        }
    gb.paste ( s.c_str() ) ;
    }
    
