#include "MiscDialogs.h"


BEGIN_EVENT_TABLE(TAlignmentDialog, wxDialog)
    EVT_BUTTON(AL_ADD,TAlignmentDialog::OnAdd)
    EVT_BUTTON(AL_DEL,TAlignmentDialog::OnDel)
    EVT_BUTTON(AL_UP,TAlignmentDialog::OnUp)
    EVT_BUTTON(AL_DOWN,TAlignmentDialog::OnDown)
    EVT_BUTTON(AL_OK,TAlignmentDialog::OnOK)
    EVT_BUTTON(AL_CANCEL,TAlignmentDialog::OnCancel)
    EVT_CHAR_HOOK(TAlignmentDialog::OnCharHook)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TMyMultipleChoiceDialog, wxDialog )
    EVT_BUTTON(MCD_OK,TMyMultipleChoiceDialog::OnOK)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TEnzymeDialog, wxDialog )
    EVT_BUTTON(ED_OK,TEnzymeDialog::OnOK)
    EVT_BUTTON(ED_CANCEL,TEnzymeDialog::OnCancel)
    EVT_CHAR_HOOK(TEnzymeDialog::OnCharHook)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TSequencingPrimerDialog, wxDialog )
    EVT_BUTTON(ED_OK,wxDialog::OnOK)
    EVT_BUTTON(ED_CANCEL,wxDialog::OnCancel)
    EVT_CHOICE(SPD_DB,TSequencingPrimerDialog::OnDB)
    EVT_CHAR_HOOK(TSequencingPrimerDialog::OnCharHook)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TransformSequenceDialog, wxDialog )
    EVT_BUTTON(TSD_OK,TransformSequenceDialog::OnOK)
    EVT_BUTTON(TSD_CANCEL,TransformSequenceDialog::OnCancel)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(TURLtext, wxTextCtrl )
    EVT_TEXT_URL(URLTEXT_DUMMY, TURLtext::OnURL)
END_EVENT_TABLE()

// ******************************************* TSequencingPrimerDialog

TSequencingPrimerDialog::TSequencingPrimerDialog (wxWindow *parent, const wxString& title )
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1a = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2a = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
    
    wxString defdb = myapp()->frame->LS->getOption ( "SEQUENCINGPRIMER_DB" , myapp()->frame->LS->getDefaultDB() ) ;
    wxString defpj = myapp()->frame->LS->getOption ( "SEQUENCINGPRIMER_PJ" , "" ) ;
    bool usepj = myapp()->frame->LS->getOption ( "SEQUENCINGPRIMER_USE_PJ" , false ) ;
    int ml = myapp()->frame->LS->getOption ( "SEQUENCINGPRIMER_MIN_ALIGNMENT" , 20 ) ;

    t_ma = new wxSpinCtrl ( this , -1 , wxString::Format ( "%d" , ml ) , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , 20 ) ) ;
    h0->Add ( new wxStaticText ( this , -1 , txt("t_minimum_alignment") ) , 0 , wxALIGN_CENTER_VERTICAL ) ;    
    h0->Add ( t_ma , 0 , wxALL|wxALIGN_CENTER_VERTICAL , 5 ) ;
    h0->Add ( new wxStaticText ( this , -1 , "bp" ) , 0 , wxALIGN_CENTER_VERTICAL ) ;
    
    c_db = new wxChoice ( this , SPD_DB ) ;
    h1->Add ( new wxStaticText ( this , -1 , txt("t_use_this_database") ) , 0 , wxEXPAND ) ;
    h1->Add ( c_db , 0 , wxEXPAND ) ;
    
    myapp()->frame->LS->getDatabaseList ( db_names , db_files ) ;
    for ( int a = 0 ; a < db_names.GetCount() ; a++ )
	    c_db->Append ( db_names[a] ) ;
    c_db->SetStringSelection ( defdb ) ;
    if ( c_db->GetSelection() == wxNOT_FOUND ) c_db->SetSelection ( 0 ) ;

    c_pj = new wxChoice ( this , -1 ) ;
    cb_pj = new wxCheckBox ( this , -1 , txt("t_use_this_project") ) ;
    h1a->Add ( cb_pj , 0 , wxEXPAND ) ;
    h1a->Add ( c_pj , 0 , wxEXPAND ) ;

    cb_clear = new wxCheckBox ( this , -1 , txt("t_clear_old_sp") ) ;
    h2->Add ( cb_clear , 0 , wxEXPAND ) ;
    
    cb_35 = new wxCheckBox ( this , -1 , txt("t_3_5") ) ;
    cb_53 = new wxCheckBox ( this , -1 , txt("t_5_3") ) ;
    h2a->Add ( new wxStaticText ( this , -1 , txt("t_find_primers") ) , 0 , wxRIGHT , 5 ) ;
    h2a->Add ( cb_53 , 0 , wxEXPAND ) ; // -->
    h2a->Add ( cb_35 , 0 , wxEXPAND ) ; // <--
    
    cb_clear->SetValue ( TRUE ) ;
    cb_35->SetValue ( TRUE ) ;
    cb_53->SetValue ( TRUE ) ;
    
    h3->Add ( new wxButton ( this , ED_OK , txt("b_ok") ) , 0 , wxEXPAND|wxALL , 5 ) ;
    h3->Add ( new wxButton ( this , ED_CANCEL , txt("b_cancel") ) , 0 , wxEXPAND|wxALL , 5 ) ;
    
    v0->Add ( h0 , 0 , wxALL , 5 ) ;
    v0->Add ( h1 , 0 , wxEXPAND|wxALL , 5 ) ;
    v0->Add ( h1a , 0 , wxEXPAND|wxALL , 5 ) ;
    v0->Add ( h2a , 0 , wxEXPAND|wxALL , 5 ) ;
    v0->Add ( h2 , 0 , wxEXPAND|wxALL , 5 ) ;
    v0->Add ( h3 , 0 , wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL , 5 ) ;
    
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;    
  
    wxCommandEvent ev ;
    OnDB ( ev ) ;
    c_pj->SetStringSelection ( defpj ) ;
    cb_pj->SetValue ( usepj ) ;

    Center () ;
    }
    
void TSequencingPrimerDialog::OnDB ( wxCommandEvent& event )
	{
	c_pj->Clear() ;
	int i = c_db->GetSelection() ;
	if ( i == wxNOT_FOUND ) return ;
	TStorage *db = myapp()->frame->getTempDB ( db_files[i] ) ;
	wxString sql = "SELECT pr_name FROM project" ;
	TSQLresult r = db->getObject ( sql ) ;
	for ( int a = 0 ; a < r.rows() ; a++ )
		c_pj->Append ( r[a][r["pr_name"]] ) ;
	}    

void TSequencingPrimerDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_RETURN ) OnOK ( ev ) ;
    else event.Skip() ;
    }
    
void TSequencingPrimerDialog::getPrimerList ( wxArrayString &p_name , wxArrayString &p_seq )
	{
	p_name.Clear () ;
	p_seq.Clear () ;
	int a ;
	TStorage *db = myapp()->frame->getTempDB ( db_files[c_db->GetSelection()] ) ;
	TSQLresult r ;
	wxString sql ;
	wxString pj = c_pj->GetStringSelection() ;
	if ( cb_pj->GetValue() && pj != "" ) // Use project
		{
		sql = "SELECT * FROM project_dna WHERE pd_project=\"" + pj + "\"" ;
		r = db->getObject ( sql ) ;
		for ( a = 0 ; a < r.rows() ; a++ )
			{
		    int i = db_names.Index ( r[a][r["pd_database"]] ) ;
		    if ( i == wxNOT_FOUND ) continue ; // Illegal db
		    TStorage *db2 = myapp()->frame->getTempDB ( db_files[i] ) ;
		    if ( !db2 ) continue ; // Strange
		    sql = "SELECT dna_name,dna_sequence FROM dna WHERE dna_type=" + 
					wxString::Format ( "%d" , TYPE_PRIMER ) + " AND dna_name=\"" +
     				r[a][r["pd_dna"]]  + "\"" ;
		    TSQLresult r2 = db2->getObject ( sql ) ;
		    if ( r2.rows() != 1 ) continue ; // Illegal dna query		    
		    p_name.Add ( r2[0][r2["dna_name"]] ) ;
		    p_seq.Add ( r2[0][r2["dna_sequence"]] ) ;
		    }    
		}
	else
 		{
		    sql = "SELECT dna_name,dna_sequence FROM dna WHERE dna_type=" + 
   				wxString::Format ( "%d" , TYPE_PRIMER ) ;
		r = db->getObject ( sql ) ;
		for ( a = 0 ; a < r.rows() ; a++ )
			{
			p_name.Add ( r[a][r["dna_name"]] ) ;
			p_seq.Add ( r[a][r["dna_sequence"]] ) ;
			}    
   		}        
    long ml = t_ma->GetValue() ;
    myapp()->frame->LS->setOption ( "SEQUENCINGPRIMER_MIN_ALIGNMENT" , ml ) ;
    myapp()->frame->LS->setOption ( "SEQUENCINGPRIMER_DB" , c_db->GetStringSelection() ) ;
    myapp()->frame->LS->setOption ( "SEQUENCINGPRIMER_PJ" , c_pj->GetStringSelection() ) ;
    myapp()->frame->LS->setOption ( "SEQUENCINGPRIMER_USE_PJ" , cb_pj->GetValue() ) ;
	}    
	
bool TSequencingPrimerDialog::matchToVector ( TVector *v , wxString name , wxString seq )
	{
	bool ret = false ;
	long ml = t_ma->GetValue() ; // Minimum length
	wxString s ;
	
 	s = v->getSequence() ;
	if ( v->isCircular() ) s += s.Left ( seq.length() - 1 ) ;
	int best_pos , best_score = 0 ;	
	if ( cb_53->GetValue() )
		best_score = findBestMatch ( s , seq , best_pos , ml ) ;
	if ( best_score > 0 )
 		{
 		addSequencingPrimer ( v , name , seq , best_pos , best_score , 1 ) ;
 		ret = true ;
 		}
	
	// opposite direction
	s = v->transformSequence ( true , true ) ;
	if ( v->isCircular() ) s += s.Left ( seq.length() - 1 ) ; // ???
	best_score = 0 ;
	if ( cb_35->GetValue() )
		best_score = findBestMatch ( s , seq , best_pos , ml ) ;
	if ( best_score > 0 )
 		{
 		addSequencingPrimer ( v , name , seq , v->getSequenceLength() - best_pos , best_score , -1 ) ;
 		ret = true ;
 		} 		
	
	
	return ret ;
	}    
	
void TSequencingPrimerDialog::addSequencingPrimer ( TVector *v , wxString name , 
						wxString seq , int best_pos , int best_score , int dir )
	{
	int from = best_pos + 1 ;
	int to = best_pos + seq.length() ;
	if ( dir == 1 ) from += seq.length() - best_score ;
	if ( dir == -1 )
		{
		from -= seq.length() ;
		to -= seq.length() ;
		to -= seq.length() - best_score ;
		}    
	TVectorItem i ( name , name , from , to , VIT_SEQUENCING ) ;
	i.setParam ( "AUTOMATIC", "SEQUENCING PRIMER" ) ;
	i.setDirection ( dir ) ;
	i.setColor ( wxColour ( 255 , 205 + 25 * dir , 0 ) ) ; // Yellow
	i.desc = wxString::Format ( txt("t_desc_sequencing_primer") , best_score ) ;
	i.desc += "\n" ;
	i.desc += wxString::Format ( txt("t_desc_sequencing_primer2") , seq.length() ) ;
	i.desc += "\n" ;
	i.desc += i.getDirection()==1 ? txt("cw") : txt("ccw") ;
	i.desc += "\n" ;
	i.desc += txt("t_desc_sequencing_primer3");
	i.desc += "\n" ;
	i.desc += seq ;
	v->items.push_back ( i ) ;
	}    

	
int TSequencingPrimerDialog::findBestMatch ( wxString &s , wxString seq , int &pos , int ml )
	{
	int a , b , cnt ;
	int best_score = 0 ;
	pos = -1 ;
	for ( a = 0 ; a + seq.length() < s.length() ; a++ )
		{
 		cnt = 0 ;
 		for ( b = 0 ; b < seq.length() ; b++ )
 			{
 			if ( seq.GetChar(b) == s.GetChar(a+b) ) cnt++ ;
 			else cnt = 0 ;
 			}    
		if ( cnt > best_score && cnt >= ml )
			{
			best_score = cnt ;
			pos = a ;
			}    
		}    
	return best_score ;
	}    
    
TSequencingPrimerDialog::~TSequencingPrimerDialog ()
    {
    }

// ******************************************* TMutationDialog

TMutationDialog::TMutationDialog (wxWindow *parent, const wxString& title , wxString _codon )
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
    codon = _codon ;
    v = new TVector ;
    aa = v->dna2aa(codon).GetChar(0) ;
    }
    
TMutationDialog::~TMutationDialog ()
    {
    delete v ;
    }

// ******************************************* TAlignmentDialog

TAlignmentDialog::TAlignmentDialog(wxWindow *parent, const wxString& title )
    : wxDialog ( myapp()->frame , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
    bo = 5 ;
    th = 15 ;
    al = (TAlignment*) parent ;
    Show ( TRUE ) ;
    int w , h ;
#ifdef __WXMSW__
    GetClientSize ( &w , &h ) ;
    h -= 30 ;
#else
    w = -1 ;
    h = -1 ;
#endif
    nb = new wxNotebook ( this , -1 , wxDefaultPosition , wxSize ( w , h ) );

    init_what () ;
    init_how () ;
//    init_disp () ;

    wxButton *b = new wxButton ( this , AL_OK , txt("b_OK") ) ;
    wxButton *c = new wxButton ( this , AL_CANCEL , txt("b_cancel") ) ;

#ifdef __WXGTK__
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;

    h0->Add ( b , 1 , wxALL , 5 ) ;
    h0->Add ( c , 1 , wxALL , 5 ) ;

    v0->Add ( nb , 1 , wxEXPAND|wxALL , 5 ) ;
    v0->Add ( h0 , 0 , wxCENTRE , 5 ) ;
    SetSizer ( v0 ) ;
#else
	b->SetSize ( w / 5 , h + 5 , -1 , -1 ) ;
	c->SetSize ( w / 5 * 3 , h + 5 , -1 , -1 ) ;
#endif
    nb->SetSelection ( 0 ) ;

    b->SetDefault () ;
    b->SetFocus () ;
    }
    
TAlignmentDialog::~TAlignmentDialog ()
    {
    nb->DeleteAllPages() ;
    }
    
void TAlignmentDialog::init_what ()
    {

	wxBoxSizer *vx = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *v2 = new wxBoxSizer ( wxVERTICAL ) ;

    pwhat = new wxPanel ( nb , -1 ) ;
    wxPanel *p = pwhat ;
    v0->Add ( new wxStaticText ( p , -1 , txt("al_cur") ) , 0 , wxEXPAND ) ;
    v2->Add ( new wxStaticText ( p , -1 , txt("al_all") ) , 0 , wxEXPAND ) ;

    cur = new wxListBox ( p , AL_CUR , 
			  wxDefaultPosition ,
			  wxDefaultSize ,
			  0 , NULL , wxLB_EXTENDED ) ;
    all = new wxListBox ( p , AL_ALL , 
			  wxDefaultPosition ,
			  wxDefaultSize ,
			  0 , NULL , wxLB_EXTENDED ) ;

    v0->Add ( cur , 1 , wxEXPAND ) ;
    v2->Add ( all , 1 , wxEXPAND ) ;

    v1->Add ( new wxStaticText ( p , -1 , " " ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxButton ( p , AL_ADD , txt("<-- add") ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxButton ( p , AL_DEL , txt("del -->") ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxStaticText ( p , -1 , " " ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxButton ( p , AL_UP , txt("b_up_in_list") ) , 0 , wxALIGN_LEFT ) ;
    v1->Add ( new wxButton ( p , AL_DOWN , txt("b_down_in_list") ) , 0 , wxALIGN_LEFT ) ;
				  
    int a ;
    MyFrame *f = myapp()->frame ;
    // All
    for ( a = 0 ; a < f->children.GetCount() ; a++ )
        {
        if ( f->children[a]->def == "dna" || 
             f->children[a]->def == "AminoAcids" ||
             f->children[a]->def == "ABIviewer" )
           {
           vav.Add ( ((MyChild*)f->children[a])->vec ) ;
           van.Add ( f->children[a]->getName() ) ;
           all->Append ( f->children[a]->getName() ) ;
           }
        }
        
    // Current
    for ( a = 0 ; a < al->lines.size() ; a++ )
        {
        if ( al->lines[a].name != txt("t_identity") )
           {
           vcv.Add ( al->lines[a].v ) ;
           vcn.Add ( al->lines[a].name ) ;
           cur->Append ( al->lines[a].name ) ;
           }
        }
        
    if ( vcn.GetCount() == 0 )
        {
        for ( a = 0 ; a < van.GetCount() ; a++ )
           {
           vcn.Add ( van[a] ) ;
           vcv.Add ( vav[a] ) ;
           cur->Append ( van[a] ) ;
           }
        }


    h0->Add ( v0 , 1 , wxEXPAND|wxALL , 5 ) ;    
    h0->Add ( v1 , 1 , wxALL , 5 ) ;    
    h0->Add ( v2 , 1 , wxEXPAND|wxALL , 5 ) ;    
    vx->Add ( h0 , 1 , wxEXPAND ) ;
    vx->Add ( new wxStaticText ( p , -1 , txt("t_alignment_txt") ) , 0 , wxEXPAND|wxALIGN_CENTER_HORIZONTAL ) ;    
    p->SetSizer ( vx ) ;
    nb->AddPage ( pwhat , txt("t_sequences") ) ;
    }
    
void TAlignmentDialog::init_how ()
    {
    phow = new wxPanel ( nb , -1 ) ;
    wxPanel *p = phow ;
    int w = 600 , h = 400 ;

    wxStaticText *st ;
    wxRect r ;
    st = new wxStaticText ( p , -1 , txt("t_algorithm") , wxPoint ( bo , th ) ) ;
    r = st->GetRect () ;
    alg = new wxListBox ( p , AL_ALG , 
                wxPoint ( bo , r.GetBottom()+bo ) , 
                wxSize ( w/2-bo , h/2 ) ,
                0 , NULL , wxLB_SINGLE ) ;
    alg->Append ( txt("t_clustal_w") ) ;
    alg->Append ( txt("t_smith_waterman") ) ;
    alg->Append ( txt("t_needleman_wunsch") ) ;
    alg->SetSelection ( al->algorithm ) ;
    
    new wxStaticText ( p , -1 , txt("t_alg_param") , wxPoint ( w/2+bo , th ) ) ;
    st = new wxStaticText ( p , -1 , txt("t_match") , wxPoint ( w/2+bo , th*2+bo ) ) ;
    r = st->GetRect() ;
    int xo = r.GetRight() ;
    
    st = new wxStaticText ( p , -1 , txt("t_mismatch") , wxPoint ( w/2+bo , th*4+bo ) ) ;
    r = st->GetRect() ;
    if ( xo < r.GetRight() ) xo = r.GetRight() ;
    
    st = new wxStaticText ( p , -1 , txt("t_gap_penalty") , wxPoint ( w/2+bo , th*6+bo ) ) ;
    r = st->GetRect() ;
    if ( xo < r.GetRight() ) xo = r.GetRight() ;
    xo += bo ;
    
    alg_match = new wxSpinCtrl ( p , AL_MATCH , "" ,
                                    wxPoint ( xo , th*2 ) ,
				 wxSize ( MYSPINBOXSIZE /*w/5*/ , th*3/2 ) ) ;
    alg_match->SetRange ( -100 , 100 ) ;
    alg_match->SetValue ( al->match ) ;

    alg_mismatch = new wxSpinCtrl ( p , AL_MISMATCH , "" ,
                                    wxPoint ( xo , th*4 ) ,
                                    wxSize ( MYSPINBOXSIZE /*w/5*/ , th*3/2 ) ) ;
    alg_mismatch->SetRange ( -100 , 100 ) ;
    alg_mismatch->SetValue ( al->mismatch ) ;

    alg_penalty = new wxSpinCtrl ( p , AL_PENALTY , "" ,
                                    wxPoint ( xo , th*6 ) ,
				   wxSize ( MYSPINBOXSIZE /*w/5*/ , th*3/2 ) ) ;
    alg_penalty->SetRange ( -100 , 100 ) ;
    alg_penalty->SetValue ( al->gap_penalty ) ;
    
    alg_matrix = new wxChoice ( p , AL_MATRIX , wxPoint ( xo , th*8 ) ) ;
    alg_matrix->Append ( txt("t_matrix_blosum") ) ;
    alg_matrix->Append ( txt("t_matrix_pam") ) ;
    alg_matrix->Append ( txt("t_matrix_gonnet") ) ;
    alg_matrix->Append ( txt("t_matrix_id") ) ;
    alg_matrix->SetSelection ( 0 ) ;
    nb->AddPage ( phow , txt("t_algorithm") ) ;
    }

void TAlignmentDialog::init_disp ()
    {
    pdisp = new wxPanel ( this , -1 ) ;
    nb->AddPage ( pdisp , txt("t_display") ) ;
    wxPanel *p = pdisp ;
    int w , h ;
    p->GetClientSize ( &w , &h ) ;
    }

void TAlignmentDialog::OnOK ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }

void TAlignmentDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }

void TAlignmentDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TAlignmentDialog::OnAdd ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = all->GetSelections ( sel ) ;
    for ( a = 0 ; a < n ; a++ )
        {
        for ( b = 0 ; b < vcv.GetCount() && vcv[b] != vav[sel[a]] ; b++ ) ;
        if ( b == vcv.GetCount() )
           {
           vcv.Add ( vav[sel[a]] ) ;
           vcn.Add ( van[sel[a]] ) ;
           cur->Append ( van[sel[a]] ) ;
           }
        }
    }

void TAlignmentDialog::OnDel ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = cur->GetSelections ( sel ) ;
    for ( a = 0 ; a < n ; a++ ) vcv[sel[a]] = NULL ;
    for ( a = 0 ; a < vcv.GetCount() ; a++ )
        {
        if ( vcv[a] == NULL )
           {
           for ( b = a+1 ; b < vcv.GetCount() ; b++ )
              {
              vcv[b-1] = vcv[b] ;
              vcn[b-1] = vcn[b] ;
              }
           vcv.RemoveAt ( vcv.GetCount()-1 ) ;
           vcn.RemoveAt ( vcn.GetCount()-1 ) ;
           a-- ;
           }
        }
    cur->Clear () ;
    for ( a = 0 ; a < vcn.GetCount() ; a++ )
        cur->Append ( vcn[a] ) ;
    }

void TAlignmentDialog::OnUp ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = cur->GetSelections ( sel ) ;
    if ( n != 1 ) return ;
    b = sel[0] ;
    if ( b == 0 ) return ;
    TVector *d_v = vcv[b] ; vcv[b] = vcv[b-1] ; vcv[b-1] = d_v ;
    wxString d_n = vcn[b] ; vcn[b] = vcn[b-1] ; vcn[b-1] = d_n ;
    cur->Clear () ;
    for ( a = 0 ; a < vcn.GetCount() ; a++ )
        cur->Append ( vcn[a] ) ;
    cur->SetSelection ( b-1 ) ;
    }

void TAlignmentDialog::OnDown ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    int a , b , n = cur->GetSelections ( sel ) ;
    if ( n != 1 ) return ;
    b = sel[0] ;
    if ( b == vcv.GetCount()-1 ) return ;
    TVector *d_v = vcv[b] ; vcv[b] = vcv[b+1] ; vcv[b+1] = d_v ;
    wxString d_n = vcn[b] ; vcn[b] = vcn[b+1] ; vcn[b+1] = d_n ;
    cur->Clear () ;
    for ( a = 0 ; a < vcn.GetCount() ; a++ )
        cur->Append ( vcn[a] ) ;
    cur->SetSelection ( b+1 ) ;
    }


// TStandbyDialog

TStandbyDialog::TStandbyDialog ( wxWindow *parent , 
                                 const wxString &s , 
                                 const wxString &t )
    : wxDialog ( parent , -1 , s )
    {
    Center () ;
    
    }



//**

TURLtext::TURLtext ( wxWindow *parent , int id , wxString title ,
                    wxPoint pos , wxSize size , int style )
    : wxTextCtrl ( parent , id , title , pos , size , 
        ( style | wxTE_RICH | wxTE_AUTO_URL ) )
    {
    }

void TURLtext::OnURL(wxTextUrlEvent& event)
    {
    wxMouseEvent me = event.GetMouseEvent() ;
    if ( !me.LeftDown() ) return ; // No left click
    long from = event.GetURLStart() ;
    long to = event.GetURLEnd() ;
    wxString url = GetRange ( from , to ) ;
    if ( url.IsEmpty() ) return ; // No url    
    wxExecute ( myapp()->getHTMLCommand ( url ) ) ;
    }


//*************************************** TMyMultipleChoiceDialog

TMyMultipleChoiceDialog::TMyMultipleChoiceDialog ()
    : wxDialog ()
    {
    clb = NULL ;
    }
    
TMyMultipleChoiceDialog::TMyMultipleChoiceDialog ( wxWindow *parent ,
                              const wxString &message ,
                              const wxString &caption ,
                              int n ,
                              const wxString choices[] ,
                              int options ,
                              const wxPoint& pos )
   : wxDialog ( parent , -1 , caption , pos )
   {
   
   
   wxSize size ( 400 , 300 ) ;
   SetSize ( size ) ;
   
   wxCoord w , h , bo = 5 ; // bo is border width
   GetClientSize ( &w , &h ) ;
           
   wxRect q ( 0 , 0 , w , 0 ) ;
   wxRect r ( 0 , h , w , h ) ;
   if ( !message.IsEmpty() )
      {
      wxStaticText *st = new wxStaticText ( this , 
                                            -1 , 
                                            message , 
                                            wxPoint ( bo , bo ) ) ;
      r = st->GetRect() ;
      }

   wxButton *buttonOK = NULL ;
   wxButton *buttonCancel = NULL ;
   
   if ( options & wxOK > 0 )
      {
      buttonOK = new wxButton ( this , MCD_OK , txt("b_ok") ) ;
      buttonOK->SetDefault () ;
      q = buttonOK->GetRect() ;
      buttonOK->Move ( w / 2 - q.GetWidth() / 2 , h - q.GetHeight() - bo ) ;
      }

   if ( options & wxCANCEL > 0 )
      {
      buttonCancel = new wxButton ( this , wxID_CANCEL , txt("b_cancel") ) ;
      q = buttonCancel->GetRect() ;
      buttonCancel->Move ( w / 2 - q.GetWidth() / 2 , h - q.GetHeight() - bo ) ;
      if ( !buttonOK ) buttonCancel->SetDefault () ;
      }
      
   if ( buttonOK && buttonCancel )
      {
      buttonOK->Move ( bo , h - q.GetHeight() - bo ) ;
      buttonCancel->Move ( w - q.GetWidth() - bo , h - q.GetHeight() - bo ) ;
      }
   
   clb = new wxCheckListBox ( this , 
                                 -1 , 
                                 wxPoint ( bo , bo + r.GetBottom() ) ,
                                 wxSize ( w - bo * 2 , h - bo * 2 - r.GetBottom() ) ,
                                 n ,
                                 choices ) ;
   if ( buttonOK || buttonCancel )
      clb->SetSize ( w - bo * 2 , h - r.GetBottom() - q.GetHeight() - bo * 3 ) ;
   
   if ( options & wxCENTRE > 0 ) Center () ;
   }

TMyMultipleChoiceDialog::~TMyMultipleChoiceDialog ()
    {
    if ( clb ) delete clb ;
    clb = NULL ;
    }
    
void TMyMultipleChoiceDialog::OnOK ( wxCommandEvent &ev )
    {
    if ( clb )
       {
        int a ;
        for ( a = 0 ; a < clb->GetCount() ; a++ )
            check.push_back ( clb->IsChecked ( a ) ) ;
        }
    wxDialog::OnOK ( ev ) ;
    }

void TMyMultipleChoiceDialog::Check ( int item, bool check )
    {
    if ( clb ) clb->Check ( item , check ) ;
    }

void TMyMultipleChoiceDialog::CheckAll ( bool check )
    {
    if ( !clb ) return ;
    int n = clb->GetCount () ;
    while ( --n >= 0 )
        clb->Check ( n , check ) ;
    }
    
bool TMyMultipleChoiceDialog::IsChecked ( int item ) const
    {
    if ( item >= 0 && item < check.size() ) return check[item] ;
    return false ;
    }

//*************************************** TransformSequenceDialog

TransformSequenceDialog::TransformSequenceDialog ( wxWindow *parent, const wxString& title )
    : wxDialog ( parent , TSD , title , wxDefaultPosition , wxSize ( 400 , 200 ) )
    {
    int bo = 5 , lh = 20 ;
    invert = new wxCheckBox ( this , -1 , txt("t_invert_sequence") , wxPoint ( bo , bo + lh * 0 ) ) ;
    complement = new wxCheckBox ( this , -1 , txt("t_complement_sequence") , wxPoint ( bo , bo + lh * 1 ) ) ;
    new_item = new wxCheckBox ( this , -1 , txt("t_as_new_item") , wxPoint ( bo , bo + lh * 2 ) ) ;
    new wxButton ( this , TSD_OK , txt("b_ok") , wxPoint ( bo , lh * 4 ) ) ;
    new wxButton ( this , TSD_CANCEL , txt("b_cancel") , wxPoint ( 100 , lh * 4 ) ) ;    
    Center() ;
    }

TransformSequenceDialog::~TransformSequenceDialog ()
    {
    delete invert ;
    delete complement ;
    delete new_item ;
    }


//*************************************** TEnzymeDialog

TEnzymeDialog::TEnzymeDialog(wxWindow *parent, const wxString& title,
                 const wxPoint& pos, const wxSize& size,
                 const long style)
         : wxDialog ( parent , ED , title , pos , size , style )
{
}

void TEnzymeDialog::initme ( TRestrictionEnzyme *_e , bool ro )
    {
    readonly = ro ;
    e = new TRestrictionEnzyme ;
    if ( _e ) *e = *_e ;
    
    // Layout
    int w = 300 , h = 190 , bo = 5 ;
    int x1 = 70 , x2 = w/2 , x2a = x2+x1 ;
    int lh = bo*6 ;
    char u[100] ;
    
    new wxStaticText ( this , -1 , "Name" , wxPoint ( bo , bo+lh/3 ) ) ;
    new wxStaticText ( this , -1 , "Sequence" , wxPoint ( bo , bo+lh+lh/3 ) ) ;
    new wxStaticText ( this , -1 , "Location" , wxPoint ( bo , bo+lh*2+lh/3 ) );
    new wxStaticText ( this , -1 , "Note" , wxPoint ( bo , bo+lh*3+lh/3 ) ) ;
    
    new wxStaticText ( this , -1 , "Cuts at" , wxPoint ( bo , bo+lh*4+lh/3 ) ) ;
    new wxStaticText ( this , -1 , "Overlap" , wxPoint ( bo , bo+lh*5+lh/3 ) ) ;
    
    eb_name = new wxTextCtrl ( this , -1 , e->name , 
                wxPoint ( x1 , bo ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_seq  = new wxTextCtrl ( this , -1 , e->sequence , 
                wxPoint ( x1 , bo+lh ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_loc  = new wxTextCtrl ( this , -1 , e->location , 
                wxPoint ( x1 , bo+lh*2 ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_note = new wxTextCtrl ( this , -1 , e->note , 
                wxPoint ( x1 , bo+lh*3 ) , wxSize ( w-bo-x1 , lh-bo ) ) ;

    sprintf ( u , "%d" , e->cut ) ;
    eb_from = new wxTextCtrl ( this , -1 , u , wxPoint ( x1 , bo+lh*4 ) , 
                    wxSize ( w-bo-x2 , lh-bo ) ) ;
    sprintf ( u , "%d" , e->overlap ) ;
    eb_to   = new wxTextCtrl ( this , -1 , u , wxPoint ( x1 , bo+lh*5 ) , 
                    wxSize ( w-bo-x2 , lh-bo ) ) ;
    
    if ( !readonly )
        {
        wxButton *b = new wxButton ( this , ED_OK , "OK" ,
                wxPoint ( x2a , bo+lh*4 ) , wxSize ( w-bo-x2a , lh-bo ) ) ;
        b->SetDefault() ;
        }
    new wxButton ( this , ED_CANCEL , "Cancel" , wxPoint ( x2a , bo+lh*5 ) , 
                wxSize ( w-bo-x2a , lh-bo ) ) ;

    SetClientSize ( w , h ) ;
    Centre() ;
    }

void TEnzymeDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TEnzymeDialog::OnOK ( wxCommandEvent &ev )
    {
    e->name = eb_name->GetValue() ;
    e->sequence = eb_seq->GetValue() ;
    e->location = eb_loc->GetValue() ;
    e->note = eb_note->GetValue() ;
    e->cut = atoi ( eb_from->GetValue().c_str() ) ;
    e->overlap = atoi ( eb_to->GetValue().c_str() ) ;
    wxDialog::OnOK ( ev ) ;
    }





