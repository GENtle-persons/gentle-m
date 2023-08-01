/** \file
	\brief Contains the myExternal class
*/

#include "main.h"
#include "SendHTTP.h"
#include <wx/filesys.h>
#include <wx/protocol/http.h>
#include <wx/protocol/ftp.h>

myExternal::myExternal () { pd = NULL ; } ;

int myExternal::copyFileHTTP ( wxString _url , wxString _file )
{
    wxFileSystem fs ;
    wxFSFile *file = fs.OpenFile ( _url ) ;
    if ( !file ) return 1 ; // Error
    wxInputStream *in_stream = file->GetStream () ;
    if ( !in_stream ) // Error
    {
	delete file ;
	return 1 ;
    }

    wxFile out ( _file , wxFile::write ) ;
    if ( !out.IsOpened() ) // Error
    {
	delete file ;
	return 1 ;
    }
    while ( !in_stream->Eof() )
	{
	    char c = in_stream->GetC() ;
	    out.Write ( &c , 1 ) ;
	}
    out.Close () ;

    delete file ;    
    return 0 ; 
}

wxString myExternal::getTextHTTP ( wxString url ) 
{ 
    wxString ret ;
    wxFileSystem fs ;
    wxFSFile *file = fs.OpenFile ( url ) ;
    if ( !file ) return _T("") ; // Error
    wxInputStream *in_stream = file->GetStream () ;

    if ( !in_stream ) // Error
    	{
		delete file ;
		return _T("") ;
    	}

	 char n[1005] ;
	 while ( !in_stream->Eof() )
	 	{
		in_stream->Read ( n , 1000 ) ;
		n[in_stream->LastRead()] = 0 ;
		ret += wxString ( n , wxConvUTF8 ) ;
		}

    delete file ;
    return ret ; 
}

wxString myExternal::getTextFTP ( wxString server , wxString dir , wxString file )
	{
	wxString ret ;
	wxFTP ftp;

    if ( !ftp.Connect(server) ) return ret ;
    if ( !ftp.ChDir(dir) ) return ret ;

	int size = ftp.GetFileSize ( file ) ;
    wxInputStream *in = ftp.GetInputStream(file);
    if ( in )
	    {
		char data[10001] ;
		int read_total = 0 ;
		wxProgressDialog pd ( txt("t_downloading_rebase") , _T("") , size , NULL , 
				wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_SMOOTH|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME|wxPD_CAN_ABORT ) ;
		while ( read_total < size )
			{
			in->Read ( data , 10000 ) ;
			int read = in->LastRead() ;
			read_total += read ;
			data[read] = 0 ;
			ret += wxString ( data , wxConvUTF8 ) ;
			wxString msg = wxString::Format ( txt("t_downloading_rebase2") , read_total * 100 / size ) ;
			if ( !pd.Update ( read_total , msg ) )
				{
				delete in ;
				ret.Clear() ;
				return ret ;
				}
			}
        delete in;
	    }
    return ret ;
	}

wxString myExternal::getText ( wxString url )
	{
	if ( url.Left(7).Lower() == _T("http://") ) return getTextHTTP ( url ) ;
	if ( url.Left(6).Lower() == _T("ftp://") )
		{
		url = url.Mid ( 6 ) ;
		wxString server = url.BeforeFirst ( '/' ) ;
		wxString file = url.AfterLast ( '/' ) ;
		wxString dir = _T("/") + url.AfterFirst('/').BeforeLast('/') ;
		return getTextFTP ( server , dir , file ) ;
		}
	return getTextLocal ( url ) ; // fallback
	}
     
int myExternal::copyFile ( wxString url , wxString file , int _t )
	{
	targetSize = _t ;
	if ( url.Left(7).Lower() == _T("http://") ) return copyFileHTTP ( url , file ) ;
	return copyFileLocal ( url , file ) ; // fallback
	}    

// ****

wxString myExternal::getTextLocal ( wxString url )
	{
	wxFile in ( url , wxFile::read ) ;
	if ( !in.IsOpened() ) return _T("") ;
	long l = in.Length() ;
	char *c = new char [l+5] ;
	in.Read ( c , l ) ;
	in.Close() ;
	wxString ret = wxString ( c , wxConvUTF8 ) ;
	delete c ;
	return ret ;
	}
     
int myExternal::copyFileLocal ( wxString url , wxString file )
	{
	if ( true == wxCopyFile ( url , file , true ) ) return 0 ;
	return 1 ;
	}	
