#ifndef _TMAINTREE_H_
#define _TMAINTREE_H_

#include "main.h"

class MyFrame ;
class ChildBase ;

class TMainTreeItem : public wxTreeItemData
    {
    public :
    TMainTreeItem () : wxTreeItemData() {} ;
    TMainTreeItem ( ChildBase *_c ) : wxTreeItemData() { c = _c ; } ;
    ChildBase *c ;
    } ;

class TMainTree : public wxTreeCtrl
    {
    public :
    TMainTree ( wxSashLayoutWindow *parent , int i ) ;

    void initme () ;
    void addChild ( ChildBase *c , int type = TYPE_VECTOR ) ;
    void removeChild ( ChildBase *c ) ;

    void OnEvent ( wxTreeEvent &event ) ;
    void OnRightClick ( wxTreeEvent &event ) ;
    
    void OnCloseChild ( wxCommandEvent &event ) ;

    // Variables
    wxTreeItemId treeroot , vectors , primers , fragments , sequences , 
                    alignments , amino_acids , misc ;
                    
    private :
    ChildBase *lastChild ;
    
    DECLARE_EVENT_TABLE()
    } ;

#endif

