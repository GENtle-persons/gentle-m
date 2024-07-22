#include "CGview.h"

/// \brief This class contains the settings dialog for the CGviewer export function
class CGdialog : public wxDialog
    {
    public :
    CGdialog ( wxWindow *parent, const wxString& title , CGview *_p ) ; ///< Constructor
    virtual void OnCharHook ( wxKeyEvent& event ) ; ///< Key event handler
    virtual void OnRunCGviewer ( wxCommandEvent &event ) ; ///< Checkbox "Run CGviewer" handler
    virtual void OnChooseJar ( wxCommandEvent &event ) ; ///< Button "Choose CGviewer JAR file" handler
    virtual void OnChooseBackgroundColor  ( wxCommandEvent &event ) ; ///< Button "Background color" handler

    wxTextCtrl *width , *height , *cgviewapp ;
    wxCheckBox *useDefaultColors , *runcgview , *runimageapp , *showrestrictionsites , *showgc ;
    wxButton *choosejar ;
    wxChoice *imagetypes ;
    CGview *p ; ///< The calling (parent) CGview class

    DECLARE_EVENT_TABLE()
    } ;

BEGIN_EVENT_TABLE(CGdialog, wxDialog )
//  EVT_BUTTON(wxID_OK,CGdialog::OnOK)
//  EVT_BUTTON(wxID_CANCEL,CGdialog::OnCancel)
    EVT_BUTTON(CGVIEW_CHOOSE_JAR,CGdialog::OnChooseJar)
    EVT_BUTTON(CGVIEW_CHOOSE_BACKGROUND_COLOR,CGdialog::OnChooseBackgroundColor)
    EVT_CHECKBOX(CGVIEW_RUN_CGVIEWER,CGdialog::OnRunCGviewer)
    EVT_CHAR_HOOK(CGdialog::OnCharHook)
END_EVENT_TABLE()

CGdialog::CGdialog ( wxWindow *parent, const wxString& title , CGview *_p )
    : wxDialog ( parent , TSD , title , wxDefaultPosition , wxSize ( 400 , 200 ) )
    {
    p = _p ;
    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
    wxBoxSizer *h1 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h2 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h3 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h4 = new wxBoxSizer ( wxHORIZONTAL ) ;
    wxBoxSizer *h5 = new wxBoxSizer ( wxHORIZONTAL ) ;

    width = new wxTextCtrl ( this , -1 , wxString::Format ( _T("%d") , p->width ) ) ;
    height = new wxTextCtrl ( this , -1 , wxString::Format ( _T("%d") , p->height ) ) ;
    h1->Add ( new wxStaticText ( this , -1 , txt("t_cgview_image_dimensions") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( width , 1 , wxEXPAND , 5 ) ;
    h1->Add ( new wxStaticText ( this , -1 , _T(" x ") ) , 0 , wxEXPAND , 5 ) ;
    h1->Add ( height , 1 , wxEXPAND , 5 ) ;

    useDefaultColors = new wxCheckBox ( this , -1 , txt("t_cgview_use_default_colors") ) ;

    wxString appname = p->cgviewapp ;
    runcgview = new wxCheckBox ( this , CGVIEW_RUN_CGVIEWER , txt("t_cgview_run_cgview") ) ;
    cgviewapp = new wxTextCtrl ( this , -1 , appname ) ;
    choosejar = new wxButton ( this , CGVIEW_CHOOSE_JAR , _T("..") ) ;
    h3->Add ( runcgview , 0 , wxEXPAND , 5 ) ;
    h3->Add ( cgviewapp , 1 , wxEXPAND , 5 ) ;
    h3->Add ( choosejar , 0 , wxALL , 5 ) ;

    runimageapp = new wxCheckBox ( this , -1 , txt("t_cgview_run_image_app") ) ;
    imagetypes = new wxChoice ( this , -1 ) ;
    h4->Add ( new wxStaticText ( this , -1 , txt("t_cgview_imagetypes") ) , 0 , wxEXPAND , 5 ) ;
    h4->Add ( imagetypes , 0 , wxEXPAND , 5 ) ;
    h4->Add ( runimageapp , 0 , wxEXPAND , 5 ) ;

    showrestrictionsites = new wxCheckBox ( this , -1 , txt("t_cgview_showrestrictionsites") ) ;
    showgc = new wxCheckBox ( this , -1 , txt("t_cgview_showgc") ) ;
    wxButton *bgc = new wxButton ( this , CGVIEW_CHOOSE_BACKGROUND_COLOR , txt("t_cgview_backgroundcolor") ) ;
    h5->Add ( showrestrictionsites , 0 , wxEXPAND , 5 ) ;
    h5->Add ( showgc , 0 , wxEXPAND , 5 ) ;
    h5->Add ( bgc , 0 , wxEXPAND , 5 ) ;


    h2->Add ( new wxButton ( this , wxID_OK , txt("b_ok" ) ) , 0 , wxALL , 5 ) ;
    h2->Add ( new wxButton ( this , wxID_CANCEL , txt("b_cancel" ) ) , 0 , wxALL , 5 ) ;

    v0->Add ( h1 , 0 , 0 ) ;
    v0->Add ( useDefaultColors , 0 , 0 ) ;
    v0->Add ( h5 , 1 , 0 ) ;
    v0->Add ( h3 , 1 , wxEXPAND ) ;
    v0->Add ( h4 , 1 , 0 ) ;
    v0->Add ( h2 , 0 , wxALIGN_CENTER ) ;

    useDefaultColors->SetValue ( p->useDefaultColors ) ;
    runcgview->SetValue ( p->runCGview ) ;
    runimageapp->SetValue ( p->runimageapp ) ;
    showrestrictionsites->SetValue ( p->showrestrictionsites ) ;
    showgc->SetValue ( p->showgc ) ;

    imagetypes->Append ( _T("PNG") ) ;
    imagetypes->Append ( _T("JPG") ) ;
    imagetypes->Append ( _T("SVG") ) ;
    imagetypes->Append ( _T("SVGZ") ) ;
    imagetypes->SetStringSelection ( p->imageformat.Upper() ) ;

    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;
    Center () ;

    wxCommandEvent dummy ;
    OnRunCGviewer ( dummy ) ;
    }

void CGdialog::OnRunCGviewer ( wxCommandEvent &event )
    {
    bool b = runcgview->GetValue() ;
    runimageapp->Enable ( b ) ;
    cgviewapp->Enable ( b ) ;
    choosejar->Enable ( b ) ;
    imagetypes->Enable ( b ) ;
    }

void CGdialog::OnChooseJar ( wxCommandEvent &event )
    {
    wxFileDialog d ( this , txt("t_cgview_choose_jar") , cgviewapp->GetLabel() , _T("") , _T("cgview.jar|cgview.jar") , wxFD_OPEN ) ;
    if ( wxID_OK != d.ShowModal() ) return ;
    cgviewapp->SetLabel ( d.GetPath() ) ;
    }

void CGdialog::OnChooseBackgroundColor  ( wxCommandEvent &event )
    {
    p->backgroundColor = wxGetColourFromUser ( this , p->backgroundColor ) ;
    }

void CGdialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) EndModal ( wxID_CANCEL ) ; // OnCancel ( ev ) ;
    else if ( k == WXK_RETURN ) EndModal ( wxID_OK ) ; // OnOK ( ev ) ;
    else event.Skip() ;
    }

//*********

CGview::CGview ( TVector *_v )
    {
    v = _v ;
    itemsShown = false ;
    width = myapp()->frame->LS->getOption ( _T("CGVIEW_WIDTH") , 800 ) ;
    height = myapp()->frame->LS->getOption ( _T("CGVIEW_HEIGHT") , 600 ) ;
    radius = myapp()->frame->LS->getOption ( _T("CGVIEW_RADIUS") , 160 ) ;
    useDefaultColors = myapp()->frame->LS->getOption ( _T("CGVIEW_USEDEFAULTCOLORS") , true ) ;
    runCGview = myapp()->frame->LS->getOption ( _T("CGVIEW_RUNCGVIEW") , false ) ;
    runimageapp = myapp()->frame->LS->getOption ( _T("CGVIEW_RUNIMAGEAPP") , true ) ;
    imageformat = myapp()->frame->LS->getOption ( _T("CGVIEW_IMAGEFORMAT") , _T("png") ) ;
    cgviewapp = myapp()->frame->LS->getOption ( _T("CGVIEW_CGVIEWJAR") , _T("") ) ;
    showrestrictionsites = myapp()->frame->LS->getOption ( _T("CGVIEW_SHOWRESTRICTIONSITES") , true ) ;
    showgc = myapp()->frame->LS->getOption ( _T("CGVIEW_SHOWGC") , false ) ;

    wxString bgcol = myapp()->frame->LS->getOption ( _T("CGVIEW_BACKGROUNDCOLOR") , _T("255255255") ) ;
    backgroundColor = wxColour ( atoi ( bgcol.Mid ( 0 , 3 ) . mb_str() ) ,
                                 atoi ( bgcol.Mid ( 3 , 3 ) . mb_str() ) ,
                                 atoi ( bgcol.Mid ( 6 , 3 ) . mb_str() ) ) ;
    }

wxString CGview::getXML()
    {
    wxBeginBusyCursor() ;
    wxString ret ;
    wxString bgcol = RGB2string ( backgroundColor ) ;
    ret += _T("<?xml version='1.0' encoding='ISO-8859-1'?>\n") ;
    ret += wxString::Format ( _T("<cgview backgroundColor='%s' sequenceLength='%d' height='%d' width='%d'>\n\n") , //backboneRadius='%d'
//                            radius ,
                              bgcol.c_str() , v->getSequenceLength() , height , width ) ;

    // Adding GC%
    if ( showgc )
        {
        ret += _T(" <featureSlot strand='direct'>\n") ;
        int l = v->getSequenceLength() ;
        int nob = v->showGC() ;
        for ( int a = 0 ; a < nob ; a++ )
            {
            int at = 0 , gc = 0 , other = 0 ;
            for ( int b = l * a / nob ; b < l * ( a + 1 ) / nob ; b++ )
                {
                char c = v->getSequenceChar ( b ) ;
                if ( c == 'A' || c == 'T' ) at++ ;
                else if ( c == 'G' || c == 'C' ) gc++ ;
                else other++ ;
                }
            int sum = at + gc + other ;
            if ( sum == 0 ) continue ;
            int per = gc * 100 / sum ;
            wxColour col ;
            makeGCcolor ( per , col ) ;
            wxString icol = RGB2string ( col ) ;
            int start = l*a/nob+1 ;
            int stop = l*(a+1)/nob ;
            if ( stop > l ) stop = l ;
            ret += _T("  <feature color='") + icol + _T("' decoration='arc'>") ;
            ret += _T("   <featureRange ") ;
            ret += wxString::Format ( _T("start='%d' stop='%d'/>\n") , start , stop ) ;
            ret += _T("  </feature>\n") ;
            }
        ret += _T(" </featureSlot>\n") ;

        // Now for the GC legend...
        ret += _T(" <legend position='upper-left'>\n") ;
        for ( int a = 0 ; a <= 100 ; a += 25 )
            {
            wxString text = wxString::Format ( _T("%3d%% GC") , a ) ;
            wxColour col ;
            makeGCcolor ( a , col ) ;
            ret += _T("  <legendItem text=\"") + text ;
            ret += _T("\" drawSwatch='true' swatchColor='") + RGB2string ( col ) ;
            ret += _T("'/>\n") ;
            }
        ret += _T(" </legend>\n\n") ;
        }

    // Adding features
    ret += addXMLfeatureSlot ( 1 ) ; // Forward
    ret += addXMLfeatureSlot ( 0 ) ; // Direct
    ret += addXMLfeatureSlot ( -1 ) ; // Reverse

    // Adding restriction sites
    if ( showrestrictionsites )
        {
        ret += _T(" <featureSlot strand='direct'>\n") ;
        for ( int a = 0 ; a < v->rc.size() ; a++ )
            {
            wxString label = v->rc[a].getDisplayName() ;
            wellform ( label ) ;
            ret += _T("  <feature color='black' decoration='arc' label=\"") ;
            ret += label + _T("\">\n") ;
            ret += _T("   <featureRange ") ;
            ret += wxString::Format ( _T("start='%d' stop='%d'/>\n") , v->rc[a].getPos() , v->rc[a].getPos() ) ;
            ret += _T("  </feature>\n") ;
            }
        ret += _T(" </featureSlot>\n") ;
        }

    // Adding legend
    if ( useDefaultColors && itemsShown ) // Only if there is a true match between type and feature
        {
        ret += _T(" <legend position='upper-right'>\n") ;
        for ( int a = 0 ; a < used_types.size() ; a++ )
            {
            if ( used_types[a] == 0 ) continue ; // Not used
            wxString text = txt(wxString::Format ( _T("itemtype%d") ,a)) ;
            wellform ( text ) ;
            ret += _T("  <legendItem text=\"") + text ;
            ret += _T("\" drawSwatch='true' swatchColor='") + getColorName ( a ) ;
            ret += _T("'/>\n") ;
            }
        ret += _T(" </legend>\n\n") ;
        }

    ret += _T("</cgview>\n") ;
    wxEndBusyCursor() ;
    return ret ;
    }

wxString CGview::addXMLfeatureSlot ( int dir )
    {
    wxString ret ;
    vector <int> i2 ;

    // Extracting items with the right direction
    for ( int a = 0 ; a < v->items.size() ; a++ )
        {
        if ( v->items[a].getDirection() == dir && v->items[a].isVisible() )
            {
            i2.push_back ( a ) ;
            }
        }

    // Sorting by size, largest ones first
    for ( int a = 1 ; a < i2.size() ; a++ )
        {
        if ( v->getItemLength ( i2[a] ) > v->getItemLength ( i2[a-1] ) )
            {
            int tmp = i2[a] ;
            i2[a] = i2[a-1] ;
            i2[a-1] = tmp ;
            a-= 2 ;
            if ( a < 0 ) a = 0 ;
            }
        }

    // Arranging in non-overlapping circles
    vector < vector<int> > vvi ;
    for ( int a = 0 ; a < i2.size() ; a++ )
        {
        int fits = -1 ;
        for ( int b = 0 ; b < vvi.size() && fits == -1 ; b++ )
            {
            int c ;
            for ( c = 0 ; c < vvi[b].size() && !itemOverlap ( v->items[vvi[b][c]] , v->items[i2[a]] ) ; c++ ) ;
            if ( c == vvi[b].size() ) fits = b ;
            }
        if ( fits == -1 )
            {
            fits = vvi.size() ;
            vvi.push_back ( vector <int> () ) ;
            }
        vvi[fits].push_back ( i2[a] ) ;
        }

    for ( int a = 0 ; a < vvi.size() ; a++ )
        {
        wxString oret = ret ;
        ret = _T("") ;
        for ( int b = 0 ; b < vvi[a].size() ; b++ )
            {
            TVectorItem *i = &v->items[vvi[a][b]] ;
            if ( i->getDirection() != dir ) continue ; // Wrong direction
            wxString label = i->name ;
            if ( label.IsEmpty() ) label = i->desc ;
            label = label.BeforeFirst ( '\n' ) ;
            wellform ( label ) ;
            ret += _T("  <feature ") ;
            wxString color = useDefaultColors ? getColorName ( i->getType() ) : getColorName ( i ) ;
            ret += _T("color='") + color + _T("' ") ;
            ret += _T("decoration='") ;
            if ( i->getDirection() ==  1 ) ret += _T("clockwise-arrow") ;
            if ( i->getDirection() ==  0 ) ret += _T("arc") ;
            if ( i->getDirection() == -1 ) ret += _T("counterclockwise-arrow") ;
            ret += _T("' ") ;
            ret += _T("label=\"") + label + _T("\">\n") ;
            ret += wxString::Format ( _T("   <featureRange start='%d' stop='%d'/>\n") , i->from , i->to ) ;
            ret += _T("  </feature>\n") ;
            while ( used_types.size() <= i->getType() ) used_types.push_back ( 0 ) ;
            used_types[i->getType()]++ ;
            itemsShown = true ;
            }

        if ( ret != _T("") )
            {
            wxString sdir ;
            if ( dir ==  1 ) sdir = _T("direct") ;
            if ( dir ==  0 ) sdir = _T("direct") ;
            if ( dir == -1 ) sdir = _T("reverse") ;
            ret = _T(" <featureSlot strand='") + sdir + _T("'>\n") + ret + _T(" </featureSlot>\n\n") ;
            }
        ret = oret + ret ;
        }
    return ret ;
    }

wxString CGview::getColorName ( const int type )
    {
    if ( type == VIT_GENE ) return _T("purple") ;
    if ( type == VIT_CDS ) return _T("blue") ;
    if ( type == VIT_REP_ORI ) return _T("fuchsia") ;
    if ( type == VIT_PROMOTER ) return _T("green") ;
    if ( type == VIT_TERMINATOR ) return _T("red") ;
//    if ( type == VIT_MISC ) return "" ;
    if ( type == VIT_PROT_BIND ) return _T("rgb(255,215,0)") ;
//    if ( type == VIT_ORI_T ) return "" ;
//    if ( type == VIT_SEQUENCING ) return "" ;
    return _T("grey") ;
    }

wxString CGview::getColorName ( const TVectorItem * const i )
    {
    return RGB2string ( i->getFontColor() ) ;
    }

bool CGview::itemOverlap ( const TVectorItem &i1 , const TVectorItem &i2 ) const // not static
    {
    int f1 = i1.from , f2 = i2.from ;
    int t1 = i1.to , t2 = i2.to ;
    if ( f1 > t1 ) t1 += v->getSequenceLength() ; // not static because of access to v
    if ( f2 > t2 ) t2 += v->getSequenceLength() ;
    if ( t2 < f1 ) return false ;
    if ( f2 > t1 ) return false ;
    return true ;
    }

void CGview::wellform ( wxString &s )
    {
    s.Replace ( _T("\"") , _T("") ) ;
    s.Replace ( _T("&") , _T("&amp;") ) ;
    }

bool CGview::runSettingsDialog ()
    {
    CGdialog d ( myapp()->frame->GetActiveChild() , txt("t_cgview_settings") , this ) ;
    if ( wxID_OK != d.ShowModal() ) return false ; // Cancelled

    // Retrieve new settings from the dialog
    width = atoi ( d.width->GetLabel().mb_str() ) ;
    height = atoi ( d.height->GetLabel().mb_str() ) ;
    useDefaultColors = d.useDefaultColors->GetValue() ;
    runCGview = d.runcgview->GetValue() ;
    cgviewapp = d.cgviewapp->GetLabel() ;
    runimageapp = d.runimageapp->GetValue() ;
    imageformat = d.imagetypes->GetStringSelection().Lower() ;
    showrestrictionsites = d.showrestrictionsites->GetValue() ;
    showgc = d.showgc->GetValue() ;


    // Saving options
    myapp()->frame->LS->startRecord() ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_WIDTH") , width ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_HEIGHT") , height ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_RADIUS") , radius ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_USEDEFAULTCOLORS") , useDefaultColors ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_RUNCGVIEW") , runCGview ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_RUNIMAGEAPP") , runimageapp ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_IMAGEFORMAT") , imageformat ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_CGVIEWJAR") , cgviewapp ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_SHOWRESTRICTIONSITES") , showrestrictionsites ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_SHOWGC") , showgc ) ;
    myapp()->frame->LS->setOption ( _T("CGVIEW_BACKGROUNDCOLOR") , wxString::Format ( _T("%3d%3d%3d") ,
                                    backgroundColor.Red() , backgroundColor.Green() , backgroundColor.Blue() ) ) ;
    myapp()->frame->LS->endRecord() ;

    return true ;
    }

void CGview::postProcess ( wxString filename )
    {
    if ( !runCGview ) return ;
    if ( !wxFileExists ( cgviewapp ) ) return ;
    wxString outfile = filename.BeforeLast ( '.' ) + _T(".") + imageformat ;
    wxString command = _T("java -jar \"") + cgviewapp +
                       _T("\" -i \"") + filename +
                       _T("\" -o \"") + outfile +
                       _T("\" -f ") + imageformat ;
    if ( 0 != wxExecute ( command , wxEXEC_SYNC ) ) return ; // Something's wrong
    if ( !runimageapp ) return ; // Don't show the image
    if ( !wxFileExists ( outfile ) ) return ; // Image was not generated

    command = myapp()->getFileFormatCommand ( imageformat , outfile ) ;
    if ( command.IsEmpty() ) return ;
    wxExecute ( command ) ;
    }

void CGview::makeGCcolor ( const int _percent , wxColour &col )
    {
    int red , green , blue ;
    int percent ( _percent ) ;

    if ( percent < 50 )
        {
        red = 255 * ( 50 - percent ) / 50 ;
        green = 255 * percent / 50 ;
        blue = 0 ;
        }
    else
        {
        percent -= 50 ;
        red = 0 ;
        green = 255 * ( 50 - percent ) / 50 ;
        blue = 255 * percent / 50 ;
        }
    col.Set ( red , green , blue ) ;
    }

wxString CGview::RGB2string ( const wxColour& col )
    {
    return RGB2string ( col.Red() , col.Green() , col.Blue() ) ;
    }

wxString CGview::RGB2string ( const int red , const int green , const int blue )
    {
    return wxString::Format ( _T("rgb(%d,%d,%d)") , red , green , blue ) ;
    }

