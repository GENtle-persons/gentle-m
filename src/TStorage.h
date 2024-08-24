/** \file
    \brief The TStorage and TSQLresult classes
*/
#ifndef _TSTORAGE_H_
#define _TSTORAGE_H_

#include <stdio.h>
#include "main.h"

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
    TSQLresult () {} ; ///< Empty constructor
    ~TSQLresult () {} ; ///< Dummy destructor
    wxArrayString field ; ///< List of result fields (or column names)
    vector <wxArrayString> content ; ///< The results table ([row][column])

    void clean() ; ///< Reset internal state
    int cols () const ; ///< Number of columns (fields) in the result table
    int rows () const ; ///< Number of rows in the result table
    wxString item ( const char * const s , const int i ) const ; ///< Returns entry for field s in row i

    ///< \brief Returns row i
    wxArrayString & operator [] ( int i )
        {
        return content[i] ;
        }

    /// \brief Returns the number of field s
    int operator [] ( const wxString &s2 ) const
        {
        for ( int a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return a ;
        return -1 ;
        }

     /// \brief Returns the number of field s
    int operator [] ( const char * const s )
        {
        wxString s2 ( s , *wxConvCurrent ) ;
        for ( int a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return a ;
        return -1 ;
        }
} ;

/// This is the famous storage class. It manages all MySQL and sqlite traffic
class TStorage
    {
    public :
    TStorage ( int nt , const wxString& fn = "" ) ; ///< Constructor
    ~TStorage () ; ///< Destructor
    void createDatabase () ; ///< Creates the database, is it does not exist
    TSQLresult getObject ( const wxString &query ) ; ///< Runs a query

    // Access
    void sqlAdd ( wxString &s1 , wxString &s2 , const wxString& key , const wxString& value ) const ;  ///< Query construction helper method
    void sqlAdd ( wxString &s1 , wxString &s2 , const wxString& key , const char * const value ) const ;  ///< Query construction helper method
    void sqlAdd ( wxString &s1 , wxString &s2 , const wxString& key , const int value ) const ;  ///< Query construction helper method

    // Restriction enzymes
    void import () ; ///< Import enzymes from database
    TRestrictionEnzyme* getRestrictionEnzyme ( const wxString& s ) ; ///< Pointer to TRestrictionEnzyme from re
    void getEnzymeGroups ( wxArrayString &vs ) ; ///< List of enzyme groups in this database
    void getEnzymesInGroup ( const wxString& gn , wxArrayString &vs ) ; ///< List of enzymes in a specific group
    void updateRestrictionEnzyme ( /* not const */ TRestrictionEnzyme * const e ) ; ///< Write enzyme information back to database
    void addRestrictionEnzyme ( TRestrictionEnzyme * const r ) ; ///< Add new restriction enzyme
    bool addEnzymeGroup (  const wxString& s ) ; ///< Add new enzyme group
    void markEnzymeForDeletion (  const wxString& s ) ; ///< Mark an enzyme for deletion

    TProtease *getProtease ( const wxString& s ) const ; ///< Pointer to a TProtease from pr
    void updateProtease ( TProtease * const p ) ; ///< Write protease information back to database

    wxString getDatabaseList ( /* not const */ wxArrayString &name , /* not const */ wxArrayString &file ) ; ///< List of all known databases
    void setOption ( const wxString& oname , const int value ) ; ///< Set option in local database
    void setOption ( const wxString& oname , const wxString& vname ) ; ///< Set option in local database
    int getOption ( const wxString& oname , const int def ) ; ///< Get option from local database
    wxString getOption ( const wxString& oname , const wxString& def ) ; ///< Get option from local database
    bool copySQLfields ( TStorage &target , const wxString& table , const wxString& cond ) ; ///< Internal use for update
    void synchronize () ; ///< Synchronize information between databases (not used)
    void startup () ; ///< Startup method

    void autoUpdateSchema () ; ///< Update database to a new schema
    wxString fixDNAname ( const wxString& s ) const ; ///< Fix name quotes
    wxString UCfirst ( const wxString& s ) const ; ///< Uppercase first letter in string
    wxString getDBname () const ; ///< Return the database name
    bool getWriteProtect () const ; ///< Is this database write protected?
    static wxString createMySQLdb ( const wxString& ip , const wxString& db , const wxString& name , const wxString& pwd ) ;  ///< Create a MySQL database
    void optimizeDatabase () ; ///< Optimize sqlite database
    wxString getDefaultDB () ; ///< Returns the name of the default (standard) database
    void addEnzymeToGroup ( const wxString& enzyme , const wxString& group ) ; ///< Add a restriction enzyme to an enzyme group
    void removeEnzymeFromGroup ( const wxString& enzyme , const wxString& group ) ; ///< Remove an enzyme from an enzyme group
    void removeEnzymeGroup ( const wxString& group ) ; ///< Remove an enzyme group
    void syncEnzymes ( TStorage *to = NULL ) ; ///< Get new enzymes from (basic) database
    void startRecord () ; ///< Start recording queries
    void endRecord () ; ///< Stop recording, execute all recorded queries

    // Variables
    wxArrayTRestrictionEnzyme re ; ///< List of restriction enzymes in this database
    wxArrayTProtease pr ; ///< List of proteases in this database
    TSQLresult results ; ///< The results of the last query

    private :
    wxString getSingleField ( const wxString& query , const wxString& field , const wxString& def = "" ) ; ///< Get a single field from a query, with default value
    int getSingleField ( const wxString& query , const wxString& field , int def = 0 ) ; ///< Get a single field from a query, with default value
    wxString makeInsert ( const wxString& table , const wxArrayString &field , const wxArrayString &data ) const ; ///< Insert all the fields with their data into a table
    void replaceTable ( const wxString& table , wxArrayString &f , wxArrayString &t ) ; ///< ???
    void tableInfoSet ( wxArrayString &f , wxArrayString &t , const wxString& nf , const wxString& nt ) ; ///< ???
    TStorage *getDBfromEnzymeGroup ( const wxString& group ) ; ///< Returns the database of the enzyme group
    wxString stripGroupName ( const wxString& s ) const ; ///< ???
    void cleanEnzymeGroupCache () ; ///< Clear the enzyme group cache
    void setEnzymeCache ( const wxString& group , wxArrayString &enzymes ) ; ///< Set cache for an enzyme group
    void getEnzymeCache ( const wxString& group , wxArrayString &enzymes ) ; ///< Get cached enzyme group data
    bool isLocalDB () const ; ///< Is this the local database?
//    bool convertSqlite2to3 () ; ///< Converts a sqlite2 database into sqlite3
    void createDatabaseSqlite3 () ; ///< Creates an sqlite3 database (NOT FUNCTIONAL)
//    TSQLresult getObjectSqlite2 ( const wxString &query ) ; ///< Get object from sqlite2 database
    TSQLresult getObjectSqlite3 ( const wxString &query ) ; ///< Get object from sqlite3 database
    TSQLresult getObject_MySQL ( const wxString &query ) ; ///< Get object from MySQL database

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

