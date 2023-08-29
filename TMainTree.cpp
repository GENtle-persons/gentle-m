#include "TMainTree.h"

BEGIN_EVENT_TABLE(TMainTree, wxTreeCtrl)
    EVT_TREE_ITEM_RIGHT_CLICK(MAIN_TREE_DUMMY,TMainTree::OnRightClick)
    EVT_TREE_SEL_CHANGED(MAIN_TREE_DUMMY,TMainTree::OnEvent)
    EVT_MENU(MT_CLOSE,TMainTree::OnCloseChild)
END_EVENT_TABLE()

/** \brief Constructor
*/
TMainTree::TMainTree ( wxSashLayoutWindow *parent , int i )
    :wxTreeCtrl ( parent , i )
    {
    lastChild = NULL ;
    }

/** \brief Initialization of the tree; removal of old items, if any
*/
void TMainTree::initme()
    {
    // Basic stuff
    DeleteAllItems () ;
    treeroot = AddRoot ( myapp()->frame->project.name ) ;

    vectors = AppendItem ( treeroot , txt("vectors") ) ;
    fragments = AppendItem ( treeroot , txt("fragments") ) ;
    sequences = AppendItem ( treeroot , txt("sequences") ) ;
    primers = AppendItem ( treeroot , txt("primers") ) ;
    alignments = AppendItem ( treeroot , txt("alignments") ) ;
    amino_acids = AppendItem ( treeroot , txt("amino_acids") ) ;
    misc = AppendItem ( treeroot , txt("misc_stuff") ) ;

    SetFont ( *MYFONT ( MYFONTSIZE , wxFONTFAMILY_SWISS , wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL ) ) ;

    EnsureVisible ( vectors ) ;
    }

/** \brief Handles right click on an item (context menu)
*/
void TMainTree::OnRightClick ( wxTreeEvent &event )
    {
    wxPoint pt = event.GetPoint() ;
    wxTreeItemId id = event.GetItem () ;
    if ( !id.IsOk() ) return ;
    TMainTreeItem *d = (TMainTreeItem*) GetItemData ( id ) ;
    if ( !d || !d->c ) return ;

    lastChild = (ChildBase*) d->c ;

    wxMenu *cm = new wxMenu ;
    cm->Append ( MT_CLOSE , txt("m_close") ) ;
    PopupMenu ( cm , pt ) ;
    delete cm ;
    }

/** \brief Adds a child to the tree
	\param c Pointer to child
	\param type Where to add it
*/
void TMainTree::addChild ( ChildBase *c , int type )
    {
    wxTreeItemId theroot ;
    if ( type == TYPE_VECTOR ) theroot = vectors ;
    else if ( type == TYPE_FRAGMENT ) theroot = fragments ;
    else if ( type == TYPE_SEQUENCE ) theroot = sequences ;
    else if ( type == TYPE_PRIMER ) theroot = primers ;
    else if ( type == TYPE_ALIGNMENT ) theroot = alignments ;
    else if ( type == TYPE_AMINO_ACIDS ) theroot = amino_acids ;
    else if ( type == TYPE_MISC ) theroot = misc ;
    c->inMainTree = AppendItem ( theroot , c->getName() , -1 ) ;
    SetItemData ( c->inMainTree , new TMainTreeItem ( c ) ) ;
    EnsureVisible ( c->inMainTree ) ;
//    SelectItem ( c->inMainTree ) ;
    }

/** \brief Remove child from the tree
*/
void TMainTree::removeChild ( ChildBase *c )
    {
    if ( !c || !c->inMainTree.IsOk() ) return ;
    Delete ( c->inMainTree ) ;
    }

/** \brief Event handler (when an item gets the focus, via keyboard or mouse)
*/
void TMainTree::OnEvent ( wxTreeEvent &event )
    {
    if ( myapp()->frame->isActivating() ) return ;
    if ( myapp()->frame->isLocked() ) return ;
    wxTreeItemId id = event.GetItem () ;
    if ( !id.IsOk() ) return ;
    TMainTreeItem *d = (TMainTreeItem*) GetItemData ( id ) ;
    if ( !d ) return ;
    if ( !d->c ) return ;
    d->c->Activate () ;
    }

/** \brief Closes the current child
*/
void TMainTree::OnCloseChild ( wxCommandEvent &event )
    {
    if ( !lastChild ) return ;
    if ( lastChild->Close ( FALSE ) ) lastChild = NULL ;
//    else wxBell();
    }
