/** \file
	\brief The vector items tree component.
*/
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
    EVT_MENU(PC_ITEM_COPY_DNA, TVectorTree::itemCopyDNA)
    EVT_MENU(PC_ITEM_BLAST_DNA, TVectorTree::blastDNA)
    EVT_MENU(PC_ITEM_BLAST_AA, TVectorTree::blastAA)
    EVT_MENU(PC_ITEM_COPY_AA , TVectorTree::copyAA)
    EVT_MENU(PC_ITEM_AS_NEW_AA_SEQUENCE , TVectorTree::AAasNewSequence)
    
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
END_EVENT_TABLE()


/** \brief Constructor
*/
TVectorTree::TVectorTree ( ChildBase *parent , int i )
    :wxTreeCtrl ( parent , i , wxDefaultPosition , wxSize ( 200 , 200 ) )
    {
    p = (MyChild*) parent ;
    textWindow = NULL ;
    }
    
/** \brief Initializes the tree

	Parses the vector and creates a list of
	* - vector information
	* - marked regions (genes etc.)
	* - restriction enzymes
*/
void TVectorTree::initme ()
    {
    int a , b ;
//    char u[1000] ;
	 wxString u ;
    wxTreeItemId x , y ;
    
    // Basic stuff
    Freeze () ;
    DeleteAllItems () ;
    treeroot = AddRoot ( p->vec->getName() ) ;
	 u = wxString::Format ( txt("#bp") , p->vec->getSequenceLength() ) ;
//    sprintf ( u , txt("#bp").mb_str() , p->vec->getSequenceLength() ) ;
    
    // Vector information
    wxString dp = _T(" : ") ;
    vroot = AppendItem ( treeroot , txt("vector") ) ;
    SetItemData ( vroot , new TTreeItem ( _T("") , _T("VECTOR") , p->vec ) ) ;
    x = AppendItem ( vroot , txt("name") + dp + p->vec->getName() ) ;
    AppendItem ( vroot , txt("size") + dp + u ) ;
    
    // Genes
    vector <wxTreeItemId> irs ;
    for ( a = 1 ; a < VIT_TYPES ; a++ )
        {
		  u = wxString::Format ( _T("itemtype%d") , a ) ;
        irs.push_back ( AppendItem ( treeroot , txt(u) ) ) ;
        }

    for ( a = 0 ; a < p->vec->items.size() ; a++ )
        {
        if ( p->vec->items[a].name.IsEmpty() )
           {
           p->vec->items[a].setTreeID ((void*)NULL);//( (long int) -1 ) ; //2.6/2.8
           continue ;
           }
        int it = p->vec->items[a].getType() ;
        wxTreeItemId iti = irs[VIT_MISC-1] ;
        if ( it >= 1 && it-1 < VIT_TYPES ) iti = irs[it-1] ;
        y = AppendItem ( iti , p->vec->items[a].name , -1 , -1 , new TTreeItem ( _T("Test" )) ) ;

        SetItemBold ( y , p->vec->items[a].isVisible() ) ;

        wxString sFrom , sTo , sLength , sType , sOritentation , sDescription ;
        sFrom = wxString::Format ( txt("s_from") , p->vec->items[a].from ) ;
        sTo = wxString::Format ( txt("s_to") , p->vec->items[a].to ) ;
        sLength = wxString::Format ( txt("s_length") , abs ( p->vec->items[a].to - p->vec->items[a].from ) ) ;
		  u = wxString::Format ( _T("itemtype%d") , p->vec->items[a].getType() ) ;
        sType = wxString::Format ( txt("s_type").c_str() , txt(u).c_str() ) ;
        if ( p->vec->items[a].getDirection() == 1 ) sOritentation = txt("cw") ;
        else sOritentation = txt("ccw") ;
        sDescription = wxString::Format ( txt("s_desc") , p->vec->items[a].desc.c_str() ) ;
        
        p->vec->items[a].setTreeID ( y ) ;
        
        // Item data
        wxString out = p->vec->items[a].name ;
        out += _T("\n") ;
        out += sFrom + _T("\n") +
               sTo + _T("\n") +
               sLength + _T("\n") +
               sType + _T("\n") +
               sOritentation + _T("\n") +
               sDescription ;
		TTreeItem *ni = new TTreeItem ( out , _T("ITEM") ) ;
		ni->data = a ;
        SetItemData ( y , ni ) ;
        }

    // Enzymes
    enzroot = AppendItem ( treeroot , txt("res_enzymes") ) ;
    for ( a = 0 ; a < p->vec->re.GetCount() ; a++ )
        {
        y = AppendItem ( enzroot , p->vec->re[a]->getName() ) ;
//        p->vec->re[a]->treeid = y ;
        bool used = false , visible = true ;
        wxString out = p->vec->re[a]->getName().c_str() ;
        out += _T("\n") ;
        for ( b = 0 ; !used && b < p->vec->rc.size() ; b++ )
           {
           if ( p->vec->rc[b].e == p->vec->re[a] )
              used = true ;
           }
        if ( !used ) Delete ( y ) ;
        else
           {
           SetItemData ( y , new TTreeItem ( out , _T("RE") , p->vec->re[a] ) ) ;
           visible = !p->vec->isEnzymeHidden ( p->vec->re[a]->getName() ) ;
           SetItemBold ( y , visible ) ;
           }
        }
    
    Thaw();
    EnsureVisible ( x ) ; // Making vector properties visible
    SelectItem ( vroot ) ;
    }
    
/** \brief Handles (selection) events
*/
void TVectorTree::OnEvent ( wxTreeEvent &event )
    {
    wxTreeItemId id = event.GetItem () ;
    TTreeItem *d = (TTreeItem*) GetItemData ( id ) ;
    if ( !textWindow ) return ;
    if ( !d ) 
        {
        textWindow->SetValue ( _T("") ) ;
        return ;
        }
    wxString out ;
    out = d->message.c_str() ;
    if ( d->type == _T("RE") )
        {
        TRestrictionEnzyme *e = (TRestrictionEnzyme*) d->p ;
        out += txt("res_seq") ;
        out += e->getSequence().c_str() ;
        out += _T("\n") ;
        out += txt("res_loc") ;
        out += e->location.c_str() ;
        out += _T("\n") ;
        out += txt("res_note") ;
        out += e->note.c_str() ;
        out += _T("\n") ;
        }
    else if ( d->type == _T("VECTOR") )
        {
        wxString dp = _T(" : ") ;
        TVector *v = (TVector*) d->p ;
        out += txt("name") + dp + v->getName() + _T("\n\n") ;
        out += txt("desc") + dp + _T("\n") + v->getDescription() ;
        }
    textWindow->SetValue ( out ) ;
    }

/** \brief Handles double-click events
*/
void TVectorTree::OnActivation ( wxTreeEvent &event )
    {
    wxTreeItemId id = event.GetItem () ;
    TTreeItem *d = (TTreeItem*) GetItemData ( id ) ;
    if ( !textWindow ) return ;
    if ( !d ) 
        {
        textWindow->SetValue ( _T("") ) ;
        return ;
        }
    if ( d->type == _T("ITEM") )
        {
        wxCommandEvent ev ;
        p->cPlasmid->setLastContextItem ( d->data ) ;
        p->cPlasmid->itemShowHide ( ev ) ;
        }
    else if ( d->type == _T("RE") )
        {
        TRestrictionEnzyme *e = (TRestrictionEnzyme*) d->p ;
        ToggleEnzymeVisibility ( e ) ;
        p->vec->updateDisplay() ;
        p->cPlasmid->Refresh () ;
        p->cSequence->arrange() ;
        p->cSequence->SilentRefresh() ;
        }
    }

/** \brief Toggles if an enzyme is displayed
*/
void TVectorTree::ToggleEnzymeVisibility ( TRestrictionEnzyme *e )
    {
    if ( !e ) return ;
    wxTreeItemId y ;
    wxTreeItemIdValue l ;
    y = GetFirstChild ( enzroot , l ) ;
    while ( y.IsOk() && GetItemText ( y ) != e->getName() )
       y = GetNextChild ( enzroot , l ) ;
    if ( !y.IsOk() ) // Automatically added enzyme
    	{
        p->vec->hideEnzyme ( e->getName() , true ) ;
        }    
    else // Manually added enzyme
    	{
        p->vec->hideEnzyme ( e->getName() , IsBold ( y ) ) ;
        SetItemBold ( y , !IsBold ( y ) ) ;
    	}    
    
    p->cPlasmid->Refresh() ;
    p->cSequence->arrange() ;
    p->cSequence->SilentRefresh() ;
    }

/** \brief Handles right mouse button clicks (context menu)
*/
void TVectorTree::OnRightClick ( wxTreeEvent &event )
    {
    wxTreeItemId id = event.GetItem () ;
    if ( !id.IsOk() ) return ;
    TTreeItem *d = (TTreeItem*) GetItemData ( id ) ;
    if ( !d ) return ;
    wxPoint pt = event.GetPoint() ;
    if ( d->type == _T("ITEM") )
        {
        wxMenu *cm = p->cPlasmid->invokeItemPopup ( d->data , pt , true ) ;
        p->cPlasmid->setLastContextItem ( d->data ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;
        }
    else if ( d->type == _T("RE") )
        {
        TRestrictionEnzyme *e = (TRestrictionEnzyme*) d->p ;
        int a ;
        for ( a = 0 ; a < p->vec->rc.size() && p->vec->rc[a].e != e ; a++ ) ;
        if ( a == p->vec->rc.size() ) return ;
        wxMenu *cm = p->cPlasmid->invokeRsPopup ( a , pt , true ) ;
        PopupMenu ( cm , pt ) ;
        delete cm ;        
        }
    else if ( d->type == _T("VECTOR") )
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

void TVectorTree::copyAA ( wxCommandEvent &ev )
    { p->cPlasmid->itemCopyAA ( ev ) ; }

void TVectorTree::AAasNewSequence ( wxCommandEvent &ev )
    { p->cPlasmid->itemAsNewAA ( ev ) ; }

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

void TVectorTree::itemCopyDNA ( wxCommandEvent &ev )
    { p->cPlasmid->itemCopyDNA ( ev ) ; }
