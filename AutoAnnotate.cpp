#include "AutoAnnotate.h"

class TAutoAnnotateDialog : public wxDialog
	{
	public :
    TAutoAnnotateDialog ( wxWindow *parent, const wxString& title , AutoAnnotate *_aa ) ; ///< Constructor
    virtual void OnCharHook ( wxKeyEvent& event ) ; ///< Key event handler
    
    wxCheckBox *useCommonDatabase , *useAdditionalDatabase , *useMachete , *doAddORFs ;
    wxChoice *additionalDatabase ;
    
    private :
	AutoAnnotate *aa ;

    DECLARE_EVENT_TABLE()
	} ;    
	
BEGIN_EVENT_TABLE(TAutoAnnotateDialog, wxDialog )
    EVT_BUTTON(wxID_OK,TAutoAnnotateDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL,TAutoAnnotateDialog::OnCancel)
    EVT_CHAR_HOOK(TAutoAnnotateDialog::OnCharHook)
END_EVENT_TABLE()

	
TAutoAnnotateDialog::TAutoAnnotateDialog ( wxWindow *parent, const wxString& title , AutoAnnotate *_aa )
    : wxDialog ( parent , TSD , title , wxDefaultPosition , wxSize ( 400 , 200 ) )
	{
	aa = _aa ;
	aa->dbname.Clear () ;
	aa->dbfile.Clear () ;
    myapp()->frame->LS->getDatabaseList ( aa->dbname , aa->dbfile ) ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h_last = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h4 = new wxBoxSizer ( wxHORIZONTAL ) ;
    
    useCommonDatabase = new wxCheckBox ( this , -1 , txt("t_autoann_use_common_database") ) ;
    useAdditionalDatabase = new wxCheckBox ( this , -1 , txt("t_autoann_use_additional_database") ) ;
    useMachete = new wxCheckBox ( this , -1 , txt("t_autoann_reduce_item_number") ) ;
    doAddORFs = new wxCheckBox ( this , -1 , txt("t_autoann_add_orfs") ) ;
    
    additionalDatabase = new wxChoice ( this , -1 ) ;

    h1->Add ( useCommonDatabase , 0 , wxALL , 5 ) ;
    
    h2->Add ( useAdditionalDatabase , 0 , wxALL , 5 ) ;
    h2->Add ( additionalDatabase , 0 , wxALL , 5 ) ;
    
    h3->Add ( useMachete , 0 , wxALL , 5 ) ;

    h4->Add ( doAddORFs , 0 , wxALL , 5 ) ;
    
    h_last->Add ( new wxButton ( this , wxID_OK , txt("b_ok" ) ) , 0 , wxALL , 5 ) ;
    h_last->Add ( new wxButton ( this , wxID_CANCEL , txt("b_cancel" ) ) , 0 , wxALL , 5 ) ;
    
    v0->Add ( h1 , 0 , 0 ) ;    
    v0->Add ( h2 , 0 , 0 ) ;    
    v0->Add ( h3 , 0 , 0 ) ;    
    v0->Add ( h4 , 0 , 0 ) ;
    v0->Add ( h_last , 0 , 0 ) ;    
    
    useCommonDatabase->SetValue ( aa->useCommonDatabase ) ;
    useAdditionalDatabase->SetValue ( aa->useAdditionalDatabase ) ;
    useMachete->SetValue ( aa->useMachete ) ;
    doAddORFs->SetValue ( aa->doAddORFs ) ;
    
    additionalDatabase->Append ( aa->dbname ) ;
    additionalDatabase->SetStringSelection ( aa->additionalDatabase ) ;
    if ( additionalDatabase->GetStringSelection() == "" )
    	additionalDatabase->SetSelection ( 0 ) ;
    
    
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;
    Center () ;    
	}
 
void TAutoAnnotateDialog::OnCharHook ( wxKeyEvent& event )
	{
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_RETURN ) OnOK ( ev ) ;
    else event.Skip() ;
	}    


//******************************************************************************

bool AutoAnnotate::SettingsDialog ()
	{
	TAutoAnnotateDialog aad ( p , txt("u_autoannotate") , this ) ;
	if ( wxID_OK != aad.ShowModal () ) return false ;
	useCommonDatabase = aad.useCommonDatabase->GetValue() ;
	useAdditionalDatabase = aad.useAdditionalDatabase->GetValue() ;
	useMachete = aad.useMachete->GetValue() ;
	doAddORFs = aad.doAddORFs->GetValue() ;
	
	wxString s = aad.additionalDatabase->GetLabel() ;
	additionalDatabase = dbfile[dbname.Index(s)] ;

	myapp()->frame->LS->startRecord() ;
  	myapp()->frame->LS->setOption ( "AUTOANN_ADDITIONALDB" , s ) ;
 	myapp()->frame->LS->setOption ( "AUTOANN_USECOMMON" , useCommonDatabase ) ;
 	myapp()->frame->LS->setOption ( "AUTOANN_USEADDITIONAL" , useAdditionalDatabase ) ;
  	myapp()->frame->LS->setOption ( "AUTOANN_USEMACHETE" , useMachete ) ;
  	myapp()->frame->LS->setOption ( "AUTOANN_DOORFS" , doAddORFs ) ;
	myapp()->frame->LS->endRecord() ;
	
	return true ;
	}    

void AutoAnnotate::Run ()
	{
//	wxStartTimer () ;
	bool foundany = false ;
	wxBeginBusyCursor() ;
	if ( useCommonDatabase ) foundany |= ScanDatabase ( commonVectorsDatabase ) ;
	if ( useAdditionalDatabase ) foundany |= ScanDatabase ( additionalDatabase ) ;
	if ( useMachete ) machete ( p->vec ) ;
	if ( doAddORFs ) foundany |= addORFs ( p->vec ) ;

	if ( foundany )
		{
  		p->vec->setChanged () ;
  		p->vec->updateDisplay() ;
        p->treeBox->SelectItem ( p->treeBox->vroot ) ;
        p->updateSequenceCanvas() ;
        p->cPlasmid->Refresh () ;
        wxCommandEvent event ;
        p->OnAA_known ( event ) ;
        p->vec->undo.stop() ;
		}    
	else p->vec->undo.abort () ;
	wxEndBusyCursor() ;
//	wxMessageBox ( wxString::Format ( "Took %d ms" , wxGetElapsedTime() ) ) ;
	}    


AutoAnnotate::AutoAnnotate ( MyChild *_p )
	{
	p = _p ;
 	commonVectorsDatabase = myapp()->homedir + myapp()->slash + "commonvectors.db" ;
  	additionalDatabase =  myapp()->frame->LS->getOption ( "AUTOANN_ADDITIONALDB" , "" ) ;
 	useCommonDatabase = myapp()->frame->LS->getOption ( "AUTOANN_USECOMMON" , true ) ;
 	useAdditionalDatabase = myapp()->frame->LS->getOption ( "AUTOANN_USEADDITIONAL" , true ) ;
  	useMachete = myapp()->frame->LS->getOption ( "AUTOANN_USEMACHETE" , true ) ;
  	doAddORFs = myapp()->frame->LS->getOption ( "AUTOANN_DOORFS" , true ) ;
	}

bool AutoAnnotate::ScanDatabase ( wxString database )
	{
	int a , b ;
	wxString oseq = p->vec->getSequence() ;
	if ( p->vec->isCircular() ) oseq += oseq ;
	p->vec->undo.start ( txt("u_autoannotate") ) ;
	
	// Load all sequences from database
	TManageDatabaseDialog mdb ( myapp()->frame , "dummy" , ACTION_MODE_STARTUP ) ;
	TStorage *db = mdb.getTempDB ( database ) ;
	TSQLresult r ;

	wxString dbname = db->UCfirst ( database.AfterLast('/').AfterLast('\\').BeforeLast('.') ) ;
	if ( wxNOT_FOUND == mdb.db_file.Index ( database.c_str() ) )
		{
		mdb.db_name.Add ( dbname ) ;
		mdb.db_file.Add ( database ) ;
		}		

	r = db->getObject ( "SELECT DISTINCT di_dna FROM dna_item" ) ; // Find all DNA with items
	bool foundany = false ;
	for ( a = 0 ; a < r.content.size() ; a++ )
		{
  		wxString name = r[a][0] ;
  		if ( name.Trim().IsEmpty() ) continue ; // Blank name - not good...
		mdb.justload = true ;
		bool success = mdb.do_load_DNA ( name , dbname ) ;
		if ( !success ) continue ; // Not loaded, ignore
		TVector *v = mdb.v ;
		mdb.v = NULL ; // Just for safety...
  		for ( b = 0 ; b < v->items.size() ; b++ )
	    	foundany |= MatchItem ( v , v->items[b] , p->vec , oseq ) ;
  		delete v ;
		}
		
	return foundany ;
	}    
	
bool AutoAnnotate::RawMatch ( TVectorItem &item , TVector *v , wxString &oseq , wxString &s )
   	{
    int from = oseq.Find ( s ) ;
    if ( from == -1 ) return false ;

    from++ ;
    int to = from + s.length() - 1 ;
    if ( to > v->getSequenceLength() )
    	to -= v->getSequenceLength() ;
	    	
   	TVectorItem i = item ;
	i.from = from ;
	i.to = to ;
	i.setLastVector ( v ) ;
	v->items.push_back ( i ) ;
	return true ;
   	}
         
bool AutoAnnotate::MatchItem ( TVector *tv , TVectorItem &item , TVector *v , wxString &oseq )
	{
    int from = item.from ;
    int to = item.to ;
    int dir = item.getDirection() == -1 ? -1 : 1 ;
    
    wxString s ;
    s = tv->getSequence() ;
    if ( to < from ) to += s.length() ;
    s += s ;
    s = s.Mid ( from - 1 , to - from + 1 ) ;
    
    if ( s.length() < 10 ) return false ; // Too short a sequence for a match to have any meaning
    
    // Did we already try this?
    wxString key = wxString::Format ( ":%d:" , item.getType() ) + s ; // Fingerprint string
    if ( wxNOT_FOUND != alreadyin.Index ( key.c_str() , false ) ) return false ; // Already have this one
    alreadyin.Add ( key ) ; // Remember it

    bool ret = false ;
    item.desc += "\n[" + tv->getName() + "]" ;

    // Raw search
    ret |= RawMatch ( item , v , oseq , s ) ;
    
    // Raw search, reverse direction
    wxString t = s ;
    for ( int a = 0 ; a < s.length() ; a++ ) t.SetChar ( t.length() - a - 1 , s.GetChar(a) ) ;
    if ( s != t ) // No need to search for reverse palindrome
    	{
    	TVectorItem i2 = item ;
    	i2.setDirection ( -dir ) ;
        ret |= RawMatch ( i2 , v , oseq , t ) ;
        }    
	
	return ret ;
	}    

bool AutoAnnotate::addORFs ( TVector *v )
	{
	v->ClearORFs () ;
	v->addORFs ( 1 ) ;
	v->addORFs ( 2 ) ;
	v->addORFs ( 3 ) ;
	v->addORFs ( -1 ) ;
	v->addORFs ( -2 ) ;
	v->addORFs ( -3 ) ;
	
	bool found = false ;
	int a , b ;
	for ( a = 0 ; a < v->countORFs() ; a++ )
		{
  		TORF *o = v->getORF ( a ) ;
 		o->from++ ;
 		o->to++ ;
  		for ( b = 0 ; b < v->items.size() ; b++ )
  			{
	    	if ( v->items[b].getType() != VIT_CDS ) continue ;
	    	if ( v->items[b].from == o->from ) break ;
	    	if ( v->items[b].from == o->to ) break ;
	    	if ( v->items[b].to == o->from ) break ;
	    	if ( v->items[b].to == o->to ) break ;
  			}    
		if ( b < v->items.size() ) continue ; // Already a CDS item
		TVectorItem i ( "???" , "Unknown open reading frame" , o->from , o->to , VIT_CDS ) ;
		i.setRF ( 1 ) ;
		i.setDirection ( o->rf > 0 ? 1 : -1 ) ;
		v->items.push_back ( i ) ;
		found |= true ;
		}    
	v->ClearORFs () ;
	return found ;
	}    
	
bool AutoAnnotate::within ( TVectorItem &i1 , TVectorItem &i2 , TVector *v ) // i2 within i1?
	{
 	int f1 = i1.from ;
 	int f2 = i2.from ;
 	int t1 = i1.to ;
 	int t2 = i2.to ;
 	if ( t1 < f1 ) t1 += v->getSequenceLength() ;
 	if ( t2 < f2 ) t2 += v->getSequenceLength() ;
 	if ( f2 >= f1 && t2 <= t1 ) return true ;
 	return false ;
	}    

void AutoAnnotate::machete ( TVector *v )
	{
	vector <TVectorItem> i2 ;
	int a , b ;
	for ( a = 0 ; a < v->items.size() ; a++ )
		{
 		if ( v->items[a].from == -1 ) continue ;
  		for ( b = 0 ; b < v->items.size() ; b++ )
  			{
	    	if ( a == b ) continue ;
	    	if ( v->items[b].from == -1 ) continue ;
	    	if ( !within ( v->items[a] , v->items[b] , v ) ) continue ;
	    	if ( v->items[a].getType() == v->items[b].getType() ||
	    		 ( v->items[a].getType() == VIT_CDS && v->items[b].getType() != VIT_GENE )
	    		 )
	    	v->items[b].from = -1 ;
  			}    
		}    
	for ( a = 0 ; a < v->items.size() ; a++ )
		if ( v->items[a].from != -1 )
			i2.push_back ( v->items[a] ) ;
	v->items = i2 ;
	}    

// *********************

void MyChild::OnAutoAnnotate(wxCommandEvent& event)
	{
	AutoAnnotate auan ( this ) ;
	if ( auan.SettingsDialog () )
		auan.Run () ;
	}    


