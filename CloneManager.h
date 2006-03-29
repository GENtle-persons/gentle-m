#ifndef __CLONE_MANAGER_5_H_
#define __CLONE_MANAGER_5_H_

#include "main.h"


class TVector ;
class TVectorItem ;

class TCloneManager
    {
    public :
    TCloneManager () ; ///< Constructor
    ~TCloneManager () ; ///< Dummy destructor
	
    void load ( wxString file ) ; ///< Load XML from file
    bool success () { return _success ; } ///< Errors during parsing?
    int countVectors () ; ///< How many vectors were made from the XML?
    TVector *getVector ( int a ) ; ///< Returns pointer to one of the vectors
    
    private :
	int scan_item ( unsigned char *t , int a , TVector *v ) ;
		
    bool _success ; ///< Internal success marker
    vector <TVector*> _v ; ///< The generated vectors
    } ;


#endif
