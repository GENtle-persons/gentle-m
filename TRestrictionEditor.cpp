#include "TRestrictionEditor.h"

BEGIN_EVENT_TABLE(TRestrictionEditor, wxDialog )
    EVT_LIST_ITEM_SELECTED(RSE_RES_LL, TRestrictionEditor::res_ll)
    EVT_LIST_ITEM_ACTIVATED(RSE_RES_LL, TRestrictionEditor::res_ll_act)
    EVT_CHOICE(RSE_RES_DD,TRestrictionEditor::res_dd)
    EVT_CHECKBOX(RSE_COC_CB,TRestrictionEditor::res_checkbox)
    EVT_CHECKBOX(RSE_ADD2GEL,TRestrictionEditor::res_cb_add2gel)
    EVT_RADIOBOX(RSE_RES_RB,TRestrictionEditor::res_cb)
    EVT_BUTTON(RSE_RES_AC,TRestrictionEditor::res_ac)
    EVT_BUTTON(RSE_COC_CT,TRestrictionEditor::res_ct)
    EVT_BUTTON(RSE_COC_RM,TRestrictionEditor::res_coc_rm)
    EVT_BUTTON(RSE_RES_OK,TRestrictionEditor::onOK)
    EVT_BUTTON(RSE_RES_CC,TRestrictionEditor::onCancel)
    EVT_BUTTON(RSE_ADD_ALL,TRestrictionEditor::onAddAll)
    EVT_LIST_ITEM_SELECTED(RSE_COC_LL, TRestrictionEditor::res_coc_ll)
    EVT_LIST_COL_CLICK(RSE_RES_LL,TRestrictionEditor::res_ll_col)
    EVT_CHAR_HOOK(TRestrictionEditor::OnCharHook)
END_EVENT_TABLE()

using namespace std ;

#define BY_NAME 0
#define BY_CUTS 1

int global_sort_mode ;
bool global_sort_ascending ;

bool operator < ( const TREcache c1 , const TREcache c2 )
    {
    if ( global_sort_mode == BY_NAME )
        return global_sort_ascending?(c1.enzyme<c2.enzyme):(c1.enzyme>c2.enzyme);
    else if ( global_sort_mode == BY_CUTS )
        return global_sort_ascending?(c1.cut<c2.cut):(c1.cut>c2.cut);
    }

bool operator == ( const TREcache c1 , const TREcache c2 )
    {
    if ( global_sort_mode == BY_NAME )
        return c1.enzyme == c2.enzyme ;
    else if ( global_sort_mode == BY_CUTS )
        return c1.cut == c2.cut ;
    }

//******************************************************************************    


void TRestrictionEditor::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TRestrictionEditor::res_ll_col ( wxListEvent &event )
    {
    int item = el->GetNextItem ( -1 , wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED) ;
    if ( item != -1 ) pre = el->GetItemText(item) ;

    int col = event.GetColumn() ;
    if ( col == global_sort_mode )
        global_sort_ascending = !global_sort_ascending ;
    else global_sort_mode = col ;
    pR_showGroupEnzymes ( gl->GetStringSelection() ) ;    
    }

TRestrictionEditor::TRestrictionEditor(MyFrame *parent, const wxString& title,
                 const wxPoint& pos , const wxSize& size ,
                 const long style )
         : wxDialog ( parent , -1 , title , pos , size , style )
{
    numberoffragments = 0 ;
    global_sort_mode = BY_NAME ;
    global_sort_ascending = true ;
}

void TRestrictionEditor::initme ( TVector *_v )
    {
    _w = 600 ;
    _h = 550 ;
    v = _v ;
    SetTitle ( txt("res_editor") ) ;
    initRestrictionPage () ;
    }
    
void TRestrictionEditor::initRestrictionPage ()
    {
    int w = _w , h = _h ;
    int bo = 5 , th = 12 ;
    int rw = w , rh = h ;

    wxClientDC dc ( (wxWindow*) this ) ;
    dc.GetTextExtent ( "X" , &bo , &th ) ;
    th = th * 5 / 6 ;
    bo = th / 2 ;


    wxFlexGridSizer *v1 = new wxFlexGridSizer ( 2 , 1 , 5 , 5 ) ;
    wxBoxSizer *v2a = new wxBoxSizer ( wxVERTICAL ) ;
    wxFlexGridSizer *v2b = new wxFlexGridSizer ( 6 , 1 , 5 , 5 ) ;
    wxBoxSizer *v2c = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    
    // Available enzymes list
    el = new wxListCtrl ( this , RSE_RES_LL , wxDefaultPosition , wxDefaultSize ,
                wxLC_REPORT|wxLC_SINGLE_SEL|MYLISTBORDER ) ;
    el->InsertColumn ( 0 , txt("name") ) ;
    el->InsertColumn ( 1 , txt("cuts") ) ;
    v2a->Add ( new wxStaticText ( this , -1 , txt("t_available_enzymes") ) , 0 , wxEXPAND , 5 ) ;
    v2a->Add ( el , 1 , wxEXPAND , 5 ) ;

    
    // Cocktail
    el2 = new wxListCtrl ( this , RSE_COC_LL , wxPoint ( rw*2/3+bo , th*2 ) ,
                wxSize ( rw/3-bo*2 , rh*2/3-th*2-bo ) , 
                wxLC_REPORT|wxLC_SINGLE_SEL|MYLISTBORDER ) ;
    el2->InsertColumn ( 0 , txt("name") ) ;
    el2->InsertColumn ( 1 , txt("cuts") ) ;
    v2c->Add ( new wxStaticText ( this , -1 , txt("Cocktail") ) , 0 , wxEXPAND , 5 ) ;
    v2c->Add ( el2 , 1 , wxEXPAND , 5 ) ;

    
    wxFlexGridSizer *h1a = new wxFlexGridSizer ( 1 , 2 , 5 , 5 ) ;
    wxFlexGridSizer *h1d = new wxFlexGridSizer ( 1 , 2 , 5 , 5 ) ;
    wxBoxSizer *h1e = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1f = new wxBoxSizer ( wxHORIZONTAL ) ;

    // Enzyme groups
    wxString _gt = txt("group") ;
    gl = new wxChoice ( this , RSE_RES_DD ) ;
    gl->Append ( txt("Current") ) ;
    gl->Append ( txt("All") ) ;
    wxArrayString vs ;
    myapp()->frame->LS->getEnzymeGroups ( vs ) ;
    for ( int i = 0 ; i < vs.GetCount() ; i++ )
        gl->Append ( vs[i] ) ;
    gl->SetStringSelection ( txt("Current") ) ;
    h1a->Add ( new wxStaticText ( this , -1 , _gt ) , 1 , wxEXPAND , 5 ) ;
    h1a->Add ( gl , 1 , wxEXPAND , 5 ) ;

    wxString rbs[3] ;
    rbs[0] = txt("All enzymes") ;
    rbs[1] = txt("Enzymes that cut") ;
    rbs[2] = txt("Enzymes that don't cut") ;
    rb = new wxRadioBox ( this , RSE_RES_RB , txt("subsel") ,
                          wxDefaultPosition ,
                          wxDefaultSize ,
                           3 , rbs , 1 , wxRA_SPECIFY_COLS ) ;
                           
    nfst = new wxCheckBox ( this , RSE_COC_CB , txt("Do not create fragments below ") ) ;

    nfstv = new wxSpinCtrl ( this , -1 , "20" , wxDefaultPosition , wxSize ( MYSPINBOXSIZE , 30 ) ) ;
    nfst->SetValue ( true ) ;
    nfstv->SetSize ( 70 , th * 2 ) ;
    h1d->Add ( nfstv , 0 , wxALL|wxALIGN_CENTER_VERTICAL , 5 ) ;
    h1d->Add ( new wxStaticText ( this , -1 , txt("base pairs.") ) , 
                1 , wxEXPAND|wxALIGN_CENTER_VERTICAL , 5 ) ;


	wxBoxSizer *v3a = new wxBoxSizer ( wxVERTICAL ) ;
	wxBoxSizer *v3b = new wxBoxSizer ( wxVERTICAL ) ;

    bb = new wxBitmapButton ( this , RSE_RES_AC ,
             wxBitmap (myapp()->bmpdir+"/cocktail.bmp", wxBITMAP_TYPE_BMP),
             wxDefaultPosition ,
             wxDefaultSize ,
             wxBU_AUTODRAW ,
             wxDefaultValidator ,
             txt("add2cocktail") ) ;
    wxBitmapButton *bib = new wxBitmapButton ( this , RSE_COC_CT ,
             wxBitmap (myapp()->bmpdir+"/scissors.bmp", wxBITMAP_TYPE_BMP),
             wxDefaultPosition ,
             wxDefaultSize ,
             wxBU_AUTODRAW ,
             wxDefaultValidator ,
             txt("start_res") ) ;

    v3a->Add ( bb , 1 , wxEXPAND , 5 ) ;
    v3a->Add ( new wxStaticText ( this , -1 , txt("add2cocktail") ) ) ;
    v3a->Add ( new wxStaticText ( this , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v3a->Add ( bib , 1 , wxEXPAND , 5 ) ;
    v3a->Add ( new wxStaticText ( this , -1 , txt("start_res") ) ) ;


    btOK = new wxButton ( this , RSE_RES_OK , txt("b_done") ) ;
    btCC = new wxButton ( this , RSE_RES_CC , txt("b_cancel") ) ;
    createFragments = new wxCheckBox ( this , -1 , txt("t_create_fragments") ) ;
    add2gel = new wxCheckBox ( this , RSE_ADD2GEL , txt("t_add2gel") ) ;
    oneLaneEach = new wxCheckBox ( this , -1 , txt("t_one_lane_each") ) ;
    v3b->Add ( new wxButton ( this , RSE_ADD_ALL , txt("t_add_all") ) , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( new wxStaticText ( this , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( new wxButton ( this , RSE_COC_RM , txt("Remove enzyme") ) , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( new wxStaticText ( this , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( btOK , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( btCC , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( new wxStaticText ( this , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( createFragments , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( add2gel , 0 , wxEXPAND , 5 ) ;
    v3b->Add ( oneLaneEach , 0 , wxEXPAND , 5 ) ;
    createFragments->SetValue ( TRUE ) ;
    add2gel->SetValue ( FALSE ) ;
    oneLaneEach->SetValue ( FALSE ) ;
    oneLaneEach->Disable() ;

    h1f->Add ( v3a , 0 , wxEXPAND , 5 ) ;
    h1f->Add ( new wxStaticText ( this , -1 , "   " ) , 0 , wxEXPAND , 5 ) ;
    h1f->Add ( v3b , 0 , wxEXPAND , 5 ) ;

    v2b->Add ( new wxStaticText ( this , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v2b->Add ( h1a , 1 , wxEXPAND , 5 ) ;
    v2b->Add ( rb , 1 , wxEXPAND , 5 ) ;
    v2b->Add ( nfst , 1 , wxEXPAND , 5 ) ;
    v2b->Add ( h1d , 1 , wxEXPAND , 5 ) ;
    v2b->Add ( h1e , 1 , wxEXPAND , 5 ) ;
    v2b->Add ( h1f , 1 , wxEXPAND , 5 ) ;

    h1->Add ( v2a , 1 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( v2b , 1 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( v2c , 1 , wxEXPAND|wxALL , 5 ) ;
    
    // "This enzyme cuts" list
    rsl = new wxListCtrl ( this , -1 , wxDefaultPosition , wxDefaultSize ,
                 wxLC_REPORT|wxLC_SINGLE_SEL|MYLISTBORDER ) ;
    rsl->InsertColumn ( 0 , "#" , wxLIST_FORMAT_LEFT , th*2 ) ;
    rsl->InsertColumn ( 1 , txt("from") ) ;
    rsl->InsertColumn ( 2 , txt("to") ) ;
    rsl->InsertColumn ( 3 , txt("length") ) ;
    
    // All fragments list
    rsl2 = new wxListCtrl ( this , -1 , wxDefaultPosition , wxDefaultSize ,
                  wxLC_REPORT|wxLC_SINGLE_SEL|MYLISTBORDER ) ;
    rsl2->InsertColumn ( 0 , "#" , wxLIST_FORMAT_LEFT , th*2 ) ;
    rsl2->InsertColumn ( 1 , txt("from") ) ;
    rsl2->InsertColumn ( 2 , txt("to") ) ;
    rsl2->InsertColumn ( 3 , txt("length") ) ;

    rsl->SetSize ( w/3 , h/3 ) ;

    h2->Add ( rsl , 1 , wxEXPAND|wxALL , 5 ) ;
    h2->Add ( rsl2 , 1 , wxEXPAND|wxALL , 5 ) ;
    
    v1->Add ( h1 , 1 , wxEXPAND , 5 ) ;
    v1->Add ( h2 , 1 , wxEXPAND , 5 ) ;
    
    SetSizer ( v1 ) ;
    v1->Fit ( this ) ;
    
    btOK->SetDefault() ;
    refreshCocktail () ;

    Centre() ;
    
    int i ;
    for ( i = 0 ; i < v->rc.size() ; i++ )
        cutcache.push_back ( TREcache ( v->rc[i].e->name , v->rc[i].pos ) ) ;
    
    pR_showGroupEnzymes ( txt("Current") ) ;
    
    for ( i = 0 ; i < myapp()->frame->lastCocktail.GetCount() ; i++ )
       add2cocktail ( myapp()->frame->lastCocktail[i] ) ;
    myapp()->frame->lastCocktail.Clear() ;
    }
    
void TRestrictionEditor::listEnzymesInGroup ( wxString gr , wxArrayString &vs )
	{
    if ( gr == txt("Current") )
        {
        int i ;
        for ( i = 0 ; i < v->re.GetCount() ; i++ )
            vs.Add ( v->re[i]->name ) ;
        for ( i = 0 ; i < v->re2.GetCount() ; i++ )
            vs.Add ( v->re2[i]->name ) ;
        }
    else myapp()->frame->LS->getEnzymesInGroup ( gr , vs ) ;
	}    

void TRestrictionEditor::pR_showGroupEnzymes ( wxString gr )
    {
    el->DeleteAllItems() ;
    rsl->DeleteAllItems() ;
    
    
    wxArrayString vs ;
    listEnzymesInGroup ( gr , vs ) ;

    char t[100] ;
    int sel = rb->GetSelection() ;
    bool docut = true , donotcut = true ;
    if ( sel == 1 ) donotcut = false ;
    if ( sel == 2 ) docut = false ;
    int cnt = 0 , i ;
    bool ascending = true ;
    vector <TREcache> list ;
    for ( i = 0 ; i < vs.GetCount() ; i++ )
        {
        int k = getcuts(vs[i]).GetCount() ;
        if ( ( k == 0 && donotcut ) ||
             ( k > 0 && docut ) )
           {
           list.push_back ( TREcache ( vs[i] , k ) ) ;
           }
        }

    sort ( list.begin() , list.end() ) ;
        
    for ( i = 0 ; i < list.size() ; i++ )
        {
        sprintf ( t , "%d" , list[i].cut ) ;
        int z = el->InsertItem ( cnt , list[i].enzyme ) ;
        el->SetItem ( z , 1 , t ) ;
        cnt++ ;
        }
        
        
    if ( !pre.IsEmpty() )
        {
        int pre_id = el->FindItem ( 0 , pre ) ;
        el->SetItemState ( pre_id , wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED );
        el->EnsureVisible ( pre_id ) ;
        el->SetFocus() ;
        pre = "" ;
        }
    }
    
wxArrayInt TRestrictionEditor::getcuts ( wxString enzyme )
    {
    wxArrayInt ret ;
    int j , k = 0 ;
    for ( j = 0 ; j < nocut.GetCount() && nocut[j] != enzyme ; j++ ) ;
    if ( j < nocut.GetCount() ) return ret ; // No cuts, returning empty list
    
    // Looking for cuts in cache
    for ( j = 0 ; j < cutcache.size() ; j++ )
        if ( cutcache[j].enzyme == enzyme )
            ret.Add ( cutcache[j].cut ) ;

    if ( ret.GetCount() == 0 ) // Determine cuts
        {
        TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( enzyme );
        vector <TRestrictionCut> x ;
        v->getCuts ( e , x ) ;
        if ( x.size() == 0 ) // No cuts
            {
            nocut.Add ( enzyme ) ;
            return ret ;
            }
        for ( j = 0 ; j < x.size() ; j++ )
            {
            cutcache.push_back ( TREcache ( enzyme , x[j].pos ) ) ;
            ret.Add ( x[j].pos ) ;
            }
        }
    return ret ;
    }

void TRestrictionEditor::res_ll_act ( wxListEvent &event )
    {
    int i = event.GetIndex () ;
    wxString s = el->GetItemText ( i ) ;
    TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
    TEnzymeDialog ed ( this , s , wxPoint(-1,-1) , wxSize(600,400) , 
                    wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
    ed.initme ( e , true ) ;
    ed.ShowModal() ;
    }
    
void TRestrictionEditor::res_ll ( wxListEvent &event )
    {
    int i = event.GetIndex () ;
    last_enzyme_selected = el->GetItemText ( i ) ;
    pR_showFragments ( i ) ;
    }
    
void TRestrictionEditor::pR_showFragments ( int i )
    {
    wxString enzyme = el->GetItemText ( i ) ;
    wxArrayInt vi = getcuts ( enzyme ) ;
    listFragments ( rsl , vi ) ;
    }

void TRestrictionEditor::getFragmentList ( wxArrayInt &cuts , vector <TFragment> &fragments )
	{
	fragments.clear () ;
    if ( cuts.GetCount() == 0 ) return ;
    if ( !v->isCircular() ) // Adding last fragment for linear DNA
        {
        int from = cuts.Last() ;
        int to = v->getSequenceLength() ;
        if ( from != to )
           cuts.Add ( to ) ;
        }

    int i ;
    char u[100] ;
    for ( i = 0 ; i < cuts.GetCount() ; i++ )
        {
        TFragment fr ;
        
        int from = 0 ;
        if ( i > 0 ) from = cuts[i-1] ;
        if ( i == 0 && v->isCircular() ) from = cuts.Last() ;
        fr.from = from ;
        fr.to = cuts[i] ;
        
        int len = cuts[i] - from ;
        if ( len <= 0 ) len += v->getSequenceLength() ;
        fr.length = len ;
        fragments.push_back ( fr ) ;
        }
	}    

void TRestrictionEditor::listFragments ( wxListCtrl *list , wxArrayInt &vi )
    {
    list->DeleteAllItems() ;
    if ( vi.GetCount() == 0 ) return ;
    if ( !v->isCircular() ) // Adding last fragment for linear DNA
        {
        int from = vi.Last() ;
        int to = v->getSequenceLength() ;
        if ( from != to )
           vi.Add ( to ) ;
        }
    numberoffragments = vi.GetCount() ;
        
    int i ;
    char u[100] ;
    for ( i = 0 ; i < vi.GetCount() ; i++ )
        {
        sprintf ( u , "%d" , i+1 ) ; // Number
        int k = list->InsertItem ( i , u ) ;
        
        int from = 0 ;
        if ( i > 0 ) from = vi[i-1] ;
        if ( i == 0 && v->isCircular() ) from = vi.Last() ;
        sprintf ( u , "%d" , from ) ; // From
        list->SetItem ( k , 1 , u ) ;
        
        sprintf ( u , "%d" , vi[i] ) ; // To
        list->SetItem ( k , 2 , u ) ;
        
        int len = vi[i] - from ;
        if ( len <= 0 ) len += v->getSequenceLength() ;
        sprintf ( u , "%d" , len ) ; // Length
        list->SetItem ( k , 3 , u ) ;
        }
    }
    
void TRestrictionEditor::refreshCocktail ()
    {
    wxArrayString vs = cocktail ;
    el2->DeleteAllItems() ;
    char t[100] ;
    int i , j ;
    for ( i = 0 ; i < vs.GetCount() ; i++ )
        {
        int k = getcuts(vs[i]).GetCount() ;
        sprintf ( t , "%d" , k ) ;
        int z = el2->InsertItem ( i , vs[i] ) ;
        el2->SetItem ( z , 1 , t ) ;
        }
        
    wxArrayInt vi , vit ;
    for ( i = 0 ; i < cocktail.GetCount() ; i++ )
        {
        vit = getcuts ( cocktail[i] ) ;
        for ( j = 0 ; j < vit.GetCount() ; j++ )
           vi.Add ( vit[j] ) ;
        }
    vi.Sort ( cmpint ) ;
    vit = vi ;
    vi.Clear () ;
    for ( i = 0 ; i < vit.GetCount() ; i++ )
        {
        if ( vi.GetCount() == 0 || vi.Last() != vit[i] )
           vi.Add ( vit[i] ) ;
        }
    cocktailFragments = vi ;
    listFragments ( rsl2 , vi ) ;
    }

void TRestrictionEditor::add2cocktail ( wxString s )
    {
    if ( s.IsEmpty() ) return ;
    wxString s2 = s ;
    int i ;
    for ( i = 0 ; i < cocktail.GetCount() && cocktail[i] != s2 ; i++ ) ;
    if ( i < cocktail.GetCount() ) return ; // Already in there
    cocktail.Add ( s2 ) ;
    cocktail.Sort() ;
    refreshCocktail () ;
    }

void TRestrictionEditor::del_from_cocktail ( wxString s )
    {
    int i ;
    wxString s2 = s ;
    for ( i = 0 ; i < cocktail.GetCount() && cocktail[i] != s2 ; i++ ) ;
    if ( i == cocktail.GetCount() ) return ; // Not there
    cocktail.RemoveAt ( i ) ;
    refreshCocktail () ;
    }

// "Restriction" handlers

void TRestrictionEditor::res_dd ( wxCommandEvent &event )
    { // This handles the enzyme groups dropdown list
    wxString s = event.GetString() ;
    pR_showGroupEnzymes ( s ) ;
    }

void TRestrictionEditor::res_cb ( wxCommandEvent &event )
    {
    wxString s = gl->GetStringSelection() ;
    pR_showGroupEnzymes ( s ) ;
    }

void TRestrictionEditor::res_ac ( wxCommandEvent &event )
    { // This handles the "Add to cocktail" button
    add2cocktail ( last_enzyme_selected ) ;
    }

void TRestrictionEditor::res_coc_rm ( wxCommandEvent &event )
    { // This handles the "Remove from cocktail" button
    del_from_cocktail ( last_enzyme_selected_coc ) ;
    }

void TRestrictionEditor::res_coc_ll ( wxListEvent &event )
    {
    int i = event.GetIndex () ;
    last_enzyme_selected_coc = el2->GetItemText ( i ) ;
    }

void TRestrictionEditor::onOK ( wxCommandEvent &event )
    {
    EndModal ( wxID_OK ) ;
    }

void TRestrictionEditor::onCancel ( wxCommandEvent &event )
    {
    EndModal ( wxID_CANCEL ) ;
    }

// "Cocktail" handlers

void TRestrictionEditor::res_ct ( wxCommandEvent &event )
    { // This handles the "Start restriction" button
    if ( numberoffragments == 0 )
        {
        event.Skip() ;
        return ;
        }
    
    if ( nfst->GetValue() ) v->setAction ( "RESTRICTION" , nfstv->GetValue() ) ;
    else v->setAction ( "RESTRICTION" , 0 ) ;
    
    
    
    EndModal ( wxID_OK ) ;
    }
    
void TRestrictionEditor::res_checkbox ( wxCommandEvent &event )
    {
    nfstv->Enable ( nfst->GetValue() ) ;
    }
    
void TRestrictionEditor::res_cb_add2gel ( wxCommandEvent &event )
	{
	oneLaneEach->Enable ( add2gel->GetValue() ) ;
	}    

void TRestrictionEditor::onAddAll ( wxCommandEvent &event )
	{
	wxArrayString vs ;
    wxString s = gl->GetStringSelection() ;
	listEnzymesInGroup ( s , vs ) ;
	for ( int a = 0 ; a < vs.GetCount() ; a++ )
		add2cocktail ( vs[a] ) ;
	}
     
