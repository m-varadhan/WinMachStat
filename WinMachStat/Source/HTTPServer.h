#include "Config.h"

#include <windows.h>
#include <http.h>

#pragma comment(lib, "httpapi.lib")

#include <string>
#include <vector>

#define ALLOC_MEM(cb) HeapAlloc(GetProcessHeap(), 0, (cb))
#define FREE_MEM(ptr) HeapFree(GetProcessHeap(), 0, (ptr))
#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )    \
    do                                                      \
    {                                                       \
        RtlZeroMemory( (resp), sizeof(*(resp)) );           \
        (resp)->StatusCode = (status);                      \
        (resp)->pReason = (reason);                         \
        (resp)->ReasonLength = (USHORT) strlen(reason);     \
    } while (FALSE)
#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)               \
    do                                                               \
    {                                                                \
        (Response).Headers.KnownHeaders[(HeaderId)].pRawValue =      \
                                                          (RawValue);\
        (Response).Headers.KnownHeaders[(HeaderId)].RawValueLength = \
            (USHORT) strlen(RawValue);                               \
    } while(FALSE)


namespace WinMachStat {
	typedef int(*HTTPRequestHandler)(PHTTP_REQUEST hReqQ);
	class HTTPServer {
	private:
		ULONG           retCode; /* Last HTTP API return code */
		HANDLE          hReqQueue;
		int             UrlAdded ;
		const HTTPAPI_VERSION HttpApiVersion ;
		
		HTTP_SERVER_SESSION_ID serverSessionID;
		HTTP_URL_GROUP_ID urlGroupID;
		std::vector<std::wstring> urls;

		HTTPRequestHandler reqHandler;

		int reqHandled = 0;

		HTTPServer(void) = delete;
		DWORD	sendHttpResponse(
				IN PHTTP_REQUEST pRequest,
				IN USHORT        StatusCode,
				IN PSTR          pReason,
				IN PSTR          pEntity
			);

		void initHTTPServer();

	public:
		HTTPServer(std::vector<std::wstring> urls, HTTPRequestHandler rH) ;
		HTTPServer::~HTTPServer();

		int Listen(int maxRequest=0);
	};
}