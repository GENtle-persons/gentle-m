#include "TVectorTree.h"

BEGIN_EVENT_TABLE(TVectorTree, wxTreeCtrl)
    EVT_TREE_SEL_CHANGED(TREE_DUMMY,TVectorTree::OnEvent)
    EVT_TREE_ITEM_ACTIVATED(TREE_DUMMY,TVectorTree::OnActivation)
    EVT_TREE_ITEM_RIGHT_CLICK(TREE_DUMMY,TVectorTree::OnRightClick)

    EVT_MENU(PC_ITEM_MARK, TVectorTree::itemMark)
    EVT_MENU(PC_ITEM_MARK_SHOW, TVectorTree::itemMarkShow)
    EVT_MENU(PC_ITEM_EDIT, TVectorTree::itemEdit)
    EVT_MENU(PC_ITEM_AS_NEW_SEQUENCE, TVectorTree::itemAsNewSequence)
    EVT_MENU(PC_ITEM_DELETE, TVectorTree::itemDelete)
    EVT_MENU(PC_ITEM_HIDE, TVectorTree::itemShowHide)
    EVT_MENU(PC_ITEM_SHOW, TVectorTree::itemShowHide)
    
    EVT_MENU(PRIMER_FORWARD, TVectorTree::OnPrimerForward)
    EVT_MENU(PRIMER_BACKWARD, TVectorTree::OnPrimerBackward)
    EVT_MENU(PRIMER_BOTH, TVectorTree::OnPrimerBoth)
    EVT_MENU(PRIMER_MUTATION, TVectorTree::OnPrimerMutation)
    
    EVT_MENU(PC_RS_EDIT, TVectorTree::rsEdit)
    EVT_MENU(PC_RS_DEL, TVectorTree::rsDel)
    EVT_MENU(PC_RS_SHOW_HIDE, TVectorTree::rsShowHide)
    EVT_MENU(PC_RS_MARK, TVectorTree::rsMark)
    EVT_MENU(PC_RS_MARK_SHOW, TVectorTree::rsMarkShow)
    EVT_MENU(PC_RS_ADD2COCKTAIL, TVectorTree::rsAdd2Cocktail)
    EVT_MENU(PC_RS_ADD_AND_CUT, TVectorTree::rsAddAndCut)
    EVT_MENU(PC_RS_CUT_WITH_COCKTAIL, TVectorTree::rsCutWithCocktail)
    EVT_MENU(PC_VECTOR_EDIT, TVectorTree::vecEdit)
    EVT_MENU(PC_BLAST_DNA, TVectorTree::blastDNA)
    EVT_MENU(PC_BLAST_AA, TVectorTree::blastAA)
END_EVENT_TABLE()


TVectorTree::TVectorTree ( ChildBase *parent , int i )
    :wxTreeCtrl ( parent , i )
    {
    p = (MyChild*) parent ;
    textWindow = NULL ;
    }
    
void TVectorTree::initme ()
    {
    int a , b ;
    char u[1000] ;
    wxTreeItemId x , y ;
    
    // Basic stuff
    DeleteAllItems () ;
    treeroot = AddRoot ( p->vec->getName() ) ;
    sprintf ( u , txt("#bp") , p->vec->getSequenceLength() ) ;
    
    // Vector information
    wxString dp = " : " ;
    vroot = AppendItem ( treeroot , txt("vector") ) ;
    SetItemData ( vroot , new TTreeItem ( "" , "VECTOR" , p->vec ) ) ;
    x = AppendItem ( vroot , txt("name") + dp + p->vec->getName() ) ;
    AppendItem ( vroot , txt("size") + dp + wxString ( u ) ) ;
    
    // Genes
    vector <wxTreeItemId> irs ;
    for ( a = 1 ; a < VIT_TYPES ; a++ )
        {
        sprintf ( u , "itemtype%d" , a ) ;
        irs.push_back ( AppendItem ( treeroot , txt(u) ) ) ;
        }
    for ( a = 0 ; a < p->vec->items.size() ; a++ )
        {
        int it = p->vec->items[a].getType() ;
        y = AppendItem ( irs[it-1] , p->vec->items[a].name , -1 , -1 , new TTreeItem ( "Test" ) ) ;
        SetItemBold ( y , p->vec->items[a].isVisible() ) ;
        
        wxString sFrom , sTo , sLength , sType , sOritentation , sDescription ;
        sprintf ( u , txt("s_from") , p->vec->items[a].from ) ;
        sFrom = u ;
        sprintf ( u , txt("s_to") , p->vec->items[a].to ) ;
        sTo = u ;
        sprintf ( u , txt("s_length") , abs ( p->vec->items[a].to - p->vec->items[a].from ) ) ;
        sLength = u ;
        sprintf ( u , "itemtype%d" , p->vec->items[a].getType() ) ;
        sprintf ( u , txt("s_type") , txt(u) ) ;
        sType = u ;
        if ( p->vec->items[a].getDirection() == 1 ) sOritentation = txt("cw") ;
        else sOritentation = txt("ccw") ;
        sprintf ( u , txt("s_desc") , p->vec->items[a].desc.c_str() ) ;
        sDescription = u ;
        
        p->vec->items[a].setTreeID ( y ) ;
        
        // Item data
        wxString out = p->vec->items[a].name ;
        out += "\n" ;
        out += sFrom + "\n" +
               sTo + "\n" +
               sLength + "\n" +
               sType + "\n" +
               sOritentation + "\n" +
               sDescription ;
        SetItemData ( y , new TTreeItem ( out , "ITEM" , (void*)a ) ) ;
        }
    
    // Enzymes
    enzroot = AppendItem ( treeroot , txt("res_enzymes") ) ;
    for ( a = 0 ; a < p->vec->re.GetCount() ; a++ )
        {
        y = AppendItem ( enzroot , p->vec->re[a]->name.c_str() ) ;
//        p->vec->re[a]->treeid = y ;
        bool used = false , visible = true ;
        wxString out = p->vec->re[a]->name.c_str() ;
        out += "\n" ;
        for ( b = 0 ; !used && b < p->vec->rc.size() ; b++ )
           {
           if ( p->vec->rc[b].e == p->vec->re[a] )
              used = true ;
           }
        if ( !used ) Delete ( y ) ;
        else
           {
           SetItemData ( y , new TTreeItem ( out , "RE" , p->vec->re[a] ) ) ;

           // Visibility check; "used" = "visible"
           for ( b = 0 ; visible && b < p->vec->hiddenEnzymes.GetCount() ; b++ )
              {
              if ( p->vec->hiddenEnzymes[b] == p->vec->re[a]->name )
                 visible = false ;
              }
           SetItemBold ( y , visible ) ;
           }
        }
    
    
    EnsureVisible ( x ) ; // Making vector properties visible
    SelectItem ( vroot ) ;
    }
    
void TVectorTree::OnEvent ( wxTreeEvent &event )
    {
    wxTreeItemId id = event.GetItem () ;
    TTreeItem *d = (TTreeItem*) GetItemData ( id ) ;
    if ( !textWindow ) return ;
    if ( !d ) 
        {
        textWindow->SetValue ( "" ) ;
        return ;
        }
    wxString out ;
    out = d->message.c_str() ;
    if ( d->type == "RE" )
        {
        TRestrictionEnzyme *e = (TRestrictionEnzyme*) d->p ;
        out += txt("res_seq") ;
        out += e->sequence.c_str() ;
        out += "\n" ;
        out += txt("res_loc") ;
        out += e->location.c_str() ;
        out += "\n" ;
        out += txt("res_note") ;
        out += e->note.c_str() ;
        out += "\n" ;
        }
    else if ( d->type == "VECTOR" )
        {
        wxString dp = " : " ;
        TVector *v = (TVector*) d->p ;
        out += txt("name") + dp + v->getName() + "\n\n" ;
        out += txt("desc") + dp + "\n" + v->getDescription() ;
        }
    textWindow->SetValue ( out ) ;
    }

void TVectorTree::OnActivation ( wxTreeEvent &event )
    {
    wxTreeItemId id = event.GetItem () ;
    TTreeItem *d = (TTreeItem*) GetItemData ( id ) ;
    if ( !textWindow ) return ;
    if ( !d ) 
        {
        textWindow->SetValue ( "" ) ;
        return ;
        }
    if ( d->type == "ITEM" )
        {
        wxCommandEvent ev ;
        p->cPlasmid->setLastContextItem ( (int) d->p ) ;
        p->cPlasmid->itemShowHide ( ev ) ;
        }
    else if ( d->type == "RE" )
        {
        TRestrictionEnzyme *e = (TRestrictionEnzyme*) d->p ;
        ToggleEnzymeVisibility ( e ) ;
        p->vec->recalcvisual = true ;
        p->cPlasmid->Refresh () ;
        p->cSequence->arrange() ;
        p->cSequence->SilentRefresh() ;
        }
    }

void TVectorTree::ToggleEnzymeVisibility ( TRestrictionEnzyme *e )
    {
    wxTreeItemId y ;
    long l ;
    y = GetFirstChild ( enzroot , l ) ;
    while ( GetItemText ( y ) != e->name.c_str() )
       y = GetNextChild ( enzroot , l ) ;
    
    if ( IsBold ( y ) )
       {
       p->vec->hiddenEnzymes.Add ( e->name ) ;
       }
    else
       {
       int a ;
       for ( a = 0 ; p->vec->hiddenEnzymes[a] != e->name ; a++ ) ;
       p->vec->hiddenEnzymes.RemoveAt ( a ) ;
       }
    SetItemBold ( y , !IsBold ( y ) ) ;
    p->cPlasmid->Refresh() ;
    p->cSequence->arrange() ;
    p->cSequence->SilentRefresh() ;
    }

// Popup menu handlers
    
void TVectorTree::OnRightClick ( wxTreeEvent &event )
    {
    wxTreeItemId id = event.GetItem () ;
    TTreeItem *d = (TTreeItem*) GetItemData ( id ) ;
    if ( !d ) return ;
    wxPoint pt = event.GetPoint() ;
    if ( d->type == "ITEM" )
        {
        int item = (int) d->p ;
        wxMenu *cm = p->cPlasmid->invokeItemPopup ( item , pt , true ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;
        }
    else if ( d->type == "RE" )
        {
        TRestrictionEnzyme *e = (TRestrictionEnzyme*) d->p ;
        int a ;
        for ( a = 0 ; a < p->vec->rc.size() && p->vec->rc[a].e != e ; a++ ) ;
        if ( a == p->vec->rc.size() ) return ;
        wxMenu *cm = p->cPlasmid->invokeRsPopup ( a , pt , true ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;        
        }
    else if ( d->type == "VECTOR" )
        {
        wxMenu *cm = p->cPlasmid->invokeVectorPopup ( pt , true ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;
        }
    }

void TVectorTree::itemMark ( wxCommandEvent &ev )
    { p->cPlasmid->itemMark ( ev ) ; }
    
void TVectorTree::itemMarkShow ( wxCommandEvent &ev )
    { p->cPlasmid->itemMarkShow ( ev ) ; }
    
void TVectorTree::itemEdit ( wxCommandEvent &ev )
    { p->cPlasmid->itemEdit ( ev ) ; }
    
void TVectorTree::itemDelete ( wxCommandEvent &ev )
    { p->cPlasmid->itemDelete ( ev ) ; }

void TVectorTree::itemAsNewSequence ( wxCommandEvent &ev )
    { p->cPlasmid->itemAsNewSequence ( ev ) ; }

void TVectorTree::itemShowHide ( wxCommandEvent &ev )
    { p->cPlasmid->itemShowHide ( ev ) ; }

void TVectorTree::rsEdit ( wxCommandEvent &ev )
    { p->cPlasmid->rsEdit ( ev ) ; }
    
void TVectorTree::rsInfo ( wxCommandEvent &ev )
    { p->cPlasmid->rsInfo ( ev ) ; }
    
void TVectorTree::rsDel ( wxCommandEvent &ev )
    { p->cPlasmid->rsDel ( ev ) ; }
    
void TVectorTree::rsMark ( wxCommandEvent &ev )
    { p->cPlasmid->rsMark ( ev ) ; }
    
void TVectorTree::rsMarkShow ( wxCommandEvent &ev )
    { p->cPlasmid->rsMarkShow ( ev ) ; }
    
void TVectorTree::rsAdd2Cocktail ( wxCommandEvent &ev )
    { p->cPlasmid->rsAdd2Cocktail ( ev ) ; }
    
void TVectorTree::rsAddAndCut ( wxCommandEvent &ev )
    { p->cPlasmid->rsAddAndCut ( ev ) ; }
    
void TVectorTree::rsCutWithCocktail ( wxCommandEvent &ev )
    { p->cPlasmid->rsCutWithCocktail ( ev ) ; }

void TVectorTree::vecEdit ( wxCommandEvent &ev )
    { p->cPlasmid->vecEdit ( ev ) ; }

void TVectorTree::blastDNA ( wxCommandEvent &ev )
    { p->cPlasmid->blastDNA ( ev ) ; }

void TVectorTree::blastAA ( wxCommandEvent &ev )
    { p->cPlasmid->blastAA ( ev ) ; }

void TVectorTree::rsShowHide ( wxCommandEvent &ev )
    { p->cPlasmid->rsShowHide ( ev ) ; }

void TVectorTree::OnPrimerForward ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerForward ( ev ) ; }

void TVectorTree::OnPrimerBackward ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerBackward ( ev ) ; }

void TVectorTree::OnPrimerBoth ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerBoth ( ev ) ; }

void TVectorTree::OnPrimerMutation ( wxCommandEvent &ev )
    { p->cPlasmid->OnPrimerMutation ( ev ) ; }


