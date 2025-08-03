#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <fstream>
#include <optional>
#include <chrono>
#include <sstream>
#include <mutex>
#include <cstring>
#include <senkaid/utils/config/root.hpp>

namespace senkaid::debug 
{

SENKAID_DIAGNOSTIC_PUSH
SENKAID_IGNORE_WARNING_UNUSED_FUNCTION

enum class LogLevel : std::uint8_t
{
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4
};

class Logger
{
public:
    constexpr Logger() : _level(LogLevel::Debug) {};

    constexpr Logger(LogLevel level) : _level(level) {};
    constexpr Logger(LogLevel level, std::string file) : _level(level), _file(file) {};

    SENKAID_FORCE_INLINE static Logger& instance()
    {
        static Logger logger;
        return logger;
    }

    SENKAID_FORCE_INLINE void init()
    {
        if (!_file.empty()) 
        {
            std::lock_guard lock(_mutex);
            if (!_stream.has_value())
                _stream.emplace(_file);
        }
    }

    void log(const std::string_view& message, LogLevel level = LogLevel::Info, const char* file = nullptr, int line = 0)
    {
        if (static_cast<std::uint8_t>(level) > static_cast<std::uint8_t>(_level)) {
            return;
        }

        const auto now = std::chrono::system_clock::now();
        const auto time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%F %T", std::localtime(&time_t));

        std::ostringstream oss;
        oss << buffer << " senkaid[" << _level_to_string(level) << "]: " << message << " " << (file ? file : "unknown file") << " on line: " << line << '\n';

        std::lock_guard lock(_mutex);
        if (_stream.has_value()) {
            *_stream << oss.str();
            _stream->flush();
        } else {
            std::fputs(oss.str().c_str(), stdout);
            std::fflush(stdout);
        }
        // output: <time> senkaid[<level>]: message
    }

    SENKAID_FORCE_INLINE void close()
    {
        std::lock_guard lock(_mutex);
        if (_stream.has_value()) 
        {
            _stream->flush();
            _stream.reset();
        }
    }

private:
    // Fields
    LogLevel _level;
    std::string _file;
    mutable std::mutex _mutex;
    std::optional<std::ofstream> _stream;

    // Functions
    constexpr SENKAID_FORCE_INLINE std::string_view _level_to_string(LogLevel level) const
    {
        switch(level)
        {
            case LogLevel::Error: return "ERROR";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Info: return "INFO";
            case LogLevel::Debug: return "DEBUG";
            default: return "UNKNOWN";
        }
    }

}; // Logger

SENKAID_DIAGNOSTIC_POP

} // namespace senkaid::debug

#define SENKAID_LOG(level, message) \
    ::senkaid::debug::Logger::instance().log(\
        message, \
        ::senkaid::debug::LogLevel::level, \
        __FILE__, \
        __LINE__ \
    )

#define SENKAID_LOG_ERROR(message)      SENKAID_LOG(Error, message)
#define SENKAID_LOG_WARNING(message)    SENKAID_LOG(Warning, message)
#define SENKAID_LOG_INFO(message)       SENKAID_LOG(Info, message)
#define SENKAID_LOG_DEBUG(message)      SENKAID_LOG(Debug, message)
