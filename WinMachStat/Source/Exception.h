#pragma once

#include <errno.h>
#include <string>
#include <sstream>

namespace WinMachStat {
	class ExceptionFormatter {
		std::stringstream efstream;
		ExceptionFormatter(const ExceptionFormatter &) = delete;
		ExceptionFormatter & operator = (ExceptionFormatter &) = delete;
	public:
		ExceptionFormatter() {}
		~ExceptionFormatter() {}

		template <typename Type>
		ExceptionFormatter &operator <<(const Type &v) {
			efstream << v;
			return *this;
		}

		std::string str() const { return efstream.str(); }
		operator std::string() const { return efstream.str(); }

		enum ConvertToString
		{
			to_str
		};
		std::string operator >> (ConvertToString) { return efstream.str(); }
	};

	class Exception {
		int errnum;
		std::string errmsg;
	public:
		Exception(int _errnum, std::string _errmsg) : errnum(_errnum) \
			, errmsg(_errmsg) {}
		Exception(std::string _errmsg) : errmsg(_errmsg) {}
		Exception(void) = delete;
		std::string getErrorMsg() { return errmsg; }
		int getErrorNo() { return errnum; }
	};
}
