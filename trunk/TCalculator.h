/** \file
	\brief Contains the TImageDisplay class, as well as the TMyImagePanel helper class
*/
#ifndef _T_CALCULATOR_H_
#define _T_CALCULATOR_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/notebook.h>
#include <wx/grid.h>

#define HTML_PRINT_PREVIEW 1
#define HTML_PRINT 2

/**	\class TGridBasic
	\brief The base class for each calculator submodule (grid/sheet)
*/
class TGridBasic : public wxGrid
    {
    public :
    TGridBasic ( wxWindow *parent , int id = -1 ) ; ///< Constructor

    virtual void recalc () ; ///< Recalculates the submodule
    virtual void OnCellChanged(wxGridEvent& event) ; ///< Cell-has-changed event
    virtual void cleanup () ; ///< Reset the submodule
    virtual void gridSetEditable ( int y , int x ) ; ///< Makes a cell editable
    virtual void gridSetEntry ( int y , int x , wxString title , wxString value = "0" , wxString unit = "" ) ; ///< Sets a cell group
    virtual void gridSetValue ( int y , int x , wxString value , wxString unit = "" ) ; ///< Sets a cell group
    virtual void print ( int mode ) ; ///< Prints the submodule
    virtual double getDouble ( int y , int x ) ; ///< Returns the value of a cell as a floating-point number
    
    protected :
    bool calculating ; ///< Are we currently (re)calculating?
    } ;
    
/**	\class TGridLigation
	\brief The ligation calculator submodule
*/
class TGridLigation : public TGridBasic
    {
    public :
    TGridLigation ( wxWindow *parent , int id = -1 ) ; ///< Constructor

    virtual void init () ; ///< Initialization
    virtual void recalc () ; ///< Recalculates the submodule
    virtual void OnSelectCell(wxGridEvent& event) ; ///< Selection changed event handler
    
    DECLARE_EVENT_TABLE()
    } ;

/**	\class TGridDNA
	\brief The DNA concentration/purity calculator submodule
*/
class TGridDNA : public TGridBasic
    {
    public :
    TGridDNA ( wxWindow *parent , int id = -1 ) ; ///< Constructor

    virtual void init () ; ///< Initialization
    virtual void recalc () ; ///< Recalculates the submodule
    virtual void OnSelectCell(wxGridEvent& event) ; ///< Selection changed event handler
    
    DECLARE_EVENT_TABLE()
    } ;

/**	\class TGridProtein
	\brief The protein concentration calculator submodule
*/
class TGridProtein : public TGridBasic
    {
    public :
    TGridProtein ( wxWindow *parent , int id = -1 ) ; ///< Constructor

    virtual void init () ; ///< Initialization
    virtual void recalc () ; ///< Recalculates the submodule
    virtual void OnSelectCell(wxGridEvent& event) ; ///< Selection changed event handler
    
    DECLARE_EVENT_TABLE()
    } ;

/**	\class TGridData
	\brief The codon table calculator submodule
*/
class TGridData : public TGridBasic
    {
    public :
    TGridData ( wxWindow *parent , int id = -1 ) ; ///< Constructor

    virtual void init () ; ///< Initialization
    virtual void recalc () ; ///< Recalculates the submodule
    virtual void OnSelectCell(wxGridEvent& event) ; ///< Selection changed event handler
    
    DECLARE_EVENT_TABLE()
    } ;

/**	\class TCalculator
	\brief The calculator module
*/
class TCalculator : public ChildBase
    {
    public :
    TCalculator(wxWindow *parent, const wxString& title) ; ///< Constructor
    ~TCalculator () ; ///< Destructor
    
    void initme () ; ///< Initialization
    virtual wxString getName () ; ///< Returns the module name

//    virtual void OnClose(wxCloseEvent& event) ;
    virtual void OnSeqPrint(wxCommandEvent& event) ; ///< Print event handler
    virtual void OnPrintPreview(wxCommandEvent& event) ; ///< Print preview event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler
    
    wxNotebook *nb ; ///< Pointer to the wxNotebook structure that holds the submodules
    TGridLigation *ligGrid ; ///< Pointer to the ligation submodule
    TGridDNA *ligDNA ; ///< Pointer to the DNA submodule
    TGridProtein *prot ; ///< Pointer to the protein submodule
    TGridData *data ; ///< Pointer to the data submodule

    DECLARE_EVENT_TABLE()
    } ;

#endif
