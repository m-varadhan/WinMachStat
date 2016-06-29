#pragma once

#include <string>

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>

/*
Include directly the different
headers from cppconn/ and mysql_driver.h + mysql_util.h
(and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


namespace WinMachStat {
	template <typename T>
	class DBConnect {
	public:
		DBConnect() {
		}

		DBConnect(std::string dburl, std::string domain, std::string username, std::string password) {
			Connect(dburl,domain,username,password)
		}

		~DBConnect() {
			if(db)	delete db;
		}

		int Connect(std::string dburl, std::string domain, std::string username, std::string password)
		{
			if (db) {
				return 0;
			} else {
				db = new T(dburl, password, username, password);
				if (db) db->connect();
			}
		}

		Execute(std::string sql) {
			if(db) *db << sql;
		}
	private:
		T *db;
	};

	class Mysql {
	private:
		std::string user;
		std::string passwd;
		std::string domain;
		std::string db;

		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
	public:
		Mysql(std::string _dburl, std::string _db, std::string _user, std::string _passwd) :
			domain(_dburl), user(_user), passwd(_passwd), db(_db) {
			driver = get_driver_instance();
		}

		~Mysql() {
			DisConnect();
		}

		void DisConnect() {
			if(con) delete con;
		}

		void Connect() {
			if(driver) {
				con = driver->connect(domain.c_str(), user.c_str(), passwd.c_str());
				con->setSchema(db.c_str());
			}
		}

		void operator <<(std::string sql) {
			if (con) {
				stmt = con->createStatement();
				stmt->execute(sql.c_str());
				delete stmt;
			}
		}

	};

	class DBConnectFactory {
	public:
		template<typename T>
		Database<T> buildDBConnect() {
			return Database<T>();
		}
	};
}
