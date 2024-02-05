#include "SCFtype.h"

// Stuff copied from staden package

/*
 * Basic type definitions
 */
typedef unsigned long int  uint_4;
typedef signed   long int  int_4;
typedef unsigned short int uint_2;
typedef signed   short int int_2;
typedef unsigned char      uint_1;
typedef signed   char      int_1;

/**    \typedef SCF_Header
    \brief Type definition for the SCF_Header structure
 */
typedef struct {
    uint_4 magic_number;
    uint_4 samples;          ///< Number of elements in Samples matrix
    uint_4 samples_offset;   ///< Byte offset from start of file
    uint_4 bases;            ///< Number of bases in Bases matrix
    uint_4 bases_left_clip;  ///< OBSOLETE: No. bases in left clip (vector)
    uint_4 bases_right_clip; ///< OBSOLETE: No. bases in right clip (qual)
    uint_4 bases_offset;     ///< Byte offset from start of file
    uint_4 comments_size;    ///< Number of bytes in Comment section
    uint_4 comments_offset;  ///< Byte offset from start of file
    char version[4];         ///< "version.revision", eg '3' '.' '0' '0'
    uint_4 sample_size;      ///< Size of samples in bytes 1=8bits, 2=16bits
    uint_4 code_set;         ///< code set used (but ignored!)
    uint_4 private_size;     ///< No. of bytes of Private data, 0 if none
    uint_4 private_offset;   ///< Byte offset from start of file
    uint_4 spare[18];        ///< Unused
} SCF_Header;

/**    \typedef SCF_Samples1
    \brief Type definition for the SCF_Samples1 data
 */
typedef struct {
        uint_1 sample_A;           ///< Sample for A trace
        uint_1 sample_C;           ///< Sample for C trace
        uint_1 sample_G;           ///< Sample for G trace
        uint_1 sample_T;           ///< Sample for T trace
} SCF_Samples1;

/**    \typedef SCF_Samples2
    \brief Type definition for the SCF_Samples2 data
 */
typedef struct {
        uint_2 sample_A;           ///< Sample for A trace
        uint_2 sample_C;           ///< Sample for C trace
        uint_2 sample_G;           ///< Sample for G trace
        uint_2 sample_T;           ///< Sample for T trace
} SCF_Samples2 ;

/*
 * Type definition for the sequence data
 */
typedef struct {
    uint_4 peak_index;        /* Index into Samples matrix for base posn */
    uint_1 prob_A;            /* Probability of it being an A */
    uint_1 prob_C;            /* Probability of it being an C */
    uint_1 prob_G;            /* Probability of it being an G */
    uint_1 prob_T;            /* Probability of it being an T */
    char   base;              /* Called base character        */
    uint_1 spare[3];          /* Spare */
} SCF_Base;

// GENtle stuff

uint_4 get_real_uint4 ( const uint_4 x )
    {
    const unsigned char *c = (const unsigned char*) (&x) ;
    uint_4 u = 0 ;
    u |= *(c+0) ;
    u <<= 8 ;
    u |= *(c+1) ;
    u <<= 8 ;
    u |= *(c+2) ;
    u <<= 8 ;
    u |= *(c+3) ;
    return u ;
    }

void make_real_uint4 ( uint_4 &x )
    {
    x = get_real_uint4 ( x ) ;
    }

uint_2 get_real_uint2 ( const uint_2 x )
    {
    const unsigned char *c = (const unsigned char*) (&x) ;
    uint_2 ret = 0 ;
    ret |= *(c+1) ;
    ret <<= 8 ;
    ret |= *(c+0) ;
    return ret ;
    }


SCFtype::SCFtype ()
    {
    }

bool SCFtype::parse ( const wxString& filename )
    {
    wxFile f ( filename , wxFile::read ) ;
    long l = f.Length() ;
    unsigned char *t = new unsigned char [l+15] ;
    f.Read ( t , l ) ;
    f.Close() ;

    if ( *(t+0) != '.' || *(t+1) != 's' || *(t+2) != 'c' || *(t+3) != 'f' )
        {
        delete [] t ;
        return false ; // No luck
        }

    SCF_Header *header = (SCF_Header*) t ;

    wxString version ;
    version += (*header).version[0] ;
    version += (*header).version[1] ;
    version += (*header).version[2] ;
    version += (*header).version[3] ;
    if ( (*header).version[0] < '3' )
        {
        wxMessageBox ( _T("Cannot read SFC prior to 3.0, this is ") + version ) ;
                delete [] t;
        return false ;
        }

    // Requires SCF V3.0 from here on!
    make_real_uint4 ( (*header).comments_size ) ;
    make_real_uint4 ( (*header).comments_offset ) ;
    make_real_uint4 ( (*header).bases ) ;
    make_real_uint4 ( (*header).bases_offset ) ;
    make_real_uint4 ( (*header).samples ) ;
    make_real_uint4 ( (*header).samples_offset ) ;
    make_real_uint4 ( (*header).sample_size ) ;

    if ( (*header).samples == 0 || (*header).bases == 0 ) return false ; // Paranoia

    unsigned char *c ;

    // Read comment
    sd.comment.Empty() ;
    unsigned int a;
    for ( a = 0 , c = t + (*header).comments_offset ; a < (*header).comments_size ; a++ , c++ )
        sd.comment += *c ;

    // Preparing to read sequence information
    void *v = t + (*header).bases_offset ;
    unsigned long *ulp ;
    sd.sequence.clear () ;
    sd.seq.Empty () ;
    for ( a = 0 , ulp = (unsigned long*) v ; a < (*header).bases ; a++ , ulp++ )
        {
        TSequencerDataSequenceItem si ;
        si.peak_index = get_real_uint4 ( *ulp ) ;
        sd.sequence.push_back ( si ) ;
        }

    // Reading sequence data
    for ( unsigned int a = 0 ; a < 5 ; a++ )
        {
        v = t + (*header).bases_offset + (*header).bases * ( 4 + a ) ;
        read_data_block ( v , a ) ;
        }

    // Sorting sequence data; might be unnecessary
    for ( unsigned int a = 1 ; a < sd.sequence.size() ; a++ )
        {
        if ( sd.sequence[a-1].peak_index <= sd.sequence[a].peak_index ) continue ;
        TSequencerDataSequenceItem d = sd.sequence[a-1] ;
        sd.sequence[a-1] = sd.sequence[a] ;
        sd.sequence[a] = d ;
        a = 0 ;
        }

/*
    // Maximum trace data
    unsigned long min = 9999 , max = 0 ;
    for ( unsigned int a = 0 ; a < sd.sequence.size() ; a++ )
        {
        if ( max < sd.sequence[a].peak_index ) max = sd.sequence[a].peak_index ;
        if ( min > sd.sequence[a].peak_index ) min = sd.sequence[a].peak_index ;
        }
    if ( max >= sd.tracer.size() ) return false ; // Paranoia
*/

    // Read tracer data
    sd.tracer.clear () ;
    while ( sd.tracer.size() < (*header).samples )
        sd.tracer.push_back ( TSequencerDataTracerItem () ) ;
    for ( unsigned int a = 0 ; a < 4 ; a++ )
        {
        unsigned long offset = (*header).samples_offset + (*header).samples * a * (*header).sample_size ;
        v = t + offset ;
        read_tracer_block ( v , a , (*header).sample_size ) ;
        }

    // Fix tracer diff diff hell
    for ( unsigned int a = 0 ; a < 4 ; a++ )
        fix_diff ( a ) ;

    // Setup alternative tracer data set
    for ( unsigned int a = 0 ; a < 4 ; a++ ) sd.tracer2[a].clear () ;
    for ( unsigned int a = 0 ; a < sd.tracer.size() ; a++ )
        {
        TSequencerDataTracerItem ti = sd.tracer[a] ;
        sd.tracer2[TRACER_ID_A].push_back ( ti.data[0] ) ;
        sd.tracer2[TRACER_ID_C].push_back ( ti.data[1] ) ;
        sd.tracer2[TRACER_ID_G].push_back ( ti.data[2] ) ;
        sd.tracer2[TRACER_ID_T].push_back ( ti.data[3] ) ;
        }


    wxFile out ( _T("C:\\test.txt") , wxFile::write ) ;
    for ( unsigned int a = 0 ; a < sd.sequence.size() ; a++ )
    {
        out.Write ( wxString::Format ( _T("%5d: %5d %c\r\n") , a , sd.sequence[a].peak_index , sd.sequence[a].base ) ) ;
    }

/*
    // THE FOLLOWING MESS MIGHT PROVE USEFUL ONE DAY
    // TO IMPORT SCF VERSIONS 1 AND 2 AS WELL
    v += (*header).bases * 4
    for ( a = 0 , base = (SCF_Base*) v ; a < (*header).bases ; a++ , base++ )
        {
        TSequencerDataSequenceItem si = sd[a] ;
//        si.peak_index = get_real_uint4 ( (*base).peak_index ) ;
        si.prob_a = (*base).prob_A ;
        si.prob_c = (*base).prob_C ;
        si.prob_g = (*base).prob_G ;
        si.prob_t = (*base).prob_T ;

        char c = 'A' ;
        int prob = si.prob_a ;
        if ( prob < si.prob_c ) { prob = si.prob_c ; c = 'C' ; }
        if ( prob < si.prob_g ) { prob = si.prob_g ; c = 'G' ; }
        if ( prob < si.prob_t ) { prob = si.prob_t ; c = 'T' ; }
        sd.seq += c ;

        si.base = (*base).base ;
        sd[a] = si ;
//        sd.sequence.push_back ( si ) ;
//        out += wxString::Format ( _T("%d:%c\n\r\n") , si.peak_index , si.base ) ;
        }

    // Read samples
    SCF_Samples1 *s1 = NULL ;
    SCF_Samples2 *s2 = NULL ;
    v = t + (*header).samples_offset ;
    bool use_word = (*header).sample_size == 2 ;
    if ( use_word ) s2 = (SCF_Samples2 *) v ;
    else s1 = (SCF_Samples1 *) v ;
    sd.tracer.clear () ;
    for ( unsigned int a = 0 ; a < 4 ; a++ ) sd.tracer2[a].clear () ;
    for ( unsigned int a = 0 ; a < (*header).samples ; a++ )
        {
        TSequencerDataTracerItem ti ;
        ti.a = use_word ? get_real_uint2 ( s2->sample_A ) : s1->sample_A ;
        ti.c = use_word ? get_real_uint2 ( s2->sample_C ) : s1->sample_C ;
        ti.g = use_word ? get_real_uint2 ( s2->sample_G ) : s1->sample_G ;
        ti.t = use_word ? get_real_uint2 ( s2->sample_T ) : s1->sample_T ;
//        out += wxString::Format ( _T("%d,%d,%d,%d; ") , ti.a , ti.c , ti.g , ti.t ) ;
        sd.tracer.push_back ( ti ) ;
        sd.tracer2[TRACER_ID_A].push_back ( ti.a ) ;
        sd.tracer2[TRACER_ID_C].push_back ( ti.c ) ;
        sd.tracer2[TRACER_ID_G].push_back ( ti.g ) ;
        sd.tracer2[TRACER_ID_T].push_back ( ti.t ) ;
        if ( use_word ) s2++ ;
        else s1++ ;
        }

    for ( unsigned int a = 1 ; a < sd.sequence.size() ; a++ )
        {
        if ( sd.sequence[a-1].peak_index <= sd.sequence[a].peak_index ) continue ;
        TSequencerDataSequenceItem d = sd.sequence[a-1] ;
        sd.sequence[a-1] = sd.sequence[a] ;
        sd.sequence[a] = d ;
        a = 0 ;
        }
*/
//    wxMessageBox ( out ) ;
//    wxMessageBox ( sd.comment ) ;

    delete [] t ;
    return true ;
    }

void SCFtype::read_tracer_block ( const void * const v , const unsigned int mode , const unsigned int sample_size )
    {
    unsigned int a , samples = sd.tracer.size() ;
    const unsigned char *c = (unsigned char*) v ;
    unsigned long lastx = 0 ;
    for ( a = 0 ; a < samples ; a++ )
        {
        unsigned long x = 0 ;
        if ( sample_size == 2 )
            {
            x |= (unsigned char) *(c+0) ;
            x <<= 8 ;
            x |= (unsigned char) *(c+1) ;

            if ( x >= 32768 ) {
                x = x - 65536 ;
            }

            c += 2 ;
            }
        else x = *c++ ;
        sd.tracer[a].data[mode] = x ;
        }
    }

void SCFtype::read_data_block ( const void * const v , const unsigned int mode )
    {
    unsigned int bases = sd.sequence.size() ;
    const unsigned char * c = (unsigned char*) v ;
    unsigned int a ;
    for ( a = 0 ; a < bases ; a++ , c++ )
        {
        switch ( mode )
            {
            case 0 : sd.sequence[a].prob_a = *c ; break ;
            case 1 : sd.sequence[a].prob_c = *c ; break ;
            case 2 : sd.sequence[a].prob_g = *c ; break ;
            case 3 : sd.sequence[a].prob_t = *c ; break ;
            case 4 : sd.sequence[a].base = *c ; sd.seq += *c ; break ;
            }
        }
    }

void SCFtype::fix_diff ( const int mode )
    {
    signed long p_sample = 0;
    unsigned long num_samples = sd.tracer.size() ;
    for ( unsigned long i = 0 ; i < num_samples ; i++ )
        {
        p_sample += sd.tracer[i].data[mode] ;
        sd.tracer[i].data[mode] = p_sample ;
        }
    p_sample = 0 ;
    for ( unsigned long i = 0 ; i < num_samples ; i++ )
        {
        p_sample += sd.tracer[i].data[mode] ;
        sd.tracer[i].data[mode] = p_sample ;
        }
    }
