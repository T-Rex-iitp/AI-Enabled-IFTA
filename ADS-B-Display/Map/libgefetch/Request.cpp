//---------------------------------------------------------------------------


#pragma hdrstop

#include "Request.h"
#include <stdio.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)
#include "Request.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char *UserAgent=(char *)"User-Agent: kh_lt/LT3.0.0762\r\n";
char *AcceptText=(char *)"Accept: text/plain, text/html, text/xml, text/xml-external-parsed-entity, application/octet-stream, application/vnd.google-earth.kml+xml, application/vnd.google-earth.kmz, image/*\r\n";
char *CacheControl=(char *)"Cache-Control: no-store\r\n";
char *ContentType=(char *)"Content-Type: application/octet-stream\r\n";
char *ConnectionAttributes=(char *)"Connection: Keep-Alive\r\n";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Request::Request()
{

}

Request::~Request()
{

}


//*******************************************************************************************************
//MemBufferCreate: 
//					Passed a MemBuffer structure, will allocate a memory buffer 
//                   of MEM_BUFFER_SIZE.  This buffer can then grow as needed.
//*******************************************************************************************************
void Request::MemBufferCreate(MemBuffer *b)
{
    b->size = MEM_BUFFER_SIZE;
    b->buffer =(unsigned	char *) malloc( b->size );
    b->position = b->buffer;
}

//*******************************************************************************************************
// MemBufferGrow:  
//					Double the size of the buffer that was passed to this function. 
//*******************************************************************************************************
void Request::MemBufferGrow(MemBuffer *b)
{
    size_t sz;
    sz = b->position - b->buffer;
    b->size = b->size *2;
    b->buffer =(unsigned	char *) realloc(b->buffer,b->size);
    b->position = b->buffer + sz;	// readjust current position
}

//*******************************************************************************************************
// MemBufferAddByte: 
//					Add a single byte to the memory buffer, grow if needed.
//*******************************************************************************************************
void Request::MemBufferAddByte(MemBuffer *b,unsigned char byt)
{
    if( (size_t)(b->position-b->buffer) >= b->size )
        MemBufferGrow(b);

    *(b->position++) = byt;
}

//*******************************************************************************************************
// MemBufferAddBuffer:
//					Add a range of bytes to the memory buffer, grow if needed.
//*******************************************************************************************************
void Request::MemBufferAddBuffer(MemBuffer *b,
                    unsigned char *buffer, size_t size)
{
    while( ((size_t)(b->position-b->buffer)+size) >= b->size )
        MemBufferGrow(b);

    memcpy(b->position,buffer,size);
    b->position+=size;
}

//*******************************************************************************************************
// GetHostAddress: 
//					Resolve using DNS or similar(WINS,etc) the IP 
//                   address for a domain name such as www.wdj.com. 
//*******************************************************************************************************
DWORD Request::GetHostAddress(LPCSTR host)
{
    struct hostent *phe;
    char *p;

    phe = gethostbyname( host );
            
    if(phe==NULL)
        return 0;
    
    p = *phe->h_addr_list;
    return *((DWORD*)p);
}

//*******************************************************************************************************
// SendString: 
//					Send a string(null terminated) over the specified socket.
//*******************************************************************************************************
void Request::SendString(SOCKET sock,LPCSTR str)
{
	send(sock,str,strlen(str),0);
}

//*******************************************************************************************************
// ValidHostChar: 
//					Return TRUE if the specified character is valid
//						for a host name, i.e. A-Z or 0-9 or -.: 
//*******************************************************************************************************
BOOL Request::ValidHostChar(char ch)
{
    return( isalpha(ch) || isdigit(ch)
        || ch=='-' || ch=='.' || ch==':' );
}


//*******************************************************************************************************
// ParseURL: 
//					Used to break apart a URL such as 
//						http://www.localhost.com:80/TestPost.htm into protocol, port, host and request.
//*******************************************************************************************************
void Request::ParseURL(LPCSTR url,LPSTR protocol,int lprotocol,
        LPSTR host,int lhost,LPSTR request,int lrequest,int *port)
{
    char *work,*ptr,*ptr2;

    *protocol = *host = *request = 0;
    *port=80;

    work = strdup(url);
    strupr(work);

    ptr = strchr(work,':');							// find protocol if any
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

    if( (*ptr=='/') && (*(ptr+1)=='/') )			// skip past opening /'s 
        ptr+=2;

    ptr2 = ptr;										// find host
    while( ValidHostChar(*ptr2) && *ptr2 )
        ptr2++;

    *ptr2=0;
    lstrcpyn(host,ptr,lhost);

    lstrcpyn(request,url + (ptr2-work),lrequest);	// find the request

    ptr = strchr(host,':');							// find the port number, if any
    if(ptr!=NULL)
    {
        *ptr=0;
        *port = atoi(ptr+1);
    }

    free(work);
}

//*******************************************************************************************************
// SendHTTP: 
//					Main entry point for this code.  
//					  url			- The URL to GET/POST to/from.
//					  headerSend		- Headers to be sent to the server.
//					  post			- Data to be posted to the server, NULL if GET.
//					  postLength	- Length of data to post.
//					  req			- Contains the message and headerSend sent by the server.
//
//					  returns 1 on failure, 0 on success.
//*******************************************************************************************************
int Request::SendHTTP(LPCSTR url,BYTE *post,
		DWORD postLength,HTTPRequest *req,char *sid,long *httpcode)
{
    WSADATA			WsaData;
    SOCKADDR_IN		sin;
    SOCKET			sock;
    char			buffer[512];
    char			protocol[20],host[256],request[1024];
    int				l,port,chars,err;
    MemBuffer		headersBuffer,messageBuffer;
	char			headerSend[1024];
    BOOL			done;



	*httpcode = -1;
    ParseURL(url,protocol,sizeof(protocol),host,sizeof(host),		// Parse the URL
        request,sizeof(request),&port);
    if(strcmp(protocol,"HTTP"))
        return 1;

    err = WSAStartup (0x0101, &WsaData);							// Init Winsock
    if(err!=0)
        return 1;

	sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
        return 1;

    
    sin.sin_family = AF_INET;										//Connect to web sever
    sin.sin_port = htons( (unsigned short)port );
    sin.sin_addr.s_addr = GetHostAddress(host);

    if( connect (sock,(LPSOCKADDR)&sin, sizeof(SOCKADDR_IN) ) )
	{
        return 1;
	}


    if( !*request )
		lstrcpyn(request,"/",sizeof(request));

    if( post == NULL )
	{
        SendString(sock,"GET ");
		strcpy(headerSend, "GET ");
	}
    else 
	{
		SendString(sock,"POST ");
		strcpy(headerSend, "POST ");
	}
	SendString(sock,request);
		strcat(headerSend, request);

	SendString(sock," HTTP/1.1\r\n");
		strcat(headerSend, " HTTP/1.1\r\n");

	SendString(sock,UserAgent);
		strcat(headerSend, UserAgent);

	SendString(sock,"Host: ");
		strcat(headerSend, "Host: ");

	SendString(sock,host);
		strcat(headerSend, host);
	SendString(sock,"\r\n");
		strcat(headerSend, "\r\n");


	SendString(sock,AcceptText);
		strcat(headerSend, AcceptText);

	SendString(sock,CacheControl);
		strcat(headerSend, CacheControl);

	SendString(sock,ContentType);
		strcat(headerSend, ContentType);

	SendString(sock,ConnectionAttributes);
		strcat(headerSend, ConnectionAttributes);

	if (sid) {
		char buf[1024];

	 snprintf(buf, sizeof(buf), "Cookie: $Version=\"0\"; SessionId=\"%s\"; State=\"1\"\r\n", sid);
	SendString(sock,buf);
		strcat(headerSend,buf );
	}

	if(postLength)
    {
		wsprintf(buffer,"Content-Length: %ld\r\n",postLength);
        SendString(sock,buffer);
		strcat(headerSend, buffer);
    }

    
    SendString(sock,"\r\n");								// Send a blank line to signal end of HTTP headerReceive
		strcat(headerSend, "\r\n");

    if( (post!=NULL) && postLength )
	{
        send(sock,(const char*)post,postLength,0);
		post[postLength]	= '\0';

		strcat(headerSend, (const char*)post);
	}

	req->headerSend		= (char*) malloc( sizeof(char*) * strlen(headerSend));
	strcpy(req->headerSend, (char*) headerSend );

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

    req->headerReceive	= (char*) headersBuffer.buffer;
    *(headersBuffer.position) = 0;

	char *cl;
	int Length;
	bool HaveLength=false;


	cl = strstr((char *)headersBuffer.buffer, "Content-Length:");
	if (cl)
	  {
	   errno = 0;
	   Length = strtol((cl + 16),NULL,10);
	   if (errno!=ERANGE)
	   {
		 HaveLength=true;
	   }
	  }
	int  httpversionMajor,httpversionMinor ,code,nc;
	nc = sscanf((const char *)headersBuffer.buffer,
						  " HTTP/%d.%d %3d",
						  &httpversionMajor,
						  &httpversionMinor,
						  &code);
	if(nc==3)
	{
	 *httpcode=code;
	}

	MemBufferCreate(&messageBuffer);							// Now read the HTTP body
	int BytesIn=0;
	do
    {
		l = recv(sock,buffer,sizeof(buffer)-1,0);
		if(l<0) break;
		*(buffer+l)=0;
		BytesIn+=l;
		MemBufferAddBuffer(&messageBuffer, (unsigned char*)&buffer, l);
		if ((HaveLength) && (BytesIn>=Length)) break;
	} while(l>0);
	*messageBuffer.position = 0;
    req->message = (char*) messageBuffer.buffer;
	req->messageLength = (messageBuffer.position - messageBuffer.buffer);


    closesocket(sock);											// Cleanup

    return 0;
}


//*******************************************************************************************************
// SendRequest
//
//*******************************************************************************************************
int Request::SendRequest(bool IsPost, LPCSTR url, char *pszHeaderSend,DWORD HeaderSendLength,HTTPRequest *Ret, char *sid,long *httpcode)
{
	int					j,rtn;
    FILE				*fp;
    LPSTR				buffer;

	Ret->headerSend			  = NULL;
	Ret->headerReceive		  = NULL;
	Ret->message			  = NULL;

    //Read in arguments
    

	if(IsPost)
	{													/* POST */
		buffer  = (char*) malloc(HeaderSendLength);
		memcpy(buffer, (LPCTSTR)pszHeaderSend,HeaderSendLength+1);

		rtn				= SendHTTP(	url,
									(unsigned char*)buffer,
									HeaderSendLength,
									Ret,sid,httpcode);

		free(buffer);
	}
	else												/* GET */
		rtn = SendHTTP(url,NULL,0,Ret,sid,httpcode);


	if(rtn)	return 1;

	else return(0);
}


