#include "GenBank.h"

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

void TGenBank::wrapit ( vector <string> &ex , string init , string data , int limit )
    {
    string blanks = expand ( "" , init.length() ) ;
    int allow = limit - blanks.length() ;
    string first = init ;
    while ( data != "" )
        {
        string s , t ;
        s = first ;
        first = blanks ;
        if ( allow > data.length() ) allow = data.length() ;
        t = data.substr ( 0 , allow ) ;
        data = data.substr ( allow ) ;
        s += t ;
        ex.push_back ( s ) ;
        }
    }
    
string TGenBank::expand ( string init , int to , string with )
    {
    while ( init.length() < to ) init += with ;
    return init.substr ( 0 , to ) ;
    }
    
string TGenBank::quote ( string pre , string q )
    {
    int a ;
    for ( a = 0 ; a < q.length() ; a++ )
       {
       if ( q[a] == '\n' || q[a] == '\r' ) q[a] = ' ' ;
       else if ( q[a] == '\"' ) q[a] = '-' ;
       }
    for ( a = 1 ; a < q.length() ; a++ )
       {
       if ( q[a-1] == ' ' && q[a] == ' ' )
          {
          q.erase ( a , 1 ) ;
          a-- ;
          }
       }
    return pre + "=\"" + q + "\"" ;
    }

void TGenBank::doExport ( TVector *v , vector <string> &ex )
    {
    while ( ex.size() ) ex.pop_back () ;
    int a , b ;
    string s , t , u ;
    char z[1000] ;
    
    string b21 = expand ( "" , 21 ) ;
    
    // Vector
    t = expand ( v->name , 24 ) ;
    sprintf ( z , "%d bp" , v->sequence.length() ) ;
    u = expand ( z , 11 ) ;
    s = "LOCUS       " ;
    s += t ;
    s += u ;
    if ( v->type == TYPE_AMINO_ACIDS ) s += "AA      " ;
    else s += "DNA     " ;
    if ( v->isCircular() ) s += "circular " ;
    else s += "linear " ;
    ex.push_back ( s ) ;
    
    // Definition
    wrapit ( ex , "DEFINITION  " , v->desc ) ;
    s = v->getParams() ;
    while ( s != "" && s[s.length()-1] == '\n' )
       s = s.substr ( 0 , s.length()-1 ) ;
    ex.push_back ( s ) ;
    
    // Features
    ex.push_back ( "FEATURES             Location/Qualifiers" ) ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        s = "     " ;
        s += expand ( gb_item_type[v->items[a].type] , 16 ) ;
        if ( v->items[a].direction == 1 )
           sprintf ( z , "%d..%d" , v->items[a].from , v->items[a].to ) ;
        else
           sprintf ( z , "complement(%d..%d)" , v->items[a].from , v->items[a].to ) ;
        ex.push_back ( s + z ) ;
        if ( v->items[a].name != "" )
           wrapit ( ex , b21 , quote ( "/gene" , v->items[a].name.c_str() ) ) ;
        if ( v->items[a].desc != "" )
           wrapit ( ex , b21 , quote ( "/note" , v->items[a].desc.c_str() ) ) ;
           
        vector <string> vs ;
        vs = v->items[a].getParamKeys () ;
        for ( b = 0 ; b < vs.size() ; b++ )
           {
           if ( vs[b][0] == '/' )
              wrapit ( ex , b21 , quote ( vs[b] , v->items[a].getParam(vs[b]).c_str() ) ) ;
           }
        }
    
    // Sequence
    int q[256] ;
    for ( a = 0 ; a < 256 ; a++ ) q[a] = 0 ;
    t = v->sequence ;
    for ( a = 0 ; a < t.length() ; a++ )
        {
        b = t[a] ;
        if ( b >= 'A' && b <= 'Z' ) b = b - 'A' + 'a' ;
        t[a] = b ;
        q[b]++ ;
        }
        
    if ( v->type != TYPE_AMINO_ACIDS )
       {
       s = "BASE COUNT   " ;
       sprintf ( z , "%6d a %6d c %6d g %6d t" , q['a'] , q['c'] , q['g'] , q['t'] ) ;
       ex.push_back ( s + z ) ;
       }
    while ( t.length() % 60 != 0 ) t += " " ;
    ex.push_back ( "ORIGIN" ) ;
    for ( a = 0 ; a < t.length() ; a += 60 )
        {
        sprintf ( z , "%9d" , a+1 ) ;
        s = z ;
        for ( b = 0 ; b < 6 ; b++ )
           {
           s += " " ;
           s += t.substr ( a + b*10 , 10 ) ;
           }
        ex.push_back ( s ) ;
        }
    
    // Finally
    ex.push_back ( "//" ) ;
    }

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

void TGenBank::load ( string s ) 
    {
	wxFile f ( s.c_str() , wxFile::read ) ;
	long l = f.Length() ;
	char *t = new char [l+15] ;
	f.Read ( t , l ) ;
	f.Close() ;
	parseLines ( t , l ) ;
	delete t ;
	}

void TGenBank::paste ( string s ) 
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
	for ( c = t ; *c ; c++ ) *c = *c == '\r' ? '\n' : *c ;
	vi.clear () ;
	vs = explode ( '\n' , t ) ;
	for ( i = 0 ; i < vs.size() ; i++ )
	   vi.push_back ( vs[i].length() - trim(vs[i]).length() ) ;
	success = true ;
	if ( vs.size() == 0 ) success = false ;
	else if ( vs[0].length() < 5 ) success = false ;
	else if ( vs[0].substr(0,5) != "LOCUS" ) success = false ;
	if ( success == false ) return ;
	
    TVS temp ;
    TVI temp_i ;
    vs_l.clear () ;
    vi_l.clear () ;
	for ( i = 0 ; i < vs.size() ; i++ )
	   {
	   if ( vs[i] == "//" )
	       {
	       vs_l.push_back ( temp ) ;
	       vi_l.push_back ( temp_i ) ;
	       temp.clear () ;
	       temp_i.clear () ;
	       }
       else if ( trim ( vs[i] ) != "" )
           {
           temp.push_back ( vs[i] ) ;
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
    string s ;
    params = "" ;
    keys.clear () ;
    
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        if ( vi[a] == 21 && vs[a][22] != '/' && vs[a].find ( ".." ) != -1 )
           {
           vs[a] = "     " + vs[a].substr ( 21 , 255 ) ;
           vi[a] = 5 ;
           }
        }
    
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        s = trim ( vs[a] ) ;
        int lk = keys.size()-1 ;
        if ( vi[a] < 5 )
           {
           z = 'D' ;
           for ( b = 0 ; b < s.length() && s[b] != ' ' ; b++ ) ;
           string key = trim ( s.substr ( 0 , b ) ) ;
           string value = trim ( s.substr ( b ) ) ;
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
           string name = trim ( s.substr ( 0 , b ) ) ;
           string value = trim ( s.substr ( b ) ) ;
           keys[lk].features.push_back ( TGenBankFeature ( name , value ) ) ;
           }
        else if ( vi[a] == 21 )
           {
           z = 'B' ;
           int lf = keys[lk].features.size()-1 ;
           
           if ( s[0] == '/' )
              {
              for ( b = 0 ; b < s.length() && s[b] != '=' ; b++ ) ;
              string qname = trim ( s.substr ( 1 , b-1 ) ) ;
              string qvalue = trim ( s.substr ( b+1 ) ) ;
              keys[lk].features[lf].qname.push_back ( qname ) ;
              keys[lk].features[lf].qvalue.push_back ( qvalue ) ;
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
        
        string t = " " ;
        t[0] = z ;
        vs[a] = t + " " + vs[a] ;
        }
    }
    
string TGenBank::trim ( string s ) // Left only!
    {
    int a ;
    for ( a = 0 ; a < s.length() && ( s[a] == ' ' || s[a] == 8 ) ; a++ ) ;
    s = s.substr ( a ) ;
    return s ;
    }

string TGenBank::trimQuotes ( string s )
    {
    if ( s[0] == '"' ) s = s.substr ( 1 ) ;
    if ( s[s.length()-1] == '"' ) s = s.substr ( 0 , s.length()-1 ) ;
    return s ;
    }
    
vector <string> TGenBank::explode ( char p , string s )
    {
    int l = 0 , a ;
    vector <string> vs ;
    for ( a = 0 ; a < s.length() ; a++ )
        {
        if ( s[a] == p )
           {
           vs.push_back ( s.substr ( l , a-l ) ) ;
           l = a+1 ;
           }
        }
    s = s.substr ( l , a-l ) ;
    if ( s != "" ) 
       vs.push_back ( s ) ;
    return vs ;
    }

    	
void TGenBank::remap ( TVector *v )
    {
    int a , k , f ;
    string seq ;
    v->setCircular ( false ) ;
    v->setParams ( params ) ;
	makeTree () ;
    for ( k = 0 ; k < keys.size() ; k++ )
        {
        if ( keys[k].key == "ORIGIN" ) // This is the sequence
           {
           for ( f = 0 ; f < keys[k].features.size() ; f++ )
              seq += keys[k].features[f].value ;
           for ( f = k+1 ; f < keys.size() ; f++ )
              seq += keys[f].key + keys[f].value ;
           k = keys.size() ;
           }
        else if ( keys[k].key == "LOCUS" ) // This is the name
           {
           vector <string> vs ;
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
    vector <string> qn , qv ;
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
        string d2 = vi.desc.c_str() ;
        int i = d2.find ( "\n" ) ;
        if ( i > -1 ) d2.erase ( i ) ;
        if ( vi.name.length() * 2 >= d2.length() )
           vi.name = d2.c_str() ;
        }
    addItem ( v , &vi , keys[k].features[f].value ) ;
    }

#define IS_NUM(_x) ((_x)>='0'&&(_x)<='9')
    
// vi is just a prototype!
void TGenBank::addItem ( TVector *v , TVectorItem *vi , string l , int depth )
    {
    int a , b ;
    string l2 = l + "_______________" ; // dummy, ensuring substr length
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
    string s ;
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

// *************************************************** TFasta

void TFasta::load ( string s )
    {
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
    else if ( l[0][0] != '>' ) success = false ;
    }
    
void TFasta::remap ( TVector *v )
    {
    int a ;
    string s , t ;
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
    }
    
