#include "stdafx.h"
//#include "DBConnect.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WinMachStatUnitTest
{
	TEST_CLASS(DBConnect)
	{
	public:

		TEST_METHOD(DBConnectMysql)
		{
			//typedef WinMachStat::Mysql MysqlT;
			//WinMachStat::DBConnect<MysqlT> *db = WinMachStat::DBConnectFactory::buildDBConnect<MysqlT>();
			//db->Connect("tcp://127.0.0.1:3306", "winmachstat", "winmachstat", "winmachstat123");
		}

	};
}