#ifndef _TDOTPLOT_H_
#define _TDOTPLOT_H_

#include "TGraph.h"
#include <wx/dcmemory.h>

class TDotPlot ;

/**	\class TDotPlotPanel
	\brief The dot plot drawing panel
*/
class TDotPlotPanel : public wxScrolledWindow
    {
    public :
    TDotPlotPanel ( wxWindow *_parent ) ;
    void Run () ;
    void Update () ;
    virtual void OnDraw(wxDC& dc) ; ///< Draw routine
    virtual void OnPaint ( wxPaintEvent &event ) ; ///< Paint event handler
    virtual void OnMouse(wxMouseEvent& event) ; ///< Mouse event handler
    virtual void OnOpenSequence1(wxCommandEvent& event) ; ///< Open sequence 1 event handler
    virtual void OnOpenSequence2(wxCommandEvent& event) ; ///< Open sequence 2 event handler
    virtual void OnCopy(wxCommandEvent& event) ; ///< Copy event handler
    virtual void OnSave(wxCommandEvent& event) ; ///< Save event handler

    private :
    void Recalc () ; ///< Recalculate dot-plot
    void Redraw ( wxDC &dc ) ; ///< Draw the visible section of the dot-plot from memory DC
    void Draw2Memory () ; ///< Generate the memory DC
    int CheckWindow ( int x , int y ) ; ///< check for valid scanning window in sequences

    TDotPlot *parent ;
    MyChild *seq1 , *seq2 ;
    long window , mismatch ;
    vector <char> data ;
    int len1 , len2 ;
    int x1 , y1 , offx , offy ;
    wxPoint last_mouse_logical_position ;
    wxMemoryDC memdc ;
    wxBitmap membmp ;

    DECLARE_EVENT_TABLE()
    } ;

/**	\class TDotPlot
	\brief The dot plot drawing module
*/
class TDotPlot : public ChildBase
    {
    public :
    TDotPlot(wxWindow *parent, const wxString& title) ; ///< Constructor
    ~TDotPlot () ; ///< Destructor

    void initme () ; ///< Initialization
    virtual wxString getName () const ; ///< Returns the module name

    virtual void OnZoom(wxScrollEvent& event); ///< Zoom event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler
 	virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    virtual void OnSelectSeq1(wxCommandEvent& event) ; ///< event handler
    virtual void OnSelectSeq2(wxCommandEvent& event) ; ///< event handler
    virtual void OnSelectWindowSize(wxCommandEvent& event) ; ///< event handler
    virtual void OnSelectMismatches(wxCommandEvent& event) ; ///< event handler


    private :
    friend class TDotPlotPanel ;
    void update_sequence_lists () ;

    TDotPlotPanel *panel ;
    wxSlider *zoom ;
    wxChoice *seq1 , *seq2 , *window_size , *mismatch_limit ;
    wxCheckBox *open_seq1 ;

    DECLARE_EVENT_TABLE()
    } ;


#endif
