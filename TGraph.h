/** \file
	\brief Contains the TGraph class and its helper classes
*/
#ifndef _T_GRAPH_H_
#define _T_GRAPH_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/notebook.h>
#include <wx/grid.h>

typedef vector <float> floatArray ;
typedef vector <wxArrayString> stringField ;

class TGraph ;
class TGraphData ;
class TGraphScale ;
class TGraphDisplay ;

class TGraphData
	{
	public :
 	TGraphData ( TGraphDisplay *d = NULL ) ;
 	void Add ( float x , float y ) ;
 	void SetScales ( TGraphScale *_sx , TGraphScale *_sy ) ;
 	void AutoScale () ;
 	
 	void drawit ( wxDC &dc ) ;
 	void DrawSymbol ( wxDC &dc , wxString symbol , int x , int y , int size = 4 ) ;

 	wxString name ;
 	wxString pointStyle ;
 	floatArray dx , dy ;
	TGraphDisplay *display ;
	TGraphScale *sx , *sy ;
	wxColour col ;
	vector <wxPoint> drawn ;
	bool selected ;
	} ;    
	
class TGraphScale
	{
	public :
 	TGraphScale ( float _min = 0 , 
 					float _max = 0 , 
  		  			bool _horizontal = false , 
      	  			bool _left = true , 
            		wxString _name = "" , 
              		wxString _unit = "" , 
              		wxColour _col = *wxBLACK ,
              		wxString _type = "linear" ) ;
	int GetWidth () ;
	void drawit ( wxDC &dc , wxRect &r , wxRect &inner ) ;
	void DrawMark ( wxDC &dc , float p , wxRect &ir , wxString text , bool big = false ) ;
	int GetRealCoord ( float f , wxRect &inner ) ;
	wxRect CalcInternalRect ( wxRect &r ) ;
 	
 	wxRect outline ;
 	float min , max ;
	float top , bottom ;
	bool horizontal , left , selected ;
	wxString name , unit , type ;
	wxColour col ;
	} ;    
	
class TGraphDisplay : public wxPanel
	{
 	public :
 	TGraphDisplay ( wxWindow *parent , int id = -1 ) ;
 	void init () ;
 	
 	stringField readCSVfile ( wxString filename ) ;
 	void setupPhotometerGraph ( const stringField &sf ) ;
 	void SetupDummy () ;

 	void AutoScale () ;
 	void drawit ( wxDC &dc ) ;
 	void showLegend ( wxDC &dc ) ;
 	
    void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    void OnEvent(wxMouseEvent& event) ;
    void OnSwapSides(wxCommandEvent &event) ;
 	
 	vector <TGraphScale*> scales ;
 	vector <TGraphData*> data ;
 	TGraph *g ;
 	
 	TGraphScale *old_scale ;
 	TGraphData *old_data ;
 	
 	wxRect inner ; ///< The place where the data is drawn
 	wxRect lr ; ///< The legend rectangle
 	static wxColour prettyColor ;

    DECLARE_EVENT_TABLE()
	} ;    

/**	\class TGraph
	\brief The calculator module
*/
class TGraph : public ChildBase
    {
    public :
    TGraph(wxWindow *parent, const wxString& title) ; ///< Constructor
    ~TGraph () ; ///< Destructor
        
    void initme () ; ///< Initialization
    virtual wxString getName () ; ///< Returns the module name

    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler

    private :
    wxNotebook *nb ; ///< Pointer to the wxNotebook structure that holds the submodules
    TGraphDisplay *gd ;

    DECLARE_EVENT_TABLE()
    } ;


#endif

