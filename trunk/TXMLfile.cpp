#include "TXMLfile.h"

TXMLfile::TXMLfile ()
    {
    TiXmlBase::SetCondenseWhiteSpace ( true ) ;
    _success = false ;
    }
    
void TXMLfile::parse ( wxString text )
    {
    TiXmlDocument doc ;
    doc.Parse ( text.c_str() ) ;
    if ( doc.Error() )
        {
        _success = false ;
        return ;
        }
    analyze ( doc ) ;   
    }
    
void TXMLfile::load ( wxString file ) 
    {
    TiXmlDocument doc( file.c_str() );
    bool ok = doc.LoadFile();
    if ( !ok || doc.Error() )
        {
        _success = false ;
        return ;
        }
    analyze ( doc ) ;   
    }

void TXMLfile::analyze ( const TiXmlDocument &doc )
    {
    if ( doc.FirstChild ( "GBSeq" ) )
        {
        readGBSeq ( doc.FirstChild ( "GBSeq" ) ) ;
        }
    else if ( doc.FirstChild ( "GBSet" ) )
        {
        TiXmlNode *x = doc.RootElement() ;// ( "GBSet" ) ;
        for ( x = x->FirstChild ( "GBSeq" ) ; x ; x = x->NextSibling ( "GBSeq" ) )
           readGBSeq ( x ) ;
        }
    
    _success = _v.size() > 0 ;
    }
    
int TXMLfile::countVectors ()
    {
    return _v.size() ;
    }
    
TVector *TXMLfile::getVector ( int a )
    {
    return _v[a] ;
    }
    

// private

wxString TXMLfile::t ( const char *txt )
    {
    return txt ? txt : "" ;
    }

wxString TXMLfile::t ( TiXmlText *txt )
    {
    return txt && txt->Value() ? txt->Value() : "" ;
    }

void TXMLfile::readGBSeq ( TiXmlNode *base )
    {
    myass ( base , "TXMLfile::readGBSeq" ) ;
    TVector *v = new TVector ;
    v->type = TYPE_VECTOR ; // Dummy
    
    int a ;
    TiXmlNode *n ;
    TiXmlElement *e ;
    wxString s ;
    TiXmlHandle h ( base ) ;
    v->setName ( t ( h.FirstChild("GBSeq_locus").FirstChild().Text() ) ) ;
    v->setSequence ( t ( h.FirstChild("GBSeq_sequence").FirstChild().Text() ) . MakeUpper() ) ;
    
    e = h.FirstChild("GBSeq_topology").Element() ;
    if ( e ) // topology
        {
        s = t ( e->Attribute ( "value" ) ) . MakeUpper() ;
        if ( s == "CIRCULAR" ) v->setCircular ( true ) ;
        }

    // Features
    for ( n = h.FirstChild("GBSeq_feature-table").FirstChild("GBFeature").Node() ; n ; 
            n = n->NextSibling("GBFeature") )
        {
        TVectorItem i ;
        TiXmlHandle h2 ( n ) ;
        s = t ( h2.FirstChild("GBFeature_key").FirstChild().Text() ) . MakeUpper() ;
        
        // Dummy values
        i.name = "" ;
        i.from = 1 ;
        i.to = 1 ;
        i.type = 0 ;
        i.direction = 1 ;
        i.setType ( s ) ;
        readGBqualifiers ( i , n ) ;
        
        vector <TVectorItem> vi ;
        readGBintervals ( vi , i , n ) ;
        for ( a = 0 ; a < vi.size() ; a++ )
           v->items.push_back ( vi[a] ) ;
        }
        
    // Sorting by size, just for fun
    for ( a = 1 ; a < v->items.size() ; a++ )
        {
        if ( v->getItemLength(a-1) < v->getItemLength(a) )
           {
           TVectorItem i = v->items[a] ;
           v->items[a] = v->items[a-1] ;
           v->items[a-1] = i ;
           a = 0 ;
           }
        }
        
    v->recalcvisual = true ;
    v->recalculateCuts () ;
    _v.push_back ( v ) ;
    }
    
void TXMLfile::readGBqualifiers ( TVectorItem &i , TiXmlNode *n )
    {
    TiXmlNode *n2 = n->FirstChild("GBFeature_quals") ;
    if ( !n2 ) return ;    
    for ( n2 = n2->FirstChild("GBQualifier") ; n2 ; n2 = n2->NextSibling("GBQualifier") )
        {
        TiXmlHandle h ( n2 ) ;
        wxString name = t ( h.FirstChild("GBQualifier_name").FirstChild().Text() ) . MakeUpper() ;
        wxString value = t ( h.FirstChild("GBQualifier_value").FirstChild().Text() ) ;
        if ( name == "" ) continue ;
        i.setParam ( "/" + name , value ) ;
        if ( name == "CODON_START" ) i.setRF ( atoi ( value.c_str() ) ) ;
        if ( name == "GENE" || name == "LABEL" ) i.name = value.c_str() ;
        if ( name == "PRODUCT" || name == "NOTE" || name == "REGION_NAME" )
           {
           if ( i.desc != "" ) i.desc += "\n" ;
           i.desc += value ;
           }
        }
    if ( i.name == "" && i.desc.length() < 10 )
        {
        i.name = i.desc ;
        i.desc = "" ;
        }
    if ( i.name == "" )
        {
        char u[100] ;
        sprintf ( u , "short_itemtype%d" , i.type ) ;
        i.name = txt(u) ;
        wxString d2 = i.desc.c_str() ;
        int k = d2.find ( "\n" ) ;
        if ( k > -1 ) d2.erase ( k ) ;
        if ( i.name.length() * 2 >= d2.length() )
           i.name = d2.c_str() ;
        }
    }
    
void TXMLfile::readGBintervals ( vector <TVectorItem> &vi , const TVectorItem &i , TiXmlNode *n )
    {
    TiXmlNode *n2 = n->FirstChild("GBFeature_intervals") ;
    if ( !n2 ) return ;
    for ( n2 = n2->FirstChild("GBInterval") ; n2 ; n2 = n2->NextSibling("GBInterval") )
        {
        TiXmlHandle h ( n2 ) ;
        wxString from = t ( h.FirstChild("GBInterval_from").FirstChild().Text() ) ;
        wxString to = t ( h.FirstChild("GBInterval_to").FirstChild().Text() ) ;
        if ( from == "" || to == "" ) continue ;
        TVectorItem j = i ;
        j.from = atoi ( from.c_str() ) ;
        j.to = atoi ( to.c_str() ) ;
        if ( j.from > j.to )
           {
           int o = j.from ;
           j.from = j.to ;
           j.to = o ;
           j.direction = -1 ;
           }
        vi.push_back ( j ) ;
        }
    }
    
