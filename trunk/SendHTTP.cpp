/* SendHTTP.c by Jeff Heaton(heatonj@jeffheaton.com), 2000
   Source code to GET/POST using the HTTP 1.0 protocol */
   
// LGPL, as far as I (Magnus Manske) know...
// Only used to work around the wxHTTP bug in 2.5.2.

/** \file
	\brief Contains the myExternal class
*/
#ifndef __WXMSW__ // Linux

#include "main.h"
#include "SendHTTP.h"
#include <wx/filesys.h>
#include <wx/protocol/http.h>

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
    if ( !file ) return "" ; // Error
    wxInputStream *in_stream = file->GetStream () ;
    if ( !in_stream ) // Error
    {
	delete file ;
	return "" ;
    }
    while ( !in_stream->Eof() ) ret += in_stream->GetC() ;
    delete file ;
    return ret ; 
}

#else // Windows

#include <stdio.h>
#include <string.h>
#include "winsock2.h"

#include "main.h"
#include "SendHTTP.h"

/* HTTPRequest: Structure that returns the HTTP headers and message
                from the request*/
typedef struct
{ 
	LPSTR headers;/* Pointer to HTTP headers */
	LPSTR message;/* Pointer to the HTTP message */
	long messageLength;/* Length of the message */
} HTTPRequest;

#define MEM_BUFFER_SIZE 10

wxProgressDialog *progressdialog = NULL ;

/* MemBuffer: Structure used to implement a memory buffer, which is a
              buffer of memory that will grow to hold variable sized
              parts of the HTTP message. */
typedef struct
{
	unsigned char *buffer;
	unsigned char *position;
	size_t size;
} MemBuffer;


#include "SendHTTP.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/file.h>
#include <wx/progdlg.h>

/* MemBufferCreate: Passed a MemBuffer structure, will allocate a
                    memory buffer of MEM_BUFFER_SIZE.  This buffer
                    can then grow as needed.*/
void MemBufferCreate(MemBuffer *b)
{
	b->size = MEM_BUFFER_SIZE;
	b->buffer = (BYTE*)malloc( b->size );
	b->position = b->buffer;
}

/* MemBufferGrow:  Double the size of the buffer that was passed
                   to this function. */
void MemBufferGrow(MemBuffer *b)
{
	size_t sz;
	sz = b->position - b->buffer;
	b->size = b->size *2;
	b->buffer = (BYTE*)realloc(b->buffer,b->size);
	b->position = b->buffer + sz;/* readjust the current position */
}

/* MemBufferAddByte: Add a single byte to the memory buffer, grow
                     if needed. */
void MemBufferAddByte(MemBuffer *b,unsigned char byt)
{
	if( (size_t)(b->position-b->buffer) >= b->size )
		MemBufferGrow(b);

	*(b->position++) = byt;
}

/* MemBufferAddBuffer: Add a range of bytes to the memory buffer,
                       grow if needed. */
void MemBufferAddBuffer(MemBuffer *b,unsigned char *buffer,size_t size)
{
	while( ((size_t)(b->position-b->buffer)+size) >= b->size )
		MemBufferGrow(b);

	memcpy(b->position,buffer,size);
	b->position+=size;
}

/* GetHostAddress: Resolve using DNS or similar(WINS,etc) the IP 
                   address for a domain name such as www.wdj.com. */
DWORD GetHostAddress(LPCSTR host)
{
	struct hostent *phe;
	char *p;

	phe = gethostbyname( host );
			
	if(phe==NULL)
		return 0;
	
	p = *phe->h_addr_list;
	return *((DWORD*)p);
}

/* SendString: Send a string(null terminated) over the specified 
               socket. */
void SendString(SOCKET sock,LPCSTR str)
{
	send(sock,str,strlen(str),0);
}

bool isalpha ( char ch )
	{
	if ( ch >= 'a' && ch <= 'z' ) return true ;
	if ( ch >= 'A' && ch <= 'Z' ) return true ;
	return false ;
	}    

bool isdigit ( char ch )
	{
	if ( ch >= '0' && ch <= '9' ) return true ;
	return false ;
	}    

/* ValidHostChar: Return TRUE if the specified character is valid
                  for a host name, i.e. A-Z or 0-9 or -.: */
BOOL ValidHostChar(char ch)
{
	return( isalpha(ch) || isdigit(ch) || ch=='-' || ch=='.' || ch==':' );
}


/* ParseURL: Used to break apart a URL such as 
             http://www.wdj.com:80/index.htm into protocol, port, host 
             and request. */
void ParseURL(LPCSTR url,LPSTR protocol,int lprotocol,LPSTR host,int lhost,LPSTR request,int lrequest,int *port)
{
	char *work,*ptr,*ptr2;

	*protocol = *host = *request = 0;
	*port=80;

	work = strdup(url);
	strupr(work);

	/* find protocol if any */
	ptr = strchr(work,':');
	if(ptr!=NULL)
	{
		*(ptr++) = 0;
		lstrcpyn(protocol,work,lprotocol);
	}
	else
	{
		lstrcpyn(protocol,"HTTP",lprotocol);
		ptr = work;
	}

	/* skip past opening /'s */
	if( (*ptr=='/') && (*(ptr+1)=='/') )
		ptr+=2;

	/* find host */
	ptr2 = ptr;
	while( ValidHostChar(*ptr2) && *ptr2 )
		ptr2++;

	*ptr2=0;
	lstrcpyn(host,ptr,lhost);

	/* find the request */
	lstrcpyn(request,url + (ptr2-work),lrequest);

	/* find the port number, if any */
	ptr = strchr(host,':');
	if(ptr!=NULL)
	{
		*ptr=0;
		*port = atoi(ptr+1);
	}

	free(work);
}

/* SendHTTP: Main entry point for this code.  
      url - The URL to GET/POST to/from.
	  headers - Headers to be sent to the server.
	  post - Data to be posted to the server, NULL if GET.
	  postLength - Length of data to post.
	  req - Contains the message and headers sent by the server.

  returns 1 on failure, 0 on success. */
int SendHTTP(LPCSTR url,LPCSTR headers,BYTE *post,DWORD postLength,HTTPRequest *req)
{
	WSADATA WsaData;
	SOCKADDR_IN sin;
	SOCKET sock;
	char buffer[1024];
	char protocol[20],host[128],request[2048];
	int l,port,chars,err;
	MemBuffer headersBuffer,messageBuffer;
	BOOL done;
	
	/* Parse the URL */
	ParseURL(url,protocol,sizeof(protocol)-1,host,sizeof(host)-1,request,sizeof(request)-1,&port);
	if(strcmp(protocol,"HTTP"))
		return 1;

	/* Init Winsock */

	err = WSAStartup (0x0101, &WsaData);
	if(err!=0)
		return 1;

	sock = socket (AF_INET, SOCK_STREAM, 0);
	if ( (unsigned int)socket == INVALID_SOCKET)
		return 1;

	/* Connect to web sever */

	sin.sin_family = AF_INET;
	sin.sin_port = htons( (unsigned short)port );
	sin.sin_addr.s_addr = GetHostAddress(host);

	if( connect (sock,(LPSOCKADDR)&sin, sizeof(SOCKADDR_IN) ) )
		return 1;


	/* Send request */

	if( !*request )
		lstrcpyn(request,"/",sizeof(request)-1);

	if( post == NULL )
		SendString(sock,"GET ");
	else SendString(sock,"POST ");
	SendString(sock,request);
	SendString(sock," HTTP/1.0\r\n");
	SendString(sock,"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/msword, application/vnd.ms-powerpoint, */*\r\n");
	SendString(sock,"Accept-Language: en-us\r\n");
	SendString(sock,"Accept-Encoding: gzip, deflate\r\n");
	SendString(sock,"User-Agent: Mozilla/4.0\r\n");
	if(postLength)
	{
		wsprintf(buffer,"Content-Length: %ld\r\n",postLength);
		SendString(sock,buffer);
	}
	SendString(sock,"Host: ");
	SendString(sock,host);
	SendString(sock,"\r\n");
	if( (headers!=NULL) && *headers )
		SendString(sock,headers);
	/* Send a blank line to signal end of HTTP headers */
	SendString(sock,"\r\n");
	if( (post!=NULL) && postLength )
		send(sock,(char*)post,postLength,0);
	

	/* Read the result */
	/* First read HTTP headers */

	MemBufferCreate(&headersBuffer );
	chars = 0;
	done = FALSE;

	while(!done)
	{
		l = recv(sock,buffer,1,0);
		if(l<0)
			done=TRUE;

		switch(*buffer)
		{
			case '\r':
				break;
			case '\n':
				if(chars==0)
					done = TRUE;
				chars=0;
				break;
			default:
				chars++;
				break;
		}

		MemBufferAddByte(&headersBuffer,*buffer);
	}

	req->headers=(char*)headersBuffer.buffer;
	*(headersBuffer.position) = 0;

	/* Now read the HTTP body */

	MemBufferCreate(&messageBuffer);

	unsigned long cnt = 0 ;
	
	do
	{
		l = recv(sock,buffer,sizeof(buffer)-1,0);
		if(l<0)
			break;
		*(buffer+l)=0;
		MemBufferAddBuffer(&messageBuffer,(BYTE*)buffer,l);
		
		if ( progressdialog )
			{
			cnt += l ;
			wxString s = wxString::Format ( "%d bytes read" , cnt ) ;
			progressdialog->Update ( cnt , s ) ;
			}			
		
		
	} while(l>0);
	*messageBuffer.position = 0;
	req->message = (char*)messageBuffer.buffer;
	req->messageLength = (messageBuffer.position-messageBuffer.buffer);

	/* Cleanup */

	closesocket(sock);

	return 0;
}
/*
/* main:  Function to demonstrate the use of SendHTTP as a 
          console application. 
int main( int argc, char *argv[ ] )
{
	HTTPRequest req;
	BOOL headers = FALSE;
	BOOL nomessage = FALSE;
	LPSTR url = NULL;
	LPSTR post = NULL;
	int i,j,rtn;
	FILE *fp;
	LPSTR buffer;

	req.headers = NULL;
	req.message = NULL;

	// Read in arguments 
	for(i=1,j=0;i<argc;i++)
	{
		if( (*argv[i]=='-') || (*argv[i]=='/') )
		{
			if( !strcmpi(argv[i]+1,"HEADERS") )
				headers=TRUE;

			if( !strcmpi(argv[i]+1,"NOMESSAGE") )
				nomessage=TRUE;
		}
		else
		{
			if(j==0)
			{
				url = argv[i];
				j++;
			}
			else if(j==1)
			{
				post = argv[i];
				j++;
			}
		}
	}

	// Display usage if needed 
	if(url==NULL)
	{
		printf("\nUsage:\n\n");
		printf("%s <URL to POST/GET> <FILE to POST> [/HEADERS] [/NOMESSAGE]\n",argv[0]);
		printf("/HEADRES to display the HTTP headers.\n");
		printf("/NOMESSAGE to not display the HTTP message.\n\n");
		return 1;
	}

	if(post!=NULL)
	{
		// POST 
		if( (fp = fopen(post,"rb")) == NULL )
		{
			printf("\nCan't open %s for read.\n\n",post);
			return 1;
		}

		fseek(fp,0,SEEK_END);
		i=ftell(fp);
		fseek(fp,0,SEEK_SET);
		buffer = (char*)malloc(i+1);
		fread(buffer,i,1,fp);
		fclose(fp);

		rtn = SendHTTP(
			url,
			"Content-Type: application/x-www-form-urlencoded\r\n",
			(unsigned char*)buffer,
			i,
			&req);
		
		free(buffer);
	}
	else
		// GET 
		rtn = SendHTTP(url,NULL,NULL,0,&req);

	if(!rtn)
	{
		// Output message and/or headers 
		if(headers)
			puts(req.headers);

		if(!nomessage)
			puts(req.message);

		free(req.headers);
		free(req.message);
	}
	else
		printf("\nFailed\n");

	// Return the error code 
	return rtn;
}

*/

myExternal::myExternal () 
	{
	pd = NULL ;
	}
     
wxString getMemSize ( long l , char unit = 'B' )
	{
	wxString ret , r2 ;
	if ( unit == 'B' ) ret = wxString::Format ( "%d Byte" , l ) ;
	if ( unit == 'K' ) ret = wxString::Format ( "%1.1f KB" , ( (float) l ) / 1024.0 ) ;
	if ( unit == 'M' ) ret = wxString::Format ( "%1.1f MB" , ( (float) l ) / 1048576.0 ) ;
	if ( unit == 'B' ) r2 = getMemSize ( l , 'K' ) ;
	if ( unit == 'K' ) r2 = getMemSize ( l , 'M' ) ;
	if ( r2 != "" && r2.length() < ret.length() ) ret = r2 ;
	return ret ;
	}    

// ****

int myExternal::copyFileHTTP ( wxString _url , wxString _file )
	{
	LPCSTR url = _url.c_str() ;
	LPCSTR headers = NULL ;
 	BYTE *post = NULL ;
  	DWORD postLength = 0 ;
   	HTTPRequest *req = new HTTPRequest ;

	WSADATA WsaData;
	SOCKADDR_IN sin;
	SOCKET sock;
	char buffer[1024];
	char protocol[20],host[128],request[2048];
	int l,port,chars,err;
	MemBuffer headersBuffer,messageBuffer;
	BOOL done;
	
	/* Parse the URL */
	ParseURL(url,protocol,sizeof(protocol)-1,host,sizeof(host)-1,request,sizeof(request)-1,&port);
	if(strcmp(protocol,"HTTP"))
		return 1;

	/* Init Winsock */

	err = WSAStartup (0x0101, &WsaData);
	if(err!=0)
		return 1;

	sock = socket (AF_INET, SOCK_STREAM, 0);
	if ( (unsigned int)socket == INVALID_SOCKET)
		return 1;

	/* Connect to web sever */

	sin.sin_family = AF_INET;
	sin.sin_port = htons( (unsigned short)port );
	sin.sin_addr.s_addr = GetHostAddress(host);

	if( connect (sock,(LPSOCKADDR)&sin, sizeof(SOCKADDR_IN) ) )
		return 1;


	/* Send request */

	if( !*request )
		lstrcpyn(request,"/",sizeof(request)-1);

	if( post == NULL )
		SendString(sock,"GET ");
	else SendString(sock,"POST ");
	SendString(sock,request);
	SendString(sock," HTTP/1.0\r\n");
	SendString(sock,"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/msword, application/vnd.ms-powerpoint, */*\r\n");
	SendString(sock,"Accept-Language: en-us\r\n");
	SendString(sock,"Accept-Encoding: gzip, deflate\r\n");
	SendString(sock,"User-Agent: Mozilla/4.0\r\n");
	if(postLength)
	{
		wsprintf(buffer,"Content-Length: %ld\r\n",postLength);
		SendString(sock,buffer);
	}
	SendString(sock,"Host: ");
	SendString(sock,host);
	SendString(sock,"\r\n");
	if( (headers!=NULL) && *headers )
		SendString(sock,headers);
	/* Send a blank line to signal end of HTTP headers */
	SendString(sock,"\r\n");
	if( (post!=NULL) && postLength )
		send(sock,(char*)post,postLength,0);
	

	/* Read the result */
	/* First read HTTP headers */

	MemBufferCreate(&headersBuffer );
	chars = 0;
	done = FALSE;

	while(!done)
	{
		l = recv(sock,buffer,1,0);
		if(l<0)
			done=TRUE;

		switch(*buffer)
		{
			case '\r':
				break;
			case '\n':
				if(chars==0)
					done = TRUE;
				chars=0;
				break;
			default:
				chars++;
				break;
		}

		MemBufferAddByte(&headersBuffer,*buffer);
	}

	req->headers=(char*)headersBuffer.buffer;
	*(headersBuffer.position) = 0;

	/* Now read the HTTP body */

	MemBufferCreate(&messageBuffer);

	wxFile out ( _file , wxFile::write ) ; // Mine

	long l2 = 0 ;
	wxString lastMessage ;
	do
	{
		l = recv(sock,buffer,sizeof(buffer)-1,0);
		if(l<0)
			break;
		*(buffer+l)=0;
		out.Write ( buffer , l ) ; // Mine
		*buffer = 0 ; // Mine
		l2 += l ;
		if ( pd )
  			{
  			wxString tmp = getMemSize ( l2 ) ;
  			if ( tmp != lastMessage )
  				{
      			lastMessage = tmp ;
      			int per = 0 ;
      			if ( targetSize != -1 ) per = 100 * l2 / targetSize ;
             	if ( !pd->Update ( per , lastMessage ) )
             		{
         		    delete req ;
         		    return 2 ;
         		    }    
         		}    
         	}
//		MemBufferAddBuffer(&messageBuffer,(BYTE*)buffer,l);
	} while(l>0);
	*messageBuffer.position = 0;
	req->message = (char*)messageBuffer.buffer;
	req->messageLength = (messageBuffer.position-messageBuffer.buffer);

	/* Cleanup */

	closesocket(sock);

	// Mine
	delete req ;
	return 0 ;
	}
     
wxString myExternal::getTextHTTP ( wxString url )
	{
	HTTPRequest req ;
	progressdialog = pd ;
	int rtn = SendHTTP ( url.c_str() , NULL , NULL , 0 , &req ) ;
	progressdialog = NULL ;
	wxString ret ;
	if ( !rtn )
 		{
   		ret.Alloc ( req.messageLength + 5 ) ;
       	ret = req.message ;
       	}   	
	if ( req.message ) delete req.message ;
	return ret ;    
	}
     
#endif

wxString myExternal::getText ( wxString url )
	{
	if ( url.Left(7).Lower() == "http://" ) return getTextHTTP ( url ) ;
	return getTextLocal ( url ) ; // fallback
	}
     
int myExternal::copyFile ( wxString url , wxString file , int _t )
	{
	targetSize = _t ;
	if ( url.Left(7).Lower() == "http://" ) return copyFileHTTP ( url , file ) ;
	return copyFileLocal ( url , file ) ; // fallback
	}    

// ****

wxString myExternal::getTextLocal ( wxString url )
	{
	wxFile in ( url , wxFile::read ) ;
	if ( !in.IsOpened() ) return "" ;
	long l = in.Length() ;
	char *c = new char [l+5] ;
	in.Read ( c , l ) ;
	in.Close() ;
	wxString ret = c ;
	delete c ;
	return ret ;
	}
     
int myExternal::copyFileLocal ( wxString url , wxString file )
	{
	if ( true == wxCopyFile ( url , file , true ) ) return 0 ;
	return 1 ;
	}	
