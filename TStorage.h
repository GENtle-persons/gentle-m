#ifndef _TSTORAGE_H_
#define _TSTORAGE_H_

#include <stdio.h>
#include "main.h"

#ifdef __WXMSW__ // Linux
#define USEMYSQL
#endif

#ifdef USEMYSQL
#include <mysql.h>
#endif

#ifdef __WXMSW__
#include "win_sqlite.h"
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

typedef wxArrayString TVS ;

class TSQLresult
    {
    public:
    TVS field ;
    vector <TVS> content ;
    virtual void clean()
        {
        field.Clear() ;
        while ( content.size() ) content.pop_back() ;
        }
    virtual int cols () { return field.GetCount() ; }
    virtual int rows () { return content.size() ; }
    virtual wxString item ( char *s , int i )
        {
        int a ;
        wxString s2 = s ;
        for ( a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return content[i][a] ;
        return "" ;
        }
    virtual TVS & operator [] ( int i )
        {
        return content[i] ;
        }
    virtual int operator [] ( char *s )
        {
        int a ;
        wxString s2 = s ;
        for ( a = 0 ; a < field.GetCount() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a] ) )
               return a ;
        return -1 ;
        }
    } ;


class TStorage
    {
    public :
    TStorage ( int nt , wxString fn = "" ) ;
    virtual ~TStorage () ;
    void createDatabase () ;
    TSQLresult getObject ( wxString query ) ;
    TSQLresult getObject_MySQL ( wxString query ) ;

    // Access
    void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , wxString value ) ;
    void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , char* value ) ;
    void sqlAdd ( wxString &s1 , wxString &s2 , wxString key , int value ) ;
    
    // Restriction enzymes
    void import () ;
    TRestrictionEnzyme* getRestrictionEnzyme ( wxString s ) ;
    void getEnzymeGroups ( wxArrayString &vs ) ;
//    void getEnzymeGroups ( vector <string> &vs ) ;
    void getEnzymesInGroup ( wxString gn , wxArrayString &vs ) ;
//    void getEnzymesInGroup ( wxString gn , vector <string> &vs ) ;
    void updateRestrictionEnzyme ( TRestrictionEnzyme *e ) ;
    void addRestrictionEnzyme ( TRestrictionEnzyme *r ) ;
    bool addEnzymeGroup ( wxString s ) ;

    TProtease *getProtease ( wxString s ) ;
    void updateProtease ( TProtease *p ) ;
    
    wxString getDatabaseList ( wxArrayString &name , wxArrayString &file ) ;
    void setOption ( wxString oname , int value ) ;
    void setOption ( wxString oname , wxString vname ) ;
    int getOption ( wxString oname , int def ) ;
    wxString getOption ( wxString oname , wxString def ) ;
    bool copySQLfields ( TStorage &target , wxString table , wxString cond ) ;
    void synchronize () ;

    void autoUpdateSchema () ;
    wxString fixDNAname ( wxString s ) ;
    wxString UCfirst ( wxString s ) ;
    wxString getDBname () ;    
    bool getWriteProtect () ;
    
    // Variables
    vector <TRestrictionEnzyme*> re ;
    vector <TProtease*> pr ;
    TSQLresult results ;
    
    private :
    wxString getSingleField ( wxString query , wxString field , wxString def = "" ) ;
    int getSingleField ( wxString query , wxString field , int def = 0 ) ;
    wxString makeInsert ( wxString table , TVS &field , TVS &data ) ;
    void replaceTable ( wxString table , TVS &f , TVS &t ) ;
    void tableInfoSet ( TVS &f , TVS &t , wxString nf , wxString nt ) ;
    
    // Variables
    wxString dbname , error ;
    int storagetype , ierror ;
    bool writeProtect ;
    int rpv ;
    bool isMySQL ;
#ifdef USEMYSQL
    MYSQL *conn,*mysql;
#endif
    } ;
 
#endif

