/** \file
	\brief Contains the methods of the TItemEditDialog class
*/
#include "TItemEditDialog.h"

BEGIN_EVENT_TABLE(TItemEditDialog, wxDialog )
    EVT_BUTTON(IED_COLOR,TItemEditDialog::OnColor)
    EVT_BUTTON(IED_OK,TItemEditDialog::OnOK)
    EVT_BUTTON(IED_CANCEL,TItemEditDialog::OnCancel)
    EVT_CHECKBOX(IED_USEOFFSET,TItemEditDialog::OnUseOffset)
    EVT_CHAR_HOOK(TItemEditDialog::OnCharHook)
    EVT_LISTBOX_DCLICK(IED_LIST,TItemEditDialog::OnList)
END_EVENT_TABLE()

TItemEditDialog::TItemEditDialog ( wxWindow *parent, const wxString& title ,
                        TVectorItem &_vi )
   : wxDialog ( parent , -1 , title )
   {
   myapp()->frame->push_help ( _T("GENtle:Sequence_editor#Features") ) ;
   vi = new TVectorItem ;
   *vi = _vi ;
   
   // Color button
   wxButton *item_color_button = new wxButton ( this , IED_COLOR , txt("b_item_color") ) ;

   // Styles
   wxString rb_ch[6] ;
   rb_ch[0] = txt("t_seq_style_default" ) ;
   rb_ch[1] = txt("t_seq_style_alpha" ) ;
   rb_ch[2] = txt("t_seq_style_beta" ) ;
   rb_ch[3] = txt("t_seq_style_mut" ) ;
   rb_ch[4] = txt("t_seq_style_noline" ) ;
   rb_ch[5] = txt("t_seq_style_vline" ) ;
   rb = new wxRadioBox ( this , -1 , txt("t_seq_style") ,
                           wxDefaultPosition ,
                           wxDefaultSize ,
                           6 ,
                           rb_ch , 3 ,
                           wxRA_SPECIFY_COLS 
                           ) ;
   rb->SetSelection ( atoi ( vi->getParam ( _T("SEQUENCE_STYLE") ).mb_str() ) ) ;

   // Offset
   int off = vi->getOffset() ;
   wxString off_t = (off==-1) ? _T("") : wxString::Format(_T("%d"),off) ;

   useOffset = new wxCheckBox ( this , IED_USEOFFSET , txt("t_use_item_offset") ) ;
   useOffset->SetValue ( (off!=-1) ) ;

   offset = new wxTextCtrl ( this , -1 , off_t ) ;
   if ( off == -1 ) offset->Disable() ;
   
   // Properties listbox
   lb = new wxListBox ( this , IED_LIST , 
                           wxDefaultPosition , 
                           wxSize ( 300 , -1 ) ,
                           0 , NULL , wxLB_SORT|wxLB_HSCROLL|wxLB_ALWAYS_SB ) ;
   initlb () ;

   wxButton *button_OK = new wxButton ( this , IED_OK , txt("b_ok") ) ;
   wxButton *button_cancel = new wxButton ( this , IED_CANCEL , txt("b_cancel") ) ;
   
   wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
   wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
   wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
   
   h0->Add ( item_color_button , 0 , wxEXPAND ) ;
   h0->Add ( new wxStaticText ( this , -1 , _T("   ") ) , 0 , wxEXPAND ) ;
   h0->Add ( useOffset , 0 , wxEXPAND ) ;
   h0->Add ( offset , 0 , wxEXPAND ) ;

#ifdef __WXMAC__
   h1->Add ( button_cancel , 0 , wxEXPAND ) ;
   h1->Add ( new wxStaticText ( this , -1 , _T("         ") ) , 0 , wxEXPAND ) ;
   h1->Add ( button_OK , 0 , wxEXPAND ) ;
#else
   h1->Add ( button_OK , 0 , wxEXPAND ) ;
   h1->Add ( new wxStaticText ( this , -1 , _T("         ") ) , 0 , wxEXPAND ) ;
   h1->Add ( button_cancel , 0 , wxEXPAND ) ;
#endif
   
   v0->Add ( rb , 0 , wxEXPAND|wxALL , 5 ) ;
   v0->Add ( h0 , 0 , wxEXPAND|wxALL , 5 ) ;
   v0->Add ( lb , 1 , wxEXPAND|wxALL , 5 ) ;
   v0->Add ( h1 , 0 , wxCENTER|wxALL , 5 ) ;
      
   button_OK->SetDefault() ;
   button_OK->SetFocus() ;
   
   this->SetSizer ( v0 ) ;
   v0->Fit ( this ) ;
   Center() ;
   }
   
TItemEditDialog::~TItemEditDialog ()
    {
    myapp()->frame->pop_help () ;
    delete vi ;
    }
    
// Handlers

void TItemEditDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }

void TItemEditDialog::OnOK ( wxCommandEvent &ev )
    {
    vi->setParam ( _T("SEQUENCE_STYLE") , rb->GetSelection() ) ;
    vi->setParam ( _T("CHANGED") , _T("YES") ) ;
    if ( useOffset->IsChecked() ) vi->setOffset ( atoi(offset->GetValue().mb_str()) ) ;
    else vi->setOffset ( -1 ) ;
    EndModal ( wxID_OK ) ; // wxDialog::OnOK(ev) ;
    }
    
void TItemEditDialog::OnCancel ( wxCommandEvent &ev )
    {
    EndModal ( wxID_CANCEL ) ; // wxDialog::OnCancel(ev) ;
    }

void TItemEditDialog::OnColor ( wxCommandEvent &ev )
    {
    wxColour col ;
    wxBrush *b = vi->getBrush() ;
    col = b->GetColour() ;
    col = wxGetColourFromUser ( this , col ) ;
    if ( !col.Ok() ) return ;
    vi->setColor ( col ) ;
    }

void TItemEditDialog::OnUseOffset ( wxCommandEvent &ev )
    {
    if ( useOffset->IsChecked() ) offset->Enable() ;
    else offset->Disable() ;
    }
    
void TItemEditDialog::initlb ()
    {
    lb->Clear () ;
    wxArrayString vs = vi->getParamKeys () ;
    for ( int a = 0 ; a < vs.GetCount() ; a++ )
        {
        wxString line = vs[a] + _T(" : ") + vi->getParam ( vs[a] ) ;
        if ( line.Left ( 1 ) == _T("/") )
           lb->Append ( line.Mid ( 1 ) ) ;
        }    
    }    
    
void TItemEditDialog::OnList ( wxCommandEvent &ev )
    {
    wxString sel = lb->GetStringSelection () ;
    wxString key = sel.BeforeFirst ( ':' ) ;
    wxString val = sel.AfterFirst ( ':' ) ;
    key = key . Trim() . Trim ( false ) ;
    val = val . Trim() . Trim ( false ) ;
    wxTextEntryDialog ted ( this , _T("1") , key , val ) ;
    if ( ted.ShowModal () != wxID_OK ) return ;
    vi->setParam ( _T("/") + key , ted.GetValue() ) ;
    initlb () ;
    }
    
