#include "TItemEditDialog.h"

BEGIN_EVENT_TABLE(TItemEditDialog, wxDialog )
    EVT_BUTTON(IED_COLOR,TItemEditDialog::OnColor)
    EVT_BUTTON(IED_OK,TItemEditDialog::OnOK)
    EVT_BUTTON(IED_CANCEL,TItemEditDialog::OnCancel)
    EVT_CHECKBOX(IED_USEOFFSET,TItemEditDialog::OnUseOffset)
    EVT_CHAR_HOOK(TItemEditDialog::OnCharHook)
END_EVENT_TABLE()

TItemEditDialog::TItemEditDialog ( wxWindow *parent, const wxString& title ,
                        TVectorItem &_vi )
   : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 400 , 400 ) )
   {
   vi = new TVectorItem ;
   *vi = _vi ;
   
   int th = 30 , bo = 5 ;
   int w , h ;
   GetClientSize ( &w , &h ) ;
   new wxButton ( this , IED_COLOR , txt("b_item_color") , wxPoint ( bo , bo ) ) ;

   wxString rb_ch[6] ;
   rb_ch[0] = txt("t_seq_style_default" ) ;
   rb_ch[1] = txt("t_seq_style_alpha" ) ;
   rb_ch[2] = txt("t_seq_style_beta" ) ;
   rb_ch[3] = txt("t_seq_style_mut" ) ;
   rb_ch[4] = txt("t_seq_style_noline" ) ;
   rb_ch[5] = txt("t_seq_style_vline" ) ;
   rb = new wxRadioBox ( this , -1 , txt("t_seq_style") ,
                           wxPoint ( bo , th + bo ) ,
                           wxDefaultSize ,
                           6 ,
                           rb_ch ,
                           wxRA_SPECIFY_COLS
                           ) ;
   rb->SetSelection ( atoi ( vi->getParam ( "SEQUENCE_STYLE" ).c_str() ) ) ;

   int off = vi->getOffset() ;
   wxString off_t = (off==-1) ? "" : wxString::Format("%d",off) ;
   wxRect r = rb->GetRect() ;
   useOffset = new wxCheckBox ( this , IED_USEOFFSET , txt("t_use_item_offset") ,
                                 wxPoint ( r.GetLeft() , r.GetBottom() + bo ) ) ;
   useOffset->SetValue ( (off!=-1) ) ;
   r = useOffset->GetRect() ;
   offset = new wxTextCtrl ( this , -1 , off_t ,
                                 wxPoint ( r.GetRight()+bo , r.GetTop() ) ) ;
   if ( off == -1 ) offset->Disable() ;

   wxButton *OK ;
   OK = new wxButton ( this , IED_OK , txt("b_ok") , wxPoint ( w/5 , h-th ) ) ;
   new wxButton ( this , IED_CANCEL , txt("b_cancel") , wxPoint ( w*3/5 , h-th ) ) ;
   
   OK->SetDefault() ;
   OK->SetFocus() ;
   }
   
TItemEditDialog::~TItemEditDialog ()
    {
    delete vi ;
    }
    
// Handlers

void TItemEditDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TItemEditDialog::OnOK ( wxCommandEvent &ev )
    {
    vi->setParam ( "SEQUENCE_STYLE" , rb->GetSelection() ) ;
    vi->setParam ( "CHANGED" , "YES" ) ;
    if ( useOffset->IsChecked() ) vi->setOffset ( atoi(offset->GetValue().c_str()) ) ;
    else vi->setOffset ( -1 ) ;
    wxDialog::OnOK(ev) ;
    }
    
void TItemEditDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel(ev) ;
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
    
