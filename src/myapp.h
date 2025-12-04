#ifndef _MYAPP_H_
#define _MYAPP_H_


/** \fn myass ( bool b , wxString msg )
    \brief "My assertion" - little inside joke...
    \param b The condition given in the call. No assertion when b is FALSE.
    \param msg The message string to write into errout.
*/
/** \def MYDEBUG
    \brief Turns on output of the myass function; should not be used for releases
*/

/** \def MYLOG
    \brief Turns on output of logging; should not be used for releases
*/

/** \def MYTEST
    \brief Turns on both MYDEBUG and MYLOG; should not be used for releases
*/

#ifdef MYDEBUG
    #define myass(a,b) myapp()->do_my_ass(a,b)
#else
    #define myass(a,b)
#endif

#ifdef MYLOG
    #define mylog(a,b) myapp()->do_my_log(a,b)
#else
    #define mylog(a,b)
#endif

#ifdef MYTEST
    #ifndef MYLOG
        #define MYLOG
    #endif // MYLOG
    #ifndef MYDEBUG
        #define MYDEBUG
    #endif // MYDEBUG
#endif // MYTEST

class MyApp;

/** \brief Returns a pointer to the application */
MyApp *myapp () ;

void setMyApp ( MyApp *app ) ;

#endif
