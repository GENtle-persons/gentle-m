

// Multilanguage interface

void init_txt () ;



class Ttext

    {

    public :

    Ttext () { dummy = "Undefined text entry" ; }

    int size () { return key.size() ; }

    void add ( string k , string v )

        {

        key.push_back ( k ) ;

        value.push_back ( v ) ;

        } ;

    char *getptr ( string k )

        {

        wxString k2 = k.c_str() ;

        for ( int a = 0 ; a < key.size() ; a++ )

           if ( 0 == k2.CmpNoCase ( key[a].c_str() ) )

              return (char*) value[a].c_str() ;

        string r = dummy + " \"" + k + "\"" ;

        return (char*) r.c_str() ;

        } ;

    string & operator [] ( const string &s )

        {

        key.push_back ( s ) ;

        value.push_back ( "" ) ;

        return value[value.size()-1] ;

        }

    vector <string> key , value ;

    string dummy ;

    } ;

    

Ttext _text ;



void init_txt ( string lang )

    {

    ifstream in ( "variables.csv" , ios::in ) ;

    char t[10000] ;

    bool firstline = true ;

    int ln = 1 ; // English is default

    TGenBank dummy ;

    while ( !in.eof() )

        {

        vector <string> v ;

        in.getline ( t , sizeof ( t ) ) ;

        if ( *t == 0 ) break ;

        char *c , *l ;

        bool quote = false ;

        for ( c = l = t ; *c ; c++ )

           {

           if ( *c == '\\' )

              {

              *c = ' ' ;

              c++ ;

              if ( *c == 't' ) *c = '\t' ;

              else if ( *c == '"' ) *c = '"' ;

              else if ( *c == 'n' ) *c = '\n' ;

              }

           else if ( *c == '"' ) quote = !quote ;

           else if ( *c == ',' && !quote )

              {

              *c = 0 ;

              v.push_back ( dummy.trimQuotes ( l ) ) ;

              l = c+1 ;

              }

           }

        if ( l < c )

           {

           *(c-1) = 0 ;

           v.push_back ( dummy.trimQuotes ( l ) ) ;

           }

        

        if ( firstline )

           {

           for ( int a = 0 ; a < v.size() ; a++ )

              if ( v[a] == lang )

                 ln = a ;

           }

        else

           {

           _text[v[0].c_str()] = v[ln] ;

           }

        firstline = false ;

        }

    }



char * txt ( string item )

    {

    return _text.getptr(item) ;

    }



