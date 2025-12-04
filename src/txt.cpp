#include <wx/wx.h>
#include "myapp.h"
#include "MyApp-class.h"

const wxString txt_internal ( wxString& item ) ;

const wxString txt ( const char * const item )
    {
    wxString s(item,wxConvUTF8) ;
    return txt_internal ( s ) ;
    }


const wxString txt ( const wxString& item )
    {
        wxString s(item) ;
        return txt_internal(s) ;
    }

/* const wxString txt ( wxString& item )
    {
        wxString s(item) ;
        return txt_internal(s) ;
    }
        */

const wxString txt_internal ( wxString& item )
    {
        //wxPrintf("txt_internal lookup for: %s\n", item) ;
#ifndef __WXMSW__
    if ( item.MakeUpper().Left(2) == _T("M_") )
        {
        wxString s = myapp()->_text[item.MakeUpper()].Trim() ;
#ifdef __WXMAC__
        s.Replace ( _T("\tStrg-") , _T("\tCtrl-") ) ; // DE fix
#endif
        return s ;
        }
#endif
    return myapp()->_text[item.MakeUpper()] ;

    }