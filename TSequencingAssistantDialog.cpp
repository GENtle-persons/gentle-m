#include "TSequencingAssistantDialog.h"

BEGIN_EVENT_TABLE(TSequencingAssistantDialog, wxDialog )
    EVT_CHAR_HOOK(TSequencingAssistantDialog::OnCharHook)
    EVT_CHOICE(SAD_SEQUENCE,TSequencingAssistantDialog::OnCheckOK)
    EVT_CHOICE(SAD_ABI1,TSequencingAssistantDialog::OnCheckOK)
    EVT_CHOICE(SAD_ABI2,TSequencingAssistantDialog::OnCheckOK)
END_EVENT_TABLE()

TSequencingAssistantDialog::TSequencingAssistantDialog (wxWindow *parent , const wxString& title )
	: wxDialog ( parent , -1 , title )
	{
	myapp()->frame->push_help ( _T("GENtle/Sequencing Assistant Dialog") ) ;
	wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
	wxFlexGridSizer *h1a = new wxFlexGridSizer ( 4 , 2 , 2 , 2 ) ;
	wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;

	sequence = new wxChoice ( this , SAD_SEQUENCE ) ;
	abi1 = new wxChoice ( this , SAD_ABI1 ) ;
	abi2 = new wxChoice ( this , SAD_ABI2 ) ;
	
	// Sequence/sequencing data dropdown boxes
	h1a->Add ( new wxStaticText ( this , -1 , txt("t_sad_sequence") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	h1a->Add ( sequence , 1 , wxEXPAND|wxALL , 5 ) ;
	h1a->Add ( new wxStaticText ( this , -1 , txt("t_sad_abi1") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	h1a->Add ( abi1 , 1 , wxEXPAND|wxALL , 5 ) ;
	h1a->Add ( new wxStaticText ( this , -1 , txt("t_sad_abi2") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	h1a->Add ( abi2 , 1 , wxEXPAND|wxALL , 5 ) ;

	// OK/Cancel
    ok = new wxButton ( this , wxID_OK , txt("b_ok") ) ;
    wxButton *btCC = new wxButton ( this , wxID_CANCEL , txt("b_cancel") ) ;
    h2->Add ( ok ) ;
    h2->Add ( btCC ) ;
	
	// Initial help text
    v0->Add ( new wxStaticText ( this , -1 , txt("t_sad_text") , wxDefaultPosition , wxSize(400,60) , wxALIGN_CENTRE ) , 
               0 , wxEXPAND|wxALL , 5 ) ;
	v0->Add ( h1a , 0 , wxEXPAND|wxALL , 5 ) ;
	v0->Add ( h2 , 0 , wxCENTER|wxALL , 5 ) ;
	
	// Initialize dropdown boxes
	sequence->Append ( _T("<none>") ) ;
	abi1->Append ( _T("<none>") ) ;
	abi2->Append ( _T("<none>") ) ;
	dna.push_back ( NULL ) ;
	abi.push_back ( NULL ) ;
	for ( int a = 0 ; a < myapp()->frame->children.size() ; a++ )
		{
		ChildBase *c = myapp()->frame->children[a] ;
		if ( c->def == _T("dna") )
			{
			sequence->Append ( c->getName() ) ;
			dna.push_back ( c ) ;
			}
		else if ( c->def == _T("ABIviewer") )
			{
			abi1->Append ( c->getName() ) ;
			abi2->Append ( c->getName() ) ;
			abi.push_back ( c ) ;
			}
		}
	
	// Pre-select if only two sequencing data items are present
	sequence->Select ( 0 ) ;
	abi1->Select ( 0 ) ;
	abi2->Select ( 0 ) ;
	if ( abi1->GetCount() == 3 )
		{
		abi1->Select ( 1 ) ;
		abi2->Select ( 2 ) ;
		}
	else if ( abi1->GetCount() == 2 ) abi1->Select ( 1 ) ;
	if ( sequence->GetCount() == 2 ) sequence->Select ( 1 ) ;
	
	wxCommandEvent ev ;
	OnCheckOK ( ev ) ;
	
	// Make it so!
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;
    Center() ;
	}

TSequencingAssistantDialog::~TSequencingAssistantDialog ()
	{
    myapp()->frame->pop_help () ;
	}

void TSequencingAssistantDialog::OnCheckOK ( wxCommandEvent &ev )
	{
	bool isok = true ;
	if ( sequence->GetSelection() <= 0 ) isok = false ;
	if ( abi1->GetSelection() <= 0 && abi2->GetSelection() <= 0 ) isok = false ;
	ok->Enable ( isok ) ;
	}

void TSequencingAssistantDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) EndModal ( wxID_CANCEL ) ; //OnCancel ( ev ) ;
    else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }

void TSequencingAssistantDialog::fix_rev_compl ( int sel )
	{
	if ( sel == -2 ) // Try both
		{
		fix_rev_compl ( abi1->GetSelection() ) ;
		fix_rev_compl ( abi2->GetSelection() ) ;
		return ;
		}
	if ( sel == 0 || sel == wxNOT_FOUND ) return ; // None selected
	int sel_dna = sequence->GetSelection() ;
	if ( sel_dna == 0 || sel_dna == wxNOT_FOUND ) return ; // None selected
	TABIviewer *ab = (TABIviewer*) abi[sel] ;
	wxString s_abi = ab->vec->getSequence() ;
	wxString s_dna1 = dna[sel_dna]->vec->getStrand53() ;
	wxString s_dna2 = dna[sel_dna]->vec->getStrand35() ;
	if ( dna[sel_dna]->vec->isCircular() )
		{
		s_dna1 += s_dna1.Left ( s_abi.length() ) ;
		s_dna2 += s_dna2.Left ( s_abi.length() ) ;
		}
	
	// Try to find parts
	int a , part_length = 10 , cnt1 = 0 , cnt2 = 0 ;
	for ( a = 0 ; a < s_abi.length() ; a += part_length )
		{
		wxString part = s_abi.Mid ( a , part_length ) ;
		if ( -1 != s_dna1.Find ( part ) ) cnt1++ ;
		if ( -1 != s_dna2.Find ( part ) ) cnt2++ ;
		}
	
	if ( cnt1 < cnt2 ) ab->toggle_inv_compl() ;
	else if ( cnt1 < 3 && cnt2 < 3 ) wxMessageBox ( ab->vec->getName() + txt("t_sad_warning_mismatch") ) ;
	}
