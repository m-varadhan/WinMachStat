#include "stdafx.h"
#include "CppUnitTest.h"

#include "ReadXMLConfig.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WinMachStatUnitTest
{
	TEST_CLASS(ReadXMLConfig)
	{
	public:

		TEST_METHOD(ParseXMLTest)
		{
			WinMachStat::ReadXMLConfig xmlRead(L"server.xml");
		}
	};
}