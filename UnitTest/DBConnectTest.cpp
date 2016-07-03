#include "stdafx.h"
#include "DBConnect.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WinMachStatUnitTest
{
	TEST_CLASS(DBConnect)
	{
	public:

		TEST_METHOD(DBConnectMysql)
		{
			try {
				WinMachStat::DBConnect<WinMachStat::Mysql> db("tcp://127.0.0.1:3306", "winmachstat", "winmachstat", "winmachstat123");

			}
			catch (sql::SQLException &e) {
			}
		}

	};
}