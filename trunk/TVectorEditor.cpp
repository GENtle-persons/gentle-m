#include "TVectorEditor.h"

BEGIN_EVENT_TABLE(TVectorEditor, wxDialog )
    EVT_BUTTON(TVE_OK,TVectorEditor::OnOK)
    EVT_BUTTON(TVE_CANCEL,TVectorEditor::OnCancel)
    EVT_LIST_ITEM_SELECTED(TVE_ILIST,TVectorEditor::SelChangeItems)
    EVT_LIST_ITEM_ACTIVATED(TVE_ILIST,TVectorEditor::itemCol)

    EVT_LISTBOX(TVE_LB_CE,TVectorEditor::enzymeSelChange)
    EVT_LISTBOX(TVE_LB_GR,TVectorEditor::enzymeSelChange)
    EVT_LISTBOX(TVE_LB_GE,TVectorEditor::enzymeSelChange)
    EVT_LISTBOX_DCLICK(TVE_LB_CE,TVectorEditor::enzymeListDlbClick)
    EVT_LISTBOX_DCLICK(TVE_LB_GR,TVectorEditor::enzymeListDlbClick)
    EVT_LISTBOX_DCLICK(TVE_LB_GE,TVectorEditor::enzymeListDlbClick)
    EVT_LISTBOX(TVE_PROTEASE_LIST,TVectorEditor::proteaseSelChange)
    
    EVT_BUTTON(TVE_EN_ADD_GR,TVectorEditor::enzymeAddGr)
    EVT_BUTTON(TVE_EN_ADD_EN,TVectorEditor::enzymeAddEn)
    EVT_BUTTON(TVE_EN_DEL_EN,TVectorEditor::enzymeDelEn)
    EVT_BUTTON(TVE_EN_ADD_TO_GR,TVectorEditor::enzymeAddToGr)
    EVT_BUTTON(TVE_EN_ADD_TO_NEW_GR,TVectorEditor::enzymeAddToNewGr)
    EVT_BUTTON(TVE_IMPORT,TVectorEditor::importCloneEnzymes)
    EVT_BUTTON(TVE_NEW_ENZYME,TVectorEditor::newEnzyme)
    
    EVT_BUTTON(TVE_EDIT_PROTEASE,TVectorEditor::editProtease)
    EVT_BUTTON(TVE_NEW_PROTEASE,TVectorEditor::newProtease)

    EVT_BUTTON(TVE_I_ADD,TVectorEditor::itemAdd)
    EVT_BUTTON(TVE_I_DEL,TVectorEditor::itemDel)
    EVT_BUTTON(TVE_I_CLR,TVectorEditor::itemClr)
    EVT_BUTTON(TVE_I_COL,TVectorEditor::itemCol)
    
    EVT_CHAR_HOOK(TVectorEditor::OnCharHook)
END_EVENT_TABLE()

void TVectorEditor::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

TVectorEditor::TVectorEditor(wxWindow *parent, const wxString& title , TVector *_v , MyApp *_app )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 550 ) )
    {
    v = _v ;
    app = _app ;
    panProp = panItem = panEnzym = NULL ;

    int w , h ;
    GetClientSize ( &w , &h ) ;
    bo = 5 ;
    th = 22 ;
    
    nb = new wxNotebook ( (wxWindow*) this ,
                          -1 ,
                          wxPoint ( 0 , 0 ) ,
                          wxSize ( w , h ) ) ;

    hideProp = hideItem = hideEnzym = false ;

    if ( v ) panProp = new wxPanel ( nb , -1 ) ;
    if ( v ) panItem = new wxPanel ( nb , -1 ) ;
    if ( v ) panProt = new wxPanel ( nb , -1 ) ;
    panEnzym = new wxPanel ( nb , -1 ) ;
    
    if ( v ) initPanProp () ;
    if ( v ) initPanItem () ;
    initPanEnzym () ;
    if ( v ) initPanProt () ;
    if ( v ) name->SetFocus() ;
    Center () ;
    }

TVectorEditor::~TVectorEditor ()
    {
    nb->DeleteAllPages() ;
    }
    
void TVectorEditor::GetMyClientSize ( int *w , int *h , wxPanel *pan )
{
#ifdef __WXMSW__
  pan->GetClientSize ( w , h ) ;
#else
  GetSize ( w , h ) ;
  *w -= 10 ;
  *h -= 30 ;
#endif
}
    
void TVectorEditor::initPanProt ()
    {
    nb->AddPage ( panProt , txt("t_vec_prot") ) ;
    int w , h ;
    GetMyClientSize ( &w , &h , panProt ) ;


    prots = new wxCheckListBox ( panProt , TVE_PROTEASE_LIST , wxPoint ( th , th ) , wxSize ( w - th*2 , h * 2 / 3 ) ) ;
    
    showProteases () ;
        
    wxButton *b ;
    b = new wxButton ( panProt , TVE_EDIT_PROTEASE , txt("t_vec_edit_prot") , wxPoint ( th , h * 2 / 3 + th * 2 ) ) ;
    new wxButton ( panProt , TVE_NEW_PROTEASE , txt("t_vec_new_prot") , wxPoint ( th , h * 2 / 3 + th * 4 ) ) ;

    wxRect r = b->GetRect() ;
    pro_txt = new TURLtext ( panProt , URLTEXT_DUMMY , "" , wxPoint ( r.GetRight() + bo , r.GetTop() ) ,
                                wxSize ( w - th - bo - r.GetRight() , h - th*2 - r.GetTop() ) ,
                                wxTE_MULTILINE|wxTE_READONLY ) ;
    pro_txt->SetBackgroundColour ( wxSystemSettings::GetColour ( wxSYS_COLOUR_BTNFACE ) ) ;
        
    addOkCancel ( panProt ) ;
    }

void TVectorEditor::showProteases ()
    {
    int a , b ;
    vector <string> vs ;
    for ( a = 0 ; a < app->frame->LS->pr.size() ; a++ )
        vs.push_back ( app->frame->LS->pr[a]->name ) ;
    sort ( vs.begin() , vs.end() ) ;
        
    prots->Clear () ;
    for ( a = 0 ; a < vs.size() ; a++ )
        {
        string n = vs[a] ;
        prots->Append ( n.c_str() ) ;
        for ( b = 0 ; b < v->proteases.size() && v->proteases[b] != n ; b++ ) ;
        if ( b < v->proteases.size() ) prots->Check ( prots->FindString ( n.c_str() ) ) ;
        }
    }

void TVectorEditor::initPanProp ()
    {
    nb->AddPage ( panProp , txt("t_vec_prop") ) ;
    int w , h ;
    GetMyClientSize ( &w , &h , panProp ) ;
    
    // Name and description
    wxRect r ;
    r = (new wxStaticText(panProp,-1,txt("name"),wxPoint(bo,bo*2)))->GetRect() ;
    name = new wxTextCtrl(panProp,-1,v->name.c_str(),wxPoint(r.GetRight()+bo,bo),wxSize(w-r.GetRight()-bo*2,th)) ;

    r = (new wxStaticText(panProp,-1,txt("desc"),wxPoint(bo,bo*2+th)))->GetRect() ;
    desc = new TURLtext(panProp,URLTEXT_DUMMY,v->desc.c_str(),wxPoint(bo,r.GetBottom()+bo),wxSize(w-bo*2,th*5),wxTE_MULTILINE);
    
    if ( v->type != TYPE_AMINO_ACIDS )
        {
        // Sticky ends
        r = (new wxStaticText(panProp,-1,"5'-",wxPoint(bo,th*8)))->GetRect() ;
        lu = new wxTextCtrl(panProp,-1,v->getStickyEnd(true,true).c_str(),wxPoint(r.GetRight()+bo,r.GetTop()-bo),wxSize(w/5,th));
    
        r = (new wxStaticText(panProp,-1,"-3'",wxPoint(w-r.GetRight(),r.GetTop())))->GetRect() ;
        ru = new wxTextCtrl(panProp,-1,v->getStickyEnd(false,true).c_str(),wxPoint(r.GetLeft()-bo-w/5,r.GetTop()-bo),wxSize(w/5,th));
        
        string k ;
        while ( k.length() * (r.GetWidth()/3) < w/2 ) k += "-" ;
        new wxStaticText(panProp,-1,k.c_str(),wxPoint(lu->GetRect().GetRight()+bo,r.GetTop())) ;
    
        r = (new wxStaticText(panProp,-1,"3'-",wxPoint(bo,r.GetBottom()+th)))->GetRect() ;
        ll = new wxTextCtrl(panProp,-1,v->getStickyEnd(true,false).c_str(),wxPoint(r.GetRight()+bo,r.GetTop()-bo),wxSize(w/5,th));
    
        r = (new wxStaticText(panProp,-1,"-5'",wxPoint(w-r.GetRight(),r.GetTop())))->GetRect() ;
        rl = new wxTextCtrl(panProp,-1,v->getStickyEnd(false,false).c_str(),wxPoint(r.GetLeft()-bo-w/5,r.GetTop()-bo),wxSize(w/5,th));
    
        new wxStaticText(panProp,-1,k.c_str(),wxPoint(lu->GetRect().GetRight()+bo,r.GetTop())) ;
        }    
    
    addOkCancel ( panProp ) ;
    }
    
void TVectorEditor::initPanItem ()
    {
    nb->AddPage ( panItem , txt("t_vec_item") ) ;
    int w , h ;
    GetMyClientSize ( &w , &h , panItem ) ;
    
    // Items list
    items = new wxListCtrl ( panItem , TVE_ILIST ,
                    wxPoint ( bo , bo ) ,
                    wxSize ( w - bo*2 , h/2 - bo ) ,
                    wxLC_REPORT|wxLC_SINGLE_SEL ) ;
    items->InsertColumn ( 0 , txt("name") ) ;
    items->InsertColumn ( 1 , txt("type") ) ;
    items->InsertColumn ( 2 , txt("from") ) ;
    items->InsertColumn ( 3 , txt("to") ) ;
    items->InsertColumn ( 4 , txt("direction") ) ;
    items->InsertColumn ( 5 , txt("length") ) ;

    int a ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        TVectorItem *nvi = new TVectorItem ;
        *nvi = v->items[a] ;
        nvi->r2 = a ;
        nvi->setParam ( "CHANGED" , "" ) ;
        newitems.push_back ( nvi ) ;
        }
    makeItemsList () ;
    
    // Edit fields
    icur = -1 ;
    wxRect r ;
    wxString vs[VIT_TYPES] ;
    char t[1000] ;
    
    r = (new wxStaticText(panItem,-1,txt("name"),wxPoint(bo,h/2+bo*2)))->GetRect() ;
    iname = new wxTextCtrl(panItem,-1,"",wxPoint(r.GetRight()+bo,h/2+bo),wxSize(w*3/4-r.GetWidth()-bo*3,th));

    r = iname->GetRect() ;
    icb = new wxCheckBox(panItem,-1,txt("cw"),wxPoint(r.GetRight()+bo,r.GetTop()));

    r = icb->GetRect() ;
    icv = new wxCheckBox(panItem,-1,txt("t_is_visible"),wxPoint(r.GetRight()+bo,r.GetTop()));

    r = (new wxStaticText(panItem,-1,txt("from"),wxPoint(w/4+bo,h/2+th+bo*2)))->GetRect() ;
    ifrom = new wxTextCtrl(panItem,-1,"",wxPoint(r.GetRight()+bo,h/2+th+bo),wxSize(w/4-r.GetWidth()-bo*3,th));

    r = (new wxStaticText(panItem,-1,txt("to"),wxPoint(w/2+bo,h/2+th+bo*2)))->GetRect() ;
    ito = new wxTextCtrl(panItem,-1,"",wxPoint(r.GetRight()+bo,h/2+th+bo),wxSize(w/4-r.GetWidth()-bo*3,th));

    r = (new wxStaticText(panItem,-1,txt("desc"),wxPoint(bo,h/2+th+bo*2)))->GetRect() ;
    idesc = new TURLtext(panItem,URLTEXT_DUMMY,"",
                            wxPoint(r.GetLeft(),r.GetBottom()+bo),
                            wxSize(w*3/4-bo,h-r.GetBottom()-th*2),
                            wxTE_PROCESS_ENTER|wxTE_MULTILINE );
    
    bClear = new wxButton ( panItem , TVE_I_CLR , txt("b_clear_selection") ,
                    wxPoint ( w*3/4+bo , r.GetBottom()+bo*4+th*2 ) ,
                    wxSize ( w/4-bo*2 , th ) ) ;
    bAdd = new wxButton ( panItem , TVE_I_ADD , txt("b_add_item") ,
                    wxPoint ( w*3/4+bo , r.GetBottom()+bo*5+th*3 ) ,
                    wxSize ( w/4-bo*2 , th ) ) ;
    bDel = new wxButton ( panItem ,TVE_I_DEL , txt("d_del_item") ,
                    wxPoint ( w*3/4+bo , r.GetBottom()+bo*6+th*4 ) ,
                    wxSize ( w/4-bo*2 , th ) ) ;
    bCol = new wxButton ( panItem ,TVE_I_COL , txt("t_edit_item") ,
                    wxPoint ( w*3/4+bo , r.GetBottom()+bo*7+th*5 ) ,
                    wxSize ( w/4-bo*2 , th ) ) ;

    for ( a = 0 ; a < VIT_TYPES ; a++ )
        {
        sprintf ( t , "itemtype%d" , a ) ;
        vs[a] = txt(t) ;
        }
    r = ito->GetRect() ;
    ichoice = new wxChoice(panItem,-1,
                wxPoint(r.GetRight()+bo,r.GetTop()),
                wxSize(w/4-bo*2,th),
                VIT_TYPES,
                vs) ;
    
    wxString vt[4] ;
    vt[0] = "-" ;
    vt[1] = "1" ;
    vt[2] = "2" ;
    vt[3] = "3" ;
    r = ichoice->GetRect() ;
    
    irb = new wxRadioBox ( panItem , -1 , txt("t_reading_frame") ,
                wxPoint(r.GetLeft(),r.GetBottom()+bo),
                wxSize(w/4-bo*2,th*2),
                4 , vt , wxRA_SPECIFY_COLS ) ;

    if ( v->type == TYPE_AMINO_ACIDS )
        {
        icb->Disable () ;
        ichoice->Disable () ;
        irb->Disable () ;
        }
    
    addOkCancel ( panItem ) ;
    }

void TVectorEditor::addItem2list ( TVectorItem &i , int a )
    {
    int l ;
    char t[1000] ;
    l = items->InsertItem ( a , i.name.c_str() ) ;
    items->SetItemData ( l , a ) ;
    i.r4 = l ;

    sprintf ( t , "itemtype%d" , i.type ) ;
    items->SetItem ( l , 1 , txt(t) ) ;

    sprintf ( t , "%d" , i.from ) ;
    items->SetItem ( l , 2 , t ) ;

    sprintf ( t , "%d" , i.to ) ;
    items->SetItem ( l , 3 , t ) ;

    if ( i.direction == 1 )
       items->SetItem ( l , 4 , txt("cw") ) ;
    else if ( i.direction == -1 )
       items->SetItem ( l , 4 , txt("ccw") ) ;
    else
       items->SetItem ( l , 4 , "---" ) ;

    int len = i.to - i.from + 1 ;
    if ( i.to < i.from ) len += v->sequence.length() ;
    sprintf ( t , "%d" , len ) ;
    items->SetItem ( l , 5 , t ) ;
    }
    
void TVectorEditor::makeItemsList ()
    {
    items->DeleteAllItems() ;
    for ( int a = 0 ; a < newitems.size() ; a++ )
        addItem2list ( *newitems[a] , a ) ;
    }
    
void TVectorEditor::initPanEnzym ()
    {
    nb->AddPage ( panEnzym , txt("t_vec_enzym") ) ;
    int w , h ;
    GetMyClientSize ( &w , &h , panEnzym ) ;
    

    int xw = 150 ;
    int bx1 = w/2-xw/2 , bx2 = w/2+xw/2 ;
    int by1 = 4*bo , by2 = h/3 , bh = 20 ;
    
    wxStaticText *tCE , *tGR , *tGE ;
    tCE = new wxStaticText ( panEnzym , -1 , txt("current_enzymes") , 
            wxPoint ( 0 , bo ) ) ;
    tGR = new wxStaticText ( panEnzym , -1 , txt("enzyme_groups") , 
            wxPoint ( bx2 , bo ) ) ;
    tGE = new wxStaticText ( panEnzym , -1 , txt("enzymes_in_group") , 
            wxPoint ( bx2 , by2-bo*2 ) ) ;
    
    listCE = new wxListBox ( panEnzym , TVE_LB_CE , wxPoint ( 0 , by1 ) , 
            wxSize ( bx1-bo , h-by1-th*2 ) , 0 , NULL , wxLB_EXTENDED|wxLB_SORT ) ;
    listGroups = new wxListBox ( panEnzym , TVE_LB_GR , wxPoint ( bx2 , by1 ) , 
            wxSize ( w-bx2 , by2-by1-bo*2 ) , 0 , NULL , wxLB_SINGLE|wxLB_SORT);
    listGE = new wxListBox ( panEnzym , TVE_LB_GE , wxPoint ( bx2 , by2+bo ) , 
            wxSize ( w-bx2 , h-by2-bo-th*2 ) , 0 , NULL , wxLB_EXTENDED|wxLB_SORT ) ;


    new wxButton ( panEnzym , TVE_EN_ADD_GR , txt("<-- add") ,
            wxPoint ( bx1+bo , by1+bo ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;

    new wxButton ( panEnzym , TVE_EN_ADD_TO_GR , txt("b_add_to_group") ,
            wxPoint ( bx1+bo , by1+bo+th ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;

    new wxButton ( panEnzym , TVE_EN_ADD_TO_NEW_GR , txt("b_add_as_new_group") ,
            wxPoint ( bx1+bo , by1+bo+th*2 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;
            
    if ( !v )
        new wxButton ( panEnzym , TVE_IMPORT , txt("import_clone_enzymes") ,
               wxPoint ( bx1+bo , by1+bo+th*4 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;
               
    new wxButton ( panEnzym , TVE_NEW_ENZYME , txt("add_new_enzyme") ,
           wxPoint ( bx1+bo , by1+bo*2+th*5 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;
            
    new wxButton ( panEnzym , TVE_EN_ADD_EN , txt("<-- add") , 
            wxPoint ( bx1+bo , by2+bo ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;

    new wxButton ( panEnzym , TVE_EN_DEL_EN , txt("del -->") , 
            wxPoint ( bx1+bo , by2+bo+th ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;


    listCE->Clear() ;
    listGroups->Clear() ;
    showEnzymeGroups () ;
    
    int a ;
    while ( ce.size() ) ce.pop_back () ;
    for ( a = 0 ; v && a < v->re.size() ; a++ )
        {
        string s = v->re[a]->name ;
        listCE->Append ( s.c_str() ) ;
        ce.push_back ( s ) ;
        }
    
    addOkCancel ( panEnzym ) ;
    }

// Handlers

void TVectorEditor::addOkCancel ( wxPanel *p )
    {
    int w , h , z ;
    GetMyClientSize ( &w , &h , p ) ;
    z = w / 5 ;
    
    wxButton *b = new wxButton ( p , TVE_OK , txt("b_ok") ,
                    wxPoint ( z , h - th - bo ) ,
                    wxSize ( z , th ) ) ;

    new wxButton ( p , TVE_CANCEL , txt("b_cancel") ,
                    wxPoint ( w - z*2 , h - th - bo ) ,
                    wxSize ( z , th ) ) ;
                    
    b->SetDefault () ;
    }

#define VCOMMIT(_p1,_p2) if(_p1!=v->_p2){v->_p2=_p1;v->setChanged();}
#define VCOMMIT_STICKY(_p1,_p2,_p3) if(_p1!=v->getStickyEnd(_p2,_p3)){v->setStickyEnd(_p2,_p3,_p1);v->setChanged();}
    
void TVectorEditor::commitVector ()
    {
    string n = name->GetValue().c_str() ;
    string d = desc->GetValue().c_str() ;
    VCOMMIT(n,name);
    VCOMMIT(d,desc);

    if ( v->type != TYPE_AMINO_ACIDS )
        {
        string _lu = lu->GetValue().c_str() ;
        string _ll = ll->GetValue().c_str() ;
        string _ru = ru->GetValue().c_str() ;
        string _rl = rl->GetValue().c_str() ;
        VCOMMIT_STICKY(_lu,true,true);
        VCOMMIT_STICKY(_ll,true,false);
        VCOMMIT_STICKY(_ru,false,true);
        VCOMMIT_STICKY(_rl,false,false);
        }
    }
    
void TVectorEditor::commitItems ()
    {
    storeItemData () ;
    int a , b ;
    vector <bool> found ;
    while ( found.size() < v->items.size() ) found.push_back ( false ) ;
    for ( a = 0 ; a < newitems.size() ; a++ )
        {
        TVectorItem c = *newitems[a] ;
        if ( c.r2 != -1 )
            {
            found[c.r2] = true ;
            TVectorItem o = v->items[c.r2] ;
            if ( o.name != c.name ||
                 o.desc != c.desc ||
                 o.type != c.type ||
                 o.from != c.from ||
                 o.to != c.to ||
                 o.direction != c.direction ||
                 c.getParam ( "CHANGED" ) != "" ||
                 o.getRF() != c.getRF() ||
                 o.isVisible() != c.isVisible() ||
                 o.getFontColor() != c.getFontColor() )
                 {
                 v->items[c.r2] = c ;
                 v->items[c.r2].r2 = -1 ;
                 v->setChanged () ;
                 v->recalcvisual = true ;
                 }
            }
        else
            {
            v->items.push_back ( c ) ;
            v->items[v->items.size()-1].r2 = -1 ;
            v->setChanged () ;
            v->recalcvisual = true ;
            }
        }
    for ( a = found.size() - 1 ; a >= 0 ; a-- )
        {
        if ( !found[a] ) 
            {
            for ( b = a+1 ; b < found.size() ; b++ ) v->items[b-1] = v->items[b] ;
            v->items.pop_back () ;
            v->setChanged () ;
            v->recalcvisual = true ;
            }
        }
    }
    
void TVectorEditor::commitEnzymes ()
    {
    int a , b ;
    bool changed = false ;
    
    // Removed enzymes
    for ( a = 0 ; a < v->re.size() ; a++ )
        {
        for ( b = 0 ; b < ce.size() && ce[b] != v->re[a]->name ; b++ ) ;
        if ( b == ce.size() )
           {
           changed = true ;
           v->re[a] = v->re[v->re.size()-1] ;
           v->re.pop_back () ;
           a-- ;
           }
        }

    // Added enzymes
    for ( b = 0 ; b < ce.size() ; b++ )
        {
        for ( a = 0 ; a < v->re.size() && ce[b] != v->re[a]->name ; a++ ) ;
        if ( a == v->re.size() )
           {
           changed = true ;
           v->re.push_back ( app->frame->LS->getRestrictionEnzyme ( ce[b] ) ) ;
           }
        }
        
    if ( changed )
        {
        v->recalculateCuts() ;
        v->recalcvisual = true ;
        }
    v->setChanged ( changed | v->isChanged() ) ;
    }

void TVectorEditor::commitProteases ()
    {
    int a , b ;
    bool changed = false ;
    
    vector <string> v1 , v2 ;
    v1 = v->proteases ;
    
    for ( a = 0 ; a < prots->GetCount() ; a++ )
        if ( prots->IsChecked ( a ) )
           v2.push_back ( prots->GetString(a).c_str() ) ;
    
    sort ( v1.begin() , v1.end() ) ;
    sort ( v2.begin() , v2.end() ) ;
    
    if ( v1.size() != v2.size() ) changed = true ;
    for ( a = 0 ; !changed && a < v1.size() ; a++ )
        if ( v1[a] != v2[a] )
           changed = true ;
    
    if ( changed ) v->proteases = v2 ;
    
    v->setChanged ( changed | v->isChanged() ) ;
    }
        
void TVectorEditor::OnOK ( wxCommandEvent &ev )
    {
    if ( v )
        {
        if ( panProp ) commitVector () ;
        if ( panItem ) commitItems () ;
        if ( panEnzym ) commitEnzymes () ;
        if ( panProt ) commitProteases () ;
        }
    wxDialog::OnOK ( ev ) ;
    }

void TVectorEditor::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }
    
void TVectorEditor::cleanup ()
    {
    while ( newitems.size() )
        {
        delete newitems[newitems.size()-1] ;
        newitems.pop_back () ;
        }
    }
    
// Handlers "enzyme"

void TVectorEditor::enzymeSelChange ( wxEvent &ev )
    {
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGroups )
        {
        showGroupEnzymes ( lb->GetString ( lb->GetSelection() ).c_str() ) ;
        }
    }
    
void TVectorEditor::enzymeListDlbClick ( wxEvent &ev )
    {
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGE )
        {
        wxArrayInt vi ;
        int i , k , n = lb->GetSelections ( vi ) ;
        for ( k = 0 ; k < n ; k++ )
            {
            string s = lb->GetString ( vi[k] ).c_str() ;
            TRestrictionEnzyme *e = app->frame->LS->getRestrictionEnzyme ( s ) ;
            TEnzymeDialog ed ( this , s.c_str() , wxPoint(-1,-1) , wxSize(600,400) , 
                            wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
            ed.initme ( e ) ;
            if ( ed.ShowModal() == wxID_OK )
               {
/*               ed.e->name = e->name ; // No name change!
               if ( e->differ ( *ed.e ) )
                  {
                  *e = *ed.e ;
                  app->frame->LS->updateRestrictionEnzyme ( e ) ;
                  }*/
               wxCommandEvent ev ;
               enzymeAddEn ( ev ) ;
               }
            }
        }
    else if ( lb == listCE )
        {
        wxArrayInt vi ;
        int n = lb->GetSelections ( vi ) ;
        if ( n != 1 ) return ;
        string s = lb->GetString ( vi[0] ).c_str() ;
        TRestrictionEnzyme *e = app->frame->LS->getRestrictionEnzyme ( s ) ;
        TEnzymeDialog ed ( this , s.c_str() , wxPoint(-1,-1) , wxSize(600,400) , 
                        wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
        ed.initme ( e ) ;
        ed.ShowModal() ;
        }
    }
    
void TVectorEditor::showEnzymeGroups ()
    {
    wxString all = txt("All") ;
    listGroups->Clear() ;
    listGroups->Append ( all ) ;
    vector <string> vs ;
    app->frame->LS->getEnzymeGroups ( vs ) ;
    for ( int i = 0 ; i < vs.size() ; i++ )
        listGroups->Append ( vs[i].c_str() ) ;
    showGroupEnzymes ( all.c_str() ) ;
    listGroups->SetStringSelection ( all ) ;
    }
    
void TVectorEditor::showGroupEnzymes ( string gr )
    {
    vector <string> vs ;
    listGE->Clear() ;
    app->frame->LS->getEnzymesInGroup ( gr , vs ) ;
    while ( eig.size() ) eig.pop_back () ;
    for ( int i = 0 ; i < vs.size() ; i++ )
        {
        listGE->Append ( vs[i].c_str() ) ;
        eig.push_back ( vs[i] ) ;
        }
    }

void TVectorEditor::enzymeAddEn ( wxEvent &ev )
    {
    wxArrayInt vi ;
    int i , k , n = listGE->GetSelections ( vi ) ;
    for ( k = 0 ; k < n ; k++ )
        {
        i = vi[k] ;
        string s = listGE->GetString ( i ).c_str() ;
        listGE->Deselect ( i ) ;
        int p = listCE->FindString ( s.c_str() ) ;
        if ( p == wxNOT_FOUND )
            {
            listCE->Append ( s.c_str() ) ;
            ce.push_back ( s ) ;
            }
        }
    }

void TVectorEditor::enzymeAddGr ( wxEvent &ev )
    {
    int a , b ;
    string s ;
    for ( a = 0 ; a < eig.size() ; a++ )
        {
        s = eig[a] ;
        b = listCE->FindString ( s.c_str() ) ;
        if ( b == wxNOT_FOUND )
           {
           listCE->Append ( s.c_str() ) ;
           ce.push_back ( s ) ;
           }
        }
    }

void TVectorEditor::enzymeAddToGr ( wxEvent &ev )
    {
    string group = listGroups->GetStringSelection().c_str() ;
    if ( group == txt("All") ) return ;
    string sql ;
    wxArrayInt vi ;
    int i , k , n = listCE->GetSelections ( vi ) ;
    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        string s = listCE->GetString(i).c_str() ;
        sql = "DELETE FROM link_enzyme_group WHERE "
              "leg_enzyme=\"" +
              s +
              "\" AND leg_group=\"" +
              group +
              "\"" ;
        app->frame->LS->getObject ( sql ) ;
        sql = "INSERT INTO link_enzyme_group (leg_enzyme,leg_group) "
              "VALUES (\"" +
              s + 
              "\",\"" +
              group + 
              "\")" ;
        app->frame->LS->getObject ( sql ) ;              
        }
    showGroupEnzymes ( group ) ;
    }

void TVectorEditor::enzymeAddToNewGr ( wxEvent &ev )
    {
    wxTextEntryDialog ted ( this , txt("b_add_as_new_group") ,
                                txt("t_new_enzyme_group_name") ) ;
    if ( wxID_OK != ted.ShowModal () ) return ;
    string ng = ted.GetValue().c_str() ;
    if ( !app->frame->LS->addEnzymeGroup ( ng ) ) return ;
    listGroups->Append ( ng.c_str() ) ;
    listGroups->SetStringSelection ( ng.c_str() ) ;
    enzymeAddToGr ( ev ) ;
    }

void TVectorEditor::enzymeDelEn ( wxEvent &ev )
    {
    wxArrayInt vi ;
    int i , k , n = listCE->GetSelections ( vi ) ;
    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        string s = listCE->GetString ( i ).c_str() ;
        listCE->Delete ( i ) ;
        for ( i = 0 ; i < ce.size() ; i++ )
            {
            if ( ce[i] == s )
               {
               ce[i] = ce[ce.size()-1] ;
               ce.pop_back () ;
               }
            }
        }
    }
    
void TVectorEditor::initialViewItem ( int num )
    {
    nb->SetSelection ( 1 ) ;
    items->SetItemState ( num , wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED ,
                            wxLIST_MASK_IMAGE|wxLIST_MASK_STATE ) ;
    }

void TVectorEditor::initialViewEnzyme ( string e )
    {
    nb->SetSelection ( 2 ) ;
    listCE->SetStringSelection ( e.c_str() ) ;
    }
    
void TVectorEditor::importCloneEnzymes ()
    {
    TStorage TS ( TEMP_STORAGE , app ) ;
    TClone clone ;
    clone.loadEnzymeList ( &TS , "./CLONE.ENZ" ) ;
    int a ;
    for ( a = 0 ; a < TS.re.size() ; a++ )
       {
       listCE->Append ( TS.re[a]->name.c_str() ) ;
       app->frame->LS->re.push_back ( TS.re[a] ) ;
       app->frame->LS->updateRestrictionEnzyme ( TS.re[a] ) ;
       TS.re[a] = NULL ; // avoid deletion
       }
    string group = listGroups->GetStringSelection().c_str() ;
    showGroupEnzymes ( group ) ;
    }


// handlers ITEM
    
void TVectorEditor::SelChangeItems ( wxListEvent &ev )
    {
    storeItemData () ;
    int i , j = ev.GetIndex() ;
    i = items->GetItemData ( j ) ;
    char t[1000] ;
    iname->SetValue ( newitems[i]->name ) ;
    idesc->SetValue ( newitems[i]->desc ) ;
    ichoice->SetSelection ( newitems[i]->type ) ;
    
    int rf = newitems[i]->getRF () ;
    if ( newitems[i]->type != VIT_CDS )
        {
        irb->SetSelection ( 0 ) ;
        irb->Disable() ;
        }
    else
        {
        irb->Enable() ;
        irb->SetSelection ( rf ) ;
        }

    
    sprintf ( t , "%d" , newitems[i]->from ) ;
    ifrom->SetValue ( t ) ;
    
    sprintf ( t , "%d" , newitems[i]->to ) ;
    ito->SetValue ( t ) ;
    
    if ( newitems[i]->direction == 1 ) icb->SetValue ( true ) ;
    else icb->SetValue ( false ) ;

    icv->SetValue ( newitems[i]->isVisible () ) ;
    
    items->SetFocus() ;
    
    icur = i ;
    }

void TVectorEditor::storeItemData ()
    {
    if ( icur == -1 ) return ;

    TVectorItem o = *newitems[icur] ;
    TVectorItem c = o ;
    c.name = iname->GetValue () ;
    c.desc = idesc->GetValue () ;
    c.type = ichoice->GetSelection () ;
    c.from = atoi ( ifrom->GetValue().c_str() ) ;
    c.to = atoi ( ito->GetValue().c_str() ) ;
    c.setRF ( irb->GetSelection() ) ;
    c.setVisible ( icv->GetValue() ) ;
    
    bool b = icb->GetValue () ;
    if ( b ) c.direction = 1 ;
    else c.direction = -1 ;
    
    if ( o.name != c.name ||
         o.desc != c.desc ||
         o.type != c.type ||
         o.from != c.from ||
         o.isVisible() != c.isVisible() ||
         o.getRF() != c.getRF() ||
         o.to != c.to ||
         o.direction != c.direction )
        {
        *newitems[icur] = c ;
        makeItemsList () ;
        }
    }

void TVectorEditor::itemAdd ( wxEvent &ev )
    {
    storeItemData () ;
    int num = newitems.size() ;
    TVectorItem *nvi = new TVectorItem ;
    nvi->name = txt("t_new_item") ;
    nvi->from = 1 ;
    nvi->to = 1 ;
    nvi->direction = 1 ;
    nvi->r2 = -1 ;
    if ( v->type == TYPE_AMINO_ACIDS )
        {
        nvi->setRF ( 0 ) ;
        nvi->type = VIT_MISC ;
        }
    newitems.push_back ( nvi ) ;
    addItem2list ( *nvi , num ) ;
    items->SetItemState ( num , wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED ,
                            wxLIST_MASK_IMAGE|wxLIST_MASK_STATE ) ;
    }
    
void TVectorEditor::itemDel ( wxEvent &ev )
    {
    int i = icur , num ;
    if ( icur == -1 ) return ;
    storeItemData () ;
    for ( num = 0 ; num < newitems.size() ; num++ )
       items->SetItemState ( num , 0 , wxLIST_MASK_IMAGE|wxLIST_MASK_STATE ) ;
    for ( num = 0 ; newitems[num]->r4 != i ; num++ ) ;
    for ( num++ ; num < newitems.size() ; num++ ) newitems[num-1] = newitems[num] ;
    newitems.pop_back () ;
    items->DeleteItem ( i ) ;
    icur = -1 ;
    }
    
void TVectorEditor::itemClr ( wxEvent &ev )
    {
    icur = -1 ;
    iname->SetValue ( "" ) ;
    idesc->SetValue ( "" ) ;
    ifrom->SetValue ( "" ) ;
    ito->SetValue ( "" ) ;
    ichoice->SetSelection ( 0 ) ;
    irb->Disable() ;
    icb->SetValue ( true ) ;
    for ( int num = 0 ; num < newitems.size() ; num++ )
       items->SetItemState ( num , 0 , wxLIST_MASK_IMAGE|wxLIST_MASK_STATE ) ;
    }

void TVectorEditor::itemCol ( wxEvent &ev )
    {
    if ( icur == -1 ) return ;
    storeItemData () ;
    
    TItemEditDialog ied ( (wxWindow*)this , txt("t_edit_item") , *newitems[icur] ) ;
    if ( ied.ShowModal() != wxID_OK ) return ;
    *newitems[icur] = *ied.vi ;

/*
    // "Stable" code
    wxColour col ;
    wxBrush *b = newitems[icur]->getBrush() ;
    col = b->GetColour() ;
    col = wxGetColourFromUser ( this , col ) ;
    if ( !col.Ok() ) return ;
    newitems[icur]->setColor ( col ) ;
*/
    }    
    
void TVectorEditor::hideEm ()
    {
    if ( hideEnzym ) 
        {
        nb->DeletePage ( 2 ) ;
        panEnzym = NULL ;
        }
    if ( hideItem ) 
        {
        nb->DeletePage ( 1 ) ;
        panItem = NULL ;
        }
    if ( hideProp )
        {
        nb->DeletePage ( 0 ) ;
        panProp = NULL ;
        }
    }

// *************************

void TVectorEditor::newEnzyme ( wxEvent &ev )
    {
    TRestrictionEnzyme *e = new TRestrictionEnzyme ;
    e->cut = 0 ;
    e->overlap = 0 ;
    TEnzymeDialog ed ( this , txt("t_new_enzyme") , wxPoint(-1,-1) , wxSize(600,400) , 
                    wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
    ed.initme ( e ) ;
    if ( ed.ShowModal() == wxID_OK )
       {
       app->frame->LS->re.push_back ( ed.e ) ;
       app->frame->LS->updateRestrictionEnzyme ( ed.e ) ;
       }
    showEnzymeGroups () ;
    }
    

void TVectorEditor::newProtease ( wxEvent &ev )
    {
    TRestrictionEnzyme *e = new TRestrictionEnzyme ;
    e->cut = 0 ;
    e->overlap = 0 ;
    TEnzymeDialog ed ( this , txt("t_new_protease") , wxPoint(-1,-1) , wxSize(600,400) , 
                    wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
    ed.initme ( e ) ;
    if ( ed.ShowModal() == wxID_OK )
       {
       TProtease *pr = new TProtease ( ed.e->name , ed.e->sequence , ed.e->note ) ;
       app->frame->LS->pr.push_back ( pr ) ;
       app->frame->LS->updateProtease ( pr ) ;
       }
    showProteases () ;
    }

void TVectorEditor::editProtease ( wxEvent &ev )
    {
    wxMessageBox ( "Not implemented yet!" ) ;
    }

void TVectorEditor::proteaseSelChange ( wxEvent &ev )
    {
    string s = prots->GetStringSelection().c_str() ;
    TProtease *pro = app->frame->LS->getProtease ( s ) ;
    
    string t ;
    t += s + "\n" ;
    t += pro->str_match ;
    pro_txt->SetValue ( t.c_str() ) ;
    }
    
