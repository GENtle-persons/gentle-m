#ifndef _COOLCANVAS_H_
#define _COOLCANVAS_H_

#include "PlasmidCanvas.h"
//#include "main.h"

class CoolCanvas : public PlasmidCanvas
    {
    public :
    CoolCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDrawCircular(wxDC& dc) ;
    virtual int x2D ( int x , int y , int z ) ;
    virtual int y2D ( int x , int y , int z ) ;
    virtual wxPoint xy2D ( int x , int y , int z ) ;
    } ;


#endif

