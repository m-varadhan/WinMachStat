#pragma once

//#pragma comment(lib, "mysqlcppconn.lib")

#include <string>

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>

/*
Include directly the different
headers from cppconn/ and mysql_driver.h + mysql_util.h
(and mysql_connection.h). This will reduce your build time!
*/
#define CPPCONN_PUBLIC_FUNC

#include "mysql_connection.h"
#include "mysql_driver.h"
#include "mysql_error.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "Exception.h"

#include "Logger.h"


namespace WinMachStat {
	class Mysql {
	private:
		std::string user;
		std::string passwd;
		std::string domain;
		std::string db;

		//sql::mysql::MySQL_Driver *driver;
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
	public:
		Mysql(std::string _dburl, std::string _db, std::string _user, std::string _passwd) :
			domain(_dburl), user(_user), passwd(_passwd), db(_db), con(NULL), stmt(NULL), res(NULL) {
			try {
				driver = sql::mysql::get_driver_instance();
			}
			catch (...) {
				throw Exception(ExceptionFormatter() << "Can't get to mysql driver:" << domain << ":" << db);
			}
			LOG(INFO) << "Created Instance of Driver " << db << ":" << con << ":" << stmt << ":" << res << ":";
		}

		~Mysql() {
			DisConnect();
		}

		void DisConnect() {
			if (con != NULL) {
				LOG(INFO) << "Disconnecting DB Mysql";
				delete con;
				con = NULL;
				LOG(INFO) << "Disconnected DB Mysql";
			}
		}

		void Connect() {
			if (driver) {
				LOG(INFO) << "Connecting to DB Mysql:";
				con = driver->connect(domain.c_str(), user.c_str(), passwd.c_str());
				if (!con)
					throw Exception(ExceptionFormatter() << "Can't connect to mysql:" << domain << ":" << db);
				con->setSchema(db.c_str());
				LOG(INFO) << "Connected to DB Mysql:";
			}
		}

		void Execute(std::string sql) {
			if (con) {
				LOG(INFO) << "Executing SQL DB Mysql:" << sql;
				stmt = con->createStatement();
				stmt->execute(sql.c_str());
				delete stmt;
				stmt = NULL;
			}
		}

	};

	template <typename T>
	class DBConnect {
	public:
		DBConnect() : db(NULL){
		}

		DBConnect(std::string dburl, std::string domain, std::string username, std::string password) : db(NULL) {
			Connect(dburl, domain, username, password);
		}

		~DBConnect() {
			if (db) {
				delete db;
				db = NULL;
			}
		}

		int Connect(std::string dburl, std::string domain, std::string username, std::string password)
		{
			if (db) {
				return 0;
			} else {
				db = new T(dburl, domain, username, password);
				if (db) db->Connect();
				LOG(INFO) << "Connected DB:" << dburl ;
			}
			return 0;
		}

		void DisConnect()
		{
			if (db) {
				LOG(INFO) << "Disconnecting DB";
				db->DisConnect();
			}
		}

		void operator <<(std::string sql) {
			LOG(INFO) << "Called Executing SQL DB:" << sql;
			if (db) {
				LOG(INFO) << "Executing SQL DB:" << sql;
				db->Execute(sql);
			}

		}

	private:
		T *db;
	};

	class DBConnectFactory {
	public:
		template<typename T>
		static DBConnect<T> *buildDBConnect() {
			return new DBConnect<T>();
		}
	};
}
