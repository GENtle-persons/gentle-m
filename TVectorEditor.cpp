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
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 620 , 550 ) )
    {
    lastSelection = -1 ;
    v = _v ;
    panProp = panItem = panEnzym = NULL ;
    panEnzyme2 = NULL ;

    int w , h ;
    GetClientSize ( &w , &h ) ;
    bo = 5 ;
    th = 22 ;
    
    nb = new wxNotebook ( (wxWindow*) this ,
                          -1 ,
                          wxPoint ( 0 , 0 ) ,
                          wxSize ( w , h - th - bo*2 ) ) ;

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
    addOkCancel ( this ) ;
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
        
//    addOkCancel ( panProt ) ;
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
    
//    addOkCancel ( panProp ) ;
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
//    addOkCancel ( panEnzyme2 ) ;
    }    
    

// Handlers

void TVectorEditor::addOkCancel ( wxWindow *p )
    {
    int w , h , z ;
    p->GetClientSize ( &w , &h ) ;
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


