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
 #elif __WXMAC__
  #include <mysql.h>
 #else
  #include "/usr/include/mysql/mysql.h"
 #endif
#endif

#ifdef __WXMSW__
 #include "win_sqlite.h"
 #include "win_sqlite3.h"
 #include <wx/msw/winundef.h>
#elif __WXMAC__
 #include "mac_sqlite2.h"
 #include "mac_sqlite3.h" 
#else
 #include "lin_sqlite.h"
 #include "lin_sqlite3.h"
#endif

#define USING_SQLITE_3

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
    
    /// \brief Reset internal state
    virtual void clean()
        {
        field.Clear() ;
        while ( content.size() ) content.pop_back() ;
        }
    virtual int cols () { return field.GetCount() ; }  ///< Number of columns (fields) in the result table
    virtual int rows () { return content.size() ; } ///< Number of rows in the result table
    
    /// \brief Returns entry for field s in row i
    virtual wxString item ( char *s , int i )
        {
        int a ;
        wxString s2 ( s , *wxConvCurrent ) ;
        for ( a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return content[i][a] ;
        return _T("") ;
        }
    
    ///< \brief Returns row i
    virtual wxArrayString & operator [] ( int i )
        {
        return content[i] ;
        }
        
    /// \brief Returns the number of field s
    virtual int operator [] ( wxString &s2 )
        {
        int a ;
        for ( a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return a ;
        return -1 ;
        }
    
     /// \brief Returns the number of field s
    virtual int operator [] ( const char *s )
        {
        int a ;
        wxString s2 ( s , *wxConvCurrent ) ;
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
    TStorage ( int nt , wxString fn = _T("") ) ; ///< Constructor
    virtual ~TStorage () ; ///< Destructor
    virtual void createDatabase () ; ///< Creates the database, is it does not exist
    virtual TSQLresult getObject ( const wxString &query ) ; ///< Runs a query

    // Access
    virtual void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , wxString value ) ;  ///< Query construction helper method
    virtual void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , char* value ) ;  ///< Query construction helper method
    virtual void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , int value ) ;  ///< Query construction helper method
    
    // Restriction enzymes
    virtual void import () ; ///< Import enzymes from database
    virtual TRestrictionEnzyme* getRestrictionEnzyme ( wxString s ) ; ///< Pointer to TRestrictionEnzyme from re
    virtual void getEnzymeGroups ( wxArrayString &vs ) ; ///< List of enzyme groups in this database
    virtual void getEnzymesInGroup ( wxString gn , wxArrayString &vs ) ; ///< List of enzymes in a specific group
    virtual void updateRestrictionEnzyme ( TRestrictionEnzyme *e ) ; ///< Write enzyme information back to database
    virtual void addRestrictionEnzyme ( TRestrictionEnzyme *r ) ; ///< Add new restriction enzyme
    virtual bool addEnzymeGroup ( wxString s ) ; ///< Add new enzyme group

    virtual TProtease *getProtease ( wxString s ) ; ///< Pointer to a TProtease from pr
    virtual void updateProtease ( TProtease *p ) ; ///< Write protease information back to database
    
    virtual wxString getDatabaseList ( wxArrayString &name , wxArrayString &file ) ; ///< List of all known databases
    virtual void setOption ( wxString oname , int value ) ; ///< Set option in local database
    virtual void setOption ( wxString oname , wxString vname ) ; ///< Set option in local database
    virtual int getOption ( wxString oname , int def ) ; ///< Get option from local database
    virtual wxString getOption ( wxString oname , wxString def ) ; ///< Get option from local database
    virtual bool copySQLfields ( TStorage &target , wxString table , wxString cond ) ; ///< Internal use for update
    virtual void synchronize () ; ///< Synchronize information between databases (not used)

    virtual void autoUpdateSchema () ; ///< Update database to a new schema
    virtual wxString fixDNAname ( wxString s ) ; ///< Fix name quotes
    virtual wxString UCfirst ( wxString s ) ; ///< Uppercase first letter in string
    virtual wxString getDBname () ; ///< Return the database name
    virtual bool getWriteProtect () ; ///< Is this database write protected?
    static wxString createMySQLdb ( wxString ip , wxString db , wxString name , wxString pwd ) ;  ///< Create a MySQL database
    virtual void optimizeDatabase () ; ///< Optimize sqlite database
    virtual wxString getDefaultDB () ; ///< Returns the name of the default (standard) database
    virtual void addEnzymeToGroup ( wxString enzyme , wxString group ) ; ///< Add a restriction enzyme to an enzyme group
    virtual void removeEnzymeFromGroup ( wxString enzyme , wxString group ) ; ///< Remove an enzyme from an enzyme group
    virtual void removeEnzymeGroup ( wxString group ) ; ///< Remove an enzyme group
    virtual void startRecord () ; ///< Start recording queries
    virtual void endRecord () ; ///< Stop recording, execute all recorded queries
    
    // Variables
    wxArrayTRestrictionEnzyme re ; ///< List of restriction enzymes in this database
    wxArrayTProtease pr ; ///< List of proteases in this database
    TSQLresult results ; ///< The results of the last query
    
    private :
    virtual wxString getSingleField ( wxString query , wxString field , wxString def = _T("") ) ; ///< Get a single field from a query, with default value
    virtual int getSingleField ( wxString query , wxString field , int def = 0 ) ; ///< Get a single field from a query, with default value
    virtual wxString makeInsert ( wxString table , wxArrayString &field , wxArrayString &data ) ; ///< Insert all the fields with their data into a table
    virtual void replaceTable ( wxString table , wxArrayString &f , wxArrayString &t ) ; ///< ???
    virtual void tableInfoSet ( wxArrayString &f , wxArrayString &t , wxString nf , wxString nt ) ; ///< ???
    virtual TStorage *getDBfromEnzymeGroup ( wxString group ) ; ///< Returns the database of the enzyme group
    virtual wxString stripGroupName ( wxString s ) ; ///< ???
    virtual void cleanEnzymeGroupCache () ; ///< Clear the enzyme group cache
    virtual void setEnzymeCache ( wxString group , wxArrayString &enzymes ) ; ///< Set cache for an enzyme group
    virtual void getEnzymeCache ( wxString group , wxArrayString &enzymes ) ; ///< Get cached enzyme group data
    virtual bool isLocalDB () ; ///< Is this the local database?
    virtual bool convertSqlite2to3 () ; ///< Converts a sqlite2 database into sqlite3
    virtual void createDatabaseSqlite3 () ; ///< Creates an sqlite3 database (NOT FUNCTIONAL)
    virtual TSQLresult getObjectSqlite2 ( const wxString &query ) ; ///< Get object from sqlite2 database
    virtual TSQLresult getObjectSqlite3 ( const wxString &query ) ; ///< Get object from sqlite3 database
    virtual TSQLresult getObject_MySQL ( const wxString &query ) ; ///< Get object from MySQL database
    
    // Variables
    wxArrayString enzymeGroupCache , enzymeGroupNameCache ;
    wxString dbname , error ;
    int storagetype , ierror ;
    bool writeProtect ; ///< Database is write-protected, because this program version is too old
    int rpv ; ///< Required Program Version
    wxString record ; ///< Recorded queries
    bool recording ; ///< Record queries?
    bool isMySQL ; ///< This is a MySQL database
    bool isSqlite3; ///< This is a sqlite3 database
#ifdef USEMYSQL
    MYSQL *conn,*mysql;
#endif
    } ;
 
#endif

