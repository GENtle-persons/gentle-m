/** \file
	\brief Contains the TMainTree and TMainTreeItem classes
*/

#ifndef _TMAINTREE_H_
#define _TMAINTREE_H_

#include "main.h"

class MyFrame ;
class ChildBase ;

/** \class TMainTreeItem
	\brief Item in TMainTree
*/
class TMainTreeItem : public wxTreeItemData
    {
    public :
    TMainTreeItem () : wxTreeItemData() {} ; ///< \brief Blank constructor
    TMainTreeItem ( ChildBase *_c ) : wxTreeItemData() { c = _c ; } ; ///< \brief Constructor
    ChildBase *c ; ///< \brief Pointer to the associated child
    } ;

/** \class TMainTree
	\brief The main tree class
*/
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
    ChildBase *lastChild ; ///< \brief Pointer to the last selected child
    
    DECLARE_EVENT_TABLE()
    } ;

#endif

