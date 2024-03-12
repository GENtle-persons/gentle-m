#include "TEliteLaChromLogDialog.h"

BEGIN_EVENT_TABLE(TEliteLaChromLogDialog, wxDialog )
    EVT_CHAR_HOOK(TEliteLaChromLogDialog::OnCharHook)
    EVT_LISTBOX(ELL_LB_DATES, TEliteLaChromLogDialog::OnDatesList)
    EVT_LISTBOX(ELL_LB_USERS, TEliteLaChromLogDialog::OnUserList)
    EVT_BUTTON(ELL_B_EXCEL,TEliteLaChromLogDialog::OnExcel)
END_EVENT_TABLE()

TEliteLaChromLogDialog::TEliteLaChromLogDialog ( wxWindow *parent, const wxString& title , const wxString& _basefile )
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 790 , 400 ) )
    {
    col_headers.Clear() ;
    col_headers.Add ( _T("Date") ) ;
    col_headers.Add ( _T("Time") ) ;
    col_headers.Add ( _T("Sample Id") ) ;
    col_headers.Add ( _T("File Name") ) ;
    col_headers.Add ( _T("Method Name") ) ;
    col_headers.Add ( _T("User Name") ) ;
    col_headers.Add ( _T("Vial") ) ;
    col_headers.Add ( _T("Volume") ) ;
    col_headers.Add ( _T("Autosampler") ) ;
    col_headers.Add ( _T("Apex") ) ;
    col_headers.Add ( _T("Area") ) ;
    col_headers.Add ( _T("EstdConc") ) ;


    // Setup data
    wxString basefile = _basefile.BeforeLast ( '.' ) ;
    filename_apex = basefile + _T(".Apex") ;
    filename_area = basefile + _T(".Area") ;
    filename_estdconc = basefile + _T(".ESTDConc") ;

    ReadFile ( filename_apex , data_apex ) ;
    ReadFile ( filename_area , data_area ) ;
    ReadFile ( filename_estdconc , data_estdconc ) ;

    for ( int a = 0 ; a <= ELL_DATE_TIME ; a++ ) unique[a].Clear() ;
    Uniquify ( data_apex , tabs_apex ) ;
    Uniquify ( data_area , tabs_area ) ;
    Uniquify ( data_estdconc , tabs_estdconc ) ;
    for ( int a = 0 ; a <= ELL_DATE_TIME ; a++ ) unique[a].Sort ( (bool) true ) ;

    tabs_merged = tabs_apex ;
    MergeWith ( tabs_area , ELL_DATA2 ) ;
    MergeWith ( tabs_estdconc , ELL_DATA3 ) ;
    SortTabs ( tabs_merged ) ;

    // Now create inteface
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;

    wxButton *b_excel = new wxButton ( this , ELL_B_EXCEL , txt("b_open_excel") ) ;

    unique_dates = new wxListBox ( this , ELL_LB_DATES , wxDefaultPosition , wxDefaultSize, 0 , NULL , wxLB_EXTENDED ) ;
    unique_users = new wxListBox ( this , ELL_LB_USERS , wxDefaultPosition , wxDefaultSize, 0 , NULL , wxLB_EXTENDED ) ;
    lines = new wxListCtrl ( this , -1 , wxDefaultPosition , wxDefaultSize , wxLC_REPORT ) ;

    for ( int a = 0 ; a < col_headers.GetCount() ; a++ )
        {
        lines->InsertColumn (  a , col_headers[a] ) ;
        }

    h0->Add ( unique_dates , 1 , wxEXPAND , 2 ) ;
    h0->Add ( unique_users , 1 , wxEXPAND , 2 ) ;

    h1->Add ( b_excel , 0 , wxEXPAND , 2 ) ;

    v0->Add ( h0 , 1 , wxEXPAND , 2 ) ;
    v0->Add ( lines , 1 , wxEXPAND , 2 ) ;
    v0->Add ( h1 , 0 , wxEXPAND , 2 ) ;

    // Fill interface with data
    unique_users->Append ( txt("t_eld_all_users") ) ;
    for ( int a = 0 ; a < unique[ELL_USER].GetCount() ; a++ )
        {
        unique_users->Append ( unique[ELL_USER][a] ) ;
        }

    unique_dates->Append ( txt("t_eld_all_dates") ) ;
    for ( int a = 0 ; a < unique[ELL_DATE_TIME].GetCount() ; a++ )
        {
        unique_dates->Append ( unique[ELL_DATE_TIME][a] ) ;
        }

    unique_users->SetSelection ( 0 ) ;
    unique_dates->SetSelection ( 0 ) ;

    UpdateLines() ;

    this->SetSizer ( v0 ) ;
    v0->Layout() ;//Fit ( this ) ;
//  Center() ;
    }


void TEliteLaChromLogDialog::ReadFile ( const wxString& filename , wxArrayString& data )
    {
    data.Clear() ;
    if ( !wxFileExists ( filename ) )
        {
	wxPrintf("E: TEliteLaChromLogDialog::ReadFile: File '%s' does not exist.", filename ) ;
        return ; // Too bad...
        }
    wxFile f ( filename ) ;
    long l = f.Length() ;
    char *c = new char[l+5] , *ld ;
    f.Read ( c , l ) ;
    c[l] = '\n' ;
    c[l+1] = 0 ;
    wxCSConv co ( _T("iso-8859-1") ) ;

    for ( char *d = ld = c ; *d ; d++ )
        {
        if ( *d == '\n' )
            {
            *d = 0 ;
            wxString s ( ld , co ) ;
            s.Replace ( _T("\r") , _T("") ) ;
            data.Add ( s ) ;
            ld = d + 1 ;
            }
        }
    delete [] c ;
    }

#define MAKE_DATE_TIME(line) line[0].Mid(6,4) + _T("-") + line[0].Mid(3,2) + _T("-") + line[0].Mid(0,2) + _T(" ") + line[1]

void TEliteLaChromLogDialog::Uniquify ( wxArrayString &data , vector <wxArrayString> &tabs )
    {
    tabs.clear() ;
    for ( int a = 3 ; a < data.GetCount() ; a++ )
        {
        wxString s = data[a] ;
        if ( s.IsEmpty() ) continue ;
        wxArrayString line ;
        while ( !s.IsEmpty() )
        {
            line.Add ( s.BeforeFirst ( (char) 9 ) ) ;
            s = s.AfterFirst ( (char) 9 ) ;
        }
        while ( line.GetCount() < ELL_DATE_TIME ) line.Add ( _T("") ) ;
        s = MAKE_DATE_TIME ( line ) ; // DateTime
        line.Add ( s ) ;
        for ( int b = 0 ; b <= ELL_DATE_TIME ; b++ )
            {
            if ( wxNOT_FOUND == unique[b].Index ( line[b] ) ) unique[b].Add ( line[b] ) ;
            }
        tabs.push_back ( line ) ;
        }
    }

void TEliteLaChromLogDialog::FilterLines ()
    {
    tabs_display = tabs_merged ;

    if ( !unique_dates->IsSelected ( 0 ) ) // If not "All dates"
        {
        wxArrayString dates , times ;
        for ( int a = 1 ; a < unique_dates->GetCount() ; a++ ) // Parse dates/times
            {
            if ( !unique_dates->IsSelected ( a ) ) continue ; // Not selected
            wxString s = unique_dates->GetString ( a ) ;
            wxString d = s.Mid ( 8 , 2 ) + _T(".") + s.Mid ( 5 , 2 ) + _T(".") + s.Left ( 4 ) ;
            wxString t = s.Mid ( 11 ) ;
            dates.Add ( d ) ;
            times.Add ( t ) ;
            }
        vector <wxArrayString> n ;
        for ( int a = 0 ; a < tabs_display.size() ; a++ )
            {
            int b = 0 ;
            while ( b < dates.GetCount() && ( dates[b] != tabs_display[a][0] || times[b] != tabs_display[a][1] ) )
                {
                b++ ;
                }
            if ( b == dates.GetCount() ) continue ; // No matching time/date
            n.push_back ( tabs_display[a] ) ;
            }
        tabs_display = n ;
        }

    if ( !unique_users->IsSelected ( 0 ) ) // If not "All dates"
        {
        wxArrayString users ;
        for ( int a = 1 ; a < unique_users->GetCount() ; a++ ) // Parse dates/times
            {
            if ( !unique_users->IsSelected ( a ) ) continue ; // Not selected
            users.Add ( unique_users->GetString ( a ) ) ;
            }
        vector <wxArrayString> n ;
        for ( int a = 0 ; a < tabs_display.size() ; a++ )
            {
            if ( wxNOT_FOUND == users.Index ( tabs_display[a][ELL_USER] ) ) continue ;
            n.push_back ( tabs_display[a] ) ;
            }
        tabs_display = n ;
        }
    }

void TEliteLaChromLogDialog::UpdateLines ()
    {
    FilterLines() ;
    lines->DeleteAllItems() ;
    for ( int a = 0 ; a < tabs_display.size() ; a++ )
        {
        for ( int b = 0 ; b < tabs_display[a].GetCount() ; b++ )
            {
            if ( b == 0 ) lines->InsertItem ( a , tabs_display[a][b] ) ;
            else lines->SetItem ( a , b , tabs_display[a][b] ) ;
            }
        }
    lines->SetColumnWidth ( ELL_DATE , -2 ) ;
    lines->SetColumnWidth ( ELL_TIME , -2 ) ;
//  lines->SetColumnWidth ( ELL_SAMPLE , -2 ) ;
    for ( int a = 3 ; a < 13 ; a++ ) lines->SetColumnWidth ( a , 70 ) ;
    lines->SetColumnWidth ( ELL_USER , 50 ) ;
    lines->SetColumnWidth ( ELL_VIAL , 30 ) ;
    lines->SetColumnWidth ( ELL_AUTOSAMPLER , 50 ) ;
    }

void TEliteLaChromLogDialog::MergeWith ( vector <wxArrayString> &tabs , int col )
    {
    vector <wxArrayString> nt ;

    for ( int a = 0 ; a < tabs.size() ; a++ )
        {
        for ( int b = 0 ; b < tabs_merged.size() ; b++ )
            {
	    int c = 0 ;
            while (  c < ELL_PROGRAM && tabs[a][c] == tabs_merged[b][c] )
                {
                c++ ;
                }
            if ( c < ELL_PROGRAM ) continue ; // No match
            wxArrayString n = tabs_merged[b] ;
            while ( n.GetCount() <= col ) n.Add ( _T("") ) ;
            n[col] = tabs[a][ELL_DATA1] ;
            nt.push_back ( n ) ;
//          tabs_merged.erase ( b ) ;
            break ;
            }
        }

    tabs_merged = nt ;
    }


void TEliteLaChromLogDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) EndModal ( wxID_CANCEL ) ; //OnCancel ( ev ) ;
//  else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }

void TEliteLaChromLogDialog::OnDatesList(wxCommandEvent &event)
    {
    FilterLines() ;
    UpdateLines() ;
    }

void TEliteLaChromLogDialog::OnUserList(wxCommandEvent &event)
    {
    FilterLines() ;
    UpdateLines() ;
    }

void TEliteLaChromLogDialog::SortTabs ( vector <wxArrayString> &tabs )
    {
    // Sorting
    for ( int a = 1 ; a < tabs.size() ; a++ )
        {
        wxArrayString t1 = tabs[a-1] ;
        wxArrayString t2 = tabs[a] ;
        wxString dt1 = MAKE_DATE_TIME ( t1 ) ;
        wxString dt2 = MAKE_DATE_TIME ( t2 ) ;
        bool swap = false ;
        if ( dt1 > dt2 ) swap = true ;
        else if ( dt1 == dt2 )
        {
            int b = 2 ;
            while ( b < t1.GetCount() && b < t2.GetCount() && t1[b] == t2[b] )
                {
                b++  ;
                }
            if ( b < t1.GetCount() && b < t2.GetCount() ) swap = t1[b] > t2[b] ;
            else if ( b < t1.GetCount() ) swap = true ;
        }
        if ( !swap ) continue ; // In correct order
        tabs[a-1] = t2 ;
        tabs[a] = t1 ;
        a -= 2 ;
        if ( a < 0 ) a = 0 ;
        }

    // Removing doubles, inverting
    vector <wxArrayString> n ;
    for ( int a = 0 ; a < tabs.size() ; a++ )
        {
        if ( a == 0 ) n.push_back ( tabs[a] ) ;
        else
            {
            int b = 0 ; 
            while ( b < tabs[a-1].GetCount() && b < tabs[a].GetCount() && tabs[a-1][b] == tabs[a][b] )
                {
                b++ ;
                }
            if ( b < tabs[a-1].GetCount() || b < tabs[a].GetCount() ) n.push_back ( tabs[a] ) ;
            }
        }
    tabs = n ;
    }

void TEliteLaChromLogDialog::OnExcel(wxCommandEvent &event)
    {
    wxString s , sep = _T("\t") , q = _T("\"") ;

    // Headings
    for ( int a = 0 ; a < col_headers.GetCount() ; a++ )
        {
        if ( a > 0 ) s += sep ;
        s += q + col_headers[a] + q ;
        }
    s += _T("\r\n") ;

    // Data
    for ( int a = 0 ; a < tabs_display.size() ; a++ )
        {
        for ( int b = 0 ; b < tabs_display[a].GetCount() ; b++ )
            {
            if ( b > 0 ) s += sep ;
            wxString t = tabs_display[a][b] ;
            t.Replace ( q , _T("'") ) ;
//            if ( b > ELL_USER ) t.Replace ( _T(".") , _T(",") ) ;
            t = t.Trim().Trim(false) ;
            if ( b >= ELL_DATA1 )
                {
                t = _T("=") + t.BeforeFirst ( ' ' ) ;
                if ( t == _T("=") ) t = _T("=0") ;
                t = t.Trim().Trim(false) ;
                }
            s += q + t + q ;
            }
        s += _T("\r\n") ;
        }

    wxString filename = wxFileName::CreateTempFileName ( _T("") ) + _T(".txt") ;
//  filename = _T("C:\\1.txt") ; // Testing...
    wxFile file ( filename , wxFile::write ) ;
    file.Write ( s , *wxConvCurrent ) ;
    file.Close() ;

//  wxString cmd = myapp()->getFileFormatCommand ( _T("csv") , filename ) ;
    wxString cmd = myapp()->getFileFormatApplication ( _T("xls") ) ;
    cmd += _T(" \"") + filename + q ;
//  wxMessageBox ( cmd ) ;
    wxExecute ( cmd ) ;
//  wxRemoveFile ( filename ) ;
    }
