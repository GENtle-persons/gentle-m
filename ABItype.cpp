#include "ABItype.h"

ABItype::~ABItype ()
    {
    while ( vf.size() )
        {
        if ( vf[vf.size()-1].data != NULL )
                delete vf[vf.size()-1].data ;
        vf.pop_back () ;
        }
    }

ABItype::ABItype ()
    {
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
               *(s+3) == 'F' ) /*&&
               *(s+4) == 0 &&
               *(s+5) == 0 &&
               *(s+6) == 0 &&
               *(s+7) == 1 &&*/
             {
             r = s ;
             break ;
             }
          }
       }
    if ( !r ) return -1 ;
    return r - t ;
    }
    
void ABItype::parse ( string filename )
    {
	wxFile f ( filename.c_str() , wxFile::read ) ;
	long l = f.Length() ;
	unsigned char *t = new unsigned char [l+15] ;
	f.Read ( t , l ) ;
	f.Close() ;
	
	// Parsing "t"
	int macOffset = getMacOffset ( t ) ; // Trying to compensate for MAC header
	if ( macOffset == -1 ) return ; // No valid ABI file
    while ( vf.size() ) vf.pop_back () ;
	int cnt = 0 , i ;
    cnt = getCMBF ( t , l ) ;
    if ( cnt == -1 ) return ; // No valid ABI file

	while ( cnt < l )
	    {
        TFLAG flag = getFlag ( t , cnt ) ;
        if ( flag.nbytes > 0 )
           {
           if ( flag.nbytes > 4 )
              {
              flag.data = new unsigned char [ flag.nbytes+5 ] ;
              for ( i = 0 ; i < flag.nbytes ; i++ )
                 flag.data[i] = t[macOffset+flag.value+i] ;
              }
           vf.push_back ( flag ) ;
           }
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
    
string ABItype::getText ( unsigned char *t , int &from )
    {
    string r ;
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
    
string ABItype::getStr ( unsigned char *t , int from , int len )
    {
    string r ;
    for ( int a = 0 ; a < len ; a++ ) r += t[from+a] ;
    return r ;
    }

//--------------------------------------------

int ABItype::getRecord ( string id , int num )
    {
    int a ;
    for ( a = 0 ; a < vf.size() && ( vf[a].flag != id || vf[a].instance != num ) ; a++ ) ;
    if ( a == vf.size() ) return -1 ; // Not found
    return a ;
    }

string ABItype::getSequence ( int num )
    {
    string r ;
    int a = getRecord ( "PBAS" , num ) , b ;
    myass ( a > -1 , "ABItype::getSequence" ) ;
    for ( b = 0 ; b < vf[a].nbytes ; b++ ) r += vf[a].data[b] ;
    return r ;
    }
    
string ABItype::getRecordPascalString ( string id , int num )
    {
    int i = getRecord ( id , num ) ;
    if ( i == -1 ) return "" ;
    return vf[i].getPascalString() ;
    }

int ABItype::getRecordValue ( string id , int num )
    {
    int i = getRecord ( id , num ) ;
    if ( i == -1 ) return 0 ;
    return vf[i].value ;
    }
    
    
//***************************************

string TFLAG::getPascalString ()
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
    string r ;
    int len = (unsigned char) data[0] ;
    for ( int a = 1 ; a <= len ; a++ ) r += data[a] ;
    return r ;
    }
    
