#include "GenBank.h"
#include <wx/textfile.h>

char* gb_item_type[VIT_TYPES] =
    {
    "",
    "gene",
    "CDS",
    "rep_origin",
    "promoter",
    "terminator",
    "misc_feature",
    "prot_bind",
    "oriT"
    } ;

TGenBank::TGenBank ()
    {
    success = false ;
    }
    
void TGenBank::load ( wxString s )
    {
	wxTextFile file ;
    file.Open ( s.c_str() ) ;
	vs.Clear() ;
	wxString str ;
	for ( str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
	 {
	 if ( trim ( str.c_str() ) != "" ) vs.Add ( str.c_str() ) ;
	 }
    if ( trim ( str.c_str() ) != "" ) vs.Add ( str.c_str() ) ;
    parseLines () ;
    }
    
void TGenBank::paste ( wxString s )
    {
    vs = explode ( "\n" , s ) ;
/*    vs.Clear() ;
    vector <string> i = explode ( "\n" , s.c_str() ) ;
    for ( int a = 0 ; a < i.size() ; a++ )
       if ( i[a] != "" ) vs.Add ( i[a].c_str() ) ;
*/
    /*
    for ( int a = 0 ; a < s.Length() ; a++ )
       if ( s.GetChar(a) < 15 )
          s.SetChar ( a , '\n' ) ;
    while ( s.BeforeFirst ( '\n' ) != "" )
       {
       wxString t = s.BeforeFirst ( '\n' ) ;
       if ( trim ( t ) != "" ) vs.Add ( t ) ;
       s = s.AfterFirst ( '\n' ) ;
       wxMessageBox ( t , s ) ;
       }
    if ( trim ( s ) != "" ) vs.Add ( s ) ;    
    */
    parseLines () ;
    }

void TGenBank::parseLines ()
    {
    success = false ;
    vi.clear () ;
    vi_l.clear () ;
    vs_l.clear () ;
    
    if ( vs.IsEmpty () ||
         vs[0].Left ( 5 ) != "LOCUS" )
       return ;

    int a ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        vi.push_back ( vs[a].Length() - trim(vs[a]).Length() ) ;

    // Pre-processing
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( vi[a] == 12 || ( vi[a] == 21 && vs[a].GetChar(21) != '/' ) )
           {
           vs[a-1] += " " + trim ( vs[a] ) ;
           vs.Remove ( a ) ;
           vi.erase ( vi.begin() + a ) ;
           a-- ;
           }
        }
    
//    ofstream out ( "bla.txt" , ios::out ) ;
//    for ( a = 0 ; a < vs.GetCount() ; a++ ) out << vs[a].c_str() << endl ;
    
    wxArrayString t ;
    TVI ti ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( trim(vs[a]).Left(2) == "//" )
           {
           vs_l.push_back ( t ) ;
           vi_l.push_back ( ti ) ;
           t.Clear () ;
           ti.clear () ;
           }
        else if ( trim(vs[a]) != "" ) 
           {
           t.Add ( trim ( vs[a] ) ) ;
           ti.push_back ( vi[a] ) ;
           }
        }
    
    if ( t.GetCount() > 0 )
        {
        vs_l.push_back ( t ) ;
        vi_l.push_back ( ti ) ;
        }
    
    if ( vs_l.size() > 0 )
        {
        success = true ;
        vs = vs_l[0] ;
        vi = vi_l[0] ;
        }
    }

void TGenBank::remap ( TVector *v )
	{
	int line ;
	wxString k1 ;
	vector <wxArrayString> items ;
	for ( line = 0 ; line < vs.GetCount() ; line++ )
	 {
	 wxString l = vs[line] ;
	 int i = vi[line] ;
	 if ( i == 0 ) // New main level keyword
	     {
	     l += " " ;
	     k1 = l.BeforeFirst ( ' ' ) . MakeUpper() ;
	     wxString l2 = trim ( l.AfterFirst ( ' ' ) ) ;
	     i += l.Length() - l2.Length() ;
	     l = l2 ;
	     }
     if ( k1 == "LOCUS" )
          {
          wxString n = l.c_str() ;
          v->name = n.BeforeFirst ( ' ' ) .c_str() ;
          v->desc = n.AfterFirst ( ' ' ) .c_str() ;
          l += " " ; // For substring search
          if ( l.MakeUpper().Contains ( " AA " ) ) v->type = TYPE_AMINO_ACIDS ;
          else v->type = TYPE_VECTOR ;
          if ( l.MakeUpper().Contains ( " CIRCULAR" ) ) v->setCircular ( true ) ;
          else v->setCircular ( false ) ;
          }
     else if ( k1 == "FEATURES" )
          {
          if ( i == 5 )
             {
             items.push_back ( wxArrayString() ) ;
             items[items.size()-1].Add ( l.BeforeFirst ( ' ' ) ) ;
             wxString l2 = trim ( l.AfterFirst ( ' ' ) ) ;
             i += l.Length() - l2.Length() ;
             l = l2 ;
             }
          if ( items.size() ) items[items.size()-1].Add ( l ) ;
          }
     else if ( k1 == "ORIGIN" )
          {
          l = l.MakeUpper() ;
          for ( int a = 0 ; a < l.Length() ; a++ )
             if ( isValidSequence ( l.GetChar(a) ) )
                v->sequence += l.GetChar(a) ;
          }
	 }
	 
    for ( int a = 0 ; a < items.size() ; a++ ) addItem ( v , items[a] ) ;
	}

bool TGenBank::isValidSequence ( char a )
    {
    if ( a == '-' ) return true ;
    if ( a >= 'A' && a <= 'Z' ) return true ;
    return false ;
    }

void TGenBank::addItem ( TVector *v , wxArrayString &va )
    {
    TVectorItem i ( "" , "" , 1 , 1 , VIT_MISC ) ; // Dummy values
    int a ;
    
    // Type
    if ( va[0].MakeLower() == "source" ) return ;
    for ( a = 0 ; a < VIT_TYPES ; a++ )
        {
        if ( va[0].CmpNoCase ( gb_item_type[a] ) == 0 ) i.type = a ;
        }
       
    // Properties
    for ( a = 1 ; a < va.GetCount() ; a++ )
        {
        if ( va[a].GetChar(0) != '/' ) continue ;
        wxString l = va[a].Mid ( 1 ) ;
        wxString p = l.BeforeFirst ( '=' ) ;
        if ( p == "" ) continue ;
        p = trim ( p.Trim ( true ) ) ;
        p = trim ( trimQuotes ( p.Lower() ) ) ;
        wxString v = l.AfterFirst ( '=' ) ;
        v = trim ( v.Trim ( true ) ) ;
        v = trim ( trimQuotes ( v ) ) ;
        if ( p == "standard_name" || p == "gene" || p == "protein_id" )
           i.name = v.c_str() ;
        else
           {
           if ( p == "product" || p == "organism" || p == "db_xref" ||
                p == "mol_type" || p == "chromosome" )
              {
              if ( i.desc != "" ) i.desc += "\n" ;
              if ( p == "chromosome" || p == "db_xref" )
                 {
                 i.desc += p.c_str() ;
                 i.desc += " : " ;
                 }
              i.desc += v.c_str() ;
              }
           p = "/" + p ;
           i.setParam ( p.c_str() , v.c_str() ) ;
           }
        }
        
    // From / To
    for ( a = 1 ; a < va.GetCount() ; a++ )
        {
        if ( va[a].GetChar(0) == '/' ) continue ;
        wxString l = va[a] ;
        l.Replace ( " " , "" ) ;
        l = l.MakeUpper () ;
        iterateItem ( v , i , l ) ;
        }
    }
    
void TGenBank::iterateItem ( TVector *v , TVectorItem &i , wxString l )
    {
    while ( l != "" )
        {
        char c = l.GetChar(0) ;
        if ( c == '<' ) l = l.Mid ( 1 ) ;
        else if ( c == ',' ) l = l.Mid ( 1 ) ;
        else if ( c >= '0' && c <= '9' )
           {
           int a ;
           wxString from , to ;
           from = l.BeforeFirst ( '.' ) ;
           l = l.AfterFirst ( '.' ) ;
           while ( l != "" && ( l.GetChar(0) < '0' || l.GetChar(0) > '9' ) ) l = l.Mid ( 1 ) ;
           for ( a = 0 ; a < l.Length() && l.GetChar(a) >= '0' && l.GetChar(a) <= '9' ; a++ ) ;
           to = l.Left ( a ) ;
           l = l.Mid ( a ) ;
           long ll ;
           from.ToLong ( &ll ) ;
           i.from = ll ;
           to.ToLong ( &ll ) ;
           i.to = ll ;
           v->items.push_back ( i ) ;
           }
        else if ( l.Left ( 4 ) == "JOIN" )
           {
           l = l.AfterFirst ( '(' ) ;
           int a , b , cnt , num = 0 ;
           char c = ' ' ;
           for ( a = b = cnt = 0 ; a < l.Length() && ( cnt >= 0 || c != ')' ) ; a++ )
              {
              c = l.GetChar ( a ) ;
              if ( c == '(' ) cnt++ ;
              if ( ( c == ',' || c == ')' ) && cnt == 0 )
                 {
                 num++ ;
                 wxString name = i.name.c_str() ;
                 wxString newname = name + wxString::Format ( " (%d)" , num ) ;
                 wxString sub = l.Mid ( b , a - b ) ;
                 if ( num > 1 ) i.setParam ( "PREDECESSOR" , (name + wxString::Format ( " (%d)" , num-1 )).c_str() ) ;
                 if ( c != ')' ) i.setParam ( "SUCCESSOR" , (name + wxString::Format ( " (%d)" , num+1 )).c_str() ) ;
                 i.name = newname.c_str() ;
                 iterateItem ( v , i , sub ) ;
                 b = a+1 ;
                 i.name = name.c_str() ;
                 i.setParam ( "PREDECESSOR" , "" ) ;
                 i.setParam ( "SUCCESSOR" , "" ) ;
                 }
              if ( c == ')' ) cnt-- ;
              }
           l = l.Mid ( a ) ;
           }
        else
           {
           return ;
           }
        }
    }


wxString TGenBank::trim ( wxString s )
	{
//	return s.Trim() ;
	while ( s != "" && ( s.GetChar(0) == ' ' || s.GetChar(0) < 15 ) ) s = s.Mid ( 1 ) ;
	return s ;
	}


wxString TGenBank::trimQuotes ( wxString s )
	{
    if ( s.GetChar(0) == '"' ) s = s.Mid ( 1 ) ;
    if ( s.Right(1) == "\"" ) s = s.Mid ( 0 , s.Length()-1 ) ;
    return s ;
	}


/*
wxString TGenBank::quote ( wxString pre , wxString q )
	{
	return "" ;
	}


wxArrayString TGenBank::explode ( char p , wxString s )
	{
	wxArrayString x ;
	return x ;
	}


void TGenBank::wrapit ( wxArrayString &ex , wxString init , wxString data , int limit )
	{
	}


wxString TGenBank::expand ( wxString init , int to , wxString with )
	{
	return "" ;
	}

*/



void TGenBank::wrapit ( wxArrayString &ex , wxString init , wxString data , int limit )
    {
    wxString blanks = expand ( "" , init.Length() ) ;
    int allow = limit - blanks.length() ;
    wxString first = init ;
    while ( data != "" )
        {
        wxString s , t ;
        s = first ;
        first = blanks ;
        if ( allow > data.length() ) allow = data.length() ;
        t = data.substr ( 0 , allow ) ;
        data = data.substr ( allow ) ;
        s += t ;
        ex.Add ( s ) ;
        }
    }
    
wxString TGenBank::expand ( wxString init , int to , wxString with )
    {
    while ( init.length() < to ) init += with ;
    return init.substr ( 0 , to ) ;
    }
    
wxString TGenBank::quote ( wxString pre , wxString q )
    {
    int a ;
    for ( a = 0 ; a < q.length() ; a++ )
       {
       if ( q.GetChar(a) == '\n' || q.GetChar(a) == '\r' ) q.SetChar ( a , ' ' ) ;
       else if ( q.GetChar(a) == '\"' ) q.SetChar ( a , '-' ) ;
       }
    for ( a = 1 ; a < q.length() ; a++ )
       {
       if ( q.GetChar(a-1) == ' ' && q.GetChar(a) == ' ' )
          {
          q.erase ( a , 1 ) ;
          a-- ;
          }
       }
    return pre + "=\"" + q + "\"" ;
    }

void TGenBank::doExport ( TVector *v , wxArrayString &ex )
    {
    ex.Clear () ;
    int a , b ;
    wxString s , t , u ;
    char z[1000] ;
    
    wxString b21 = expand ( "" , 21 ) ;
    
    // Vector
    t = expand ( v->name.c_str() , 24 ) ;
    sprintf ( z , "%d bp" , v->sequence.length() ) ;
    u = expand ( z , 11 ) ;
    s = "LOCUS       " ;
    s += t ;
    s += u ;
    if ( v->type == TYPE_AMINO_ACIDS ) s += "AA      " ;
    else s += "DNA     " ;
    if ( v->isCircular() ) s += "circular " ;
    else s += "linear " ;
    ex.Add ( s ) ;
    
    // Definition
    wrapit ( ex , "DEFINITION  " , v->desc.c_str() ) ;
    s = v->getParams().c_str() ;
    while ( s != "" && s[s.length()-1] == '\n' )
       s = s.substr ( 0 , s.length()-1 ) ;
    ex.Add ( s ) ;
    
    // Features
    ex.Add ( "FEATURES             Location/Qualifiers" ) ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        s = "     " ;
        s += expand ( gb_item_type[v->items[a].type] , 16 ) ;
        if ( v->items[a].direction == 1 )
           sprintf ( z , "%d..%d" , v->items[a].from , v->items[a].to ) ;
        else
           sprintf ( z , "complement(%d..%d)" , v->items[a].from , v->items[a].to ) ;
        ex.Add ( s + z ) ;
        if ( v->items[a].name != "" )
           wrapit ( ex , b21 , quote ( "/gene" , v->items[a].name.c_str() ) ) ;
        if ( v->items[a].desc != "" )
           wrapit ( ex , b21 , quote ( "/note" , v->items[a].desc.c_str() ) ) ;
           
        wxArrayString vs ;
        vector <string> vss ;
        vss = v->items[a].getParamKeys() ;
        for ( b = 0 ; b < vss.size() ; b++ )
           {
           vs.Add ( vss[b].c_str() ) ;
           if ( vs[b].GetChar(0) == '/' )
              wrapit ( ex , b21 , quote ( vs[b] , v->items[a].getParam(vs[b].c_str()).c_str() ) ) ;
           }
        }
    
    // Sequence
    int q[256] ;
    for ( a = 0 ; a < 256 ; a++ ) q[a] = 0 ;
    t = v->sequence.c_str() ;
    for ( a = 0 ; a < t.length() ; a++ )
        {
        b = t.GetChar(a) ;
        if ( b >= 'A' && b <= 'Z' ) b = b - 'A' + 'a' ;
        t.SetChar(a,b) ;
        q[b]++ ;
        }
        
    if ( v->type != TYPE_AMINO_ACIDS )
       {
       s = "BASE COUNT   " ;
       sprintf ( z , "%6d a %6d c %6d g %6d t" , q['a'] , q['c'] , q['g'] , q['t'] ) ;
       ex.Add ( s + z ) ;
       }
    while ( t.length() % 60 != 0 ) t += " " ;
    ex.Add ( "ORIGIN" ) ;
    for ( a = 0 ; a < t.length() ; a += 60 )
        {
        sprintf ( z , "%9d" , a+1 ) ;
        s = z ;
        for ( b = 0 ; b < 6 ; b++ )
           {
           s += " " ;
           s += t.substr ( a + b*10 , 10 ) ;
           }
        ex.Add ( s ) ;
        }
    
    // Finally
    ex.Add ( "//" ) ;
    }
/*
TGenBank::TGenBank ()
    {
    int a ;
    for ( a = 0 ; a < 256 ; a++ ) perm[a] = false ;
    for ( a = 'A' ; a <= 'Z' ; a++ ) perm[a] = true ;
    for ( a = 'a' ; a <= 'z' ; a++ ) perm[a] = true ;
    for ( a = '0' ; a <= '9' ; a++ ) perm[a] = true ;
    perm['_'] = true ;
    perm['-'] = true ;
    perm['*'] = true ;
    perm[39] = true ; // '
    }

void TGenBank::load ( wxString s ) 
    {

//	wxFile f ( s.c_str() , wxFile::read ) ;
//	long l = f.Length() ;
//	char *t = new char [l+15] ;
//	f.Read ( t , l ) ;
//	f.Close() ;
//	parseLines ( t , l ) ;
//	delete t ;

	wxTextFile file ;
    file.Open ( s.c_str() ) ;
	vs.Clear() ;
	wxString str ;
	for ( str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
	 {
	 if ( trim ( str.c_str() ) != "" ) vs.Add ( str.c_str() ) ;
	 }
    if ( trim ( str.c_str() ) != "" ) vs.Add ( str.c_str() ) ;
	parseLines () ;
	}

void TGenBank::paste ( wxString s ) 
    {
	long l = s.length() ;
	char *t ;
	t = new char [l+5] ;
	strcpy ( t , s.c_str() ) ;
	
	parseLines ( t , l ) ;
	delete t ;
	}

void TGenBank::parseLines ( char *t , long l ) 
    {
	int i ;
	char *c ;
//	for ( c = t ; *c ; c++ ) *c = *c == '\r' ? '\n' : *c ;
	vi.clear () ;
//	vs = explode ( '\n' , t ) ;


    t[l] = 0 ;
    char *d ;
    for ( c = d = t ; *c ; c++ )
        {
        if ( *c == '\n' || *c == '\r' || *c == '\m' || *c == 10 )
           {
           *c = 0 ;
           vs.Add ( d ) ;
           d = c+1 ;
           }
        }
    vs.Add ( d ) ;



	for ( i = 0 ; i < vs.GetCount() ; i++ )
	   vi.push_back ( vs[i].length() - trim(vs[i]).length() ) ;
	success = true ;
	if ( vs.GetCount() == 0 ) success = false ;
	else if ( vs[0].length() < 5 ) success = false ;
	else if ( vs[0].substr(0,5) != "LOCUS" ) success = false ;
	if ( success == false ) return ;
	
    TVS temp ;
    TVI temp_i ;
    vs_l.clear () ;
    vi_l.clear () ;
	for ( i = 0 ; i < vs.GetCount() ; i++ )
	   {
	   if ( trim ( vs[i] ) == "//" || ( i+1 == vs.GetCount() && temp.GetCount() > 0 ) )
	       {
	       wxMessageBox ( wxString::Format("%d",temp.GetCount() ) ) ;
	       vs_l.push_back ( temp ) ;
	       vi_l.push_back ( temp_i ) ;
	       temp.Clear () ;
	       temp_i.clear () ;
	       }
       else if ( trim ( vs[i] ) != "" )
           {
           temp.Add ( vs[i] ) ;
           temp_i.push_back ( vi[i] ) ;
           }
	   }
    if ( vs_l.size() == 0 )
        {
        success = false ;
        return ;
        }
	vs = vs_l[0] ;
	vi = vi_l[0] ;
	}
	
void TGenBank::parseLines ( ) 
    {
    int i ;
    vi.clear() ;
	for ( i = 0 ; i < vs.GetCount() ; i++ )
	   vi.push_back ( vs[i].length() - trim(vs[i]).length() ) ;
	success = true ;
	if ( vs.GetCount() == 0 ) success = false ;
	else if ( vs[0].length() < 5 ) success = false ;
	else if ( vs[0].substr(0,5) != "LOCUS" ) success = false ;
	if ( success == false ) return ;
	
    TVS temp ;
    TVI temp_i ;
    vs_l.clear () ;
    vi_l.clear () ;
	for ( i = 0 ; i < vs.GetCount() ; i++ )
	   {
	   if ( trim ( vs[i] ) == "//" )
	       {
    ofstream aout ( "bla.txt" , ios::out ) ;
    for ( i = 0 ; i < temp.GetCount() ; i++ ) aout << vs[i] << endl ;
	       vs_l.push_back ( temp ) ;
	       vi_l.push_back ( temp_i ) ;
	       temp.Clear () ;
	       temp_i.clear () ;
	       }
       else if ( trim ( vs[i] ) != "" )
           {
           temp.Add ( vs[i] ) ;
           temp_i.push_back ( vi[i] ) ;
           }
	   }
    if ( vs_l.size() == 0 )
        {
        success = false ;
        return ;
        }
	vs = vs_l[0] ;
	vi = vi_l[0] ;
	}
	
void TGenBank::makeTree ()
    {
    int a , b ;
    title = description = "" ;
    char z ;
    wxString s ;
    params = "" ;
    keys.Clear () ;
    
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( vi[a] == 21 && vs[a][22] != '/' && vs[a].find ( ".." ) != -1 )
           {
           vs[a] = "     " + vs[a].substr ( 21 , 255 ) ;
           vi[a] = 5 ;
           }
        }
    
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        s = trim ( vs[a] ) ;
        int lk = keys.GetCount()-1 ;
        if ( vi[a] < 5 )
           {
           z = 'D' ;
           for ( b = 0 ; b < s.length() && s[b] != ' ' ; b++ ) ;
           wxString key = trim ( s.substr ( 0 , b ) ) ;
           wxString value = trim ( s.substr ( b ) ) ;
           keys.push_back ( TGenBankKey ( key , value ) ) ;
           for ( b = 0 ; b < key.length() ; b++ )
              if ( key[b] >= 'a' && key[b] <= 'z' ) key[b] = key[b] - 'a' + 'A';
           if ( vi[a] == 0 )
              {
              if ( key == "TITLE" ) title = value ;
              if ( key == "ACCESSION" || key == "PID" || key == "DBSOURCE" ||
                   key == "KEYWORDS" || key == "DEFINITION" )
                 {  
                 if ( description != "" ) description += "\n" ;
                 if ( key != "DEFINITION" ) description += key + " : " ;
                 description += value ;
                 }
              }
           if ( key != "LOCUS" &&
                key != "DEFINITION" &&   
                key != "ORIGIN" &&   
                key != "BASE" &&   
                key != "FEATURES" 
              ) params += vs[a] + "\n" ;
           }
        else if ( vi[a] == 12 )
           {
           keys[lk].value += " " + s ;
           if ( s != "" ) params += vs[a] + "\n" ;
           }
        else if ( vi[a] < 20 )
           {
           z = 'F' ;
           for ( b = 0 ; b < s.length() && s[b] != ' ' ; b++ ) ;
           wxString name = trim ( s.substr ( 0 , b ) ) ;
           wxString value = trim ( s.substr ( b ) ) ;
           keys[lk].features.push_back ( TGenBankFeature ( name , value ) ) ;
           }
        else if ( vi[a] == 21 )
           {
           z = 'B' ;
           int lf = keys[lk].features.size()-1 ;
           
           if ( s[0] == '/' )
              {
              for ( b = 0 ; b < s.length() && s[b] != '=' ; b++ ) ;
              wxString qname = trim ( s.substr ( 1 , b-1 ) ) ;
              wxString qvalue = trim ( s.substr ( b+1 ) ) ;
              keys[lk].features[lf].qname.Add ( qname ) ;
              keys[lk].features[lf].qvalue.Add ( qvalue ) ;
              }
           else
              {
              int lq = keys[lk].features[lf].qvalue.size()-1 ;
              keys[lk].features[lf].qvalue[lq] += s ;
              }
           }
        else
           {
           z = 'L' ;
           }
        
        wxString t = " " ;
        t[0] = z ;
        vs[a] = t + " " + vs[a] ;
        }
    }
    
wxString TGenBank::trim ( wxString s ) // Left only!
    {
    int a ;
    for ( a = 0 ; a < s.length() && ( s[a] == ' ' || s[a] == 8 ) ; a++ ) ;
    s = s.substr ( a ) ;
    return s ;
    }

wxString TGenBank::trimQuotes ( wxString s )
    {
    if ( s[0] == '"' ) s = s.substr ( 1 ) ;
    if ( s[s.length()-1] == '"' ) s = s.substr ( 0 , s.length()-1 ) ;
    return s ;
    }
    
wxArrayString TGenBank::explode ( char p , wxString s )
    {
    int l = 0 , a ;
    wxArrayString vs ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( s[a] == p )
           {
           vs.Add ( s.substr ( l , a-l ) ) ;
           l = a+1 ;
           }
        }
    s = s.substr ( l , a-l ) ;
    if ( s != "" ) 
       vs.Add ( s ) ;
    return vs ;
    }

    	
void TGenBank::remap ( TVector *v )
    {
    int a , k , f ;
    wxString seq ;
    v->setCircular ( false ) ;
    v->setParams ( params ) ;
	makeTree () ;
    for ( k = 0 ; k < keys.GetCount() ; k++ )
        {
        if ( keys[k].key == "ORIGIN" ) // This is the sequence
           {
           for ( f = 0 ; f < keys[k].features.size() ; f++ )
              seq += keys[k].features[f].value ;
           for ( f = k+1 ; f < keys.GetCount() ; f++ )
              seq += keys[f].key + keys[f].value ;
           k = keys.GetCount() ;
           }
        else if ( keys[k].key == "LOCUS" ) // This is the name
           {
           wxArrayString vs ;
           vs = explode ( ' ' , keys[k].value ) ;
           v->name = trim ( vs[0] ) ;
           for ( a = 1 ; a < vs.size() ; a++ )
              {
              if ( vs[a] == "circular" ) v->setCircular ( true ) ;
              if ( vs[a] == "linear" ) v->setCircular ( false ) ;
              }
           }
        else if ( keys[k].key == "FEATURES" ) // This is the items list
           {
           for ( f = 0 ; f < keys[k].features.size() ; f++ )
              {
              if ( keys[k].features[f].name == "CDS" ) scanItem ( v , k , f , VIT_CDS , true ) ;
              if ( keys[k].features[f].name == "gene" ) scanItem ( v , k , f , VIT_GENE , true ) ;
              if ( keys[k].features[f].name == "promoter" ) scanItem ( v , k , f , VIT_PROMOTER ) ;
              if ( keys[k].features[f].name == "rep_origin" ) scanItem ( v , k , f , VIT_REP_ORI ) ;
              if ( keys[k].features[f].name == "terminator" ) scanItem ( v , k , f , VIT_TERMINATOR ) ;
              if ( keys[k].features[f].name == "misc_feature" ) scanItem ( v , k , f , VIT_MISC ) ;
              if ( keys[k].features[f].name == "Protein" ) scanItem ( v , k , f , VIT_MISC ) ;
              if ( keys[k].features[f].name == "Region" ) scanItem ( v , k , f , VIT_MISC ) ;
              }
           }
        }
    
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        if ( v->items[a].type == VIT_CDS )
           {
           for ( k = 0 ; k < v->items.size() ; k++ )
              {
              if ( v->items[k].type == VIT_GENE &&
                   v->items[k].name == v->items[a].name &&
                   v->items[k].from == v->items[a].from &&
                   v->items[k].to == v->items[a].to )
                  v->items[k].setVisible ( false ) ;
              }
           }
        }
    
    for ( a = 0 ; a < seq.length() ; a++ )
        {
        char c = seq[a] ;
        if ( c != ' ' )
           {
           if ( c >= 'a' && c <= 'z' )
              c = c-'a'+'A' ;
           if ( ( c >= 'A' && c <= 'Z' ) || c == '-' )
              v->sequence += c ;
           }
        }
           
    if ( title != "" ) v->name = title ;
    v->desc = description ;
    v->recalcvisual = true ;
    v->recalculateCuts () ;
    }
 
 
void TGenBank::scanItem ( TVector *v , int k , int f , int type , bool descfirst )
    {
    int a ;
    TVectorItem vi ;
    wxArrayString qn , qv ;
    qn = keys[k].features[f].qname ;
    qv = keys[k].features[f].qvalue ;
    vi.type = type ;
    vi.direction = 1 ;
    for ( a = 0 ; a < qn.size() ; a++ )
        {
        if ( qn[a] == "gene" || qn[a] == "label" ) // Name
           {
           vi.name += trimQuotes(qv[a]).c_str() ;
           }
        else if ( qn[a] == "product" || qn[a] == "note" || 
                qn[a] == "region_name" ) // Desc
           {
           vi.desc += trimQuotes(qv[a]).c_str() ;
           vi.desc += "\n" ;
           }
        else
           {
           vi.setParam ( "/"+qn[a] , qv[a] ) ;
           }
        }
    char u[100] ;
    sprintf ( u , "short_itemtype%d" , type ) ;
    if ( descfirst && vi.name == "" && vi.desc.length() < 10 )
       vi.name = vi.desc ;
    if ( vi.name == "" )
        {
        vi.name = txt(u) ;
        wxString d2 = vi.desc.c_str() ;
        int i = d2.find ( "\n" ) ;
        if ( i > -1 ) d2.erase ( i ) ;
        if ( vi.name.length() * 2 >= d2.length() )
           vi.name = d2.c_str() ;
        }
    addItem ( v , &vi , keys[k].features[f].value ) ;
    }

#define IS_NUM(_x) ((_x)>='0'&&(_x)<='9')
    
// vi is just a prototype!
void TGenBank::addItem ( TVector *v , TVectorItem *vi , wxString l , int depth )
    {
    int a , b ;
    wxString l2 = l + "_______________" ; // dummy, ensuring substr length
    if ( IS_NUM(l2[0]) )
        {
        TVectorItem *i = new TVectorItem ;
        *i = *vi ;
        i->from = i->to = 0 ;
        for ( a = 0 ; IS_NUM(l[a]) ; a++ ) i->from = i->from * 10 + ( l[a] - '0') ;
        while ( a < l.length() && l[a] == '.' ) a++ ;
        if ( a == l.length() ) i->to = i->from ;
        while ( a < l.length() ) i->to = i->to * 10 + ( l[a++] - '0') ;
        v->items.push_back ( *i ) ;
        }
    else if ( l2.substr ( 0 , 10 ) == "complement" )
        {
        l = l.substr ( 11 ) ;
        b = 1 ;
        for ( a = 0 ; a < l.length() && b > 0 ; a++ )
           {
           if ( l[a] == '(' ) b++ ;
           if ( l[a] == ')' ) b-- ;
           }
        l2 = l.substr ( 0 , a-1 ) ;
        vi->direction = -vi->direction ;
        addItem ( v , vi , l2 , depth+1 ) ;
        vi->direction = -vi->direction ;
        }
    else if ( l2.substr ( 0 , 10 ) == "join" ) // Needs to be tested !!!!
        {
        l = l.substr ( 5 ) ;
        b = 1 ;
        for ( a = 0 ; a < l.length() && b > 0 ; a++ )
           {
           if ( l[a] == '(' ) b++ ;
           if ( l[a] == ')' ) b-- ;
           if ( b == 1 && l[a] == ',' )
              {
              l2 = l.substr ( 0 , a-1 ) ;
              addItem ( v , vi , l2 , depth+1 ) ;
              l = l.substr ( a+1 ) ;
              a = -1 ;
              }
           }
        }
    }

void TGenBank::showtree ( TVectorTree *z )
    {
    wxTreeItemId d ;
    d = z->AppendItem ( z->treeroot , "GenBank raw data" ) ;
    wxString s ;
    for ( int k = 0 ; k < keys.size() ; k++ )
        {
        s = keys[k].key + "=" + keys[k].value + "\n" ;
        wxTreeItemId _k = z->AppendItem ( d , s.c_str() ) ;
        for ( int f = 0 ; f < keys[k].features.size() ; f++ )
           {
           s = keys[k].features[f].name + "=" + keys[k].features[f].value ;
           wxTreeItemId _f = z->AppendItem ( _k , s.c_str() ) ;
           for ( int x = 0 ; x < keys[k].features[f].qname.size() ; x++ )
              {
              s = keys[k].features[f].qname[x] + ":" + keys[k].features[f].qvalue[x] ;
              wxTreeItemId _x = z->AppendItem ( _f , s.c_str() ) ;
              }
           }
        }
    }
*/
// *************************************************** TFasta

void TFasta::load ( wxString s )
    {/*
    char t[10000] ;
    while ( l.size() ) l.pop_back () ;
    ifstream in ( s.c_str() , ios::in ) ;
    while ( !in.eof() )
        {
        in.getline ( t , sizeof ( t ) ) ;
        if ( *t ) l.push_back ( t ) ;
        }
    success = true ;
    if ( l.size() == 0 ) success = false ;
    else if ( l[0][0] != '>' ) success = false ;*/
    success = false ;
    }
    
void TFasta::remap ( TVector *v )
    {/*
    int a ;
    wxString s , t ;
    for ( a = 1 ; a < l.size() ; a++ )
        s += l[a] ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( s[a] >= 'A' && s[a] <= 'Z' ) t += s[a] ;
        if ( s[a] >= 'a' && s[a] <= 'z' ) t += s[a]-'a'+'A' ;
        }
    v->name = "" ;
    for ( a = 0 ; a < l[0].length() && l[0][a] != ' ' ; a++ )
        v->name += l[0][a] ;
    v->desc = "FASTA\n" ;
    v->desc += l[0].c_str() ;
    v->sequence = t.c_str() ;
    v->setCircular ( false ) ;
    */
    }
    
