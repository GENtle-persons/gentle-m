/* File: ureadseq.h
 *
 * Header for module UReadSeq
 */

#ifndef UREADSEQ_H
#define UREADSEQ_H


#ifndef __WXMSW__
typedef char  boolean;
#endif

#define NEWLINE         '\n'
#ifndef false
    #define false 0
    #define true  1
#endif
#define min(a,b)      (a<b)?a:b
#define max(a,b)      (a>b)?a:b
#define skipwhitespace(string)  {while (*string <= ' ' && *string != 0) string++;}

  /* NLM strings */
#define is_upper(c) ('A'<=(c) && (c)<='Z')
#define is_lower(c) ('a'<=(c) && (c)<='z')
#define to_lower(c) ((char)(is_upper(c) ? (c)+' ' : (c)))
#define to_upper(c) ((char)(is_lower(c) ? (c)-' ' : (c)))


  /* readSeq errors */
#define eFileNotFound   -1
#define eNoData         -2
#define eMemFull        -3
#define eItemNotFound   -4
#define eOneFormat      -5
#define eUnequalSize    -6
#define eFileCreate     -7
#define eUnknownFormat  -8
#define eOptionBad      -9
#define eASNerr         -10

  /* magic number for readSeq(whichEntry) to give seq list */
#define kListSequences  -1

  /* sequence types parsed by getseqtype */
#define kOtherSeq   0
#define kDNA        1
#define kRNA        2
#define kNucleic    3
#define kAmino      4

  /* formats known to readSeq */
#define kIG             1
#define kGenBank        2
#define kNBRF           3
#define kEMBL           4
#define kGCG            5
#define kStrider        6
#define kFitch          7
#define kPearson        8
#define kZuker          9
#define kOlsen          10
#define kPhylip2        11
#define kPhylip4        12
#define kPhylip3        kPhylip4
#define kPhylip         kPhylip4
#define kPlain          13  /* keep this at #13 */
#define kPIR            14
#define kMSF            15
#define kASN1           16
#define kPAUP           17
#define kPretty         18

#define kMaxFormat      18
#define kMinFormat      1
#define kNoformat       -1    /* format not tested */
#define kUnknown        0     /* format not determinable */

  /* subsidiary types */
#define kASNseqentry    51
#define kASNseqset      52

#define kPhylipInterleave 61
#define kPhylipSequential 62


/// Part of the ClustalW package
typedef struct  {
  boolean isactive, baseonlynum;
  boolean numright, numleft, numtop, numbot;
  boolean nameright, nameleft, nametop;
  boolean noleaves, domatch, degap;
  char  matchchar, gapchar;
  short numline, atseq;
  short namewidth, numwidth;
  short interline, spacer, seqwidth, tab;
  } prettyopts;

#define gPrettyInit(p) { \
  p.isactive=false;\
  p.baseonlynum=true;\
  p.numline= p.atseq= 0;\
  p.numright= p.numleft= p.numtop= p.numbot= false;\
  p.nameright= p.nameleft= p.nametop= false;\
  p.noleaves= p.domatch= p.degap= false;\
  p.matchchar='.';\
  p.gapchar='-';\
  p.namewidth=8;\
  p.numwidth=5;\
  p.interline=1;\
  p.spacer=10;\
  p.seqwidth=50;\
  p.tab=0; }

#ifdef UREADSEQ_G
prettyopts  gPretty;
#else
extern  prettyopts  gPretty;
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern short seqFileFormat(const char *filename, size_t *skiplines, short *error );
extern short seqFileFormatFp(FILE *fseq, size_t  *skiplines, short *error );

extern char *listSeqs(const char * const filename, const size_t skiplines, const short format, size_t *nseq, short *error );

extern char *readSeq(const size_t whichEntry, const char *filename,
                      const size_t skiplines, const short format,
                      size_t *seqlen, size_t *nseq, short *error, char *seqid );

extern char *readSeqFp(const size_t whichEntry_, FILE  *fp_, const size_t  skiplines_, const short format_, size_t  *seqlen_,  size_t *nseq_, short *error_, char *seqid_ );

extern size_t writeSeq(FILE *outf, const char *seq, const size_t seqlen, const short outform, const char *seqid );

extern unsigned long CRC32checksum(const char *seq, const size_t seqlen, size_t *checktotal);
extern unsigned long GCGchecksum(const char *seq, const size_t seqlen, size_t *checktotal);
#ifdef SMALLCHECKSUM
#define seqchecksum  GCGchecksum
#else
#define seqchecksum  CRC32checksum
#endif

extern short getseqtype(const char *seq, const size_t seqlen );
extern char *compressSeq( const char gapc, const char *seq, const size_t seqlen, size_t *newlen);

#ifdef NCBI

extern char *listASNSeqs(const char *filename, const long skiplines, const short format, size_t *nseq, short *error );

extern char *readASNSeq(const short whichEntry, const char *filename, const size_t skiplines, const short format, size_t *seqlen, size_t *nseq, short *error, char **seqid );
#endif


  /* patches for some missing string.h stuff */
extern int Strcasecmp(const char *a, const char *b);
extern int Strncasecmp(const char *a, const char *b, long maxn);

#ifdef __cplusplus
}
#endif

#endif /*UREADSEQ_H*/


