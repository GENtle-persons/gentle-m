#ifndef _SENDHTTP_H_
#define _SENDHTTP_H_

class wxString ;
class wxProgressDialog ;

class myExternal
	{
	public :
	myExternal () ;
	virtual int copyFile ( wxString url , wxString file , int _t = -1 ) ;
	virtual wxString getText ( wxString url ) ;
//	virtual wxString getPostText
	
	wxProgressDialog *pd ;
	
	private :
	virtual int copyFileLocal ( wxString url , wxString file ) ;
	virtual wxString getTextLocal ( wxString url ) ;
	virtual int copyFileHTTP ( wxString _url , wxString _file ) ;
	virtual wxString getTextHTTP ( wxString url ) ;
	
	int targetSize ;
	} ;    

#endif
