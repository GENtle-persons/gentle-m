/** \file
    \brief Contains the CGview class, which deals with CGview export
*/
#ifndef __CGview__
#define __CGview__

#include "main.h"

/// This class manages the CGview export
class CGview
    {
    public :
    CGview ( TVector *_v = NULL ) ; ///< Constructor
    ~CGview () {} ; ///< Destructor (empty)

    wxString getXML () ; ///< Generates the XML for the vector v and returns it as a wxString
    bool runSettingsDialog () ; ///< Runs the settings dialog prior to the actual XML generation
    void postProcess ( wxString filename ) ; ///< Runs CGview and the image viewer, if requested
     
    private :
    friend class CGdialog ;
    wxString addXMLfeatureSlot ( int dir ) ; ///< Adds the features for the given direction to the XML
    wxString getColorName ( int type ) ; ///< Returns XML item comor, based on the item type
    wxString getColorName ( TVectorItem *i ) ; ///< Returns XML item color, based on the item color used in GENtle
    bool itemOverlap ( TVectorItem &i1 , TVectorItem &i2 ) ; ///< Checks if two items overlap
    void wellform ( wxString &s ) ; ///< Fixes quotes and &amp; prior to XML generation
    void makeGCcolor ( int percent , wxColour &col ) ;
    wxString RGB2string ( wxColour col ) ;
    wxString RGB2string ( int red , int green , int blue ) ;
      
    TVector *v ; ///< Pointer to the TVector structure to export
    int width , height , radius ;
    bool useDefaultColors , runCGview , runimageapp , showrestrictionsites , showgc ;
    bool itemsShown ;
    wxColour backgroundColor ;
    wxString cgviewapp , imageformat ;
    vector <int> used_types ; ///< The item types used in this vector (internal use only)
    } ;

#endif
