#ifndef _SENDHTTP_H_
#define _SENDHTTP_H_

class wxString ;
class wxProgressDialog ;

/* HTTPRequest: Structure that returns the HTTP headers and message
                from the request*/
//typedef struct
//{ 
//	LPSTR headers;/* Pointer to HTTP headers */
//	LPSTR message;/* Pointer to the HTTP message */
//	long messageLength;/* Length of the message */
//} HTTPRequest;

//int SendHTTP(LPCSTR url,LPCSTR headers,BYTE *post,DWORD postLength,HTTPRequest *req) ;

class myExternal
	{
	public :
	myExternal () ;
	virtual int copyFile ( wxString url , wxString file , int _t = -1 ) ;
	virtual wxString getText ( wxString url ) ;
	
	wxProgressDialog *pd ;
	
	private :
	virtual int copyFileLocal ( wxString url , wxString file ) ;
	virtual wxString getTextLocal ( wxString url ) ;
	virtual int copyFileHTTP ( wxString _url , wxString _file ) ;
	virtual wxString getTextHTTP ( wxString url ) ;
	
	int targetSize ;
	} ;    

#endif
