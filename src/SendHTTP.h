/** \file
    \brief Contains the myExternal class
*/
#ifndef _SENDHTTP_H_
#define _SENDHTTP_H_

class wxString ;
class wxProgressDialog ;

/// This class is a wrapper around HTTP GET commands
class myExternal
    {
    public :
    myExternal () ; ///< Constructor
    ~myExternal () {} ; ///< Destructor

    int copyFile ( const wxString& url , const wxString& file , int _t = -1 ) ; ///< Copy a file from URL to local
    wxString getText ( const wxString& url ) ; ///< Get the text at an URL
//  virtual wxString getPostText

    wxProgressDialog *pd ; ///< Pointer to a process dialog

    private :
    int copyFileLocal ( const wxString& url , const wxString& file ) ; ///< Local file copy
    wxString getTextLocal ( const wxString& url ) ; ///< Local get text
    int copyFileHTTP ( const wxString& _url , const wxString& _file ) ; ///< HTTP file copy
    wxString getTextHTTP ( const wxString& url ) ; ///< HTTP get text
    wxString getTextFTP ( const wxString& server , const wxString& dir , const wxString& file ) ; ///< FTP get text

    int targetSize ; ///< File size, if known
    } ;

#endif
