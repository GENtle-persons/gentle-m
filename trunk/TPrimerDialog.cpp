/** \file
	\brief Contains the methods of the TPrimerDialog class
*/
#include "TPrimerDialog.h"

BEGIN_EVENT_TABLE(TPrimerDialog, wxDialog )
    EVT_BUTTON(PD_OK,TPrimerDialog::OnOK)
    EVT_BUTTON(PD_SEARCH,TPrimerDialog::OnSearch)
    EVT_BUTTON(PD_RESET,TPrimerDialog::OnReset)
    EVT_BUTTON(PD_CANCEL,TPrimerDialog::OnCancel)
    EVT_LIST_ITEM_ACTIVATED(PD_DIAG_LC,TPrimerDialog::OnActivatePrimer)

    EVT_SPINCTRL(PD_DIAG_LC,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_3L,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_5L,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_3R,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_5R,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_MINLEN,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_MAXLEN,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_MINDEG,TPrimerDialog::OnSpinChanged)
    EVT_SPINCTRL(PD_DIAG_MAXDEG,TPrimerDialog::OnSpinChanged)

    EVT_CHAR_HOOK(TPrimerDialog::OnCharHook)
END_EVENT_TABLE()

bool operator < ( const TPrimer &p1 , const TPrimer &p2 )
    {
    TPrimer *x1 = (TPrimer*) &p1 ;
    TPrimer *x2 = (TPrimer*) &p2 ;
    return x1->getEvaluation() > x2->getEvaluation() ;
    }
    
bool operator == ( const TPrimer &p1 , const TPrimer &p2 )
    {
    TPrimer *x1 = (TPrimer*) &p1 ;
    TPrimer *x2 = (TPrimer*) &p2 ;
    return x1->getEvaluation() == x2->getEvaluation() ;
    }
    

TPrimerDialog::TPrimerDialog(wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 760 , 500 ) )
    {
    Center() ;
    cp = NULL ;
    }
    
TPrimerDialog::~TPrimerDialog()
    {
    if ( cp ) delete cp ;
    }
    
void TPrimerDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

wxSpinCtrl *TPrimerDialog::addSpinCtrl ( int id , wxString title , int x , int y , int def )
    {
    wxSpinCtrl *n ;
    wxStaticText *t ;
    int w = 140 ;
    x *= w ;
    x += 10 ;
    y *= 30 ;
    y -= 5 ;
    new wxStaticText ( this , -1 , title , wxPoint ( x , y ) ) ;
    x += w - 80 ;
    y -= 3 ;
    n = new wxSpinCtrl ( this , id , "0" , wxPoint ( x, y ) , wxSize ( MYSPINBOXSIZE , 20 ) ) ;
    char s[100] ;
    sprintf ( s , "%d" , def ) ;
    n->SetValue ( s ) ;
    return n ;
    }
    
void TPrimerDialog::initme ( TPrimer *_p , TPrimerDesign *_pd )
    {
    p = _p ;
    pd = _pd ;
    cp = new TPrimer ;
    SetTitle ( p->sequence ) ;
    
    int w , h ;
    GetClientSize ( &w , &h ) ;
    
    new wxStaticBox ( this , -1 , txt("t_limits") , wxPoint ( 5 , 5 ) , wxSize ( 290 , 135 ) ) ;
    
    int mintmp = 45 , maxtmp = 65 ;
    int minlen = 20 , maxlen = 40 ;
    int varlen = 5 ;
    
    l5 = addSpinCtrl ( PD_DIAG_5L , txt("t_5_l") , 0 , 1 , varlen ) ;
    r5 = addSpinCtrl ( PD_DIAG_5R , txt("t_5_r") , 1 , 1 , varlen ) ;
    l3 = addSpinCtrl ( PD_DIAG_3L , txt("t_3_l") , 0 , 2 , varlen ) ;
    r3 = addSpinCtrl ( PD_DIAG_3R , txt("t_3_r") , 1 , 2 , varlen ) ;
    lmin = addSpinCtrl ( PD_DIAG_MINLEN , txt("t_minlen") , 0 , 3 , minlen ) ;
    lmax = addSpinCtrl ( PD_DIAG_MAXLEN , txt("t_maxlen") , 1 , 3 , maxlen ) ;    
    tmin = addSpinCtrl ( PD_DIAG_MINDEG , txt("t_mintmp") , 0 , 4 , mintmp ) ;
    tmax = addSpinCtrl ( PD_DIAG_MAXDEG , txt("t_maxtmp") , 1 , 4 , maxtmp ) ;    
    
    lmin->SetRange ( 1 , 99 ) ;
    lmax->SetRange ( 1 , 99 ) ;
    
//    wxButton *bs = new wxButton ( this , PD_SEARCH , txt("t_recalc") , wxPoint ( 300 , 10 ) ) ;
    wxButton *br = new wxButton ( this , PD_RESET , txt("t_reset") , wxPoint ( 5 , 150 ) ) ;
    wxButton *bo = new wxButton ( this , PD_OK , txt("b_ok") , wxPoint ( 105 , 150 ) ) ;
    wxButton *bc = new wxButton ( this , PD_CANCEL , txt("b_cancel") , wxPoint ( 205 , 150 ) ) ;
    
    seq = new wxTextCtrl ( this , -1 , "" , 
                        wxPoint ( 0 , 180 ) ,
                        wxSize ( w , 20 ) ,
                        wxTE_READONLY|wxTE_CENTRE ) ;
                        
    rep = new wxTextCtrl ( this , -1 , "" , 
                        wxPoint ( 300 , 10 ) ,
                        wxSize ( w-300 , 160 ) ,
                        wxTE_READONLY|wxTE_MULTILINE|wxTE_DONTWRAP ) ;
                        
    lc = new wxListCtrl ( this , PD_DIAG_LC , 
                            wxPoint ( 0 , 210 ) , 
                            wxSize ( w , h-210 ) , 
                            wxLC_REPORT|wxLC_SINGLE_SEL ) ;

    rep->SetFont ( *MYFONT ( 8 , wxMODERN , wxNORMAL , wxNORMAL ) ) ;
                        
    bo->SetDefault() ;
                        
    p->annealingVector = pd->vec ;
    p->evaluate ( ( mintmp + maxtmp ) / 2 ) ;
    *cp = *p ;
    cp->evaluate ( ( mintmp + maxtmp ) / 2 ) ;
    updateList() ;
    }
    
void TPrimerDialog::OnSpinChanged ( wxSpinEvent &ev )
    {
    updateList () ;
    }
    
void TPrimerDialog::OnActivatePrimer ( wxListEvent& event )
    {
    int i = event.GetIndex () ;
    *cp = pl[i] ;
    ShowCur () ;
    }
    
void TPrimerDialog::OnOK ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }

void TPrimerDialog::OnReset ( wxCommandEvent &ev )
    {
    *cp = *p ;
    ShowCur() ;
    }

void TPrimerDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;    
    }

void TPrimerDialog::OnSearch ( wxCommandEvent &ev )
    {
    updateList () ;
    }    

void TPrimerDialog::updateList ()
    {
    while ( pl.size() ) pl.pop_back () ;
    
    wxString s , t ;
    int a , b ;
    int l1 , l2 , r1 , r2 ;
    int minlen = lmin->GetValue() ;
    int maxlen = lmax->GetValue() ;
    int mintmp = tmin->GetValue() ;
    int maxtmp = tmax->GetValue() ;
    if ( p->upper )
        {
        t = pd->vec->getSequence() ;
        s = t ;
        for ( a = 0 ; a < p->sequence.length() ; a++ )
           s.SetChar( a+p->from-1 , p->sequence.GetChar(a) ) ;
        l1 = p->from - 1 - l5->GetValue() ;
        l2 = p->from - 1 + r5->GetValue() ;
        r1 = p->to - 1 - l3->GetValue() ;
        r2 = p->to - 1 + r3->GetValue() ;
        }
    else // !!!! incomplete!
        {
        t = pd->vc->getSequence() ;
        s = t ; 
        for ( a = 0 ; a < p->sequence.length() ; a++ )
           s.SetChar ( a+p->from-1 , p->sequence.GetChar(a) ) ;
        l1 = p->from - 1 - l3->GetValue() ;
        l2 = p->from - 1 + r3->GetValue() ;
        r1 = p->to - 1 - l5->GetValue() ;
        r2 = p->to - 1 + r5->GetValue() ;
        }
        
    for ( a = l1 ; a <= l2 ; a++ )
       {
       for ( b = r1 ; b <= r2 ; b++ )
          {
          int len = b - a + 1 ;
          if ( a >= 0 && len >= minlen && len <= maxlen )
             {
             TPrimer np ( a+1 , b+1 , p->upper ) ;
             np.sequence = s.substr ( a , b-a+1 ) ;
             np.annealingVector = pd->vec ;
             np.makeStats() ;
             np.evaluate ( ( mintmp + maxtmp ) / 2 ) ;
             if ( np.getTm() >= mintmp && np.getTm() <= maxtmp )
                pl.push_back ( np ) ;
             }
          }
       }
       
    sort ( pl.begin() , pl.end() ) ;    
    ShowLC() ;
    lc->SetFocus() ;
    }


void TPrimerDialog::ShowCur ()
    {
    wxString cur = cp->sequence ;
    if ( p->upper ) seq->SetValue ( "5'-" + cur + "-3'" ) ;
    else seq->SetValue ( "3'-" + cur + "-5'" ) ;
    rep->SetValue ( cp->report() ) ;
    }
    
void TPrimerDialog::ShowLC ()
    {
    ShowCur () ;

    lc->ClearAll () ;
    lc->InsertColumn ( 0 , txt("#") ) ;
    lc->InsertColumn ( 1 , txt("length") ) ;
    lc->InsertColumn ( 2 , txt("h_tm") ) ;
    lc->InsertColumn ( 3 , txt("score") ) ;
    lc->InsertColumn ( 4 , txt("sequence") ) ;

    if ( pl.size() == 0 ) return ;

    char t[100] ;
    for ( int a = 0 ; a < pl.size() ; a++ )
        {
        int len = pl[a].to - pl[a].from + 1 ;
        
        sprintf ( t , "%d" , a+1 ) ;
        long l = lc->InsertItem ( a , t ) ;
        
        sprintf ( t , "%d" , len ) ;
        lc->SetItem ( l , 1 , t ) ;
        
        sprintf ( t , "%2.1f" , pl[a].getTm() ) ;
        lc->SetItem ( l , 2 , t ) ;
        
        sprintf ( t , "%2.1f" , pl[a].getEvaluation() ) ;
        lc->SetItem ( l , 3 , t ) ;

        lc->SetItem ( l , 4 , pl[a].sequence ) ;
        }
        
    lc->SetColumnWidth ( 0 , wxLIST_AUTOSIZE ) ;
    lc->SetColumnWidth ( 1 , wxLIST_AUTOSIZE_USEHEADER ) ;
    lc->SetColumnWidth ( 2 , wxLIST_AUTOSIZE_USEHEADER ) ;
    lc->SetColumnWidth ( 3 , wxLIST_AUTOSIZE_USEHEADER ) ;
    lc->SetColumnWidth ( 4 , wxLIST_AUTOSIZE_USEHEADER ) ;
    /*
    int cw = lc->GetClientRect().GetWidth() ;
    cw -= lc->GetColumnWidth ( 0 ) ;
    cw -= lc->GetColumnWidth ( 1 ) ;
    cw -= lc->GetColumnWidth ( 2 ) ;
    cw -= lc->GetColumnWidth ( 3 ) ;
    lc->SetColumnWidth ( 4 , cw ) ;*/
    }
