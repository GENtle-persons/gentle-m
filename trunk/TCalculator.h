#ifndef _T_CALCULATOR_H_
#define _T_CALCULATOR_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/notebook.h>
#include <wx/grid.h>

    
    

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



class TCalculator : public ChildBase
    {
    public :
    TCalculator(wxMDIParentFrame *parent, const wxString& title) ;
    ~TCalculator () ;
    
    void initme ( MyApp *_app ) ;
    virtual string getName () ;

    virtual void OnClose(wxCloseEvent& event) ;
    
    private :
    wxNotebook *nb ;
    TGridLigation *ligGrid ;
    TGridDNA *ligDNA ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
