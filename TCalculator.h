#ifndef _T_CALCULATOR_H_
#define _T_CALCULATOR_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/notebook.h>
#include <wx/grid.h>

#define HTML_PRINT_PREVIEW 1
#define HTML_PRINT 2
    

class TGridBasic : public wxGrid
    {
    public :
    TGridBasic ( wxWindow *parent , int id = -1 ) ;

    virtual void recalc () ;
    virtual void OnCellChanged(wxGridEvent& event) ;
    virtual void cleanup () ;
    virtual void gridSetEditable ( int y , int x ) ;
    virtual void gridSetEntry ( int y , int x , wxString title , wxString value = "0" , wxString unit = "" ) ;
    virtual void gridSetValue ( int y , int x , wxString value , wxString unit = "" ) ;
    virtual void print ( int mode ) ;
    virtual double getDouble ( int y , int x ) ;
    } ;
    
class TGridLigation : public TGridBasic
    {
    public :
    TGridLigation ( wxWindow *parent , int id = -1 ) ;

    virtual void init () ;
    virtual void recalc () ;
    virtual void OnSelectCell(wxGridEvent& event) ;
    
    DECLARE_EVENT_TABLE()
    } ;

class TGridDNA : public TGridBasic
    {
    public :
    TGridDNA ( wxWindow *parent , int id = -1 ) ;

    virtual void init () ;
    virtual void recalc () ;
    virtual void OnSelectCell(wxGridEvent& event) ;
    
    DECLARE_EVENT_TABLE()
    } ;

class TGridProtein : public TGridBasic
    {
    public :
    TGridProtein ( wxWindow *parent , int id = -1 ) ;

    virtual void init () ;
    virtual void recalc () ;
    virtual void OnSelectCell(wxGridEvent& event) ;
    
    DECLARE_EVENT_TABLE()
    } ;



class TCalculator : public ChildBase
    {
    public :
    TCalculator(wxWindow *parent, const wxString& title) ;
    ~TCalculator () ;
    
    void initme () ;
    virtual wxString getName () ;

//    virtual void OnClose(wxCloseEvent& event) ;
    virtual void OnSeqPrint(wxCommandEvent& event) ;
    virtual void OnPrintPreview(wxCommandEvent& event) ;
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){};
    
    wxNotebook *nb ;
    TGridLigation *ligGrid ;
    TGridDNA *ligDNA ;
    TGridProtein *prot ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
