#include "GenBank.h"
#include <wx/textfile.h>

#define TAG_COMPLEMENT 1

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
	vs.Clear() ;
 	wxTextFile file ;
    file.Open ( s ) ;
    mylog ( "-GenBank import" , "file opened" ) ;
	vs.Alloc ( file.GetLineCount() ) ;
	wxString str ;
	for ( str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
	   if ( !trim(str).IsEmpty() ) vs.Add ( str ) ;
    if ( !trim(str).IsEmpty() ) vs.Add ( str ) ;
    mylog ( "-GenBank import" , "file added" ) ;
    
    parseLines () ;
    mylog ( "-GenBank import" , "file parsed" ) ;
    }
    
void TGenBank::paste ( wxString s )
    {
    explode ( "\n" , s , vs ) ;
    parseLines () ;
    }

void TGenBank::parseLines ()
    {
    success = false ;
    vi.Clear () ;
    vi_l.clear () ;
    vs_l.clear () ;
    
    if ( vs.IsEmpty () ||
         vs[0].Left ( 5 ) != "LOCUS" )
       return ;

    int a ;
    vi.Alloc ( vs.GetCount() ) ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        vi.Add ( vs[a].Length() - trim(vs[a]).Length() ) ;

    // Pre-processing
    int last ;    
    wxArrayString vs2 ;
    wxArrayInt vi2 ;
    vs2.Alloc ( vs.GetCount() ) ;
    vi2.Alloc ( vi.GetCount() ) ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( vi[a] == 12 || ( vi[a] == 21 && vs[a].GetChar(21) != '/' ) )
           {
           vs2[last] += " " + trim ( vs[a] ) ;
           }
        else
           {
           last = vs2.GetCount() ;
           vs2.Add ( vs[a] ) ;
           vi2.Add ( vi[a] ) ;
           }    
        }
    vs = vs2 ;
    vi = vi2 ;
    vs2.Clear() ;
    vi2.Clear() ;
    

    // Checking if only one sequence, can be handeled faster
    if ( vs.GetCount()-1 == vs.Index ( "//" ) )
       {
       success = true ;
       vs.Remove ( vs.GetCount() - 1 ) ;
       vi.Remove ( vi.GetCount() - 1 ) ;
       for ( a = 0 ; a < vs.GetCount() ; a++ ) vs[a] = trim ( vs[a] ) ;
       vs_l.push_back ( vs ) ;
       vi_l.push_back ( vi ) ;
       return ;
       }    
    
    
    wxArrayString t ;
    wxArrayInt ti ;
    t.Alloc ( vs.GetCount() ) ;
    ti.Alloc ( vs.GetCount() ) ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        if ( vs[a].GetChar(vi[a]) == '/' && vs[a].GetChar(vi[a]+1) == '/' )
           {
           vs_l.push_back ( t ) ;
           vi_l.push_back ( ti ) ;
           t.Clear () ;
           ti.Clear () ;
           }
        else if ( !trim(vs[a]).IsEmpty() ) 
           {
           t.Add ( trim ( vs[a] ) ) ;
           ti.Add ( vi[a] ) ;
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
	wxString ns ;
	
	int a = 0 ;
	bool validseq[256] ;
	for ( a = 0 ; a < 256 ; a++ ) validseq[a] = isValidSequence ( a ) ;
	
	for ( line = 0 ; line < vs.GetCount() ; line++ ) a += (vi[line]==5) ? 1 : 0 ;
	items.reserve ( a ) ;
	
	wxString l , l2 ;
	for ( line = 0 ; line < vs.GetCount() ; line++ )
	 {
	 l = vs[line] ;
	 int i = vi[line] ;
	 if ( i == 0 ) // New main level keyword
	     {
	     l += " " ;
	     k1 = l.BeforeFirst ( ' ' ) . Upper() ;
	     l2 = trim ( l.AfterFirst ( ' ' ) ) ;
	     i += l.Length() - l2.Length() ;
	     l = l2 ;
	     }
     if ( k1 == "LOCUS" )
          {
          v->setName ( l.BeforeFirst ( ' ' ) ) ;
          v->setDescription ( l.AfterFirst ( ' ' ) ) ;
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
             l2 = l.AfterFirst ( ' ' ) ;
             i += l.Length() - l2.Length() ;
             l = l2 ;
             }
          if ( items.size() ) items[items.size()-1].Add ( l ) ;
          }
     else if ( k1 == "ORIGIN" )
          {
          if ( ns.IsEmpty() ) ns.Alloc ( ( vs.GetCount() - line ) * 60 ) ;
          for ( a = 0 ; a < l.length() && !validseq[l.GetChar(a)] ; a++ ) ;
          if ( a < l.length() )
             {
             l2 = l.Mid ( a ) ;
             l2.Replace ( " " , "" ) ;
             ns += l2 ;
             }    
          }
	 }
	 
    v->setSequence ( ns.MakeUpper() ) ;
    for ( a = 0 ; a < items.size() ; a++ ) addItem ( v , items[a] ) ;
	}

bool TGenBank::isValidSequence ( char a )
    {
    if ( a == '-' ) return true ;
    if ( a >= 'A' && a <= 'Z' ) return true ;
    if ( a >= 'a' && a <= 'z' ) return true ;
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
        if ( va[0].CmpNoCase ( gb_item_type[a] ) == 0 )
           {
           i.setType ( a ) ;
           break ;
           }    
        }
       
    // Properties
    for ( a = 1 ; a < va.GetCount() ; a++ )
        {
        if ( va[a].GetChar(0) != '/' ) continue ;
        wxString l = va[a].Mid ( 1 ) ;
        wxString p = l.BeforeFirst ( '=' ) ;
        if ( p.IsEmpty() ) continue ;
        p = trim ( p.Trim ( true ) ) ;
        p = trim ( trimQuotes ( p.Lower() ) ) ;
        wxString v = l.AfterFirst ( '=' ) ;
        v = trim ( v.Trim ( true ) ) ;
        v = trim ( trimQuotes ( v ) ) ;
        if ( p == "standard_name" || p == "gene" || p == "protein_id" )
           i.name = v ;
        else
           {
           if ( p == "product" || p == "organism" || p == "db_xref" ||
                p == "mol_type" || p == "chromosome" )
              {
              if ( !i.desc.IsEmpty() ) i.desc += "\n" ;
              if ( p == "chromosome" || p == "db_xref" )
                 {
                 i.desc += p ;
                 i.desc += " : " ;
                 }
              i.desc += v ;
              }
           p = "/" + p ;
           i.setParam ( p , v ) ;
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
    
void TGenBank::iterateItem ( TVector *v , TVectorItem &i , wxString l , int tag )
    {
    while ( !l.IsEmpty() )
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
           while ( !l.IsEmpty() && ( l.GetChar(0) < '0' || l.GetChar(0) > '9' ) ) l = l.Mid ( 1 ) ;
           for ( a = 0 ; a < l.Length() && l.GetChar(a) >= '0' && l.GetChar(a) <= '9' ; a++ ) ;
           to = l.Left ( a ) ;
           l = l.Mid ( a ) ;
           long ll ;
           from.ToLong ( &ll ) ;
           i.from = ll ;
           to.ToLong ( &ll ) ;
           i.to = ll ;
           if ( ( tag & TAG_COMPLEMENT ) > 0 ) i.setDirection ( -1 ) ;
           v->items.push_back ( i ) ;
           }
        else if ( l.Left ( 10 ) == "COMPLEMENT" )
           {
           l = l.AfterFirst ( '(' ) ;
           int a , b , cnt , num = 0 ;
           char c = ' ' ;
           for ( a = b = cnt = 0 ; a < l.Length() && ( cnt >= 0 || c != ')' ) ; a++ )
              {
              c = l.GetChar ( a ) ;
              if ( c == '(' ) cnt++ ;
              if ( ( c == ')' ) && cnt == 0 )
                 {
                 num++ ;
                 wxString sub = l.Mid ( b , a - b ) ;
                 int tag2 = tag ;
                 if ( ( tag & TAG_COMPLEMENT ) > 0 ) tag2 -= TAG_COMPLEMENT ;
                 else tag2 += TAG_COMPLEMENT ;
                 iterateItem ( v , i , sub , tag2 ) ;
                 b = a+1 ;
                 }
              if ( c == ')' ) cnt-- ;
              }
           l = l.Mid ( a ) ;
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
                 wxString name = i.name ;
                 wxString newname = name + wxString::Format ( " (%d)" , num ) ;
                 wxString sub = l.Mid ( b , a - b ) ;
                 if ( num > 1 ) i.setParam ( "PREDECESSOR" , (name + wxString::Format ( " (%d)" , num-1 )) ) ;
                 if ( c != ')' ) i.setParam ( "SUCCESSOR" , (name + wxString::Format ( " (%d)" , num+1 )) ) ;
                 i.name = newname ;
                 iterateItem ( v , i , sub ) ;
                 b = a+1 ;
                 i.name = name ;
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


wxString TGenBank::trim ( const wxString &s )
	{
    int a ;
    for ( a = 0 ; a < s.length() && ( s.GetChar(a) == ' ' || s.GetChar(a) < 15 ) ; a++ ) ;
	return s.Mid ( a ) ;
	}

wxString TGenBank::trimQuotes ( wxString s )
	{
    if ( s.GetChar(0) == '"' ) s = s.Mid ( 1 ) ;
    if ( s.Right(1) == "\"" ) s = s.Mid ( 0 , s.Length()-1 ) ;
    return s ;
	}

void TGenBank::wrapit ( wxArrayString &ex , wxString init , wxString data , int limit )
    {
    wxString blanks = expand ( "" , init.Length() ) ;
    int allow = limit - blanks.length() ;
    wxString first = init ;
    while ( !data.IsEmpty() )
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
    t = expand ( v->getName() , 24 ) ;
    sprintf ( z , "%d bp" , v->getSequenceLength() ) ;
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
    wrapit ( ex , "DEFINITION  " , v->getDescription() ) ;
    s = v->getParams() ;
    while ( !s.IsEmpty() && s[s.length()-1] == '\n' )
       s.RemoveLast() ;
    ex.Add ( s ) ;
    
    // Features
    ex.Add ( "FEATURES             Location/Qualifiers" ) ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        s = "     " ;
        s += expand ( gb_item_type[v->items[a].getType()] , 16 ) ;
        if ( v->items[a].getDirection() == 1 )
           sprintf ( z , "%d..%d" , v->items[a].from , v->items[a].to ) ;
        else
           sprintf ( z , "complement(%d..%d)" , v->items[a].from , v->items[a].to ) ;
        ex.Add ( s + z ) ;
        if ( !v->items[a].name.IsEmpty() )
           wrapit ( ex , b21 , quote ( "/gene" , v->items[a].name ) ) ;
        if ( !v->items[a].desc.IsEmpty() )
           wrapit ( ex , b21 , quote ( "/note" , v->items[a].desc ) ) ;
           
        wxArrayString vs ;
        wxArrayString vss ;
        vss = v->items[a].getParamKeys() ;
        for ( b = 0 ; b < vss.GetCount() ; b++ )
           {
           vs.Add ( vss[b] ) ;
           if ( vs[b].GetChar(0) == '/' )
              wrapit ( ex , b21 , quote ( vs[b] , v->items[a].getParam(vs[b]) ) ) ;
           }
        }
    
    // Sequence
    int q[256] ;
    for ( a = 0 ; a < 256 ; a++ ) q[a] = 0 ;
    t = v->getSequence() ;
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

