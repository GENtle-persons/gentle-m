/** \file
	\brief Contains the TImageDisplay class, as well as the TMyImagePanel helper class
*/
#ifndef _TIMAGE_DISPLAY_H_
#define _TIMAGE_DISPLAY_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>
#include <wx/spinctrl.h>
#include <wx/dirctrl.h>
#include <wx/image.h>

class TIMGreader ;
class TMyImagePanel ;
class TMyFileBox ;

/**	\class TImageDisplay
	\brief The image display module
*/
class TImageDisplay : public ChildBase
    {
    public :
    TImageDisplay(wxWindow *parent, const wxString& title) ; ///< Constructor
    ~TImageDisplay () ; ///< Destructor
    
    void initme () ; ///< Initialization
    virtual wxString getName () ; ///< Returns the image module name

    virtual void OnCB ( wxCommandEvent &event ) ; ///< Show labels event handler
    virtual void OnCBinvert ( wxCommandEvent &event ) ; ///< Invert image event handler
    virtual void OnDir ( wxCommandEvent &event ) ; ///< Choose directory event handler
    virtual void OnFile ( wxCommandEvent &event ) ; ///< File selection event handler
    
    void ShowDir ( wxString s ) ; ///< Displays the list of files in the chosen directory

    TIMGreader *r ; ///< Pointer to the BioRad image reader class
    
    private :    
    TMyImagePanel *right ; ///< Pointer to the image display class
    wxListBox *lb ; ///< The list box containing the file list
    wxButton *bu ; ///< The directory button
    wxCheckBox *cb ; ///< The "Show labels" check box
    wxCheckBox *invert ; ///< The "Invert image" check box
    
    DECLARE_EVENT_TABLE()
    } ;
    
/**	\class TMyImagePanel
	\brief This class represents the actual image displayed by TImageDisplay
*/
class TMyImagePanel : public wxPanel
    {
    public :
    TMyImagePanel ( wxWindow *parent , int id = -1 ) ; ///< Constructor
    ~TMyImagePanel () ; ///< Destructor

    virtual void OnDraw(wxDC& pdc) ; ///< The draw event handler
    virtual void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL) ; ///< Refresh the image display
    virtual void OnSize(wxSizeEvent &event) ; ///< The resize event handler
    virtual void OnPaint(wxPaintEvent& event) ; ///< The paint event handler

    virtual void OnEvent(wxMouseEvent& event); ///< The mouse event handler
    virtual void OnSaveAsBitmap(wxCommandEvent &event); ///< The "Save bitmap" context menu event handler
    virtual void OnCopy(wxCommandEvent &event); ///< The copy event handler
    virtual void OnPrint(wxCommandEvent &event); ///< The print event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< The dummy event handler
    
    virtual void WriteIntoBitmap(wxBitmap &bmp2) ; ///< Converts an image into a bitmap

    wxBitmap *bmp ; ///< Pointer to current bitmap structure
    wxImage i ; ///< The current image
    wxString dir , file ;
    bool printing , show_text , invert ;
    TImageDisplay *imdi ; ///< The parent image display module

    DECLARE_EVENT_TABLE()
    } ;
    
#endif
