#include "TMainTree.h"

BEGIN_EVENT_TABLE(TMainTree, wxTreeCtrl)
    EVT_TREE_ITEM_RIGHT_CLICK(MAIN_TREE_DUMMY,TMainTree::OnRightClick)
    EVT_TREE_SEL_CHANGED(MAIN_TREE_DUMMY,TMainTree::OnEvent)
    EVT_MENU(MT_CLOSE,TMainTree::OnCloseChild)
END_EVENT_TABLE()


TMainTree::TMainTree ( wxSashLayoutWindow *parent , int i )
    :wxTreeCtrl ( parent , i )
    {
    lastChild = NULL ;
    }
    
void TMainTree::initme()
    {
    wxTreeItemId x , y ;
    
    // Basic stuff
    DeleteAllItems () ;
    treeroot = AddRoot ( myapp()->frame->project_name.c_str() ) ;

    vectors = AppendItem ( treeroot , txt("vectors") ) ;
    fragments = AppendItem ( treeroot , txt("fragments") ) ;
    sequences = AppendItem ( treeroot , txt("sequences") ) ;
    primers = AppendItem ( treeroot , txt("primers") ) ;
    alignments = AppendItem ( treeroot , txt("alignments") ) ;
    amino_acids = AppendItem ( treeroot , txt("amino_acids") ) ;
    misc = AppendItem ( treeroot , txt("misc_stuff") ) ;

    wxFont myFont ( 8 , wxSWISS , wxNORMAL , wxNORMAL ) ;
    SetFont ( myFont ) ;
    
    EnsureVisible ( vectors ) ;

    }

void TMainTree::OnRightClick ( wxTreeEvent &event )
    {
    wxPoint pt = event.GetPoint() ;
    wxTreeItemId id = event.GetItem () ;
    TMainTreeItem *d = (TMainTreeItem*) GetItemData ( id ) ;
    if ( !d || !d->c ) return ;
    
    lastChild = (ChildBase*) d->c ;
    
    wxMenu *cm = new wxMenu ;
    cm->Append ( MT_CLOSE , txt("m_close") ) ;
    PopupMenu ( cm , pt ) ;
    delete cm ;
    }
    
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
    c->inMainTree = AppendItem ( theroot , c->getName().c_str() , -1 ) ;
    SetItemData ( c->inMainTree , new TMainTreeItem ( c ) ) ;
    EnsureVisible ( c->inMainTree ) ;
    SelectItem ( c->inMainTree ) ;
    }

void TMainTree::removeChild ( ChildBase *c )
    {
    Delete ( c->inMainTree ) ;
    }
    
void TMainTree::OnEvent ( wxTreeEvent &event )
    {
    wxTreeItemId id = event.GetItem () ;
    TMainTreeItem *d = (TMainTreeItem*) GetItemData ( id ) ;
    if ( !d ) return ;
    d->c->Activate () ;
    d->c->Refresh () ;
    }
    
void TMainTree::OnCloseChild ( wxCommandEvent &event )
    {
    lastChild->Close ( TRUE ) ;
    }
