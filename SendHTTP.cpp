/** \file
    \brief Contains the myExternal class
*/

#include "main.h"
#include "SendHTTP.h"
#include <wx/filesys.h>
#include <wx/protocol/http.h>
#include <wx/protocol/ftp.h>
#include <wx/filefn.h> // wxCopyFile

myExternal::myExternal () { pd = NULL ; } ;

int myExternal::copyFileHTTP ( const wxString& _url , const wxString& _file )
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

wxString myExternal::getTextHTTP ( const wxString& url )
    {
    wxString ret ;
    wxFileSystem fs ;
    wxFSFile *file = fs.OpenFile ( url ) ;
    if ( !file ) return "" ; // Error
    wxInputStream *in_stream = file->GetStream () ;

    if ( !in_stream ) // Error
        {
        delete file ;
        return "" ;
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

wxString myExternal::getTextFTP ( const wxString& server , const wxString& dir , const wxString& file )
    {
    wxString ret ;
    wxFTP ftp;

    if ( !ftp.Connect(server) )
        {
        wxPrintf( "E: Could not connect to server '%s'\n", server ) ;
        return ret ;
        }
    if ( !ftp.ChDir(dir) )
        {
        wxPrintf( "E: Connected to server '%s', could not change to dir '%s'\n" , server , dir ) ;
        return ret ;
        }

    int size = ftp.GetFileSize ( file ) ;
    if ( -1 == size )
        {
        wxPrintf( "E: Connected to server '%s', could not determined size of file '%s' from dir '%s'\n" , server , file, dir ) ;
        return ret;
        }

    wxInputStream *in = ftp.GetInputStream(file);
    if ( ! in )
        {
        wxPrintf( "E: Connected to server '%s', could not retrieve file '%s' from dir '%s'\n" , server , file, dir ) ;
        return ret;
        }

    char data[10000+1] ;
    int read_total = 0 ;
    wxProgressDialog pd ( txt("t_downloading_rebase") , "" , size , NULL ,
                          wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_SMOOTH|wxPD_ELAPSED_TIME|wxPD_ESTIMATED_TIME|wxPD_REMAINING_TIME|wxPD_CAN_ABORT ) ;
    while ( read_total < size )
        {
        in->Read ( data , sizeof(data)-1 ) ;
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
    return ret ;
    }

wxString myExternal::getText ( const wxString& _u )
    {
    wxString url ( _u ) ;
    if ( url.Left(7).Lower() == "http://" || url.Left(8).Lower() == "https://" )
        {
        wxPrintf( "D: Invoking getTextHTTP(%s)\n" , url ) ;
        return getTextHTTP ( url ) ;
        }
    else if ( url.Left(6).Lower() == "ftp://" )
       {
        url = url.Mid ( 6 ) ;
        wxString server = url.BeforeFirst ( '/' ) ;
        wxString file = url.AfterLast ( '/' ) ;
        wxString dir = _T("/") + url.AfterFirst('/').BeforeLast('/') ;
        wxPrintf( "D: Invoking getTextFTP(%s,%s,%s)\n" , server, dir, file ) ;
        return getTextFTP ( server , dir , file ) ;
        }
    wxPrintf( "D: Invoking getTextLocal(%s)\n" , url ) ;
    return getTextLocal ( url ) ; // fallback
    }

int myExternal::copyFile ( const wxString& url , const wxString& file , const int _t )
    {
    targetSize = _t ;
    if ( url.Left(7).Lower() == "http://" ) return copyFileHTTP ( url , file ) ;
    return copyFileLocal ( url , file ) ; // fallback
    }

// ****

wxString myExternal::getTextLocal ( const wxString& url )
    {
    wxFile in ( url , wxFile::read ) ;
    if ( !in.IsOpened() ) return _T("") ;
    long l = in.Length() ;
    char *c = new char [l+5] ;
    in.Read ( c , l ) ;
    in.Close() ;
    wxString ret = wxString ( c , wxConvUTF8 ) ;
    delete [] c ;
    return ret ;
    }

int myExternal::copyFileLocal ( const wxString& url , const wxString& file )
    {
    if ( wxCopyFile ( url , file , true ) ) return 0 ;
    return 1 ;
    }
