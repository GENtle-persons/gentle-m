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

// ===
// declarations
// ===

// ---
// headers
// ---

// For compilers that support precompilation, includes "wx/wx.h".


#include "util.h"
#include "MyApp-class.h"
#include "myapp.h"

#include <wx/tipdlg.h>
#include <wx/splash.h>
#include <wx/filesys.h>
#include <wx/file.h>

#ifdef __WXMSW__
#include "wx/msw/registry.h"
#endif


using namespace std ;
#include <wx/arrimpl.cpp> // required for WX_DEFINE_OBJARRAY implementation
WX_DEFINE_OBJARRAY(wxArrayFloat);

// GLOBAL FUNCTIONS
int cmpint(/* const */ int * const first, /* const */ int * const second) // not const for compatibility with CMPFUNC
    {
    return (*first) > (*second) ;
    }

int cmpre(/*const*/ TRestrictionEnzyme * const first, /* const */ TRestrictionEnzyme * const second)
    {
    return first > second ; //????
    }

void explode ( const wxString& sep , const wxString& s , wxArrayString &r )
    {
    wxString n ;
    r.Clear () ;
    for ( int a = 0 ; a + sep.Length() <= s.Length() ; a++ )
        {
	int b ;
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

wxString implode ( const wxString& sep , wxArrayString &r )
    {
    if ( r.GetCount() == 0 ) return _T("") ;
    wxString ret = r[0] ;
    for ( int a = 1 ; a < r.GetCount() ; a++ )
        ret += sep + r[a] ;
    return ret ;
    }

// END GLOBAL FUNCTIONS

