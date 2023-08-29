/** \file
	\brief Contains the ExternalInterface, EIpanel and EILB class members
*/
#include "ExternalInterface.h"


BEGIN_EVENT_TABLE(ExternalInterface, MyChildBase)
    EVT_CLOSE(ChildBase::OnClose)
    EVT_SET_FOCUS(ChildBase::OnFocus)

    // Dummies
    EVT_MENU(MDI_TOGGLE_FEATURES,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_RESTRICTION,ChildBase::OnDummy)
    EVT_MENU(MDI_TOGGLE_IDNA,ChildBase::OnDummy)
    EVT_MENU(MDI_VIEW_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_ORFS,ChildBase::OnDummy)
    EVT_MENU(MDI_CIRCULAR_LINEAR,ChildBase::OnDummy)
    EVT_MENU(MDI_UNDO,ChildBase::OnDummy)
    EVT_MENU(MDI_CUT,ChildBase::OnDummy)
    EVT_MENU(MDI_COPY,ChildBase::OnDummy)
    EVT_MENU(MDI_PASTE,ChildBase::OnDummy)
    EVT_MENU(MDI_EDIT_MODE,ChildBase::OnDummy)
    EVT_MENU(MDI_EXPORT,ChildBase::OnDummy)
    EVT_MENU(MDI_MARK_ALL,ChildBase::OnDummy)
    EVT_MENU(MDI_FILE_SAVE,ChildBase::OnDummy)
    EVT_MENU(MDI_FIND,ChildBase::OnDummy)
    EVT_MENU(AA_NONE,TABIviewer::OnDummy)
    EVT_MENU(AA_KNOWN, TABIviewer::OnDummy)
    EVT_MENU(AA_ALL, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE, TABIviewer::OnDummy)
    EVT_MENU(AA_ONE, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_3, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M1, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M2, TABIviewer::OnDummy)
    EVT_MENU(AA_THREE_M3, TABIviewer::OnDummy)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(EIpanel, wxPanel)
    EVT_BUTTON(ID_B1, EIpanel::OnB1)
    EVT_BUTTON(ID_B2, EIpanel::OnB2)
    EVT_BUTTON(ID_B3, EIpanel::OnB3)
    EVT_BUTTON(ID_B_LAST, EIpanel::OnBlast)
    EVT_BUTTON(ID_B_NEXT, EIpanel::OnBnext)
    EVT_TEXT_ENTER(ID_T1, EIpanel::OnB1)
    EVT_TEXT_ENTER(ID_T2, EIpanel::OnB1)
    EVT_TEXT_ENTER(ID_T3, EIpanel::OnB1)
    EVT_TEXT_ENTER(ID_T4, EIpanel::OnB1)
    EVT_CHOICE(ID_C1, EIpanel::OnC1)
    EVT_LISTBOX_DCLICK(ID_HLB, EIpanel::OnLboxDClick)
END_EVENT_TABLE()


ExternalInterface::ExternalInterface(wxWindow *parent, const wxString& title)
    : ChildBase(parent, title)
    {
    def = _T("EXTERNAL") ;
    }

ExternalInterface::~ExternalInterface ()
    {
    }

void ExternalInterface::initme ()
    {
    // Menus
    wxMenu *file_menu = myapp()->frame->getFileMenu () ;
    wxMenu *tool_menu = myapp()->frame->getToolMenu () ;
    wxMenu *help_menu = myapp()->frame->getHelpMenu () ;

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, txt("m_file") );
    menu_bar->Append(tool_menu, txt("m_tools") );
    menu_bar->Append(help_menu, txt("m_help") );

    SetMenuBar(menu_bar);

    nb = new wxNotebook ( this , -1 ) ;

    EIpanel *panel = new EIpanel ( nb , EI_NCBI ) ;
    nb->AddPage ( panel  , _T("NCBI") ) ; // Default panel

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;

    if ( !myapp()->frame->mainToolBar )
       {
       myapp()->frame->setDummyToolbar ( this ) ;
       myapp()->frame->addDefaultTools ( toolbar ) ;
       toolbar->Realize() ;
       v0->Add ( toolbar , 0 , wxEXPAND , 2 ) ;
       }
    v0->Add ( nb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

    myapp()->frame->setChild ( this ) ;
    Maximize () ;
    Activate () ;
    if ( panel->t1 ) panel->t1->SetFocus () ;
    }

wxString ExternalInterface::getName ()
    {
    return _T("External Interface") ;
    }

void ExternalInterface::runBlast ( wxString seq , wxString prg )
{
    EIpanel *bl = new EIpanel ( nb , EI_BLAST ) ;
    prg.MakeUpper() ;
    nb->AddPage ( bl , prg ) ;
    if ( prg == _T("BLASTP") ) bl->c1->SetSelection ( 0 ) ;
    if ( prg == _T("BLASTN") ) bl->c1->SetSelection ( 1 ) ;
    bl->t1->SetValue ( seq ) ;
    nb->SetSelection ( nb->GetPageCount()-1 ) ;
    wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, ID_B1 );
    wxPostEvent ( bl , event ) ;
}


// *****************************************************************************
EILB::EILB ( wxWindow *parent , int id )
	: wxHtmlListBox ( parent , id , wxDefaultPosition , wxDefaultSize , wxLB_MULTIPLE )
	{
	SetItemCount ( 0 ) ;
	}

wxString EILB::OnGetItem(size_t n) const
	{
	    if ( n >= was.GetCount() ) return _T("") ;
	wxString ret = was[n] ;
	wxString bgcolor ;
	if ( n % 3 == 0 ) bgcolor = _T("#FFFFFF") ;
	if ( n % 3 == 1 ) bgcolor = _T("#EEEEEE") ;
	if ( n % 3 == 2 ) bgcolor = _T("#DDDDDD") ;
	ret = _T("<table bgcolor='") + bgcolor + _T("' width='100%' cellspacing=0 cellpadding=0><tr><td>") + ret + _T("</td></tr></table>") ;
	return ret ;
	}

void EILB::Clear ()
	{
	DeselectAll () ;
	wxHtmlListBox::Clear () ;
	SetItemCount ( 0 ) ;
	was.Clear () ;
	data.Clear () ;
	}

void EILB::Set ( int id , wxString s , wxString t )
	{
	while ( was.GetCount() <= id ) was.Add ( _T("") ) ;
	while ( data.GetCount() <= id ) data.Add ( _T("") ) ;
	was[id] = s ;
	data[id] = t ;
	}

void EILB::Sort()
	{
	int a ;
	for ( a = 1 ; a < was.GetCount() ; a++ )
		{
		if ( was[a-1] <= was[a] ) continue ;
		wxString tmp ;
		tmp = was[a] ; was[a] = was[a-1] ; was[a-1] = tmp ;
		tmp = data[a] ; data[a] = data[a-1] ; data[a-1] = tmp ;
		a = 0 ;
		}
	}

void EILB::Update()
	{
	SetItemCount ( was.GetCount() ) ;
	Refresh () ;
	}

// *****************************************************************************

EIpanel::EIpanel ( wxWindow *parent , int _mode )
    : wxPanel ( parent )
{
    mode = _mode ;
    t1 = NULL ;

    blast_thread = NULL ;

    up = new wxPanel ( this ) ;
    hlb = new EILB ( this , ID_HLB ) ;

    if ( mode == EI_NCBI ) init_ncbi() ;
    if ( mode == EI_BLAST ) init_blast() ;

    v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( up , 0 , wxEXPAND , 5 ) ;
    v0->Add ( hlb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
	if ( t1 ) t1->SetFocus() ;
}


wxString EIpanel::valFC ( TiXmlNode *n )
	{
	if ( !n ) return _T("") ;
	if ( !n->FirstChild() ) return _T("") ;
	return val ( n->FirstChild() ) ;
	}

wxString EIpanel::val ( TiXmlNode *n )
	{
	if ( !n ) return _T("") ;
	if ( !n->Value() ) return _T("") ;
	return wxString ( n->Value() , wxConvUTF8 ) ;
	}


void EIpanel::OnLboxDClick ( wxCommandEvent& event )
	{
	OnB2 ( event ) ;
	}

void EIpanel::OnC1 ( wxCommandEvent& event )
	{
	if ( mode == EI_NCBI )
		{
		wxString database = c1->GetStringSelection() ;
		database = database.Lower() ;
		if ( database == _T("pubmed") )
			{
            v1->Show ( h1 , true ) ;
			}
		else
  			{
            v1->Show ( h1 , false ) ;
     		}
   		v0->Layout() ;
		}
	}

void EIpanel::OnB1 ( wxCommandEvent& event )
	{
	res_start = 0 ;
	b_next->Disable () ;
	b_last->Disable () ;
	process () ;
 	}

void EIpanel::OnB2 ( wxCommandEvent& event )
	{
	wxBeginBusyCursor () ;
	if ( mode == EI_NCBI ) execute_ncbi() ;
	if ( mode == EI_BLAST ) execute_blast() ;
	wxEndBusyCursor () ;
	}

void EIpanel::OnB3 ( wxCommandEvent& event )
	{
	wxBeginBusyCursor () ;
	if ( mode == EI_NCBI ) execute_ncbi_b3() ;
	if ( mode == EI_BLAST ) execute_blast_b3() ;
	wxEndBusyCursor () ;
	}

void EIpanel::OnBlast ( wxCommandEvent& event )
	{
	if ( res_start == 0 ) return ;
	res_start -= RETMAX ;
	process () ;
	}

void EIpanel::OnBnext ( wxCommandEvent& event )
	{
	if ( res_start + RETMAX > res_count ) return ;
	res_start += RETMAX ;
	process () ;
	}

void EIpanel::process ()
	{
    mylog ( "EIpanel::process" , "1" ) ;
	wxBeginBusyCursor () ;
	hlb->Clear () ;
    mylog ( "EIpanel::process" , "2" ) ;
	if ( mode == EI_NCBI ) process_ncbi() ;
	if ( mode == EI_BLAST ) process_blast() ;
    mylog ( "EIpanel::process" , "3" ) ;
	hlb->Update () ;
    mylog ( "EIpanel::process" , "4" ) ;
	wxEndBusyCursor () ;
	}

wxString EIpanel::num2html ( int num , int digits )
	{
	wxString s = wxString::Format ( _T("%d") , num ) ;
	while ( s.length() < digits ) s = _T(" ") + s ;
	s.Replace ( _T(" ") , _T("&nbsp;") ) ;
	return s ;
	}

void EIpanel::showMessage ( wxString msg )
	{
	if ( !st_msg->IsShown() )
		{
		v1->Show ( st_msg , true ) ;
		v0->Layout () ;
		}
	st_msg->SetLabel ( msg ) ;
	}

