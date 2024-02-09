/** \file
    \brief Contains the TVectorEditor class methods concerning items
*/
#include "TVectorEditor.h"

void TVectorEditor::commitItems ()
    {
    storeItemData () ;
    vector <bool> found ;
    while ( found.size() < v->items.size() ) found.push_back ( false ) ;
    for ( int a = 0 ; a < newitems.GetCount() ; a++ )
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
                 !c.getParam(_T("CHANGED")).IsEmpty() ||
                 o.getRF() != c.getRF() ||
                 o.isVisible() != c.isVisible() ||
                 o.getFontColor() != c.getFontColor() )
                 {
                 v->items[c.r2] = c ;
                 v->items[c.r2].r2 = -1 ;
                 v->setChanged () ;
                 v->updateDisplay() ;
                 }
            }
        else
            {
            v->items.push_back ( c ) ;
            v->items[v->items.size()-1].r2 = -1 ;
            v->setChanged () ;
            v->updateDisplay() ;
            found.push_back ( true ) ;
            }
        }

    vector <TVectorItem> ni ;
    bool doUpdate = false ;
    for ( int a = 0 ; a < found.size() ; a++ )
        {
        if ( found[a] ) ni.push_back ( v->items[a] ) ;
        else doUpdate = true ;
        }
    v->items = ni ;

    if ( doUpdate )
        {
        v->setChanged () ;
        v->updateDisplay() ;
        }

/*
    for ( a = found.size() - 1 ; a >= 0 ; a-- )
        {
        if ( !found[a] )
            {
            for ( b = a+1 ; b < found.size() ; b++ ) v->items[b-1] = v->items[b] ;
            v->items.pop_back () ;
            v->setChanged () ;
            v->updateDisplay() ;
            }
        }
*/
    }

void TVectorEditor::initialViewItem ( const int num )
    {
    nb->SetSelection ( 1 ) ;
    items->SetItemState ( num , wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED ,
                            wxLIST_MASK_IMAGE|wxLIST_MASK_STATE ) ;
    items->EnsureVisible ( num ) ;
    }


int TVectorEditor::getCurrentItem () const
    {
    for ( int a = 0 ; a < items->GetItemCount() ; a++ )
        if ( items->GetItemState ( a , wxLIST_STATE_SELECTED/*wxLIST_STATE_DONTCARE*/ ) != 0 )
           return items->GetItemData ( a ) ;
    return -1 ;
    }

void TVectorEditor::clearItemSelection ()
    {
    lastSelection = -1 ;
    }

void TVectorEditor::initPanItem ()
    {

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h0a = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h0b = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1a = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1b = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *v2 = new wxBoxSizer ( wxVERTICAL ) ;

    int w , h ;
    GetMyClientSize ( &w , &h , panItem ) ;

    // Items list
    items = new wxListCtrl ( panItem , TVE_ILIST , wxDefaultPosition , wxDefaultSize , wxLC_REPORT|wxLC_SINGLE_SEL|MYLISTBORDER ) ;
    items->InsertColumn ( 0 , txt("name") ) ;
    items->InsertColumn ( 1 , txt("type") ) ;
    items->InsertColumn ( 2 , txt("from") ) ;
    items->InsertColumn ( 3 , txt("to") ) ;
    items->InsertColumn ( 4 , txt("direction") ) ;
    items->InsertColumn ( 5 , txt("length") ) ;

    for ( int a = 0 ; a < v->items.size() ; a++ )
        {
        TVectorItem *nvi = new TVectorItem ;
        *nvi = v->items[a] ;
        nvi->r2 = a ;
        nvi->setParam ( _T("CHANGED") , _T("") ) ;
        newitems.Add ( nvi ) ;
        }
    makeItemsList () ;

    // Edit fields
    clearItemSelection () ;
    wxRect r ;
    wxString vs[VIT_TYPES] ;
    char t[1000] ;

    iname = new wxTextCtrl(panItem,-1,_T(""));
    h0a->Add ( new wxStaticText(panItem,-1,txt("name")),0,wxALIGN_CENTER_VERTICAL);
    h0a->Add ( iname , 1 , wxEXPAND ) ;

    icb = new wxCheckBox(panItem,-1,txt("cw"));
    icv = new wxCheckBox(panItem,-1,txt("t_is_visible"));
    h0b->Add ( icb ) ;
    h0b->Add ( icv ) ;

    ifrom = new wxTextCtrl(panItem,-1,_T(""));
    ito = new wxTextCtrl(panItem,-1,_T(""));
    h1a->Add (new wxStaticText(panItem,-1,txt("desc"),wxPoint(bo,h/2+th+bo*2)) , 1 , wxALIGN_BOTTOM ) ;
    h1a->Add (new wxStaticText(panItem,-1,txt("from"),wxPoint(w/4+bo,h/2+th+bo*2)),0,wxALIGN_CENTER_VERTICAL);
    h1a->Add ( ifrom , 0 , wxRIGHT , 5 ) ;
    h1a->Add (new wxStaticText(panItem,-1,txt("to"),wxPoint(w/2+bo,h/2+th+bo*2)),0,wxALIGN_CENTER_VERTICAL);
    h1a->Add ( ito ) ;

    // Item type list
    for ( int a = 0 ; a < VIT_TYPES ; a++ )
        {
        snprintf ( t , sizeof(t)-1, "itemtype%d" , a ) ;
        vs[a] = txt(t) ;
        }
    ichoice = new wxChoice(panItem,TVE_ICHOICE, wxDefaultPosition , wxDefaultSize , VIT_TYPES, vs) ;
    h1b->Add ( ichoice ) ;

    idesc = new TURLtext(panItem,URLTEXT_DUMMY,_T(""), wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER|wxTE_MULTILINE );
    bAdd = new wxButton ( panItem , TVE_I_ADD , txt("b_add_item") ) ;
    bDel = new wxButton ( panItem ,TVE_I_DEL , txt("d_del_item") ) ;
    bCol = new wxButton ( panItem ,TVE_I_COL , txt("t_edit_item") ) ;

    wxString vt[4] ;
    vt[0] = _T("-") ;
    vt[1] = _T("1") ;
    vt[2] = _T("2") ;
    vt[3] = _T("3") ;
    irb = new wxRadioBox ( panItem , -1 , txt("t_reading_frame") , wxDefaultPosition , wxDefaultSize , 4 , vt , wxRA_SPECIFY_COLS ) ;

    v1->Add ( h0a , 0 , wxEXPAND , 2 ) ;
    v1->Add ( h1a , 0 , wxEXPAND , 2 ) ;
    v1->Add ( idesc , 1 , wxEXPAND , 2 ) ;

    v2->Add ( h0b ) ;
    v2->Add ( h1b ) ;
    v2->Add ( irb ) ;
    v2->Add ( bAdd ) ;
    v2->Add ( bDel ) ;
    v2->Add ( bCol ) ;

    if ( v->getType() == TYPE_AMINO_ACIDS )
        {
        icb->Disable () ;
        ichoice->Disable () ;
        irb->Disable () ;
        }

    h2->Add ( v1 , 1, wxEXPAND ) ;
    h2->Add ( v2 , 0 , 0 ) ;

    v0->Add ( items , 1 , wxEXPAND , 5 ) ;
    v0->Add ( h2 , 1 , wxEXPAND , 5 ) ;
    panItem->SetSizer ( v0 ) ;
    nb->AddPage ( panItem , txt("t_vec_item") ) ;
    }


void TVectorEditor::updateItem ( TVectorItem &i )
    {
    int l = i.r4 ;
    wxString t ;
//  char t[1000] ;
    t = wxString::Format ( _T("itemtype%d") , i.type ) ;
//  snprintf ( t , sizeof(t)-1, "itemtype%d" , i.type ) ;
    items->SetItemText ( l , i.name ) ;
    items->SetItem ( l , 1 , txt(t) ) ;

    items->SetItem ( l , 2 , wxString::Format ( _T("%d") , i.from ) ) ;
    items->SetItem ( l , 3 , wxString::Format ( _T("%d") , i.to ) ) ;

    if ( i.direction == 1 )
        items->SetItem ( l , 4 , txt("cw") ) ;
    else if ( i.direction == -1 )
        items->SetItem ( l , 4 , txt("ccw") ) ;
    else
        items->SetItem ( l , 4 , _T("---") ) ;

    int len = i.to - i.from + 1 ;
    if ( i.to < i.from ) len += v->getSequenceLength() ;
    t = wxString::Format ( _T("%d") , len ) ;
//  snprintf ( t , sizeof(t)-1, "%d" , len ) ;
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
    int j = ev.GetIndex() ;
    storeItemData ( items->GetItemData ( j ) ) ;
    lastSelection = -1 ;
    itemClr () ;
    }

void TVectorEditor::SelChangeItems ( wxListEvent &ev )
    {
    int j = ev.GetIndex() ;
    if ( lastSelection == j ) return ;
    lastSelection = j ;
    int i = items->GetItemData ( j ) ;
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


    ifrom->SetValue ( wxString::Format ( _T("%d") , newitems[i]->from ) ) ;
    ito->SetValue ( wxString::Format ( _T("%d") , newitems[i]->to ) ) ;

    if ( newitems[i]->direction == 1 ) icb->SetValue ( true ) ;
    else icb->SetValue ( false ) ;

    icv->SetValue ( newitems[i]->isVisible () ) ;
//  items->SetItemState ( newitems[i]->r4 , wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED ) ;
    }

void TVectorEditor::storeItemData ( int i )
    {
    if ( i == -1 ) i = getCurrentItem() ;
    if ( i == -1 ) return ;
//  if ( lastSelection == -1 ) return ;

    TVectorItem o = *newitems[lastSelection] ;
    TVectorItem c = o ;
    c.name = iname->GetValue () ;
    c.desc = idesc->GetValue () ;
    c.type = ichoice->GetSelection () ;
    c.from = atoi ( ifrom->GetValue().mb_str() ) ;
    c.to = atoi ( ito->GetValue().mb_str() ) ;
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
    int i = getCurrentItem() ;
    if ( i == -1 )
        {
        wxPrintf("W: TVectorEditor::itemDel: Nothing to delete. Why am I called?\n") ;
        return ;
        }
    storeItemData () ;
    delete newitems[i] ;
    newitems.RemoveAt ( i ) ;
    makeItemsList () ;
    itemClr () ;
    int j = i;
    if (j >= newitems.GetCount())
        {
        // i was last element in list
        j = newitems.GetCount() - 1 ;
        }
    if ( j < 0 )
        {
        wxPrintf("I: TVectorEditor::itemDel: Removed last element.\n") ;
        return ;
        }
    // Choosing next element to highlight - if reaching to this point.
    items->SetItemState ( newitems[j]->r4 , wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED ) ;
    items->EnsureVisible ( newitems[j]->r4 ) ;
    }

void TVectorEditor::itemClr ()
    {
    iname->SetValue ( _T("") ) ;
    idesc->SetValue ( _T("") ) ;
    ifrom->SetValue ( _T("") ) ;
    ito->SetValue ( _T("") ) ;
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

