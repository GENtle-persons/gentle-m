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

#define GRAPH_DRAW_SCALES 1
#define GRAPH_DRAW_MAP 2
#define GRAPH_DRAW_MINI 4
#define GRAPH_DRAW_ALL (GRAPH_DRAW_SCALES|GRAPH_DRAW_MAP)

class TGraphData
	{
	public :
 	TGraphData ( TGraphDisplay *d = NULL ) ;
 	void Add ( float x , float y ) ;
 	void SetScales ( TGraphScale *_sx , TGraphScale *_sy ) ;
 	void AutoScale () ;
 	unsigned long GetCount () ;
 	int Distance ( const wxPoint &pt ) ;
 	
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
	float GetVirtualCoordinate ( int i , wxRect &inner ) ;
	wxRect CalcInternalRect ( wxRect &r ) ;
	void Drag ( int delta ) ;
	float GetTotalWidth () { return max - min ; }
	float GetVisibleWidth () { return top - bottom ; }
 	
 	wxRect outline ;
 	wxRect last_inner ;
 	float min , max ;
	float top , bottom ;
	bool horizontal , left , selected ;
	wxString name , unit , type ;
	wxColour col ;
	int mark ;
	bool show_mark ;
	} ;    
	
class TGraphDisplay : public wxPanel
	{
 	public :
 	TGraphDisplay ( wxWindow *parent , int id = -1 ) ;
 	void init () ;
 	
 	stringField readCSVfile ( wxString filename ) ;
 	void setupPhotometerGraph ( const stringField &sf ) ;
 	void setupFluorimeterGraph ( const stringField &sf ) ;
 	void SetupDummy () ;
 	void SetZoom ( int _zx , int _zy ) ;

 	void AutoScale () ;
 	void drawit ( wxDC &dc , int mode = GRAPH_DRAW_ALL ) ;
 	void showLegend ( wxDC &dc ) ;
 	void showMiniature ( wxDC &dc ) ;
 	void showDraggingRect ( wxDC &dc ) ;
 	
 	void OnCharHook(wxKeyEvent& event) ;
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
 	int zx , zy ;
 	wxPoint mouse_pos ;
 	static wxColour prettyColor ;
 	wxRect draggingRect ;

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

    virtual void OnZoomX(wxScrollEvent& event); ///< Zoom event handler
    virtual void OnZoomY(wxScrollEvent& event); ///< Zoom event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler

    private :
    friend class TGraphDisplay ;
    wxNotebook *nb ; ///< Pointer to the wxNotebook structure that holds the submodules
    TGraphDisplay *gd ;
    wxSlider *zoom_x , *zoom_y ;
    wxCheckBox *zoom_linked ;

    DECLARE_EVENT_TABLE()
    } ;


#endif

