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

    wxString getPascalString () const ;

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

    int getRecord ( const wxString& id , const int num ) const ;
    int getRecordValue ( const wxString& id , const int num ) const ;
    wxString getRecordPascalString ( const wxString& id , const int num ) const ;
    wxString getSequence ( const int num = 2 ) const ;
    int getMacOffset ( const unsigned char * const t ) const ;

    // Parser functions
    void parse ( const wxString& filename ) ;
    TFLAG getFlag ( const unsigned char * const t , int &from ) const ;
    wxString getStr ( const unsigned char * const t , const  int from , const int len ) const ;
    wxString getText ( const unsigned char * const t , int &from ) const ; ///< Reads a Pascal-like string
    int getInt1 ( const unsigned char * const t , int &from ) const ; ///< Reads a 1-byte number
    int getInt2 ( const unsigned char * const t , int &from ) const ; ///< Reads a 2-byte number
    int getInt4 ( const unsigned char * const t , int &from ) const ; ///< Reads a 4-byte number
    int getInt10 ( const unsigned char * const t , int &from ) const ; ///< Jumps ahead 10 bytes, ignores contents
    int getCMBF ( const unsigned char * const t , const int l ) const ; ///< Reads a Pascal-like string with known length

    // Variables
    vector <TFLAG> vf ; ///< All the flags
    } ;

#endif
