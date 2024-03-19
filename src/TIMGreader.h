/** \file
    \brief Contains the TIMGreader class, and its TIMGitem helper class
*/
#ifndef _TIMGreader_H_
#define _TIMGreader_H_

#include "main.h"

#define IMGTYPE_MISC 0
#define IMGTYPE_TEXT 1

/// Annotation drawing class
class TIMGitem
    {
    public :
    void draw ( wxDC &dc , int x1 , int y1 , int x2 , int y2 ) ; ///< Draws the annotation
    int xx ( int x ) ; ///< X-coordinate conversion
    int yy ( int y ) ; ///< Y-coordinate conversion

    int type ; ///< Annotation type
    wxPoint p1 , p2 ;
    wxString s ; ///< Annotation
    wxSize orig , cur ;
    wxPoint off ; ///< Offset
    int font_size ; ///< Font size
    wxString font_name ; ///< Font name
    } ;

/// This class can read the ancient BioRad IMG format (most of the time...)
class TIMGreader
    {
    public :
    TIMGreader () { buffer = NULL ; output = NULL ; } ; ///< Constructor
    ~TIMGreader () ; ///< Destructor
    void readFile ( const wxString& fn ) ; ///< Reads the IMG file
    int getInt ( const int adr ) const ; ///< Gets an integer value from a dertain adress in the buffer
    wxImage makeImage () ; ///< Returns the parsed IMG file as a wxImage

    vector <TIMGitem> items ; ///< Annotation items

    private :
    unsigned char *output ; ///< Output memory
    unsigned char *buffer ; ///< Buffer memory
    long size; ///< Buffer size

    /// Image width
    int w , h ; ///< Image height
    } ;

#endif

