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
   rb->SetSelection ( atoi ( vi->getParam ( _T("SEQUENCE_STYLE") ).mb_str() ) ) ;

   int off = vi->getOffset() ;
   wxString off_t = (off==-1) ? _T("") : wxString::Format(_T("%d"),off) ;
   wxRect r = rb->GetRect() ;
   useOffset = new wxCheckBox ( this , IED_USEOFFSET , txt("t_use_item_offset") ,
                                 wxPoint ( r.GetLeft() , r.GetBottom() + bo ) ) ;
   useOffset->SetValue ( (off!=-1) ) ;
   r = useOffset->GetRect() ;
   offset = new wxTextCtrl ( this , -1 , off_t ,
                                 wxPoint ( r.GetRight()+bo , r.GetTop() ) ) ;
   r = offset->GetRect() ;
   if ( off == -1 ) offset->Disable() ;
   
   lb = new wxListBox ( this , IED_LIST , 
                           wxPoint ( bo , r.GetBottom() + bo ) , 
                           wxSize ( w - bo*2 , h - r.GetBottom() - th - bo*2 ) ,
                           0 , NULL , wxLB_SORT|wxLB_HSCROLL|wxLB_ALWAYS_SB ) ;
   initlb () ;

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
    vi->setParam ( _T("SEQUENCE_STYLE") , rb->GetSelection() ) ;
    vi->setParam ( _T("CHANGED") , _T("YES") ) ;
    if ( useOffset->IsChecked() ) vi->setOffset ( atoi(offset->GetValue().mb_str()) ) ;
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
    
