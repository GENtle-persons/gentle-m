#ifndef _TIMGreader_H_
#define _TIMGreader_H_

#include "main.h"

#define IMGTYPE_MISC 0
#define IMGTYPE_TEXT 1

class TIMGitem
    {
    public :
    void draw ( wxDC &dc , int x1 , int y1 , int x2 , int y2 ) ;
    int xx ( int x ) ;
    int yy ( int y ) ;
    
    int type ;
    wxPoint p1 , p2 ;
    wxString s ;
    wxSize orig , cur ;
    wxPoint off ;
    int font_size ;
    wxString font_name ;
    } ;

class TIMGreader
    {
    public :
    TIMGreader () { buffer = NULL ; output = NULL ; } ;
    ~TIMGreader () ;
    void readFile ( wxString fn ) ;
    int getInt ( int adr ) ;
    wxImage makeImage () ;
    
    vector <TIMGitem> items ;
    
    private :
    unsigned char *output ;
    unsigned char *buffer ;
    long size;
    int w , h ;
    } ;
    
#endif

