#include "TCalculator.h"

BEGIN_EVENT_TABLE(TCalculator, MyChildBase)
    EVT_MENU(SEQ_PRINT, TCalculator::OnSeqPrint)
    EVT_MENU(MDI_PRINT_REPORT,TCalculator::OnPrintPreview)
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

BEGIN_EVENT_TABLE(TGridLigation, wxGrid)
   EVT_GRID_EDITOR_HIDDEN(TGridLigation::OnCellChanged)
   EVT_GRID_CELL_CHANGE(TGridLigation::OnCellChanged)
   EVT_GRID_SELECT_CELL(TGridLigation::OnSelectCell)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TGridDNA, wxGrid)
   EVT_GRID_EDITOR_HIDDEN(TGridDNA::OnCellChanged)
   EVT_GRID_CELL_CHANGE(TGridDNA::OnCellChanged)
   EVT_GRID_SELECT_CELL(TGridDNA::OnSelectCell)
END_EVENT_TABLE()

TCalculator::TCalculator(wxWindow *parent, const wxString& title) 
    : ChildBase(parent, title)
    {
    def = "CALCULATOR" ;
    vec = NULL ;
    }

TCalculator::~TCalculator ()
    {
    delete nb ;
    }
    
void TCalculator::OnSeqPrint(wxCommandEvent& event)
    {
    TGridBasic *g = (TGridBasic*) nb->GetPage ( nb->GetSelection () ) ;
    g->print ( HTML_PRINT ) ;
    }
    
void TCalculator::OnPrintPreview(wxCommandEvent& event)
    {
    TGridBasic *g = (TGridBasic*) nb->GetPage ( nb->GetSelection () ) ;
    g->print ( HTML_PRINT_PREVIEW ) ;
    }


void TCalculator::initme ()
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
    
    menu_bar->FindItem ( SEQ_PRINT ) -> SetText ( txt("m_print") ) ;
    menu_bar->FindItem ( MDI_PRINT_REPORT ) -> SetText ( txt("m_print_preview") ) ;
    

    nb = new wxNotebook ( this , -1 ) ;

    ligGrid = new TGridLigation ( nb , -1 ) ;
    nb->AddPage ( ligGrid , txt("t_lig_grid") ) ;
    ligGrid->init () ;

    ligDNA = new TGridDNA ( nb , -1 ) ;
    nb->AddPage ( ligDNA , txt("t_lig_dna") ) ;
    ligDNA->init () ;

    wxBoxSizer *v0 = new wxBoxSizer ( wxVERTICAL ) ;
//    v0->Add ( toolbar , 0 , wxEXPAND , 5 ) ;
    v0->Add ( nb , 1 , wxEXPAND , 5 ) ;
    SetSizer ( v0 ) ;
    v0->Fit ( this ) ;

    myapp()->frame->setChild ( this ) ;
    Maximize () ;
    Activate () ;
    }

string TCalculator::getName ()
    {
    return txt("t_calculator") ;
    }

//------------------------------ TGridLigation

TGridLigation::TGridLigation ( wxWindow *parent , int id ) : TGridBasic ( parent , id ) {}

void TGridLigation::OnSelectCell(wxGridEvent& event)
    {
    int x = event.GetCol() ;
    int y = event.GetRow() ;
    if ( GetBatchCount() > 0 ) event.Skip() ;
    else if ( x != 1 || y > 6 ) event.Veto() ;
    else event.Skip() ;
    }
    
void TGridLigation::init ()
    {
    BeginBatch() ;
    CreateGrid ( 10 , 9 ) ;
    EnableGridLines ( false ) ;
    SetGridCursor ( 0 , 1 ) ;
    DisableDragGridSize() ;
    DisableDragRowSize() ;
    DisableDragColSize() ;
    
    cleanup () ;
        
    gridSetEntry ( 0 , 0 , txt("t_calc_lig_1") , "0" , "ng" ) ;
    gridSetEntry ( 1 , 0 , txt("t_calc_lig_2") , "0" , "bp" ) ;
    gridSetEntry ( 2 , 0 , txt("t_calc_lig_3") , "0" , "ng/µl" ) ;
    gridSetEntry ( 3 , 0 , txt("t_calc_lig_4") , "0" , "bp" ) ;
    gridSetEntry ( 4 , 0 , txt("t_calc_lig_5") , "0" , "ng/µl" ) ;
    gridSetEntry ( 5 , 0 , txt("t_calc_lig_6") , "0" , " : 1" ) ;

    SetCellValue ( 0 , 3 , "<=>" ) ;
    SetCellValue ( 1 , 3 , "<=>" ) ;
    SetCellValue ( 3 , 3 , "<=>" ) ;
    SetCellValue ( 8 , 3 , "<=>" ) ;
    SetCellValue ( 9 , 3 , "<=>" ) ;
    SetCellValue ( 8 , 6 , "<=>" ) ;
    SetCellValue ( 9 , 6 , "<=>" ) ;
    
    gridSetValue ( 0 , 4 , "0" , txt("t_calc_lig_7") ) ;
    gridSetValue ( 1 , 4 , "0" , txt("t_calc_lig_8") ) ;
    gridSetValue ( 3 , 4 , "0" , txt("t_calc_lig_8") ) ;
    
    SetCellValue ( 7 , 0 , txt("t_calc_lig_9") ) ;
    SetCellValue ( 8 , 0 , txt("t_calc_lig_10") ) ;
    SetCellValue ( 9 , 0 , txt("t_calc_lig_11") ) ;
    gridSetValue ( 8 , 1 , "0" , txt("t_calc_lig_12") ) ;
    gridSetValue ( 9 , 1 , "0" , txt("t_calc_lig_12") ) ;
    gridSetValue ( 8 , 4 , "0" , txt("t_calc_lig_13") ) ;
    gridSetValue ( 9 , 4 , "0" , txt("t_calc_lig_13") ) ;

    SetCellValue ( 7 , 7 , txt("t_calc_lig_14") ) ;
    gridSetValue ( 8 , 7 , "0" , txt("t_calc_lig_15") ) ;
    gridSetValue ( 9 , 7 , "0" , txt("t_calc_lig_15") ) ;
    SetCellTextColour ( 8 , 7 , *wxRED ) ;
    SetCellTextColour ( 9 , 7 , *wxRED ) ;
        
    AutoSizeColumns () ;
    SetColSize ( 1 , 60 ) ;
    EndBatch () ;
    
    recalc () ;
    }
    
void TGridLigation::recalc ()
    {
    double mb = 610 ;
    double ten3 = 1000 , ten6 = ten3*ten3 , ten9 = ten6*ten3 , ten15 = ten9*ten6 ;
    double t_g , v_l , v_c , i_l , i_c , i2v ;
    GetCellValue(0,1).ToDouble ( &t_g ) ;
    GetCellValue(1,1).ToDouble ( &v_l ) ;
    GetCellValue(2,1).ToDouble ( &v_c ) ;
    GetCellValue(3,1).ToDouble ( &i_l ) ;
    GetCellValue(4,1).ToDouble ( &i_c ) ;
    GetCellValue(5,1).ToDouble ( &i2v ) ;
    
    double t_ng = t_g / ten9 ;
    double v_gm = mb * v_l ;
    double i_gm = mb * i_l ;
    
    double v_dna_mol = v_gm + i2v * i_gm ;
    if ( v_dna_mol != 0 ) v_dna_mol = ten15 * t_ng / v_dna_mol ;
    double i_dna_mol = i2v * v_dna_mol ;
    
    double v_dna_ng = v_dna_mol * v_gm / ten6 ;
    double i_dna_ng = i_dna_mol * i_gm / ten6 ;
    
    double v_sol = 0 ;
    if ( v_c != 0 ) v_sol = v_dna_ng / v_c ;

    double i_sol = 0 ;
    if ( i_c != 0 ) i_sol = i_dna_ng / i_c ;
    
    SetCellValue ( 0 , 4 , wxString::Format("%0.8f", t_ng) ) ;
    SetCellValue ( 1 , 4 , wxString::Format("%0.0f", v_gm) ) ;
    SetCellValue ( 3 , 4 , wxString::Format("%0.0f", i_gm) ) ;
    
    SetCellValue ( 8 , 1 , wxString::Format("%0.2f", v_dna_mol) ) ;
    SetCellValue ( 9 , 1 , wxString::Format("%0.2f", i_dna_mol) ) ;

    SetCellValue ( 8 , 4 , wxString::Format("%0.2f", v_dna_ng) ) ;
    SetCellValue ( 9 , 4 , wxString::Format("%0.2f", i_dna_ng) ) ;

    SetCellValue ( 8 , 7 , wxString::Format("%0.2f", v_sol) ) ;
    SetCellValue ( 9 , 7 , wxString::Format("%0.2f", i_sol) ) ;

    AutoSizeColumn ( 4 ) ;
    AutoSizeColumn ( 7 ) ;
    }

//------------------------------ TGridDNA

TGridDNA::TGridDNA ( wxWindow *parent , int id ) : TGridBasic ( parent , id ) {}

void TGridDNA::OnSelectCell(wxGridEvent& event)
    {
    int x = event.GetCol() ;
    int y = event.GetRow() ;
    if ( GetBatchCount() > 0 ) event.Skip() ;
    else if ( x != 1 || y > 4 ) event.Veto() ;
    else event.Skip() ;
    }
    
void TGridDNA::init ()
    {
    BeginBatch() ;
    CreateGrid ( 7 , 3 ) ;
    EnableGridLines ( false ) ;
    SetGridCursor ( 0 , 1 ) ;
    DisableDragGridSize() ;
    DisableDragRowSize() ;
    DisableDragColSize() ;
    
    cleanup () ;
        
    gridSetEntry ( 0 , 0 , txt("t_calc_dna_1") , "0" ) ;
    gridSetEntry ( 1 , 0 , txt("t_calc_dna_2") , "0" ) ;
    gridSetEntry ( 2 , 0 , txt("t_calc_dna_3") , "0" , "(1:X)" ) ;
    gridSetEntry ( 3 , 0 , txt("t_calc_dna_4") , "50" , txt("t_calc_dna_5") ) ;

    gridSetValue ( 5 , 1 , "0" , txt("t_calc_dna_6") ) ;
    gridSetValue ( 6 , 1 , "0" , txt("t_calc_dna_7") ) ;
    
    SetCellValue ( 5 , 0 , txt("t_calc_dna_8") ) ;
    SetCellValue ( 6 , 0 , txt("t_calc_dna_9") ) ;

    SetCellTextColour ( 5 , 1 , *wxRED ) ;
    SetCellTextColour ( 6 , 1 , *wxRED ) ;
        
    AutoSizeColumns () ;
    SetColSize ( 1 , 60 ) ;
    EndBatch () ;
    
    recalc () ;
    }
    
void TGridDNA::recalc ()
    {
    double a260 , a280 , v , u ;
    GetCellValue(0,1).ToDouble ( &a260 ) ;
    GetCellValue(1,1).ToDouble ( &a280 ) ;
    GetCellValue(2,1).ToDouble ( &v ) ;
    GetCellValue(3,1).ToDouble ( &u ) ;
    
    double conc = a260 * v * u / 1000 ;
    double pure = 0 ;
    if ( a280 != 0 ) pure = a260 / a280 ;
    
    SetCellValue ( 5 , 1 , wxString::Format("%0.2f", conc) ) ;
    SetCellValue ( 6 , 1 , wxString::Format("%0.2f", pure) ) ;
    }

// ----

TGridBasic::TGridBasic ( wxWindow *parent , int id )
    : wxGrid ( parent , id )
    {
    }

void TGridBasic::gridSetEditable ( int y , int x )
    {
    SetReadOnly ( y , x , false ) ;
    SetCellBackgroundColour( y , x , wxColour(187,187,255) ) ;
    SetCellAlignment ( y , x , wxALIGN_RIGHT , wxALIGN_TOP ) ;
    }

void TGridBasic::gridSetEntry ( int y , int x , wxString title , wxString value , wxString unit )
    {
    SetCellValue ( y , x , title ) ;
    if ( value != "" ) SetCellValue ( y , x+1 , value ) ;
    if ( unit != "" ) SetCellValue ( y , x+2 , unit ) ;
    gridSetEditable ( y , x+1 ) ;
    }


void TGridBasic::gridSetValue ( int y , int x , wxString value , wxString unit )
    {
    SetCellValue ( y , x , value ) ;
    SetCellAlignment ( y , x , wxALIGN_RIGHT , wxALIGN_CENTRE ) ;
    if ( unit != "" ) SetCellValue ( y , x+1 , unit ) ;
    }

void TGridBasic::recalc ()
    {
    }
    
void TGridBasic::OnCellChanged(wxGridEvent& event)
    {
    recalc () ;
    }

void TGridBasic::cleanup ()
    {
    int x , y ;
    for ( x = 0 ; x < GetNumberCols() ; x++ )
        {
        SetColLabelValue ( x , "" ) ;
        for ( y = 0 ; y < GetNumberRows() ; y++ )
           SetReadOnly ( y , x , true ) ;
        }
    for ( y = 0 ; y < GetNumberRows() ; y++ )
        SetRowLabelValue ( y , "" ) ;
        
    SetRowLabelSize ( 0 ) ;
    SetColLabelSize ( 0 ) ;
    }
    
void TGridBasic::print ( int mode )
    {
    int cols = GetNumberCols() ;
    int rows = GetNumberRows() ;
    int col , row ;
    wxString html ;
    html = "<HTML><BODY><TABLE border=0>" ;

    int colwidth = 0 ;
    for ( col = 0 ; col < cols ; col++ )
        colwidth += GetColSize ( col ) ;

    for ( row = 0 ; row < rows ; row++ )
        {
        html += "<tr>" ;
        for ( col = 0 ; col < cols ; col++ )
           {
           int w = GetColSize ( col ) ;
           wxColour bgc = GetCellBackgroundColour ( row , col ) ;
           wxColour tc = GetCellTextColour ( row , col ) ;
           int ha , va ;
           GetCellAlignment ( row , col , &ha , &va ) ;
           wxString align = (ha==wxALIGN_CENTRE)?"center":((ha==wxALIGN_RIGHT)?"right":"") ;
           wxString valign = (va==wxALIGN_CENTRE)?"center":((ha==wxALIGN_BOTTOM)?"bottom":"top") ;
           if ( align != "" ) align = " align=" + align ;
           if ( valign != "" ) valign = " valign=" + valign ;
           wxString tct = wxString::Format ( "color='#%2x%2x%2x'" , 
                                            tc.Red() , 
                                            tc.Green() , 
                                            tc.Blue() ) ;       
           tct.Replace ( " " , "0" ) ;                                 
           tct = "<font " + tct + ">" ;    
           wxString bgct = wxString::Format ( "#%2x%2x%2x" ,
                                            bgc.Red() , 
                                            bgc.Green() , 
                                            bgc.Blue() ) ;
           bgct.Replace ( " " , "0" ) ;                                 
           html += wxString::Format ( "<td width='%d%%' bgcolor='%s'%s%s>" , 
                                            w * 100 / colwidth , 
                                            bgct.c_str() ,
                                            align.c_str() ,
                                            valign.c_str() ) ;
           html += tct ;
           html += GetCellValue ( row , col ) ;
           html += "</font></td>" ;
           }
        html += "</tr>" ;
        }
    html += "</TABLE></BODY></HTML>" ;

    wxDateTime now = wxDateTime::Now() ;
    myapp()->frame->html_ep->SetHeader ( "<p align=right><u>" + now.Format() + "</u></p>" ) ;
    myapp()->frame->html_ep->SetFooter ( "<p align=right>@PAGENUM@/@PAGESCNT@</p>" ) ;

    if ( mode == HTML_PRINT_PREVIEW )
        {
        myapp()->frame->html_ep->PreviewText ( html ) ;
        }
    else if ( mode == HTML_PRINT )
        {
        myapp()->frame->html_ep->PrinterSetup () ;
        }
    }
    
