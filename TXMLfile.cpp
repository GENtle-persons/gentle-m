/** \file
    \brief The TXMLfile class
*/
#include "TXMLfile.h"

TXMLfile::TXMLfile ()
    {
    TiXmlBase::SetCondenseWhiteSpace ( true ) ;
    _success = false ;
    }

void TXMLfile::parse ( const wxString& text )
    {
    TiXmlDocument doc ;
    doc.Parse ( text.mb_str() ) ;
    if ( doc.Error() )
        {
        _success = false ;
        return ;
        }
    analyze ( doc ) ;
    }

void TXMLfile::load ( const wxString& file )
    {
//   wxStartTimer() ;
    TiXmlDocument doc ( file.mb_str() );
    bool ok = doc.LoadFile();
/*
     // Alternative (faster?) opening method
     char *x ;
     wxFile f ( file ) ;
     if ( !f.IsOpened() ) { _success = false ; return ; }
     x = new char [ f.Length() + 5 ] ;
     f.Read ( x , f.Length() ) ;
     x[f.Length()] = 0 ;
     if ( *x != '<' ) { delete x ; _success = false ; return ; }
     TiXmlDocument doc ( "" );
     doc.Parse ( (const char*) x ) ;
     bool ok = !doc.Error() ;
     delete x ;
*/
//     cout << "Done in " << wxGetElapsedTime() << " ms\n" ;
    if ( !ok || doc.Error() )
        {
        _success = false ;
        return ;
        }

    analyze ( doc ) ;
    }

void TXMLfile::analyze ( TiXmlDocument &doc )
    {
    if ( doc.FirstChild ( "GBSeq" ) )
        {
        readGBSeq ( doc.FirstChild ( "GBSeq" ) ) ;
        }
    else if ( doc.FirstChild ( "GBSet" ) )
        {
        TiXmlNode *x = doc.RootElement() ;// ( _T("GBSet") ) ;
        for ( x = x->FirstChild ( "GBSeq" ) ; x ; x = x->NextSibling ( "GBSeq" ) )
            {
            readGBSeq ( x ) ;
            }
        }
    _success = _v.GetCount() > 0 ;
    }

int TXMLfile::countVectors () const
    {
    return _v.GetCount() ;
    }

TVector *TXMLfile::getVector ( const int a ) const
    {
    return _v[a] ;
    }


// private

wxString TXMLfile::t ( const char * const txt ) const
    {
    return txt ? wxString ( txt , wxConvUTF8 ) : _T("") ;
    }

wxString TXMLfile::t ( TiXmlText * const txt ) const
    {
    return txt && txt->Value() ? wxString ( txt->Value() , wxConvUTF8 ) : wxString() ;
    }

void TXMLfile::readGBSeq ( TiXmlNode *base )
    {
    myass ( base , _T("TXMLfile::readGBSeq") ) ;
    TVector *v = new TVector ;
    v->setType ( TYPE_VECTOR ) ; // Dummy

    wxString s ;
    TiXmlHandle h ( base ) ;

    wxString n1 = t ( h.FirstChild("GBSeq_definition").FirstChild().Text() ) ;
    wxString n2 = t ( h.FirstChild("GBSeq_locus").FirstChild().Text() ) ;
    if ( !n1.IsEmpty() && n1.Freq ( '\n' ) == 0 && n2.Freq ( ' ' ) == 0 )
        {
        n2 = n1 + _T("(") + n2 + _T("))") ;
        }

    v->setName ( n2 ) ;
    v->setSequence ( t ( h.FirstChild("GBSeq_sequence").FirstChild().Text() ) . MakeUpper() ) ;

    TiXmlElement *e = h.FirstChild("GBSeq_topology").Element() ;
    if ( e ) // topology
        {
        s = t ( e->Attribute ( "value" ) ) . MakeUpper() ;
        if ( s == _T("CIRCULAR") ) v->setCircular ( true ) ;
        }
//  cout << "Up to features took " << wxGetElapsedTime() << " ms\n" ;

    // Features
    for ( TiXmlNode *n = h.FirstChild("GBSeq_feature-table").FirstChild("GBFeature").Node() ; n ; n = n->NextSibling("GBFeature") )
        {
        TVectorItem i ;
        TiXmlHandle h2 ( n ) ;
        s = t ( h2.FirstChild("GBFeature_key").FirstChild().Text() ) . MakeUpper() ;

        // Dummy values
        i.name = _T("") ;
        i.from = 1 ;
        i.to = 1 ;
        i.setType ( 0 ) ;
        i.setDirection ( 1 ) ;
        i.setType ( s ) ;
        readGBqualifiers ( i , n ) ;

        vector <TVectorItem> vi ;
        readGBintervals ( vi , i , n ) ;
        for ( int a = 0 ; a < vi.size() ; a++ ) v->items.push_back ( vi[a] ) ;
        }
//     cout << "Features took " << wxGetElapsedTime() << " ms\n" ;

    // The rest
    for ( TiXmlNode *n = h.Node()->FirstChild() ; n ; n = n->NextSibling() )
        {
        wxString x = t ( n->Value() ) ;
        if ( x == _T("GBSeq_definition") ) continue ;
        if ( x == _T("GBSeq_locus") ) continue ;
        if ( x == _T("GBSeq_sequence") ) continue ;
        if ( x == _T("GBSeq_feature-table") ) continue ;
        x = get_sub_desc ( n ) ;
        v->addDescription ( x ) ;
        }

    // Sorting by size, just for fun
    // Only for less than 100 items
    for ( int a = 1 ; v->items.size() < 100 && a < v->items.size() ; a++ )
        {
        if ( v->getItemLength(a-1) < v->getItemLength(a) )
            {
            TVectorItem i = v->items[a] ;
            v->items[a] = v->items[a-1] ;
            v->items[a-1] = i ;
            a = 0 ;
            }
        }

    v->updateDisplay() ;
    v->recalculateCuts () ;
    _v.Add ( v ) ;
//     cout << "The rest took " << wxGetElapsedTime() << " ms\n" ;
    }

wxString TXMLfile::get_sub_desc ( /* not const */ TiXmlNode * const base , const int depth ) const
    {
    if ( !base ) return _T("") ; // Just fore safety...
    wxString name = t ( base->Value() ) ;
    if ( name.Left ( 6 ) == _T("GBSeq_") ) name = name.Mid(6,1).Upper() + name.Mid ( 7 ) ;
    wxString ret ;
    ret = wxString ( (wxChar)' ' , depth * 2 ) ;
    ret += name ;
    ret += _T("\n") ;
    TiXmlNode *n ;
    for ( n = /* not const */ base->FirstChild() ; n ; n = n->NextSibling() )
        ret += get_sub_desc ( n , depth + 1 ) ;
    return ret ;
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
        if ( name.IsEmpty() ) continue ;
        i.setParam ( _T("/") + name , value ) ;
        if ( name == _T("CODON_START") ) i.setRF ( atoi ( value.mb_str() ) ) ;
        if ( name == _T("GENE") || name == _T("LABEL") ) i.name = value ;
        if ( name == _T("PRODUCT") || name == _T("NOTE") || name == _T("REGION_NAME") )
           {
           if ( !i.desc.IsEmpty() ) i.desc += _T("\n") ;
           i.desc += value ;
           }
        }
    if ( i.name.IsEmpty() && i.desc.length() < 10 )
        {
        i.name = i.desc ;
        i.desc = _T("") ;
        }
    if ( i.name.IsEmpty() )
        {
        wxString try2 = i.desc ;
        while ( try2.Right ( 1 ) == _T("\n") )
            {
            try2 = try2.Left ( try2.length() - 1 ) ;
            }
        try2 = try2.AfterLast('\n').AfterLast('\r') ;
        if ( !try2.IsEmpty() && try2.length() < 20 ) i.name = try2 ;
        }
    if ( i.name.IsEmpty() )
        {
        char u[100+42] ;
        snprintf ( u , sizeof(u)-1, "short_itemtype%d" , i.getType() ) ;
        i.name = txt(u);
        wxString d2 = i.desc ;
        int k = d2.find ( _T("\n") ) ;
        if ( k > -1 ) d2.erase ( k ) ;
        if ( i.name.length() * 2 >= d2.length() )
            i.name = d2 ;
        }
    }

void TXMLfile::readGBintervals ( vector <TVectorItem> &vi , const TVectorItem &i , TiXmlNode * const n )
    {
    TiXmlNode *n2 = n->FirstChild("GBFeature_intervals") ;
    if ( !n2 ) return ;
    for ( n2 = n2->FirstChild("GBInterval") ; n2 ; n2 = n2->NextSibling("GBInterval") )
        {
        TiXmlHandle h ( n2 ) ;
        wxString from = t ( h.FirstChild("GBInterval_from").FirstChild().Text() ) ;
        wxString to = t ( h.FirstChild("GBInterval_to").FirstChild().Text() ) ;
        if ( from.IsEmpty() || to.IsEmpty() ) continue ;
        TVectorItem j = i ;
        j.from = atoi ( from.mb_str() ) ;
        j.to = atoi ( to.mb_str() ) ;
        if ( j.from > j.to )
            {
            const int o = j.from ;
            j.from = j.to ;
            j.to = o ;
            j.setDirection ( -1 ) ;
            }
        vi.push_back ( j ) ;
        }
    }

