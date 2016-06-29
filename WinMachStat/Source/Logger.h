#pragma once

#define ELPP_STL_LOGGING
#define ELPP_PERFORMANCE_MICROSECONDS
#define ELPP_LOG_STD_ARRAY
#define ELPP_LOG_UNORDERED_MAP
#define ELPP_UNORDERED_SET
#define ELPP_THREAD_SAFE
#define ELPP_WINSOCK2

#include "Config.h"
#include "easylogging++.h"
#include <string>

namespace WinMachStat {
class Logger {
	Logger::Logger() = delete;
	static bool init;

	public:
	static bool InitLogger(std::string config) {
		if (!init) {
			el::Configurations conf(config.c_str());
			el::Loggers::reconfigureLogger("default", conf);
		}

		return (init = true);
	}
};

}
