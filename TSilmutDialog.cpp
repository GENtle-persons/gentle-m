/** \file
	\brief Contains the methods of the TSilmutDialog class, and its helper class TSilmutItem
*/
#include "TSilmutDialog.h"

BEGIN_EVENT_TABLE(TSilmutDialog, wxDialog )
    EVT_SPINCTRL(PD_SILMUT_MAX_XHG,TSilmutDialog::OnSpin)
    EVT_SPINCTRL(PD_SILMUT_MAX_CUT,TSilmutDialog::OnSpin)
    EVT_TEXT(PD_SILMUT_MAX_XHG,TSilmutDialog::OnSpin2)
    EVT_TEXT(PD_SILMUT_MAX_CUT,TSilmutDialog::OnSpin2)
    EVT_CHOICE(PD_SILMUT_EGR,TSilmutDialog::OnChoose)
    EVT_CHECKBOX(PD_SILMUT_ACR,TSilmutDialog::OnACR)
    EVT_BUTTON(PD_SILMUT_OK,TSilmutDialog::OnOK)
    EVT_BUTTON(PD_SILMUT_CANCEL,TSilmutDialog::OnCancel)
    EVT_LISTBOX_DCLICK(PD_SILMUT_LB,TSilmutDialog::OnLbDoubleClick)
    EVT_CHAR_HOOK(TSilmutDialog::OnCharHook)
END_EVENT_TABLE()

void TSilmutDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }

TSilmutDialog::TSilmutDialog ( wxWindow *parent , const wxString &s , int _mode , int _mut_pos , int _mut_dir )
    : wxDialog ( parent , -1 , s )
    {
	myapp()->frame->push_help ( _T("GENtle:Silent_Mutagenesis") ) ;
    running = false ;
    mut_pos = _mut_pos ;
    mut_dir = _mut_dir ;
    SetSize ( 600 , 400 ) ;
    Center () ;
    mode = _mode ;
    
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;


	// Max mutations (only for PCR)
	wxStaticText *st = new wxStaticText ( this , -1 , txt("t_silmut_max_xhg") ) ;
    lim_xhg = new wxSpinCtrl ( this , PD_SILMUT_MAX_XHG , _T("2") ,
                            wxDefaultPosition , wxSize ( MYSPINBOXSIZE , -1 ) ,
                            wxSP_ARROW_KEYS , 0 , 9 , 2 ) ;
    allow_cut_removal = new wxCheckBox ( this , PD_SILMUT_ACR , txt("t_silmut_allow_cut_removal") ) ;
    h0->Add ( st , 0 , wxEXPAND|wxALL , 5 ) ;
	h0->Add ( lim_xhg , 0 , wxEXPAND|wxALL , 5 ) ;
	h0->Add ( allow_cut_removal , 0 , wxEXPAND|wxALL , 5 ) ;
	
	// Max cuts
    lim_max = new wxSpinCtrl ( this , PD_SILMUT_MAX_CUT , _T("5") ,
                            wxDefaultPosition , wxSize ( MYSPINBOXSIZE , -1 ) ,
                            wxSP_ARROW_KEYS , 0 , 99 , 5 ) ;
    h1->Add ( new wxStaticText ( this , -1 , txt("t_silmut_max_cut1") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	h1->Add ( lim_max , 0 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( new wxStaticText ( this , -1 , txt("t_silmut_max_cut2") ) , 0 , wxEXPAND|wxALL , 5 ) ;

	status = new wxStaticText ( this , -1 , _T("") ) ;
	status->Hide() ;

	// Enzyme groups
    wxArrayString z ;
    myapp()->frame->LS->getEnzymeGroups ( z ) ;
    z.Add ( txt("Current") ) ;
    z.Add ( txt("All") ) ;
    z.Sort () ;
    
    egr = new wxChoice ( this , PD_SILMUT_EGR ) ;
    for ( int a = 0 ; a < z.GetCount() ; a++ )
        egr->Append ( z[a] ) ;
    
    if ( mode == M_WHATCUTS ) egr->SetStringSelection ( txt("All") ) ;
    else if ( mode == M_SILMUT ) egr->SetStringSelection ( txt("Current") ) ;
    
    h2->Add ( egr , 0 , wxEXPAND|wxALL , 5 ) ;
    h2->Add ( new wxButton ( this , PD_SILMUT_OK , txt("b_ok") ) , 0 , wxEXPAND|wxALL , 5 ) ;
    h2->Add ( new wxButton ( this , PD_SILMUT_CANCEL , txt("b_cancel") ) , 0 , wxEXPAND|wxALL , 5 ) ;
    
    if ( mut_pos > -1 ) // I have no idea what this is for...
        {
        mut = new wxChoice ( this , PD_SILMUT_EGR ) ;
        h2->Add ( mut , 0 , wxEXPAND|wxALL , 5 ) ;
        }
        
	// List
    lb = new wxListBox ( this , PD_SILMUT_LB , wxDefaultPosition , wxDefaultSize ,
                            0 , NULL , wxLB_SINGLE|wxLB_SORT ) ;
    lb->SetFont ( *MYFONT ( 8 , wxFONTFAMILY_MODERN , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;
	
	v0->Add ( h0 , 0 , wxEXPAND ) ;
	v0->Add ( h1 , 0 , wxEXPAND ) ;
	v0->Add ( h2 , 0 , wxEXPAND ) ;
	v0->Add ( lb , 1 , wxEXPAND ) ;
	v0->Add ( status , 0 , wxEXPAND ) ;
	
    if ( mode == M_WHATCUTS )
        {
        lim_xhg->SetValue ( 0 ) ;
        lim_xhg->Hide() ;
        st->Hide() ;
        allow_cut_removal->Hide() ;
        }
	
    SetSizer ( v0 ) ;

    if ( mode == M_WHATCUTS ) pd = NULL ;
    else pd = (TPrimerDesign*) parent ;
    last_selection = -1 ;
    }

TSilmutDialog::~TSilmutDialog ()
	{
	myapp()->frame->pop_help () ;
	}

void TSilmutDialog::initme ( TVector *vec , int _from , int _to )
    {
    v = vec ;
    from = _from ;
    to = _to ;
    calc () ;
    showit () ;
    running = true ;
    }
    
void TSilmutDialog::OnACR ( wxCommandEvent &ev )
    {
	OnSpin2 ( ev ) ;
	lim_xhg->Enable ( !allow_cut_removal->GetValue() ) ;
    }
    
void TSilmutDialog::OnLbDoubleClick ( wxCommandEvent &ev )
    {
    OnOK ( ev ) ;
    }
    
wxString TSilmutDialog::getSequence ()
    {
    int i = selID() ;
    if ( i == -1 ) return _T("") ;
    return vs[i].mut ;
    }
    
TRestrictionEnzyme *TSilmutDialog::getEnzyme ()
    {
    int i = selID() ;
    if ( i == -1 ) return NULL ;
    return vs[i].e ;
    }
    
int TSilmutDialog::selID ()
    {
    if ( last_selection == -1 ) return -1 ;
    int a ;
    for ( a = 0 ; a < lb_retro.GetCount() ; a++ )
       if ( lb_retro[a] == last_selection )
          return a ;
    return -1 ;
    }
    
void TSilmutDialog::OnOK ( wxCommandEvent &ev )
    {
    last_selection = lb->GetSelection () ;
    if ( last_selection < 0 )
        EndModal ( wxID_CANCEL ) ; //wxDialog::OnCancel ( ev ) ;
    else
        EndModal ( wxID_OK ) ; //wxDialog::OnOK ( ev ) ;
    }
    
void TSilmutDialog::OnCancel ( wxCommandEvent &ev )
    {
    EndModal ( wxID_CANCEL ) ; //wxDialog::OnCancel ( ev ) ;
    }
    
void TSilmutDialog::OnSpin ( wxSpinEvent &event )
    {
 wxCommandEvent event2 ;
 OnSpin2 ( event2 ) ; 
    }

void TSilmutDialog::OnSpin2 ( wxCommandEvent &event )
    {
    if ( !running ) return ;
    calc () ;
    showit () ;
    }
    
void TSilmutDialog::OnChoose ( wxCommandEvent &event )
    {
    calc () ;
    showit () ;
    }
    
void TSilmutDialog::calc ()
    {
    SetCursor ( *wxHOURGLASS_CURSOR ) ;
    bool acr = allow_cut_removal->GetValue() ;
    wxString orig_aa = getAAresult ( v->getSequence() ) ;
    wxString orig_aa_trim = orig_aa ;
    
    if ( mode == M_SILMUT && orig_aa_trim.Trim().IsEmpty() )
    	{
		status->SetLabel ( txt("t_no_reading_frame") ) ;
		status->Show ( TRUE ) ;
		SetCursor ( *wxSTANDARD_CURSOR ) ;
		return ;
		}
	else status->Hide() ;
    
    int limit = lim_xhg->GetValue() ; // Maximum number of exchanges
    int limit_cuts = lim_max->GetValue() ; // Maximum number of cuts
    vs.clear() ;
    int a , b , c ;
    int match , mismatch ;
    
    // List of restriction enzymes to check
    wxArrayTRestrictionEnzyme re ;
    wxString group = egr->GetStringSelection() ;
    if ( group == txt("Current") ) re = v->re ;
    else
        {
        wxArrayString z ;
        myapp()->frame->LS->getEnzymesInGroup ( group , z ) ;
        for ( a = 0 ; a < z.GetCount() ; a++ )
        	re.Add ( myapp()->frame->LS->getRestrictionEnzyme ( z[a] ) ) ;
        }
    
    wxString vseq = v->getSequence() ; // Marked sequence, uppercase
    wxString vseq_l = vseq.Lower() ; // Marked sequence, lowercase
    
    
    
    // Check each restriction enzyme
    for ( a = 0 ; a < re.GetCount() ; a++ )
		{
		TRestrictionEnzyme *e = re[a] ;
		wxString s = e->getSequence() ;
		int f , t ;
		f = from - s.length() + 1 ;
		t = to - 1 ;
		if ( f < 0 ) f = 0 ;
		while ( t + s.length() >= vseq.length() ) t-- ;
		for ( b = f ; b <= t ; b++ )
			{
			match = mismatch = 0 ;
			wxString x ;
			for ( c = 0 ; c < s.length() && mismatch <= limit ; c++ )
				{
				if ( v->basematch ( vseq.GetChar(b+c) , s.GetChar(c) ) )
					{
					match++ ;
					x += vseq_l.GetChar(b+c) ;
					}
				else if ( b+c < from-1 || b+c >= to )
					{
					mismatch = limit + 1 ; // Needs mutation outside specified region
					}
				else
					{
					mismatch++ ;
					x += s.GetChar(c) ;
					}
				}
			
			// Does this preserve the amino acid sequence?
			wxString new_dna ;
			bool useit = false ;
			if ( mismatch <= limit )
				{
				new_dna = vseq ;//v->getSequence() ;
				for ( c = 0 ; c < new_dna.length() ; c++ )
					{
					if ( c >= b && c < b + s.length() && 
						c-b >= 0 && c-b <= x.length() &&
						x.GetChar(c-b) >= 'A' &&
						x.GetChar(c-b) <= 'Z' ) 
						new_dna.SetChar ( c ,  x.GetChar(c-b) ) ;
					}
				wxString new_aa = getAAresult ( new_dna ) ;
				if ( new_aa == orig_aa ) useit = true ;
				}

			
			if ( acr && mismatch != 0 ) useit = false ; // Shorcut if used as "first stage"
			
			if ( !useit ) continue ; // Don't use it!

			// Add it to the list
			wxString y ;
			for ( c = from-1 ; c < to ; c++ )
				{
				if ( c >= b && c < b + s.length() ) y += x.GetChar(c-b) ;
				else y += vseq_l.GetChar(c) ; 
				}
			TSilmutItem si ;
			si.e = e ;
			si.changes = mismatch ;
			si.mut = y ;
			wxString old_dna = v->getSequence() ;
			v->setSequence ( new_dna ) ;
			int ndl = new_dna.length() ;
			vector <TRestrictionCut> vc ;
			v->getCuts ( e , vc , true ) ;
			
			// Calculating the resulting fragments
			wxArrayInt fragments ;
			fragments.Alloc ( vc.size() + 5 ) ;
			int now , last = -1 ;
			for ( c = 0 ; c < vc.size() ; c++ )
				{
				now = vc[c].getPos() ;
				if ( now > ndl ) now -= ndl ;
				if ( now == last ) continue ;
				last = now ;
				fragments.Add ( now ) ;
				}
			fragments.Sort(cmpint) ;
			now = ndl-1 ;
			if ( last != now ) fragments.Add ( now ) ;
			
			for ( c = fragments.GetCount()-1 ; c > 0 ; c-- ) fragments[c] -= fragments[c-1] ;
			fragments[0]++ ;

			if ( v->isCircular() )
				{
				fragments[0] += fragments.Last() ;
				fragments.RemoveAt ( fragments.GetCount()-1 ) ;
				}

			fragments.Sort(cmpint) ; // Sort fragments by size
			
			int cnt , sum = 0 ;
			for ( cnt = 0 ; cnt < fragments.size() ; cnt++ ) sum += fragments[cnt] ;
			fragments.Add ( sum ) ;
			
			si.fragments = fragments ;
			
			si.cuts = vc.size() ;
			v->setSequence ( old_dna ) ;
			if ( vc.size() <= limit_cuts ) vs.push_back ( si ) ;

			}
		}
    
    if ( acr ) calc_acr () ;
    
    SetCursor ( *wxSTANDARD_CURSOR ) ;
    }

void TSilmutDialog::calc_acr ()
    {
    wxString orig_aa = getAAresult ( v->getSequence() ) ;
//    int limit = lim_xhg->GetValue() ; // Maximum number of exchanges
    int limit_cuts = lim_max->GetValue() ; // Maximum number of cuts
    int a , b , c ;
//    int match , mismatch ;

    
    // List of restriction enzymes to check
	// Only those which already exist from previous run ("first stage")
    wxArrayTRestrictionEnzyme re ;
	for ( a = 0 ; a < vs.size() ; a++ ) re.Add ( vs[a].e ) ;
    vs.clear() ;
    
    wxString vseq = v->getSequence() ; // Marked sequence, uppercase
    wxString vseq_l = vseq ; // Marked sequence, lowercase
    for ( a = 0 ; a < vseq.length() ; a++ )
        {
        if ( vseq.GetChar(a) >= 'A' && vseq.GetChar(a) <= 'Z' )
           vseq_l.SetChar ( a , vseq.GetChar(a) - 'A' + 'a' ) ;
        }
    
    wxString bases = _T("ACTG") ;
    
    // Check each restriction enzyme
    for ( a = 0 ; a < re.GetCount() ; a++ )
        {
        TRestrictionEnzyme *e = re[a] ;
		vector <TRestrictionCut> vc_base ;
		v->getCuts ( e , vc_base ) ;
		
		// Sanity checks
		if ( vc_base.size() == 0 ) continue ;
		if ( vc_base.size() > limit_cuts ) continue ;

        for ( b = from-1 ; b <= to ; b++ )
           {
			for ( int base = 0 ; base < 4 ; base++ ) 
				{
				wxString new_dna = v->getSequence() ;
				new_dna.SetChar ( b , bases.GetChar ( base ) ) ;
				wxString new_aa = getAAresult ( new_dna ) ;
				if ( new_aa != orig_aa ) continue ;
				
				// This is a silent mutation
				vector <TRestrictionCut> vc_new ;
				v->setSequence ( new_dna ) ;
				v->getCuts ( e , vc_new ) ;
				v->setSequence ( vseq ) ;
				
				// Did this reduce the number of cuts?
				if ( vc_base.size() <= vc_new.size() ) continue ;
				
				wxString y ;
				for ( c = from-1 ; c < to ; c++ )
					{
					if ( b == c ) y += bases.GetChar ( base ) ; 
					else y += vseq_l.GetChar(c) ; 
					}

				TSilmutItem si ;
				si.e = e ;
				si.changes = 1 ;
				si.mut = y ;

				// Calculating the resulting fragments
				si.fragments.Alloc ( vc_new.size() + 5 ) ;
				for ( c = 0 ; c < vc_new.size() ; c++ ) si.fragments.Add ( vc_new[c].getCut() ) ;
				si.fragments.Add ( v->getSequenceLength()-1 ) ;
				for ( c = si.fragments.GetCount()-1 ; c > 0 ; c-- )
				si.fragments[c] -= si.fragments[c-1] ;
				si.fragments[0]++ ;
				if ( v->isCircular() )
					{
					si.fragments[0] += si.fragments.Last() ;
					si.fragments.RemoveAt ( si.fragments.GetCount()-1 ) ;
					}
				si.fragments.Sort(cmpint) ;
				
				si.cuts = vc_new.size() ;
				vs.push_back ( si ) ;
				}	
			}
        }
	}

wxString TSilmutDialog::getAAresult ( wxString dna )
    {
    if ( !pd )
        {
        wxString s = dna ;
        for ( int a = 0 ; a < s.length() ; a++ ) s.SetChar ( a , ' ' ) ;
        return s ;
        }
    TVector v2 ;
    v2.setFromVector ( *v ) ;
    v2.setSequence ( dna ) ;
    SeqAA *a3 ;
    a3 = new SeqAA ( pd->sc ) ;
    a3->mode = pd->aa_state ;
    a3->disp = pd->aa_disp ;
    a3->unknownAA = ' ' ;
    a3->initFromTVector ( &v2 ) ;
    a3->showNumbers = false ;
    wxString ret = a3->s ;
    delete a3 ;
    return ret ;
    }
    
void TSilmutDialog::showit ()
    {
    int a , b ;
    lb->Clear() ;
    wxArrayString list ;
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        vector <TRestrictionCut> vc ;
        wxString e_name = vs[a].e->getName() ;
        while ( e_name.length() < 8 ) e_name += ' ' ;
        v->getCuts ( vs[a].e , vc ) ;
        int cuts_before = vc.size() ;

        wxString fragments ;
        for ( b = 0 ; b < vs[a].fragments.GetCount() ; b++ )
           {
           if ( b > 0 ) fragments += _T(", ") ;
           fragments += wxString::Format ( _T("%d") , vs[a].fragments[b] ) ;
           }
        
        wxString s ;
        if ( mode == M_SILMUT )
           {
           s = wxString::Format ( _T("%2d %s %s (%2d=>%2d) [%s]") ,
                        vs[a].changes , 
                        e_name.c_str() ,
                        vs[a].mut.c_str() ,
                        cuts_before ,
                        vs[a].cuts ,
                        fragments.c_str() ) ;
           }
        else if ( mode == M_WHATCUTS )
           {
           s = wxString::Format ( txt("t_whatcuts_format") , 
                        e_name.c_str() ,
                        vs[a].cuts ,
                        fragments.c_str() ) ;
           }
           
        lb->Append ( s ) ;
        list.Add ( s ) ;
        }

    lb_retro.Clear() ;
    for ( a = 0 ; a < list.GetCount() ; a++ )
        {
        lb_retro.Add ( lb->FindString ( list[a] ) ) ;
        }
    }

