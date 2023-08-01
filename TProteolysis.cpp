#include "TProteolysis.h"

BEGIN_EVENT_TABLE(TProteolysis, wxDialog)
	EVT_BUTTON(POD_OK,TProteolysis::OnOK)
	EVT_BUTTON(POD_CANCEL, TProteolysis::OnCancel)
	EVT_BUTTON(PRO_FRAGMENTS_ALL, TProteolysis::OnAll)
	EVT_BUTTON(PRO_FRAGMENTS_NONE, TProteolysis::OnNone)
	EVT_BUTTON(PRO_CREATE_REPORT, TProteolysis::OnReport)
	EVT_RADIOBOX(PRO_SORT_RESULTS, TProteolysis::OnSortResults)
	EVT_RADIOBOX(PRO_SEP_NUM_PROT, TProteolysis::OnNumProts)
	EVT_CHAR_HOOK(TProteolysis::OnCharHook)
	EVT_CHECKLISTBOX(PRO_PROTEASES, TProteolysis::OnProtease)
	EVT_CHECKLISTBOX(PRO_RESULTS, TProteolysis::OnResults)
	EVT_CHECKLISTBOX(PRO_IGNORE, TProteolysis::OnIgnore)
	EVT_CHECKLISTBOX(PRO_CUTS, TProteolysis::OnCuts)
	EVT_CHECKLISTBOX(PRO_SEP_FRAGMENTS, TProteolysis::OnSepFragments)
	EVT_LISTBOX(PRO_SEP_RES, TProteolysis::OnSepResults)
	EVT_CHECKBOX(PRO_SHOW_UNCUT, TProteolysis::OnShowGel)
	EVT_CHECKBOX(PRO_PARTIAL, TProteolysis::OnPartial)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TProteolysisGel, wxScrolledWindow)
    EVT_MOUSE_EVENTS(TProteolysisGel::OnEvent)
END_EVENT_TABLE()

void TProteolysisGel::OnEvent(wxMouseEvent& event)
    {
    if ( !event.LeftDown() ) return ;
	wxClientDC dc ( this ) ;
    wxPoint p = event.GetLogicalPosition ( dc ) ;
    
    wxArrayInt res ;
    int a , best = 99999 ;
    for ( a = 0 ; a < screen.GetCount() ; a++ )
        {
        int dist = abs ( p.y - screen[a] ) ;
        if ( dist > best ) continue ; // Farther than the best so far
        if ( dist < best ) res.Clear() ;
        best = dist ;
        res.Add ( a ) ;
        }
    if ( best > 30 ) res.Clear() ; // No good match
    
    TProteolysis *parent = (TProteolysis *) GetParent() ;
    parent->select_fragments ( res ) ;
    }

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
	myapp()->frame->push_help ( _T("GENtle:Proteolysis_assistant") ) ;
	parent = _parent ;
	v = parent->vec ;
	
	sep_fragments = new wxCheckListBox ( this , PRO_SEP_FRAGMENTS ) ;
	proteases = new wxCheckListBox ( this , PRO_PROTEASES ) ;
	ignore = new wxCheckListBox ( this , PRO_IGNORE ) ;
	cuts = new wxCheckListBox ( this , PRO_CUTS ) ;
	results = new wxCheckListBox ( this , PRO_RESULTS ) ;
	gel = new TProteolysisGel ( this , -1 ) ;
		
	create_fragments = new wxCheckBox ( this , -1 , txt("t_proteolysis_create_fragments") ) ;
	create_labels = new wxCheckBox ( this , -1 , txt("t_proteolysis_annotate") ) ;
	use_proteases = new wxCheckBox ( this , -1 , txt("t_proteolysis_keep_proteases") ) ;
	show_uncut = new wxCheckBox ( this , PRO_SHOW_UNCUT , txt("t_proteolysis_show_uncut") ) ;
    partial_digestion = new wxCheckBox ( this , PRO_PARTIAL , txt("t_proteolysis_partial_digestion") ) ;
	
    sep_results = new wxListBox ( this , PRO_SEP_RES ) ;
	
    wxString rb_np[4] ;
    rb_np[0] = _T("1") ;
    rb_np[1] = _T("2") ;
    rb_np[2] = _T("3") ;
    rb_np[3] = _T("4") ;
	sep_num_prot = new wxRadioBox ( this , PRO_SEP_NUM_PROT , 
								txt("t_proteolysis_auto_num_prot") ,
								wxDefaultPosition , wxDefaultSize ,
								4 , rb_np , wxRA_SPECIFY_COLS ) ;
	sep_num_prot->SetSelection ( 1 ) ;
	
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
//	wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxFlexGridSizer *h1 = new wxFlexGridSizer ( 3 ) ;
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

	h3->Add ( new wxButton ( this , PRO_FRAGMENTS_ALL , txt("b_all") ) , 0 , wxALL|wxEXPAND , 2 ) ;
	h3->Add ( new wxButton ( this , PRO_FRAGMENTS_NONE , txt("b_none") ) , 0 , wxALL|wxEXPAND , 2 ) ;

	vr->Add ( new wxStaticText ( this , -1 , txt("t_proteolysis_cuts") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	vr->Add ( cuts , 1 , wxEXPAND|wxALL , 5 ) ;
	vr->Add ( partial_digestion , 0 , wxEXPAND|wxALL , 5 ) ;
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
	h1->Add ( show_uncut , 0 , wxALL|wxEXPAND , 2 ) ;
	h1->Add ( new wxButton ( this , PRO_CREATE_REPORT , txt("b_pro_report") ) , 0 , wxALL|wxEXPAND , 2 ) ;

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
	ls = myapp()->frame->LS ;
	find_cutting_proteases () ;

	// Initialize protease list
	wxArrayString as ;
	for ( a = 0 ; a < cutting_proteases.size() ; a++ )
		as.Add ( cutting_proteases[a]->name ) ;
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
	Centre () ;
	}

TProteolysis::~TProteolysis ()
	{
	myapp()->frame->pop_help () ;
	for ( int a = 0 ; a < pc_cache.size() ; a++ )
		{
		for ( int b = 0 ; b < pc_cache[a].size() ; b++ )
			delete pc_cache[a][b] ;
		}
	}

void TProteolysis::select_fragments ( const wxArrayInt &ai )
    {
    results->Select ( wxNOT_FOUND ) ;
    for ( int a = 0 ; a < ai.GetCount() ; a++ )
        {
        results->Select ( ai[a] ) ;
        }
    }

void TProteolysis::find_cutting_proteases ()
	{
	for ( int a = 0 ; a < ls->pr.GetCount() ; a++ )
		{
		TProteaseArray p1 ;
		TProteaseCutArray apc ;
		p1.push_back ( ls->pr[a] ) ;
		determine_cuts ( p1 , apc ) ;
		if ( apc.size() > 0 ) cutting_proteases.push_back ( ls->pr[a] ) ;
		}
	}

void TProteolysis::recalc ()
	{
	Freeze () ;
	calc_cut_list () ;
	calc_fragment_list () ;
	Thaw () ;
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
	
	pc.clear () ;
	wxBeginBusyCursor () ;
	TProteaseArray prop ;
	for ( max_dep = 0 ; max_dep < sep_num_prot->GetSelection() + 1 ; max_dep++ )
		calc_spearation_sub ( max_dep + 1 , prop , tobe ) ;
	sort ( suggestions.begin() , suggestions.end() ) ;
	for ( a = 0 ; a < 20 && a < suggestions.size() ; a++ ) // Show only the best 20
		{
		sep_results->Append ( suggestions[a].name ) ;
		}
	wxEndBusyCursor () ;
	}

void TProteolysis::calc_spearation_sub ( int depth , TProteaseArray &prop , vector <TFragment> &tobe , int start )
	{
	int a , b , c , cur = prop.size() ;
	prop.push_back ( NULL ) ;
	for ( a = start ; a < cutting_proteases.size() ; a++ )
		{
		// Add this protease to the cocktail and try
		prop[cur] = cutting_proteases[a] ;
		if ( depth > 1 ) // Invoke next sub?
			{
			calc_spearation_sub ( depth - 1 , prop , tobe , a+1 ) ;
			continue ;
			}
		
		// Do the cutting!
		wxString name , desc ;
		TProteaseCutArray apc ;
		determine_cuts ( prop , apc ) ;
		remove_ignored_cuts ( apc ) ;

		// Start description
		desc += _T("* ") + wxString::Format ( txt("t_proteolysis_auto_fragments") , apc.size()+1 ) + _T("\r\n") ;
		for ( b = 0 ; b < prop.size() ; b++ )
			{
			if ( name != _T("") ) name += _T(", ") ;
			name += prop[b]->name ;
			desc += _T("* ") + prop[b]->name + _T("\r\n") ;
			}
		
		// Rate this cocktail
		int grade = max_dep ; // larger value = worse
		
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
		s.name = name + _T(" (" ) + wxString::Format ( txt("t_proteolysis_auto_fragments") , apc.size()+1 ) + _T(")") ;
		suggestions.push_back ( s ) ;
		}
	prop.pop_back () ;
	}

void TProteolysis::determine_cuts ( TProteaseArray &prop , TProteaseCutArray &apc )
	{
	int a , q , w ;
	TProteaseArray prop2 ;
	
	// Trying cache first
	for ( a = 0 ; a < prop.size() ; a++ )
		{
		for ( q = 0 ; q < pr_cache.size() && prop[a] != pr_cache[q] ; q++ ) ;
		if ( q == pr_cache.size() ) prop2.push_back ( prop[a] ) ;
		else // Read from cache
			{
			for ( w = 0 ; w < pc_cache[q].size() ; w++ )
				{
				apc.push_back ( pc_cache[q][w] ) ;
				}
			}
		}

	
	// Determine cuts
	for ( q = 0 ; q < prop2.size() ; q++ )
		{
		TProtease *pr = prop2[q] ;
		pr_cache.push_back ( pr ) ;
		TProteaseCutArray dummy ;
		for ( a = 0 ; a < v->getSequenceLength() ; a++ )
			{
			wxString s = v->getSequence().Left(a) ;
			if ( pr->len() <= s.length() )
				{
				wxString w2 = s.substr ( s.length() - pr->len() , pr->len() ) ;
				if ( pr->does_match ( w2 ) )
					{
					TProteaseCut *cut = new TProteaseCut ;
					cut->protease = pr ;
					cut->cut = s.length()-pr->len()+pr->cut+1 ;
					cut->left = false ; // Unused
					apc.push_back ( cut ) ;
					dummy.push_back ( cut ) ;
					}
				}
			}
		pc_cache.push_back ( dummy ) ;
		}
	}
	
void TProteolysis::remove_ignored_cuts ( TProteaseCutArray &apc )
	{
	// Remove ignored cuts
	for ( int a = 0 ; a < v->items.size() ; a++ )
		{
		if ( !ignore->IsChecked ( a ) ) continue ; // Not checked
		for ( int q = 0 ; q < apc.size() ; q++ )
			{
			if ( apc[q]->cut < v->items[a].from ) continue ; // Cuts before item
			if ( apc[q]->cut >= v->items[a].to ) continue ; // Cuts after item
			apc[q] = apc[apc.size()-1] ;
			apc.pop_back () ;
			q-- ;
			}
		}
	}

void TProteolysis::sort_cuts ( TProteaseCutArray &apc )
	{
	// Sort cuts
	for ( int a = 0 ; a+1 < pc.size() ; a++ )
		{
		if ( pc[a]->cut > pc[a+1]->cut )
			{
			TProteaseCut *temp = pc[a] ;
			pc[a] = pc[a+1] ;
			pc[a+1] = temp ;
			a -= 2 ;
			if ( a < -1 ) a = -1 ;
			}
		}
	}

void TProteolysis::add_final_piece ( TProteaseCutArray &apc )
	{
	// Add final piece
	if ( apc.size() && apc[apc.size()-1]->cut != v->getSequenceLength() )
		{
		TProteaseCut *cut = new TProteaseCut ;
		cut->protease = NULL ;
		cut->cut = v->getSequenceLength() ;
		cut->left = false ; // Unused
		apc.push_back ( cut ) ;
		}
	}

void TProteolysis::calc_cut_list ()
	{
	int nop = proteases->GetCount() ;
	int a , q ;
	
	TProteaseArray prop ; // PROtease Pointers
	for ( q = 0 ; q < nop ; q++ )
		{
		if ( proteases->IsChecked ( q ) )
			{
			prop.push_back ( ls->getProtease ( proteases->GetString ( q ) ) ) ;
			}
		}

	// Clear old list
	pc.clear () ;
	determine_cuts ( prop , pc ) ;
	remove_ignored_cuts ( pc ) ;
	sort_cuts ( pc ) ;
	add_final_piece ( pc ) ;

	
	// Display list
	cuts->Clear () ;
	for ( a = 0 ; a+1 < pc.size() ; a++ )
		{
		wxString s ;
		s = wxString::Format ( txt("t_proteolysis_cut_after") , pc[a]->cut , pc[a]->protease->name.c_str() ) ;
		cuts->Append ( s ) ;
		cuts->Check ( a ) ;
		}
	}

void TProteolysis::calc_fragment_list ()
{
	// Create fragment list
	fragments.clear () ;
	int a , last = 1 ;
	bool partial = partial_digestion->IsChecked() ;
	
	if ( partial ) {
        int b , c ;
        last = 1 ;
    	for ( a = 0 ; a < pc.size() ; a++ )
    	{
        	if ( a < cuts->GetCount() && !cuts->IsChecked ( a ) ) continue ;
//            if ( a == 0 ) last = 1 ; else
//            last = pc[a-1]->cut + 1 ;
            for ( b = a ; b < pc.size() ; b++ )
            {
        		if ( b < cuts->GetCount() && !cuts->IsChecked ( b ) ) continue ;
        		if ( pc[b]->cut == last ) continue ;
        		TFragment f ;
        		f.from = last ;
        		f.to = pc[b]->cut ;
        		f.length = pc[b]->cut - last + 1 ;
        		f.weight = get_weight ( f.from , f.to ) ;
        		
        		if ( f.length == v->getSequenceLength() ) continue ; // No digestion
        		for ( c = 0 ; c < fragments.size() && ( fragments[c].from != f.from || fragments[c].to != f.to ) ; c++ ) ;
        		if ( c != fragments.size() ) continue ;
        		
                fragments.push_back ( f ) ;
            }
            last = pc[a]->cut + 1 ;
    	}
    } else {
    	for ( a = 0 ; a < pc.size() ; a++ )
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
    }
	show_fragment_list () ;
	show_gel () ;
    
    wxCommandEvent ev ;
    OnSortResults ( ev ) ;
}
	
void TProteolysis::show_fragment_list ()
	{
	// Display list
	int a ;
	results->Clear () ;
	for ( a = 0 ; a < fragments.size() ; a++ )
		{
		wxString s ;
		s = wxString::Format ( _T("%d-%d (%dAA; %2.2fkD)") , 
				fragments[a].from , 
				fragments[a].to , 
				fragments[a].length ,
				fragments[a].weight ) ;
		results->Append ( s ) ;
		results->Check ( a , true ) ;
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
            if ( !results->IsChecked ( a ) ) continue ; // Don't draw unchecked fragments
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

    gel->screen.Clear() ;
    gel->logical.Clear() ;

	// Draw fragments
	for ( a = 0 ; a < fragments.size() ; a++ )
		{
        if ( !results->IsChecked ( a ) ) continue ; // Don't draw unchecked fragments
		y = get_y ( fragments[a].weight , h , min , max ) ;
		dc.DrawLine ( w/2 , y , w*9/10 , y ) ;
		gel->logical.Add ( a ) ;
		gel->screen.Add ( y ) ;
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
			wxString name = wxString::Format ( txt("t_proteolysis_fragment") , cnt ) ;
			wxString desc = wxString::Format ( txt("t_proteolysis_fragment_long") , cnt ) ;
			name += wxString::Format ( _T(" (%d-%d; %dbp; %2.2fkD)") , 
                                        fragments[a].from , 
                                        fragments[a].to ,
                                        fragments[a].length ,
                                        fragments[a].weight ) ;
			TVectorItem i ( name , 
								 desc , 
								 fragments[a].from , 
								 fragments[a].to ,
								 VIT_MISC ) ;
			if ( a & 1 ) i.setColor ( *wxGREEN ) ;
			else i.setColor ( *wxBLUE ) ;
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
			
			TVector *nv = v->newFromMark ( fragments[a].from , fragments[a].to ) ;
			nv->addName ( wxString::Format ( _T(" [%d-%d; %dbp; %2.2fkD]") , 
                                                fragments[a].from , 
                                                fragments[a].to ,
                                                fragments[a].length , 
                                                fragments[a].weight ) ) ;

			myapp()->frame->newAminoAcids ( nv , nv->getName() ) ;
			}
		}
	
   EndModal ( wxID_OK ) ; //wxDialog::OnOK ( ev ) ;
	}

void TProteolysis::OnAll ( wxCommandEvent &ev )
	{
	int a ;
	for ( a = 0 ; a < results->GetCount() ; a++ )
		results->Check ( a , true ) ;
	show_gel() ;
	}
	
void TProteolysis::OnNone ( wxCommandEvent &ev )
	{
	int a ;
	for ( a = 0 ; a < results->GetCount() ; a++ )
		results->Check ( a , false ) ;
	show_gel() ;
	}
	

void TProteolysis::OnCancel ( wxCommandEvent &ev )
	{
	EndModal ( wxID_CANCEL ) ; //wxDialog::OnCancel ( ev ) ;
	}

void TProteolysis::OnResults ( wxCommandEvent &ev )
	{
	show_gel () ;
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

	for ( a = 0 ; a < cutting_proteases.size() ; a++ )
		{
		int c = proteases->FindString ( cutting_proteases[a]->name ) ;
		if ( c == wxNOT_FOUND ) continue ; // Strange...
		bool check = false ;
		for ( b = 0 ; !check && b < suggestions[i].proteases.size() ; b++ )
			{
			if ( cutting_proteases[a] == suggestions[i].proteases[b] )
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
	else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
	else event.Skip() ;
	}

void TProteolysis::OnReport ( wxCommandEvent &ev )
{
	int a ;
    wxString html , s ;
    
    // Protease list
    html += txt("t_vec_prot") + _T(" : ") ;
	int nop = proteases->GetCount() ;
	for ( a = 0 ; a < nop ; a++ )
		{
		if ( !proteases->IsChecked ( a ) ) continue ;
		if ( !s.IsEmpty() ) s += _T(", ") ;
		s += proteases->GetString ( a ) ;
		}
	if ( partial_digestion->IsChecked() ) s += _T(" (") + txt("t_proteolysis_partial_digestion") + _T(")") ;
    html += s + _T("<br/>\n") ;
    
    html += _T("<table width=\"100%\" border=\"1\">\n") ;
	for ( a = 0 ; a < fragments.size() ; a++ )
		{
        if ( !results->IsChecked ( a ) ) continue ;
		s = wxString::Format ( _T("<tr><td>%d-%d (%dAA; %2.2fkD)<br>\n") , 
				fragments[a].from , 
				fragments[a].to , 
				fragments[a].length ,
				fragments[a].weight ) ;
		html += s ;
		s = v->getSequence().Mid ( fragments[a].from-1 , fragments[a].length ) ;
		
		html += _T("<tt><small>") ;
		while ( !s.IsEmpty() )
		{
            html += s.Left ( 10 ) + _T(" ") ;
            s = s.Mid ( 10 ) ;
        }
        html += _T("</small></tt><br/></td></tr>\n") ;
		}
	html += _T("</table>") ;
    
	html = _T("<html><body>") + html + _T("</body></html>") ;
	wxDateTime now = wxDateTime::Now() ;
	myapp()->frame->html_ep->SetHeader ( _T("<table width='100%'><tr><td width='100%'>") + v->getName() + _T("</td><td nowrap align='right'>") + now.Format() + _T("</td></tr></table>") ) ;
	myapp()->frame->html_ep->SetFooter ( _T("<p align=right>@PAGENUM@/@PAGESCNT@</p>") ) ;
	myapp()->frame->html_ep->PreviewText ( html ) ;
}

void TProteolysis::OnPartial ( wxCommandEvent &ev )
{
	Freeze () ;
	calc_fragment_list () ;
	Thaw () ;
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
