#include "FindSequenceDialog.h"

BEGIN_EVENT_TABLE(FindSequenceDialog, wxDialog )
    EVT_BUTTON(SH_SEARCH,FindSequenceDialog::OnSearch)
    EVT_BUTTON(SH_CANCEL,FindSequenceDialog::OnCancel)
    EVT_LISTBOX(SH_LB,FindSequenceDialog::OnLB)
    EVT_LISTBOX_DCLICK(SH_LB,FindSequenceDialog::OnLBdclick)
    EVT_TEXT(SH_TEXT,FindSequenceDialog::OnTextChange)
    EVT_CHAR_HOOK(FindSequenceDialog::OnCharHook)
END_EVENT_TABLE()


FindSequenceDialog::FindSequenceDialog ( wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 350 , 400 ) )
    {
    p = 0 ;
    c = (ChildBase*) parent ;
    bo = 5 ;
    fh = 20 ;
    allowed_chars = "AGCT" ; // For DNA search
    int w , h ;
    GetClientSize ( &w , &h ) ;
    t = new wxTextCtrl ( this , SH_TEXT , "" , wxPoint ( bo , bo ) ,
                            wxSize ( w-bo*2 , fh ) ) ;
                            
    wxButton *f = new wxButton ( this , SH_SEARCH , txt("b_find") , wxPoint ( bo , fh+bo*2 ) , 
                            wxSize ( w/3 , fh ) ) ;

    new wxButton ( this , SH_CANCEL , txt("b_cancel") , wxPoint ( w*2/3-bo , fh+bo*2 ) , 
                            wxSize ( w/3 , fh ) ) ;
                            
    lb = new wxListBox ( this , SH_LB , wxPoint ( bo , fh*2+bo*3 ) , wxSize ( w - bo*2 , h - fh*2 - bo*4 ) ) ;
    
    f->SetDefault () ;
    t->SetFocus () ;
    Center () ;
    int x , y ;
    GetPosition ( &x , &y ) ;
    x -= w ;
    y -= h/2 ;
    if ( x < bo ) x = bo ;
    if ( y < bo ) y = bo ;
    Move ( x , y ) ;
    }
    
void FindSequenceDialog::OnLB ( wxCommandEvent &ev )
    {
    doAction ( false ) ;
    }
    
void FindSequenceDialog::OnLBdclick ( wxCommandEvent &ev )
    {
    doAction ( true ) ;
    wxDialog::OnOK ( ev ) ;
    }    
    
void FindSequenceDialog::doAction ( bool doubleclick )
    {
    int idx = lb->GetSelection () ;
    wxString s = lb->GetStringSelection () ;
    wxString type = s.BeforeFirst ( ':' ) ;
    wxString data = s.AfterFirst ( ':' ) ;
    
    wxString mark ;
    SequenceCanvas *canvas ;
    if ( c->def == "dna" )
        {
        mark = "DNA" ;
        canvas = ( (MyChild*) c ) -> cSequence ;
        }    
    else if ( c->def == "PrimerDesign" )
        {
        mark = "PRIMER_UP" ;
        canvas = ( (TPrimerDesign*) c ) -> sc ;
        }    
    else if ( c->def == "AminoAcids" )
        {
        mark = "AA" ;
        canvas = ( (TAminoAcids*) c ) -> sc ;
        }    
    
    if ( type == txt("sequence") )
        {
        long from , to ;
        data.BeforeFirst('-').ToLong ( &from ) ;
        data.AfterFirst('-').ToLong ( &to ) ;        
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        }    
    else if ( type == txt("amino_acid") )
        {
        long from , to ;
        data = data.AfterLast ( '(' ) ;
        data.BeforeFirst('-').ToLong ( &from ) ;
        data.AfterFirst('-').ToLong ( &to ) ;        
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        }    
    else if ( type == txt("t_vec_item") )
        {
        int from = c->vec->items[vi[idx]].from ;
        int to = c->vec->items[vi[idx]].to ;
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        if ( doubleclick )
           {
           Hide () ;
           if ( c->def == "dna" ) ( (MyChild*) c ) -> cPlasmid -> invokeVectorEditor ( "item" , vi[idx] ) ;
           if ( c->def == "AminoAcids" ) ( (TAminoAcids*) c ) -> invokeVectorEditor ( "item" , vi[idx] ) ;
           }    
        }    
    else if ( type == txt("m_restriction") )
        {
        int a = vi[idx] ;
        int from = c->vec->rc[a].pos - c->vec->rc[a].e->cut + 1 ;
        int to = c->vec->rc[a].pos - c->vec->rc[a].e->cut + c->vec->rc[a].e->sequence.length() + 1 ;
        canvas->mark ( mark , from , to ) ;
        canvas->ensureVisible ( canvas->getBatchMark() ) ;
        if ( doubleclick && c->def == "dna" ) // Only is DNA
           {
           Hide () ;
           ( (MyChild*) c ) -> cPlasmid -> invokeVectorEditor ( "enzyme" , a ) ;
           }    
        }    
    }    
    
void FindSequenceDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

bool FindSequenceDialog::doesMatch ( char a , char b )
    {
    if ( a == b ) return true ;
    if ( a == '?' || b == '?' ) return true ;
    return false ;
    }
    
int FindSequenceDialog::subsearch ( const wxString &s , const wxString &sub , int start )
    {
    int a , b ;
    if ( s.length() < sub.length() ) return -1 ;
    for ( a = start ; a < s.length() - sub.length() + 1 ; a++ )
        {
        for ( b = 0 ; b < sub.length() ; b++ )
           {
           if ( sub[b] == '*' )
              {
              if ( subsearch ( s , sub.substr ( b+1 ) , a ) == -1 ) break ;
              b = sub.length()-1 ;
              }
           else
              {
              if ( !doesMatch ( sub[b] , s[a+b] ) ) break ;
              last = a+b ;
              }
           }
        if ( b == sub.length() ) return a ;
        }
    return -1 ;
    }

void FindSequenceDialog::OnSearch ( wxCommandEvent &ev )
    {
    lb->Clear () ;
    vi.Clear () ;
    sequenceSearch() ;
    if ( c->def == "dna" || c->def == "PrimerDesign" ) sequenceSearch ( true ) ;
    if ( c->def == "dna" || c->def == "PrimerDesign" ) aaSearch () ;
    itemSearch() ;
    restrictionSearch() ;
    if ( !vi.IsEmpty() )
        {
        lb->SetSelection ( 0 ) ;
        OnLB ( ev ) ;
        }    
    }
    
void FindSequenceDialog::aaSearch ()
    {
    int a , b ;
    TVector *v = c->vec ;
    if ( v->type != TYPE_VECTOR && 
    	 v->type != TYPE_SEQUENCE && 
    	 v->type != TYPE_PRIMER )
    	 return ;
    	 
    wxString s = t->GetValue().Upper() ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        wxString ls = v->items[a].getAminoAcidSequence() ;
        if ( ls.IsEmpty() ) continue ;
        int dir = v->items[a].getDirection() ;
        int off = v->items[a].getOffset() ;
        vector <Tdna2aa> dna2aa ;
        p = 0 ;
        b = subsearch ( ls , s , p ) ;
        while ( b != -1 )
           {
           if ( dna2aa.size() == 0 )
              v->items[a].translate ( v , NULL , dna2aa ) ;
           int from = dir==1?b:last ;
           int to = dir==1?last:b ;
           wxString msg ;
           if ( off != -1 )
              msg = wxString::Format ( " [%d-%d]" , from+off , to+off ) ;
           from = dna2aa[from].dna[0] + 1 ;
           to = dna2aa[to].dna[2] + 1 ;
           if ( from > to ) { int x = from ; from = to ; to = x ; }
           lb->Append ( wxString::Format ( "%s: %s (%d-%d)%s" , 
                                 txt("amino_acid") ,
                                 v->items[a].name.c_str() ,
                                 from , to , msg.c_str() ) ) ;
           vi.Add ( -1 ) ;
           p = b + 1 ;
           b = subsearch ( ls , s , p ) ;
           }
        }   
        
    // Fixed reading frames
    wxString ss ;
    ss = v->getSequence() ;
    if ( v->isCircular() ) ss += ss.Left ( 2 ) ;
    else ss += "  " ;
    aaSubSearch ( ss , 0 , 1 , txt("m_aa_1") ) ; // Reading frame +1     
    aaSubSearch ( ss , 1 , 1 , txt("m_aa_2") ) ; // Reading frame +2     
    aaSubSearch ( ss , 2 , 1 , txt("m_aa_3") ) ; // Reading frame +3
    ss = v->transformSequence ( true , false ) ;
    if ( v->isCircular() ) ss = ss.Right ( 2 ) + ss ;
    else ss = "  " + ss ;
    aaSubSearch ( ss , -1 , -1 , txt("m_aa_m1") ) ; // Reading frame -1     
    aaSubSearch ( ss , -2 , -1 , txt("m_aa_m2") ) ; // Reading frame -2     
    aaSubSearch ( ss , -3 , -1 , txt("m_aa_m3") ) ; // Reading frame -3    
    }    
    
void FindSequenceDialog::aaSubSearch ( const wxString &s , int start , int dir , wxString rf )
	{
	int a ;
	wxString codon , res ;
	wxArrayInt ai ;
    TVector *v = c->vec ;
    wxString sub = t->GetValue().Upper() ;
    int ostart = start < 0 ? 2 : 0 ;
    if ( start < 0 )
    	{
	    for ( start = -start ; start + 3 < s.length() ; start += 3 ) ;
    	}    
	for ( a = start ; a + dir * 2 >= 0 && a + dir * 2 < s.length() ; a += dir * 3 )
		{
		codon.Empty() ;
		codon += s.GetChar ( a + dir * 0 ) ;
		codon += s.GetChar ( a + dir * 1 ) ;
		codon += s.GetChar ( a + dir * 2 ) ;
		res += v->dna2aa ( codon ) . GetChar ( 0 ) ;
		ai.Add ( a ) ;
		}    
	a = res.Find ( sub ) ;
	while ( a != -1 )
		{
		int from = ai[a] + 1 - ostart ;
		int to = ai[a+sub.length()-1] + dir * 2 + 1 - ostart ;
		if ( from > to ) { int z = from ; from = to ; to = z ; }
		wxString msg = rf.BeforeFirst ( '\t' ) ;
        lb->Append ( wxString::Format ( "%s: %s (%d-%d)" , 
                             txt("amino_acid") ,
                             msg.c_str() ,
                             from , to ) ) ;
		res.SetChar ( a , '_' ) ; // Invalidating
		a = res.Find ( sub ) ;
		}    
	}    
    
void FindSequenceDialog::itemSearch ()
    {
    int a , b ;
    TVector *v = c->vec ;
    wxString s = t->GetValue().Upper() ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        if ( v->items[a].name.Upper().Find(s) != -1 ||
             v->items[a].desc.Upper().Find(s) != -1 )
           {
           lb->Append ( wxString::Format ( "%s: %s (%d-%d)" , 
                                 txt("t_vec_item") , 
                                 v->items[a].name.c_str() ,
                                 v->items[a].from , 
                                 v->items[a].to ) ) ;
           vi.Add ( a ) ;           
           }    
        }    
    }    
    
void FindSequenceDialog::restrictionSearch ()
    {
    int a , b ;
    TVector *v = c->vec ;
    wxString s = t->GetValue().Upper() ;
    for ( a = 0 ; a < v->rc.size() ; a++ )
        {
        if ( v->rc[a].e->name.Upper().Find(s) != -1 )
           {
           lb->Append ( wxString::Format ( "%s: %s (%d)" , 
                                 txt("m_restriction") , 
                                 v->rc[a].e->name.c_str() ,
                                 v->rc[a].pos+1 ) ) ;
           vi.Add ( a ) ;
           }    
        }    
    }    
    
void FindSequenceDialog::sequenceSearch ( bool invers )
    {
    bool cont = true ;
    int a , b ;
    wxString sub = t->GetValue().Upper() ;
    p = 0 ;
    if ( sub.IsEmpty() ) return ;

    // Preparing sequence    
    wxString s = c->vec->getSequence() ;
    if ( invers ) { s = c->vec->transformSequence ( true , true ) ; }
    if ( c->vec->isCircular() )
        {
        wxString t ;
        t = s ;
        while ( t.length() < sub.length() ) t += " " ;
        s += t.substr ( 0 , sub.length()-1 ) ;
        }
        

    while ( cont )
        {
/*        wxString dummy , allow = allowed_chars + "?*" ;
        for ( a = 0 ; a < sub.length() ; a++ ) // Filtering bogus chars
            {
            if ( sub.GetChar(a) >= 'a' && sub.GetChar(a) <= 'z' ) sub.SetChar ( a , sub.GetChar(a) - 'a' + 'A' ) ;
            for ( b = 0 ; b < allow.length() && sub.GetChar(a) != allow.GetChar(b) ; b++ ) ;
            if ( b < allow.length() ) dummy += sub.GetChar(a) ;
            }
        sub = dummy ;
        if ( sub.IsEmpty() ) return ;
*/
        // Now we search...
        a = subsearch ( s , sub , p ) ;
        if ( a > -1 )
            {
            p = a+1 ;
            int from = a + 1 ;
            int to = last + 1 ;
            if ( to > c->vec->getSequenceLength() ) to -= c->vec->getSequenceLength() ;
            wxString msg ;
            if ( invers )
               {
               msg = " (3'->5')" ;
               int nt = c->vec->getSequenceLength() - from + 1 ;
               int nf = c->vec->getSequenceLength() - to + 1 ;
               from = nf ;
               to = nt ;
               }    
            lb->Append ( wxString::Format ( "%s: %d-%d%s" , txt("sequence") , from , to , msg.c_str() ) ) ;
            vi.Add ( -1 ) ;
            }
        else
            {
            cont = false ;
            }
        }    
    }
    
void FindSequenceDialog::OnTextChange ( wxCommandEvent &ev )
    {
    if ( c->vec->getGenomeMode() ) return ;
    if ( t->GetValue().length() < 3 )
       {
       lb->Clear () ;
       vi.Clear () ;
       return ;
       }    
    OnSearch ( ev ) ;
    }    

void FindSequenceDialog::OnCancel ( wxCommandEvent &ev )
    {
    wxDialog::OnOK ( ev ) ;
    }

