#include "hzpch.h"
#include "Hazel/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Hazel {
	Ref<spdlog::logger> Log::s_CoreLogger = nullptr;
	Ref<spdlog::logger> Log::s_ClientLogger = nullptr;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		// 控制台日志输出
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

		// 文件日志输出
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Hazel.log", true));

		// 设置输出格式
		logSinks[0]->set_pattern("%^[%c] [%l] %n: %v%$");
		logSinks[1]->set_pattern("[%c] [%l] %n: %v");

		// 同时向控制台和文件输出
		s_CoreLogger = std::make_shared<spdlog::logger>("HAZEL", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}
}
