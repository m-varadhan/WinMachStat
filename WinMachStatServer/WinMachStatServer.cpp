#include "HttpServer.h"
#include "LoggerMain.h"

#include "Exception.h"

#include "DBConnect.h"

static int HTTPRequestHandler(PHTTP_REQUEST hRequest);

static int HTTPRequestHandler(PHTTP_REQUEST pReq)
{
	switch (pReq->Verb)
	{
	case HttpVerbGET:
		LOG(INFO) << "Got GET Request for " << pReq->CookedUrl.pFullUrl;
		return 200;
		break;
	}
	LOG(INFO) << "Got Unknown Request for " << pReq->CookedUrl.pFullUrl;
	return 503;
}

int main()
{
	std::vector<std::wstring> urls;
	WinMachStat::HTTPServer *HTTPServer = NULL;

	urls.push_back(L"http://localhost:50080/");

	WinMachStat::Logger::InitLogger("logger.conf");

	LOG(INFO) << "Starting Server...";
	try {
		HTTPServer = new WinMachStat::HTTPServer(urls, HTTPRequestHandler);
		LOG(INFO) << "Starting Server Listening...";
		HTTPServer->Listen(2);
	}
	catch (WinMachStat::Exception &e) {
		LOG(INFO) << "Caught exception: " << e.getErrorMsg();
	}
	catch (...) {
		LOG(INFO) << "Caught unknown exception";
	}
	if (HTTPServer) {
		delete HTTPServer;
	}
}