#ifndef __AUTOANNOTATE__
#define __AUTOANNOTATE__

#include "main.h"

class MyChild ;

class AutoAnnotate
	{
	public :
	AutoAnnotate ( MyChild *_p = NULL ) ;
	virtual void ScanDatabase ( wxString database ) ;
	
	private :
	virtual bool addORFs ( TVector *v ) ;
 	virtual bool MatchItem ( TVector *tv , TVectorItem &item , TVector *v , wxString &oseq ) ;
 	virtual bool RawMatch ( TVectorItem &item , TVector *v , wxString &oseq , wxString &s ) ;
 	virtual void machete ( TVector *v ) ;
 	virtual bool within ( TVectorItem &i1 , TVectorItem &i2 , TVector *v ) ;
 	
 	MyChild *p ;
 	wxArrayString alreadyin ;
	} ;    

#endif
