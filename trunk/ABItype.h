/** \file
	\brief Contains the ABItype class, as well as its TFLAG helper class
*/
#ifndef _ABI_TYPE_H_
#define _ABI_TYPE_H_

#include "main.h"

#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <algorithm>

/** \class TFLAG
	\brief Helper class for ABItype
*/
class TFLAG
    {
    public :
    TFLAG () ;
    ~TFLAG () ;
    /// \var pos ???
    /// \var after ????
    /// \var data Carries the associated data
    int pos , after ;
    unsigned char *data ;
    
    wxString getPascalString () ;
    
    wxString flag ; ///< The flag name
    int instance ; ///< Flag data item
    int datatype ; ///< Flag data item
    int datasize ; ///< Flag data item
    int nrecords ; ///< Flag data item
    int nbytes ; ///< Flag data item
    int value ; ///< Flag data item
    int remainder ; ///< Flag data item
    int spare ; ///< Flag data item
    } ;

/** \class ABItype
	\brief Reads files in ABI format
*/
class ABItype
    {
    public :
    ABItype () ;
    ~ABItype () ;
    
    int getRecord ( wxString id , int num ) ;
    int getRecordValue ( wxString id , int num ) ;
    wxString getRecordPascalString ( wxString id , int num ) ;
    wxString getSequence ( int num = 2 ) ;
    int getMacOffset ( unsigned char *t ) ;
    
    // Parser functions
    void parse ( wxString filename ) ;
    TFLAG getFlag ( unsigned char *t , int &from ) ;
    wxString getStr ( unsigned char *t , int from , int len ) ;
    wxString getText ( unsigned char *t , int &from ) ; ///< Reads a Pascal-like string
    int getInt1 ( unsigned char *t , int &from ) ; ///< Reads a 1-byte number
    int getInt2 ( unsigned char *t , int &from ) ; ///< Reads a 2-byte number
    int getInt4 ( unsigned char *t , int &from ) ; ///< Reads a 4-byte number
    int getInt10 ( unsigned char *t , int &from ) ; ///< Jumps ahead 10 bytes, ignores contents
    int getCMBF ( unsigned char *t , int l ) ; ///< Reads a Pascal-like string with known length
    
    // Variables
    vector <TFLAG> vf ; ///< All the flags
    } ;

#endif
