#ifndef _ABI_TYPE_H_
#define _ABI_TYPE_H_

#include "main.h"

#include <math.h>
//#include <io.h>
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
    
    string getPascalString () ;
    
    string flag ;
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
    
    int getRecord ( string id , int num ) ;
    int getRecordValue ( string id , int num ) ;
    string getRecordPascalString ( string id , int num ) ;
    string getSequence ( int num = 2 ) ;
    int getMacOffset ( unsigned char *t ) ;
    
    // Parser functions
    void parse ( string filename ) ;
    TFLAG getFlag ( unsigned char *t , int &from ) ;
    string getStr ( unsigned char *t , int from , int len ) ;
    string getText ( unsigned char *t , int &from ) ;
    int getInt1 ( unsigned char *t , int &from ) ;
    int getInt2 ( unsigned char *t , int &from ) ;
    int getInt4 ( unsigned char *t , int &from ) ;
    int getInt10 ( unsigned char *t , int &from ) ;
    int getCMBF ( unsigned char *t , int l ) ;
    
    // Variables
    vector <TFLAG> vf ;
    } ;

#endif
