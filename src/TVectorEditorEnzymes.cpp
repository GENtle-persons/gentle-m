/** \file
    \brief Contains the TVectorEditor class methods concerning enzymes
*/
#include "TVectorEditor.h"

void TVectorEditor::initPanEnzym ()
    {
    wxPrintf( "D: TVectorEditor::initPanEnzym - start\n" ) ;
    nb->AddPage ( panEnzym , txt("t_vec_enzym") ) ;
    if ( v ) initPanEnzym2 () ;

//  int w , h ;
//  GetMyClientSize ( &w , &h , panEnzym ) ;

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
            //wxDefaultSize , 0 , NULL , wxLB_EXTENDED|wxLB_SORT ) ;
            wxDefaultSize , 0 , NULL , wxLB_EXTENDED) ;

    // List of Groups
    listGroups = new wxListBox ( panEnzym , TVE_LB_GR , wxDefaultPosition ,
            //wxDefaultSize , 0 , NULL , wxLB_SINGLE|wxLB_SORT);
            wxDefaultSize , 0 , NULL , wxLB_SINGLE);

    // List of enzymes in group
    listGE = new wxListBox ( panEnzym , TVE_LB_GE , wxDefaultPosition ,
            //wxDefaultSize , 0 , NULL , wxLB_EXTENDED|wxLB_SORT ) ;
            wxDefaultSize , 0 , NULL , wxLB_EXTENDED) ;

    wxButton *b1 = NULL , *b2 , *b3 , *b4 , *b_asng , *b_addgr , *b_import_rebase ;
    b_addgr = new wxButton ( panEnzym , TVE_EN_ADD_GR , txt("<-- add") ) ;
    b_atg = new wxButton ( panEnzym , TVE_EN_ADD_TO_GR , txt("b_add_to_group") ) ;
    b_asng = new wxButton ( panEnzym , TVE_EN_ADD_TO_NEW_GR , txt("b_add_as_new_group") ) ;
    b_dg = new wxButton ( panEnzym , TVE_EN_DEL_GR , txt("b_del_group") ) ;

// The button to import CLONE enzymes should stay deactivated permanently
//    if ( !v ) b1 = new wxButton ( panEnzym , TVE_IMPORT , txt("import_clone_enzymes") ) ;

    b2 = new wxButton ( panEnzym , TVE_NEW_ENZYME , txt("add_new_enzyme") ) ;
    delete_enzyme_button = new wxButton ( panEnzym , TVE_DEL_ENZYME , txt("b_del_enzyme") ) ;
    b3 = new wxButton ( panEnzym , TVE_EN_ADD_EN , txt("<-- add") ) ;
    b4 = new wxButton ( panEnzym , TVE_EN_DEL_EN , txt("del -->") ) ;
    b_dfg = new wxButton ( panEnzym , TVE_DEL_FROM_GROUP , txt("b_del_from_group") ) ;
    b_import_rebase = new wxButton ( panEnzym , TVE_EN_IMPORT_REBASE , txt("b_import_rebase") ) ;


    v1->Add ( tCE , 0 , wxEXPAND , 5 ) ;
    v1->Add ( listCE , 1 , wxEXPAND , 5 ) ;

    v2->Add ( new wxStaticText ( panEnzym , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b_addgr , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b_atg , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b_asng , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b_dg , 0 , wxEXPAND , 5 ) ;
    if ( b1 ) v2->Add ( b1 , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b2 , 0 , wxEXPAND , 5 ) ;
    v2->Add ( delete_enzyme_button , 0 , wxEXPAND , 5 ) ;
    v2->Add ( new wxStaticText ( panEnzym , -1 , "" ) , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b3 , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b4 , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b_dfg , 0 , wxEXPAND , 5 ) ;
    v2->Add ( b_import_rebase , 0 , wxEXPAND , 5 ) ;

    v3->Add ( tGR , 0 , wxEXPAND , 5 ) ;
    v3->Add ( listGroups , 1 , wxEXPAND , 5 ) ;
    v3->Add ( tGE , 0 , wxEXPAND , 5 ) ;
    v3->Add ( listGE , 3 , wxEXPAND , 5 ) ;

    h1->Add ( v1 , 1 , wxEXPAND , 5 ) ;
    h1->Add ( v2 , 0 , wxEXPAND , 5 ) ;
    h1->Add ( v3 , 1 , wxEXPAND , 5 ) ;

    panEnzym->SetSizer ( h1 ) ;
    h1->Fit ( panEnzym ) ;

    listCE->Clear() ;
    listGroups->Clear() ;
    showEnzymeGroups () ;

    wxPrintf( "I: Before clearing ce, ce contains:\n" ) ;
    debugStdout("ce") ;
    for ( int a = 0 ; a < TVectorEditor::ce.GetCount() ; a++ )
        {
        wxString s = TVectorEditor::ce[a] ;
        wxPrintf( "D: TVectorEditor::initPanEnzym - Adding '%s' from ce list to widget\n" , s ) ;
        listCE->Append ( s ) ;
        }

    //TVectorEditor::ce.Clear() ; // not loosing previously used settings in "current" list
    for ( int a = 0 ; v && a < v->re.GetCount() ; a++ )
        {
        wxString s = v->re[a]->getName() ;
        if ( v->re[a]->getSequence().IsEmpty() )
            {
            wxPrintf( "E: TVectorEditor::initPanEnzym - enzyme %s = re[%d]->getSequence.IsEmpty()\n" , v->re[a]->getName(), a ) ;
            abort() ;
            }
        int foundInCE=0 ;
        for(int a = 0; a < TVectorEditor::ce.GetCount() ; a++ )
            {
            if (s == ce[a] )
                {
                foundInCE = 1 ;
                }
            }
        if ( ! foundInCE )
            {
            listCE->Append ( s ) ;
            TVectorEditor::ce.Add ( s ) ;
            }
        }

    debugStdout("ce") ;

    wxPrintf( "D: TVectorEditor::initPanEnzym - end\n" ) ;
    }

/** \brief prints state of TVectorEditor to stdout
 * @param whatToPrint - blank-separated list indicating what to print, only ce at the moment
 */
void TVectorEditor::debugStdout(const wxString &whatToPrint ) const
    {
    wxPrintf( "D: TVectorEditor::debugStdout - ce: " );
    for( int a = 0 ; a < TVectorEditor::ce.GetCount() ; a++ )
        {
        wxPrintf( " %s" , TVectorEditor::ce[a] ) ;
        }
    wxPrintf( "\n" );
    }

// Handlers "enzyme"

void TVectorEditor::enzymeSelChange ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeSelChange - start\n" ) ;
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGroups )
        {
        showGroupEnzymes ( lb->GetString ( lb->GetSelection() ) ) ;
        }
    wxPrintf( "D: TVectorEditor::enzymeSelChange - end\n" ) ;
    }

void TVectorEditor::enzymeListDlbClick ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeListDlbClick - start\n" ) ;
    wxListBox *lb = (wxListBox*) ev.GetEventObject() ;
    if ( lb == listGE )
        {
        wxArrayInt vi ;
        int n = lb->GetSelections ( vi ) ;
        for ( int k = 0 ; k < n ; k++ )
            {
            wxString s = lb->GetString ( vi[k] ) ;
            TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( s ) ;
            TEnzymeDialog ed ( this , s , wxPoint(-1,-1) , wxSize(600,400) , wxDEFAULT_DIALOG_STYLE|wxCENTRE ) ;
            ed.initme ( e ) ;
            if ( ed.ShowModal() == wxID_OK )
               {
               bool addit = false ;
               ed.e->setName ( e->getName() ) ; // No name change!
               if ( e->differ ( *ed.e ) )
                  {
                  int x = wxMessageBox ( txt("t_enzyme_list_double_click") , txt("msg_box") , wxYES_NO|wxCANCEL ) ;
                  if ( x == wxYES )
                      {
                      *e = *ed.e ;
                      myapp()->frame->LS->updateRestrictionEnzyme ( e ) ;
                      }
                  if ( x == wxNO ) addit = true ;
                  }
               else addit = true ;
               if ( addit )
                  {
                  wxCommandEvent ev ;
                  enzymeAddEn ( ev ) ;
                  }
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
        if ( e->getSequence().IsEmpty() )
            {
            wxPrintf( "W: TVectorEditor::enzymeListDlbClick: myapp()->frame->LS->getRestrictionEnzyme (%s) retrieved empty sequence.\n" , s ) ;
            }
        TEnzymeDialog ed ( this , s , wxPoint(-1,-1) , wxSize(600,400) , wxDEFAULT_DIALOG_STYLE|wxCENTRE ) ;
        ed.initme ( e ) ;
        ed.ShowModal() ;
        }
    wxPrintf( "D: TVectorEditor::enzymeListDlbClick - end\n" ) ;
    }

void TVectorEditor::showEnzymeGroups ()
    {
    wxPrintf( "D: TVectorEditor::showEnzymeGroups - start\n" ) ;
    wxString all = txt("All") ;
    listGroups->Clear() ;
    listGroups->Append ( all ) ;
    wxArrayString vs ;
    myapp()->frame->LS->getEnzymeGroups ( vs ) ;
    for ( int i = 0 ; i < vs.GetCount() ; i++ )
        {
        wxPrintf( "D: TVectorEditor::showEnzymeGroups - appending '%s'\n" , vs[i] ) ;
        listGroups->Append ( vs[i] ) ;
        }
    showGroupEnzymes ( all ) ;
    listGroups->SetStringSelection ( all ) ;
    wxPrintf( "D: TVectorEditor::showEnzymeGroups - end\n" ) ;
    }

void TVectorEditor::showGroupEnzymes ( const wxString& gr )
    {
    wxPrintf( "D: TVectorEditor::showGroupEnzymes (%s) - start\n" , gr ) ;
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
        delete_enzyme_button->Enable() ;
        }
    else
        {
        b_atg->Enable() ;
        b_dg->Enable() ;
        b_dfg->Enable() ;
        delete_enzyme_button->Disable() ;
        }
    wxPrintf( "D: TVectorEditor::showGroupEnzymes (%s -> %s) - end\n" , gr , gr2 ) ;
    }

void TVectorEditor::enzymeAddEn ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeAddEn - start\n" ) ;
    wxArrayInt vi ;
    int n = listGE->GetSelections ( vi ) ;
    for ( int k = 0 ; k < n ; k++ )
        {
        int i = vi[k] ;
        wxString s = listGE->GetString ( i ) ;
        listGE->Deselect ( i ) ;
        int p = listCE->FindString ( s ) ;
        if ( p == wxNOT_FOUND )
            {
            listCE->Append ( s ) ;
            TVectorEditor::ce.Add ( s ) ;
            }
        }
    wxPrintf( "D: TVectorEditor::enzymeAddEn - end\n" ) ;
    }

void TVectorEditor::enzymeAddGr ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeAddGr - start\n" ) ;
    wxString s ;
    for ( int a = 0 ; a < eig.GetCount() ; a++ )
        {
        s = eig[a] ;
        int b = listCE->FindString ( s ) ;
        if ( b == wxNOT_FOUND )
           {
           listCE->Append ( s ) ;
           TVectorEditor::ce.Add ( s ) ;
           }
        }
    wxPrintf( "D: TVectorEditor::enzymeAddGr - end\n" ) ;
    }

void TVectorEditor::enzymeAddToGr ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeAddToGr - start\n" ) ;
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;

    wxArrayInt vi ;
    int n = listCE->GetSelections ( vi ) ;

    if ( n == 0 ) // Nothing selected, so select all
        {
        for ( n = 0 ; n < listCE->GetCount() ; n++ )
            {
            listCE->SetSelection ( n , TRUE ) ;
            }
        n = listCE->GetSelections ( vi ) ;
        }

    for ( int k = n-1 ; k >= 0 ; k-- )
        {
        int i = vi[k] ;
        wxString s = listCE->GetString(i) ;
        myapp()->frame->LS->addEnzymeToGroup ( s , group ) ;
        }
    showGroupEnzymes ( group ) ;
    wxPrintf( "D: TVectorEditor::enzymeAddToGr - end\n" ) ;
    }

void TVectorEditor::enzymeAddToNewGr ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeAddToNewGr - start\n" ) ;
    wxTextEntryDialog ted ( this , txt("b_add_as_new_group") , txt("t_new_enzyme_group_name") ) ;
    if ( wxID_OK != ted.ShowModal () ) return ;
    wxString ng = ted.GetValue() ;
    ng = myapp()->frame->LS->UCfirst ( ng ) ;
    if ( !myapp()->frame->LS->addEnzymeGroup ( ng ) )
        {
        wxPrintf( "W: TVectorEditor::enzymeAddToNewGr: Failure to add enzyme group '%s'\n" , ng ) ;
        return ;
        }

    listGroups->Append ( ng ) ;
    listGroups->SetStringSelection ( ng ) ;
    enzymeAddToGr ( ev ) ;
    wxPrintf( "D: TVectorEditor::enzymeAddToNewGr - end\n" ) ;
    }

void TVectorEditor::enzymeReallyDeleteEnzyme ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeReallyDeleteEnzyme - start\n" ) ;
    wxArrayInt vi ;
    int n = listGE->GetSelections ( vi ) ;
    if ( n == 0 ) return ;
    if ( wxYES != wxMessageBox ( txt("t_warn_delete_enzymes") , txt("msg_box") , wxYES_NO ) ) return ;
    for ( int a = 0 ; a < n ; a++ )
        {
        wxString name = listGE->GetString ( vi[a] ) ;
        myapp()->frame->LS->markEnzymeForDeletion ( name ) ;
        }
    wxMessageBox ( txt("t_enzymes_marked_for_deletion") ) ;
    wxPrintf( "D: TVectorEditor::enzymeReallyDeleteEnzyme - end\n" ) ;
    }

void TVectorEditor::enzymeDelEn ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::enzymeDelEn - start\n" ) ;
    wxArrayInt vi ;
    int n = listCE->GetSelections ( vi ) ;
    for ( int k = n-1 ; k >= 0 ; k-- )
        {
        int i = vi[k] ;
        wxString s = listCE->GetString ( i ) ;
        listCE->Delete ( i ) ;

        int numDeleted = 0 ;
        for ( int j = 0 ; j < TVectorEditor::ce.GetCount() ; j++ )
            {
            if ( TVectorEditor::ce[i] == s )
               {
               TVectorEditor::ce.RemoveAt ( j ) ;
               numDeleted++ ;
               /*
               TVectorEditor::ce[i] = TVectorEditor::ce[TVectorEditor::ce.size()-1] ;
               TVectorEditor::ce.pop_back () ;
               */
               }
            }
        if (0 == numDeleted)
            {
            wxPrintf( "W: TVectorEditor::enzymeDelEn: Did not delete from CE - expected '%s'\n" , s ) ;
            }
        else if (1 < numDeleted )
            {
            wxPrintf( "W: TVectorEditor::enzymeDelEn: Deleted '%s' multiple (%d) times from CE\n" , s , numDeleted ) ;
            }
        }

    wxPrintf( "D: TVectorEditor::enzymeDelEn - end\n" ) ;
    }

void TVectorEditor::initialViewEnzyme ( const wxString& e )
    {
    wxPrintf( "D: TVectorEditor::initialViewEnzyme(%s) - start\n" , e ) ;
    nb->SetSelection ( 2 ) ;
    listCE->SetStringSelection ( e ) ;
    wxPrintf( "D: TVectorEditor::initialViewEnzyme(%s) - end\n" , e ) ;
    }

void TVectorEditor::importCloneEnzymes ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::importCloneEnzymes - start\n" ) ;
    TStorage TS ( TEMP_STORAGE ) ;
    TClone clone ;
    clone.loadEnzymeList ( &TS , "./CLONE.ENZ" ) ;
    wxPrintf(" I: TVectorEditor::importCloneEnzymes: Loaded %lu CEs\n" , TS.re.GetCount() ) ;
    for ( int a = 0 ; a < TS.re.GetCount() ; a++ )
        {
        TRestrictionEnzyme *rea = TS.re[a] ;
        if ( rea->getSequence().empty() )
            {
            wxPrintf( "W: TVectorEditor::importCloneEnzymes - Enzyme '%s' has empty sequence - abort.\n" , rea->getName() ) ;
            abort() ;
            }
        listCE->Append ( rea->getName() ) ;
        myapp()->frame->LS->re.Add ( rea ) ;
        myapp()->frame->LS->updateRestrictionEnzyme ( rea ) ;
        TS.re[a] = NULL ; // avoid deletion of re[a]
        }
    wxString group = listGroups->GetStringSelection() ;
    showGroupEnzymes ( group ) ;
    wxPrintf( "D: TVectorEditor::importCloneEnzymes - end\n" ) ;
    }


void TVectorEditor::newEnzyme ( wxCommandEvent &ev )
    {
    wxPrintf( "D: TVectorEditor::newEnzyme - start\n" ) ;
    TRestrictionEnzyme *e = new TRestrictionEnzyme ;
    e->setCut ( 0 ) ;
    e->setOverlap ( 0 ) ;
    TEnzymeDialog ed ( this , txt("t_new_enzyme") , wxPoint(-1,-1) , wxSize(600,400) , wxDEFAULT_DIALOG_STYLE|wxCENTRE ) ;
    ed.initme ( e ) ;
    if ( ed.ShowModal() == wxID_OK )
        {
        if ( ed.e->getSequence().IsEmpty() )
            {
            wxPrintf( "D: TVectorEditor::newEnzyme - Not accepting new enzyme since sequence is empty.\n" ) ;
            }
        else
            {
            myapp()->frame->LS->re.Add ( ed.e ) ;
            myapp()->frame->LS->updateRestrictionEnzyme ( ed.e ) ;
            }
        }
    showEnzymeGroups () ;
    wxPrintf( "D: TVectorEditor::newEnzyme - end\n" ) ;
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

void TVectorEditor::enzymeImportRebase ( wxCommandEvent &ev )
    {
    if ( wxYES != wxMessageBox ( txt("t_import_rebase_warning") , txt("t_attention") , wxYES_NO ) )
        return ;
    wxString filename = wxFileName::CreateTempFileName ( "GENtle_rebase_" ) ;
    wxPrintf("D: TVectorEditor::enzymeImportRebase created temp filename '%s'.\n",filename ) ;

    // Read file
    wxBeginBusyCursor () ;

    myExternal ex ;
    wxString version = ex.getText ( "ftp://ftp.neb.com/pub/rebase/VERSION" ) ;
    version.Trim(true) ;
    wxPrintf( "D: Found rebase version '%s'.\n" , version ) ;
    wxString text = ex.getText ( "ftp://ftp.neb.com/pub/rebase/bairoch." + version ) ;
    wxPrintf( "D: Downloaded text of length %ld.\n" , text.Len() ) ;

    // Success?
    if ( text.Len() < 100000 )
        {
        wxEndBusyCursor () ;
        wxMessageBox ( txt("t_download_failed") ) ;
        return ;
        }

    wxArrayString lines ;
    explode ( "\n" , text , lines ) ;
    text.Clear() ; // Free memory

    unsigned int changed = 0 , added = 0 , last_enzyme_count = 0 ;
    wxString tag , enzyme_name , last_enzyme ;
    for ( unsigned int a = 0 ; a < lines.GetCount() ; a++ )
        {
        wxString text = lines[a] ;
        wxString tag = text.Mid ( 0 , 2 ) ;
        text = text.Mid(2).Trim().Trim(false) ;
        if ( tag == "ID" )
            {
            enzyme_name = text ;
            wxPrintf("D: enzyme name: '%s'\n", enzyme_name ) ;
            if ( last_enzyme == enzyme_name )
                {
                last_enzyme_count++ ;
                enzyme_name += wxString::Format ( " [%d]" , last_enzyme_count ) ;
                }
            else
                {
                last_enzyme = enzyme_name ;
                last_enzyme_count = 0 ;
                }
            }
        else if ( tag == "//" ) enzyme_name.Clear () ;
        else if ( tag == "RS" )
            {
            wxString p1 = text.BeforeFirst(';') ;
            wxString p2 = text.AfterFirst(';') ;
            p2.Replace(";","") ;
            wxString seq1 = p1.BeforeFirst(',').Trim().Trim(false) ;
            wxString seq2 = p2.BeforeFirst(',').Trim().Trim(false) ;
            wxString cut1 = p1.AfterFirst(',').Trim().Trim(false) ;
            wxString cut2 = p2.AfterFirst(',').Trim().Trim(false) ;

            if ( enzyme_name.IsEmpty() )
                {
                wxPrintf( "W: Could not read name prior to sequence '%s' - ignoring.\n" , seq1 ) ;
                continue ; // Paranoia
                }
            if ( seq1.IsEmpty() || cut1.IsEmpty() )
                {
                continue ; // Paranoia
                }
            if ( !seq2.IsEmpty() && seq1.length() != seq2.length() ) continue ; // Paranoia
            if ( enzyme_name.StartsWith ( "M." ) ) continue ; // Strange things, ignore
            if ( cut1 == "?" ) continue ; // Unknown cut, ignore

            signed long lcut1 , lcut2 , ol1 = seq1.length() ;
            cut1.ToLong ( &lcut1 ) ;
            cut2.ToLong ( &lcut2 ) ;

            if ( seq2.IsEmpty() ) lcut2 = lcut1 ; // No overlap given, creating one in VectorNTI count

            while ( lcut1 > 0 && seq1.length() < lcut1 ) seq1 += "N" ;
            lcut2 = ol1 - lcut2 - lcut1 ;
            while ( lcut1 + lcut2 > 0 && seq1.length() < lcut1 + lcut2 ) seq1 += "N" ;
            while ( lcut1 < 0 )
                {
                lcut1++ ;
                // lcut2 is now relative to lcut1, so it stays unchanged
                seq1 = "N" + seq1 ;
                }

            

            TRestrictionEnzyme *re = myapp()->frame->LS->getRestrictionEnzyme ( enzyme_name ) ;
            if ( re == NULL ) // Add new enzyme
                {
                unsigned long b , c = myapp()->frame->LS->re.GetCount() ;
                for ( b = 0 ; b < c ; b++ )
                    {
                    if ( myapp()->frame->LS->re[b]->getSequence() == seq1 &&
                         myapp()->frame->LS->re[b]->getCut() == lcut1 &&
                         myapp()->frame->LS->re[b]->getOverlap() == lcut2 )
                         {
                         break ;
                         }
                    }
                if ( b < c )
                    {
                    continue ; // Already an enzyme with these properties, no need to add it under another name again
                    }

                // A real new enzyme!
                re = new TRestrictionEnzyme ;
                re->setName ( enzyme_name ) ;
                re->setCut ( lcut1 ) ;
                re->setOverlap ( lcut2 ) ;
                re->setSequence ( seq1 ) ;
                if ( re->getSequence().IsEmpty() )
                    {
                    wxPrintf( "W: TVectorEditor::enzymeImportRebase: Not adding new enzyme '%s' with empty sequence\n" , re->getName() ) ;
                    delete re ;
                    }
                else
                    {
                    added++ ;
                    myapp()->frame->LS->addRestrictionEnzyme ( re ) ;
                    myapp()->frame->LS->updateRestrictionEnzyme ( re ) ;
                    }
                //wxMessageBox ( enzyme_name , "New enzyme!" ) ;
                }
            else if ( re->getCut() != lcut1 || re->getOverlap() != lcut2 || re->getSequence() != seq1 ) // Update enzyme
                {
                changed++ ;
                re->setCut ( lcut1 ) ;
                re->setOverlap ( lcut2 ) ;
                re->setSequence ( seq1 ) ;
                if ( re->getSequence().IsEmpty() )
                    {
                    wxPrintf( "W: TVectorEditor::enzymeImportRebase: Not updating enzyme '%s' with empty sequence\n" , re->getName() ) ;
                    }
                else
                    {
                    myapp()->frame->LS->updateRestrictionEnzyme ( re ) ;
                    }
                //wxMessageBox ( wxString::Format ( "%d:%d ; %d:%d / " , lcut1 , lcut2 , re->getCut() , re->getOverlap() ) + seq1 + ":" + re->getSequence() , enzyme_name ) ;
                }
            }
        }
    wxEndBusyCursor () ;

    if ( changed + added != 0 )
        {
        wxMessageBox ( wxString::Format ( txt("t_import_rebase_updates") , added , changed ) ) ;
        exit ( 1 ) ; // Hard exit
        }
    else
        {
        wxMessageBox ( txt("t_import_rebase_no_updates") ) ;
        }
    }

void TVectorEditor::enzymeDelFromGr ( wxCommandEvent &ev )
    {
    wxString group = listGroups->GetStringSelection() ;
    group = myapp()->frame->LS->UCfirst ( group ) ;
    if ( group == txt("All") ) return ;

    wxArrayInt vi ;
    int n = listGE->GetSelections ( vi ) ;
    for ( int k = n-1 ; k >= 0 ; k-- )
        {
        int i = vi[k] ;
        wxString s = listGE->GetString(i) ;
        myapp()->frame->LS->removeEnzymeFromGroup ( s , group ) ;
        }
    showGroupEnzymes ( group ) ;
    }

