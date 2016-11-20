#include "stdafx.h"
#include "CppUnitTest.h"

#include "ReadXMLConfig.h"
#include "Logger.h"
#include "Exception.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WinMachStatUnitTest
{
	TEST_CLASS(ReadXMLConfig)
	{
	public:

		TEST_METHOD(ParseXMLTest)
		{
			WinMachStat::Logger::InitLogger("logger.conf");
			try {
				WinMachStat::ReadXMLConfig xmlRead(L"server.xml");
				LOG(INFO) << "Success opening file server.xml";
				LOG(DEBUG) << "DEBUG: Success opening file server.xml";
			}
			catch (WinMachStat::Exception &e) {
				LOG(INFO) << "Caught exception: " << _getcwd(NULL,0) << ":" << e.getErrorMsg();
			}
			catch (...) {
				LOG(INFO) << "Caught unknown exception";
			}
		}
	};
}