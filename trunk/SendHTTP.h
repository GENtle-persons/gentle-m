#ifndef _SENDHTTP_H_
#define _SENDHTTP_H_

/* HTTPRequest: Structure that returns the HTTP headers and message
                from the request*/
typedef struct
{ 
	LPSTR headers;/* Pointer to HTTP headers */
	LPSTR message;/* Pointer to the HTTP message */
	long messageLength;/* Length of the message */
} HTTPRequest;

/* MemBuffer: Structure used to implement a memory buffer, which is a
              buffer of memory that will grow to hold variable sized
              parts of the HTTP message. */
typedef struct
{
	unsigned char *buffer;
	unsigned char *position;
	size_t size;
} MemBuffer;


int SendHTTP(LPCSTR url,LPCSTR headers,BYTE *post,DWORD postLength,HTTPRequest *req) ;

#endif
