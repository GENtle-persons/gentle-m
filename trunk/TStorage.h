/** \file
	\brief The TStorage and TSQLresult classes
*/
#ifndef _TSTORAGE_H_
#define _TSTORAGE_H_

#include <stdio.h>
#include "main.h"

/// Additionally, use MySQL (instead of sqlite only)
#define USEMYSQL

#ifdef USEMYSQL
 #ifdef __WXMSW__
  #include <mysql.h>
 #else
  #include "/usr/include/mysql/mysql.h"
 #endif
#endif

#ifdef __WXMSW__
 #include "win_sqlite.h"
 #include "win_sqlite3.h"
 #define USING_SQLITE_3
 #include <wx/msw/winundef.h>
#else
 #include "lin_sqlite.h"
#endif

#define LOCAL_STORAGE 1
#define SHARED_STORAGE 2
#define TEMP_STORAGE 3

class TRestrictionEnzyme ;
class MyApp ;
class TProtease ;

/// This class stores the results from SQL queries
class TSQLresult
    {
    public:
    wxArrayString field ; ///< List of result fields (or column names)
    vector <wxArrayString> content ; ///< The results table ([row][column])
    
    virtual void clean() ///< Reset internal state
        {
        field.Clear() ;
        while ( content.size() ) content.pop_back() ;
        }
    virtual int cols () { return field.GetCount() ; }  ///< Number of columns (fields) in the result table
    virtual int rows () { return content.size() ; } ///< Number of rows in the result table
    virtual wxString item ( char *s , int i ) ///< Returns entry for field s in row i
        {
        int a ;
        wxString s2 = s ;
        for ( a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return content[i][a] ;
        return "" ;
        }
    virtual wxArrayString & operator [] ( int i ) ///< Returns row i
        {
        return content[i] ;
        }
    virtual int operator [] ( char *s ) ///< Returns the number of field s
        {
        int a ;
        wxString s2 = s ;
        for ( a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return a ;
        return -1 ;
        }
    } ;

/// This is the famous storage class. It manages all MySQL and sqlite traffic
class TStorage
    {
    public :
    TStorage ( int nt , wxString fn = "" ) ;
    virtual ~TStorage () ;
    virtual void createDatabase () ;
    virtual TSQLresult getObject ( const wxString &query ) ;

    // Access
    virtual void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , wxString value ) ;
    virtual void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , char* value ) ;
    virtual void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , int value ) ;
    
    // Restriction enzymes
    virtual void import () ;
    virtual TRestrictionEnzyme* getRestrictionEnzyme ( wxString s ) ;
    virtual void getEnzymeGroups ( wxArrayString &vs ) ;
    virtual void getEnzymesInGroup ( wxString gn , wxArrayString &vs ) ;
    virtual void updateRestrictionEnzyme ( TRestrictionEnzyme *e ) ;
    virtual void addRestrictionEnzyme ( TRestrictionEnzyme *r ) ;
    virtual bool addEnzymeGroup ( wxString s ) ;

    virtual TProtease *getProtease ( wxString s ) ;
    virtual void updateProtease ( TProtease *p ) ;
    
    virtual wxString getDatabaseList ( wxArrayString &name , wxArrayString &file ) ;
    virtual void setOption ( wxString oname , int value ) ;
    virtual void setOption ( wxString oname , wxString vname ) ;
    virtual int getOption ( wxString oname , int def ) ;
    virtual wxString getOption ( wxString oname , wxString def ) ;
    virtual bool copySQLfields ( TStorage &target , wxString table , wxString cond ) ;
    virtual void synchronize () ;

    virtual void autoUpdateSchema () ;
    virtual wxString fixDNAname ( wxString s ) ;
    virtual wxString UCfirst ( wxString s ) ;
    virtual wxString getDBname () ;    
    virtual bool getWriteProtect () ;
    static wxString createMySQLdb ( wxString ip , wxString db , wxString name , wxString pwd ) ;
    virtual void optimizeDatabase () ;
    virtual wxString getDefaultDB () ;
    virtual void addEnzymeToGroup ( wxString enzyme , wxString group ) ;
    virtual void removeEnzymeFromGroup ( wxString enzyme , wxString group ) ;
    virtual void removeEnzymeGroup ( wxString group ) ;
    virtual void startRecord () ;
    virtual void endRecord () ;
    
    // Variables
    wxArrayTRestrictionEnzyme re ;
    wxArrayTProtease pr ;
    TSQLresult results ;
    
    private :
    virtual wxString getSingleField ( wxString query , wxString field , wxString def = "" ) ;
    virtual int getSingleField ( wxString query , wxString field , int def = 0 ) ;
    virtual wxString makeInsert ( wxString table , wxArrayString &field , wxArrayString &data ) ;
    virtual void replaceTable ( wxString table , wxArrayString &f , wxArrayString &t ) ;
    virtual void tableInfoSet ( wxArrayString &f , wxArrayString &t , wxString nf , wxString nt ) ;
    virtual TStorage *getDBfromEnzymeGroup ( wxString group ) ;
    virtual wxString stripGroupName ( wxString s ) ;
    virtual void cleanEnzymeGroupCache () ;
    virtual void setEnzymeCache ( wxString group , wxArrayString &enzymes ) ;
    virtual void getEnzymeCache ( wxString group , wxArrayString &enzymes ) ;
    virtual bool isLocalDB () ;
    virtual bool convertSqlite2to3 () ;
    virtual void createDatabaseSqlite3 () ;
    virtual TSQLresult getObjectSqlite2 ( const wxString &query ) ;
    virtual TSQLresult getObjectSqlite3 ( const wxString &query ) ;
    virtual TSQLresult getObject_MySQL ( const wxString &query ) ;
    
    // Variables
    wxArrayString enzymeGroupCache , enzymeGroupNameCache ;
    wxString dbname , error ;
    int storagetype , ierror ;
    bool writeProtect ;
    int rpv ;
    wxString record ;
    bool recording ;
    bool isMySQL , isSqlite3;
#ifdef USEMYSQL
    MYSQL *conn,*mysql;
#endif
    } ;
 
#endif

