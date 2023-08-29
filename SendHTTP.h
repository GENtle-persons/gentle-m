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
	
	int copyFile ( wxString url , wxString file , int _t = -1 ) ; ///< Copy a file from URL to local
	wxString getText ( wxString url ) ; ///< Get the text at an URL
//	virtual wxString getPostText
	
	wxProgressDialog *pd ; ///< Pointer to a process dialog
	
	private :
	int copyFileLocal ( wxString url , wxString file ) ; ///< Local file copy
	wxString getTextLocal ( wxString url ) ; ///< Local get text
	int copyFileHTTP ( wxString _url , wxString _file ) ; ///< HTTP file copy
	wxString getTextHTTP ( wxString url ) ; ///< HTTP get text
	wxString getTextFTP ( wxString server , wxString dir , wxString file ) ; ///< FTP get text
	
	int targetSize ; ///< File size, if known
	} ;

#endif
