#include "TVectorEditor.h"

BEGIN_EVENT_TABLE(TVectorEditor, wxDialog )
    EVT_BUTTON(TVE_OK,TVectorEditor::OnOK)
    EVT_BUTTON(TVE_CANCEL,TVectorEditor::OnCancel)
    EVT_LIST_ITEM_SELECTED(TVE_ILIST,TVectorEditor::SelChangeItems)
    EVT_LIST_ITEM_DESELECTED(TVE_ILIST,TVectorEditor::DeselItems)
    EVT_LIST_ITEM_ACTIVATED(TVE_ILIST,TVectorEditor::itemCol)
    EVT_CHOICE(TVE_ICHOICE,TVectorEditor::itemChoice)

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
    EVT_BUTTON(TVE_EN_DEL_GR,TVectorEditor::enzymeDelGr)
    EVT_BUTTON(TVE_DEL_FROM_GROUP,TVectorEditor::enzymeDelFromGr)
    EVT_BUTTON(TVE_IMPORT,TVectorEditor::importCloneEnzymes)
    EVT_BUTTON(TVE_NEW_ENZYME,TVectorEditor::newEnzyme)
    
    EVT_BUTTON(TVE_EDIT_PROTEASE,TVectorEditor::editProtease)
    EVT_BUTTON(TVE_NEW_PROTEASE,TVectorEditor::newProtease)

    EVT_BUTTON(TVE_I_ADD,TVectorEditor::itemAdd)
    EVT_BUTTON(TVE_I_DEL,TVectorEditor::itemDel)
    EVT_BUTTON(TVE_I_COL,TVectorEditor::itemCol2)
    
    EVT_CHAR_HOOK(TVectorEditor::OnCharHook)
END_EVENT_TABLE()

void TVectorEditor::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

TVectorEditor::TVectorEditor(wxWindow *parent, const wxString& title , TVector *_v )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 550 ) )
    {
    lastSelection = -1 ;
    v = _v ;
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
    wxArrayString vs ;
    for ( a = 0 ; a < myapp()->frame->LS->pr.GetCount() ; a++ )
        vs.Add ( myapp()->frame->LS->pr[a]->name ) ;
    vs.Sort () ;        
    prots->Clear () ;
    for ( a = 0 ; a < vs.GetCount() ; a++ )
        {
        wxString n = vs[a] ;
        prots->Append ( n ) ;
        for ( b = 0 ; b < v->proteases.GetCount() && v->proteases[b] != n ; b++ ) ;
        if ( b < v->proteases.GetCount() ) prots->Check ( prots->FindString ( n ) ) ;
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
    name = new wxTextCtrl(panProp,-1,v->getName(),wxPoint(r.GetRight()+bo,bo),
                                wxSize(w-r.GetRight()-bo*2,th)) ;

    r = (new wxStaticText(panProp,-1,txt("desc"),wxPoint(bo,bo*2+th)))->GetRect() ;
    desc = new TURLtext(panProp,URLTEXT_DUMMY,v->getDescription(),wxPoint(bo,r.GetBottom()+bo),
                wxSize(w-bo*2,th*5),wxTE_MULTILINE);
    
    if ( v->type != TYPE_AMINO_ACIDS )
        {
        // Sticky ends
        r = (new wxStaticText(panProp,-1,"5'-",wxPoint(bo,th*8)))->GetRect() ;
        lu = new wxTextCtrl(panProp,-1,v->getStickyEnd(true,true),
                                wxPoint(r.GetRight()+bo,r.GetTop()-bo),wxSize(w/5,th));
    
        r = (new wxStaticText(panProp,-1,"-3'",wxPoint(w-r.GetRight(),r.GetTop())))->GetRect() ;
        ru = new wxTextCtrl(panProp,-1,v->getStickyEnd(false,true),
                                wxPoint(r.GetLeft()-bo-w/5,r.GetTop()-bo),wxSize(w/5,th));
        
        wxString k ;
        while ( k.length() * (r.GetWidth()/3) < w/2 ) k += "-" ;
        new wxStaticText(panProp,-1,k,wxPoint(lu->GetRect().GetRight()+bo,r.GetTop())) ;
    
        r = (new wxStaticText(panProp,-1,"3'-",wxPoint(bo,r.GetBottom()+th)))->GetRect() ;
        ll = new wxTextCtrl(panProp,-1,v->getStickyEnd(true,false),
                                wxPoint(r.GetRight()+bo,r.GetTop()-bo),wxSize(w/5,th));
    
        r = (new wxStaticText(panProp,-1,"-5'",wxPoint(w-r.GetRight(),r.GetTop())))->GetRect() ;
        rl = new wxTextCtrl(panProp,-1,v->getStickyEnd(false,false),
                                wxPoint(r.GetLeft()-bo-w/5,r.GetTop()-bo),wxSize(w/5,th));
    
        new wxStaticText(panProp,-1,k,wxPoint(lu->GetRect().GetRight()+bo,r.GetTop())) ;
        }    
    
    addOkCancel ( panProp ) ;
    }
    
void TVectorEditor::initPanEnzym2 ()
    {
    oldEnzymeRules = v->getEnzymeRule() ;
    e_diduseit = oldEnzymeRules->useit ;
    if ( !v->enzyme_rules )
    	{
	    v->enzyme_rules = new TEnzymeRules ;
	    *v->enzyme_rules = *oldEnzymeRules ;
	    v->enzyme_rules->useit = false ;
    	}    
    panEnzyme2 = new TEnzymeSettingsTab ( nb , EST_SINGLE ) ;
    v->enzyme_rules->setup_options ( panEnzyme2 ) ;
    nb->AddPage ( panEnzyme2 , txt("t_enzymes_2") ) ;    
    addOkCancel ( panEnzyme2 ) ;
    }    
    
void TVectorEditor::initPanEnzym ()
    {
    nb->AddPage ( panEnzym , txt("t_vec_enzym") ) ;
    initPanEnzym2 () ;
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

    b_atg = new wxButton ( panEnzym , TVE_EN_ADD_TO_GR , txt("b_add_to_group") ,
            wxPoint ( bx1+bo , by1+bo+th ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;

    new wxButton ( panEnzym , TVE_EN_ADD_TO_NEW_GR , txt("b_add_as_new_group") ,
            wxPoint ( bx1+bo , by1+bo+th*2 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;
            
    b_dg = new wxButton ( panEnzym , TVE_EN_DEL_GR , txt("b_del_group") ,
            wxPoint ( bx1+bo , by1+bo+th*3 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;
            
    if ( !v )
        new wxButton ( panEnzym , TVE_IMPORT , txt("import_clone_enzymes") ,
               wxPoint ( bx1+bo , by1+bo+th*4 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;
               
    new wxButton ( panEnzym , TVE_NEW_ENZYME , txt("add_new_enzyme") ,
           wxPoint ( bx1+bo , by1+bo*2+th*5 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;
            
    new wxButton ( panEnzym , TVE_EN_ADD_EN , txt("<-- add") , 
            wxPoint ( bx1+bo , by2+bo ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;

    new wxButton ( panEnzym , TVE_EN_DEL_EN , txt("del -->") , 
            wxPoint ( bx1+bo , by2+bo+th ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;

    b_dfg = new wxButton ( panEnzym , TVE_DEL_FROM_GROUP , txt("b_del_from_group") , 
            wxPoint ( bx1+bo , by2+bo+th*3 ) , wxSize ( bx2-bx1-bo*2 , bh ) ) ;


    listCE->Clear() ;
    listGroups->Clear() ;
    showEnzymeGroups () ;
    
    int a ;
    ce.Clear() ;
    for ( a = 0 ; v && a < v->re.GetCount() ; a++ )
        {
        wxString s = v->re[a]->name ;
        listCE->Append ( s ) ;
        ce.Add ( s ) ;
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
#define VCOMMIT_STICKY(_p1,_p2,_p3) if(_p1!=v->getStickyEnd(_p2,_p3))\
                    {v->setStickyEnd(_p2,_p3,_p1);v->setChanged();}
    
void TVectorEditor::commitVector ()
    {
    wxString n = name->GetValue() ;
    wxString d = desc->GetValue() ;
    if(n!=v->getName()){v->setName(n);v->setChanged();}
    if(d!=v->getDescription()){v->setDescription(d);v->setChanged();}

    if ( v->type != TYPE_AMINO_ACIDS )
        {
        wxString _lu = lu->GetValue().MakeUpper() ;
        wxString _ll = ll->GetValue().MakeUpper() ;
        wxString _ru = ru->GetValue().MakeUpper() ;
        wxString _rl = rl->GetValue().MakeUpper() ;
        VCOMMIT_STICKY(_lu,true,true);
        VCOMMIT_STICKY(_ll,true,false);
        VCOMMIT_STICKY(_ru,false,true);
        VCOMMIT_STICKY(_rl,false,false);
        }
    }
    

void TVectorEditor::commitEnzymes ()
    {
    int a , b ;
    bool changed = false ;
    
    // Removed enzymes
    for ( a = 0 ; a < v->re.GetCount() ; a++ )
        {
        for ( b = 0 ; b < ce.GetCount() && ce[b] != v->re[a]->name ; b++ ) ;
        if ( b == ce.GetCount() )
           {
           changed = true ;
           v->re.RemoveAt ( a ) ;
           a-- ;
           }
        }

    // Added enzymes
    for ( b = 0 ; b < ce.GetCount() ; b++ )
        {
        for ( a = 0 ; a < v->re.GetCount() && ce[b] != v->re[a]->name ; a++ ) ;
        if ( a == v->re.GetCount() )
           {
           changed = true ;
           v->re.Add ( myapp()->frame->LS->getRestrictionEnzyme ( ce[b] )  ) ;
           }
        }
        
    v->enzyme_rules->lookup_options ( panEnzyme2 ) ;
    if ( v->enzyme_rules != oldEnzymeRules || 
    	( v->enzyme_rules->useit != e_diduseit ) )
    	{
    	changed = true ;
    	v->setParam ( "enzymerule" , v->enzyme_rules->to_string() ) ;
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
    
    wxArrayString v1 , v2 ;
    for ( a = 0 ; a < v->proteases.GetCount() ; a++ )
        v1.Add ( v->proteases[a] ) ;
    
    for ( a = 0 ; a < prots->GetCount() ; a++ )
        if ( prots->IsChecked ( a ) )
           v2.Add ( prots->GetString(a) ) ;
    
    v1.Sort () ;
    v2.Sort () ;
    
    if ( v1.GetCount() != v2.GetCount() ) changed = true ;
    for ( a = 0 ; !changed && a < v1.GetCount() ; a++ )
        if ( v1[a] != v2[a] )
           changed = true ;
    
    if ( changed )
        {
        v->proteases.Clear() ;
        for ( a = 0 ; a < v2.GetCount() ; a++ )
           v->proteases.Add ( v2[a] ) ;
        }
    
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
    CLEAR_DELETE ( newitems ) ;
    }
    
// Handlers "enzyme"

void TVectorEditor::enzymeSelChange ( wxCommandEvent &ev )
    {
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGroups )
        {
        showGroupEnzymes ( lb->GetString ( lb->GetSelection() ) ) ;
        }
    }
    
void TVectorEditor::enzymeListDlbClick ( wxCommandEvent &ev )
    {
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGE )
        {
        wxArrayInt vi ;
        int i , k , n = lb->GetSelections ( vi ) ;
        for ( k = 0 ; k < n ; k++ )
            {
            wxString s = lb->GetString ( vi[k] ) ;
            TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
            TEnzymeDialog ed ( this , s , wxPoint(-1,-1) , wxSize(600,400) , 
                            wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
            ed.initme ( e ) ;
            if ( ed.ShowModal() == wxID_OK )
               {
/*               ed.e->name = e->name ; // No name change!
               if ( e->differ ( *ed.e ) )
                  {
                  *e = *ed.e ;
                  myapp()->frame->LS->updateRestrictionEnzyme ( e ) ;
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
        wxString s = lb->GetString ( vi[0] ) ;
        TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
        TEnzymeDialog ed ( this , s , wxPoint(-1,-1) , wxSize(600,400) , 
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
    wxArrayString vs ;
    myapp()->frame->LS->getEnzymeGroups ( vs ) ;
    for ( int i = 0 ; i < vs.GetCount() ; i++ )
        listGroups->Append ( vs[i] ) ;
    showGroupEnzymes ( all ) ;
    listGroups->SetStringSelection ( all ) ;
    }
    
void TVectorEditor::showGroupEnzymes ( wxString gr )
    {
    wxArrayString vs ;
    listGE->Clear() ;
    wxString gr2 = myapp()->frame->LS->UCfirst ( gr ) ;
    myapp()->frame->LS->getEnzymesInGroup ( gr2 , vs ) ;
    eig.Clear () ;
    for ( int i = 0 ; i < vs.GetCount() ; i++ )
        {
        listGE->Append ( vs[i] ) ;
        eig.Add ( vs[i] ) ;
        }
    if ( gr2 == txt("All") )
        {
        b_atg->Disable() ;
        b_dg->Disable() ;
        b_dfg->Disable() ;
        }
    else
        {
        b_atg->Enable() ;
        b_dg->Enable() ;
        b_dfg->Enable() ;
        }
    }

void TVectorEditor::enzymeAddEn ( wxCommandEvent &ev )
    {
    wxArrayInt vi ;
    int i , k , n = listGE->GetSelections ( vi ) ;
    for ( k = 0 ; k < n ; k++ )
        {
        i = vi[k] ;
        wxString s = listGE->GetString ( i ) ;
        listGE->Deselect ( i ) ;
        int p = listCE->FindString ( s ) ;
        if ( p == wxNOT_FOUND )
            {
            listCE->Append ( s ) ;
            ce.Add ( s ) ;
            }
        }
    }

void TVectorEditor::enzymeAddGr ( wxCommandEvent &ev )
    {
    int a , b ;
    wxString s ;
    for ( a = 0 ; a < eig.GetCount() ; a++ )
        {
        s = eig[a] ;
        b = listCE->FindString ( s ) ;
        if ( b == wxNOT_FOUND )
           {
           listCE->Append ( s ) ;
           ce.Add ( s ) ;
           }
        }
    }

void TVectorEditor::enzymeAddToGr ( wxCommandEvent &ev )
    {
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;
    wxString sql ;
    wxArrayInt vi ;
    int i , k , n = listCE->GetSelections ( vi ) ;
    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        wxString s = listCE->GetString(i) ;
        sql = "DELETE FROM link_enzyme_group WHERE "
              "leg_enzyme=\"" +
              s +
              "\" AND leg_group=\"" +
              group +
              "\"" ;
        myapp()->frame->LS->getObject ( sql ) ;
        sql = "INSERT INTO link_enzyme_group (leg_enzyme,leg_group) "
              "VALUES (\"" +
              s + 
              "\",\"" +
              group + 
              "\")" ;
        myapp()->frame->LS->getObject ( sql ) ;              
        }
    showGroupEnzymes ( group ) ;
    }

void TVectorEditor::enzymeAddToNewGr ( wxCommandEvent &ev )
    {
    wxTextEntryDialog ted ( this , txt("b_add_as_new_group") ,
                                txt("t_new_enzyme_group_name") ) ;
    if ( wxID_OK != ted.ShowModal () ) return ;
    wxString ng = ted.GetValue() ;
    ng = myapp()->frame->LS->UCfirst ( ng ) ;
    if ( !myapp()->frame->LS->addEnzymeGroup ( ng ) ) return ;
    listGroups->Append ( ng ) ;
    listGroups->SetStringSelection ( ng ) ;
    enzymeAddToGr ( ev ) ;
    }

void TVectorEditor::enzymeDelEn ( wxCommandEvent &ev )
    {
    wxArrayInt vi ;
    int i , k , n = listCE->GetSelections ( vi ) ;
    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        wxString s = listCE->GetString ( i ) ;
        listCE->Delete ( i ) ;
        for ( i = 0 ; i < ce.GetCount() ; i++ )
            {
            if ( ce[i] == s )
               {
               ce.RemoveAt ( i ) ;
               /*
               ce[i] = ce[ce.size()-1] ;
               ce.pop_back () ;
               */
               }
            }
        }
    }
    
void TVectorEditor::initialViewEnzyme ( wxString e )
    {
    nb->SetSelection ( 2 ) ;
    listCE->SetStringSelection ( e ) ;
    }
    
void TVectorEditor::importCloneEnzymes ( wxCommandEvent &ev )
    {
    TStorage TS ( TEMP_STORAGE ) ;
    TClone clone ;
    clone.loadEnzymeList ( &TS , "./CLONE.ENZ" ) ;
    int a ;
    for ( a = 0 ; a < TS.re.GetCount() ; a++ )
       {
       listCE->Append ( TS.re[a]->name ) ;
       myapp()->frame->LS->re.Add ( TS.re[a] ) ;
       myapp()->frame->LS->updateRestrictionEnzyme ( TS.re[a] ) ;
       TS.re[a] = NULL ; // avoid deletion
       }
    wxString group = listGroups->GetStringSelection() ;
    showGroupEnzymes ( group ) ;
    }

    
void TVectorEditor::hideEm ()
    {
    if ( hideEnzym ) 
        {
        nb->DeletePage ( 3 ) ;
        nb->DeletePage ( 2 ) ;
        panEnzym = NULL ;
        panEnzyme2 = NULL ;
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
    if ( panProp == NULL )
        {
        nb->SetSelection ( 1 ) ;
        nb->SetSelection ( 0 ) ;
        }
    }

// *************************

void TVectorEditor::newEnzyme ( wxCommandEvent &ev )
    {
    TRestrictionEnzyme *e = new TRestrictionEnzyme ;
    e->cut = 0 ;
    e->overlap = 0 ;
    TEnzymeDialog ed ( this , txt("t_new_enzyme") , wxPoint(-1,-1) , wxSize(600,400) , 
                    wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
    ed.initme ( e ) ;
    if ( ed.ShowModal() == wxID_OK )
       {
       myapp()->frame->LS->re.Add ( ed.e ) ;
       myapp()->frame->LS->updateRestrictionEnzyme ( ed.e ) ;
       }
    showEnzymeGroups () ;
    }
    

void TVectorEditor::newProtease ( wxCommandEvent &ev )
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
       myapp()->frame->LS->pr.Add ( pr ) ;
       myapp()->frame->LS->updateProtease ( pr ) ;
       }
    showProteases () ;
    }

void TVectorEditor::editProtease ( wxCommandEvent &ev )
    {
    wxMessageBox ( "Not implemented yet!" ) ;
    }

void TVectorEditor::proteaseSelChange ( wxCommandEvent &ev )
    {
    wxString s = prots->GetStringSelection() ;
    TProtease *pro = myapp()->frame->LS->getProtease ( s ) ;
    
    wxString t ;
    t += s + "\n" ;
    t += pro->str_match ;
    pro_txt->SetValue ( t ) ;
    }

void TVectorEditor::enzymeDelGr ( wxCommandEvent &ev )
    {
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;
    
    wxString s = wxString::Format ( txt("t_del_group") , group ) ;
    if ( wxMessageBox ( s , txt("msg_box") , wxYES_NO|wxICON_QUESTION ) != wxYES ) return ;

    wxString sql ;
    sql = "DELETE FROM link_enzyme_group WHERE leg_group=\"" + group + "\"" ;
    myapp()->frame->LS->getObject ( sql ) ; 
    sql = "DELETE FROM enzyme_group WHERE eg_name=\"" + group + "\"" ;
    myapp()->frame->LS->getObject ( sql ) ; 
    showEnzymeGroups () ;
    }

void TVectorEditor::enzymeDelFromGr ( wxCommandEvent &ev )
    {
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;
    
    wxString sql ;
    wxArrayInt vi ;
    int i , k , n = listGE->GetSelections ( vi ) ;
    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        wxString s = listGE->GetString(i) ;
        sql = "DELETE FROM link_enzyme_group WHERE "
              "leg_enzyme=\"" +
              s +
              "\" AND leg_group=\"" +
              group +
              "\"" ;
        myapp()->frame->LS->getObject ( sql ) ;              
        }
    showGroupEnzymes ( group ) ;
    }
    
