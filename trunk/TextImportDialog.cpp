#include "TextImportDialog.h"

BEGIN_EVENT_TABLE(TextImportDialog, wxDialog )
    EVT_BUTTON(TID_OK,TextImportDialog::OnOK)
    EVT_BUTTON(TID_CANCEL,TextImportDialog::OnCancel)
    EVT_CHAR_HOOK(TextImportDialog::OnCharHook)
END_EVENT_TABLE()


TextImportDialog::TextImportDialog(wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
    {
    int w , h ;
    int bo = 5 , lh = 22 ;
    GetClientSize ( &w , &h ) ;
    
    wxStaticText *s1 = new wxStaticText ( this , -1 , txt("title") , wxPoint ( bo , bo+2 ) ) ;
    wxRect r = s1->GetRect() ;
    
    name = new wxTextCtrl ( this , -1 , "" ,
                                wxPoint ( r.GetRight() + bo , bo ) , 
                                wxSize ( w * 2 / 3 - r.GetRight() - bo , lh ) ) ;
    type = new wxChoice ( this , -1 ,
                                wxPoint ( w * 2 / 3 + bo , bo ) ,
                                wxSize ( w / 4 , lh ) ) ;
    sequence = new wxTextCtrl ( this , -1 , "" ,
                                wxPoint ( bo , bo*2 + lh ) , 
                                wxSize ( w - bo*2 , h - bo*4 - lh*2 ) ,
                                wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2 ) ;
                                
    wxButton *OK = new wxButton ( this , TID_OK , txt("b_ok") ,
                                wxPoint ( bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;
    new wxButton ( this , TID_CANCEL , txt("b_cancel") ,
                                wxPoint ( w*3/4 - bo , h - lh - bo ) ,
                                wxSize ( w/4 , lh ) ) ;

    type->Append ( txt("dna") ) ;
    type->Append ( txt("amino_acid") ) ;
    type->Append ( txt("t_genbank_format") ) ;
    type->Append ( txt("t_xml") ) ;
    type->Append ( txt("t_primer") ) ;
    type->SetSelection ( 0 ) ;

    Center () ;
    OK->SetDefault () ;
    name->SetFocus() ;
    }

void TextImportDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

void TextImportDialog::OnOK ( wxCommandEvent &ev )
    {
    sName = name->GetValue () ;
    sSequence = "" ;
    wxString s = sequence->GetValue() ;
    for ( int a = 0 ; a < s.length() ; a++ )
        {
        if ( s.GetChar(a) >= 'a' && s.GetChar(a) <= 'z' ) sSequence += s.GetChar(a)-'a'+'A' ;
        else if ( s.GetChar(a) >= 'A' && s.GetChar(a) <= 'Z' ) sSequence += s.GetChar(a) ;
        }
    wxDialog::OnOK ( ev ) ;
    }

void TextImportDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnCancel ( ev ) ;
    }


