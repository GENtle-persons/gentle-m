#include "PCR_Troubleshoot.h"

// WHILE DEVELOPING
void TPrimerDesign::OnTroubleshoot ( wxCommandEvent &ev )
	{
	PCR_troubleshoot_dialog td ( this , txt("t_pcr_ts_title") ) ;
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
#ifdef __WXMSW__
	nl = _T("\r\n") ;
#else
	nl = _T("\n") ;
#endif
	fillSantaLucia () ;
	
	wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
	
	list = new wxListBox ( this , PCR_TROUBLESHOOT_LIST ) ;
	text = new wxTextCtrl ( this , -1 , _T("") , wxDefaultPosition , wxDefaultSize , wxTE_MULTILINE ) ;
	text->SetFont ( *MYFONT ( MYFONTSIZE , wxMODERN , wxNORMAL , wxNORMAL ) ) ;
	
	h0->Add ( new wxStaticText ( this , -1 , _T("")) , 1 , wxEXPAND|wxALL , 5 ) ;
	h0->Add ( new wxButton ( this , POD_OK , txt("b_ok") ) , 1 , wxEXPAND|wxALL , 5 ) ;
	h0->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxEXPAND|wxALL , 5 ) ;
	h0->Add ( new wxButton ( this , POD_CANCEL , txt("b_cancel") ) , 1 , wxEXPAND|wxALL , 5 ) ;
	h0->Add ( new wxStaticText ( this , -1 , _T("") ) , 1 , wxEXPAND|wxALL , 5 ) ;
	
	v0->Add ( list , 1 , wxEXPAND|wxALL , 5 ) ;
	v0->Add ( text , 2 , wxEXPAND|wxALL , 5 ) ;
	v0->Add ( h0 , 0 , wxEXPAND|wxALL , 5 ) ;
	
	scan () ;
	for ( int a = 0 ; a < l_title.GetCount() ; a++ )
		list->Append ( l_title[a] ) ;
	if ( l_title.GetCount() > 0 ) show_item ( 0 ) ;

	SetSizer ( v0 ) ;
	v0->Layout () ;
	Center () ;
	}
	
void PCR_troubleshoot_dialog::fillSantaLucia ()
	{
	if ( santa_lucia.size() > 0 ) return ; // Already done ;
	santa_lucia.push_back ( TSantaLucia ( _T("AA/TT") , -1.00 , - 7.9 , -22.2 ) ) ; // AA
	santa_lucia.push_back ( TSantaLucia ( _T("AT/TA") , -0.88 , - 7.2 , -20.4 ) ) ; // AT
	santa_lucia.push_back ( TSantaLucia ( _T("TA/AT") , -0.58 , - 7.2 , -21.3 ) ) ; // TA
	santa_lucia.push_back ( TSantaLucia ( _T("CA/GT") , -1.45 , - 8.5 , -22.7 ) ) ; // CA
	santa_lucia.push_back ( TSantaLucia ( _T("GT/CA") , -1.44 , - 8.4 , -22.4 ) ) ; // GT
	santa_lucia.push_back ( TSantaLucia ( _T("CT/GA") , -1.28 , - 7.8 , -21.0 ) ) ; // CT
	santa_lucia.push_back ( TSantaLucia ( _T("GA/CT") , -1.30 , - 8.2 , -22.2 ) ) ; // GA
	santa_lucia.push_back ( TSantaLucia ( _T("CG/GC") , -2.17 , -10.6 , -27.2 ) ) ; // CG
	santa_lucia.push_back ( TSantaLucia ( _T("GC/CG") , -2.24 , - 9.8 , -24.4 ) ) ; // GC
	santa_lucia.push_back ( TSantaLucia ( _T("GG/CC") , -1.84 , - 8.0 , -19.9 ) ) ; // GG
	
	// I guess...
	santa_lucia.push_back ( TSantaLucia ( _T("TT/AA") , -1.00 , - 7.9 , -22.2 ) ) ; // TT
	santa_lucia.push_back ( TSantaLucia ( _T("CC/GG") , -1.84 , - 8.0 , -19.9 ) ) ; // CC
	santa_lucia.push_back ( TSantaLucia ( _T("TG/AC") , -1.44 , - 8.4 , -22.4 ) ) ; // TG
	santa_lucia.push_back ( TSantaLucia ( _T("TC/AG") , -1.28 , - 7.8 , -21.0 ) ) ; // TC

	santa_lucia.push_back ( TSantaLucia ( _T("AC/TG") , -1.44 , - 8.4 , -22.4 ) ) ; // AC
	santa_lucia.push_back ( TSantaLucia ( _T("AG/TC") , -1.28 , - 7.8 , -21.0 ) ) ; // AG
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
	if ( s.Left ( 1 ) == _T("C") || s.Left ( 1 ) == _T("G") ) ret += 0.98 ;
	if ( s.Right ( 1 ) == _T("C") || s.Right ( 1 ) == _T("G") ) ret += 0.98 ;
	if ( s.Left ( 1 ) == _T("A") || s.Left ( 1 ) == _T("T") ) ret += 1.03 ;
	if ( s.Right ( 1 ) == _T("A") || s.Right ( 1 ) == _T("T") ) ret += 1.03 ;
	
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
	if ( s.Left ( 1 ) == _T("C") || s.Left ( 1 ) == _T("G") ) ret += 0.1 ;
	if ( s.Right ( 1 ) == _T("C") || s.Right ( 1 ) == _T("G") ) ret += 0.1 ;
	if ( s.Left ( 1 ) == _T("A") || s.Left ( 1 ) == _T("T") ) ret += 2.3 ;
	if ( s.Right ( 1 ) == _T("A") || s.Right ( 1 ) == _T("T") ) ret += 2.3 ;
	
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
	if ( s.Left ( 1 ) == _T("C") || s.Left ( 1 ) == _T("G") ) ret -= 2.8 ;
	if ( s.Right ( 1 ) == _T("C") || s.Right ( 1 ) == _T("G") ) ret -= 2.8 ;
	if ( s.Left ( 1 ) == _T("A") || s.Left ( 1 ) == _T("T") ) ret += 4.1 ;
	if ( s.Right ( 1 ) == _T("A") || s.Right ( 1 ) == _T("T") ) ret += 4.1 ;
	
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
	text->SetValue ( l_text[n] ) ;
	}

void PCR_troubleshoot_dialog::scan ()
	{
/*	TPrimer p ;
	p.sequence = "CGTTGA" ;
	double x = deltaG0 ( p ) ;
	wxMessageBox ( wxString::Format ( "%2.2f" , x ) ) ;
*/	
	int a , b ;
	for ( a = 0 ; a < parent->primer.size() ; a++ )
		{
		scan_length ( parent->primer[a] , a ) ;
		scan_GCcontent ( parent->primer[a] , a ) ;
		scan_GCclamp ( parent->primer[a] , a ) ;
		scan_Runs ( parent->primer[a] , a , 1 ) ; // Runs
		scan_Runs ( parent->primer[a] , a , 2 ) ; // Repeats
//		scan_hairpin ( parent->primer[a] , a ) ;
		scan_dimer ( parent->primer[a] , parent->primer[a] , a , a ) ;
		scan_end_stability ( parent->primer[a] , a ) ;
		scan_specificity ( parent->primer[a] , a ) ;
//		scan_melting_temperature ( parent->primer[a] , a ) ;
		}

	for ( a = 0 ; a < parent->primer.size() ; a++ )
		{
		for ( b = 0 ; b < a ; b++ )
			{
			scan_dimer ( parent->primer[a] , parent->primer[b] , a , b ) ;
			}
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
		if ( s1.GetChar ( a ) != opp ( s2.GetChar ( a ) ) ) continue ; // Mismatch
		if ( s1.GetChar ( a+1 ) != opp ( s2.GetChar ( a+1 ) ) ) continue ; // Mismatch
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
	double best = 10000 ; // Just to be on the safe side :-)
	wxString best_text ;
	for ( a = 0 ; a < sl ; a++ )
		{
		double g0 = deltaG0_dimer ( s1 , s2 ) ;
		if ( g0 < best )
			{
			best = g0 ;
			best_text = wxString::Format ( txt("t_pcr_ts_deltag0") , best ) ;
			best_text += trim_both ( s1 , get_dimer_connections ( s1 , s2 ) , s2 ) ;
			}
		s2 = _T(" ") + s2 ; // Move
		}
	
	if ( best > -5 ) return ; // Seems OK
	
	wxString msg = txt("t_pcr_ts_warning_dimer_formation") ;
	if ( nr1 == nr2 ) msg += txt("t_pcr_ts_warning_self_dimer") ;
	else msg += wxString::Format ( txt("t_pcr_ts_warning_crossdimer") , nr2+1 ) ;
	
	add_warning ( p1 , nr1 , msg , best_text ) ;
	}

wxString PCR_troubleshoot_dialog::trim_both ( wxString s1 , wxString s2 , wxString s3 )
	{
	s1 = s1.Trim () ;
	s3 = s3.Trim () ;
	while ( s1.Left(1) + s2.Left(1) + s3.Left(1) == _T("   ") )
		{
		s1 = s1.Mid ( 1 ) ;
		s2 = s2.Mid ( 1 ) ;
		s3 = s3.Mid ( 1 ) ;
		}
	return nl + nl + s1 + nl + s2 + nl + s3 + nl ;
	}

void PCR_troubleshoot_dialog::scan_melting_temperature ( TPrimer &p , int nr )
	{
	double K , H , R , S , c ;
	double tm ;
	
	K = 0.050 ; // 50 mM
	H = deltaH0 ( p ) * 1 ;
	R = 8.314472 ;
	S = deltaS0 ( p ) ;
	c = 2.5E-2 ; // 250 pM
		
//	tm = H / ( S + R * log ( c / 4.0 ) - 237.15 + 16.6 * log ( K ) / log ( 10.0 ) ) ;


	tm = ( (-H) - 5 ) / ( (-S) + R * log ( 1 / c ) ) + 16.6 * log ( K ) / log ( 10.0 ) ;

	add_warning ( p , nr , wxString::Format ( _T("Melting temperature %f; dS=%f; dH=%f; X=%f") , tm , S , H , R*log ( 1/c ) ) , _T("") ) ;		
	}

void PCR_troubleshoot_dialog::scan_specificity ( TPrimer &p , int nr )
	{
	int a , psl = p.sequence.length() ;
	wxString s1 = parent->vec->getStrand53() ;
	wxString s3 = parent->vec->getStrand35() ;
	wxString s2 ;
	for ( a = 0 ; a < s3.length() ; a++ )
		s2 += s3.GetChar ( s3.length() - a - 1 ) ;
	if ( parent->vec->isCircular() )
		{
		s1 += s1.Left ( psl ) ;
		s2 += s2.Left ( psl ) ;
		}
	wxString s = s2 + _T(" ") + s1 ;
	
	wxArrayString vs ;
	for ( a = 0 ; a + psl < s.length() ; a++ )
		{
		wxString su = s.Mid ( a , psl ) ;
		double d = deltaG0_dimer ( p.sequence , su ) ;
		
		wxString x = wxString::Format ( _T("%6.2f : %5d") , d , a ) ;

		x += nl ;
		x += p.sequence ;
		x += nl ;
		x += get_dimer_connections ( p.sequence , su ) ;
		x += nl ;
		x += su ;
		x += nl + nl ;
		
		vs.Add ( x ) ;
		if ( vs.GetCount() <= 5 ) continue ;
		vs.Sort ( true ) ;
		vs.RemoveAt ( vs.GetCount() - 1 ) ;
		}
	vs.Sort ( true ) ;
	
	double d0 , d1 ;
	vs[0].ToDouble ( &d0 ) ;
	vs[1].ToDouble ( &d1 ) ;
	if ( d1 * 2 > d0 ) return ; // The second best is less than half as good as the first fit, so everything's OK...
	
	wxString text ;
	for ( a = 0 ; a < vs.size() ; a++ )
		text += vs[a] + nl ;
//		text += wxString::Format ( _T("%s") , vs[a].mb_str() ) + nl ;
	add_warning ( p , nr , txt("t_pcr_ts_warning_specificity") , text ) ;		
	}

void PCR_troubleshoot_dialog::scan_end_stability ( TPrimer &p , int nr )
	{
	wxString s = p.get53sequence().Right(5) ;
	int a ;
	double best = -100000 ;
	for ( a = 0 ; a+1 < s.length() ; a++ )
		{
		int i = getSLindex ( s.Mid ( a , 2 ) ) ;
		if ( i >= santa_lucia.size() ) continue ; // Ignore this one
		double j = santa_lucia[i].unifiedFE ;
		if ( j > best ) best = j ;
		}
	add_warning ( p , nr , txt("t_pcr_ts_warning_end_stability") , 
	wxString::Format ( txt("t_pcr_ts_warning_end_stability_text") , best ) ) ;
	}

void PCR_troubleshoot_dialog::scan_length ( TPrimer &p , int nr )
	{
	if ( p.sequence.length() >= 18 && p.sequence.length() <= 22 ) return ; // OK
	add_warning ( p , nr , txt("t_pcr_ts_warning_primer_length") , 
	wxString::Format ( txt("t_pcr_ts_warning_primer_length_text") , p.sequence.length() ) ) ;
	}

void PCR_troubleshoot_dialog::scan_GCcontent ( TPrimer &p , int nr )
	{
	float gc = p.getGCcontents () ;
	if ( gc >= 40 && gc <= 60 ) return ; // Everthing's OK
	
	add_warning ( p , nr , txt("t_pcr_ts_warning_GC") , 
	wxString::Format ( txt("t_pcr_ts_warning_GC_text") , gc ) ) ;
	}

void PCR_troubleshoot_dialog::scan_GCclamp ( TPrimer &p , int nr )
	{
	wxString s = p.get53sequence().Right ( 5 ) ;
	int gc = s.Freq ( 'C' ) + s.Freq ( 'G' ) ;
	if ( gc == 0 )
		{
		add_warning ( p , nr , txt("t_pcr_ts_warning_gc_clamp") , 
		txt("t_pcr_ts_warning_gc_clamp_t1") ) ;
		}
	else if ( gc > 3 )
		{
		add_warning ( p , nr , txt("t_pcr_ts_warning_gc_clamp") , 
		txt("t_pcr_ts_warning_gc_clamp_t2") ) ;
		}
	}

void PCR_troubleshoot_dialog::scan_Runs ( TPrimer &p , int nr , int length )
	{
	wxString s = p.get53sequence() ;
	
	int a , b ;
	wxString msg = txt("t_pcr_ts_warning_runs") ;
	if ( length > 1 ) msg = txt("t_pcr_ts_warning_repeats") ;
	for ( a = 0 ; a < s.length() ; a++ )
		{
		for ( b = a ; b < s.length() && s.Mid(a,length) == s.Mid(b,length) ; b += length ) ;
		if ( ( b - a ) / length > 4 )
			{
			add_error ( p , nr , msg ,
			wxString::Format ( txt("t_pcr_ts_warning_runs_text") , (char*)s.Mid(a,length).c_str() ) ) ;
			a = b - length ;
			}
		else if ( ( b - a ) / length  == 4 )
			{
			add_warning ( p , nr , msg ,
			wxString::Format ( txt("t_pcr_ts_warning_runs_text2") , (char*)s.Mid(a,length).c_str() ) ) ;
			a = b - length ;
			}
		}
	}

void PCR_troubleshoot_dialog::scan_hairpin ( TPrimer &p , int nr )
	{return;
	wxArrayString m = get_matrix ( p.get53sequence() , invert ( p.get53sequence() ) ) ;
	wxArrayString n = get_matches ( m , 4 , 1 ) ;
	
	wxString s ;

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
	if ( !p.getName().IsEmpty() ) s += _T("(") + p.getName() + _T(") ") ;
	s += _T(": ") + head ;
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
    wxDialog::OnOK ( ev ) ;
	}

void PCR_troubleshoot_dialog::OnCancel ( wxCommandEvent &ev )
	{
    wxDialog::OnCancel ( ev ) ;
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
