#include "TStorage.h"

using namespace std ;

bool TStorage::getWriteProtect () { return writeProtect ; }

TStorage::TStorage ( int nt , string fn )
    {
    rpv = 0 ;
    writeProtect = false ;
    storagetype = nt ;
    if ( fn == "" ) fn = myapp()->homedir+"/local.db" ;
    dbname = fn ;
    autoUpdateSchema() ;
    }

TStorage::~TStorage ()
    {
    for ( int a = 0 ; a < re.size() ; a++ ) // Cleaning up enzymes
        delete re[a] ;
    }    
    
TStorage *st ;
static int callback (void *NotUsed, int argc, char **argv, char **azColName)
    {
    int i , nf ;
    if ( st->results.content.size() == 0 )
        {
        for(i=0; i<argc; i++)
                st->results.field.push_back ( azColName[i] ) ;
        }
        
    nf = st->results.content.size() ;
    st->results.content.push_back ( TVS() ) ;

    for(i=0; i<argc; i++)
        {
        if ( argv[i] ) st->results.content[nf].push_back( argv[i] ) ;
        else st->results.content[nf].push_back ( "" ) ;
        }
    return 0;
    }
    
void TStorage::createDatabase ()
    {
    sqlite *db ;
    char *e = 0 ;
    int rc ;
    db = sqlite_open ( dbname.c_str() , 0 , &e ) ;
    ierror = (int) e ;
    if ( e ) error = e ;
    else error = "Alles OK" ;
    sqlite_close ( db ) ;
    }

TSQLresult TStorage::getObject ( string query )
    {
    sqlite *db ;
    char *e = 0 ;
    int rc ;
    
    db = sqlite_open ( dbname.c_str() , 0 , &e ) ;
    st = this ;
    results.clean() ;
    if ( db == NULL ) return results ; // Database broken or does not exist
    
    if ( writeProtect ) // Prevent old program version breaking the DB
       {
       string q = query.substr ( 0 , 6 ) ;
       if ( q != "SELECT" )
          {
          sqlite_close ( db ) ;
          return results ;
          }
       }
    
    do {
        rc = sqlite_exec ( db , query.c_str() , callback , 0 , &e ) ;
        if ( rc == SQLITE_BUSY ) wxUsleep ( 200 ) ; // If busy, wait 200 ms
        } while ( rc == SQLITE_BUSY ) ;
    
    ierror = (int) e ;
    if ( e ) error = e ;
    else error = "Alles OK" ;

    sqlite_close ( db ) ;
    
    return results ;
    }
    
void TStorage::import ()
    {
    TSQLresult sr ;
    int a ;
    
    // Importing restriction enzymes
    sr = getObject ( "SELECT * FROM enzyme" ) ;
    for ( a = 0 ; a < sr.content.size() ; a++ )
        {
        string name = sr[a][sr["e_name"]] ;
        if ( name == "" ) {}
        else if ( name[0] == '*' )
           {
           name = name.substr ( 1 , name.length() - 1 ) ;
           pr.push_back ( new TProtease ( name , 
                                            sr[a][sr["e_sequence"]] , 
                                            sr[a][sr["e_note"]] ) ) ;
           }
        else
           {
           TRestrictionEnzyme *e = new TRestrictionEnzyme ;
           e->name = name ;
           e->dbid = atol ( sr[a][sr["e_id"]].c_str() ) ;
           e->sequence = sr[a][sr["e_sequence"]] ;
           e->note = sr[a][sr["e_note"]] ;
           e->location = sr[a][sr["e_location"]] ;
           e->cut = atoi ( sr[a][sr["e_cut"]].c_str() ) ;
           e->overlap = atoi ( sr[a][sr["e_overlap"]].c_str() ) ;
           re.push_back ( e ) ;
           }
        }
    }

TRestrictionEnzyme* TStorage::getRestrictionEnzyme ( string s )
    {
    TRestrictionEnzyme *ret = NULL , *ret2 ;
    if ( storagetype == TEMP_STORAGE ) 
        {
        ret2 = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
        if ( ret2 ) return ret2 ;
        }

    for ( int a = 0 ; !ret && a < re.size() ; a++ )
        if ( re[a]->name == s )
           ret = re[a] ;
           
    if ( storagetype == TEMP_STORAGE && ret )
        {
        ret2 = new TRestrictionEnzyme ;
        *ret2 = *ret ;
        myapp()->frame->LS->re.push_back ( ret2 ) ;
        myapp()->frame->LS->updateRestrictionEnzyme ( ret2 ) ;
        ret = ret2 ;
        }
           
    return ret ;
    }
    
void TStorage::getEnzymesInGroup ( string gn , vector <string> &vs )
    {
    TSQLresult sr ;
    int a ;
    while ( vs.size() ) vs.pop_back() ;
    string sql ;
    if ( gn != txt("All") )
        {
        sql = "SELECT leg_enzyme FROM link_enzyme_group" ;
        sql += " WHERE leg_group=\""+gn+"\"" ;
        sr = getObject ( sql ) ;
        for ( a = 0 ; a < sr.content.size() ; a++ )
           if ( sr[a][sr["leg_enzyme"]][0] != '*' )
              vs.push_back ( sr[a][sr["leg_enzyme"]] ) ;
        }
    else
        {
        sql = "SELECT e_name from enzyme" ;
        sr = getObject ( sql ) ;
        for ( a = 0 ; a < sr.content.size() ; a++ )
           if ( sr[a][sr["e_name"]][0] != '*' )
              vs.push_back ( sr[a][sr["e_name"]] ) ;
        }
    
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        if ( vs[a] == "" )
           {
           vs[a] = vs[vs.size()-1] ;
           vs.pop_back () ;
           a-- ;
           }
        }
    }
    
void TStorage::getEnzymeGroups ( vector <string> &vs )
    {
    TSQLresult sr ;
    int a ;
    while ( vs.size() ) vs.pop_back() ;
    string sql = "SELECT eg_name FROM enzyme_group" ;
    sr = getObject ( sql ) ;
    for ( a = 0 ; a < sr.content.size() ; a++ )
        vs.push_back ( sr[a][sr["eg_name"]] ) ;
    }
    
void TStorage::updateRestrictionEnzyme ( TRestrictionEnzyme *e )
    {
    TSQLresult sr ;
    string sql ;
    char u[100] ;
    
    // Remove old enzyme, if any
    sql = "DELETE FROM enzyme WHERE e_name=\""+e->name+"\"" ;
    getObject ( sql ) ;
    
    // Get new id
    sql = "SELECT max(e_id) FROM enzyme" ;
    sr = getObject ( sql ) ;
    if ( ierror ) return ;
    string eid = sr.content[0][0] ;
    int e_id = atoi ( eid.c_str() ) ;
    sprintf ( u , "%d" , e_id+1 ) ;
    e->dbid = e_id ;
    
    // Insert new enzyme
    sql = "INSERT INTO enzyme (e_id,e_name,e_sequence,e_location,e_note,e_cut,e_overlap) VALUES (\"" ;
    sql += u ;
    sql += "\",\"" ;
    sql += e->name + "\",\"" ;
    sql += e->sequence + "\",\"" ;
    sql += e->location + "\",\"" ;
    sql += e->note + "\",\"" ;
    sprintf ( u , "%d" , e->cut ) ;
    sql += u ;
    sql += "\",\"" ;
    sprintf ( u , "%d" , e->overlap ) ;
    sql += u ;
    sql += "\")" ;
    getObject ( sql ) ;
    }

void TStorage::sqlAdd ( string &s1 , string &s2 , string key , string value )
    {
    int a ;
    for ( a = 0 ; a < value.length() ; a++ ) // Avoiding single quotes in value
        if ( value[a] == '"' ) value[a] = 39 ;

//    for ( a = 0 ; a < value.length() ; a++ ) // Avoiding single quotes in value
//        if ( value[a] == 39 ) value[a] = '"' ;
    
    if ( s1 != "" ) s1 += "," ;
    if ( s2 != "" ) s2 += "," ;
    s1 += key ;
    s2 += "\"" + value + "\"" ;
    }

void TStorage::sqlAdd ( string &s1 , string &s2 , string key , char* value )
    {
    sqlAdd ( s1 , s2 , key , string ( value ) ) ;
    }
    
void TStorage::sqlAdd ( string &s1 , string &s2 , string key , wxString value )
    {
    sqlAdd ( s1 , s2 , key , string ( value.c_str() ) ) ;
    }
    
void TStorage::sqlAdd ( string &s1 , string &s2 , string key , int value )
    {
    char t[1000] ;
    sprintf ( t , "%d" , value ) ;
    if ( s1 != "" ) s1 += "," ;
    if ( s2 != "" ) s2 += "," ;
    s1 += key ;
    s2 += "\"" + string ( t ) + "\"" ;
    }
    
// This function is *only* called for the local database! Ever!!
// It writes the list of available databases in the name and file vectors,
// and returns the name of the default database
string TStorage::getDatabaseList ( vector <string> &name , vector <string> &file )
    {
    TSQLresult r ;
    string defdb = txt("local_db") ;
    while ( name.size() ) name.pop_back () ;
    while ( file.size() ) file.pop_back () ;
    name.push_back ( defdb ) ;
    file.push_back ( dbname ) ;
    string sql = "SELECT * FROM stuff WHERE s_type=\"DATABASE\"" ;
    r = getObject ( sql ) ;
    for ( int a = 0 ; a < r.content.size() ; a++ )
        {
        name.push_back ( r[a][r["s_name"]] ) ;
        file.push_back ( r[a][r["s_value"]] ) ;
        }

    sql = "SELECT * FROM stuff WHERE s_type=\"DEFAULT_DATABASE\"" ;
    r = getObject ( sql ) ;
    if ( r.content.size() == 0 ) return defdb ;
    else return r[0][r["s_name"]] ;
    }
    
string TStorage::getSingleField ( string query , string field , string def )
    {
    TSQLresult sr ;
    sr = getObject ( query ) ;
    if ( sr.content.size() > 0 ) return sr[0][sr[(char*)field.c_str()]] ;
    else return def ;
    }
    
int TStorage::getSingleField ( string query , string field , int def )
    {
    char t[100] ;
    sprintf ( t , "%d" , def ) ;
    string r = getSingleField ( query , field , t ) ;
    return atoi ( r.c_str() ) ;
    }
    
string TStorage::getOption ( string oname , string def )
    {
    return getSingleField ( "SELECT s_value FROM stuff WHERE s_name=\"" +
                            oname + 
                            "\" AND s_type=\"OPTION\"" , 
                            "s_value" , def ) ;
    }

int TStorage::getOption ( string oname , int def )
    {
    return getSingleField ( "SELECT s_value FROM stuff WHERE s_name=\"" +
                            oname + 
                            "\" AND s_type=\"OPTION\"" , 
                            "s_value" , def ) ;
    }
    
bool TStorage::addEnzymeGroup ( string s )
    {
    if ( s == "" ) return false ;
    int a ;
    for ( a = 0 ; a < s.length() ; a++ )
        if ( s[a] >= 'A' && s[a] <= 'Z' )
           s[a] = s[a] - 'A' + 'a' ;
    if ( s[0] >= 'a' && s[0] <= 'z' ) s[0] = s[0] - 'a' + 'A' ;
    if ( s == txt("all") ) return false ;

    TSQLresult sr ;
    string sql = "SELECT eg_name FROM enzyme_group WHERE eg_name=\"" + 
                 s + "\"" ;
    sr = getObject ( sql ) ;
    if ( sr.rows() > 0 ) return false ; // Already exists
    
    string s1 , s2 ;
    sqlAdd ( s1 , s2 , "eg_name" , s ) ;
    sqlAdd ( s1 , s2 , "eg_index" , 1 ) ;
    sqlAdd ( s1 , s2 , "eg_creator" , 1 ) ;
    sql = "INSERT INTO enzyme_group (" + s1 + ") VALUES (" + s2 + ")" ;    
    getObject ( sql ) ;
    return true ;
    }
    
bool TStorage::copySQLfields ( TStorage &target , string table , string cond )
    {
    int a , b ;
    TSQLresult r ;
    string sql , s1 , s2 ;
    sql = "SELECT * FROM " + table + " WHERE " + cond ;
    r = getObject ( sql ) ;
    sql = "DELETE FROM " + table + " WHERE " + cond ;
    target.getObject ( sql ) ;
    for ( a = 0 ; a < r.rows() ; a++ )
        {
        s1 = s2 = "" ;
        for ( b = 0 ; b < r.cols() ; b++ )
           target.sqlAdd ( s1 , s2 , r.field[b] , r[a][b] ) ;
        sql = "INSERT INTO " + table + " (" + s1 + ") VALUES (" + s2 + ")" ;
        target.getObject ( sql ) ;
        }
    return true ;
    }
    
    
    
    
void TStorage::replaceTable ( string table , TVS &f , TVS &t )
    {
    int a , b ;
    TSQLresult r ;
    string sql , s1 , s2 ;
    sql = "SELECT * FROM " + table ;
    r = getObject ( sql ) ;
    getObject ( "DROP TABLE " + table ) ;
    
    string create ;
    for ( a = 0 ; a < f.size() ; a++ )
        {
        if ( create != "" ) create += ",\n" ;
        create += f[a] + " " + t[a] ;
        }
    create = "CREATE TABLE " + table + "(\n" + create + ")" ;
    
    getObject ( create ) ;
    
    for ( a = 0 ; a < r.rows() ; a++ )
        {
        s1 = s2 = "" ;
        for ( b = 0 ; b < f.size() ; b++ )
           {
           int id = r[(char*)f[b].c_str()] ;
           if ( id > -1 ) sqlAdd ( s1 , s2 , f[b] , r[a][id] ) ;
           else sqlAdd ( s1 , s2 , f[b] , "" ) ;
           }
        sql = "INSERT INTO " + table + " (" + s1 + ") VALUES (" + s2 + ")" ;
        getObject ( sql ) ;
        }
    }
    
void TStorage::tableInfoSet ( TVS &f , TVS &t , string nf , string nt )
    {
    int a ;
    for ( a = 0 ; a < f.size() && f[a] != nf ; a++ ) ;
    if ( a == f.size() )
        {
        f.push_back ( nf ) ;
        t.push_back ( nt ) ;
        }
    else
        {
        t[a] = nt ;
        }
    }
    
string TStorage::fixDNAname ( string s )
    {
    int a ;
    for ( a = 0 ; a < s.length() ; a++ )
        if ( s[a] == '"' ) s[a] = 39 ;
    return s ;
    }
    
void TStorage::setOption ( string oname , int value )
    {
    char t[100] ;
    sprintf ( t , "%d" , value ) ;
    setOption ( oname , string ( t ) ) ;
    }
    
void TStorage::setOption ( string oname , string vname )
    {
    string sql ;
    sql = "DELETE FROM stuff WHERE s_type=\"OPTION\" AND s_name=\"" ;
    sql += oname ;
    sql += "\"" ;
    getObject ( sql ) ;
    sql = "INSERT INTO stuff (s_type,s_name,s_value) VALUES (" ;
    sql += "\"OPTION\",\"" + oname + "\",\"" + vname + "\")" ;
    getObject ( sql ) ;
    }
    
void TStorage::autoUpdateSchema ()
    {
    TSQLresult r ;
    string sql , s1 , s2 ;
    char t[1000] ;
    
    // Database version
    int version , oversion ;
    sql = "SELECT s_value FROM stuff WHERE s_type=\"INTERNAL\" AND s_name=\"DATABASE_VERSION\"" ;
    r = getObject ( sql ) ;
    if ( r.rows() == 0 ) version = 0 ;
    else version = atoi ( r[0][0].c_str() ) ;
    oversion = version ;

    // Required program version
    sql = "SELECT s_value FROM stuff WHERE s_type=\"INTERNAL\" AND s_name=\"REQUIRED_PROGRAM_VERSION\"" ;
    r = getObject ( sql ) ;
    if ( r.rows() == 0 ) rpv = 0 ;
    else rpv = atoi ( r[0][0].c_str() ) ;
    if ( rpv > myapp()->programVersion ) // Old program version, no write to this DB
       {
       writeProtect = true ;
       if ( !myapp()->dbWarningIssued && txt("LOADED") == string("yes") )
          {
          wxMessageBox ( txt("t_pov_warning") ) ;
          myapp()->dbWarningIssued = true ;
          }
       }
    
    TVS dnaF , dnaT ;
    
    // Version 0
    tableInfoSet ( dnaF , dnaT , "dna_name" , "tinytext" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_description" , "tinytext" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_type" , "varchar(32)" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_sequence" , "mediumtext" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_sticky_ul" , "varchar(32)" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_sticky_ur" , "varchar(32)" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_sticky_ll" , "varchar(32)" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_sticky_lr" , "varchar(32)" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_restriction_enzymes" , "mediumtext" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_circular" , "boolean" ) ;
    
    // Version 0.1
    tableInfoSet ( dnaF , dnaT , "dna_params" , "mediumtext" ) ;
    tableInfoSet ( dnaF , dnaT , "dna_type" , "int" ) ;
    if ( version < 1 ) // Enabeling different DNA types
        {
        version = 1 ;
        sql = "UPDATE dna SET dna_type=\"0\"" ;
        getObject ( sql ) ;
        replaceTable ( "dna" , dnaF , dnaT ) ;
        }
        
    // Version 0.2
    if ( version < 2 ) // Creating indices
        {
        version = 2 ;
        getObject ( "CREATE INDEX k_dna ON dna (dna_name)" ) ;
        getObject ( "CREATE INDEX k_dna_type ON dna (dna_type)" ) ;
        getObject ( "CREATE INDEX k_dna_desc ON dna (dna_description)" ) ;
        }
    
    // Writing new version, if necessary
    if ( version > oversion )
        {
        sql = "DELETE FROM stuff WHERE s_type=\"INTERNAL\" AND s_name=\"DATABASE_VERSION\"" ;
        getObject ( sql ) ;
        s1 = s2 = "" ;
        sprintf ( t , "%d" , version ) ;
        sqlAdd ( s1 , s2 , "s_type"  , "INTERNAL" ) ;
        sqlAdd ( s1 , s2 , "s_name"  , "DATABASE_VERSION" ) ;
        sqlAdd ( s1 , s2 , "s_value" , t ) ;
        sql = "INSERT INTO stuff (" + s1 + ") VALUES (" + s2 + ")" ;
        getObject ( sql ) ;
        }
    }
    
void TStorage::synchronize ()
    {
    bool changed ;
    TSQLresult r ;
    int a , b , c ;
    
    // Known database list
    vector <string> files ;
    r = getObject ( "SELECT s_value FROM stuff WHERE s_type=\"DATABASE\"" ) ;
    for ( a = 0 ; a < r.rows() ; a++ )
        files.push_back ( r[a][r["s_value"]] ) ;

    do {
        changed = false ;
        for ( a = 0 ; a < files.size() ; a++ )
           {
           TStorage t ( TEMP_STORAGE , files[a] ) ;

           TSQLresult r1 , r2 ;
           r1 = getObject ( "SELECT * FROM enzyme ORDER BY e_name" ) ;
           r2 = t.getObject ( "SELECT * FROM enzyme ORDER BY e_name" ) ;

           for ( b = c = 0 ; b < r1.rows() && c < r2.rows() ; )
              {
              if ( r1[b][r1["e_name"]] == r2[c][r2["e_name"]] )
                 {
                 b++ ;
                 c++ ;
                 }
              else if ( r1[b][r1["e_name"]] < r2[c][r2["e_name"]] ) // Export
                 {
                 t.getObject ( makeInsert ( "enzyme" , r1.field , r1[b] ) ) ;
                 b++ ;
                 }
              else if ( r1[b][r1["e_name"]] > r2[c][r2["e_name"]] ) // Import
                 {
                 changed = true ;
                 getObject ( makeInsert ( "enzyme" , r2.field , r2[c] ) ) ;                 
                 c++ ;
                 }
              }
            while ( b < r1.rows() )
               {
               t.getObject ( makeInsert ( "enzyme" , r1.field , r1[b++] ) ) ;
               }
            while ( c < r2.rows() )
               {
               getObject ( makeInsert ( "enzyme" , r2.field , r2[c++] ) ) ;
               changed = true ;
               }
           }
        } while ( changed ) ;
    }
    
string TStorage::makeInsert ( string table , TVS &field , TVS &data )
    {
    string sql , s1 , s2 ;
    for ( int a = 0 ; a < field.size() ; a++ )
        sqlAdd ( s1 , s2 , field[a] , data[a] ) ;
    sql = "INSERT INTO " + table + " (" + s1 + ") VALUES (" + s2 + ")" ;
    return sql ;
    }
TProtease *TStorage::getProtease ( string s )
    {
    int a ;
    for ( a = 0 ; a < pr.size() ; a++ )
        if ( pr[a]->name == s )
           return pr[a] ;
    return NULL ;
    }
    
void TStorage::updateProtease ( TProtease *p )
    {
    TRestrictionEnzyme e ;
    e.name = "*" + p->name ;
    e.cut = p->cut ;
    e.sequence = p->str_match ;
    e.note = p->note ;
    updateRestrictionEnzyme ( &e ) ;
    }
    
void TStorage::addRestrictionEnzyme ( TRestrictionEnzyme *r )
    {
    re.push_back ( r ) ;
    }
    
