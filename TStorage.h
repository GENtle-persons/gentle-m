#ifndef _TSTORAGE_H_
#define _TSTORAGE_H_

#include <stdio.h>
#include "main.h"
#include <mysql.h>

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

typedef vector <string> TVS ;

class TSQLresult
    {
    public:
    TVS field ;
    vector <TVS> content ;
    virtual void clean()
        {
        while ( field.size() ) field.pop_back() ;
        while ( content.size() ) content.pop_back() ;
        }
    virtual int cols () { return field.size() ; }
    virtual int rows () { return content.size() ; }
    virtual string item ( char *s , int i )
        {
        int a ;
        wxString s2 = s ;
        for ( a = 0 ; a < field.size() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a].c_str() ) )
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
        for ( a = 0 ; a < field.size() ; a++ )
           if ( 0 == s2.CmpNoCase ( field[a].c_str() ) )
               return a ;
        return -1 ;
        }
    } ;


class TStorage
    {
    public :
    TStorage ( int nt , string fn = "" ) ;
    virtual ~TStorage () ;
    void createDatabase () ;
    TSQLresult getObject ( string query ) ;
    TSQLresult getObject_MySQL ( string query ) ;
    
    // Restriction enzymes
    void import () ;
    TRestrictionEnzyme* getRestrictionEnzyme ( string s ) ;
    void getEnzymeGroups ( vector <string> &vs ) ;
    void getEnzymesInGroup ( string gn , vector <string> &vs ) ;
    void getEnzymeGroups ( vector <wxString> &vs ) ;
//    void getEnzymesInGroup ( wxString gn , vector <wxString> &vs ) ;
    void updateRestrictionEnzyme ( TRestrictionEnzyme *e ) ;
    bool addEnzymeGroup ( string s ) ;
    void sqlAdd ( string &s1 , string &s2 , string key , string value ) ;
    void sqlAdd ( string &s1 , string &s2 , string key , wxString value ) ;
    void sqlAdd ( string &s1 , string &s2 , string key , char* value ) ;
    void sqlAdd ( string &s1 , string &s2 , string key , int value ) ;
    string getDatabaseList ( vector <string> &name , vector <string> &file ) ;
    string getSingleField ( string query , string field , string def = "" ) ;
    int getSingleField ( string query , string field , int def = 0 ) ;
    void setOption ( string oname , int value ) ;
    void setOption ( string oname , string vname ) ;
    int getOption ( string oname , int def ) ;
    string getOption ( string oname , string def ) ;
    bool copySQLfields ( TStorage &target , string table , string cond ) ;
    void synchronize () ;
    string makeInsert ( string table , TVS &field , TVS &data ) ;
    TProtease *getProtease ( string s ) ;
    void updateProtease ( TProtease *p ) ;
    
    void autoUpdateSchema () ;
    void replaceTable ( string table , TVS &f , TVS &t ) ;
    void tableInfoSet ( TVS &f , TVS &t , string nf , string nt ) ;
    string fixDNAname ( string s ) ;
    string UCfirst ( string s ) ;
    
    bool getWriteProtect () ;
    void addRestrictionEnzyme ( TRestrictionEnzyme *r ) ;
    
    // Variables
    vector <TRestrictionEnzyme*> re ;
    vector <TProtease*> pr ;
    
    TSQLresult results ;
    
    private :
    string dbname , error ;
    int storagetype , ierror ;
    bool writeProtect ;
    int rpv ;
    bool isMySQL ;
    MYSQL *conn,*mysql;
    } ;
 
#endif

