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
    def = "EXTERNAL" ;
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
    
    nb->AddPage ( new EIpanel ( nb , EI_NCBI ) , "NCBI" ) ;
    nb->AddPage ( new EIpanel ( nb , EI_BLAST ) , "BLAST" ) ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
//    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( nb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

    myapp()->frame->setChild ( this ) ;
    Maximize () ;
    Activate () ;
    }

wxString ExternalInterface::getName ()
    {
    return "External Interface" ;
    }
    

// *****************************************************************************
EILB::EILB ( wxWindow *parent , int id )
	: wxHtmlListBox ( parent , id , wxDefaultPosition , wxDefaultSize , wxLB_MULTIPLE )
	{
	SetItemCount ( 0 ) ;
	}    

wxString EILB::OnGetItem(size_t n) const
	{
	    if ( n >= was.GetCount() ) return "" ;
	wxString ret = was[n] ;
	wxString bgcolor ;
	if ( n % 3 == 0 ) bgcolor = "#FFFFFF" ;
	if ( n % 3 == 1 ) bgcolor = "#EEEEEE" ;
	if ( n % 3 == 2 ) bgcolor = "#DDDDDD" ;
	ret = "<table bgcolor='" + bgcolor + "' width='100%' cellspacing=0 cellpadding=0><tr><td>" + ret + "</td></tr></table>" ;
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
	while ( was.GetCount() <= id ) was.Add ( "" ) ;
	while ( data.GetCount() <= id ) data.Add ( "" ) ;
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
    
    blast_thread = NULL ;

    up = new wxPanel ( this ) ;
    hlb = new EILB ( this , ID_HLB ) ;
    
    if ( mode == EI_NCBI ) init_ncbi() ;
    if ( mode == EI_BLAST ) init_blast() ;
    
    v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( up , 0 , wxEXPAND , 5 ) ;
    v0->Add ( hlb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
}
     

wxString EIpanel::valFC ( TiXmlNode *n )
	{
	if ( !n ) return "" ;
	if ( !n->FirstChild() ) return "" ;
	return val ( n->FirstChild() ) ;
	}
     
wxString EIpanel::val ( TiXmlNode *n )
	{
	if ( !n ) return "" ;
	if ( !n->Value() ) return "" ;
	return wxString ( n->Value() ) ;
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
		if ( database == "pubmed" ) 
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
	wxBeginBusyCursor () ;
	hlb->Clear () ;
	if ( mode == EI_NCBI ) process_ncbi() ;
	if ( mode == EI_BLAST ) process_blast() ;
	hlb->Update () ;
	wxEndBusyCursor () ;
	}    
	
wxString EIpanel::num2html ( int num , int digits )
	{
	wxString s = wxString::Format ( "%d" , num ) ;
	while ( s.length() < digits ) s = " " + s ;
	s.Replace ( " " , "&nbsp;" ) ;
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
     
