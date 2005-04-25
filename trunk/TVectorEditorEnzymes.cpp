/** \file
	\brief Contains the TVectorEditor class methods concerning enzymes
*/
#include "TVectorEditor.h"

void TVectorEditor::initPanEnzym ()
    {
    nb->AddPage ( panEnzym , txt("t_vec_enzym") ) ;
    if ( v ) initPanEnzym2 () ;
    int w , h ;
    GetMyClientSize ( &w , &h , panEnzym ) ;
    
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *v1 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *v2 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *v3 = new wxBoxSizer ( wxVERTICAL ) ;
    
    wxStaticText *tCE , *tGR , *tGE ;
    tCE = new wxStaticText ( panEnzym , -1 , txt("current_enzymes") ) ;
    tGR = new wxStaticText ( panEnzym , -1 , txt("enzyme_groups") ) ;
    tGE = new wxStaticText ( panEnzym , -1 , txt("enzymes_in_group") ) ;
    
    // List of current enzymes
    listCE = new wxListBox ( panEnzym , TVE_LB_CE , wxDefaultPosition , 
            wxSize ( w/3 , h - th ) , 0 , NULL , wxLB_EXTENDED|wxLB_SORT ) ;
            
    // List of Groups
    listGroups = new wxListBox ( panEnzym , TVE_LB_GR , wxDefaultPosition , 
            wxDefaultSize , 0 , NULL , wxLB_SINGLE|wxLB_SORT);
            
    // List of enzymes in group
    listGE = new wxListBox ( panEnzym , TVE_LB_GE , wxDefaultPosition , 
            wxDefaultSize , 0 , NULL , wxLB_EXTENDED|wxLB_SORT ) ;

    wxButton *b1 = NULL , *b2 , *b3 , *b4 , *b_asng , *b_addgr ;  
    b_addgr = new wxButton ( panEnzym , TVE_EN_ADD_GR , txt("<-- add") ) ;
    b_atg = new wxButton ( panEnzym , TVE_EN_ADD_TO_GR , txt("b_add_to_group") ) ;
    b_asng = new wxButton ( panEnzym , TVE_EN_ADD_TO_NEW_GR , txt("b_add_as_new_group") ) ;            
    b_dg = new wxButton ( panEnzym , TVE_EN_DEL_GR , txt("b_del_group") ) ;
    
// The button to import CLONE enzymes should stay deactivated permanently
//    if ( !v ) b1 = new wxButton ( panEnzym , TVE_IMPORT , txt("import_clone_enzymes") ) ;

    b2 = new wxButton ( panEnzym , TVE_NEW_ENZYME , txt("add_new_enzyme") ) ;
    b3 = new wxButton ( panEnzym , TVE_EN_ADD_EN , txt("<-- add") ) ;
    b4 = new wxButton ( panEnzym , TVE_EN_DEL_EN , txt("del -->") ) ;
    b_dfg = new wxButton ( panEnzym , TVE_DEL_FROM_GROUP , txt("b_del_from_group") ) ;


    v1->Add ( tCE , 0 , wxEXPAND , 5 ) ;
    v1->Add ( listCE , 1 , wxEXPAND , 5 ) ;

    v2->Add ( new wxStaticText ( panEnzym , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b_addgr , 0 , wxEXPAND|wxALL , 5 ) ;
    v2->Add ( b_atg , 0 , wxEXPAND|wxALL , 5 ) ;
    v2->Add ( b_asng , 0 , wxEXPAND|wxALL , 5 ) ;
    v2->Add ( b_dg , 0 , wxEXPAND|wxALL , 5 ) ;
    if ( b1 ) v2->Add ( b1 , 0 , wxEXPAND|wxALL , 5 ) ;
    v2->Add ( b2 , 0 , wxEXPAND|wxALL , 5 ) ;
    v2->Add ( new wxStaticText ( panEnzym , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b3 , 0 , wxEXPAND|wxALL , 5 ) ;
    v2->Add ( b4 , 0 , wxEXPAND|wxALL , 5 ) ;
    v2->Add ( b_dfg , 0 , wxEXPAND|wxALL , 5 ) ;
    
    v3->Add ( tGR , 0 , wxEXPAND , 5 ) ;
    v3->Add ( listGroups , 1 , wxEXPAND , 5 ) ;
    v3->Add ( tGE , 0 , wxEXPAND , 5 ) ;
    v3->Add ( listGE , 3 , wxEXPAND , 5 ) ;
    
    h1->Add ( v1 , 1 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( v2 , 0 , wxEXPAND , 5 ) ;
    h1->Add ( v3 , 1 , wxEXPAND|wxALL , 5 ) ;

    panEnzym->SetSizer ( h1 ) ;
    h1->Fit ( panEnzym ) ;

    listCE->Clear() ;
    listGroups->Clear() ;
    showEnzymeGroups () ;
    
    int a ;
    ce.Clear() ;
    for ( a = 0 ; v && a < v->re.GetCount() ; a++ )
        {
        wxString s = v->re[a]->name ;
        listCE->Append ( s ) ;
        ce.Add ( s ) ;
        }
    }

// Handlers "enzyme"

void TVectorEditor::enzymeSelChange ( wxCommandEvent &ev )
    {
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGroups )
        {
        showGroupEnzymes ( lb->GetString ( lb->GetSelection() ) ) ;
        }
    }
    
void TVectorEditor::enzymeListDlbClick ( wxCommandEvent &ev )
    {
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGE )
        {
        wxArrayInt vi ;
        int i , k , n = lb->GetSelections ( vi ) ;
        for ( k = 0 ; k < n ; k++ )
            {
            wxString s = lb->GetString ( vi[k] ) ;
            TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
            TEnzymeDialog ed ( this , s , wxPoint(-1,-1) , wxSize(600,400) , 
                            wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
            ed.initme ( e ) ;
            if ( ed.ShowModal() == wxID_OK )
               {
/*               ed.e->name = e->name ; // No name change!
               if ( e->differ ( *ed.e ) )
                  {
                  *e = *ed.e ;
                  myapp()->frame->LS->updateRestrictionEnzyme ( e ) ;
                  }*/
               wxCommandEvent ev ;
               enzymeAddEn ( ev ) ;
               }
            }
        }
    else if ( lb == listCE )
        {
        wxArrayInt vi ;
        int n = lb->GetSelections ( vi ) ;
        if ( n != 1 ) return ;
        wxString s = lb->GetString ( vi[0] ) ;
        TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
        TEnzymeDialog ed ( this , s , wxPoint(-1,-1) , wxSize(600,400) , 
                        wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
        ed.initme ( e ) ;
        ed.ShowModal() ;
        }
    }
    
void TVectorEditor::showEnzymeGroups ()
    {
    wxString all = txt("All") ;
    listGroups->Clear() ;
    listGroups->Append ( all ) ;
    wxArrayString vs ;
    myapp()->frame->LS->getEnzymeGroups ( vs ) ;
    for ( int i = 0 ; i < vs.GetCount() ; i++ )
        listGroups->Append ( vs[i] ) ;
    showGroupEnzymes ( all ) ;
    listGroups->SetStringSelection ( all ) ;
    }
    
void TVectorEditor::showGroupEnzymes ( wxString gr )
    {
    wxArrayString vs ;
    listGE->Clear() ;
    wxString gr2 = myapp()->frame->LS->UCfirst ( gr ) ;
    myapp()->frame->LS->getEnzymesInGroup ( gr2 , vs ) ;
    eig.Clear () ;
    for ( int i = 0 ; i < vs.GetCount() ; i++ )
        {
        listGE->Append ( vs[i] ) ;
        eig.Add ( vs[i] ) ;
        }
    if ( gr2 == txt("All") )
        {
        b_atg->Disable() ;
        b_dg->Disable() ;
        b_dfg->Disable() ;
        }
    else
        {
        b_atg->Enable() ;
        b_dg->Enable() ;
        b_dfg->Enable() ;
        }
    }

void TVectorEditor::enzymeAddEn ( wxCommandEvent &ev )
    {
    wxArrayInt vi ;
    int i , k , n = listGE->GetSelections ( vi ) ;
    for ( k = 0 ; k < n ; k++ )
        {
        i = vi[k] ;
        wxString s = listGE->GetString ( i ) ;
        listGE->Deselect ( i ) ;
        int p = listCE->FindString ( s ) ;
        if ( p == wxNOT_FOUND )
            {
            listCE->Append ( s ) ;
            ce.Add ( s ) ;
            }
        }
    }

void TVectorEditor::enzymeAddGr ( wxCommandEvent &ev )
    {
    int a , b ;
    wxString s ;
    for ( a = 0 ; a < eig.GetCount() ; a++ )
        {
        s = eig[a] ;
        b = listCE->FindString ( s ) ;
        if ( b == wxNOT_FOUND )
           {
           listCE->Append ( s ) ;
           ce.Add ( s ) ;
           }
        }
    }

void TVectorEditor::enzymeAddToGr ( wxCommandEvent &ev )
    {
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;
    wxArrayInt vi ;
    int i , k , n = listCE->GetSelections ( vi ) ;

    if ( n == 0 ) // Nothing selected, so select all
    	{
	    for ( n = 0 ; n < listCE->GetCount() ; n++ )
	    	listCE->SetSelection ( n , TRUE ) ;
    	n = listCE->GetSelections ( vi ) ;
    	}    

    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        wxString s = listCE->GetString(i) ;
        myapp()->frame->LS->addEnzymeToGroup ( s , group ) ;
        }
    showGroupEnzymes ( group ) ;
    }

void TVectorEditor::enzymeAddToNewGr ( wxCommandEvent &ev )
    {
    wxTextEntryDialog ted ( this , txt("b_add_as_new_group") ,
                                txt("t_new_enzyme_group_name") ) ;
    if ( wxID_OK != ted.ShowModal () ) return ;
    wxString ng = ted.GetValue() ;
    ng = myapp()->frame->LS->UCfirst ( ng ) ;
    if ( !myapp()->frame->LS->addEnzymeGroup ( ng ) ) return ;
/*
    wxArrayInt vi ;
    int n = listCE->GetSelections ( vi ) ;
    if ( n = 0 ) // Nothing selected, so select all
    	{
	    for ( n = 0 ; n < listCE->GetCount() ; n++ )
	    	listCE->SetSelection ( n , TRUE ) ;
    	}    
*/
    listGroups->Append ( ng ) ;
    listGroups->SetStringSelection ( ng ) ;
    enzymeAddToGr ( ev ) ;
    }

void TVectorEditor::enzymeDelEn ( wxCommandEvent &ev )
    {
    wxArrayInt vi ;
    int i , k , n = listCE->GetSelections ( vi ) ;
    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        wxString s = listCE->GetString ( i ) ;
        listCE->Delete ( i ) ;
        for ( i = 0 ; i < ce.GetCount() ; i++ )
            {
            if ( ce[i] == s )
               {
               ce.RemoveAt ( i ) ;
               /*
               ce[i] = ce[ce.size()-1] ;
               ce.pop_back () ;
               */
               }
            }
        }
    }
    
void TVectorEditor::initialViewEnzyme ( wxString e )
    {
    nb->SetSelection ( 2 ) ;
    listCE->SetStringSelection ( e ) ;
    }
    
void TVectorEditor::importCloneEnzymes ( wxCommandEvent &ev )
    {
    TStorage TS ( TEMP_STORAGE ) ;
    TClone clone ;
    clone.loadEnzymeList ( &TS , "./CLONE.ENZ" ) ;
    int a ;
    for ( a = 0 ; a < TS.re.GetCount() ; a++ )
       {
       listCE->Append ( TS.re[a]->name ) ;
       myapp()->frame->LS->re.Add ( TS.re[a] ) ;
       myapp()->frame->LS->updateRestrictionEnzyme ( TS.re[a] ) ;
       TS.re[a] = NULL ; // avoid deletion
       }
    wxString group = listGroups->GetStringSelection() ;
    showGroupEnzymes ( group ) ;
    }
    

void TVectorEditor::newEnzyme ( wxCommandEvent &ev )
    {
    TRestrictionEnzyme *e = new TRestrictionEnzyme ;
    e->cut = 0 ;
    e->overlap = 0 ;
    TEnzymeDialog ed ( this , txt("t_new_enzyme") , wxPoint(-1,-1) , wxSize(600,400) , 
                    wxDEFAULT_DIALOG_STYLE|wxCENTRE|wxDIALOG_MODAL ) ;
    ed.initme ( e ) ;
    if ( ed.ShowModal() == wxID_OK )
       {
       myapp()->frame->LS->re.Add ( ed.e ) ;
       myapp()->frame->LS->updateRestrictionEnzyme ( ed.e ) ;
       }
    showEnzymeGroups () ;
    }
    
void TVectorEditor::enzymeDelGr ( wxCommandEvent &ev )
    {
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;
    
    wxString s = wxString::Format ( txt("t_del_group") , group.c_str() ) ;
    if ( wxMessageBox ( s , txt("msg_box") , wxYES_NO|wxICON_QUESTION ) != wxYES ) return ;

    myapp()->frame->LS->removeEnzymeGroup ( group ) ;
    showEnzymeGroups () ;
    }

void TVectorEditor::enzymeDelFromGr ( wxCommandEvent &ev )
    {
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;
    
    wxArrayInt vi ;
    int i , k , n = listGE->GetSelections ( vi ) ;
    for ( k = n-1 ; k >= 0 ; k-- )
        {
        i = vi[k] ;
        wxString s = listGE->GetString(i) ;
        myapp()->frame->LS->removeEnzymeFromGroup ( s , group ) ;
        }
    showGroupEnzymes ( group ) ;
    }
    
