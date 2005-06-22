/** \file
	\brief Contains the class members for various dialogs
*/
#include "MiscDialogs.h"
#include <wx/sound.h>

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
    EVT_CHAR_HOOK(TransformSequenceDialog::OnCharHook)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TURLtext, wxTextCtrl )
    EVT_TEXT_URL(URLTEXT_DUMMY, TURLtext::OnURL)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TIPCDialog, wxDialog )
	EVT_TEXT(TIPC_DIA_LIMIT,TIPCDialog::OnLimit)
    EVT_BUTTON(AL_OK,wxDialog::OnOK)
    EVT_BUTTON(AL_CANCEL,wxDialog::OnCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TSpeakDialog, wxDialog )
    EVT_BUTTON(SPEAK_PLAY,TSpeakDialog::OnPlay)
    EVT_BUTTON(SPEAK_STOP,TSpeakDialog::OnStop)
    EVT_CHAR_HOOK(TSpeakDialog::OnCharHook)
END_EVENT_TABLE()

// ******************************************* TSpeakDialog

TSpeakDialog::TSpeakDialog(wxWindow *parent, const wxString& title , wxString _seq )
    : wxDialog ( parent , -1 , title )
    {
	sequence = _seq ;
	doPlay = false ;
	
	wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
	wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
	
	seq = new wxTextCtrl ( this , -1 , sequence , 
			   		wxDefaultPosition , wxDefaultSize ,
#ifndef __WXMAC__
					wxTE_READONLY|
#endif
			   		wxTE_NOHIDESEL ) ;
	
	wxButton *b1 = new wxButton ( this , SPEAK_PLAY , txt("b_speak_play") ) ;
	wxButton *b2 = new wxButton ( this , SPEAK_STOP , txt("b_speak_stop") ) ;
	
	h0->Add ( b1 , 1 , wxEXPAND|wxALL , 5 ) ;
	h0->Add ( b2 , 1 , wxEXPAND|wxALL , 5 ) ;
	
	doPause = new wxCheckBox ( this , -1 , txt("t_speak_do_pause") ) ;
	pause = new wxSpinCtrl ( this , -1 ) ;
	doPause->SetValue ( true ) ;
	pause->SetRange ( 1 , 10 ) ;
	pause->SetValue ( _T("3") ) ;

	h1->Add ( doPause , 0 , wxEXPAND|wxALL , 5 ) ;
	h1->Add ( pause , 0 , wxEXPAND|wxALL , 5 ) ;
	h1->Add ( new wxStaticText ( this , -1 , txt("t_speak_pause") ) , 0 , wxEXPAND|wxALL , 5 ) ;
	
	v0->Add ( seq , 0 , wxEXPAND|wxALL , 5 ) ;
	v0->Add ( h1 , 0 , wxEXPAND|wxALL , 5 ) ;
	v0->Add ( h0 , 0 , wxEXPAND|wxALL , 5 ) ;

	
	
	SetSizer ( v0 ) ;
	v0->Fit ( this ) ;
	
	seq->SetSelection ( -1 , -1 ) ;
//	seq->ShowPosition ( 0 ) ;
	}
	
void TSpeakDialog::OnPlay ( wxCommandEvent &ev )
	 {
	 long from , to ;
	 wxString s ;
	 int cnt = 0 ;
	 doPlay = true ;

	 while ( doPlay )
	    {
		seq->GetSelection ( &from , &to ) ;
		s = seq->GetStringSelection () ;
		
		if ( s == _T("") )
		   {
 	   	   doPlay = false ;
		   seq->SetSelection ( -1 , -1 ) ;
		   break ;
		   }
		
		speakLetter ( s.Left ( 1 ) ) ;
		seq->SetSelection ( from+1 , to ) ;
		myapp()->Yield() ;
		if ( doPause->IsChecked() && ++cnt >= pause->GetValue() )
		   {
 	   	   cnt = 0 ;
 	   	   wxSleep ( 1 ) ;
 	   	   }
		}
	 }

void TSpeakDialog::OnStop ( wxCommandEvent &ev )
	 {
	 doPlay = false ;
	 }

void TSpeakDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TSpeakDialog::speakLetter ( wxString c )
	{
	wxString file = myapp()->homedir + myapp()->slash ;
#ifndef __WXMAC__
	file += _T("wav/") ;
#endif
	file += c.Left(1).Upper() + _T(".wav") ;
//	wxMessageBox ( file ) ;
	wxSound sound ( file ) ;
	if ( !sound.IsOk() ) { wxMessageBox ( txt("t_error") ) ; return ; }
	sound.Play ( wxSOUND_SYNC ) ;
  	}

// ******************************************* TIPCDialog

#define POWER(base,exponent) exp ( exponent * log ( base ) )

TIPCDialog::TIPCDialog(wxWindow *parent, const wxString& title , int _seqlen )
    : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 300 , 100 ) )
	{
	seqlen = _seqlen ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    
    limit = new wxTextCtrl ( this , TIPC_DIA_LIMIT , _T("") ) ;
    h0->Add ( new wxStaticText ( this , -1 , txt("t_limit") ) , 0 , wxALL , 5 ) ;
    h0->Add ( limit , 0 , wxALL , 5 ) ;
    
    est = new wxStaticText ( this , -1 , _T("") ) ;
    
    h1->Add ( new wxButton ( this , AL_OK , txt("b_ok") ) , 0 , wxALL , 5 ) ;
    h1->Add ( new wxButton ( this , AL_CANCEL , txt("b_cancel") ) , 0 , wxALL , 5 ) ;
    
    v0->Add ( new wxStaticText ( this , -1 , title ) , 0 , wxALL , 5 ) ;
    v0->Add ( h0 , 0 , wxALL , 5 ) ;
    v0->Add ( est , 0 , wxALL , 5 ) ;
    v0->Add ( h1 , 0 , wxALL , 5 ) ;

    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;    

    limit->SetValue ( _T("1000") ) ;
	}

TIPCDialog::~TIPCDialog ()
	{
	}        

void TIPCDialog::OnLimit ( wxCommandEvent &event )
	{
	wxString s = limit->GetValue() ;
	long l ;
	s.ToLong ( &l ) ;
	double d = estimate_time ( l ) ;
	s = wxString::Format ( _T("%3.1f") , (float) d ) ;
	s = s.Trim ( false ) ;
	s = wxString::Format ( txt("t_ipc_est") , s.c_str() ) ;
	est->SetLabel ( s ) ;
	}    

double TIPCDialog::estimate_time ( int f )
	{
    double a , b ;
    a = seqlen ;
    b = seqlen ;
    a = POWER ( a , 2.424 ) ;
    b = POWER ( b , 3.27 ) ;
    a = a * 0.000002 ;
    b = b * 0.00000002 ;
    
    double e = f ;
    e = e * a + b ;
    if ( e < 0 ) e = 0 ; // Fix for very small f
    return e ; // [ms]
    }    


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
    
    wxString defdb = myapp()->frame->LS->getOption ( _T("SEQUENCINGPRIMER_DB") , myapp()->frame->LS->getDefaultDB() ) ;
    wxString defpj = myapp()->frame->LS->getOption ( _T("SEQUENCINGPRIMER_PJ") , _T("") ) ;
    bool usepj = myapp()->frame->LS->getOption ( _T("SEQUENCINGPRIMER_USE_PJ") , false ) ;
    int ml = myapp()->frame->LS->getOption ( _T("SEQUENCINGPRIMER_MIN_ALIGNMENT") , 20 ) ;

    t_ma = new wxSpinCtrl ( this , -1 , wxString::Format ( _T("%d") , ml ) , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , 20 ) ) ;
    h0->Add ( new wxStaticText ( this , -1 , txt("t_minimum_alignment") ) , 0 , wxALIGN_CENTER_VERTICAL ) ;    
    h0->Add ( t_ma , 0 , wxALL|wxALIGN_CENTER_VERTICAL , 5 ) ;
    h0->Add ( new wxStaticText ( this , -1 , _T("bp") ) , 0 , wxALIGN_CENTER_VERTICAL ) ;
    
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
	wxString sql = _T("SELECT pr_name FROM project") ;
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
	if ( cb_pj->GetValue() && pj != _T("") ) // Use project
		{
		sql = _T("SELECT * FROM project_dna WHERE pd_project=\"") + pj + _T("\"") ;
		r = db->getObject ( sql ) ;
		for ( a = 0 ; a < r.rows() ; a++ )
			{
		    int i = db_names.Index ( r[a][r["pd_database"]] ) ;
		    if ( i == wxNOT_FOUND ) continue ; // Illegal db
		    TStorage *db2 = myapp()->frame->getTempDB ( db_files[i] ) ;
		    if ( !db2 ) continue ; // Strange
		    sql = _T("SELECT dna_name,dna_sequence FROM dna WHERE dna_type=") +
					wxString::Format ( _T("%d") , TYPE_PRIMER ) + _T(" AND dna_name=\"") +
     				r[a][r["pd_dna"]]  + _T("\"") ;
		    TSQLresult r2 = db2->getObject ( sql ) ;
		    if ( r2.rows() != 1 ) continue ; // Illegal dna query		    
		    p_name.Add ( r2[0][r2["dna_name"]] ) ;
		    p_seq.Add ( r2[0][r2["dna_sequence"]] ) ;
		    }    
		}
	else
 		{
		    sql = _T("SELECT dna_name,dna_sequence FROM dna WHERE dna_type=") +
   				wxString::Format ( _T("%d") , TYPE_PRIMER ) ;
		r = db->getObject ( sql ) ;
		for ( a = 0 ; a < r.rows() ; a++ )
			{
			p_name.Add ( r[a][r["dna_name"]] ) ;
			p_seq.Add ( r[a][r["dna_sequence"]] ) ;
			}    
   		}        
    long ml = t_ma->GetValue() ;
    myapp()->frame->LS->setOption ( _T("SEQUENCINGPRIMER_MIN_ALIGNMENT") , ml ) ;
    myapp()->frame->LS->setOption ( _T("SEQUENCINGPRIMER_DB") , c_db->GetStringSelection() ) ;
    myapp()->frame->LS->setOption ( _T("SEQUENCINGPRIMER_PJ") , c_pj->GetStringSelection() ) ;
    myapp()->frame->LS->setOption ( _T("SEQUENCINGPRIMER_USE_PJ") , cb_pj->GetValue() ) ;
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
	i.setParam ( _T("AUTOMATIC") , _T("SEQUENCING PRIMER") ) ;
	i.setDirection ( dir ) ;
	i.setColor ( wxColour ( 255 , 205 + 25 * dir , 0 ) ) ; // Yellow
	i.desc = wxString::Format ( txt("t_desc_sequencing_primer") , best_score ) ;
	i.desc += _T("\n") ;
	i.desc += wxString::Format ( txt("t_desc_sequencing_primer2") , seq.length() ) ;
	i.desc += _T("\n") ;
	i.desc += i.getDirection()==1 ? txt("cw") : txt("ccw") ;
	i.desc += _T("\n") ;
	i.desc += txt("t_desc_sequencing_primer3");
	i.desc += _T("\n") ;
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
/*
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
*/

/*
// TStandbyDialog

TStandbyDialog::TStandbyDialog ( wxWindow *parent , 
                                 const wxString &s , 
                                 const wxString &t )
    : wxDialog ( parent , -1 , s )
    {
    Center () ;
    
    }
*/


//************************************

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

void TransformSequenceDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_RETURN ) OnOK ( ev ) ;
    else event.Skip() ;
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
//    char u[100] ;
    
    new wxStaticText ( this , -1 , _T("Name") , wxPoint ( bo , bo+lh/3 ) ) ;
    new wxStaticText ( this , -1 , _T("Sequence") , wxPoint ( bo , bo+lh+lh/3 ) ) ;
    new wxStaticText ( this , -1 , _T("Location") , wxPoint ( bo , bo+lh*2+lh/3 ) );
    new wxStaticText ( this , -1 , _T("Note") , wxPoint ( bo , bo+lh*3+lh/3 ) ) ;
    
    new wxStaticText ( this , -1 , _T("Cuts at") , wxPoint ( bo , bo+lh*4+lh/3 ) ) ;
    new wxStaticText ( this , -1 , _T("Overlap") , wxPoint ( bo , bo+lh*5+lh/3 ) ) ;
    
    eb_name = new wxTextCtrl ( this , -1 , e->name , 
                wxPoint ( x1 , bo ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_seq  = new wxTextCtrl ( this , -1 , e->sequence , 
                wxPoint ( x1 , bo+lh ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_loc  = new wxTextCtrl ( this , -1 , e->location , 
                wxPoint ( x1 , bo+lh*2 ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_note = new wxTextCtrl ( this , -1 , e->note , 
                wxPoint ( x1 , bo+lh*3 ) , wxSize ( w-bo-x1 , lh-bo ) ) ;

//    sprintf ( u , "%d" , e->cut ) ;
    eb_from = new wxTextCtrl ( this , -1 , wxString::Format ( _T("%d") , e->cut) , wxPoint ( x1 , bo+lh*4 ) ,
                    wxSize ( w-bo-x2 , lh-bo ) ) ;
//    sprintf ( u , "%d" , e->overlap ) ;
    eb_to   = new wxTextCtrl ( this , -1 , wxString::Format ( _T("%d") , e->overlap ) , wxPoint ( x1 , bo+lh*5 ) ,
                    wxSize ( w-bo-x2 , lh-bo ) ) ;
    
    if ( !readonly )
        {
        wxButton *b = new wxButton ( this , ED_OK , _T("OK") ,
                wxPoint ( x2a , bo+lh*4 ) , wxSize ( w-bo-x2a , lh-bo ) ) ;
        b->SetDefault() ;
        }
    new wxButton ( this , ED_CANCEL , _T("Cancel") , wxPoint ( x2a , bo+lh*5 ) ,
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
    e->cut = atoi ( eb_from->GetValue().mb_str() ) ;
    e->overlap = atoi ( eb_to->GetValue().mb_str() ) ;
    wxDialog::OnOK ( ev ) ;
    }





