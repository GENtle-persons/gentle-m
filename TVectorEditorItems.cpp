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
    items->EnsureVisible ( num ) ;
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
    
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxFlexGridSizer *f0 = new wxFlexGridSizer ( 3 , 2 , 5 , 5 ) ;
    wxBoxSizer *h0a = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h0b = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1a = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1b = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *v2 = new wxBoxSizer ( wxVERTICAL ) ;

    int w , h ;
    GetMyClientSize ( &w , &h , panItem ) ;

//    panItem->SetFont ( *MYFONT ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ) ;    
    
    // Items list
    items = new wxListCtrl ( panItem , TVE_ILIST ,
			     wxDefaultPosition , wxDefaultSize ,
			     wxLC_REPORT|wxLC_SINGLE_SEL|MYLISTBORDER ) ;
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
    
    iname = new wxTextCtrl(panItem,-1,"");
    h0a->Add ( new wxStaticText(panItem,-1,txt("name")),0,wxALIGN_CENTER_VERTICAL);
    h0a->Add ( iname , 1 , wxEXPAND ) ;

    icb = new wxCheckBox(panItem,-1,txt("cw"));
    icv = new wxCheckBox(panItem,-1,txt("t_is_visible"));
    h0b->Add ( icb ) ;
    h0b->Add ( icv ) ;

    ifrom = new wxTextCtrl(panItem,-1,"");
    ito = new wxTextCtrl(panItem,-1,"");
    h1a->Add (new wxStaticText(panItem,-1,txt("desc"),wxPoint(bo,h/2+th+bo*2)) , 1 , wxALIGN_BOTTOM ) ;
    h1a->Add (new wxStaticText(panItem,-1,txt("from"),wxPoint(w/4+bo,h/2+th+bo*2)),0,wxALIGN_CENTER_VERTICAL);
    h1a->Add ( ifrom , 0 , wxRIGHT , 5 ) ;
    h1a->Add (new wxStaticText(panItem,-1,txt("to"),wxPoint(w/2+bo,h/2+th+bo*2)),0,wxALIGN_CENTER_VERTICAL);
    h1a->Add ( ito ) ;

    // Item type list
    for ( a = 0 ; a < VIT_TYPES ; a++ )
        {
        sprintf ( t , "itemtype%d" , a ) ;
        vs[a] = txt(t) ;
        }
    ichoice = new wxChoice(panItem,TVE_ICHOICE,
			   wxDefaultPosition , wxDefaultSize ,
			   VIT_TYPES,
			   vs) ;
    h1b->Add ( ichoice ) ;

    idesc = new TURLtext(panItem,URLTEXT_DUMMY,"",
			 wxDefaultPosition , wxDefaultSize ,
			 wxTE_PROCESS_ENTER|wxTE_MULTILINE );

    bAdd = new wxButton ( panItem , TVE_I_ADD , txt("b_add_item") ) ;
    bDel = new wxButton ( panItem ,TVE_I_DEL , txt("d_del_item") ) ;
    bCol = new wxButton ( panItem ,TVE_I_COL , txt("t_edit_item") ) ;

    wxString vt[4] ;
    vt[0] = "-" ;
    vt[1] = "1" ;
    vt[2] = "2" ;
    vt[3] = "3" ;
    irb = new wxRadioBox ( panItem , -1 , txt("t_reading_frame") ,
			   wxDefaultPosition , wxDefaultSize ,
			   4 , vt , wxRA_SPECIFY_COLS ) ;

    v1->Add ( h0a , 0 , wxEXPAND|wxALL , 2 ) ;
    v1->Add ( h1a , 0 , wxEXPAND|wxALL , 2 ) ;
    v1->Add ( idesc , 1 , wxEXPAND|wxALL , 2 ) ;

    v2->Add ( h0b ) ;
    v2->Add ( h1b ) ;
    v2->Add ( irb ) ;
    v2->Add ( bAdd ) ;
    v2->Add ( bDel ) ;
    v2->Add ( bCol ) ;

    if ( v->type == TYPE_AMINO_ACIDS )
        {
        icb->Disable () ;
        ichoice->Disable () ;
        irb->Disable () ;
        }
    
//    addOkCancel ( panItem ) ;

/*
    f0->Add ( h0a , 1 , wxEXPAND ) ;
    f0->Add ( h0b , 0 , 0 );
    f0->Add ( h1a , 1 , wxEXPAND ) ;
    f0->Add ( h1b , 0 , 0 );
    f0->Add ( idesc , 1 , wxEXPAND ) ;
    f0->Add ( v2 , 0 , 0 );
*/
    h2->Add ( v1 , 1, wxEXPAND ) ;
    h2->Add ( v2 , 0 , 0 ) ;

    v0->Add ( items , 1 , wxEXPAND|wxALL , 5 ) ;
    v0->Add ( h2 , 1 , wxEXPAND|wxALL , 5 ) ;
//    v0->Add ( f0 , 1 , wxEXPAND|wxALL , 5 ) ;
    panItem->SetSizer ( v0 ) ;
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

