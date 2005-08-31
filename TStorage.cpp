/** \file
	\brief The TStorage and TSQLresult classes
*/
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
    if ( fn.IsEmpty() ) fn = myapp()->getLocalDBname() ;
    dbname = fn ;

    isSqlite3 = false ;
    if ( !isMySQL && fn != _T("") )
    	{
    	wxFile f ( fn , wxFile::read ) ;
    	char xx[100] ;
    	f.Read ( xx , 15 ) ;
    	xx[15] = 0 ;
    	if ( wxString ( xx , wxConvUTF8 ) == _T("SQLite format 3") )
    		isSqlite3 = true ;
    	}    

#ifdef USEMYSQL
    if ( isMySQL )
        {
        conn = new MYSQL ;
        mysql_init (conn);
        wxArrayString ex ;
        explode ( _T(":") , fn + _T(":") , ex ) ;
        if ( !mysql_real_connect ( conn , 
                                ex[1].mb_str() ,
                                ex[2].mb_str() ,
                                ex[3].mb_str() ,
                                ex[4].mb_str() , 0 , NULL , CLIENT_COMPRESS ) )
           {
//!!!!!           wxSafeShowMessage ( wxString::Format ( txt("t_mysql_error").c_str() , dbname.mb_str() ) , mysql_error ( conn ) ) ;
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
           st->results.field.Add ( wxString ( azColName[i] , wxConvUTF8 ) ) ;
        }
        
    nf = st->results.content.size() ;
    st->results.content.push_back ( wxArrayString() ) ;

    for(i=0; i<argc; i++)
        {
        wxString tmp ;
        if ( argv[i] )
		  	{
			tmp = wxString ( argv[i] , wxConvUTF8 ) ;
			tmp.Replace ( _T("\013") , _T("\n") ) ;
			}
        st->results.content[nf].Add ( tmp ) ;
//        if ( argv[i] ) st->results.content[nf].Add( argv[i] ) ;
//        else st->results.content[nf].Add ( "" ) ;
        }
    return 0;
    }
    
void TStorage::createDatabaseSqlite3 ()
    {
        return;
#ifdef USING_SQLITE_3
	 sqlite3 *db ;
    char *e = 0 ;
    int rc ;
    sqlite3_open ( dbname.mb_str() , &db ) ;
//    ierror = (int) e ;
//    if ( e ) error = e ;
//    else error = "Alles OK" ;
    sqlite3_close ( db ) ;
#endif
    }
        
void TStorage::createDatabase ()
    {
    if ( isMySQL ) return ;
    if ( isSqlite3 ) { createDatabaseSqlite3() ; return ; }
    sqlite *db ;
    char *e = 0 ;
    int rc ;
    db = sqlite_open ( dbname.mb_str() , 0 , &e ) ;
    ierror = (int) e ;
    if ( e ) error = wxString ( e , wxConvUTF8 ) ;
    else error = _T("Alles OK") ;
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
       if ( q != _T("SELECT") )
          {
          mysql_close ( conn ) ;
          return results ;
          }
       }

    int err = mysql_query ( conn , query.mb_str() ) ;
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
               results.content.push_back ( wxArrayString() ) ;
               for(i = 0; i < num_fields; i++)
               {
               	  wxString tmp = row[i] ? wxString ( row[i] , wxConvUTF8 ) : _T("") ;
               	  tmp.Replace ( _T("\013") , _T("\n") ) ;
                  results.content[rownum].Add ( tmp ) ;
               }
            }        
            
            MYSQL_FIELD *fields;            
            fields = mysql_fetch_fields(result);
            for(i = 0; i < num_fields; i++)
            {
               results.field.Add ( wxString ( fields[i].name , wxConvUTF8 ) ) ;
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
             wxMessageBox ( wxString ( mysql_error(conn) , wxConvUTF8 ) , wxString::Format ( _T("MySQL error %d") , err ) ) ;
             wxFile of ( _T("mysqlerr.txt") , wxFile::write ) ;
             of.Write ( query ) ;
         }    
        }    
#endif
    return results ;
    }
    
TSQLresult TStorage::getObjectSqlite3 ( const wxString &query )
    {
#ifdef USING_SQLITE_3
    sqlite3 *db ;
    char *e = 0 ;
    int rc ;
    
    sqlite3_open ( dbname.mb_str() , &db ) ;
    st = this ;
    results.clean() ;
    if ( db == NULL ) return results ; // Database broken or does not exist
    
    if ( writeProtect ) // Prevent old program version breaking the DB
       {
       wxString q = query.substr ( 0 , 6 ) ;
       if ( q != _T("SELECT") )
          {
          sqlite3_close ( db ) ;
          return results ;
          }
       }
    do {
        rc = sqlite3_exec ( db , query.mb_str() , callback , 0 , &e ) ;
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
    if ( e ) error = wxString ( e , wxConvUTF8 ) ;
    else error = _T("Alles OK") ;

    sqlite3_close ( db ) ;
    
    return results ;
#else
	 results.clean() ;
	 return results ;
#endif
    }
        
TSQLresult TStorage::getObjectSqlite2 ( const wxString &query )
	{    
    sqlite *db ;
    char *e = 0 ;
    int rc ;
    
    db = sqlite_open ( dbname.mb_str() , 0 , &e ) ;
    st = this ;
    results.clean() ;
    if ( db == NULL ) return results ; // Database broken or does not exist
    
    if ( writeProtect ) // Prevent old program version breaking the DB
       {
       wxString q = query.substr ( 0 , 6 ) ;
       if ( q != _T("SELECT") )
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
        rc = sqlite_exec ( db , query.mb_str() , callback , 0 , &e ) ;
        if ( rc == SQLITE_BUSY ) // If busy, wait 200 ms
        	{
#ifdef __WXMSW__
			wxUsleep ( 200 ) ;
#else
//       	    wxMilliSleep ( 200 ) ;
#endif
        	}    
        } while ( rc == SQLITE_BUSY ) ;
    
    ierror = (int) e ;
    if ( e ) error = wxString ( e , wxConvUTF8 ) ;
    else error = _T("Alles OK") ;

    sqlite_close ( db ) ;
    
    return results ;
    }
    
void TStorage::import ()
    {
    TSQLresult sr ;
    int a ;
    
    // Importing restriction enzymes
    sr = getObject ( _T("SELECT * FROM enzyme") ) ;
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
           e->dbid = atol ( sr[a][sr["e_id"]].mb_str() ) ;
           e->sequence = sr[a][sr["e_sequence"]] ;
           e->note = sr[a][sr["e_note"]] ;
           e->location = sr[a][sr["e_location"]] ;
           e->cut = atoi ( sr[a][sr["e_cut"]].mb_str() ) ;
           e->overlap = atoi ( sr[a][sr["e_overlap"]].mb_str() ) ;
           re.Add ( e ) ;
           }
        }
    }

void TStorage::sqlAdd ( wxString &s1 , wxString &s2 , wxString key , wxString value )
    {
    int a ;
    for ( a = 0 ; a < value.length() ; a++ ) // Avoiding single quotes in value
        if ( value .GetChar(a) == '"' ) value.SetChar(a,39) ;
    value.Replace ( _T("\n") , _T("\013") ) ;
    if ( !s1.IsEmpty() ) s1 += _T(",") ;
    if ( !s2.IsEmpty() ) s2 += _T(",") ;
    s1 += key ;
    s2 += _T("\"") + value + _T("\"") ;
    }

void TStorage::sqlAdd ( wxString &s1 , wxString &s2 , wxString key , char* value )
    {
    sqlAdd ( s1 , s2 , key , wxString ( value , wxConvUTF8 ) ) ;
    }

void TStorage::sqlAdd ( wxString &s1 , wxString &s2 , wxString key , int value )
    {
    wxString t = wxString::Format ( _T("%d") , (unsigned int) value ) ;
//    char t[1000] ;
//    sprintf ( t , "%d" , value ) ;
    if ( !s1.IsEmpty() ) s1 += _T(",") ;
    if ( !s2.IsEmpty() ) s2 += _T(",") ;
    s1 += key ;
    s2 += _T("\"") + t + _T("\"") ;
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
    wxString sql = _T("SELECT * FROM stuff WHERE s_type=\"DATABASE\"") ;
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
    wxString sql = _T("SELECT * FROM stuff WHERE s_type=\"DEFAULT_DATABASE\"") ;
    r = getObject ( sql ) ;
    if ( r.content.size() == 0 ) return defdb ;
    else return r[0][r["s_name"]] ;
    }
    
wxString TStorage::getSingleField ( wxString query , wxString field , wxString def )
    {
    TSQLresult sr ;
    sr = getObject ( query ) ;
    if ( sr.content.size() > 0 )
	 	{
		int p = sr[field.mb_str()] ;
		return sr[0][p] ;
		}
    else return def ;
    }
    
int TStorage::getSingleField ( wxString query , wxString field , int def )
    {
//    char t[100] ;
//    sprintf ( t , "%d" , def ) ;
    wxString r = getSingleField ( query , field , wxString::Format ( _T("%d") , def ) ) ;
    return atoi ( r.mb_str() ) ;
    }
    
wxString TStorage::getOption ( wxString oname , wxString def )
    {
    return getSingleField ( _T("SELECT s_value FROM stuff WHERE s_name=\"") +
                            oname + 
                            _T("\" AND s_type=\"OPTION\"") ,
                            _T("s_value") , def ) ;
    }

int TStorage::getOption ( wxString oname , int def )
    {
    return getSingleField ( _T("SELECT s_value FROM stuff WHERE s_name=\"") +
                            oname + 
                            _T("\" AND s_type=\"OPTION\"") ,
                            _T("s_value") , def ) ;
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
    

/**	\brief Copies a whole table to another database

	This is used for auto-updating sqlite databases, which do not support ALTER TABLE
*/
bool TStorage::copySQLfields ( TStorage &target , wxString table , wxString cond )
    {
    int a , b ;
    TSQLresult r ;
    wxString sql , s1 , s2 ;
    sql = _T("SELECT * FROM ") + table + _T(" WHERE ") + cond ;
    r = getObject ( sql ) ;
    sql = _T("DELETE FROM ") + table + _T(" WHERE ") + cond ;
    target.getObject ( sql ) ;
    for ( a = 0 ; a < r.rows() ; a++ )
        {
        s1 = s2 = _T("") ;
        for ( b = 0 ; b < r.cols() ; b++ )
           target.sqlAdd ( s1 , s2 , r.field[b] , r[a][b] ) ;
        sql = _T("INSERT INTO ") + table + _T(" (") + s1 + _T(") VALUES (") + s2 + _T(")") ;
        target.getObject ( sql ) ;
        }
    return true ;
    }
    
    
    
    
void TStorage::replaceTable ( wxString table , wxArrayString &f , wxArrayString &t )
    {
    int a , b ;
    TSQLresult r ;
    wxString sql , s1 , s2 ;
    sql = _T("SELECT * FROM ") + table ;
    r = getObject ( sql ) ;
    getObject ( _T("DROP TABLE ") + table ) ;
    
    wxString create ;
    for ( a = 0 ; a < f.GetCount() ; a++ )
        {
        if ( !create.IsEmpty() ) create += _T(",\n") ;
        create += f[a] + _T(" ") + t[a] ;
        }
    create = _T("CREATE TABLE ") + table + _T("(\n") + create + _T(")") ;
    
    getObject ( create ) ;
    
    for ( a = 0 ; a < r.rows() ; a++ )
        {
        s1 = s2 = _T("") ;
        for ( b = 0 ; b < f.GetCount() ; b++ )
           {
           int id = r[(char*)f[b].c_str()] ;
           if ( id > -1 ) sqlAdd ( s1 , s2 , f[b] , r[a][id] ) ;
           else sqlAdd ( s1 , s2 , f[b] , _T("") ) ;
           }
        sql = _T("INSERT INTO ") + table + _T(" (") + s1 + _T(") VALUES (") + s2 + _T(")") ;
        getObject ( sql ) ;
        }
    }
    
void TStorage::tableInfoSet ( wxArrayString &f , wxArrayString &t , wxString nf , wxString nt )
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
    setOption ( oname , wxString::Format ( _T("%d") , value ) ) ;
    }
    
void TStorage::setOption ( wxString oname , wxString vname )
    {
    wxString sql ;
    sql = _T("DELETE FROM stuff WHERE s_type=\"OPTION\" AND s_name=\"") ;
    sql += oname ;
    sql += _T("\"") ;
    getObject ( sql ) ;
    sql = _T("INSERT INTO stuff (s_type,s_name,s_value) VALUES (") ;
    sql += _T("\"OPTION\",\"") + oname + _T("\",\"") + vname + _T("\")") ;
    getObject ( sql ) ;
    }
    
void TStorage::autoUpdateSchema ()
    {
    if ( isMySQL ) return ; // No updates yet
    if ( dbname == _T("blank.db") ) return ; // No update of blank db
    TSQLresult r ;
    wxString sql , s1 , s2 ;
    char t[1000] ;
    
    // Database version
    int version , oversion ;
    sql = _T("SELECT s_value FROM stuff WHERE s_type=\"INTERNAL\" AND s_name=\"DATABASE_VERSION\"") ;
    r = getObject ( sql ) ;
    if ( r.rows() == 0 ) version = 0 ;
    else version = atoi ( r[0][0].mb_str() ) ;
    oversion = version ;

    // Required program version
    sql = _T("SELECT s_value FROM stuff WHERE s_type=\"INTERNAL\" AND s_name=\"REQUIRED_PROGRAM_VERSION\"") ;
    r = getObject ( sql ) ;
    if ( r.rows() == 0 ) rpv = 0 ;
    else rpv = atoi ( r[0][0].mb_str() ) ;
    if ( rpv > myapp()->programVersion ) // Old program version, no write to this DB
       {
       writeProtect = true ;
       if ( !myapp()->dbWarningIssued && txt("LOADED") == wxString(_T("yes")) )
          {
          wxMessageBox ( txt("t_pov_warning") ) ;
          myapp()->dbWarningIssued = true ;
          }
       }
    
    wxArrayString dnaF , dnaT ;
    
    // Version 0
    tableInfoSet ( dnaF , dnaT , _T("dna_name") , _T("tinytext") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_description") , _T("tinytext") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_type") , _T("varchar(32)") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_sequence") , _T("mediumtext") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_sticky_ul") , _T("varchar(32)") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_sticky_ur") , _T("varchar(32)") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_sticky_ll") , _T("varchar(32)") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_sticky_lr") , _T("varchar(32)") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_restriction_enzymes") , _T("mediumtext") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_circular") , _T("boolean") ) ;
    
    // Version 0.1
    tableInfoSet ( dnaF , dnaT , _T("dna_params") , _T("mediumtext") ) ;
    tableInfoSet ( dnaF , dnaT , _T("dna_type") , _T("int") ) ;
    if ( version < 1 ) // Enabeling different DNA types
        {
        version = 1 ;
        sql = _T("UPDATE dna SET dna_type=\"0\"") ;
        getObject ( sql ) ;
        replaceTable ( _T("dna") , dnaF , dnaT ) ;
        }
        
    // Version 0.2
    if ( version < 2 ) // Creating indices
        {
        version = 2 ;
        getObject ( _T("CREATE INDEX k_dna ON dna (dna_name)") ) ;
        getObject ( _T("CREATE INDEX k_dna_type ON dna (dna_type)") ) ;
        getObject ( _T("CREATE INDEX k_dna_desc ON dna (dna_description)") ) ;
        }
        
    // Version 0.3
    if ( version < 3 )
        {
        version = 3 ;
        r = getObject ( _T("SELECT eg_name FROM enzyme_group") ) ;
        getObject ( _T("DROP TABLE enzyme_group") ) ;
        getObject ( _T("CREATE TABLE enzyme_group ( eg_name tinytext )") ) ;
        for ( int a = 0 ; a < r.rows() ; a++ )
           {
           sql = _T("INSERT INTO enzyme_group ( eg_name ) VALUES ( \"") ;
           sql += r[a][0] ;
           sql += _T("\")") ;
           getObject ( sql ) ;
           }
        }

    // Version 0.4
    if ( version < 4 )
        {
        wxBeginBusyCursor() ;
        version = 4 ;
        TStorage *bl = myapp()->frame->getTempDB ( _T("blank.db") ) ;
        r = bl->getObject ( _T("SELECT * FROM enzyme") ) ;
        for ( int a = 0 ; a < r.rows() ; a++ )
           {
           wxString name = r[a][r["e_name"]] ;
           wxString seq = r[a][r["e_sequence"]] ;
           wxString cut = r[a][r["e_cut"]] ;
           wxString ol = r[a][r["e_overlap"]] ;
           sql = _T("UPDATE enzyme SET e_sequence='") + seq +
                      _T("', e_cut='") + cut +
                      _T("', e_overlap='") + ol +
                      _T("' WHERE e_name='") + name + _T("'") ;
           getObject ( sql ) ;
           }    
        wxEndBusyCursor() ;
        }    
    
    // Writing new version, if necessary
    if ( version > oversion )
        {
        sql = _T("DELETE FROM stuff WHERE s_type=\"INTERNAL\" AND s_name=\"DATABASE_VERSION\"") ;
        getObject ( sql ) ;
        s1 = s2 = _T("") ;
        sprintf ( t , "%d" , version ) ;
        sqlAdd ( s1 , s2 , _T("s_type")  , _T("INTERNAL") ) ;
        sqlAdd ( s1 , s2 , _T("s_name")  , _T("DATABASE_VERSION") ) ;
        sqlAdd ( s1 , s2 , _T("s_value") , t ) ;
        sql = _T("INSERT INTO stuff (") + s1 + _T(") VALUES (") + s2 + _T(")") ;
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
    wxString ds = wxString::Format ( _T("%4d-%3d") , d.GetYear() , d.GetDayOfYear() ) ;
    if ( getOption ( _T("LASTSYNC") , _T("") ) == ds ) return ;
    setOption ( _T("LASTSYNC") , ds ) ;
    
    // Known database list
    wxArrayString files ;
    r = getObject ( _T("SELECT s_value FROM stuff WHERE s_type=\"DATABASE\"") ) ;
    for ( a = 0 ; a < r.rows() ; a++ )
        files.Add ( r[a][r["s_value"]] ) ;

    do {
        changed = false ;
        for ( a = 0 ; a < files.GetCount() ; a++ )
           {
           TStorage t ( TEMP_STORAGE , files[a] ) ;

           TSQLresult r1 , r2 ;
           r1 = getObject ( _T("SELECT * FROM enzyme ORDER BY e_name") ) ;
           r2 = t.getObject ( _T("SELECT * FROM enzyme ORDER BY e_name") ) ;

           for ( b = c = 0 ; b < r1.rows() && c < r2.rows() ; )
              {
              if ( r1[b][r1["e_name"]] == r2[c][r2["e_name"]] )
                 {
                 b++ ;
                 c++ ;
                 }
              else if ( r1[b][r1["e_name"]] < r2[c][r2["e_name"]] ) // Export
                 {
                 t.getObject ( makeInsert ( _T("enzyme") , r1.field , r1[b] ) ) ;
                 b++ ;
                 }
              else if ( r1[b][r1["e_name"]] > r2[c][r2["e_name"]] ) // Import
                 {
                 changed = true ;
                 getObject ( makeInsert ( _T("enzyme") , r2.field , r2[c] ) ) ;
                 c++ ;
                 }
              }
            while ( b < r1.rows() )
               {
               t.getObject ( makeInsert ( _T("enzyme") , r1.field , r1[b++] ) ) ;
               }
            while ( c < r2.rows() )
               {
               getObject ( makeInsert ( _T("enzyme") , r2.field , r2[c++] ) ) ;
               changed = true ;
               }
           }
        } while ( changed ) ;
    }
    
wxString TStorage::makeInsert ( wxString table , wxArrayString &field , wxArrayString &data )
    {
    wxString sql , s1 , s2 ;
    for ( int a = 0 ; a < field.GetCount() ; a++ )
        sqlAdd ( s1 , s2 , field[a] , data[a] ) ;
    sql = _T("INSERT INTO ") + table + _T(" (") + s1 + _T(") VALUES (") + s2 + _T(")") ;
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
    e.name = _T("*") + p->name ;
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
    mysql_real_connect ( &c , ip.mb_str() ,
                                    name.mb_str() , pwd.mb_str() ,
                                    "mysql" ,
                                    0 , 
                                    NULL , CLIENT_COMPRESS ) ;
    
    wxString query = _T("CREATE DATABASE ") + db ;
    int err = mysql_query ( &c , query.mb_str() ) ;
    
    if ( err != 0 )
       {
       mysql_close ( &c ) ;
       return _T("") ;
       }
       
    mysql_real_connect ( &c , ip.mb_str() ,
                                    name.mb_str() , pwd.mb_str() ,
                                    db.mb_str() ,
                                    0 , 
                                    NULL , CLIENT_COMPRESS ) ;

    wxString t , fn = myapp()->homedir + myapp()->slash + _T("MySQL template.txt") ;
    wxTextFile tf ( fn ) ;
    tf.Open ( *(myapp()->isoconv) ) ;
    for ( int lc = 0 ; lc < tf.GetLineCount() ; lc++ )
        {
        wxString s = tf[lc].Trim().Trim(true) ;
        if ( s != _T("") && !s.StartsWith ( _T("#") ) )
           {
           t += s + _T(" ") ;
           if ( s.Last() == ';' )
              {
              t.Truncate ( t.length() - 2 ) ;
              err = mysql_query ( &c , t.mb_str() ) ;
              if ( err != 0 ) wxMessageBox ( t , wxString::Format ( _T("Error %d") , err ) ) ;
              t = _T("") ;
              }    
           }    
        }    
    
    mysql_close ( &c ) ;
    return _T(":") + ip + _T(":") + name + _T(":") + pwd + _T(":") + db ;
#endif  
    return _T("") ;  
    }    

void TStorage::optimizeDatabase ()
	{
    if ( isMySQL ) return ;
	getObject ( _T("VACUUM;") ) ;
	}

TSQLresult TStorage::getObject ( const wxString &query )
    {
    if ( recording )
    	{
	    record += query + _T("; ") ;
        results.clean() ;
	    return results ;
    	}    
    if ( isMySQL ) return getObject_MySQL ( query ) ;
    if ( isSqlite3 ) return getObjectSqlite3 ( query ) ;
   	return getObjectSqlite2 ( query ) ;
   	}   	

void TStorage::startRecord ()
	{
 	if ( isMySQL ) { /*getObject ( "BEGIN" ) ; */return ; }
	record = _T("") ;
	recording = true ;
	}    

void TStorage::endRecord ()
	{
 	if ( isMySQL ) { /*getObject ( "COMMIT" ) ; */return ; }
	recording = false ;
	if ( !record.IsEmpty() )
		{
    	record = _T("BEGIN; ") + record + _T("COMMIT;") ;
		getObject ( record ) ;
		}		
	record = _T("") ;
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
    wxString sql = _T("SELECT eg_name FROM enzyme_group WHERE eg_name=\"") + s + _T("\"") ;
    sr = getObject ( sql ) ;
    if ( sr.rows() > 0 ) return false ; // Already exists
    
    wxString s1 , s2 ;
    sqlAdd ( s1 , s2 , _T("eg_name") , s ) ;
    sql = _T("INSERT INTO enzyme_group (") + s1 + _T(") VALUES (") + s2 + _T(")") ;
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
        sql = _T("SELECT leg_enzyme FROM link_enzyme_group") ;
        sql += _T(" WHERE leg_group=\"") + gn + _T("\"") ;
        sr = getObject ( sql ) ;
        for ( a = 0 ; a < sr.content.size() ; a++ )
           if ( sr[a][sr["leg_enzyme"]].GetChar(0) != '*' )
              vs.Add ( sr[a][sr["leg_enzyme"]] ) ;
        }
    else
        {
        sql = _T("SELECT e_name from enzyme") ;
        sr = getObject ( sql ) ;
        for ( a = 0 ; a < sr.content.size() ; a++ )
           if ( sr[a][sr["e_name"]].GetChar(0) != '*' )
              vs.Add ( sr[a][sr["e_name"]] ) ;
        }
    
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( vs[a].IsEmpty() )
           {
           vs.RemoveAt ( a ) ;
           a-- ;
           }
        }
        
    if ( !isLocalDB() ) setEnzymeCache ( gn , vs ) ;
    }

void TStorage::getEnzymeGroups ( wxArrayString &vs )
    {
    wxString defdb = getDefaultDB() ;
    TStorage *t = getDBfromEnzymeGroup ( defdb + _T(":dummy") ) ;
    if ( t && isLocalDB() && t != this )
    	{
	    t->getEnzymeGroups ( vs ) ;
	    for ( int a = 0 ; a < vs.GetCount() ; a++ )
	    	vs[a] = defdb + _T(":") + vs[a] ;
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
    wxString sql = _T("SELECT eg_name FROM enzyme_group") ;
    sr = getObject ( sql ) ;
    for ( a = 0 ; a < sr.content.size() ; a++ )
    	{
	    wxString groupname = UCfirst ( sr[a][sr["eg_name"]] ) ;
        vs.Add ( groupname ) ;
        if ( !isLocalDB() )
        	{
        	enzymeGroupNameCache.Add ( groupname ) ; // Add to cache
        	enzymeGroupCache.Add ( _T("") ) ; // Add blank dummy to cache
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
    sql = _T("DELETE FROM enzyme WHERE e_name=\"")  +e->name + _T("\"") ;
    getObject ( sql ) ;
    
    // Get new id
    sql = _T("SELECT max(e_id) FROM enzyme") ;
    sr = getObject ( sql ) ;
    if ( ierror ) return ;
    int e_id = atoi ( sr.content[0][0].mb_str() ) ;
    sprintf ( u , "%d" , e_id+1 ) ;
    e->dbid = e_id ;
    
    // Insert new enzyme
    sql = _T("INSERT INTO enzyme (e_id,e_name,e_sequence,e_location,e_note,e_cut,e_overlap) VALUES (\"") ;
    sql += wxString ( u , wxConvUTF8 ) ;
    sql += _T("\",\"") ;
    sql += e->name + _T("\",\"") ;
    sql += e->sequence + _T("\",\"") ;
    sql += e->location + _T("\",\"") ;
    sql += e->note + _T("\",\"") ;
    //sprintf ( u , "%d" , e->cut ) ;
    sql += wxString::Format ( _T("%d") , e->cut ) ;
    sql += _T("\",\"") ;
    //sprintf ( u , _T("%d") , e->overlap ) ;
    sql += wxString::Format ( _T("%d") , e->overlap ) ;
    sql += _T("\")") ;
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
    sql = _T("DELETE FROM link_enzyme_group WHERE leg_enzyme=\"") +
          enzyme + _T("\" AND leg_group=\"") + group + _T("\"") ;
    getObject ( sql ) ;
    
    sql = _T("INSERT INTO link_enzyme_group (leg_enzyme,leg_group) VALUES (\"") +
	 	enzyme + _T("\",\"") + group + _T("\")") ;
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
    sql = _T("DELETE FROM link_enzyme_group WHERE leg_enzyme=\"") +
          enzyme +
          _T("\" AND leg_group=\"") +
          group +
          _T("\"") ;
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
    sql = _T("DELETE FROM link_enzyme_group WHERE leg_group=\"") + group + _T("\"") ;
    getObject ( sql ) ; 
    sql = _T("DELETE FROM enzyme_group WHERE eg_name=\"") + group + _T("\"") ;
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
		enzymeGroupCache.Add ( _T("") ) ;
		}    
	enzymeGroupCache[a] = implode ( _T(",") , enzymes ) ;
	}

void TStorage::getEnzymeCache ( wxString group , wxArrayString &enzymes )
	{
	enzymes.Clear () ;
	if ( group == txt("All") ) return ;

	int a ;
	for ( a = 0 ; a < enzymeGroupNameCache.GetCount() && group != enzymeGroupNameCache[a] ; a++ ) ;
	if ( a == enzymeGroupNameCache.GetCount() ) return ;

	explode ( _T(",") , enzymeGroupCache[a] , enzymes ) ;
	}

// This function can convert a sqlite 2.X database into an sqlite 3.X one
bool TStorage::convertSqlite2to3 ()
	{
    wxBeginBusyCursor() ;
	wxString filename = wxFileName::CreateTempFileName ( _T("GENtle_") ) ;
	wxCopyFile ( _T("blank.db") , filename ) ;
	TStorage s3 ( TEMP_STORAGE , filename ) ;
	
	wxString sql = _T("SELECT name FROM sqlite_master WHERE type='table'") ;
    TSQLresult r ;
    r = getObjectSqlite2 ( sql ) ;
    
    wxArrayString tables ;
    int a , b , c ;
    for ( a = 0 ; a < r.content.size() ; a++ )
    	tables.Add ( r[a][r["name"]] ) ;
    	
   	for ( a = 0 ; a < tables.GetCount() ; a++ )
   		{
	    sql = _T("SELECT * FROM ") + tables[a] ;
	    r = getObjectSqlite2 ( sql ) ;
	    s3.startRecord() ;
	    s3.getObject ( _T("DELETE FROM ") + tables[a] ) ;
	    for ( b = 0 ; b < r.content.size() ; b++ )
	    	{
 	    	wxString s1 , s2 ;
 	    	for ( c = 0 ; c < r.field.GetCount() ; c++ )
  		    	s3.sqlAdd ( s1 , s2 , r.field[c] , r[b][c] ) ;
			sql = _T("INSERT INTO ") + tables[a] + _T(" (") + s1 + _T(") VALUES (") + s2 + _T(")") ;
         s3.getObject ( sql ) ;
	    	}    
  	    s3.endRecord() ;
   		}    
    	
	wxCopyFile ( filename , dbname ) ;
	isSqlite3 = true ;
	getObjectSqlite3 ( _T("VACUUM;") ) ;
    wxEndBusyCursor() ;
    wxMessageBox ( txt("t_conversion_complete") ) ;
    return true ; // Dummy default
	}
    
void TStorage::syncEnzymes ( TStorage *to )
	{
	bool useBlank = false ;
	if ( to == NULL )
		{
		to = new TStorage ( TEMP_STORAGE , myapp()->homedir + myapp()->slash + _T("blank.db") ) ;
		useBlank = true ;
		}
	
	TSQLresult r1 , r2 ;
	r1 = getObject ( "SELECT * FROM enzyme" ) ;
	r2 = to->getObject ( "SELECT * FROM enzyme" ) ;
	
	int a ;
	wxArrayString s1 , s2 ;
	for ( a = 0 ; a < r1.rows() ; a++ ) s1.Add ( r1[a][r1["e_name"]] ) ;
	for ( a = 0 ; a < r2.rows() ; a++ ) s2.Add ( r2[a][r2["e_name"]] ) ;
	
	startRecord() ;
	for ( a = 0 ; a < s2.GetCount() ; a++ )
		{
		if ( wxNOT_FOUND != s1.Index ( s2[a] ) ) continue ; // It's there
		wxString sql , k , v  ;
		sqlAdd ( k , v , "e_name" , s2[a] ) ;
		sqlAdd ( k , v , "e_sequence" , r2[a][r2["e_sequence"]] ) ;
		sqlAdd ( k , v , "e_note" , _T("") ) ;
		sqlAdd ( k , v , "e_location" , _T("") ) ;
		sqlAdd ( k , v , "e_cut" , _T("0") ) ;
		sqlAdd ( k , v , "e_overlap" , _T("0") ) ;
		k.Replace ( "\"" , "'" ) ;
		v.Replace ( "\"" , "'" ) ;
		sql = _T("INSERT INTO enzyme (e_id,") + k + _T(") VALUES ((SELECT max(e_id) FROM enzyme)+1,") + v + _T(")") ;
		getObject ( sql ) ;
		}
	endRecord() ;
	
	if ( useBlank ) delete to ;
	}
