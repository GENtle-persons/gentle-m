#include "ABItype.h"

ABItype::ABItype ()
    {
    }
    
ABItype::~ABItype ()
    {
    for ( int a = 0 ; a < vf.size() ; a++ )
        if ( vf[a].data ) delete vf[a].data ;
    vf.clear () ;
    }

int ABItype::getCMBF ( unsigned char *t , int l )
    {
    unsigned char *s , *r = NULL ;
    for ( s = t + l - 8 ; s > t ; s-- )
       {
       if ( *s == 'C' )
          {
          if ( *(s+1) == 'M' &&
               *(s+2) == 'B' &&
               *(s+3) == 'F' )
             {
             r = s ;
             break ;
             }
          }
       }
    if ( !r ) return -1 ;
    return r - t ;
    }
    
void ABItype::parse ( wxString filename )
    {
	wxFile f ( filename , wxFile::read ) ;
	long l = f.Length() ;
	unsigned char *t = new unsigned char [l+15] ;
	f.Read ( t , l ) ;
	f.Close() ;
	
	// Parsing "t"
	int macOffset = getMacOffset ( t ) ; // Trying to compensate for MAC header
	if ( macOffset == -1 ) return ; // No valid ABI file
	vf.clear () ;
    
    int pos = macOffset ;
    pos += 4 ; // ABIF
    pos += 2 ; // Unknown
    pos += 4 ; // tdir
    pos += 4 ; // Instance = 1
    pos += 2 ; // Datatype
    pos += 2 ; // Datasize
    int nrecords = getInt4 ( t , pos ) ;
    pos += 4 ; // # Bytes
    int cnt = getInt4 ( t , pos ) + macOffset ; // Pointer
    
    while ( nrecords > 0 )
	    {
        TFLAG flag = getFlag ( t , cnt ) ;
        if ( flag.nbytes > 0 )
           {
           if ( flag.nbytes > 4 )
              {
              flag.data = new unsigned char [ flag.nbytes+5 ] ;
              for ( int i = 0 ; i < flag.nbytes ; i++ )
                 flag.data[i] = t[macOffset+flag.value+i] ;
              }
           vf.push_back ( flag ) ;
           }
        nrecords-- ;
        }

	delete t ;
    }
    
int ABItype::getMacOffset ( unsigned char *t )
    {
    int r = 0 ;
    if ( t[r+0]=='A' && t[r+1]=='B' && t[r+2]=='I' && t[r+3]=='F' ) return r ;
    r = 128 ;
    if ( t[r+0]=='A' && t[r+1]=='B' && t[r+2]=='I' && t[r+3]=='F' ) return r ;
    return -1 ;
    }
    
TFLAG ABItype::getFlag ( unsigned char *t , int &from )
    {
    TFLAG r ;
    r.data = NULL ;
    r.pos = from ;
    r.flag = getStr ( t , from , 4 ) ; from += 4 ;
    r.instance = getInt4 ( t , from ) ;
    r.datatype = getInt2 ( t , from ) ;
    r.datasize = getInt2 ( t , from ) ;
    r.nrecords = getInt4 ( t , from ) ;
    r.nbytes = getInt4 ( t , from ) ;
    r.value = getInt4 ( t , from ) ;
    r.spare = getInt4 ( t , from ) ;
    r.after = from ;
    return r ;
    }
    
wxString ABItype::getText ( unsigned char *t , int &from )
    {
    wxString r ;
    int l = t[from++] ;
    for ( int a = 0 ; a < l ; a++ ) r += t[from++] ;
    return r ;
    }
    
int ABItype::getInt10 ( unsigned char *t , int &from )
    {
    from += 10 ; // Essentially ignoring
    }
    
int ABItype::getInt1 ( unsigned char *t , int &from )
    {
    return (unsigned char) t[from++] ;
    }
    
int ABItype::getInt2 ( unsigned char *t , int &from )
    {
    int r = 0 ;
    int t1 = (unsigned char) t[from++] ;
    int t2 = (unsigned char) t[from++] ;
    r = t2 + ( t1 << 8 ) ;
    return r ;
    }
    
int ABItype::getInt4 ( unsigned char *t , int &from )
    {
    int r = 0 ;
    int t1 = getInt2 ( t , from ) ;
    int t2 = getInt2 ( t , from ) ;
    r = ( t1 << 16 ) + t2 ;
    return r ;
    }
    
wxString ABItype::getStr ( unsigned char *t , int from , int len )
    {
    wxString r ;
    for ( int a = 0 ; a < len ; a++ ) r += t[from+a] ;
    return r ;
    }

//--------------------------------------------

int ABItype::getRecord ( wxString id , int num )
    {
    int a ;
    for ( a = 0 ; a < vf.size() && ( vf[a].flag != id || vf[a].instance != num ) ; a++ ) ;
    if ( a == vf.size() ) return -1 ; // Not found
    return a ;
    }

wxString ABItype::getSequence ( int num )
    {
    wxString r ;
    int a = getRecord ( "PBAS" , num ) , b ;
    myass ( a > -1 , "ABItype::getSequence" ) ;
    for ( b = 0 ; b < vf[a].nbytes ; b++ ) r += vf[a].data[b] ;
    return r ;
    }
    
wxString ABItype::getRecordPascalString ( wxString id , int num )
    {
    int i = getRecord ( id , num ) ;
    if ( i == -1 ) return "" ;
    return vf[i].getPascalString() ;
    }

int ABItype::getRecordValue ( wxString id , int num )
    {
    int i = getRecord ( id , num ) ;
    if ( i == -1 ) return 0 ;
    return vf[i].value ;
    }
    
    
//***************************************

TFLAG::TFLAG ()
    {
    }

TFLAG::~TFLAG ()
    {
    }    

wxString TFLAG::getPascalString ()
    {
    if ( !data )
       {
       char t[5] ;
//       t[0] = ( value >> 24 ) & 255 ;
       t[1] = ( value >> 16 ) & 255 ;
       t[2] = ( value >>  8 ) & 255 ;
       t[3] = ( value >>  0 ) & 255 ;
       t[4] = 0 ;
       return t+1 ;
       }
    wxString r ;
    int len = (unsigned char) data[0] ;
    for ( int a = 1 ; a <= len ; a++ ) r += data[a] ;
    return r ;
    }
    
