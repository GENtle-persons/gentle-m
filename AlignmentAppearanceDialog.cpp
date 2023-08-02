#include "AlignmentAppearanceDialog.h"

BEGIN_EVENT_TABLE(AlignmentAppearanceDialog, wxDialog)
    EVT_BUTTON(ALIGN_APPEARANCE_LINE_COLOR_BUTTON1,AlignmentAppearanceDialog::OnLineColorButton)
    EVT_BUTTON(ALIGN_APPEARANCE_LINE_COLOR_BUTTON2,AlignmentAppearanceDialog::OnLineColorButton)
    EVT_BUTTON(ALIGN_APPEARANCE_LINE_COLOR_BUTTON3,AlignmentAppearanceDialog::OnLineColorButton)
    EVT_BUTTON(ALIGN_APPEARANCE_LINE_COLOR_BUTTON4,AlignmentAppearanceDialog::OnLineColorButton)
    EVT_BUTTON(ALIGN_APPEARANCE_LINE_COLOR_BUTTON5,AlignmentAppearanceDialog::OnLineColorButton)
    EVT_BUTTON(ALIGN_APPEARANCE_LINE_COLOR_BUTTON6,AlignmentAppearanceDialog::OnLineColorButton)
    EVT_BUTTON(ALIGN_APPEARANCE_TEXT_FOREGROUND,AlignmentAppearanceDialog::OnForegroundButton)
    EVT_BUTTON(ALIGN_APPEARANCE_TEXT_BACKGROUND,AlignmentAppearanceDialog::OnBackgroundButton)
    EVT_BUTTON(wxID_OK,AlignmentAppearanceDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL,AlignmentAppearanceDialog::OnCancel)
    EVT_BUTTON(ALIGN_APPEARANCE_RESET,AlignmentAppearanceDialog::OnReset)
    EVT_CHAR_HOOK(AlignmentAppearanceDialog::OnCharHook)
END_EVENT_TABLE()

void AlignmentAppearanceDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_F1 ) myapp()->frame->OnHelp(ev) ;
    else event.Skip() ;
    }

void AlignmentAppearanceDialog::addLine ( wxString name , wxArrayString &as , wxFlexGridSizer *sizer )
    {
    wxBoxSizer *line = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxButton *col = new wxButton ( this , ALIGN_APPEARANCE_LINE_COLOR_BUTTON1 + line_color_buttons , 
                        txt("t_alignment_appearance_line_color") ) ;
    wxRadioBox *line_rb = new wxRadioBox ( this , -1 , txt("t_alignment_appearance_line_style") , 
                                  wxDefaultPosition , wxDefaultSize , as ) ;
    wxSpinCtrl *spin = new wxSpinCtrl ( this , -1 , _T("") , wxDefaultPosition , wxSize ( 40 , -1 ) ) ;
    spin->SetRange ( 0 , 5 ) ;
    spin->SetValue ( 0 ) ;
    
    line->Add ( line_rb , 0 , wxALIGN_LEFT ) ;
    line->Add ( spin , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT , 5 ) ;
    line->Add ( new wxStaticText ( this , -1 , txt("t_alignment_appearance_px") ) , 
                       0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT , 5 ) ;
    line->Add ( col , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT , 5 ) ;
    
    sizer->Add ( new wxStaticText ( this , -1 , name ) , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    sizer->Add ( line , 0 , wxALIGN_LEFT ) ;
    
    line_color_buttons++ ;
    radioboxes.push_back ( line_rb ) ;
    thickness.push_back ( spin ) ;
    colors.push_back ( *wxBLACK ) ;
    
//    line_rb->SetSelection ( 1 ) ;
    spin->SetValue ( 1 ) ;
    }

AlignmentAppearanceDialog::~AlignmentAppearanceDialog ()
	{
	myapp()->frame->pop_help () ;
	}

AlignmentAppearanceDialog::AlignmentAppearanceDialog ( wxWindow *_parent , const wxString& title )
    : wxDialog ( _parent , -1 , title )
    {
	myapp()->frame->push_help ( _T("GENtle:Alignment_Appearance_Dialog") ) ;
    line_color_buttons = 0 ;
    wxFlexGridSizer *fgs = new wxFlexGridSizer ( 2 , 2 , 2 ) ;

    wxArrayString line_choices ;
    line_choices.Add ( txt("t_alignment_appearance_line_no_change") ) ;
    line_choices.Add ( txt("t_alignment_appearance_line_solid") ) ;
    line_choices.Add ( txt("t_alignment_appearance_line_dashed") ) ;
    line_choices.Add ( txt("t_alignment_appearance_line_dotted") ) ;
    
    addLine ( txt("t_alignment_appearance_line_top") , line_choices , fgs ) ;
    addLine ( txt("t_alignment_appearance_line_bottom") , line_choices , fgs ) ;
    addLine ( txt("t_alignment_appearance_line_left") , line_choices , fgs ) ;
    addLine ( txt("t_alignment_appearance_line_right") , line_choices , fgs ) ;
    addLine ( txt("t_alignment_appearance_line_inner_horiz") , line_choices , fgs ) ;
    addLine ( txt("t_alignment_appearance_line_inner_vert") , line_choices , fgs ) ;

    wxBoxSizer *line = new wxBoxSizer ( wxHORIZONTAL ) ;
    use_foreground = new wxCheckBox ( this , -1 , _T("") ) ;
    use_background = new wxCheckBox ( this , -1 , _T("") ) ;
    wxButton *foreground = new wxButton ( this , ALIGN_APPEARANCE_TEXT_FOREGROUND , txt("t_alignment_appearance_foreground") ) ;
    wxButton *background = new wxButton ( this , ALIGN_APPEARANCE_TEXT_BACKGROUND , txt("t_alignment_appearance_background") ) ;
    bold = new wxCheckBox ( this , -1 , txt("m_align_bold") ) ;
    italics = NULL ; //new wxCheckBox ( this , -1 , txt("m_align_italics") ) ;
    
    line->Add ( use_foreground , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    line->Add ( foreground , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    line->Add ( use_background , 0 , wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL , 25 ) ;
    line->Add ( background , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    line->Add ( bold , 0 , wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL , 5 ) ;
//    line->Add ( italics , 0 , wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL , 5 ) ;

    fgs->Add ( new wxStaticText ( this , -1 , txt("t_alignment_appearance_text") ) , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    fgs->Add ( line , 0 , wxALIGN_LEFT ) ;

    wxBoxSizer *line2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxButton *b_ok = new wxButton ( this , wxID_OK , txt("b_ok") ) ;
    wxButton *b_cancel = new wxButton ( this , wxID_CANCEL , txt("b_cancel") ) ;
    wxButton *b_reset = new wxButton ( this , ALIGN_APPEARANCE_RESET , txt("b_alignment_appearance_reset") ) ;

    line2->Add ( b_ok , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    line2->Add ( new wxStaticText ( this , -1 , _T("") ) , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT , 60 ) ;
    line2->Add ( b_reset , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    line2->Add ( new wxStaticText ( this , -1 , _T("") ) , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT , 60 ) ;
    line2->Add ( b_cancel , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;

    fgs->Add ( new wxStaticText ( this , -1 , _T("") ) , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    fgs->Add ( line2 , 0 , wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL ) ;
    
    SetSizer ( fgs ) ;
    fgs->Fit ( this ) ;
    }                                                                

void AlignmentAppearanceDialog::OnForegroundButton ( wxCommandEvent &event )
    {
    wxColour c = wxGetColourFromUser ( this , color_foreground ) ;
    if ( !c.Ok() ) return ;
    color_foreground = c ;
    use_foreground->SetValue ( true ) ;
    }

void AlignmentAppearanceDialog::OnBackgroundButton ( wxCommandEvent &event )
    {
    wxColour c = wxGetColourFromUser ( this , color_background ) ;
    if ( !c.Ok() ) return ;
    color_background = c ;
    use_background->SetValue ( true ) ;
    }

void AlignmentAppearanceDialog::OnLineColorButton ( wxCommandEvent &event )
    {
    int id = event.GetId() - ALIGN_APPEARANCE_LINE_COLOR_BUTTON1 ;
    wxColour c = wxGetColourFromUser ( this , colors[id] ) ;
    if ( !c.Ok() ) return ;
    colors[id] = c ;
    }

void AlignmentAppearanceDialog::set_pen ( SequenceCharMarkup &scm , int id , int border )
    {
    int b = radioboxes[id]->GetSelection() ;
    int t = thickness[id]->GetValue() ;
    if ( b == 0 ) return ;
    
    wxPen *p = NULL ;
    scm.borders |= border ;
    
    if ( border == wxTOP ) p = &scm.borderTop ;
    else if ( border == wxBOTTOM ) p = &scm.borderBottom ;
    else if ( border == wxLEFT ) p = &scm.borderLeft ;
    else if ( border == wxRIGHT ) p = &scm.borderRight ;
    
    switch ( b )
       {
       case 1 : *p = wxPen ( colors[id] , t , wxSOLID ) ; break ;
       case 2 : *p = wxPen ( colors[id] , t , wxSHORT_DASH ) ; break ;
       case 3 : *p = wxPen ( colors[id] , t , wxDOT ) ; break ;
       }
    }

void AlignmentAppearanceDialog::OnReset ( wxCommandEvent &event )
    {
    int l , x ;
    for ( l = firstline ; l <= lastline ; l++ )
        {
        while ( ali->lines[l].markup.size() <= to )
              ali->lines[l].markup.push_back ( SequenceCharMarkup() ) ;
        for ( x = from ; x <= to ; x++ )
            ali->lines[l].markup[x] = SequenceCharMarkup () ;
        while ( ali->lines[l].markup.size() > 0 && 
                ali->lines[l].markup[ali->lines[l].markup.size()-1].ignore )
              ali->lines[l].markup.pop_back() ;
        }
    EndModal ( wxID_OK ) ; //wxDialog::OnOK(event);
    }

void AlignmentAppearanceDialog::OnOK ( wxCommandEvent &event )
    {
    int l , x ;
    for ( l = firstline ; l <= lastline ; l++ )
        {
        for ( x = from ; x <= to ; x++ )
            {
            SequenceCharMarkup scm ;
            ali->getCharMarkup ( scm , l , x , 0 ) ;
            scm.ignore = false ;
            
            if ( use_foreground->GetValue() ) scm.textcolor = color_foreground ;
            if ( use_background->GetValue() ) scm.backcolor = color_background ;
            scm.bold = bold->GetValue() ;
            if ( italics ) scm.italics = italics->GetValue() ;
            
            set_pen ( scm , l==firstline ? 0 : 4 , wxTOP ) ;
            set_pen ( scm , l==lastline ? 1 : 4 , wxBOTTOM ) ;
            set_pen ( scm , x==from ? 2 : 5 , wxLEFT ) ;
            set_pen ( scm , x==to ? 3 : 5 , wxRIGHT ) ;
            
            while ( ali->lines[l].markup.size() <= x )
                  ali->lines[l].markup.push_back ( SequenceCharMarkup() ) ;
            ali->lines[l].markup[x] = scm ;
            }
        }
    EndModal ( wxID_OK ) ; //wxDialog::OnOK(event);
    }

void AlignmentAppearanceDialog::OnCancel ( wxCommandEvent &event )
    {
    EndModal ( wxID_CANCEL ) ; //wxDialog::OnCancel(event);
    }

void AlignmentAppearanceDialog::setup ( int _from , int _to , int _firstline , int _lastline , TAlignment *_ali )
    {
    ali = _ali ;
    from = _from ;
    to = _to ;
    firstline = _firstline ;
    lastline = _lastline ;
    }
