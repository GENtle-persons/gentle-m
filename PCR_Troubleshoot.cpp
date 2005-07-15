#include "PCR_Troubleshoot.h"

// WHILE DEVELOPING
void TPrimerDesign::OnTroubleshoot ( wxCommandEvent &ev )
	{
	PCR_troubleshoot_dialog td ( this , "TEST" ) ;
	td.ShowModal () ;
	}
// WHILE DEVELOPING


BEGIN_EVENT_TABLE(PCR_troubleshoot_dialog, wxDialog )
    EVT_BUTTON(POD_OK,PCR_troubleshoot_dialog::OnOK)
    EVT_BUTTON(POD_CANCEL,PCR_troubleshoot_dialog::OnCancel)
    EVT_CHAR_HOOK(PCR_troubleshoot_dialog::OnCharHook)
    EVT_LISTBOX(PCR_TROUBLESHOOT_LIST,PCR_troubleshoot_dialog::OnList)
END_EVENT_TABLE()

vector <TSantaLucia> PCR_troubleshoot_dialog::santa_lucia = vector <TSantaLucia> () ;

PCR_troubleshoot_dialog::PCR_troubleshoot_dialog(TPrimerDesign *_parent, const wxString& title )
	: wxDialog ( (wxWindow*) _parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 500 ) )
	{
	parent = _parent ;
	fillSantaLucia () ;
	
	wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
	
	list = new wxListBox ( this , PCR_TROUBLESHOOT_LIST ) ;
	text = new wxTextCtrl ( this , -1 , _T("") , wxDefaultPosition , wxDefaultSize , wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL ) ;
	text->SetFont ( *MYFONT ( MYFONTSIZE , wxMODERN , wxNORMAL , wxNORMAL ) ) ;
	
	v0->Add ( list , 0 , wxEXPAND|wxALL , 5 ) ;
	v0->Add ( text , 1 , wxEXPAND|wxALL , 5 ) ;
	
	scan () ;
	for ( int a = 0 ; a < l_title.GetCount() ; a++ )
		list->Append ( l_title[a] ) ;
	if ( l_title.GetCount() > 0 ) show_item ( 0 ) ;

	SetSizer ( v0 ) ;
	v0->Layout () ;
	}
	
void PCR_troubleshoot_dialog::fillSantaLucia ()
	{
	if ( santa_lucia.size() > 0 ) return ; // Already done ;
	santa_lucia.push_back ( TSantaLucia ( "AA/TT" , -1.00 , - 7.9 , -22.2 ) ) ; // AA
	santa_lucia.push_back ( TSantaLucia ( "AT/TA" , -0.88 , - 7.2 , -20.4 ) ) ; // AT
	santa_lucia.push_back ( TSantaLucia ( "TA/AT" , -0.58 , - 7.2 , -21.3 ) ) ; // TA
	santa_lucia.push_back ( TSantaLucia ( "CA/GT" , -1.45 , - 8.5 , -22.7 ) ) ; // CA
	santa_lucia.push_back ( TSantaLucia ( "GT/CA" , -1.44 , - 8.4 , -22.4 ) ) ; // GT
	santa_lucia.push_back ( TSantaLucia ( "CT/GA" , -1.28 , - 7.8 , -21.0 ) ) ; // CT
	santa_lucia.push_back ( TSantaLucia ( "GA/CT" , -1.30 , - 8.2 , -22.2 ) ) ; // GA
	santa_lucia.push_back ( TSantaLucia ( "CG/GC" , -2.17 , -10.6 , -27.2 ) ) ; // CG
	santa_lucia.push_back ( TSantaLucia ( "GC/CG" , -2.24 , - 9.8 , -24.4 ) ) ; // GC
	santa_lucia.push_back ( TSantaLucia ( "GG/CC" , -1.84 , - 8.0 , -19.9 ) ) ; // GG
	
	// I guess...
	santa_lucia.push_back ( TSantaLucia ( "TT/AA" , -1.00 , - 7.9 , -22.2 ) ) ; // TT
	santa_lucia.push_back ( TSantaLucia ( "CC/GG" , -1.84 , - 8.0 , -19.9 ) ) ; // CC
	santa_lucia.push_back ( TSantaLucia ( "TG/AC" , -1.44 , - 8.4 , -22.4 ) ) ; // TG
	santa_lucia.push_back ( TSantaLucia ( "TC/AG" , -1.28 , - 7.8 , -21.0 ) ) ; // TC
	}

double PCR_troubleshoot_dialog::deltaG0 ( TPrimer &p )
	{
	int a ;
	double ret = 0 ;
	wxString s = p.get53sequence() ;
	for ( a = 0 ; a+1 < s.length() ; a++ )
		{
		int i = getSLindex ( s.Mid ( a , 2 ) ) ;
		if ( i >= santa_lucia.size() ) continue ; // Ignore this one
		ret += santa_lucia[i].unifiedFE ;
		}
	
	s = s.Right ( 2 ) ; // This is the end, my friend
	if ( s.Left ( 1 ) == "C" || s.Left ( 1 ) == "G" ) ret += 0.98 ;
	if ( s.Right ( 1 ) == "C" || s.Right ( 1 ) == "G" ) ret += 0.98 ;
	if ( s.Left ( 1 ) == "A" || s.Left ( 1 ) == "T" ) ret += 1.03 ;
	if ( s.Right ( 1 ) == "A" || s.Right ( 1 ) == "T" ) ret += 1.03 ;
	
	return ret ;
	}

double PCR_troubleshoot_dialog::deltaH0 ( TPrimer &p )
	{
	int a ;
	double ret = 0 ;
	wxString s = p.get53sequence() ;
	for ( a = 0 ; a+1 < s.length() ; a++ )
		{
		int i = getSLindex ( s.Mid ( a , 2 ) ) ;
		ret += santa_lucia[i].deltaH ;
		}
	
	s = s.Right ( 2 ) ; // This is the end, my friend
	if ( s.Left ( 1 ) == "C" || s.Left ( 1 ) == "G" ) ret += 0.1 ;
	if ( s.Right ( 1 ) == "C" || s.Right ( 1 ) == "G" ) ret += 0.1 ;
	if ( s.Left ( 1 ) == "A" || s.Left ( 1 ) == "T" ) ret += 2.3 ;
	if ( s.Right ( 1 ) == "A" || s.Right ( 1 ) == "T" ) ret += 2.3 ;
	
	return ret ;
	}

double PCR_troubleshoot_dialog::deltaS0 ( TPrimer &p )
	{
	int a ;
	double ret = 0 ;
	wxString s = p.get53sequence() ;
	for ( a = 0 ; a+1 < s.length() ; a++ )
		{
		int i = getSLindex ( s.Mid ( a , 2 ) ) ;
		ret += santa_lucia[i].deltaS ;
		}
	
	s = s.Right ( 2 ) ; // This is the end, my friend
	if ( s.Left ( 1 ) == "C" || s.Left ( 1 ) == "G" ) ret -= 2.8 ;
	if ( s.Right ( 1 ) == "C" || s.Right ( 1 ) == "G" ) ret -= 2.8 ;
	if ( s.Left ( 1 ) == "A" || s.Left ( 1 ) == "T" ) ret += 4.1 ;
	if ( s.Right ( 1 ) == "A" || s.Right ( 1 ) == "T" ) ret += 4.1 ;
	
	return ret - 1.4 ;
	}

int PCR_troubleshoot_dialog::getSLindex ( wxString s )
	{
	s = s.Left ( 2 ) ; // Just fore safety ;
	int a ;
	for ( a = 0 ; a < santa_lucia.size() && santa_lucia[a].seq.Left ( 2 ) != s ; a++ ) ;
	return a ;
	}

void PCR_troubleshoot_dialog::show_item ( int n )
	{
	list->SetSelection ( n ) ;
	text->SetLabel ( l_text[n] ) ;
	}

void PCR_troubleshoot_dialog::scan ()
	{
/*	TPrimer p ;
	p.sequence = "CGTTGA" ;
	double x = deltaG0 ( p ) ;
	wxMessageBox ( wxString::Format ( "%2.2f" , x ) ) ;
*/	
	int a ;
	for ( a = 0 ; a < parent->primer.size() ; a++ )
		{
//		scan_hairpin ( parent->primer[a] , a ) ;
		scan_length ( parent->primer[a] , a ) ;
		scan_GCcontent ( parent->primer[a] , a ) ;
		scan_GCclamp ( parent->primer[a] , a ) ;
		scan_Runs ( parent->primer[a] , a , 1 ) ; // Runs
		scan_Runs ( parent->primer[a] , a , 2 ) ; // Repeats
		scan_dimer ( parent->primer[a] , parent->primer[a] , a , a ) ;
		}
	}

wxString PCR_troubleshoot_dialog::get_dimer_connections ( wxString s1 , wxString s2 )
	{
	wxString ret ;
	for ( int b = 0 ; b < s1.length() ; b++ )
		{
		if ( s1.GetChar(b) == opp ( s2.GetChar(b) ) ) ret += _T("|") ;
		else ret += _T(" ") ;
		}
	return ret ;
	}

wxChar PCR_troubleshoot_dialog::opp ( wxChar c )
	{
	switch ( c )
		{
		case 'A' : return 'T' ; break ;
		case 'C' : return 'G' ; break ;
		case 'G' : return 'C' ; break ;
		case 'T' : return 'A' ; break ;
		}
	return '*' ; // Unknown
	}

double PCR_troubleshoot_dialog::deltaG0_dimer ( wxString s1 , wxString s2 )
	{
	int a ;
	double ret = 0 ;
	for ( a = 0 ; a+1 < s1.length() ; a++ )
		{
//		if ( s1.GetChar ( a ) != opp ( s2.GetChar ( a ) ) ) continue ; // Mismatch
//		if ( s1.GetChar ( a+1 ) != opp ( s2.GetChar ( a+1 ) ) ) continue ; // Mismatch
//		wxMessageBox ( s1.Mid ( a , 2 ) , s2.Mid ( a , 2 ) ) ;
		int i = getSLindex ( s1.Mid ( a , 2 ) ) ;
		if ( i >= santa_lucia.size() ) continue ; // Ignore this one
		ret += santa_lucia[i].unifiedFE ;
		}
/*
	s = s.Right ( 2 ) ; // This is the end, my friend
	if ( s.Left ( 1 ) == "C" || s.Left ( 1 ) == "G" ) ret += 0.98 ;
	if ( s.Right ( 1 ) == "C" || s.Right ( 1 ) == "G" ) ret += 0.98 ;
	if ( s.Left ( 1 ) == "A" || s.Left ( 1 ) == "T" ) ret += 1.03 ;
	if ( s.Right ( 1 ) == "A" || s.Right ( 1 ) == "T" ) ret += 1.03 ;
*/
	return ret ;
	}

void PCR_troubleshoot_dialog::scan_dimer ( TPrimer &p1 , TPrimer &p2 , int nr1 , int nr2 )
	{
	wxString s1 = p1.get53sequence() ;
	wxString s2 = p2.get35sequence() ;
	int sl = s1.length() + s2.length() ;
	s1.Pad ( s2.length() , ' ' , true ) ;
	s1.Pad ( s2.length() , ' ' , false ) ;
	while ( s2.length() < s1.length() ) s2.Pad ( 1 ) ;
	
	int a ;
	double best = 10000 ;
	wxString best_text ;
	for ( a = 0 ; a < sl ; a++ )
		{
		double g0 = deltaG0_dimer ( s1 , s2 ) ;
//		wxMessageBox ( wxString::Format ( _T("DeltaG0=%2.2f\r\n") , g0 ) ) ;
		if ( g0 < best )
			{
			best = g0 ;
			best_text = wxString::Format ( _T("DeltaG0=%2.2f\r\n") , g0 ) ;
			best_text += s1.Trim() + "\r\n" ;
			best_text += get_dimer_connections ( s1 , s2 ) . Trim() + "\r\n" ;
			best_text += s2.Trim() + "\r\n" ;
			best_text.Replace ( " " , "." ) ;
			}
		s2 = _T(" ") + s2.Mid ( 1 , s2.length() - 1 ) ; // Move
		}
	
//	if ( best > 5 ) return ; // Seems OK
	add_warning ( p1 , nr1 , "Dimer formation" , best_text ) ;
	}

void PCR_troubleshoot_dialog::scan_length ( TPrimer &p , int nr )
	{
	if ( p.sequence.length() >= 18 && p.sequence.length() <= 22 ) return ; // OK
	add_warning ( p , nr , "Primer length" , 
	wxString::Format ( _T("This primer is %d bases long. Optimal length is 18-22 bases.") , p.sequence.length() ) ) ;
	}

void PCR_troubleshoot_dialog::scan_GCcontent ( TPrimer &p , int nr )
	{
	int gc = p.getGCcontents () ;
	if ( gc >= 40 && gc <= 60 ) return ; // Everthing's OK
	
	add_warning ( p , nr , "GC Contents" , 
	wxString::Format ( _T("The %%GC content of the primer should be between 40%% and 60%%; it is currently %d%%") , gc ) ) ;
	}

void PCR_troubleshoot_dialog::scan_GCclamp ( TPrimer &p , int nr )
	{
	wxString s = p.get53sequence().Right ( 5 ) ;
	int gc = s.Freq ( 'C' ) + s.Freq ( 'G' ) ;
	if ( gc == 0 )
		{
		add_warning ( p , nr , "GC Clamp" , 
		"The last five bases of the 3' end of the primer should contain at least one G or C" ) ;
		}
	else if ( gc > 3 )
		{
		add_warning ( p , nr , "GC Clamp" , 
		"The last five bases of the 3' end of the primer should not contain more than three Gs or Cs" ) ;
		}
	}

void PCR_troubleshoot_dialog::scan_Runs ( TPrimer &p , int nr , int length )
	{
	wxString s = p.get53sequence() ;
	
	int a , b ;
	wxString msg = "Runs" ;
	if ( length > 1 ) msg = "Repeats" ;
	for ( a = 0 ; a < s.length() ; a++ )
		{
		for ( b = a ; b < s.length() && s.Mid(a,length) == s.Mid(b,length) ; b += length ) ;
		if ( ( b - a ) / length > 4 )
			{
			add_error ( p , nr , msg ,
			"There are more than four repeats of '" + s.Mid(a,length) + "' in a row. This will likely lead to mispriming." ) ;
			a = b - length ;
			}
		else if ( ( b - a ) / length  == 4 )
			{
			add_warning ( p , nr , msg ,
			"There are four repeats of '" + s.Mid(a,length) + "' in a row. This might lead to mispriming." ) ;
			a = b - length ;
			}
		}
	}

void PCR_troubleshoot_dialog::scan_hairpin ( TPrimer &p , int nr )
	{return;
	wxArrayString m = get_matrix ( p.get53sequence() , invert ( p.get53sequence() ) ) ;
	wxArrayString n = get_matches ( m , 4 , 1 ) ;
	
	wxString s ;
	int a ;
//	for ( a = 0 ; a < m.GetCount() ; a++ )
//		s += m[a] + "\r\n" ;
	
	l_title.Add ( wxString::Format ( _T("%d: ") , nr ) + p.getName() ) ;
	l_text.Add ( s ) ;
	}
	
void PCR_troubleshoot_dialog::add_error ( TPrimer &p , int nr , wxString head , wxString tail )
	{
	add_warning ( p , nr , head.Upper() + _T("!") , tail ) ;
	}

void PCR_troubleshoot_dialog::add_warning ( TPrimer &p , int nr , wxString head , wxString tail )
	{
	wxString s = wxString::Format ( _T("%d ") , nr+1 ) ;
	if ( !p.getName().IsEmpty() ) s += "(" + p.getName() + ") " ;
	s += ": " + head ;
	l_title.Add ( s ) ;
	l_text.Add ( tail ) ;
	}

wxArrayString PCR_troubleshoot_dialog::get_matrix ( wxString s1 , wxString s2 )
	{
	int row , col ;
	wxArrayString ret ;
	for ( row = 0 ; row < s2.length() ; row++ )
		{
		ret.Add ( _T("") ) ;
		for ( col = 0 ; col < s1.length() ; col++ )
			{
			if ( s1.GetChar ( col ) == s1.GetChar ( row ) ) ret[row] += '1' ;
			else ret[row] += '0' ;
			}
		}
	return ret ;
	}

wxArrayString PCR_troubleshoot_dialog::get_matches ( wxArrayString &m , int min , int allowed_gaps )
	{
	int col , row ;
	for ( row = 0 ; row < m.GetCount() ; row++ )
		{
		int row2 = row ;
		wxString t ;
		for ( col = 0 ; row2 < m.GetCount() && col < m[row2].length() ; col++ , row2++ )
			t += m[row2].GetChar ( col ) ;
		
		int cnt = 0 ;
		for ( int a = 0 ; a < t.length() ; a++ )
			if ( t.GetChar ( a ) == '1' ) cnt++ ;
		}
	}

wxString PCR_troubleshoot_dialog::invert ( wxString s )
	{
	int a ;
	for ( a = 0 ; a < s.length() ; a++ )
		{
		switch ( s.GetChar ( a ) )
			{
			case 'A' : s.SetChar ( a , 'T' ) ; break ;
			case 'C' : s.SetChar ( a , 'G' ) ; break ;
			case 'G' : s.SetChar ( a , 'C' ) ; break ;
			case 'T' : s.SetChar ( a , 'A' ) ; break ;
			}
		}
	return s ;
	}




void PCR_troubleshoot_dialog::OnOK ( wxCommandEvent &ev )
	{
	}

void PCR_troubleshoot_dialog::OnCancel ( wxCommandEvent &ev )
	{
	}

void PCR_troubleshoot_dialog::OnList ( wxCommandEvent &ev )
	{
	show_item ( list->GetSelection() ) ;
	}
void PCR_troubleshoot_dialog::OnCharHook(wxKeyEvent& event)
	{
	int k = event.GetKeyCode () ;
	wxCommandEvent ev ;
	if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
	else event.Skip() ;
	}
