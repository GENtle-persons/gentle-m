#include "TSilmutDialog.h"

BEGIN_EVENT_TABLE(TSilmutDialog, wxDialog )
    EVT_SPINCTRL(PD_SILMUT_MAX_XHG,TSilmutDialog::OnSpin)
    EVT_SPINCTRL(PD_SILMUT_MAX_CUT,TSilmutDialog::OnSpin)
    EVT_CHOICE(PD_SILMUT_EGR,TSilmutDialog::OnChoose)
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
    else event.Skip() ;
    }

TSilmutDialog::TSilmutDialog ( wxWindow *parent , const wxString &s , int _mode , int _mut_pos , int _mut_dir )
    : wxDialog ( parent , -1 , s )
    {
    mut_pos = _mut_pos ;
    mut_dir = _mut_dir ;
    SetSize ( 600 , 400 ) ;
    Center () ;
    mode = _mode ;
    
    int w , h ;
    GetClientSize ( &w , &h ) ;
    int bo = 5 ;
    int th = 20 ;
    
    lb = new wxListBox ( this , PD_SILMUT_LB , wxPoint ( bo , th*4 ) , 
                            wxSize (  w - bo*2 , h - th*4 - bo ) ,
                            0 , NULL , 
                            wxLB_SINGLE|wxLB_SORT ) ;

    wxRect r ;
    wxStaticText *st ;
    st = new wxStaticText ( this , -1 , txt("t_silmut_max_xhg") , wxPoint ( bo , bo*2 ) ) ;
    r = st->GetRect() ;
    lim_xhg = new wxSpinCtrl ( this , PD_SILMUT_MAX_XHG , "2" ,
                            wxPoint ( r.GetRight() + bo , bo ) ,
                            wxSize ( th * 2 , th ) ,
                            wxSP_ARROW_KEYS ,
                            0 , 9 , 2 ) ;

    if ( mode == M_WHATCUTS )
        {
        lim_xhg->SetValue ( 0 ) ;
        lim_xhg->Hide() ;
        st->Hide() ;
        }

    r = lim_xhg->GetRect() ;
    st = new wxStaticText ( this , -1 , txt("t_silmut_max_cut1") , wxPoint ( r.GetRight()+th , bo*2 ) ) ;
    r = st->GetRect() ;
    lim_max = new wxSpinCtrl ( this , PD_SILMUT_MAX_CUT , "5" ,
                            wxPoint ( r.GetRight() + bo , bo ) ,
                            wxSize ( th * 2 , th ) ,
                            wxSP_ARROW_KEYS ,
                            0 , 99 , 5 ) ;
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
        
    if ( mode == M_WHATCUTS ) egr->SetStringSelection ( txt("All") ) ;
    else if ( mode == M_SILMUT ) egr->SetStringSelection ( txt("Current") ) ;
    
    r = egr->GetRect() ;
    wxButton *bd ;
    bd = new wxButton ( this , PD_SILMUT_OK , txt("b_ok") , wxPoint ( r.GetRight()+th , r.GetTop() ) ) ;
    r = bd->GetRect() ;
    bd = new wxButton ( this , PD_SILMUT_CANCEL , txt("b_cancel") , wxPoint ( r.GetRight()+th , r.GetTop() ) ) ;
    r = bd->GetRect() ;
    
    if ( mut_pos > -1 )
        {
        mut = new wxChoice ( this , PD_SILMUT_EGR , wxPoint ( r.GetRight() + bo , r.GetTop() ) ) ;
        }

    wxFont myFont ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ;
    lb->SetFont ( myFont ) ;
    pd = (TPrimerDesign*) parent ;
    last_selection = -1 ;
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
    int i = selID() ;
    if ( i == -1 ) return "" ;
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
    for ( a = 0 ; a < lb_retro.size() ; a++ )
       if ( lb_retro[a] == last_selection )
          return a ;
    return -1 ;
    }
    
void TSilmutDialog::OnOK ( wxCommandEvent &ev )
    {
    last_selection = lb->GetSelection () ;
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
    vector <wxString> list ;
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
        
        wxString s ;
        if ( mode == M_SILMUT )
           {
           s = wxString::Format ( "%2d %s %s (%2d=>%2d) [%s]" , 
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
        list.push_back ( s ) ;
        }

    lb_retro.clear() ;
    for ( a = 0 ; a < list.size() ; a++ )
        {
        lb_retro.push_back ( lb->FindString ( list[a] ) ) ;
        }
    }

