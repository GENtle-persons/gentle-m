#include "ExternalInterface.h"

enum {
	ID_T1 = 6000,
	} ;	
   	

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
	EVT_BUTTON(wxID_OK, EIpanel::OnOK)
	EVT_TEXT_ENTER(ID_T1, EIpanel::OnOK)
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
    
    nb->AddPage ( new EIpanel ( nb , EI_PUBMED_NUCLEOTIDE ) , "Test" ) ;

/*
    rl = new wxSplitterWindow ( this , -1 ) ;
    right = new TMyImagePanel ( rl , IV_IMAGE ) ;
    right->imdi = this ;
    
    wxSplitterWindow *ud ;
    ud = new wxSplitterWindow ( rl , -1 ) ;

    rl->SplitVertically ( ud , right , 300 ) ;
    
    lb = new wxListBox ( ud , IV_LIST , wxDefaultPosition , wxDefaultSize ,
                            0 , NULL , wxLB_SORT ) ;
    bu = new wxButton ( ud , IV_BUTTON , "" ) ;
    
    ud->SplitHorizontally ( bu , lb , 20 ) ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
//    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( rl , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;
    
    wxString s_dir = myapp()->frame->LS->getOption ( "IMGDIR" , wxGetCwd() ) ;    
    ShowDir ( s_dir ) ;
*/

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
	: wxHtmlListBox ( parent , id )
	{
	SetItemCount ( 0 ) ;
	}    

wxString EILB::OnGetItem(size_t n) const
	{
	return was[n] ;
	}
     
// *****************************************************************************

EIpanel::EIpanel ( wxWindow *parent , int _mode )
	: wxPanel ( parent )
	{
	mode = _mode ;
	
	up = new wxPanel ( this ) ;
	hlb = new EILB ( this ) ;
	
	if ( mode == EI_PUBMED_NUCLEOTIDE ) init_pubmed_nucleotide() ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    v0->Add ( up , 0 , wxEXPAND , 5 ) ;
    v0->Add ( hlb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
//    v0->Fit ( this ) ;
	}
     
void EIpanel::init_pubmed_nucleotide()
	{
	t1 = new wxTextCtrl ( up , ID_T1 , "pGEX 3x" , wxDefaultPosition , wxDefaultSize , wxTE_PROCESS_ENTER ) ;
	b1 = new wxButton ( up , wxID_OK , "OK" , wxDefaultPosition ) ;

    wxBoxSizer *h0 = new wxBoxSizer ( wxHORIZONTAL ) ;
    h0->Add ( t1 , 1 , wxEXPAND , 5 ) ;
    h0->Add ( b1 , 0 , wxEXPAND , 5 ) ;
    up->SetSizer ( h0 ) ;
    h0->Fit ( up ) ;

	t1->SetFocus() ;
	}
	
void EIpanel::process_pubmed_nucleotide()
	{
	
	}    
    
void EIpanel::OnOK ( wxCommandEvent& event )
	{
	if ( mode == EI_PUBMED_NUCLEOTIDE ) process_pubmed_nucleotide() ;
 	}    
