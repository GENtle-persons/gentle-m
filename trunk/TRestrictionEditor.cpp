#include "TRestrictionEditor.h"

BEGIN_EVENT_TABLE(TRestrictionEditor, wxDialog )
    EVT_LIST_ITEM_SELECTED(RSE_RES_LL, TRestrictionEditor::res_ll)
    EVT_LIST_ITEM_ACTIVATED(RSE_RES_LL, TRestrictionEditor::res_ll_act)
    EVT_CHOICE(RSE_RES_DD,TRestrictionEditor::res_dd)
    EVT_CHECKBOX(RSE_COC_CB,TRestrictionEditor::res_checkbox)
    EVT_RADIOBOX(RSE_RES_RB,TRestrictionEditor::res_cb)
    EVT_BUTTON(RSE_RES_AC,TRestrictionEditor::res_ac)
    EVT_BUTTON(RSE_COC_CT,TRestrictionEditor::res_ct)
    EVT_BUTTON(RSE_COC_RM,TRestrictionEditor::res_coc_rm)
    EVT_BUTTON(RSE_RES_OK,TRestrictionEditor::onOK)
    EVT_BUTTON(RSE_RES_CC,TRestrictionEditor::onCancel)
    EVT_LIST_ITEM_SELECTED(RSE_COC_LL, TRestrictionEditor::res_coc_ll)
    EVT_LIST_COL_CLICK(RSE_RES_LL,TRestrictionEditor::res_ll_col)
    EVT_CHAR_HOOK(TRestrictionEditor::OnCharHook)
END_EVENT_TABLE()

using namespace std ;

#define BY_NAME 0
#define BY_CUTS 1

int global_sort_mode ;
bool global_sort_ascending ;

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
    if ( item != -1 ) pre = el->GetItemText(item).c_str() ;

    int col = event.GetColumn() ;
    if ( col == global_sort_mode )
        global_sort_ascending = !global_sort_ascending ;
    else global_sort_mode = col ;
    pR_showGroupEnzymes ( gl->GetStringSelection().c_str() ) ;    
    }

TRestrictionEditor::TRestrictionEditor(wxMDIParentFrame *parent, const wxString& title,
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

    // Restriction
    
    // Enzyme groups
    wxString _gt = txt("group") ;
    new wxStaticText ( this , -1 , _gt , wxPoint ( bo*2+rw/3 , th+2 ) ) ;
    gl = new wxChoice ( this , RSE_RES_DD , wxPoint ( bo*2+rw/3+th*_gt.length()*2/3 , th ) ,
                wxSize ( rw/6 , th ) ) ;
    gl->Append ( txt("Current") ) ;
    gl->Append ( txt("All") ) ;
    vector <string> vs ;
    myapp()->frame->LS->getEnzymeGroups ( vs ) ;
    for ( int i = 0 ; i < vs.size() ; i++ )
            gl->Append ( vs[i].c_str() ) ;
    gl->SetStringSelection ( txt("Current") ) ;
    
    wxString rbs[3] ;
    rbs[0] = txt("All enzymes") ;
    rbs[1] = txt("Enzymes that cut") ;
    rbs[2] = txt("Enzymes that don't cut") ;
    rb = new wxRadioBox ( this , RSE_RES_RB , txt("subsel") ,
                          wxPoint ( bo*2+rw/3 , th*3 ) ,
                          wxDefaultSize ,
                           3 , rbs , 1 , wxRA_SPECIFY_COLS ) ;
    
    // Available enzymes list
    new wxStaticText ( this , -1 , txt("t_available_enzymes") , wxPoint ( bo , bo ) ) ;
    el = new wxListCtrl ( this , RSE_RES_LL , wxPoint ( bo , th*2 ) ,
                wxSize ( rw/3 , rh*2/3-th*2-bo ) ,
                wxLC_REPORT|wxLC_SINGLE_SEL ) ;
    el->InsertColumn ( 0 , txt("name") ) ;
    el->InsertColumn ( 1 , txt("cuts") ) ;

    // "This enzyme cuts" list
    rsl = new wxListCtrl ( this , -1 , wxPoint ( bo , rh*2/3 ) ,
                 wxSize ( rw/2 , rh/3-bo ) ,
                 wxLC_REPORT|wxLC_SINGLE_SEL ) ;
    rsl->InsertColumn ( 0 , "#" , wxLIST_FORMAT_LEFT , th*2 ) ;
    rsl->InsertColumn ( 1 , txt("from") ) ;
    rsl->InsertColumn ( 2 , txt("to") ) ;
    rsl->InsertColumn ( 3 , txt("length") ) ;

    // Cocktail
    new wxStaticText ( this , -1 , txt("Cocktail") , wxPoint ( rw*2/3+bo , bo ) ) ;
    el2 = new wxListCtrl ( this , RSE_COC_LL , wxPoint ( rw*2/3+bo , th*2 ) ,
                wxSize ( rw/3-bo*2 , rh*2/3-th*2-bo ) , 
                wxLC_REPORT|wxLC_SINGLE_SEL ) ;
    el2->InsertColumn ( 0 , txt("name") ) ;
    el2->InsertColumn ( 1 , txt("cuts") ) ;

    // All fragments list
    rsl2 = new wxListCtrl ( this , -1 , wxPoint ( rw/2+bo , rh*2/3 ) ,
                  wxSize ( rw/2-bo*2 , rh/3-bo ) , 
                  wxLC_REPORT|wxLC_SINGLE_SEL ) ;
    rsl2->InsertColumn ( 0 , "#" , wxLIST_FORMAT_LEFT , th*2 ) ;
    rsl2->InsertColumn ( 1 , txt("from") ) ;
    rsl2->InsertColumn ( 2 , txt("to") ) ;
    rsl2->InsertColumn ( 3 , txt("length") ) ;

    // Fragment threshold
    wxRect r ;
    r = rb->GetRect() ;
    nfst = new wxCheckBox ( this , RSE_COC_CB , txt("Do not create fragments below ") , 
                wxPoint ( bo*2+rw/3 , r.GetBottom() + bo ) ) ;
    r = nfst->GetRect() ;
    nfstv = new wxSpinCtrl ( this , -1 , "20" , 
                    wxPoint ( r.GetLeft() , r.GetBottom()+bo ) ,
                    wxSize ( 70 , th*2 ) ) ;
    r = nfstv->GetRect() ;
    new wxStaticText ( this , -1 , txt("base pairs.") ,
             wxPoint ( r.GetRight()+bo , r.GetTop()+bo ) ) ;
    nfst->SetValue ( true ) ;

    // Remove enzyme button
    r = nfstv->GetRect() ;
    new wxButton ( this , RSE_COC_RM , txt("Remove enzyme") ,
                   wxPoint ( bo*2+rw/3 + 100 , r.GetBottom() + bo ) ) ;
                   
    // Image buttons
    r = nfstv->GetRect() ;
    bb = new wxBitmapButton ( this , RSE_RES_AC ,
             wxBitmap (myapp()->bmpdir+"\\cocktail.bmp", wxBITMAP_TYPE_BMP),
             wxPoint ( bo*2+rw/3 , r.GetBottom() + bo ) ,
             wxDefaultSize ,
             wxBU_AUTODRAW ,
             wxDefaultValidator ,
             txt("add2cocktail") ) ;
    r = bb->GetRect() ;
    new wxStaticText ( this , -1 , txt("add2cocktail") ,
             wxPoint ( bo*2+rw/3 , r.GetBottom() + bo ) ) ;

    wxBitmapButton *bib = new wxBitmapButton ( this , RSE_COC_CT ,
             wxBitmap (myapp()->bmpdir+"\\scissors.bmp", wxBITMAP_TYPE_BMP),
             wxPoint ( bo*2+rw/3 , r.GetBottom() + th*2 ) ,
             wxDefaultSize ,
             wxBU_AUTODRAW ,
             wxDefaultValidator ,
             txt("start_res") ) ;
    r = bib->GetRect() ;
    new wxStaticText ( this , -1 , txt("start_res") ,
             wxPoint ( bo*2+rw/3 , r.GetBottom() + bo ) ) ;

    // Done / Cancel buttons
    btOK = new wxButton ( this , RSE_RES_OK , txt("b_done") , 
                wxPoint ( bo*2+rw/3 + 100 , rh*2/3 - th*5 - bo ) ) ;
    btCC = new wxButton ( this , RSE_RES_CC , txt("b_cancel") , 
                wxPoint ( bo*2+rw/3 + 100 , rh*2/3 - th*2 - bo ) ) ;
    btOK->SetDefault() ;

    refreshCocktail () ;

    SetClientSize ( w , h ) ;
    Centre() ;
    
    int i ;
    for ( i = 0 ; i < v->rc.size() ; i++ )
        cutcache.push_back ( TREcache ( v->rc[i].e->name.c_str() , v->rc[i].pos ) ) ;
    
    pR_showGroupEnzymes ( txt("Current") ) ;
    
    for ( i = 0 ; i < myapp()->frame->lastCocktail.size() ; i++ )
       add2cocktail ( myapp()->frame->lastCocktail[i].c_str() ) ;
    myapp()->frame->lastCocktail.clear() ;
    }

void TRestrictionEditor::pR_showGroupEnzymes ( wxString gr )
    {
    vector <string> vs ;
    el->DeleteAllItems() ;
    rsl->DeleteAllItems() ;
    if ( gr == txt("Current") )
        {
        for ( int i = 0 ; i < v->re.size() ; i++ )
            vs.push_back ( v->re[i]->name ) ;
        }
    else myapp()->frame->LS->getEnzymesInGroup ( gr.c_str() , vs ) ;
    char t[100] ;
    int sel = rb->GetSelection() ;
    bool docut = true , donotcut = true ;
    if ( sel == 1 ) donotcut = false ;
    if ( sel == 2 ) docut = false ;
    int cnt = 0 , i ;
    bool ascending = true ;
    vector <TREcache> list ;
    for ( i = 0 ; i < vs.size() ; i++ )
        {
        int k = getcuts(vs[i].c_str()).size() ;
        if ( ( k == 0 && donotcut ) ||
             ( k > 0 && docut ) )
           {
           list.push_back ( TREcache ( vs[i].c_str() , k ) ) ;
           }
        }

    sort ( list.begin() , list.end() ) ;
        
    for ( i = 0 ; i < list.size() ; i++ )
        {
        sprintf ( t , "%d" , list[i].cut ) ;
        int z = el->InsertItem ( cnt , list[i].enzyme.c_str() ) ;
        el->SetItem ( z , 1 , t ) ;
        cnt++ ;
        }
        
        
    if ( pre != "" )
        {
        int pre_id = el->FindItem ( 0 , pre.c_str() ) ;
        el->SetItemState ( pre_id , wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED );
        el->EnsureVisible ( pre_id ) ;
        el->SetFocus() ;
        pre = "" ;
        }
    }
    
vector <int> TRestrictionEditor::getcuts ( wxString enzyme )
    {
    vector <int> ret ;
    int j , k = 0 ;
    for ( j = 0 ; j < nocut.size() && nocut[j] != enzyme ; j++ ) ;
    if ( j < nocut.size() ) return ret ; // No cuts, returning empty list
    
    // Looking for cuts in cache
    for ( j = 0 ; j < cutcache.size() ; j++ )
        if ( cutcache[j].enzyme == enzyme )
            ret.push_back ( cutcache[j].cut ) ;

    if ( ret.size() == 0 ) // Determine cuts
        {
        TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( enzyme.c_str() );
        vector <TRestrictionCut> x = v->getCuts ( e ) ;
        if ( x.size() == 0 ) // No cuts
            {
            nocut.push_back ( enzyme ) ;
            return ret ;
            }
        for ( j = 0 ; j < x.size() ; j++ )
            {
            cutcache.push_back ( TREcache ( enzyme , x[j].pos ) ) ;
            ret.push_back ( x[j].pos ) ;
            }
        }
    return ret ;
    }

void TRestrictionEditor::res_ll_act ( wxListEvent &event )
    {
    int i = event.GetIndex () ;
    wxString s = el->GetItemText ( i ).c_str() ;
    TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( s.c_str() ) ;
    TEnzymeDialog ed ( this , s.c_str() , wxPoint(-1,-1) , wxSize(600,400) , 
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
    wxString enzyme = el->GetItemText ( i ).c_str() ;
    vector <int> vi = getcuts ( enzyme ) ;
    listFragments ( rsl , vi ) ;
    }

void TRestrictionEditor::listFragments ( wxListCtrl *list , vector <int> &vi )
    {
    list->DeleteAllItems() ;
    if ( vi.size() == 0 ) return ;
    if ( !v->isCircular() ) // Adding last fragment for linear DNA
        {
        int from = vi[vi.size()-1] ;
        int to = v->sequence.length() ;
        if ( from != to )
           vi.push_back ( to ) ;
        }
    numberoffragments = vi.size() ;
        
    int i ;
    char u[100] ;
    for ( i = 0 ; i < vi.size() ; i++ )
        {
        sprintf ( u , "%d" , i+1 ) ; // Number
        int k = list->InsertItem ( i , u ) ;
        
        int from = 0 ;
        if ( i > 0 ) from = vi[i-1] ;
        if ( i == 0 && v->isCircular() ) from = vi[vi.size()-1] ;
        sprintf ( u , "%d" , from ) ; // From
        list->SetItem ( k , 1 , u ) ;
        
        sprintf ( u , "%d" , vi[i] ) ; // To
        list->SetItem ( k , 2 , u ) ;
        
        int len = vi[i] - from ;
        if ( len <= 0 ) len += v->sequence.length() ;
        sprintf ( u , "%d" , len ) ; // Length
        list->SetItem ( k , 3 , u ) ;
        }
    
    }
    
void TRestrictionEditor::refreshCocktail ()
    {
    vector <wxString> vs = cocktail ;
    el2->DeleteAllItems() ;
    char t[100] ;
    int i , j ;
    for ( i = 0 ; i < vs.size() ; i++ )
        {
        int k = getcuts(vs[i]).size() ;
        sprintf ( t , "%d" , k ) ;
        int z = el2->InsertItem ( i , vs[i] ) ;
        el2->SetItem ( z , 1 , t ) ;
        }
        
    vector <int> vi , vit ;
    for ( i = 0 ; i < cocktail.size() ; i++ )
        {
        vit = getcuts ( cocktail[i] ) ;
        for ( j = 0 ; j < vit.size() ; j++ )
           vi.push_back ( vit[j] ) ;
        }
    sort ( vi.begin() , vi.end() ) ;
    vit = vi ;
    while ( vi.size() ) vi.pop_back() ;
    for ( i = 0 ; i < vit.size() ; i++ )
        {
        if ( vi.size() == 0 || vi[vi.size()-1] != vit[i] )
           vi.push_back ( vit[i] ) ;
        }
    listFragments ( rsl2 , vi ) ;
    }

void TRestrictionEditor::add2cocktail ( wxString s )
    {
    if ( s == "" ) return ;
    int i ;
    for ( i = 0 ; i < cocktail.size() && cocktail[i] != s ; i++ ) ;
    if ( i < cocktail.size() ) return ; // Already in there
    cocktail.push_back ( s ) ;
    refreshCocktail () ;
    }

void TRestrictionEditor::del_from_cocktail ( wxString s )
    {
    int i ;
    for ( i = 0 ; i < cocktail.size() && cocktail[i] != s ; i++ ) ;
    if ( i == cocktail.size() ) return ; // Not there
    cocktail[i] = cocktail[cocktail.size()-1] ;
    cocktail.pop_back () ;
    refreshCocktail () ;
    }

// "Restriction" handlers

void TRestrictionEditor::res_dd ( wxCommandEvent &event )
    { // This handles the enzyme groups dropdown list
    wxString s = event.GetString() ;
    pR_showGroupEnzymes ( s.c_str() ) ;
    }

void TRestrictionEditor::res_cb ( wxCommandEvent &event )
    {
    wxString s = gl->GetStringSelection() ;
    pR_showGroupEnzymes ( s.c_str() ) ;
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

//******************************************************************************    

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

