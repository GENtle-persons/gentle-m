#include "TProteolysis.h"

BEGIN_EVENT_TABLE(TProteolysis, wxDialog)
	EVT_BUTTON(POD_OK,TProteolysis::OnOK)
	EVT_BUTTON(POD_CANCEL, TProteolysis::OnCancel)
	EVT_BUTTON(PRO_FRAGMENTS_ALL, TProteolysis::OnAll)
	EVT_BUTTON(PRO_FRAGMENTS_NONE, TProteolysis::OnNone)
	EVT_RADIOBOX(PRO_SORT_RESULTS, TProteolysis::OnSortResults)
	EVT_RADIOBOX(PRO_SEP_NUM_PROT, TProteolysis::OnNumProts)
	EVT_CHAR_HOOK(TProteolysis::OnCharHook)
	EVT_CHECKLISTBOX(PRO_PROTEASES, TProteolysis::OnProtease)
	EVT_CHECKLISTBOX(PRO_IGNORE, TProteolysis::OnIgnore)
	EVT_CHECKLISTBOX(PRO_CUTS, TProteolysis::OnCuts)
	EVT_CHECKLISTBOX(PRO_SEP_FRAGMENTS, TProteolysis::OnSepFragments)
	EVT_LISTBOX(PRO_SEP_RES, TProteolysis::OnSepResults)
	EVT_CHECKBOX(PRO_SHOW_UNCUT, TProteolysis::OnShowGel)
END_EVENT_TABLE()

//BEGIN_EVENT_TABLE(TProteolysisGel, wxScrolledWindow)
//END_EVENT_TABLE()

bool operator < ( const TProteolysisSuggestion &c1 , const TProteolysisSuggestion &c2 )
    {
    return c1.grade < c2.grade ;
    }
    
bool operator == ( const TProteolysisSuggestion &c1 , const TProteolysisSuggestion &c2 )
    {
    return c1.grade == c2.grade ;
    }



TProteolysis::TProteolysis(TAminoAcids *_parent, const wxString& title )
	: wxDialog ( (wxWindow*) _parent , -1 , title , wxDefaultPosition , wxSize ( 710 , 550 ) )
	{
	parent = _parent ;
	v = parent->vec ;
		
	sep_fragments = new wxCheckListBox ( this , PRO_SEP_FRAGMENTS ) ;
	proteases = new wxCheckListBox ( this , PRO_PROTEASES ) ;
	ignore = new wxCheckListBox ( this , PRO_IGNORE ) ;
	cuts = new wxCheckListBox ( this , PRO_CUTS ) ;
	results = new wxCheckListBox ( this , -1 ) ;
	gel = new TProteolysisGel ( this , -1 ) ;
		
	create_fragments = new wxCheckBox ( this , -1 , txt("t_proteolysis_create_fragments") ) ;
	create_labels = new wxCheckBox ( this , -1 , txt("t_proteolysis_annotate") ) ;
	use_proteases = new wxCheckBox ( this , -1 , txt("t_proteolysis_keep_proteases") ) ;
	show_uncut = new wxCheckBox ( this , PRO_SHOW_UNCUT , txt("t_proteolysis_show_uncut") ) ;
	
	sep_results = new wxListBox ( this , PRO_SEP_RES ) ;
	
   wxString rb_np[3] ;
   rb_np[0] = _T("1") ;
   rb_np[1] = _T("2") ;
   rb_np[2] = _T("3") ;
	sep_num_prot = new wxRadioBox ( this , PRO_SEP_NUM_PROT , 
								txt("t_proteolysis_auto_num_prot") ,
								wxDefaultPosition , wxDefaultSize ,
								3 , rb_np , wxRA_SPECIFY_COLS ) ;
	sep_num_prot->SetSelection ( 0 ) ;
	
	sep_desc = new wxTextCtrl ( this , -1 , _T("") , wxDefaultPosition , 
							wxDefaultSize , wxTE_MULTILINE|wxTE_READONLY ) ;
	sep_desc->SetBackgroundColour ( GetBackgroundColour() ) ;
	sep_desc->SetFont ( *MYFONT ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ) ;
	
   wxString rb_sr[3] ;
   rb_sr[0] = txt("t_pro_sort_results_1" ) ;
   rb_sr[1] = txt("t_pro_sort_results_2" ) ;
   rb_sr[2] = txt("t_pro_sort_results_3" ) ;
	sortresults = new wxRadioBox ( this , PRO_SORT_RESULTS , txt("t_pro_sort_results") ,
                           wxDefaultPosition ,
                           wxDefaultSize ,
                           3 ,
                           rb_sr ,
                           wxRA_SPECIFY_COLS
                           ) ;
	
	
	wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *va = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *vl = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *vr = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *vg = new wxBoxSizer ( wxVERTICAL ) ;
	
	va->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_separate") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	va->Add ( sep_fragments , 2 , wxEXPAND|wxALL , 5 ) ;
	va->Add ( sep_num_prot , 0 , wxEXPAND|wxALL , 5 ) ;
	va->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_auto_results") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	va->Add ( sep_results , 2 , wxEXPAND|wxALL , 5 ) ;
	va->Add ( sep_desc , 1 , wxEXPAND|wxALL , 5 ) ;

	vl->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_proteases") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	vl->Add ( proteases , 1 , wxEXPAND|wxALL , 5 ) ;
	vl->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_skip") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	vl->Add ( ignore , 1 , wxEXPAND|wxALL , 5 ) ;

	vr->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_cuts") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	vr->Add ( cuts , 1 , wxEXPAND|wxALL , 5 ) ;

	h3->Add ( new wxButton ( this , PRO_FRAGMENTS_ALL , txt("b_all") ) , 0 , wxALL|wxEXPAND , 2 ) ;
	h3->Add ( new wxButton ( this , PRO_FRAGMENTS_NONE , txt("b_none") ) , 0 , wxALL|wxEXPAND , 2 ) ;

	vr->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_results") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	vr->Add ( results , 1 , wxEXPAND|wxALL , 5 ) ;
	vr->Add ( sortresults , 0 , wxEXPAND|wxALL , 5 ) ;
	vr->Add ( h3 , 0 , wxEXPAND|wxALL , 5 ) ;
	
	vg->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_gel") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	vg->Add ( gel , 1 , wxEXPAND|wxALL , 5 ) ;

	
	h0->Add ( va , 2 , wxEXPAND ) ;
	h0->Add ( vl , 2 , wxEXPAND ) ;
	h0->Add ( vr , 2 , wxEXPAND ) ;
	h0->Add ( vg , 1 , wxEXPAND ) ;

	h1->Add ( create_fragments , 0 , wxALL|wxEXPAND , 2 ) ;
	h1->Add ( create_labels , 0 , wxALL|wxEXPAND , 2 ) ;
	h1->Add ( use_proteases , 0 , wxALL|wxEXPAND , 2 ) ;
	h1->Add ( show_uncut , 0 , wxALL|wxALIGN_RIGHT , 2 ) ;

	h2->Add ( new wxStaticText ( this , -1 , txt("") ) , 2 , wxEXPAND|wxALL , 2 ) ;
	h2->Add ( new wxButton ( this , POD_OK , txt("b_ok") ) , 1 , wxALL|wxEXPAND , 2 ) ;
	h2->Add ( new wxStaticText ( this , -1 , txt("") ) , 1 , wxEXPAND|wxALL , 2 ) ;
	h2->Add ( new wxButton ( this , POD_CANCEL , txt("b_cancel") ) , 1 , wxALL|wxEXPAND , 2 ) ;
	h2->Add ( new wxStaticText ( this , -1 , txt("") ) , 2 , wxEXPAND|wxALL , 2 ) ;
	
	v0->Add ( h0 , 1 , wxEXPAND ) ;
	v0->Add ( h1 , 0 , wxEXPAND ) ;
	v0->Add ( h2 , 0 , wxEXPAND ) ;
	
	SetSizer ( v0 ) ;
	v0->Layout () ;
	
	int a ;

	// Initialize protease list
	ls = myapp()->frame->LS ;
	wxArrayString as ;
	for ( a = 0 ; a < ls->pr.GetCount() ; a++ )
		as.Add ( ls->pr[a]->name ) ;
	as.Sort () ;
	for ( a = 0 ; a < as.GetCount() ; a++ )
		proteases->Append ( as[a] ) ;

	// Initialize feature list
	for ( a = 0 ; a < v->items.size() ; a++ )
		{
		wxString s ;
		s += v->items[a].name ;
		s += wxString::Format ( _T(" (%d-%d)") , v->items[a].from , v->items[a].to ) ;
		ignore->Append ( s ) ; // The "do not cut in this feature" list
		sep_fragments->Append ( s ) ; // The "separate fragments" list
		}
	
	// Initialize used proteases
	for ( a = 0 ; a < v->proteases.GetCount() ; a++ )
		{
		int b = proteases->FindString ( v->proteases[a] ) ;
		if ( b == wxNOT_FOUND ) continue ;
		proteases->Check ( b ) ;
		}
		
	show_uncut->SetValue ( true ) ;
	
	recalc () ;
	}

void TProteolysis::recalc ()
	{
	calc_cut_list () ;
	calc_fragment_list () ;
	}

void TProteolysis::calc_spearation ()
	{
	suggestions.clear () ;
	sep_results->Clear () ;
	sep_desc->Clear () ;

	int a ;
	vector <TFragment> tobe ; // To-be parts
	for ( a = 0 ; a < v->items.size() ; a++ )
		{
		if ( !sep_fragments->IsChecked ( a ) ) continue ;
		TFragment f ;
		f.from = v->items[a].from ;
		f.to = v->items[a].to ;
		f.length = a ; // Abusing length element as index
		tobe.push_back ( f ) ;
		}
	sort ( tobe.begin() , tobe.end() ) ;
	
	if ( tobe.size() < 2 ) return ; // No need to run all this
	
	wxBeginBusyCursor () ;
	wxArrayTProtease prop ;
	for ( a = 0 ; a < sep_num_prot->GetSelection() + 1 ; a++ )
		calc_spearation_sub ( a + 1 , prop , tobe ) ;
	sort ( suggestions.begin() , suggestions.end() ) ;
	for ( a = 0 ; a < 20 && a < suggestions.size() ; a++ ) // Show only the best 20
		{
		sep_results->Append ( suggestions[a].name ) ;
		}
	wxEndBusyCursor () ;
	}

void TProteolysis::calc_spearation_sub ( int depth , wxArrayTProtease &prop , vector <TFragment> &tobe , int start )
	{
	int a , b , c , cur = prop.size() ;
	prop.Add ( NULL ) ;
	for ( a = start ; a < ls->pr.GetCount() ; a++ )
		{
		// Are we already cutting with this one?
		bool in = false ;
		for ( b = 0 ; !in && b < prop.size()-1 ; b++ )
			{
			if ( ls->pr[a] == prop[b] ) in = true ;
			}
		if ( in ) continue ;
		
		// Add this protease to the cocktail and try
		prop[cur] = ls->pr[a] ;
		if ( depth > 1 ) // Invoke next sub?
			{
			calc_spearation_sub ( depth - 1 , prop , tobe , a+1 ) ;
			continue ;
			}
		
		// Do the cutting!
		wxString name , desc ;
		wxArrayTProteaseCut apc ;
		determine_cuts ( prop , apc ) ;
		remove_ignored_cuts ( apc ) ;
		sort_cuts ( apc ) ;
//		add_final_piece ( apc ) ; // Unnecessary for this

//		if ( apc.size() > 200 ) continue ; // HARD LIMIT for number of cuts

		// Start description
		desc += _T("* ") + wxString::Format ( txt("t_proteolysis_auto_fragments") , apc.size() ) + _T("\r\n") ;
		for ( b = 0 ; b < prop.size() ; b++ )
			{
			if ( name != _T("") ) name += _T(", ") ;
			name += prop[b]->name ;
			desc += _T("* ") + prop[b]->name + _T("\r\n") ;
			}
		
		// Rate this cocktail
		int grade = depth - 1 ; // larger value = worse
		
		for ( b = 1 ; b < tobe.size() ; b++ )
			{
			bool sep = false ;
			for ( c = 0 ; !sep && c < apc.size() ; c++ )
				{
				if ( apc[c]->cut >= tobe[b-1].to && apc[c]->cut < tobe[b].from )
					sep = true ;
				}
			if ( sep ) // Does separate
				{
				desc += txt("t_proteolysis_auto_desc_1") ;
				desc += v->items[tobe[b-1].length].name ;
				desc += txt("t_proteolysis_auto_desc_2") ;
				desc += v->items[tobe[b].length].name ;
				}
			else
				{
				desc += txt("t_proteolysis_auto_desc_n1") ;
				desc += v->items[tobe[b-1].length].name ;
				desc += txt("t_proteolysis_auto_desc_n2") ;
				desc += v->items[tobe[b].length].name ;
				grade++ ; // Does not cut! Bad enzyme!
				}
			desc += _T("\r\n") ;
			}
		
		grade *= 100 ; // Number of not separated parts is really bad!
		grade += apc.size() ; // Many created fragments are bad too
		
		// Add this setup to the suggestion list
		TProteolysisSuggestion s ;
		s.proteases = prop ;
		s.grade = grade ;
		s.desc = desc ;
		s.name = name + _T(" (" ) + wxString::Format ( txt("t_proteolysis_auto_fragments") , apc.size() ) + _T(")") ;
		suggestions.push_back ( s ) ;

		while ( apc.GetCount() )
			{
			delete apc[0] ;
			apc.RemoveAt ( 0 ) ;
			}
		}
	prop.RemoveAt ( cur ) ;
	}

void TProteolysis::determine_cuts ( wxArrayTProtease &prop , wxArrayTProteaseCut &apc )
	{
	// Determine cuts
	for ( int a = 0 ; a < v->getSequenceLength() ; a++ )
		{
		wxString s = v->getSequence().Left(a) ;
		for ( int q = 0 ; q < prop.GetCount() ; q++ )
			{
			TProtease *pr = prop[q] ;
			if ( pr->len() <= s.length() )
				{
				wxString w2 = s.substr ( s.length() - pr->len() , pr->len() ) ;
				if ( pr->does_match ( w2 ) )
					{
					TProteaseCut *cut = new TProteaseCut ;
					cut->protease = pr ;
					cut->cut = s.length()-pr->len()+pr->cut+1 ;
					cut->left = false ; // Unused
					apc.Add ( cut ) ;
					}
				}
			}
		}
	}
	
void TProteolysis::remove_ignored_cuts ( wxArrayTProteaseCut &apc )
	{
	// Remove ignored cuts
	for ( int a = 0 ; a < v->items.size() ; a++ )
		{
		if ( !ignore->IsChecked ( a ) ) continue ; // Not checked
		for ( int q = 0 ; q < apc.GetCount() ; q++ )
			{
			if ( apc[q]->cut < v->items[a].from ) continue ; // Cuts before item
			if ( apc[q]->cut >= v->items[a].to ) continue ; // Cuts after item
			delete apc[q] ;
			apc.RemoveAt ( q ) ;
			q-- ;
			}
		}
	}

void TProteolysis::sort_cuts ( wxArrayTProteaseCut &apc )
	{
	// Sort cuts
	for ( int a = 0 ; a+1 < pc.GetCount() ; a++ )
		{
		if ( pc[a]->cut > pc[a+1]->cut )
			{
			TProteaseCut *temp = pc[a] ;
			pc[a] = pc[a+1] ;
			pc[a+1] = temp ;
			a = -1 ;
			}
		}
	}

void TProteolysis::add_final_piece ( wxArrayTProteaseCut &apc )
	{
	// Add final piece
	if ( apc.GetCount() && apc[apc.GetCount()-1]->cut != v->getSequenceLength() )
		{
		TProteaseCut *cut = new TProteaseCut ;
		cut->protease = NULL ;
		cut->cut = v->getSequenceLength() ;
		cut->left = false ; // Unused
		apc.Add ( cut ) ;
		}
	}

void TProteolysis::calc_cut_list ()
	{
	int nop = proteases->GetCount() ;
	int a , q ;
	
	wxArrayTProtease prop ; // PROtease Pointers
	for ( q = 0 ; q < nop ; q++ )
		{
		if ( proteases->IsChecked ( q ) )
			{
			prop.Add ( ls->getProtease ( proteases->GetString ( q ) ) ) ;
			}
		}

	// Clear old list
	while ( pc.GetCount() )
		{
		delete pc[0] ;
		pc.RemoveAt ( 0 ) ;
		}
	
	determine_cuts ( prop , pc ) ;
	remove_ignored_cuts ( pc ) ;
	sort_cuts ( pc ) ;
	add_final_piece ( pc ) ;

	
	// Display list
	cuts->Clear () ;
	for ( a = 0 ; a+1 < pc.GetCount() ; a++ )
		{
		wxString s ;
		s = wxString::Format ( txt("t_proteolysis_cut_after") , pc[a]->cut , pc[a]->protease->name.mb_str() ) ;
		cuts->Append ( s ) ;
		cuts->Check ( a ) ;
		}
	}

void TProteolysis::calc_fragment_list ()
	{
	// Create fragment list
	fragments.clear () ;
	int a , last = 1 ;
	for ( a = 0 ; a < pc.GetCount() ; a++ )
		{
		if ( a < cuts->GetCount() && !cuts->IsChecked ( a ) ) continue ;
		if ( pc[a]->cut == last ) continue ;
		TFragment f ;
		f.from = last ;
		f.to = pc[a]->cut ;
		f.length = pc[a]->cut - last + 1 ;
		f.weight = get_weight ( f.from , f.to ) ;
		fragments.push_back ( f ) ;
		last = pc[a]->cut + 1 ;
		}	
	show_fragment_list () ;
	show_gel () ;
	}
	
void TProteolysis::show_fragment_list ()
	{
	// Display list
	int a ;
	results->Clear () ;
	for ( a = 0 ; a < fragments.size() ; a++ )
		{
		wxString s ;
		s = wxString::Format ( _T("%d-%d (%d; %2.2fkD)") , 
				fragments[a].from , 
				fragments[a].to , 
				fragments[a].length ,
				fragments[a].weight ) ;
		results->Append ( s ) ;
		}
	}

void TProteolysis::show_gel ()
	{
	wxClientDC dc ( gel ) ;
	draw_gel ( dc ) ;
	}

void TProteolysis::draw_gel ( wxDC &dc ) 
	{
	wxString s ;
	int a , y , tw , th , w , h ;
	dc.GetSize ( &w , &h ) ;
	
	dc.SetFont( *MYFONT ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ) ;
	
	dc.SetBrush ( *wxWHITE_BRUSH ) ;
	dc.SetPen ( *wxBLACK_PEN ) ;
	dc.DrawRectangle ( 0 , 0 , w , h ) ;
	
	double min = 1.0 ; // Smallest visible band
	double max ; // Largest visible band
	
	if ( show_uncut->IsChecked() )
		{
		max = get_weight ( 1 , v->getSequenceLength() ) ; // Uncut protein
		}
	else
		{
		max = 0 ;
		for ( a = 0 ; a < fragments.size() ; a++ )
			{
			if ( fragments[a].weight > max ) 
				max = fragments[a].weight ;
			}
		}
	max = ( ((int)max/10) + 1 ) * 10 ; // Just a little larger...
		
	// Draw labels
	for ( a = 10 ; a <= (int) max ; a += 10 )
		{
		y = get_y ( a , h , min , max ) ;
		s = wxString::Format ( _T("%3d") , a ) ;
		dc.GetTextExtent ( s , &tw , &th ) ;
		dc.DrawText ( s , w*3/10-tw , y-th/2 ) ;
		dc.DrawLine ( w*3/10 , y , w*4/10 , y ) ;
		}
	for ( a = 1 ; a < 10 ; a++ )
		{
		y = get_y ( a , h , min , max ) ;
		s = wxString::Format ( _T("%3d") , a ) ;
		dc.GetTextExtent ( s , &tw , &th ) ;
		dc.DrawText ( s , w*3/10-tw , y-th/2 ) ;
		dc.DrawLine ( w*3/10 , y , w*4/10 , y ) ;
		}

	// Draw fragments
	for ( a = 0 ; a < fragments.size() ; a++ )
		{
		y = get_y ( fragments[a].weight , h , min , max ) ;
		dc.DrawLine ( w/2 , y , w*9/10 , y ) ;
		}

	// Draw uncut protein
	if ( show_uncut->IsChecked() )
		{
		dc.SetPen ( *wxRED_PEN ) ;
		y = get_y ( get_weight ( 1 , v->getSequenceLength() ) , h , min , max ) ;
		dc.DrawLine ( w/2 , y , w*9/10 , y ) ;
		}
	}

int TProteolysis::get_y ( double y , int h , double min , double max )
	{
	h -= 20 ;
	double a = -((double)h) / (log(min)-log(max)) ;
	double r ;
	r = a * log ( y ) ;
	return h - ((int) r) + 10 ;
	}

double TProteolysis::get_weight ( int from , int to )
	{
	double w = 0 ;
	int a ;
	for ( a = from ; a <= to ; a++ )
		{
		char c = v->getSequenceChar ( a-1 ) ;
      w += v->getAAmw ( c ) ;
		}
	return w / 1000 ;
	}

void TProteolysis::OnOK ( wxCommandEvent &ev )
	{
	int a ;
	wxString s ;
	
	// Use proteases
	if ( use_proteases->IsChecked() )
		{
		v->proteases.Clear () ;
		for ( a = 0 ; a < proteases->GetCount() ; a++ )
			{
			if ( !proteases->IsChecked ( a ) ) continue ;
			s = proteases->GetString ( a ) ;
			v->proteases.Add ( s ) ;
			}
		v->setChanged() ;
      parent->showSequence () ;
		}
	
	// Annotate
	if ( create_labels->IsChecked() )
		{
		int cnt = 0 ;
		for ( a = 0 ; a < fragments.size() ; a++ )
			{
			if ( !results->IsChecked ( a ) ) continue ;
			cnt++ ;
			TVectorItem i ( wxString::Format ( txt("t_proteolysis_fragment") , cnt ) , 
								 wxString::Format ( txt("t_proteolysis_fragment_long") , cnt ) , 
								 fragments[a].from , 
								 fragments[a].to ,
								 VIT_MISC ) ;
			v->items.push_back ( i ) ;
			}
		v->setChanged() ;
      parent->showSequence () ;
		}
	
	// Create fragments
	if ( create_fragments->IsChecked() )
		{
		for ( a = 0 ; a < fragments.size() ; a++ )
			{
			if ( !results->IsChecked ( a ) ) continue ;
			
			TVector *nv = new TVector ;
			nv->setFromVector ( *v ) ;
			nv->eraseSequence ( fragments[a].to , v->getSequenceLength() ) ;
			nv->eraseSequence ( 0 , fragments[a].from-1 ) ;
			nv->addName ( wxString::Format ( _T(" [%d-%d]") , fragments[a].from , fragments[a].to ) ) ;

			myapp()->frame->newAminoAcids ( nv , nv->getName() ) ;
			}
		}
	
   wxDialog::OnOK ( ev ) ;
	}

void TProteolysis::OnAll ( wxCommandEvent &ev )
	{
	int a ;
	for ( a = 0 ; a < results->GetCount() ; a++ )
		results->Check ( a , true ) ;
	}
	
void TProteolysis::OnNone ( wxCommandEvent &ev )
	{
	int a ;
	for ( a = 0 ; a < results->GetCount() ; a++ )
		results->Check ( a , false ) ;
	}
	

void TProteolysis::OnCancel ( wxCommandEvent &ev )
	{
   wxDialog::OnCancel ( ev ) ;
	}
	
void TProteolysis::OnProtease ( wxCommandEvent &ev )
	{
	recalc () ;
	}
	
void TProteolysis::OnIgnore ( wxCommandEvent &ev )
	{
	recalc () ;
	}
	
void TProteolysis::OnCuts ( wxCommandEvent &ev )
	{
	calc_fragment_list () ;
	}
	
void TProteolysis::OnShowGel ( wxCommandEvent &ev )
	{
	show_gel () ;
	}

void TProteolysis::OnSortResults ( wxCommandEvent &ev )
	{
	int a , mode = sortresults->GetSelection() ;
	for ( a = 1 ; a < fragments.size() ; a++ )
		{
		bool flip = false ;
		if ( mode == 0 ) // By position
			{
			if ( fragments[a-1].from > fragments[a].from ) flip = true ;
			}
		else if ( mode == 1 ) // By length
			{
			if ( fragments[a-1].length < fragments[a].length ) flip = true ;
			}
		else if ( mode == 2 ) // By size
			{
			if ( fragments[a-1].weight < fragments[a].weight ) flip = true ;
			}
		if ( flip )
			{
			TFragment f = fragments[a-1] ;
			fragments[a-1] = fragments[a] ;
			fragments[a] = f ;
			a = 0 ;
			}
		}
	show_fragment_list () ;
	}


void TProteolysis::OnSepFragments ( wxCommandEvent &ev )
	{
	calc_spearation () ;
	}

void TProteolysis::OnSepResults ( wxCommandEvent &ev )
	{
	int a , b , i = sep_results->GetSelection () ;
	sep_desc->SetValue ( suggestions[i].desc ) ;

	for ( a = 0 ; a < ls->pr.GetCount() ; a++ )
		{
		int c = proteases->FindString ( ls->pr[a]->name ) ;
		if ( c == wxNOT_FOUND ) continue ; // Strange...
		bool check = false ;
		for ( b = 0 ; !check && b < suggestions[i].proteases.size() ; b++ )
			{
			if ( ls->pr[a] == suggestions[i].proteases[b] )
				check = true ;
			}
		proteases->Check ( c , check ) ;
		}
	recalc () ;
	}

void TProteolysis::OnNumProts(wxCommandEvent& event)
	{
	OnSepFragments ( event ) ;
	}

void TProteolysis::OnCharHook(wxKeyEvent& event)
	{
	int k = event.GetKeyCode () ;
	wxCommandEvent ev ;
	if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
	else event.Skip() ;
	}

//************************************


TProteolysisGel::TProteolysisGel(wxWindow* parent, wxWindowID id )
	: wxScrolledWindow ( parent , id )
	{
	}

void TProteolysisGel::OnDraw(wxDC& dc)
	{
	((TProteolysis*)GetParent())->draw_gel ( dc ) ;
	}

//************************************

void TAminoAcids::OnProteolysis(wxCommandEvent& event)
	{
	TProteolysis pro ( this , txt("t_proteolysis") ) ;
	pro.ShowModal() ;
	}
