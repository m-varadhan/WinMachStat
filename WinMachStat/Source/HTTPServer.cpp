#include "HTTPServer.h"

#include "Exception.h"
#include "Logger.h"

#define RETCODE_CHECK_THROW(rc,msg) if((rc)!=NO_ERROR)	{ throw Exception(errno,ExceptionFormatter()<<(msg)<<(rc)); }
#define RETNULL_CHECK_THROW(rc,msg) if((rc)==NULL)		{ throw Exception(errno,ExceptionFormatter()<<(msg)); }

namespace WinMachStat {
	//public functions
	HTTPServer::HTTPServer(std::vector<std::wstring> _urls, HTTPRequestHandler rH) :
			reqHandler(rH), HttpApiVersion(HTTPAPI_VERSION_2), urls(_urls)
	{
		initHTTPServer();
	}

	HTTPServer::HTTPServer(HTTPRequestHandler rH) :
		reqHandler(rH), HttpApiVersion(HTTPAPI_VERSION_2)
	{
		initHTTPServer();
	}

	int HTTPServer::Listen(int maxRequest)
	{
		ULONG			   result;
		HTTP_REQUEST_ID    requestId;
		DWORD              bytesRead;
		PHTTP_REQUEST      pRequest;
		PCHAR              pRequestBuffer;
		ULONG              RequestBufferLength;

		RequestBufferLength = sizeof(HTTP_REQUEST) + 2048;
		pRequestBuffer = (PCHAR)ALLOC_MEM(RequestBufferLength);

		RETNULL_CHECK_THROW(pRequestBuffer, "Not enough Memory");

		pRequest = (PHTTP_REQUEST)pRequestBuffer;

		for (reqHandled=0; (maxRequest == 0) || reqHandled<maxRequest; ++reqHandled) {
			HTTP_SET_NULL_ID(&requestId);
			if ((retCode=HttpReceiveHttpRequest(
					hReqQueue,
					requestId,
					0,
					pRequest,
					RequestBufferLength,
					&bytesRead,
					NULL
				)) == NO_ERROR) {
				int responseCode = reqHandler(pRequest);
				LOG(INFO) << "Got Response:" << responseCode;
				switch (responseCode) {
				case 200:
					result = sendHttpResponse(
						pRequest,
						200,
						"OK",
						"Hey! You hit the server \r\n"
					);
					break;
				default:
					result = sendHttpResponse(
						pRequest,
						503,
						"Not Implemented",
						NULL
					);
					break;
				}
			}
			else {
				LOG(INFO) << "Error occured while receiving request:" << retCode;
			}
		}

		if (pRequestBuffer) {
			FREE_MEM(pRequestBuffer);
		}

		return 0;
	}

	HTTPServer::~HTTPServer() {
		HttpCloseUrlGroup(urlGroupID);
		HttpCloseRequestQueue(hReqQueue);
		HttpCloseServerSession(serverSessionID);
		HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);
	}
	//private functions
	void HTTPServer::initHTTPServer() {
		HTTP_URL_CONTEXT urlContext = 0;
		retCode = HttpInitialize(
			HttpApiVersion,
			HTTP_INITIALIZE_SERVER,
			NULL);
		RETCODE_CHECK_THROW(retCode, "Failed to Initialize HTTP Service: ");

		retCode = HttpCreateServerSession(
			HttpApiVersion,
			&serverSessionID,
			NULL);
		RETCODE_CHECK_THROW(retCode, "Failed to Create HTTP Session: ");

		retCode = HttpCreateRequestQueue(
			HttpApiVersion,
			NULL,
			NULL,
			0,
			&hReqQueue);
		RETCODE_CHECK_THROW(retCode, "Failed to Create HTTP Request Service: ");

		retCode = HttpCreateUrlGroup(
			serverSessionID,
			&urlGroupID,
			NULL);
		RETCODE_CHECK_THROW(retCode, "Failed to Create URL Group: ");

		for (auto url : urls) {
			retCode = HttpAddUrlToUrlGroup(
				urlGroupID,
				url.c_str(),
				urlContext++,
				NULL);
			RETCODE_CHECK_THROW(retCode, "Failed to Add URL Group: ");
		}

		HTTP_BINDING_INFO bindingInfo;
		bindingInfo.RequestQueueHandle = hReqQueue;
		HTTP_PROPERTY_FLAGS propertyFlags;
		propertyFlags.Present = 1;
		bindingInfo.Flags = propertyFlags;
		retCode = HttpSetUrlGroupProperty(
			urlGroupID,
			HttpServerBindingProperty,
			&bindingInfo,
			sizeof(HTTP_BINDING_INFO));
	}


	DWORD HTTPServer::sendHttpResponse(
		IN PHTTP_REQUEST pRequest,
		IN USHORT        StatusCode,
		IN PSTR          pReason,
		IN PSTR          pEntityString
	)
	{
		HTTP_RESPONSE   response;
		HTTP_DATA_CHUNK dataChunk;
		DWORD           result;
		DWORD           bytesSent;

		//
		// Initialize the HTTP response structure.
		//
		INITIALIZE_HTTP_RESPONSE(&response, StatusCode, pReason);

		//
		// Add a known header.
		//
		ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");

		if (pEntityString)
		{
			// 
			// Add an entity chunk.
			//
			dataChunk.DataChunkType = HttpDataChunkFromMemory;
			dataChunk.FromMemory.pBuffer = pEntityString;
			dataChunk.FromMemory.BufferLength =
				(ULONG)strlen(pEntityString);

			response.EntityChunkCount = 1;
			response.pEntityChunks = &dataChunk;
		}

		// 
		// Because the entity body is sent in one call, it is not
		// required to specify the Content-Length.
		//

		result = HttpSendHttpResponse(
			hReqQueue,           // ReqQueueHandle
			pRequest->RequestId, // Request ID
			0,                   // Flags
			&response,           // HTTP response
			NULL,                // pReserved1
			&bytesSent,          // bytes sent  (OPTIONAL)
			NULL,                // pReserved2  (must be NULL)
			0,                   // Reserved3   (must be 0)
			NULL,                // LPOVERLAPPED(OPTIONAL)
			NULL                 // pReserved4  (must be NULL)
		);

		if (result != NO_ERROR)
		{
			LOG(INFO) << "HttpSendHttpResponse failed with:" << result;
		}

		return result;
	}


}