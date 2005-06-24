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
	virtual ~myExternal () {} ; ///< Destructor
	
	virtual int copyFile ( wxString url , wxString file , int _t = -1 ) ; ///< Copy a file from URL to local
	virtual wxString getText ( wxString url ) ; ///< Get the text at an URL
//	virtual wxString getPostText
	
	wxProgressDialog *pd ; ///< Pointer to a process dialog
	
	private :
	virtual int copyFileLocal ( wxString url , wxString file ) ; ///< Local file copy
	virtual wxString getTextLocal ( wxString url ) ; ///< Local get text
	virtual int copyFileHTTP ( wxString _url , wxString _file ) ; ///< HTTP file copy
	virtual wxString getTextHTTP ( wxString url ) ; ///< HTTP get text
	
	int targetSize ; ///< File size, if known
	} ;    

#endif
