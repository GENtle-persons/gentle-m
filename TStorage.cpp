#include "TStorage.h"
#include <wx/textfile.h>
#include <wx/filename.h>

using namespace std ;

bool TStorage::getWriteProtect () { return writeProtect ; }

TStorage::TStorage ( int nt , wxString fn )
    {
    if ( fn.IsEmpty() ) isMySQL = false ;
    else isMySQL = (fn.GetChar(0)==':') ;
    rpv = 0 ;
    recording = false ;
    writeProtect = false ;
    storagetype = nt ;
    if ( fn.IsEmpty() ) fn = myapp()->homedir+"/local.db" ;
    dbname = fn ;

    isSqlite3 = false ;
    if ( !isMySQL && fn != "" )
    	{
    	wxFile f ( fn , wxFile::read ) ;
    	char xx[100] ;
    	f.Read ( xx , 15 ) ;
    	xx[15] = 0 ;
    	if ( wxString ( xx ) == "SQLite format 3" )
    		isSqlite3 = true ;
    	}    

#ifdef USEMYSQL
    if ( isMySQL )
        {
        conn = new MYSQL ;
        mysql_init (conn);
        wxArrayString ex ;
        explode ( ":" , fn+":" , ex ) ;
        if ( !mysql_real_connect ( conn , 
                                ex[1].c_str() , 
                                ex[2].c_str() ,
                                ex[3].c_str() , 
                                ex[4].c_str() , 0 , NULL , CLIENT_COMPRESS ) )
           {
           wxSafeShowMessage ( wxString::Format ( txt("t_mysql_error") , dbname.c_str() ) , mysql_error ( conn ) ) ;
           conn = NULL ;
           }
        }
#endif
    autoUpdateSchema() ;
    }

TStorage::~TStorage ()
    {
#ifdef USEMYSQL
    if ( isMySQL && conn )
        {
        mysql_close (conn);
        delete conn ;
        }
#endif
    for ( int a = 0 ; a < re.GetCount() ; a++ ) // Cleaning up enzymes
        delete re[a] ;
    }    
    
TStorage *st ;
static int callback (void *NotUsed, int argc, char **argv, char **azColName)
    {
    int i , nf ;
    if ( st->results.content.size() == 0 )
        {
        for(i=0; i<argc; i++)
           st->results.field.Add ( azColName[i] ) ;
        }
        
    nf = st->results.content.size() ;
    st->results.content.push_back ( TVS() ) ;

    for(i=0; i<argc; i++)
        {
        if ( argv[i] ) st->results.content[nf].Add( argv[i] ) ;
        else st->results.content[nf].Add ( "" ) ;
        }
    return 0;
    }
    
void TStorage::createDatabaseSqlite3 ()
    {
        return;
    sqlite3 *db ;
    char *e = 0 ;
    int rc ;
    sqlite3_open ( dbname.c_str() , &db ) ;
//    ierror = (int) e ;
//    if ( e ) error = e ;
//    else error = "Alles OK" ;
    sqlite3_close ( db ) ;
    }
        
void TStorage::createDatabase ()
    {
    if ( isMySQL ) return ;
    if ( isSqlite3 ) { createDatabaseSqlite3() ; return ; }
    sqlite *db ;
    char *e = 0 ;
    int rc ;
    db = sqlite_open ( dbname.c_str() , 0 , &e ) ;
    ierror = (int) e ;
    if ( e ) error = e ;
    else error = "Alles OK" ;
    sqlite_close ( db ) ;
    }

TSQLresult TStorage::getObject_MySQL ( const wxString &query )
    {
    results.clean() ;
#ifdef USEMYSQL
    if ( conn == NULL ) return results ;

    if ( writeProtect ) // Prevent old program version breaking the DB
       {
       wxString q = query.substr ( 0 , 6 ) ;
       if ( q != "SELECT" )
          {
          mysql_close ( conn ) ;
          return results ;
          }
       }

    int err = mysql_query ( conn , query.c_str() ) ;
    if ( err == 0 )
        {
        MYSQL_RES *result ;
        result = mysql_store_result ( conn ) ;
        if ( result != NULL ) 
            {
            MYSQL_ROW row;
            unsigned int i , num_fields = mysql_num_fields(result) ;
            while ((row = mysql_fetch_row(result)))
            {
               unsigned long *lengths;
               lengths = mysql_fetch_lengths(result);
               int rownum = results.content.size() ;
               results.content.push_back ( TVS() ) ;
               for(i = 0; i < num_fields; i++)
               {
                  results.content[rownum].Add ( row[i] ? row[i] : "" ) ;
               }
            }        
            
            MYSQL_FIELD *fields;            
            fields = mysql_fetch_fields(result);
            for(i = 0; i < num_fields; i++)
            {
               results.field.Add ( fields[i].name ) ;
            }
            
            mysql_free_result ( result ) ;
            }
        }
    else
    	{
	    err = mysql_errno(conn) ;
	    if ( err == 1153 )
	    	{
 	    	wxMessageBox ( txt("t_mysql_error_1153" ) ) ;
	    	}    
        else
        	{
             wxMessageBox ( mysql_error(conn) , wxString::Format ( "MySQL error %d" , err ) ) ;
             wxFile of ( "mysqlwrr.txt" , wxFile::write ) ;
             of.Write ( query ) ;
         }    
        }    
#endif
    return results ;
    }
    
TSQLresult TStorage::getObjectSqlite3 ( const wxString &query )
    {
    sqlite3 *db ;
    char *e = 0 ;
    int rc ;
    
    sqlite3_open ( dbname.c_str() , &db ) ;
    st = this ;
    results.clean() ;
    if ( db == NULL ) return results ; // Database broken or does not exist
    
    if ( writeProtect ) // Prevent old program version breaking the DB
       {
       wxString q = query.substr ( 0 , 6 ) ;
       if ( q != "SELECT" )
          {
          sqlite3_close ( db ) ;
          return results ;
          }
       }
    do {
        rc = sqlite3_exec ( db , query.c_str() , callback , 0 , &e ) ;
        if ( rc == SQLITE_BUSY ) // If busy, wait 200 ms
        	{
#ifdef __WXMSW__
			wxUsleep ( 200 ) ;
#else
       	    wxMilliSleep ( 200 ) ;
#endif
        	}    
        } while ( rc == SQLITE_BUSY ) ;
    
    ierror = (int) e ;
    if ( e ) error = e ;
    else error = "Alles OK" ;

    sqlite3_close ( db ) ;
    
    return results ;
    }
        
TSQLresult TStorage::getObject ( const wxString &query )
    {
    if ( recording )
    	{
	    record += query + "; " ;
        results.clean() ;
	    return results ;
    	}    
    if ( isMySQL ) return getObject_MySQL ( query ) ;
    if ( isSqlite3 ) return getObjectSqlite3 ( query ) ;
   	return getObjectSqlite2 ( query ) ;
   	}   	

TSQLresult TStorage::getObjectSqlite2 ( const wxString &query )
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
       wxString q = query.substr ( 0 , 6 ) ;
       if ( q != "SELECT" )
          {
          sqlite_close ( db ) ;
          return results ;
          }
       }
       
    if ( query.length() > 1000000 ) // Approx. 1MB, too large for sqlite 2.X
    	{
	    if ( wxYES != wxMessageBox ( txt("t_large_warning_message") , txt("t_large_warning_caption") , wxYES_NO ) )
	    	return results ; // No conversion wanted, abort
    	convertSqlite2to3 () ; // Convert to 3.X
    	return getObject ( query ) ;
    	}    
    
    do {
        rc = sqlite_exec ( db , query.c_str() , callback , 0 , &e ) ;
        if ( rc == SQLITE_BUSY ) // If busy, wait 200 ms
        	{
#ifdef __WXMSW__
			wxUsleep ( 200 ) ;
#else
       	    wxMilliSleep ( 200 ) ;
#endif
        	}    
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
        wxString name = sr[a][sr["e_name"]] ;
        if ( name.IsEmpty() ) {}
        else if ( name.GetChar(0) == '*' )
           {
           name = name.substr ( 1 , name.length() - 1 ) ;
           pr.Add ( new TProtease ( name , 
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
           re.Add ( e ) ;
           }
        }
    }

void TStorage::sqlAdd ( wxString &s1 , wxString &s2 , wxString key , wxString value )
    {
    int a ;
    for ( a = 0 ; a < value.length() ; a++ ) // Avoiding single quotes in value
        if ( value.GetChar(a) == '"' ) value.SetChar(a,39) ;
    if ( !s1.IsEmpty() ) s1 += "," ;
    if ( !s2.IsEmpty() ) s2 += "," ;
    s1 += key ;
    s2 += "\"" + value + "\"" ;
    }

void TStorage::sqlAdd ( wxString &s1 , wxString &s2 , wxString key , char* value )
    {
    sqlAdd ( s1 , s2 , key , wxString ( value ) ) ;
    }

void TStorage::sqlAdd ( wxString &s1 , wxString &s2 , wxString key , int value )
    {
    wxString t = wxString::Format ( "%d" , (unsigned int) value ) ;
//    char t[1000] ;
//    sprintf ( t , "%d" , value ) ;
    if ( !s1.IsEmpty() ) s1 += "," ;
    if ( !s2.IsEmpty() ) s2 += "," ;
    s1 += key ;
    s2 += "\"" + t + "\"" ;
    }
    
// This function is *only* called for the local database! Ever!!
// It writes the list of available databases in the name and file vectors,
// and returns the name of the default database
wxString TStorage::getDatabaseList ( wxArrayString &name , wxArrayString &file )
    {
    TSQLresult r ;
    name.Clear() ;
    file.Clear() ;
    name.Add ( txt("local_db") ) ;
    file.Add ( dbname ) ;
    wxString sql = "SELECT * FROM stuff WHERE s_type=\"DATABASE\"" ;
    r = getObject ( sql ) ;
    for ( int a = 0 ; a < r.content.size() ; a++ )
        {
        name.Add ( r[a][r["s_name"]] ) ;
        file.Add ( r[a][r["s_value"]] ) ;
        }
    return getDefaultDB () ;
    }    

wxString TStorage::getDefaultDB ()
	{
    TSQLresult r ;
    wxString defdb = txt("local_db") ;
    wxString sql = "SELECT * FROM stuff WHERE s_type=\"DEFAULT_DATABASE\"" ;
    r = getObject ( sql ) ;
    if ( r.content.size() == 0 ) return defdb ;
    else return r[0][r["s_name"]] ;
    }
    
wxString TStorage::getSingleField ( wxString query , wxString field , wxString def )
    {
    TSQLresult sr ;
    sr = getObject ( query ) ;
    if ( sr.content.size() > 0 ) return sr[0][sr[(char*)field.c_str()]] ;
    else return def ;
    }
    
int TStorage::getSingleField ( wxString query , wxString field , int def )
    {
    char t[100] ;
    sprintf ( t , "%d" , def ) ;
    wxString r = getSingleField ( query , field , t ) ;
    return atoi ( r.c_str() ) ;
    }
    
wxString TStorage::getOption ( wxString oname , wxString def )
    {
    return getSingleField ( "SELECT s_value FROM stuff WHERE s_name=\"" +
                            oname + 
                            "\" AND s_type=\"OPTION\"" , 
                            "s_value" , def ) ;
    }

int TStorage::getOption ( wxString oname , int def )
    {
    return getSingleField ( "SELECT s_value FROM stuff WHERE s_name=\"" +
                            oname + 
                            "\" AND s_type=\"OPTION\"" , 
                            "s_value" , def ) ;
    }
    
wxString TStorage::UCfirst ( wxString s )
    {
    int a ;
    for ( a = 0 ; a < s.length() ; a++ )
        if ( s.GetChar(a) >= 'A' && s.GetChar(a) <= 'Z' )
           s.SetChar ( a , s.GetChar(a) - 'A' + 'a' ) ;
    if ( !s.IsEmpty() && s.GetChar(0) >= 'a' && s.GetChar(0) <= 'z' ) s.SetChar ( 0 , s.GetChar(0) - 'a' + 'A' ) ;
    for ( a = 0 ; a < s.length() ; a++ )
	if ( s.GetChar(a) < 10 ) s.SetChar ( a , '_' ) ; // !!!!!!!!!!!!!
    return s ;
    }
    

bool TStorage::copySQLfields ( TStorage &target , wxString table , wxString cond )
    {
    int a , b ;
    TSQLresult r ;
    wxString sql , s1 , s2 ;
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
    
    
    
    
void TStorage::replaceTable ( wxString table , TVS &f , TVS &t )
    {
    int a , b ;
    TSQLresult r ;
    wxString sql , s1 , s2 ;
    sql = "SELECT * FROM " + table ;
    r = getObject ( sql ) ;
    getObject ( "DROP TABLE " + table ) ;
    
    wxString create ;
    for ( a = 0 ; a < f.GetCount() ; a++ )
        {
        if ( !create.IsEmpty() ) create += ",\n" ;
        create += f[a] + " " + t[a] ;
        }
    create = "CREATE TABLE " + table + "(\n" + create + ")" ;
    
    getObject ( create ) ;
    
    for ( a = 0 ; a < r.rows() ; a++ )
        {
        s1 = s2 = "" ;
        for ( b = 0 ; b < f.GetCount() ; b++ )
           {
           int id = r[(char*)f[b].c_str()] ;
           if ( id > -1 ) sqlAdd ( s1 , s2 , f[b] , r[a][id] ) ;
           else sqlAdd ( s1 , s2 , f[b] , "" ) ;
           }
        sql = "INSERT INTO " + table + " (" + s1 + ") VALUES (" + s2 + ")" ;
        getObject ( sql ) ;
        }
    }
    
void TStorage::tableInfoSet ( TVS &f , TVS &t , wxString nf , wxString nt )
    {
    int a ;
    for ( a = 0 ; a < f.GetCount() && f[a] != nf ; a++ ) ;
    if ( a == f.GetCount() )
        {
        f.Add ( nf ) ;
        t.Add ( nt ) ;
        }
    else
        {
        t[a] = nt ;
        }
    }
    
wxString TStorage::fixDNAname ( wxString s )
    {
    int a ;
    for ( a = 0 ; a < s.length() ; a++ )
        if ( s.GetChar(a) == '"' ) s.SetChar ( a , 39 ) ;
    return s ;
    }
    
void TStorage::setOption ( wxString oname , int value )
    {
    setOption ( oname , wxString::Format ( "%d" , value ) ) ;
    }
    
void TStorage::setOption ( wxString oname , wxString vname )
    {
    wxString sql ;
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
    if ( isMySQL ) return ; // No updates yet
    if ( dbname == "blank.db" ) return ; // No update of blank db    
    TSQLresult r ;
    wxString sql , s1 , s2 ;
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
       if ( !myapp()->dbWarningIssued && txt("LOADED") == wxString("yes") )
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
        
    // Version 0.3
    if ( version < 3 )
        {
        version = 3 ;
        r = getObject ( "SELECT eg_name FROM enzyme_group" ) ;
        getObject ( "DROP TABLE enzyme_group" ) ;
        getObject ( "CREATE TABLE enzyme_group ( eg_name tinytext )" ) ;
        for ( int a = 0 ; a < r.rows() ; a++ )
           {
           sql = "INSERT INTO enzyme_group ( eg_name ) VALUES ( \"" ;
           sql += r[a][0] ;
           sql += "\")" ;
           getObject ( sql ) ;
           }
        }

    // Version 0.4
    if ( version < 4 )
        {
        wxBeginBusyCursor() ;
        version = 4 ;
        TStorage *bl = myapp()->frame->getTempDB ( "blank.db" ) ;
        r = bl->getObject ( "SELECT * FROM enzyme" ) ;
        for ( int a = 0 ; a < r.rows() ; a++ )
           {
           wxString name = r[a][r["e_name"]] ;
           wxString seq = r[a][r["e_sequence"]] ;
           wxString cut = r[a][r["e_cut"]] ;
           wxString ol = r[a][r["e_overlap"]] ;
           sql = "UPDATE enzyme SET e_sequence='" + seq + 
                      "', e_cut='" + cut +
                      "', e_overlap='" + ol +
                      "' WHERE e_name='" + name + "'" ;
           getObject ( sql ) ;
           }    
        wxEndBusyCursor() ;
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
    
// This is only called for the local database
// It synchronizes the enzyme lists between known databases
void TStorage::synchronize ()
    {
    bool changed ;
    TSQLresult r ;
    int a , b , c ;
    
    // Sync only once a day
    wxDateTime d = wxDateTime::Now () ;
    wxString ds = wxString::Format ( "%4d-%3d" , d.GetYear() , d.GetDayOfYear() ) ;
    if ( getOption ( "LASTSYNC" , "" ) == ds ) return ;
    setOption ( "LASTSYNC" , ds ) ;
    
    // Known database list
    wxArrayString files ;
    r = getObject ( "SELECT s_value FROM stuff WHERE s_type=\"DATABASE\"" ) ;
    for ( a = 0 ; a < r.rows() ; a++ )
        files.Add ( r[a][r["s_value"]] ) ;

    do {
        changed = false ;
        for ( a = 0 ; a < files.GetCount() ; a++ )
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
    
wxString TStorage::makeInsert ( wxString table , TVS &field , TVS &data )
    {
    wxString sql , s1 , s2 ;
    for ( int a = 0 ; a < field.GetCount() ; a++ )
        sqlAdd ( s1 , s2 , field[a] , data[a] ) ;
    sql = "INSERT INTO " + table + " (" + s1 + ") VALUES (" + s2 + ")" ;
    return sql ;
    }
TProtease *TStorage::getProtease ( wxString s )
    {
    int a ;
    for ( a = 0 ; a < pr.GetCount() ; a++ )
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
    

wxString TStorage::getDBname ()
    {
    return dbname ;
    }
    
wxString TStorage::createMySQLdb ( wxString ip , wxString db , wxString name , wxString pwd )
    {
#ifdef USEMYSQL
    MYSQL c ;
    mysql_init (&c);
    mysql_real_connect ( &c , ip.c_str() , 
                                    name.c_str() , pwd.c_str() ,
                                    "mysql" ,
                                    0 , 
                                    NULL , CLIENT_COMPRESS ) ;
    
    wxString query = "CREATE DATABASE " + db ;
    int err = mysql_query ( &c , query.c_str() ) ;
    
    if ( err != 0 )
       {
       mysql_close ( &c ) ;
       return "" ;
       }
       
    mysql_real_connect ( &c , ip.c_str() , 
                                    name.c_str() , pwd.c_str() ,
                                    db.c_str() ,
                                    0 , 
                                    NULL , CLIENT_COMPRESS ) ;

    wxString t , fn = myapp()->homedir + myapp()->slash + "MySQL template.txt" ;
    wxTextFile tf ( fn ) ;
    tf.Open () ;
    for ( int lc = 0 ; lc < tf.GetLineCount() ; lc++ )
        {
        wxString s = tf[lc].Trim().Trim(true) ;
        if ( s != "" && !s.StartsWith ( "#" ) )
           {
           t += s + " " ;
           if ( s.Last() == ';' )
              {
              t.Truncate ( t.length() - 2 ) ;
              err = mysql_query ( &c , t.c_str() ) ;
              if ( err != 0 ) wxMessageBox ( t , wxString::Format ( "Error %d" , err ) ) ;
              t = "" ;
              }    
           }    
        }    
    
    mysql_close ( &c ) ;
    return ":" + ip + ":" + name + ":" + pwd + ":" + db ;
#endif  
    return "" ;  
    }    

void TStorage::optimizeDatabase ()
	{
    if ( isMySQL ) return ;
	getObject ( "VACUUM;" ) ;
	}

void TStorage::startRecord ()
	{
 	if ( isMySQL ) return ;
	record = "" ;
	recording = true ;
	}    

void TStorage::endRecord ()
	{
 	if ( isMySQL ) return ;
	recording = false ;
	record = "BEGIN; " + record + " END;" ;
	getObject ( record ) ;
	record = "" ;
	}

// Enzyme functions

bool TStorage::addEnzymeGroup ( wxString s )
    {
    if ( s.IsEmpty() ) return false ;
    s = UCfirst ( s ) ;
    if ( s == txt("all") ) return false ;

	TStorage *t = getDBfromEnzymeGroup ( s ) ;
	if ( t )
		{
		bool b = t->addEnzymeGroup ( stripGroupName ( s ) ) ;
		return b ;
		}    


    TSQLresult sr ;
    cleanEnzymeGroupCache () ;
    wxString sql = "SELECT eg_name FROM enzyme_group WHERE eg_name=\"" + s + "\"" ;
    sr = getObject ( sql ) ;
    if ( sr.rows() > 0 ) return false ; // Already exists
    
    wxString s1 , s2 ;
    sqlAdd ( s1 , s2 , "eg_name" , s ) ;
    sql = "INSERT INTO enzyme_group (" + s1 + ") VALUES (" + s2 + ")" ;    
    getObject ( sql ) ;
    return true ;
    }
    
void TStorage::addRestrictionEnzyme ( TRestrictionEnzyme *r )
    {
    cleanEnzymeGroupCache() ;
    re.Add ( r ) ;
    }
    
TRestrictionEnzyme* TStorage::getRestrictionEnzyme ( wxString s )
    {
    TRestrictionEnzyme *ret = NULL , *ret2 ;
    if ( storagetype == TEMP_STORAGE ) 
        {
        ret2 = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
        if ( ret2 ) return ret2 ;
        }

    for ( int a = 0 ; !ret && a < re.GetCount() ; a++ )
        if ( re[a]->name == s )
           ret = re[a] ;
           
    if ( storagetype == TEMP_STORAGE && ret )
        {
        ret2 = new TRestrictionEnzyme ;
        *ret2 = *ret ;
        myapp()->frame->LS->re.Add ( ret2 ) ;
        myapp()->frame->LS->updateRestrictionEnzyme ( ret2 ) ;
        ret = ret2 ;
        }
           
    return ret ;
    }

void TStorage::getEnzymesInGroup ( wxString gn , wxArrayString &vs )
    {
    int a ;
	TStorage *t = getDBfromEnzymeGroup ( gn ) ;
	if ( t )
		{
		t->getEnzymesInGroup ( stripGroupName ( gn ) , vs ) ;
		return ;
		}
		
	if ( !isLocalDB() ) // Use cache
		{
		getEnzymeCache ( gn , vs ) ;
		if ( vs.GetCount() ) return ;
		}    
		
    TSQLresult sr ;
    vs.Clear() ;
    wxString sql ;
    gn = UCfirst ( gn ) ;
    if ( gn != txt("All") )
        {
        sql = "SELECT leg_enzyme FROM link_enzyme_group" ;
        sql += " WHERE leg_group=\""+gn+"\"" ;
        sr = getObject ( sql ) ;
        for ( a = 0 ; a < sr.content.size() ; a++ )
           if ( sr[a][sr["leg_enzyme"]].GetChar(0) != '*' )
              vs.Add ( sr[a][sr["leg_enzyme"]] ) ;
        }
    else
        {
        sql = "SELECT e_name from enzyme" ;
        sr = getObject ( sql ) ;
        for ( a = 0 ; a < sr.content.size() ; a++ )
           if ( sr[a][sr["e_name"]].GetChar(0) != '*' )
              vs.Add ( sr[a][sr["e_name"]] ) ;
        }
    
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( vs[a].IsEmpty() )
           {
           vs.Remove ( a ) ;
           a-- ;
           }
        }
        
    if ( !isLocalDB() ) setEnzymeCache ( gn , vs ) ;
    }

void TStorage::getEnzymeGroups ( wxArrayString &vs )
    {
    wxString defdb = getDefaultDB() ;
    TStorage *t = getDBfromEnzymeGroup ( defdb + ":dummy" ) ;
    if ( t && isLocalDB() && t != this )
    	{
	    t->getEnzymeGroups ( vs ) ;
	    for ( int a = 0 ; a < vs.GetCount() ; a++ )
	    	vs[a] = defdb + ":" + vs[a] ;
     	}   	
    else vs.Clear() ;

    int a ;
   	if ( !isLocalDB() && enzymeGroupNameCache.GetCount() ) // Use cache
   		{
	    for ( a = 0 ; a < enzymeGroupNameCache.GetCount() ; a++ )
	    	vs.Add ( enzymeGroupNameCache[a] ) ;
    	return ;
   		}    

    TSQLresult sr ;
    if ( !isLocalDB() ) cleanEnzymeGroupCache () ;
    wxString sql = "SELECT eg_name FROM enzyme_group" ;
    sr = getObject ( sql ) ;
    for ( a = 0 ; a < sr.content.size() ; a++ )
    	{
	    wxString groupname = UCfirst ( sr[a][sr["eg_name"]] ) ;
        vs.Add ( groupname ) ;
        if ( !isLocalDB() )
        	{
        	enzymeGroupNameCache.Add ( groupname ) ; // Add to cache
        	enzymeGroupCache.Add ( "" ) ; // Add blank dummy to cache
        	}   	
        }    
    }
    
void TStorage::updateRestrictionEnzyme ( TRestrictionEnzyme *e )
    {
    TSQLresult sr ;
    wxString sql ;
    char u[100] ;
    if ( e->name.IsEmpty() ) return ;
    
    cleanEnzymeGroupCache() ;
    // Remove old enzyme, if any
    sql = "DELETE FROM enzyme WHERE e_name=\""+e->name+"\"" ;
    getObject ( sql ) ;
    
    // Get new id
    sql = "SELECT max(e_id) FROM enzyme" ;
    sr = getObject ( sql ) ;
    if ( ierror ) return ;
    int e_id = atoi ( sr.content[0][0].c_str() ) ;
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
    
void TStorage::cleanEnzymeGroupCache ()
	{
	enzymeGroupNameCache.Clear () ;
	enzymeGroupCache.Clear () ;
	}    
    
void TStorage::addEnzymeToGroup ( wxString enzyme , wxString group )
	{
	TStorage *t = getDBfromEnzymeGroup ( group ) ;
	if ( t )
		{
		t->cleanEnzymeGroupCache () ;
		t->addEnzymeToGroup ( enzyme , stripGroupName ( group ) ) ;
		return ;
		}
    wxString sql ;
    
    cleanEnzymeGroupCache() ;
    sql = "DELETE FROM link_enzyme_group WHERE leg_enzyme=\"" +
          enzyme + "\" AND leg_group=\"" + group + "\"" ;
    getObject ( sql ) ;
    
    sql = "INSERT INTO link_enzyme_group (leg_enzyme,leg_group) "
          "VALUES (\"" + enzyme + "\",\"" + group + "\")" ;
    getObject ( sql ) ;              
	}
     
TStorage *TStorage::getDBfromEnzymeGroup ( wxString group )
	{
	wxString s = group.BeforeLast ( ':' ) ;
	if ( s.IsEmpty() ) return NULL ;
	
	wxArrayString db_name , db_file ;
    myapp()->frame->LS->getDatabaseList ( db_name , db_file ) ;
    for ( int a = 0 ; a < db_name.GetCount() ; a++ )
    	{
	    if ( db_name[a] == s )
	    	return myapp()->frame->getTempDB ( db_file[a] ) ;
    	}    
	return NULL ;
	}    
	
wxString TStorage::stripGroupName ( wxString s )
	{
	return UCfirst ( s.AfterLast ( ':' ) ) ;
	}    

void TStorage::removeEnzymeFromGroup ( wxString enzyme , wxString group )
	{
	TStorage *t = getDBfromEnzymeGroup ( group ) ;
	if ( t )
		{
		t->removeEnzymeFromGroup ( enzyme , stripGroupName ( group ) ) ;
		return ;
		}    

    cleanEnzymeGroupCache() ;
    wxString sql ;
    sql = "DELETE FROM link_enzyme_group WHERE "
          "leg_enzyme=\"" +
          enzyme +
          "\" AND leg_group=\"" +
          group +
          "\"" ;
    getObject ( sql ) ;              
	}    

void TStorage::removeEnzymeGroup ( wxString group )
	{
	TStorage *t = getDBfromEnzymeGroup ( group ) ;
	if ( t )
		{
		t->removeEnzymeGroup ( stripGroupName ( group ) ) ;
		return ;
		}    

    cleanEnzymeGroupCache() ;
    wxString sql ;
    sql = "DELETE FROM link_enzyme_group WHERE leg_group=\"" + group + "\"" ;
    getObject ( sql ) ; 
    sql = "DELETE FROM enzyme_group WHERE eg_name=\"" + group + "\"" ;
    getObject ( sql ) ; 
	}    

bool TStorage::isLocalDB ()
	{
	return dbname == myapp()->frame->LS->dbname ;
	}
    
void TStorage::setEnzymeCache ( wxString group , wxArrayString &enzymes )
	{
	if ( group == txt("All") ) return ;
	int a ;
	for ( a = 0 ; a < enzymeGroupNameCache.GetCount() && group != enzymeGroupNameCache[a] ; a++ ) ;
	if ( a == enzymeGroupNameCache.GetCount() )
		{
		enzymeGroupNameCache.Add ( group ) ;
		enzymeGroupCache.Add ( "" ) ;
		}    
	enzymeGroupCache[a] = implode ( "," , enzymes ) ;
	}

void TStorage::getEnzymeCache ( wxString group , wxArrayString &enzymes )
	{
	enzymes.Clear () ;
	if ( group == txt("All") ) return ;

	int a ;
	for ( a = 0 ; a < enzymeGroupNameCache.GetCount() && group != enzymeGroupNameCache[a] ; a++ ) ;
	if ( a == enzymeGroupNameCache.GetCount() ) return ;

	explode ( "," , enzymeGroupCache[a] , enzymes ) ;
	}

// This function can convert a sqlite 2.X database into an sqlite 3.X one
bool TStorage::convertSqlite2to3 ()
	{
    wxBeginBusyCursor() ;
	wxString filename = wxFileName::CreateTempFileName ( "GENtle_" ) ;
	wxCopyFile ( "blank.db" , filename ) ;
	TStorage s3 ( TEMP_STORAGE , filename ) ;
	
	wxString sql = "SELECT name FROM sqlite_master WHERE type='table'" ;
    TSQLresult r ;
    r = getObjectSqlite2 ( sql ) ;
    
    TVS tables ;
    int a , b , c ;
    for ( a = 0 ; a < r.content.size() ; a++ )
    	tables.Add ( r[a][r["name"]] ) ;
    	
   	for ( a = 0 ; a < tables.GetCount() ; a++ )
   		{
	    sql = "SELECT * FROM " + tables[a] ;
	    r = getObjectSqlite2 ( sql ) ;
	    s3.startRecord() ;
	    s3.getObject ( "DELETE FROM " + tables[a] ) ;
	    for ( b = 0 ; b < r.content.size() ; b++ )
	    	{
 	    	wxString s1 , s2 ;
 	    	for ( c = 0 ; c < r.field.GetCount() ; c++ )
  		    	s3.sqlAdd ( s1 , s2 , r.field[c] , r[b][c] ) ;
			sql = "INSERT INTO " + tables[a] + " (" + s1 + ") VALUES (" + s2 + ")" ;
            s3.getObject ( sql ) ;
	    	}    
  	    s3.endRecord() ;
   		}    
    	
	wxCopyFile ( filename , dbname ) ;
	isSqlite3 = true ;
	getObjectSqlite3 ( "VACUUM;" ) ;
    wxEndBusyCursor() ;
    wxMessageBox ( txt("t_conversion_complete") ) ;
    return true ; // Dummy default
	}
    
