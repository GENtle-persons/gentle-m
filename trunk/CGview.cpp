#include "CGview.h"

/// \brief This class contains the settings dialog for the CGviewer export function
class CGdialog : public wxDialog
	{
	public :
    CGdialog ( wxWindow *parent, const wxString& title , CGview *_p ) ; ///< Constructor
    virtual void OnCharHook ( wxKeyEvent& event ) ; ///< Key event handler
    virtual void OnRunCGviewer ( wxCommandEvent &event ) ; ///< Checkbox "Run CGviewer" handler
    virtual void OnChooseJar ( wxCommandEvent &event ) ; ///< Button "Choose CGviewer JAR file" handler
    
    wxTextCtrl *width , *height , *cgviewapp ;
    wxCheckBox *useDefaultColors , *runcgview , *runimageapp ;
    wxButton *choosejar ;
    wxChoice *imagetypes ;
    CGview *p ; ///< The calling (parent) CGview class

    DECLARE_EVENT_TABLE()
	} ;    

BEGIN_EVENT_TABLE(CGdialog, wxDialog )
    EVT_BUTTON(wxID_OK,CGdialog::OnOK)
    EVT_BUTTON(wxID_CANCEL,CGdialog::OnCancel)
    EVT_BUTTON(CGVIEW_CHOOSE_JAR,CGdialog::OnChooseJar)
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
    
    width = new wxTextCtrl ( this , -1 , wxString::Format ( "%d" , p->width ) ) ;
    height = new wxTextCtrl ( this , -1 , wxString::Format ( "%d" , p->height ) ) ;
    h1->Add ( new wxStaticText ( this , -1 , txt("t_cgview_image_dimensions") ) , 0 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( width , 1 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( new wxStaticText ( this , -1 , " x " ) , 0 , wxEXPAND|wxALL , 5 ) ;
    h1->Add ( height , 1 , wxEXPAND|wxALL , 5 ) ;
    
    useDefaultColors = new wxCheckBox ( this , -1 , txt("t_cgview_use_default_colors") ) ;
    
    wxString appname = p->cgviewapp ;
    runcgview = new wxCheckBox ( this , CGVIEW_RUN_CGVIEWER , txt("t_cgview_run_cgview") ) ;
    cgviewapp = new wxTextCtrl ( this , -1 , appname ) ;
    choosejar = new wxButton ( this , CGVIEW_CHOOSE_JAR , ".." ) ;
    h3->Add ( runcgview , 0 , wxEXPAND|wxALL , 5 ) ;
    h3->Add ( cgviewapp , 1 , wxEXPAND|wxALL , 5 ) ;
    h3->Add ( choosejar , 0 , wxALL , 5 ) ;
    
    runimageapp = new wxCheckBox ( this , -1 , txt("t_cgview_run_image_app") ) ;
    imagetypes = new wxChoice ( this , -1 ) ;
    h4->Add ( new wxStaticText ( this , -1 , txt("t_cgview_imagetypes") ) , 0 , wxEXPAND|wxALL , 5 ) ;
    h4->Add ( imagetypes , 0 , wxEXPAND|wxALL , 5 ) ;
    h4->Add ( runimageapp , 0 , wxEXPAND|wxALL , 5 ) ;
    
    h2->Add ( new wxButton ( this , wxID_OK , txt("b_ok" ) ) , 0 , wxALL , 5 ) ;
    h2->Add ( new wxButton ( this , wxID_CANCEL , txt("b_cancel" ) ) , 0 , wxALL , 5 ) ;
    
    v0->Add ( h1 , 0 , 0 ) ;
    v0->Add ( useDefaultColors , 0 , 0 ) ;
    v0->Add ( h3 , 1 , wxEXPAND ) ;
    v0->Add ( h4 , 1 , 0 ) ;
    v0->Add ( h2 , 0 , wxALIGN_CENTER ) ;
    
    useDefaultColors->SetValue ( p->useDefaultColors ) ;
    runcgview->SetValue ( p->runCGview ) ;
    runimageapp->SetValue ( p->runimageapp ) ;
    
    imagetypes->Append ( "PNG" ) ;
    imagetypes->Append ( "JPG" ) ;
    imagetypes->Append ( "SVG" ) ;
    imagetypes->Append ( "SVGZ" ) ;
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
	}    

void CGdialog::OnChooseJar ( wxCommandEvent &event )
	{
    wxFileDialog d ( this , txt("t_cgview_choose_jar") , cgviewapp->GetLabel() , "" , "cgview.jar|cgview.jar" , wxOPEN ) ;
    if ( wxID_OK != d.ShowModal() ) return ;
    cgviewapp->SetLabel ( d.GetPath() ) ;
	}
    
void CGdialog::OnCharHook ( wxKeyEvent& event )
    {
    int k = event.GetKeyCode () ;
    wxCommandEvent ev ;
    if ( k == WXK_ESCAPE ) OnCancel ( ev ) ;
    else if ( k == WXK_RETURN ) OnOK ( ev ) ;
    else event.Skip() ;
    }

//*********

CGview::CGview ( TVector *_v )
	{
 	v = _v ;
 	width = myapp()->frame->LS->getOption ( "CGVIEW_WIDTH" , 800 ) ;
 	height = myapp()->frame->LS->getOption ( "CGVIEW_HEIGHT" , 600 ) ;
 	radius = myapp()->frame->LS->getOption ( "CGVIEW_RADIUS" , 160 ) ;
 	useDefaultColors = myapp()->frame->LS->getOption ( "CGVIEW_USEDEFAULTCOLORS" , true ) ;
 	runCGview = myapp()->frame->LS->getOption ( "CGVIEW_RUNCGVIEW" , false ) ;
 	runimageapp = myapp()->frame->LS->getOption ( "CGVIEW_RUNIMAGEAPP" , true ) ;
 	imageformat = myapp()->frame->LS->getOption ( "CGVIEW_IMAGEFORMAT" , "png" ) ;
 	cgviewapp = myapp()->frame->LS->getOption ( "CGVIEW_CGVIEWJAR" , "" ) ;
	}    
	
wxString CGview::getXML()
	{
	wxBeginBusyCursor() ;
 	wxString ret ;
 	ret += "<?xml version='1.0' encoding='ISO-8859-1'?>\n" ;
 	ret += wxString::Format ( "<cgview backboneRadius='%d' sequenceLength='%d' height='%d' width='%d'>\n\n" ,
 						radius ,
 						v->getSequenceLength() , 
 						height ,
 						width ) ;
	
	// Adding features
	ret += addXMLfeatureSlot ( 1 ) ; // Forward
	ret += addXMLfeatureSlot ( 0 ) ; // Direct
	ret += addXMLfeatureSlot ( -1 ) ; // Reverse
	
	// Adding legend
	if ( useDefaultColors ) // Only if there is a true match between type and feature
		{
    	ret += " <legend position='upper-right'>\n" ;
    	for ( int a = 0 ; a < used_types.size() ; a++ )
    		{
     		if ( used_types[a] == 0 ) continue ; // Not used
     		wxString text = txt(wxString::Format("itemtype%d",a)) ;
     		wellform ( text ) ;
     		ret += "  <legendItem text=\"" + text ;
       		ret += "\" drawSwatch='true' swatchColor='" + getColorName ( a ) ;
       		ret += "'/>\n" ;
    		}    
    	ret += " </legend>\n\n" ;
     	}   	
	
	ret += "</cgview>\n" ;
	wxEndBusyCursor() ;
 	return ret ;
	}
     
wxString CGview::addXMLfeatureSlot ( int dir )
	{
	int a , b , c ;
	wxString ret ;
	vector <int> i2 ;

	// Extracting items with the right direction
	for ( a = 0 ; a < v->items.size() ; a++ )
		{
  		if ( v->items[a].getDirection() == dir &&
    			v->items[a].isVisible() )
  			i2.push_back ( a ) ;
  		}  		
	
	// Sorting by size, largest ones first
	for ( a = 1 ; a < i2.size() ; a++ )
		{
  		if ( v->getItemLength ( i2[a] ) > v->getItemLength ( i2[a-1] ) )
  			{
	    	b = i2[a] ;
	    	i2[a] = i2[a-1] ;
	    	i2[a-1] = b ;
	    	a-= 2 ;
	    	if ( a < 0 ) a = 0 ;
  			}    
		}    
		
	// Arranging in non-overlapping circles
	vector < vector<int> > vvi ;
	for ( a = 0 ; a < i2.size() ; a++ )
		{
  		int fits = -1 ;
		for ( b = 0 ; b < vvi.size() && fits == -1 ; b++ )
			{
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
	
	for ( a = 0 ; a < vvi.size() ; a++ )
		{
  		wxString oret = ret ;
  		ret = "" ;
    	for ( b = 0 ; b < vvi[a].size() ; b++ )
    		{
      		TVectorItem *i = &v->items[vvi[a][b]] ;
      		if ( i->getDirection() != dir ) continue ; // Wrong direction
      		wxString label = i->name ;
      		if ( label.IsEmpty() ) label = i->desc ;
      		label = label.BeforeFirst ( '\n' ) ;
      		wellform ( label ) ;
      		ret += "  <feature " ;
      		wxString color = useDefaultColors ? getColorName ( i->getType() ) : getColorName ( i ) ;
      		ret += "color='" + color + "' " ;
    		ret += "decoration='" ;
    		if ( i->getDirection() ==  1 ) ret += "clockwise-arrow" ;
    		if ( i->getDirection() ==  0 ) ret += "arc" ;
    		if ( i->getDirection() == -1 ) ret += "counterclockwise-arrow" ;
    		ret += "' " ;
    		ret += "label=\"" + label + "\">\n" ;
    		ret += wxString::Format ( "   <featureRange start='%d' stop='%d'/>\n" , i->from , i->to ) ;
      		ret += "  </feature>\n" ;
      		while ( used_types.size() <= i->getType() ) used_types.push_back ( 0 ) ;
      		used_types[i->getType()]++ ;
    		}    
    	
    	if ( ret != "" )
    		{
        	wxString sdir ;
        	if ( dir ==  1 ) sdir = "direct" ;
        	if ( dir ==  0 ) sdir = "direct" ;
        	if ( dir == -1 ) sdir = "reverse" ;
        	ret = " <featureSlot strand='" + sdir + "'>\n" + ret + " </featureSlot>\n\n" ;
         	}   
        ret = oret + ret ;	
     	} 	
	return ret ;
	}
     
wxString CGview::getColorName ( int type )
	{
    if ( type == VIT_GENE ) return "purple" ;
    if ( type == VIT_CDS ) return "blue" ;
    if ( type == VIT_REP_ORI ) return "fuchsia" ;
    if ( type == VIT_PROMOTER ) return "green" ;
    if ( type == VIT_TERMINATOR ) return "red" ;
//    if ( type == VIT_MISC ) return "" ;
    if ( type == VIT_PROT_BIND ) return "rgb(255,215,0)" ;
//    if ( type == VIT_ORI_T ) return "" ;
//    if ( type == VIT_SEQUENCING ) return "" ;
    return "grey" ;
    }
    
wxString CGview::getColorName ( TVectorItem *i )
	{
	wxColour c = i->getFontColor() ;
	return wxString::Format ( "rgb(%d,%d,%d)" , c.Red() , c.Green() , c.Blue() ) ;
	}    

bool CGview::itemOverlap ( TVectorItem &i1 , TVectorItem &i2 )
	{
	int f1 = i1.from , f2 = i2.from ;
	int t1 = i1.to , t2 = i2.to ;
	if ( f1 > t1 ) t1 += v->getSequenceLength() ;
	if ( f2 > t2 ) t2 += v->getSequenceLength() ;
	if ( t2 < f1 ) return false ;
	if ( f2 > t1 ) return false ;
	return true ;
	}

void CGview::wellform ( wxString &s )
	{
	s.Replace ( "\"" , "" ) ;
	s.Replace ( "&" , "&amp;" ) ;
	}

bool CGview::runSettingsDialog ()
	{
	CGdialog d ( myapp()->frame->GetActiveChild() , txt("t_cgview_settings") , this ) ;
	if ( wxID_OK != d.ShowModal() ) return false ; // Cancelled
	
	// Retrieve new settings from the dialog
	width = atoi ( d.width->GetLabel() ) ;
	height = atoi ( d.height->GetLabel() ) ;
	useDefaultColors = d.useDefaultColors->GetValue() ;
	runCGview = d.runcgview->GetValue() ;
	cgviewapp = d.cgviewapp->GetLabel() ;
	runimageapp = d.runimageapp->GetValue() ;
	imageformat = d.imagetypes->GetStringSelection().Lower() ;


	// Saving options
	myapp()->frame->LS->startRecord() ;
 	myapp()->frame->LS->setOption ( "CGVIEW_WIDTH" , width ) ;
 	myapp()->frame->LS->setOption ( "CGVIEW_HEIGHT" , height ) ;
 	myapp()->frame->LS->setOption ( "CGVIEW_RADIUS" , radius ) ;
 	myapp()->frame->LS->setOption ( "CGVIEW_USEDEFAULTCOLORS" , useDefaultColors ) ;
 	myapp()->frame->LS->setOption ( "CGVIEW_RUNCGVIEW" , runCGview ) ;
 	myapp()->frame->LS->setOption ( "CGVIEW_RUNIMAGEAPP" , runimageapp ) ;
 	myapp()->frame->LS->setOption ( "CGVIEW_IMAGEFORMAT" , imageformat ) ;
 	myapp()->frame->LS->setOption ( "CGVIEW_CGVIEWJAR" , cgviewapp ) ;
	myapp()->frame->LS->endRecord() ;

	return true ;
	}

void CGview::postProcess ( wxString filename )
	{
	if ( !runCGview ) return ;
	if ( !wxFileExists ( cgviewapp ) ) return ;
	wxString outfile = filename.BeforeLast ( '.' ) + "." + imageformat ;
	wxString command = "java -jar \"" + cgviewapp + 
 						"\" -i \"" + filename + 
 						"\" -o \"" + outfile +
       					"\" -f " + imageformat ;
	if ( 0 != wxExecute ( command , wxEXEC_SYNC ) ) return ; // Something's wrong
	if ( !runimageapp ) return ; // Don't show the image
	if ( !wxFileExists ( outfile ) ) return ; // Image was not generated
	
    command = myapp()->getFileFormatCommand ( imageformat , outfile ) ;    
    if ( command.IsEmpty() ) return ;
    wxExecute ( command ) ;
	}

