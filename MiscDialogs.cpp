#include "MiscDialogs.h"

BEGIN_EVENT_TABLE(TSilmutDialog, wxDialog )
    EVT_SPINCTRL(PD_SILMUT_MAX_XHG,TSilmutDialog::OnSpin)
    EVT_SPINCTRL(PD_SILMUT_MAX_CUT,TSilmutDialog::OnSpin)
    EVT_CHOICE(PD_SILMUT_EGR,TSilmutDialog::OnChoose)
    EVT_BUTTON(PD_SILMUT_OK,TSilmutDialog::OnOK)
    EVT_BUTTON(PD_SILMUT_CANCEL,TSilmutDialog::OnCancel)
    EVT_LISTBOX_DCLICK(PD_SILMUT_LB,TSilmutDialog::OnLbDoubleClick)
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

// TSilmutDialog


TSilmutDialog::TSilmutDialog ( wxWindow *parent , const wxString &s )
    : wxDialog ( parent , -1 , s )
    {
    SetSize ( 600 , 400 ) ;
    Center () ;
    
    int w , h ;
    GetClientSize ( &w , &h ) ;
    int bo = 5 ;
    int th = 20 ;
    
    lb = new wxListBox ( this , PD_SILMUT_LB , wxPoint ( bo , th*4 ) , 
                            wxSize (  w - bo*2 , h - th*4 - bo ) ,
                            0 , NULL , 
                            wxLB_SINGLE|wxLB_SORT ) ;

    wxRect r ;
    r = ( new wxStaticText ( this , -1 , txt("t_silmut_max_xhg") , wxPoint ( bo , bo*2 ) ) ) -> GetRect() ;
    lim_xhg = new wxSpinCtrl ( this , PD_SILMUT_MAX_XHG , "2" ,
                            wxPoint ( r.GetRight() + bo , bo ) ,
                            wxSize ( th * 2 , th ) ,
                            wxSP_ARROW_KEYS ,
                            0 , 9 , 2 ) ;
    r = lim_xhg->GetRect() ;
    r = ( new wxStaticText ( this , -1 , txt("t_silmut_max_cut1") , wxPoint ( r.GetRight()+th , bo*2 ) ) ) -> GetRect() ;
    lim_max = new wxSpinCtrl ( this , PD_SILMUT_MAX_CUT , "3" ,
                            wxPoint ( r.GetRight() + bo , bo ) ,
                            wxSize ( th * 2 , th ) ,
                            wxSP_ARROW_KEYS ,
                            0 , 99 , 3 ) ;
    r = lim_max->GetRect() ;
    new wxStaticText ( this , -1 , txt("t_silmut_max_cut2") , wxPoint ( r.GetRight()+bo , bo*2 ) ) ;
    
    vector <string> z ;
    myapp()->frame->LS->getEnzymeGroups ( z ) ;
    z.push_back ( txt("Current") ) ;
    z.push_back ( txt("All") ) ;
    sort ( z.begin() , z.end() ) ;
    
    r = lim_xhg->GetRect() ;
    egr = new wxChoice ( this , PD_SILMUT_EGR , wxPoint ( bo , r.GetBottom() + bo ) ,
                            wxSize ( r.GetRight() - bo , th ) ) ;
    for ( int a = 0 ; a < z.size() ; a++ )
        egr->Append ( z[a].c_str() ) ;
    egr->SetStringSelection ( txt("Current") ) ;
    
    r = egr->GetRect() ;
    r = (new wxButton ( this , PD_SILMUT_OK , txt("b_ok") , wxPoint ( r.GetRight()+th , r.GetTop() ) ) ) -> GetRect() ;
    r = (new wxButton ( this , PD_SILMUT_CANCEL , txt("b_cancel") , wxPoint ( r.GetRight()+th , r.GetTop() ) ) ) -> GetRect() ;
    

    wxFont myFont ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ;
    lb->SetFont ( myFont ) ;
    pd = (TPrimerDesign*) parent ;
    }

void TSilmutDialog::initme ( TVector *vec , int _from , int _to )
    {
    v = vec ;
    from = _from ;
    to = _to ;
    calc () ;
    showit () ;
    }
    
void TSilmutDialog::OnLbDoubleClick ( wxCommandEvent &ev )
    {
    OnOK ( ev ) ;
    }
    
string TSilmutDialog::getSequence ()
    {
    int i = lb->GetSelection () ;
    if ( i < 0 ) return "" ;
    return vs[i].mut ;
    }
    
TRestrictionEnzyme *TSilmutDialog::getEnzyme ()
    {
    int i = lb->GetSelection () ;
    if ( i < 0 ) return NULL ;
    return vs[i].e ;
    }
    
void TSilmutDialog::OnOK ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }
    
void TSilmutDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }
    
void TSilmutDialog::OnSpin ( wxSpinEvent &event )
    {
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
    string orig_aa = getAAresult ( v->sequence ) ;
    int limit ; // Maximum number of exchanges
    limit = lim_xhg->GetValue() ;
    int limit_cuts = lim_max->GetValue() ;
    vs.clear() ;
    int a , b , c ;
    int match , mismatch ;
    
    vector <TRestrictionEnzyme*> re ;
    wxString group = egr->GetStringSelection() ;
    if ( group == txt("Current") ) re = v->re ;
    else
        {
        vector <string> z ;
        myapp()->frame->LS->getEnzymesInGroup ( group.c_str() , z ) ;
        for ( a = 0 ; a < z.size() ; a++ )
           re.push_back ( myapp()->frame->LS->getRestrictionEnzyme ( z[a] ) ) ;
        }
    
    string vseq = v->sequence ;
    string vseq_l = vseq ;
    for ( a = 0 ; a < vseq.length() ; a++ )
        {
        if ( vseq[a] >= 'A' && vseq[a] <= 'Z' )
           vseq_l[a] = vseq[a] - 'A' + 'a' ;
        }
    for ( a = 0 ; a < re.size() ; a++ )
        {
        TRestrictionEnzyme *e = re[a] ;
        string s = e->sequence ;
        int f , t ;
        f = from - s.length() + 1 ;
        t = to - 1 ;
        if ( f < 0 ) f = 0 ;
        while ( t + s.length() >= vseq.length() ) t-- ;
        for ( b = f ; b <= t ; b++ )
           {
           match = mismatch = 0 ;
           string x ;
           for ( c = 0 ; c < s.length() && mismatch <= limit ; c++ )
              {
              if ( v->basematch ( vseq[b+c] , s[c] ) )
                 {
                 match++ ;
                 x += vseq_l[b+c] ;
                 }
              else if ( b+c < from-1 || b+c >= to ) mismatch = limit + 1 ; // Needs mutation outside specified region
              else
                 {
                 mismatch++ ;
                 x += s[c] ;
                 }
              }
              
           string new_dna = v->sequence ;
           bool useit = false ;
           if ( mismatch <= limit )
              {
              for ( c = 0 ; c < new_dna.length() ; c++ )
                 if ( c >= b && c < b + s.length() && 
                                  c-b >= 0 && c-b <= x.length() &&
                                  x[c-b] >= 'A' && x[c-b] <= 'Z' ) 
                    new_dna[c] = x[c-b] ;
              string new_aa = getAAresult ( new_dna ) ;
              if ( new_aa == orig_aa ) useit = true ;
              }
           else useit = false ;
           
           if ( useit )
              {
              string y ;
              for ( c = from-1 ; c < to ; c++ )
                 {
                 if ( c >= b && c < b + s.length() ) y += x[c-b] ;
                 else y += vseq_l[c] ; 
                 }
              TSilmutItem si ;
              si.e = e ;
              si.changes = mismatch ;
              si.mut = y ;
              string old_dna = v->sequence ;
              v->sequence = new_dna ;
              vector <TRestrictionCut> vc = v->getCuts(e) ;
              
              // Calculating the resulting fragments
              for ( c = 0 ; c < vc.size() ; c++ ) si.fragments.push_back ( vc[c].pos ) ;
              si.fragments.push_back ( v->sequence.length()-1 ) ;
              for ( c = si.fragments.size()-1 ; c > 0 ; c-- )
                 si.fragments[c] -= si.fragments[c-1] ;
              si.fragments[0]++ ;
              if ( v->isCircular() )
                 {
                 si.fragments[0] += si.fragments[si.fragments.size()-1] ;
                 si.fragments.pop_back () ;
                 }
              sort ( si.fragments.begin() , si.fragments.end() ) ;

              si.cuts = vc.size() ;
              v->sequence = old_dna ;
              if ( vc.size() <= limit_cuts )
                 vs.push_back ( si ) ;
              }
           }
        }
    SetCursor ( *wxSTANDARD_CURSOR ) ;
    }
    
string TSilmutDialog::getAAresult ( string dna )
    {
    TVector v2 = *v ;
    v2.sequence = dna ;
    SeqAA *a3 = new SeqAA ( pd->sc ) ;
    a3->mode = pd->aa_state ;
    a3->disp = pd->aa_disp ;
    a3->unknownAA = ' ' ;
    a3->initFromTVector ( &v2 ) ;
    a3->showNumbers = false ;
    string ret = a3->s ;
    delete a3 ;
    return ret ;
    }
    
void TSilmutDialog::showit ()
    {
    int a , b ;
    lb->Clear() ;
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        vector <TRestrictionCut> vc ;
        wxString e_name = vs[a].e->name.c_str() ;
        while ( e_name.length() < 8 ) e_name += ' ' ;
        vc = v->getCuts(vs[a].e) ;
        int cuts_before = vc.size() ;

        wxString fragments ;
        for ( b = 0 ; b < vs[a].fragments.size() ; b++ )
           {
           if ( b > 0 ) fragments += ", " ;
           fragments += wxString::Format ( "%d" , vs[a].fragments[b] ) ;
           }
        
        wxString s = wxString::Format ( "%2d %s %s (%2d=>%2d) [%s]" , 
                        vs[a].changes , 
                        e_name.c_str() ,
                        vs[a].mut.c_str() ,
                        cuts_before ,
                        vs[a].cuts ,
                        fragments.c_str() ) ;
        lb->Append ( s ) ;
        }
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
    if ( url == "" ) return ; // No url
    
//    string appl = myapp()->getFileFormatApplication ( "pdb" ) ;
//    wxMessageBox ( url , appl.c_str() ) ;
    
    wxExecute ( myapp()->getHTMLCommand ( url.c_str() ).c_str() ) ;
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
   if ( message != "" )
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
    
    eb_name = new wxTextCtrl ( this , -1 , e->name.c_str() , 
                wxPoint ( x1 , bo ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_seq  = new wxTextCtrl ( this , -1 , e->sequence.c_str() , 
                wxPoint ( x1 , bo+lh ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_loc  = new wxTextCtrl ( this , -1 , e->location.c_str() , 
                wxPoint ( x1 , bo+lh*2 ) , wxSize ( w-bo-x1 , lh-bo ) ) ;
    eb_note = new wxTextCtrl ( this , -1 , e->note.c_str() , 
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
    f = (MyFrame*) parent ;
    GetClientSize ( &w , &h ) ;
    
    wxStaticText *st ;
    st = new wxStaticText ( this , -1 , "Language" , wxPoint ( bo , lh ) ) ;
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
                        
    wxButton *OK = new wxButton ( this , POD_OK , txt("b_ok") ,
                                wxPoint ( bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;
    new wxButton ( this , POD_CANCEL , txt("b_cancel") ,
                                wxPoint ( w*3/4 - bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;

    enhancedDisplay->SetValue ( f->enhancedRefresh ) ;
    vectorTitle->SetValue ( f->showVectorTitle ) ;
    vectorLength->SetValue ( f->showVectorLength ) ;
    loadLastProject->SetValue ( f->loadLastProject ) ;
    useMetafile->SetValue ( f->useMetafile ) ;
    showSplashScreen->SetValue ( f->showSplashScreen ) ;
    checkUpdate->SetValue ( f->checkUpdate ) ;
    language->Append ( "en" ) ;
    language->Append ( "de" ) ;
    language->SetStringSelection ( f->lang_string.c_str() ) ;

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
    
int FindSequenceDialog::subsearch ( const string &s , const string &sub , int start )
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
    string sub = t->GetValue().c_str() ;
    string dummy , allow = allowed_chars + "?*" ;
    for ( a = 0 ; a < sub.length() ; a++ ) // Filtering bogus chars
        {
        if ( sub[a] >= 'a' && sub[a] <= 'z' ) sub[a] = sub[a] - 'a' + 'A' ;
        for ( b = 0 ; b < allow.length() && sub[a] != allow[b] ; b++ ) ;
        if ( b < allow.length() ) dummy += sub[a] ;
        }
    sub = dummy ;
    if ( sub == "" ) return ;

    // Preparing sequence    
    string s = c->vec->sequence ;
    if ( c->vec->isCircular() )
        {
        string t ;
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
           d->cSequence->ensureVisible ( d->cPlasmid->mark_from ) ;
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

