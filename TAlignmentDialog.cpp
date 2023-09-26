/** \file
	\brief Contains the methods of the TAlignmentDialog class
*/
#include "TAlignmentDialog.h"

BEGIN_EVENT_TABLE(TAlignmentDialog, wxDialog)
    EVT_BUTTON(AL_ADD,TAlignmentDialog::OnAdd)
    EVT_BUTTON(AL_DEL,TAlignmentDialog::OnDel)
    EVT_BUTTON(AL_UP,TAlignmentDialog::OnUp)
    EVT_BUTTON(AL_DOWN,TAlignmentDialog::OnDown)
    EVT_BUTTON(AL_OK,TAlignmentDialog::OnOK)
    EVT_BUTTON(AL_CANCEL,TAlignmentDialog::OnCancel)
    EVT_CHAR_HOOK(TAlignmentDialog::OnCharHook)
END_EVENT_TABLE()


// ******************************************* TAlignmentDialog

TAlignmentDialog::TAlignmentDialog(wxWindow *parent, const wxString& title )
    : wxDialog ( myapp()->frame , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
	myapp()->frame->push_help ( _T("GENtle:Alignments") ) ; // TODO : Link to dialog help page
    bo = 5 ;
    th = 15 ;
    al = (TAlignment*) parent ;
#ifndef __WXMAC__
    Show ( TRUE ) ;
#endif
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

#ifndef __WXMSW__
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;

    h0->Add ( b , 1 , wxALL , 5 ) ;
    h0->Add ( c , 1 , wxALL , 5 ) ;

    v0->Add ( nb , 1 , wxEXPAND , 5 ) ;
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

    v1->Add ( new wxStaticText ( p , -1 , _T(" ") ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxButton ( p , AL_ADD , txt("<-- add") ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxButton ( p , AL_DEL , txt("del -->") ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxStaticText ( p , -1 , _T(" ") ) , 0 , wxALIGN_CENTER_HORIZONTAL ) ;
    v1->Add ( new wxButton ( p , AL_UP , txt("b_up_in_list") ) , 0 , wxALIGN_LEFT ) ;
    v1->Add ( new wxButton ( p , AL_DOWN , txt("b_down_in_list") ) , 0 , wxALIGN_LEFT ) ;

    unsigned int a ;
    MyFrame *f = myapp()->frame ;
    // All
    for ( a = 0 ; a < f->children.GetCount() ; a++ )
        {
        if ( f->children[a]->def == _T("dna") ||
             f->children[a]->def == _T("AminoAcids") ||
             f->children[a]->def == _T("ABIviewer") )
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
/*
	// Add all when dialog is first opened
    if ( vcn.GetCount() == 0 )
        {
        for ( a = 0 ; a < van.GetCount() ; a++ )
           {
           vcn.Add ( van[a] ) ;
           vcv.Add ( vav[a] ) ;
           cur->Append ( van[a] ) ;
           }
        }
*/

    h0->Add ( v0 , 1 , wxEXPAND , 5 ) ;
    h0->Add ( v1 , 1 , wxALL , 5 ) ;
    h0->Add ( v2 , 1 , wxEXPAND , 5 ) ;
    vx->Add ( h0 , 1 , wxEXPAND ) ;
    vx->Add ( new wxStaticText ( p , -1 , txt("t_alignment_txt") ) , 0 , wxEXPAND ) ;
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

    alg_match = new wxSpinCtrl ( p , AL_MATCH , _T("") ,
                                    wxPoint ( xo , th*2 ) ,
				 wxSize ( MYSPINBOXSIZE /*w/5*/ , th*3/2 ) ) ;
    alg_match->SetRange ( -100 , 100 ) ;
    alg_match->SetValue ( al->match ) ;

    alg_mismatch = new wxSpinCtrl ( p , AL_MISMATCH , _T("") ,
                                    wxPoint ( xo , th*4 ) ,
                                    wxSize ( MYSPINBOXSIZE /*w/5*/ , th*3/2 ) ) ;
    alg_mismatch->SetRange ( -100 , 100 ) ;
    alg_mismatch->SetValue ( al->mismatch ) ;

    alg_penalty = new wxSpinCtrl ( p , AL_PENALTY , _T("") ,
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
    EndModal ( wxID_OK ) ; // wxDialog::OnOK ( ev ) ;
    }

void TAlignmentDialog::OnCancel ( wxCommandEvent &ev )
    {
    EndModal ( wxID_CANCEL ) ; // wxDialog::OnCancel ( ev ) ;
    }

void TAlignmentDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }

void TAlignmentDialog::OnAdd ( wxCommandEvent &ev )
    {
    wxArrayInt sel ;
    unsigned int a , b , n = all->GetSelections ( sel ) ;
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
    unsigned int a , b , n = cur->GetSelections ( sel ) ;
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
    unsigned int a , b , n = cur->GetSelections ( sel ) ;
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
    unsigned int a , b , n = cur->GetSelections ( sel ) ;
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

