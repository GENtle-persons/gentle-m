#include "AutoAnnotate.h"

AutoAnnotate::AutoAnnotate ( MyChild *_p )
	{
	p = _p ;
	}

void AutoAnnotate::ScanDatabase ( wxString database )
	{
	wxString oseq = p->vec->getSequence() ;
	if ( p->vec->isCircular() ) oseq += oseq ;
	p->vec->undo.start ( txt("u_autoannotate") ) ;
	
	// Load all sequences from database
	TStorage db ( TEMP_STORAGE , database ) ;
	TSQLresult r ;
	wxString dbname = myapp()->frame->LS->UCfirst ( database.AfterLast(':').AfterLast('/').AfterLast('\\').BeforeLast('.') ) ;
	r = db.getObject ( "SELECT dna_name FROM dna" ) ;
	int a , b ;
	bool foundany = false ;
	for ( a = 0 ; a < r.content.size() ; a++ )
		{
  		wxString name = r[a][0] ;
  		if ( name.Trim().IsEmpty() ) continue ; // Blank name - not good...
		TManageDatabaseDialog mdb ( myapp()->frame , "dummy" , ACTION_MODE_STARTUP ) ;
		mdb.justload = true ;
		bool success = mdb.do_load_DNA ( name , dbname ) ;
		if ( !success ) continue ; // Not loaded, ignore
		TVector *v = mdb.v ;
		mdb.v = NULL ; // Just for safety...
  		for ( b = 0 ; b < v->items.size() ; b++ )
	    	foundany |= MatchItem ( v , v->items[b] , p->vec , oseq ) ;
  		delete v ;
		}

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
	}    
	
bool AutoAnnotate::MatchItem ( TVector *tv , TVectorItem &item , TVector *v , wxString &oseq )
	{
    int from = item.from ;
    int to = item.to ;
//    int dir = item.getDirection() == -1 ? -1 : 1 ;
    
    wxString s ;
    s = tv->getSequence() ;
    if ( to < from ) to += s.length() ;
    s += s ;
    s = s.Mid ( from - 1 , to - from + 1 ) ;
    
    wxString key = item.name + wxString::Format ( ":%d:" , item.getType() ) + s ;
    if ( wxNOT_FOUND != alreadyin.Index ( key.c_str() , false ) ) return false ; // Already have one

    if ( s.length() < 5 ) return false ; // Too short a sequence for a match to have any meaning
    
    // Raw search
    from = oseq.Find ( s ) ;
    if ( from != -1 )
    	{
	    from++ ;
	    to = from + s.length() - 1 ;
	    if ( to > v->getSequenceLength() )
	    	to -= v->getSequenceLength() ;
	    	
    	TVectorItem i = item ;
    	i.from = from ;
    	i.to = to ;
    	i.setLastVector ( v ) ;
    	v->items.push_back ( i ) ;
    	alreadyin.Add ( key ) ;
    	return true ;
    	}    
	
	return false ;
	}    




// *********************

void MyChild::OnAutoAnnotate(wxCommandEvent& event)
	{
	AutoAnnotate auan ( this ) ;
	auan.ScanDatabase ( myapp()->homedir + myapp()->slash + "standardseq.db" ) ;
	}    


