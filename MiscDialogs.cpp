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

BEGIN_EVENT_TABLE(ProgramOptionsDialog, wxDialog )
    EVT_BUTTON(POD_OK,ProgramOptionsDialog::OnOK)
    EVT_BUTTON(POD_CANCEL,ProgramOptionsDialog::OnCancel)
    EVT_CHAR_HOOK(ProgramOptionsDialog::OnCharHook)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(FindSequenceDialog, wxDialog )
    EVT_BUTTON(SH_SEARCH,FindSequenceDialog::OnSearch)
    EVT_BUTTON(SH_CANCEL,FindSequenceDialog::OnCancel)
    EVT_CHAR_HOOK(FindSequenceDialog::OnCharHook)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TransformSequenceDialog, wxDialog )
    EVT_BUTTON(TSD_OK,TransformSequenceDialog::OnOK)
    EVT_BUTTON(TSD_CANCEL,TransformSequenceDialog::OnCancel)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(TURLtext, wxTextCtrl )
    EVT_TEXT_URL(URLTEXT_DUMMY, TURLtext::OnURL)
END_EVENT_TABLE()

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
#else
    w = 600 ;
    h = 450 ;
#endif
    nb = new wxNotebook ( this , -1 , wxPoint ( 0 , 0 ) , wxSize ( w , h-40 ) ) ;
    init_what () ;
    init_how () ;
    init_disp () ;
    wxButton *b = new wxButton ( this , AL_OK , txt("b_OK") , wxPoint ( w/3 , h-30 ) ) ;
    new wxButton ( this , AL_CANCEL , txt("b_cancel") , wxPoint ( w/2 , h-30 ) ) ;
    b->SetDefault () ;
    b->SetFocus () ;
    }
    
TAlignmentDialog::~TAlignmentDialog ()
    {
    nb->DeleteAllPages() ;
    }
    
void TAlignmentDialog::init_what ()
    {
    pwhat = new wxPanel ( this , -1 ) ;
    nb->AddPage ( pwhat , txt("t_sequences") ) ;
    wxPanel *p = pwhat ;
    int w , h ;
    p->GetClientSize ( &w , &h ) ;
    new wxStaticText ( p , -1 , txt("al_cur") , wxPoint ( bo , bo ) ) ;
    new wxStaticText ( p , -1 , txt("al_all") , wxPoint ( bo + w*2/3 , bo ) ) ;
    cur = new wxListBox ( p , AL_CUR , 
                wxPoint ( bo , bo*2+th ) , 
                wxSize ( w/3-bo , h-bo*2-th ) ,
                0 , NULL , wxLB_EXTENDED ) ;
    all = new wxListBox ( p , AL_ALL , 
                wxPoint ( bo + w*2/3 , bo*2+th ) , 
                wxSize ( w*2/3-bo , h-bo*2-th ) ,
                0 , NULL , wxLB_EXTENDED ) ;
    new wxButton ( p , AL_ADD , txt("<-- add") , wxPoint ( w*3/8 , th*2 ) , wxSize ( w/4 , th*3/2 ) ) ;
    new wxButton ( p , AL_DEL , txt("del -->") , wxPoint ( w*3/8 , th*4 ) , wxSize ( w/4 , th*3/2 ) ) ;
    new wxButton ( p , AL_UP , txt("b_up_in_list") , wxPoint ( w/3+bo , th*7 ) , wxSize ( w/6 , th*3/2 ) ) ;
    new wxButton ( p , AL_DOWN , txt("b_down_in_list") , wxPoint ( w/3+bo , th*9 ) , wxSize ( w/6 , th*3/2 ) ) ;
    
    new wxStaticText ( p , -1 , txt("t_alignment_txt") , wxPoint ( w/3+bo , th*11 ) , wxSize ( w/4 , th*4 ) ) ;
    
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
    }
    
void TAlignmentDialog::init_how ()
    {
    phow = new wxPanel ( this , -1 ) ;
    nb->AddPage ( phow , txt("t_algorithm") ) ;
    wxPanel *p = phow ;
    int w , h ;
    p->GetClientSize ( &w , &h ) ;
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
                                    wxSize ( w/5 , th*3/2 ) ) ;
    alg_match->SetRange ( -100 , 100 ) ;
    alg_match->SetValue ( al->match ) ;

    alg_mismatch = new wxSpinCtrl ( p , AL_MISMATCH , "" ,
                                    wxPoint ( xo , th*4 ) ,
                                    wxSize ( w/5 , th*3/2 ) ) ;
    alg_mismatch->SetRange ( -100 , 100 ) ;
    alg_mismatch->SetValue ( al->mismatch ) ;

    alg_penalty = new wxSpinCtrl ( p , AL_PENALTY , "" ,
                                    wxPoint ( xo , th*6 ) ,
                                    wxSize ( w/5 , th*3/2 ) ) ;
    alg_penalty->SetRange ( -100 , 100 ) ;
    alg_penalty->SetValue ( al->gap_penalty ) ;
    
    alg_matrix = new wxChoice ( p , AL_MATRIX , wxPoint ( xo , th*8 ) ) ;
    alg_matrix->Append ( txt("t_matrix_blosum") ) ;
    alg_matrix->Append ( txt("t_matrix_pam") ) ;
    alg_matrix->Append ( txt("t_matrix_gonnet") ) ;
    alg_matrix->Append ( txt("t_matrix_id") ) ;
    alg_matrix->SetSelection ( 0 ) ;
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


// *****************************************************************************

ProgramOptionsDialog::ProgramOptionsDialog(wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
    int w , h ;
    int bo = 5 , lh = 22 ;
    GetClientSize ( &w , &h ) ;
    
    wxStaticText *st ;
    st = new wxStaticText ( this , -1 , txt("t_language") , wxPoint ( bo , lh ) ) ;
    wxRect r1 = st->GetRect() ;
    
    int mw = r1.GetRight() ;
    mw += bo ;
    
    language = new wxChoice ( this , -1 , wxPoint ( mw , lh - bo ) ) ;
    enhancedDisplay = new wxCheckBox ( this , -1 , 
                        txt("t_enhanced_display") , 
                        wxPoint ( bo , lh*3 ) ) ;
    vectorTitle = new wxCheckBox ( this , -1 , 
                        txt("t_vector_title") , 
                        wxPoint ( bo , lh*4 ) ) ;
    vectorLength = new wxCheckBox ( this , -1 , 
                        txt("t_vector_length") , 
                        wxPoint ( bo , lh*5 ) ) ;
    loadLastProject = new wxCheckBox ( this , -1 , 
                        txt("t_load_last_project") , 
                        wxPoint ( bo , lh*6 ) ) ;
    useMetafile = new wxCheckBox ( this , -1 , 
                        txt("t_use_metafile") , 
                        wxPoint ( bo , lh*7 ) ) ;
    showSplashScreen = new wxCheckBox ( this , -1 , 
                        txt("t_show_splashscreen") , 
                        wxPoint ( bo , lh*8 ) ) ;
    checkUpdate = new wxCheckBox ( this , -1 , 
                        txt("t_check4update") , 
                        wxPoint ( bo , lh*9 ) ) ;
    useInternalHelp = new wxCheckBox ( this , -1 , 
                        txt("t_useinternalhelp") , 
                        wxPoint ( bo , lh*10 ) ) ;
                        
    wxButton *OK = new wxButton ( this , POD_OK , txt("b_ok") ,
                                wxPoint ( bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;
    new wxButton ( this , POD_CANCEL , txt("b_cancel") ,
                                wxPoint ( w*3/4 - bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;

    enhancedDisplay->SetValue ( myapp()->frame->enhancedRefresh ) ;
    vectorTitle->SetValue ( myapp()->frame->showVectorTitle ) ;
    vectorLength->SetValue ( myapp()->frame->showVectorLength ) ;
    loadLastProject->SetValue ( myapp()->frame->loadLastProject ) ;
    useMetafile->SetValue ( myapp()->frame->useMetafile ) ;
    showSplashScreen->SetValue ( myapp()->frame->showSplashScreen ) ;
    checkUpdate->SetValue ( myapp()->frame->checkUpdate ) ;
    useInternalHelp->SetValue ( myapp()->frame->useInternalHelp ) ;
    language->Append ( "en" ) ;
    language->Append ( "de" ) ;
    language->SetStringSelection ( myapp()->frame->lang_string ) ;

    Center () ;
    OK->SetDefault () ;
    OK->SetFocus() ;
    }

void ProgramOptionsDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void ProgramOptionsDialog::OnOK ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }

void ProgramOptionsDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }

//*************************************************** FindSequenceDialog

FindSequenceDialog::FindSequenceDialog ( wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 300 , 100 ) )
    {
    p = 0 ;
    c = (ChildBase*) parent ;
    bo = 5 ;
    fh = 20 ;
    allowed_chars = "AGCT" ; // For DNA search
    int w , h ;
    GetClientSize ( &w , &h ) ;
    t = new wxTextCtrl ( this , -1 , "" , wxPoint ( bo , bo ) ,
                            wxSize ( w-bo*2 , fh ) ) ;
                            
    wxButton *f = new wxButton ( this , SH_SEARCH , txt("b_find") , wxPoint ( bo , fh+bo*2 ) , 
                            wxSize ( w/3 , fh ) ) ;

    new wxButton ( this , SH_CANCEL , txt("b_cancel") , wxPoint ( w*2/3-bo , fh+bo*2 ) , 
                            wxSize ( w/3 , fh ) ) ;
    
    f->SetDefault () ;
    t->SetFocus () ;
    Center () ;
    int x , y ;
    GetPosition ( &x , &y ) ;
    Move ( x , y/2 ) ;
    }
    
void FindSequenceDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

bool FindSequenceDialog::doesMatch ( char a , char b )
    {
    if ( a == b ) return true ;
    if ( a == '?' || b == '?' ) return true ;
    return false ;
    }
    
int FindSequenceDialog::subsearch ( const wxString &s , const wxString &sub , int start )
    {
    int a , b ;
    for ( a = start ; a < s.length() - sub.length() + 1 ; a++ )
        {
        for ( b = 0 ; b < sub.length() ; b++ )
           {
           if ( sub[b] == '*' )
              {
              if ( subsearch ( s , sub.substr ( b+1 ) , a ) == -1 ) break ;
              b = sub.length()-1 ;
              }
           else
              {
              if ( !doesMatch ( sub[b] , s[a+b] ) ) break ;
              last = a+b ;
              }
           }
        if ( b == sub.length() ) return a ;
        }
    return -1 ;
    }

void FindSequenceDialog::OnSearch ( wxCommandEvent &ev )
    {
    int a , b ;
    wxString sub = t->GetValue() ;
    wxString dummy , allow = allowed_chars + "?*" ;
    for ( a = 0 ; a < sub.length() ; a++ ) // Filtering bogus chars
        {
        if ( sub.GetChar(a) >= 'a' && sub.GetChar(a) <= 'z' ) sub.SetChar ( a , sub.GetChar(a) - 'a' + 'A' ) ;
        for ( b = 0 ; b < allow.length() && sub.GetChar(a) != allow.GetChar(b) ; b++ ) ;
        if ( b < allow.length() ) dummy += sub.GetChar(a) ;
        }
    sub = dummy ;
    if ( sub.IsEmpty() ) return ;

    // Preparing sequence    
    wxString s = c->vec->getSequence() ;
    if ( c->vec->isCircular() )
        {
        wxString t ;
        t = s ;
        while ( t.length() < sub.length() ) t += " " ;
        s += t.substr ( 0 , sub.length()-1 ) ;
        }
        
    // Now we search...
    a = subsearch ( s , sub , p ) ;
    if ( a > -1 )
        {
        if ( c->def == "dna" )
           {
           MyChild *d = (MyChild*) c ;
           d->cSequence->mark ( "DNA" , a+1 , last+1) ;
           d->cSequence->ensureVisible ( d->cPlasmid->getMarkFrom() ) ;
           }
        else if ( c->def == "PrimerDesign" )
           {
           TPrimerDesign *d = (TPrimerDesign*) c ;
           d->sc->mark ( "PRIMER_UP" , a+1 , last+1) ;
           d->sc->ensureVisible ( d->sc->getBatchMark() ) ;
           }
        else //if ( c->def == "AminoAcids" )
           {
           TAminoAcids *d = (TAminoAcids*) c ;
           d->sc->mark ( "AA" , a+1 , last+1) ;
           d->sc->ensureVisible ( d->sc->_from ) ;
           }
        p = a+1 ;
        }
    else
        {
        wxMessageDialog md ( this , txt("t_search_reached_end") ,
                                txt("msg_box") , wxYES|wxNO|wxICON_QUESTION ) ;
        int r = md.ShowModal() ;
        if ( r == wxID_YES )
           {
           p = 0 ;
           OnSearch ( ev ) ;
           }
        }
    }

void FindSequenceDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }

