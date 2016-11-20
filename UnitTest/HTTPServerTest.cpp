#include "stdafx.h"
#include "CppUnitTest.h"
#include "HTTPServer.h"
#include "Logger.h"
#include "Exception.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WinMachStatUnitTest
{		
	TEST_CLASS(HTTPServer)
	{
	public:
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
		
		TEST_METHOD(createServer)
		{
			std::vector<std::wstring> urls;
			
			WinMachStat::HTTPServer *httpServer = NULL;

			urls.push_back(L"http://localhost:50080/");

			WinMachStat::Logger::InitLogger("logger.conf");

			LOG(INFO) << "Starting Server...";
			try {
				httpServer = new WinMachStat::HTTPServer(urls, HTTPRequestHandler);
				LOG(INFO) << "Starting Server Listening...";
				//httpServer->Listen(2);
			}
			catch (WinMachStat::Exception &e) {
				LOG(INFO) << "Caught exception: " << e.getErrorMsg();
			}
			catch (...) {
				LOG(INFO) << "Caught unknown exception";
			}

			if (httpServer) {
				delete httpServer;
			}
		}

	};
}