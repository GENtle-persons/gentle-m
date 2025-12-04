#include "MyApp-class.h"
#include "myapp.h"

static MyApp *theapp ;

MyApp *myapp ()
   {
   if (!theapp)
       {
       wxLogError ( _T("myapp(): Application pointer is NULL!") ) ;
       wxASSERT ( false ) ;
       } 
   return theapp ;
   }

void setMyApp ( MyApp *app )
   {
   theapp = app ;
   }

