#include "TVirtualGel.h"

BEGIN_EVENT_TABLE(TRestrictionIdentifier, TVirtualGel)
    EVT_CHECKLISTBOX(RI_DNA_LIST,TRestrictionIdentifier::OnDNAListChange)
    EVT_CHOICE(RI_MIN_PERCENT,TRestrictionIdentifier::OnDNAListChange)
    EVT_CHOICE(RI_MIN_BP,TRestrictionIdentifier::OnDNAListChange)
    EVT_CHOICE(RI_GROUP,TRestrictionIdentifier::OnDNAListChange)
    EVT_LISTBOX(RI_ENZYMES_LIST,TRestrictionIdentifier::OnEnzymeListChange)

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


TRestrictionIdentifier::TRestrictionIdentifier(wxWindow *parent, const wxString& title)
    : TVirtualGel ( parent , title )
{
    def = _T("RESTRICTION_IDENTIFIER") ;
    type = _T("DNA") ;
    running = false ;
}

wxString TRestrictionIdentifier::getName ()
    {
    return _T("Restriction Identifier") ;
    }

void TRestrictionIdentifier::initme ()
{
    TVirtualGel::initme () ;

    wxBoxSizer *main = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
//    wxBoxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *v = new wxBoxSizer ( wxVERTICAL ) ;

    dna_list     = new wxCheckListBox ( this , RI_DNA_LIST ) ;
    bp_list      = new wxChoice ( this , RI_MIN_BP ) ;
    percent_list = new wxChoice ( this , RI_MIN_PERCENT ) ;
    group_list   = new wxChoice ( this , RI_GROUP ) ;
    enzymes_list = new wxListBox ( this , RI_ENZYMES_LIST ) ;

    h1->Add ( new wxStaticText(this,-1,txt("t_minumum")+" ") ,  0 , wxEXPAND , 2 ) ;
    h1->Add ( bp_list ,                                         0 , wxEXPAND , 2 ) ;
    h1->Add ( new wxStaticText(this,-1,txt("t_bp")) ,           0 , wxEXPAND , 2 ) ;

    h2->Add ( new wxStaticText(this,-1,txt("t_minumum")+" ") ,  0 , wxEXPAND , 2 ) ;
    h2->Add ( percent_list ,                                    0 , wxEXPAND , 2 ) ;
    h2->Add ( new wxStaticText(this,-1,_T("%")) ,               0 , wxEXPAND , 2 ) ;

//    h3->Add ( new wxStaticText(this,-1,txt("enzyme_groups")+_T(" ")) , 0 , wxEXPAND , 2 ) ;
//    h3->Add ( group_list , 0 , wxEXPAND , 2 ) ;

    v->Add ( new wxStaticText(this,-1,txt("band_difference")) , 0 , wxEXPAND , 2 ) ;
    v->Add ( h1 ,                                               0 , wxEXPAND , 2 ) ;
    v->Add ( h2 ,                                               0 , wxEXPAND , 2 ) ;
//    v->Add ( h3 , 0 , wxEXPAND , 5 ) ;

    v->Add ( new wxStaticText(this,-1,txt("enzyme_groups")) ,   0 , wxEXPAND , 2 ) ;
    v->Add ( group_list ,                                       0 , wxEXPAND , 2 ) ;

    v->Add ( enzymes_list ,                                     2 , wxEXPAND , 2 ) ;
    v->Add ( dna_list ,                                         1 , wxEXPAND , 2 ) ;

    main->Add ( v ,                                             0 , wxEXPAND , 2 ) ;
    //main->Add ( virtualGel ,                                  1 , wxEXPAND , 2 ) ;
    main->Add ( right ,                                         1 , wxEXPAND , 2 ) ;


    wxBoxSizer *vs = new wxBoxSizer (wxVERTICAL) ;
    vs->Add ( toolbar ,                                         1 , wxEXPAND , 2 ) ;
    vs->Add ( main ,                                            1 , wxEXPAND , 2 ) ;


    // Init bp/% lists
    for ( int a = 0 ; a < 51 ; a++ )
	{
        percent_list->Append ( wxString::Format ( "%2d" , a ) ) ;
	}
    bp_list->Append ( "0" ) ;
    bp_list->Append ( "20" ) ;
    bp_list->Append ( "50" ) ;
    bp_list->Append ( "100" ) ;
    bp_list->Append ( "200" ) ;
    bp_list->Append ( "500" ) ;
    percent_list->SetSelection ( 10 ) ;
    bp_list->SetSelection ( 2 ) ;

    // Init enzyme groups list
    wxArrayString as ;
    myapp()->frame->LS->getEnzymeGroups ( as ) ;
    group_list->Append ( txt("All") ) ;
    for ( int a = 0 ; a < as.GetCount() ; a++ )
        {
        group_list->Append ( as[a] ) ;
        }
    group_list->SetSelection ( 0 ) ;

    // Init dna list
    listDNA() ;

    SetSizerAndFit ( vs ) ;

    //for ( int a = 0 ; a < dna_list->GetCount() ; a++ ) dna_list->Check ( a , true ) ;
    recalcEnzymes() ;
    running = true ;
}

void TRestrictionIdentifier::OnDNAListChange(wxCommandEvent &event)
{
    recalcEnzymes() ;
}

void TRestrictionIdentifier::OnEnzymeListChange(wxCommandEvent &event)
{
    while ( lanes.size() > 1 ) lanes.pop_back() ;

    int sel = enzymes_list->GetSelection() ;
    if ( sel != wxNOT_FOUND )
    {
        wxString en = enzymes_list->GetString ( sel ) ;
        int a ;
        for ( a = 0 ; a < dna_list->GetCount() ; a++ )
        {
            if ( !dna_list->IsChecked ( a ) ) continue ; // Not checked
            TVector *v = dna_items[a]->vec ;
            if ( !v ) continue ; // Paranoia
            addRestrictionLane ( en , v ) ;
        }
    }

    right->Refresh ( true ) ;
}

wxArrayInt TRestrictionIdentifier::getRestrictionFragments ( wxString en , TVector *v )
{
    wxArrayInt ret ;
    TRestrictionEnzyme *e = myapp()->frame->LS->getRestrictionEnzyme ( en );
    vector <TRestrictionCut> x ;
    v->getCuts ( e , x ) ;
    if ( x.size() == 0 ) return ret ; // Empty


    int a , last , len = v->getSequenceLength() ;
    if ( v->isCircular() )
    {
        last = x[x.size()-1].getPos() - len ;
    }
    else
    {
        last = 0 ;
        x.push_back ( TRestrictionCut ( len , e ) ) ;
    }

    for ( a = 0 ; a < x.size() ; a++ )
    {
        int diff = x[a].getPos() - last ;
        last = x[a].getPos() ;
        if ( diff == 0 ) continue ;
        if ( diff > len ) diff -= len ;
        ret.Add ( diff ) ;
    }

    return ret ;
}

void TRestrictionIdentifier::addRestrictionLane ( wxString en , TVector *v )
{
    wxArrayInt ai = getRestrictionFragments ( en , v ) ;
    TGelLane l ;
    if ( v->isLinear() ) l.add ( v->getSequenceLength() , 10 , wxString::Format ( txt("f_orig") , v->getSequenceLength() ) ) ;
    for ( int a = 0 ; a < ai.GetCount() ; a++ )
    {
        l.add ( ai[a] , 100 ) ;
    }
    l.name = v->getName() ;
    lanes.push_back ( l ) ;
}


void TRestrictionIdentifier::recalcEnzymes()
{

    int a ;
    wxArrayString enzyme_names ;
    myapp()->frame->LS->getEnzymesInGroup ( group_list->GetStringSelection() , enzyme_names ) ;
    enzyme_names.Sort() ;

    enzymes_list->Clear() ;
    for ( a = 0 ; a < enzyme_names.GetCount() ; a++ )
    {
        wxString en = enzyme_names[a] ;
        if ( !check4separation ( en ) ) continue ;
        enzymes_list->Append ( en ) ;
    }

    if ( enzymes_list->GetCount() > 0 ) enzymes_list->SetSelection ( 0 ) ;
    wxCommandEvent event ;
    OnEnzymeListChange(event) ;
}

void TRestrictionIdentifier::listDNA ()
{
    int a , b ;

    // Remember which ones were checked
    vector <MyChild*> was_checked ;
    for ( a = 0 ; a < dna_items.size() ; a++ )
    {
        if ( !dna_list->IsChecked ( a ) ) continue ;
        was_checked.push_back ( dna_items[a] ) ;
    }

    // Regenerate visual and pointer list
    dna_items.clear() ;
    dna_list->Clear() ;
	for ( a = 0 ; a < myapp()->frame->children.GetCount() ; a++ )
		{
 		if ( myapp()->frame->children[a]->def != _T("dna") ) continue ;
 		dna_items.push_back ( (MyChild*) myapp()->frame->children[a] ) ;
 		dna_list->Append ( myapp()->frame->children[a]->getName() ) ;
		}

	// Re-check previously checked ones
	for ( a = 0 ; a < was_checked.size() ; a++ )
	{
        for ( b = 0 ; b < dna_items.size() ; b++ )
        {
            if ( dna_items[b] != was_checked[a] ) continue ;
            dna_list->Check ( a ) ;
            break ;
        }
    }
}

bool TRestrictionIdentifier::check4separation ( wxString en )
{
    int a , b ;
    vector <wxArrayInt> vai ;
    for ( a = 0 ; a < dna_list->GetCount() ; a++ )
    {
        if ( !dna_list->IsChecked ( a ) ) continue ; // Not checked
        TVector *v = dna_items[a]->vec ;
        if ( !v ) continue ; // Paranoia
        wxArrayInt ai = getRestrictionFragments ( en , v ) ;
        for ( b = 0 ; b < vai.size() ; b++ )
        {
            if ( !arrays_are_separate ( vai[b] , ai ) ) return false ;
        }
        vai.push_back ( ai ) ;
    }
    return true ;
}

bool TRestrictionIdentifier::arrays_are_separate ( const wxArrayInt &a1 , const wxArrayInt &a2 )
{
    long bp , percent ;
    bp_list->GetStringSelection().ToLong ( &bp ) ;
    percent_list->GetStringSelection().ToLong ( &percent ) ;

    if ( arrays_differ ( a1 , a2 , bp , percent ) ) return true ;
    if ( arrays_differ ( a2 , a1 , bp , percent ) ) return true ;
    return false ;
}

bool TRestrictionIdentifier::arrays_differ ( const wxArrayInt &a1 , const wxArrayInt &a2 , int bp , int percent )
{
    int a , b ;
    for ( a = 0 ; a < a1.GetCount() ; a++ )
    {
        bool dupe = false ;
        int low = a1[a] - a1[a] * percent / 100 ;
        if ( low > a1[a] - bp ) low = a1[a] - bp ;

        int high = a1[a] + a1[a] * percent / 100 ;
        if ( high < a1[a] + bp ) high = a1[a] + bp ;

        for ( b = 0 ; b < a2.GetCount() ; b++ )
        {
            if ( a2[b] < low || a2[b] > high ) continue ;
            dupe = true ;
            break ;
        }

        if ( !dupe ) return true ;
    }
    return false ;
}


void TRestrictionIdentifier::otherChildrenChanged ()
{
    if ( !running ) return ;
    listDNA () ;
    recalcEnzymes() ;
}

//_____________________________________________________________________________________

/** \brief Handles the Restriction Identifier menu event
*/
void MyFrame::OnRestrictionIdentifier(wxCommandEvent& event)
    {
    TRestrictionIdentifier *subframe = new TRestrictionIdentifier ( getCommonParent() , txt("m_restriction_identifier") ) ;

    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon(_T("chrt_icn")));
#elif __WXMAC__
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;
    subframe->showName() ;

    mainTree->addChild ( subframe , TYPE_MISC ) ;
    setChild ( subframe ) ;
    activateChild ( children.GetCount()-1 ) ;

//    return subframe ;
    }
