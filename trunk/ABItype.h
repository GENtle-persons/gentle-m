#ifndef _ABI_TYPE_H_
#define _ABI_TYPE_H_

#include "main.h"

#include <math.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

class TFLAG
    {
    public :
    int pos , after ;
    unsigned char *data ;
    
    wxString getPascalString () ;
    
    wxString flag ;
    int instance ;
    int datatype ;
    int datasize ;
    int nrecords ;
    int nbytes ;
    int value ;
    int remainder ;
    int spare ;
    } ;

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
    wxString getText ( unsigned char *t , int &from ) ;
    int getInt1 ( unsigned char *t , int &from ) ;
    int getInt2 ( unsigned char *t , int &from ) ;
    int getInt4 ( unsigned char *t , int &from ) ;
    int getInt10 ( unsigned char *t , int &from ) ;
    int getCMBF ( unsigned char *t , int l ) ;
    
    // Variables
    vector <TFLAG> vf ;
    } ;

#endif
