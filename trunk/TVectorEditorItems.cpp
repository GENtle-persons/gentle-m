#include "TVectorEditor.h"

void TVectorEditor::commitItems ()
    {
    storeItemData () ;
    int a , b ;
    vector <bool> found ;
    while ( found.size() < v->items.size() ) found.push_back ( false ) ;
    for ( a = 0 ; a < newitems.GetCount() ; a++ )
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
                 !c.getParam("CHANGED").IsEmpty() ||
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

void TVectorEditor::initialViewItem ( int num )
    {
    nb->SetSelection ( 1 ) ;
    items->SetItemState ( num , wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED ,
                            wxLIST_MASK_IMAGE|wxLIST_MASK_STATE ) ;
    }


int TVectorEditor::getCurrentItem ()
    {
    for ( int a = 0 ; a < items->GetItemCount() ; a++ )
        if ( items->GetItemState ( a , wxLIST_STATE_DONTCARE ) != 0 )
           return items->GetItemData ( a ) ;
    return -1 ;
    }
    
void TVectorEditor::clearItemSelection ()
    {
    lastSelection = -1 ;
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
        newitems.Add ( nvi ) ;
        }
    makeItemsList () ;
    
    // Edit fields
    clearItemSelection () ;
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
    ichoice = new wxChoice(panItem,TVE_ICHOICE,
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






void TVectorEditor::updateItem ( TVectorItem &i )
    {
    int l = i.r4 ;
    char t[1000] ;
    sprintf ( t , "itemtype%d" , i.type ) ;
    items->SetItemText ( l , i.name ) ;
    items->SetItem ( l , 1 , txt(t) ) ;

    items->SetItem ( l , 2 , wxString::Format ( "%d" , i.from ) ) ;
    items->SetItem ( l , 3 , wxString::Format ( "%d" , i.to ) ) ;

    if ( i.direction == 1 )
       items->SetItem ( l , 4 , txt("cw") ) ;
    else if ( i.direction == -1 )
       items->SetItem ( l , 4 , txt("ccw") ) ;
    else
       items->SetItem ( l , 4 , "---" ) ;

    int len = i.to - i.from + 1 ;
    if ( i.to < i.from ) len += v->getSequenceLength() ;
    sprintf ( t , "%d" , len ) ;
    items->SetItem ( l , 5 , t ) ;    
    }    

void TVectorEditor::addItem2list ( TVectorItem &i , int a )
    {
    int l = items->InsertItem ( a , i.name ) ;
    items->SetItemData ( l , a ) ;
    i.r4 = l ;
    updateItem ( i ) ;
    }
    
void TVectorEditor::makeItemsList ()
    {
    items->DeleteAllItems() ;
    for ( int a = 0 ; a < newitems.GetCount() ; a++ )
        addItem2list ( *newitems[a] , a ) ;
    lastSelection = -1 ;
    }
    
// handlers ITEM
    
void TVectorEditor::DeselItems ( wxListEvent &ev )
    {
    int i , j = ev.GetIndex() ;
    storeItemData ( items->GetItemData ( j ) ) ;
    lastSelection = -1 ;
    itemClr () ;
    }
        
void TVectorEditor::SelChangeItems ( wxListEvent &ev )
    {
    int i , j = ev.GetIndex() ;
    if ( lastSelection == j ) return ;
    lastSelection = j ;
    i = items->GetItemData ( j ) ;
    loadItemData ( i ) ;
    items->SetFocus() ;
    }

void TVectorEditor::loadItemData ( int i )
    {
    if ( i == -1 ) return ;
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

    
    ifrom->SetValue ( wxString::Format ( "%d" , newitems[i]->from ) ) ;
    ito->SetValue ( wxString::Format ( "%d" , newitems[i]->to ) ) ;
    
    if ( newitems[i]->direction == 1 ) icb->SetValue ( true ) ;
    else icb->SetValue ( false ) ;

    icv->SetValue ( newitems[i]->isVisible () ) ;
//    items->SetItemState ( newitems[i]->r4 , wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED ) ;
    }

void TVectorEditor::storeItemData ( int i )
    {
    if ( i == -1 ) i = getCurrentItem() ;
    if ( i == -1 ) return ;
//    if ( lastSelection == -1 ) return ;

    TVectorItem o = *newitems[lastSelection] ;
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
        *newitems[i] = c ;
        updateItem ( c ) ;
        items->EnsureVisible ( newitems[i]->r4 ) ;
        }
    }

void TVectorEditor::itemAdd ( wxCommandEvent &ev )
    {
    storeItemData () ;
    int num = newitems.GetCount() ;
    TVectorItem *nvi = new TVectorItem ;
    nvi->name = txt("t_new_item") ;
    nvi->from = 1 ;
    nvi->to = 1 ;
    nvi->direction = 1 ;
    nvi->r2 = -1 ;
    nvi->type = VIT_MISC ;
    nvi->setRF ( 0 ) ;
    newitems.Add ( nvi ) ;
    addItem2list ( *nvi , num ) ;
    items->SetItemState ( num , wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED ,
                            wxLIST_MASK_IMAGE|wxLIST_MASK_STATE ) ;
    items->EnsureVisible ( nvi->r4 ) ;
    }
    
void TVectorEditor::itemDel ( wxCommandEvent &ev )
    {
    int i = getCurrentItem() , num ;
    if ( i == -1 ) return ;
    storeItemData () ;
    delete newitems[i] ;
    newitems.RemoveAt ( i ) ;
    makeItemsList () ;
    itemClr () ;
    while ( i >= 0 && i >= newitems.GetCount() ) i-- ;
    if ( i < 0 ) return ;
    items->SetItemState ( newitems[i]->r4 , wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED ) ;
    items->EnsureVisible ( newitems[i]->r4 ) ;
    }
    
void TVectorEditor::itemClr ()
    {
    iname->SetValue ( "" ) ;
    idesc->SetValue ( "" ) ;
    ifrom->SetValue ( "" ) ;
    ito->SetValue ( "" ) ;
    ichoice->SetSelection ( 0 ) ;
    irb->Disable() ;
    icb->SetValue ( true ) ;
    }

// Invokes "Edit feature" dialog
// Color, sequence display type, AA offset etc.
void TVectorEditor::itemColInternal()
    {
    if ( getCurrentItem() == -1 ) return ;
    storeItemData () ;    
    TItemEditDialog ied ( (wxWindow*)this , txt("t_edit_item") , *newitems[getCurrentItem()] ) ;
    if ( ied.ShowModal() != wxID_OK ) return ;
    *newitems[getCurrentItem()] = *ied.vi ;
    }    

void TVectorEditor::itemCol ( wxListEvent &ev )
    {
    itemColInternal() ;
    }    
    
void TVectorEditor::itemCol2 ( wxCommandEvent &ev )
    {
    itemColInternal() ;
    }    
    
// Item choice dropdown box handler
// Enables reading frame selection box if item type is CDS,
// disables it if not
void TVectorEditor::itemChoice ( wxCommandEvent &ev )
    {
    int i = ichoice->GetSelection () ;
    if ( i == -1 ) return ;
    if ( i == VIT_CDS ) irb->Enable() ;
    else irb->Disable() ;
    }

