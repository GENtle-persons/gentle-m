#include "TStoreAllDialog.h"

BEGIN_EVENT_TABLE(TStoreAllDialog, wxDialog )
    EVT_BUTTON(wxID_OK,TStoreAllDialog::OnOK)
    EVT_BUTTON(SAD_ALL,TStoreAllDialog::OnAll)
    EVT_BUTTON(SAD_NONE,TStoreAllDialog::OnNone)
    EVT_BUTTON(SAD_INVERT,TStoreAllDialog::OnInvert)
//    EVT_CHECKLISTBOX(SAD_LIST,TStoreAllDialog::OnListChange)
END_EVENT_TABLE()

TStoreAllDialog::TStoreAllDialog ( wxWindow *_parent , const wxString& title )
    : wxDialog ( _parent , -1 , title )
{
   wxSize size ( 400 , 300 ) ;
   SetSize ( size ) ;

   wxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
   wxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
   wxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
   wxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
   wxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
   
   list = new wxCheckListBox ( this , SAD_LIST ) ;
   database = new wxChoice ( this , -1 ) ;

   wxButton *b_all = new wxButton ( this , SAD_ALL , txt("b_all") ) ;
   wxButton *b_none = new wxButton ( this , SAD_NONE , txt("b_none") ) ;
   wxButton *b_invert = new wxButton ( this , SAD_INVERT , txt("b_invert") ) ;
   wxButton *b_ok = new wxButton ( this , wxID_OK , txt("b_ok") ) ;
   wxButton *b_cancel = new wxButton ( this , wxID_CANCEL , txt("b_cancel") ) ;
   
   force_db = new wxCheckBox ( this , -1 , txt("t_sad_force_database") ) ;
   auto_overwrite = new wxCheckBox ( this , -1 , txt("t_sad_auto_overwrite") ) ;

   h0->Add ( b_all , 1 , wxEXPAND|wxALL , 2 ) ;
   h0->Add ( b_none , 1 , wxEXPAND|wxALL , 2 ) ;
   h0->Add ( b_invert , 1 , wxEXPAND|wxALL , 2 ) ;
   
   h1->Add ( b_ok , 1 , wxEXPAND|wxALL , 2 ) ;
   h1->Add ( b_cancel , 1 , wxEXPAND|wxALL , 2 ) ;
   
   h2->Add ( auto_overwrite , 0 , wxEXPAND|wxALL , 2 ) ;   
   h2->Add ( force_db , 0 , wxEXPAND|wxALL , 2 ) ;

   h3->Add ( new wxStaticText ( this , -1 , _T("t_default_database") ) , 0 , wxEXPAND|wxALL , 2 ) ;
   h3->Add ( database , 1 , wxEXPAND|wxALL , 2 ) ;
   
   v0->Add ( list , 1 , wxEXPAND|wxALL , 2 ) ;
   v0->Add ( h3 , 0 , wxEXPAND|wxALL , 2 ) ;
   v0->Add ( h2 , 0 , wxEXPAND|wxALL , 2 ) ;
   v0->Add ( h0 , 0 , wxEXPAND|wxALL , 2 ) ;
   v0->Add ( h1 , 0 , wxEXPAND|wxALL , 2 ) ;
   
   PopulateList() ;
   database->SetStringSelection ( defdb ) ;
   auto_overwrite->SetValue ( true ) ;
   b_ok->SetDefault() ;

   SetSizer ( v0 ) ;
   v0->Fit ( this ) ;
}

TStoreAllDialog::~TStoreAllDialog ()
{
}

void TStoreAllDialog::PopulateList ()
{
     unsigned int a ;
     children.clear() ;
     for ( a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
     {
         wxString def = myapp()->frame->children[a]->def ;
         if ( def != _T("ABIviewer") && 
              def != _T("dna") && 
              def != _T("alignment") && 
              def != _T("AminoAcids") ) continue ;
         if ( !myapp()->frame->children[a]->vec ) continue ;
         
         int pos = list->Append ( myapp()->frame->children[a]->getName() ) ;
         children.push_back ( myapp()->frame->children[a] ) ;
         if ( def == _T("dna") && !myapp()->frame->children[a]->HasUndoData() ) continue ;
         list->Check ( pos ) ;
     }

    defdb = myapp()->frame->LS->getDatabaseList ( db_name , db_file ) ;
    database->Clear () ;
    for ( a = 0 ; a < db_name.GetCount() ; a++ )
        database->Append ( db_name[a] ) ;
}

void TStoreAllDialog::OnOK ( wxCommandEvent &ev )
{
     unsigned int a ;
     wxString sel_db = database->GetStringSelection() ;
     for ( a = 0 ; a < list->GetCount() ; a++ )
     {
         if ( !list->IsChecked ( a ) ) continue ; // Entry not checked
         
         wxString name , db ;
         name = children[a]->getName() ;
         if ( children[a]->vec && !children[a]->vec->getDatabase().IsEmpty() ) db = children[a]->vec->getDatabase() ;
         else db = sel_db ;
         if ( force_db->GetValue() ) db = sel_db ;

         TManageDatabaseDialog mdd ( children[a] , _T("") , ACTION_MODE_SAVE|ACTION_MODE_STARTUP , children[a]->vec ) ;
         mdd.silent_overwrite = auto_overwrite->GetValue() ;
         mdd.pm_name->SetValue ( name ) ;
         mdd.pm_dd_save->SetStringSelection ( db ) ;
         mdd.do_save_DNA () ;
     }
     
     EndModal ( wxID_OK ) ; //wxDialog::OnOK ( ev ) ;
}

void TStoreAllDialog::OnAll ( wxCommandEvent &ev )
{
     unsigned int a ;
     for ( a = 0 ; a < list->GetCount() ; a++ )
         list->Check ( a , true ) ;
}

void TStoreAllDialog::OnNone ( wxCommandEvent &ev )
{
     unsigned int a ;
     for ( a = 0 ; a < list->GetCount() ; a++ )
         list->Check ( a , false ) ;
}

void TStoreAllDialog::OnInvert ( wxCommandEvent &ev )
{
     unsigned int a ;
     for ( a = 0 ; a < list->GetCount() ; a++ )
         list->Check ( a , !list->IsChecked(a) ) ;
}

void TStoreAllDialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) EndModal ( wxID_CANCEL ) ; //OnCancel ( ev ) ;
    else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }
