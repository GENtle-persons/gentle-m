/** \file util.cpp
	\brief Contains global functions, includes
*/

/////////////////////////////////////////////////////////////////////////////
// Name:        GENtle
// Purpose:     DNA/AA manipulation
// Author:      Magnus Manske
// Modified by:
// Created:     2002
// Copyright:   (c) Magnus Manske
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".


#include "main.h"
#include <wx/tipdlg.h>
#include <wx/splash.h>
#include <wx/filesys.h>
#include <wx/file.h>

#ifdef __WXMSW__
#include "wx/msw/registry.h"
#endif


using namespace std ;

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxArrayFloat);

// GLOBAL FUNCTIONS
int cmpint(int *first, int *second)
    {
    return *first > *second ;
    }

int cmpre(TRestrictionEnzyme *first, TRestrictionEnzyme *second)
    {
    return first > second ; //????
    }

void explode ( wxString sep , wxString s , wxArrayString &r )
    {
    int a , b ;
    wxString n ;
    r.Clear () ;
    for ( a = 0 ; a + sep.Length() <= s.Length() ; a++ )
        {
        for ( b = 0 ; b < sep.Length() && s.GetChar(a+b) == sep.GetChar(b) ; b++ ) ;
        if ( b == sep.Length() )
           {
           r.Add ( n ) ;
           n = _T("") ;
           }
        else n += s.GetChar(a) ;
        }
    if ( !n.IsEmpty() ) r.Add ( n ) ;
    }

wxString implode ( wxString sep , wxArrayString &r )
    {
    if ( r.GetCount() == 0 ) return _T("") ;
    wxString ret = r[0] ;
    for ( int a = 1 ; a < r.GetCount() ; a++ )
        ret += sep + r[a] ;
    return ret ;
    }

const wxString txt ( const char * const item )
    {
    return txt ( wxString(item,wxConvUTF8) ) ;
    }

const wxString txt ( wxString item )
    {
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

// END GLOBAL FUNCTIONS

