#include "TLigationDialog.h"

BEGIN_EVENT_TABLE(TLigationDialog, wxDialog )
    EVT_CHECKLISTBOX(LD_SOURCES,TLigationDialog::OnToggleSources)
    EVT_BUTTON(LD_OK,TLigationDialog::OnOK)
    EVT_BUTTON(LD_CANCEL,TLigationDialog::OnCancel)
    EVT_CHAR_HOOK(TLigationDialog::OnCharHook)
END_EVENT_TABLE()

void TLigationDialog::OnCharHook(wxKeyEvent& event)
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else event.Skip() ;
    }

TLigationDialog::TLigationDialog(wxWindow *parent, const wxString& title )
         : wxDialog ( parent , -1 , title , wxDefaultPosition , wxSize ( 600 , 450 ) )
{
    doLigate = false ;
}

void TLigationDialog::init ()
    {
    int w , h ;
    int bo = 5 ;
    int lh = 2 * bo ;
    GetClientSize ( &w , &h ) ;
    l_sources = new wxCheckListBox ( this , LD_SOURCES , wxPoint ( bo , lh*2 ) , wxSize ( w/3-bo , h-lh*6 ) ) ;
    l_targets = new wxCheckListBox ( this , LD_TARGETS , wxPoint ( w/3 , lh*2 ) , wxSize ( w*2/3-bo , h-lh*6 ) ) ;
    
    new wxStaticText ( this , -1 , txt("frag2lig") , wxPoint ( bo , bo ) ) ;
    new wxStaticText ( this , -1 , txt("what2create") , wxPoint ( w/3 , bo ) ) ;

    new wxButton ( this , LD_OK , txt("b_ligate") , 
                                wxPoint ( bo , h - lh*3 - bo ) , 
                                wxSize ( w / 4 , lh*3 )
                                ) ;
    new wxButton ( this , LD_CANCEL , txt("b_cancel") , 
                                wxPoint ( w - w/4 - bo , h - lh*3 - bo ) , 
                                wxSize ( w / 4 , lh*3 )
                                ) ;

    
    for ( int a = 0 ; a < vv.size() ; a++ )
        {
        bool state = true ;
        if ( !vv[a]->hasStickyEnds() )
           state = false ; // blunt ends are not ligated by default...
        l_sources->Append ( vv[a]->getName().c_str() ) ;
        l_sources->Check ( a , state ) ;
        }
        
    Center() ;
    generateTargets () ;
    }
    
void TLigationDialog::generateTargets ()
    {
    int a , b ;
    int cnt = l_sources->GetCount() ;
    vector <bool> vc , used ;
    wxArrayInt vi ;
    for ( a = 0 ; a < cnt ; a++ )
        {
        vc.push_back ( l_sources->IsChecked ( a ) ) ;
        used.push_back ( !l_sources->IsChecked ( a ) ) ;
        }
    orientation.clear () ;
    vt.Clear () ;
    ligates.clear () ;
    l_targets->Clear () ;

    // Using each selected item as a starter
    for ( a = 0 ; a < cnt ; a++ )
        {
        if ( vc[a] )
           {
           myass ( a < vc.size() , "TLigationDialog::generateTargets:1" ) ;
           myass ( a < used.size() , "TLigationDialog::generateTargets:2" ) ;
           used[a] = true ;
           vi.Add ( a ) ;
           orientation.push_back ( false ) ;
           curseTargets ( vc , used , vi ) ;
           orientation.pop_back () ;
           vi.RemoveAt ( vi.GetCount()-1 ) ;
           used[a] = false ;
           }
        }

    for ( a = 0 ; a < vt.GetCount() ; a++ )
        {
        l_targets->Append ( vt[a] ) ;
        l_targets->Check ( a , true ) ;
        }
    }
    
void TLigationDialog::curseTargets ( vector <bool> &vc , vector <bool> &used , wxArrayInt &vi  )
    {
    int a , b ;
    int cnt = vi.GetCount() ;
    string name ;
    myass ( cnt-1 >= 0 && cnt-1 < orientation.size() , "TLigationDialog::curseTargets:1" ) ;
    myass ( cnt-1 >= 0 && cnt-1 < vi.GetCount() , "TLigationDialog::curseTargets:2" ) ;
    bool o = orientation[cnt-1] ;
    if ( doMatch ( vi[cnt-1] , vi[0] , o ) )
        {
        name = getVIName ( vi ) + txt("_circ") ;
        addVTname ( name , vi , true ) ;
        }
    else if ( cnt > 1 )
        {
        name = getVIName ( vi ) ;
        addVTname ( name , vi , false ) ;
        }
        
    for ( a = 0 ; a < vc.size() ; a++ )
        {
        myass ( cnt-1 >= 0 && cnt-1 < vi.GetCount() , "TLigationDialog::curseTargets:3a" ) ;
        myass ( a >= 0 && a < vc.size() , "TLigationDialog::curseTargets:3b" ) ;
        myass ( a >= 0 && a < used.size() , "TLigationDialog::curseTargets:3c" ) ;
        if ( vc[a] && !used[a] && doMatch ( vi[cnt-1] , a ) ) // Normal orientation
           {
           used[a] = true ;
           vi.Add ( a ) ;
           orientation.push_back ( false ) ;
           curseTargets ( vc , used , vi ) ;
           orientation.pop_back () ;
           vi.RemoveAt ( vi.GetCount()-1 ) ;
           used[a] = false ;
           }
        }

    for ( a = 0 ; a < vc.size() ; a++ )
        {
        myass ( cnt-1 >= 0 && cnt-1 < vi.GetCount() , "TLigationDialog::curseTargets:4a" ) ;
        myass ( a >= 0 && a < vc.size() , "TLigationDialog::curseTargets:4b" ) ;
        myass ( a >= 0 && a < used.size() , "TLigationDialog::curseTargets:4c" ) ;
        if ( vc[a] && !used[a] && doMatch ( vi[cnt-1] , a , true ) ) // Turned around
           {
           used[a] = true ;
           vi.Add ( a ) ;
           orientation.push_back ( true ) ;
           curseTargets ( vc , used , vi ) ;
           orientation.pop_back () ;
           vi.RemoveAt ( vi.GetCount() - 1 ) ;
           used[a] = false ;
           }
        }
    }
    
wxString TLigationDialog::invert ( wxString s )
    {
    wxString t ;
    for ( int a = 0 ; a < s.length() ; a++ ) t = s.GetChar(a) + t ;
    return t ;
    }

bool TLigationDialog::doMatch ( int l , int r , bool invertSecond )
    {
    wxString s1 , s2 ; 

    myass ( l >= 0 && l < vv.size() , "TLigationDialog::doMatch:1" ) ;
    myass ( vv[l] , "TLigationDialog::doMatch:2" ) ;
    myass ( r >= 0 && r < vv.size() , "TLigationDialog::doMatch:3" ) ;
    myass ( vv[r] , "TLigationDialog::doMatch:4" ) ;
    if ( invertSecond )
       {
       s1 = vv[l]->getStickyEnd(false,true) + invert ( vv[r]->getStickyEnd(false,false) ) ;
       s2 = vv[l]->getStickyEnd(false,false) + invert ( vv[r]->getStickyEnd(false,true) ) ;
       }
    else
       {
       s1 = vv[l]->getStickyEnd(false,true) + vv[r]->getStickyEnd(true,true) ;
       s2 = vv[l]->getStickyEnd(false,false) + vv[r]->getStickyEnd(true,false) ;
       }
    
    TVector v ;
    v.setSequence ( s1 ) ;
    wxString s3 = v.transformSequence ( true , false ) ;
    if ( s2 == s3 ) return true ;
    return false ;
    }
    
string TLigationDialog::getVIName ( wxArrayInt &vi )
    {
    int a ;
    string ret ;
    for ( a = 0 ; a < vi.GetCount() ; a++ )
        {
        myass ( a >= 0 && a < vi.GetCount() , "TLigationDialog::getVIName:1" ) ;
        myass ( vi[a] >= 0 && vi[a] < vv.size() , "TLigationDialog::getVIName:2" ) ;
        myass ( vv[vi[a]] , "TLigationDialog::getVIName:3" ) ;
        wxString name = vv[vi[a]]->getName() ;
        if ( a < orientation.size() && orientation[a] ) name = "!" + name ;
        if ( ret != "" ) ret += "-" ;
        ret += name.c_str() ;
        }
    return ret ;
    }
    
void TLigationDialog::addVTname ( string name , wxArrayInt &vi , bool circular )
    {
    vt.Add ( name.c_str() ) ;
    
    int a ;
    TVector v ;
    wxString d ;
    myass ( vi[0] >= 0 && vi[0] < vv.size() , "TLigationDialog::addVTname:1" ) ;
    myass ( vv[vi[0]] , "TLigationDialog::addVTname:2" ) ;
    v.setFromVector ( *vv[vi[0]] ) ;
    d = txt("lig_of") ;
    d += v.getName() + " (" + v.getDescription() + ")" ;
    v.setName ( name.c_str() ) ;
    for ( a = 1 ; a < vi.GetCount() ; a++ )
        {
        bool o = false ;
        if ( a < orientation.size() && orientation[a] ) o = true ;
        TVector tv ;
        myass ( vi[a] >= 0 && vi[a] < vv.size() , "TLigationDialog::addVTname:3" ) ;
        myass ( vv[vi[a]] , "TLigationDialog::addVTname:4" ) ;
        tv.setFromVector ( *vv[vi[a]] ) ;
        v.ligate_right ( tv , o ) ;
        d += ", " ;
        if ( o ) d += "!" ;
        d += vv[vi[a]]->getName() + " (" + vv[vi[a]]->getDescription() + ")" ;
        }
    if ( circular ) v.closeCircle () ;
    v.recalculateCuts () ;
    v.setDescription ( d ) ;
    ligates.push_back ( v ) ;
    }
    
void TLigationDialog::OnToggleSources ( wxCommandEvent &ev )
    {
    generateTargets () ;
    }

void TLigationDialog::OnCancel ( wxCommandEvent &ev )
    {
    SetReturnCode ( wxID_CANCEL ) ;
    EndModal ( false ) ;    
    }
        
void TLigationDialog::OnOK ( wxCommandEvent &ev )
    {
    doLigate = true ;
    int a ;
    for ( a = 0 ; a < ligates.size() ; a++ )
        {
        if ( !l_targets->IsChecked ( a ) )
           ligates[a].setSequence ( wxString("") ) ; // Yes, it's brutal ;-)
        }
    SetReturnCode ( wxID_OK ) ;
    EndModal ( true ) ;    
    }

