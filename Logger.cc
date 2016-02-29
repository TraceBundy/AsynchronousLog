#include "Logger.h"
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <memory>
namespace snow
{
    __thread char times[32];
    __thread char thread_tag[128];

    const char *logLevelString[NUM_OF_LEVEL] =
    {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "FATAL",
    };



    Logger::Logger(std::string basename, size_t rollSize, Level level)
        :level_(level),
        lasttime_(0),
        asynLogger_(basename, rollSize)
    {
        asynLogger_.start();
    }

    Logger::~Logger() {}

    void Logger::writeLine(Level level, const char *file, int line, const char *func, const char *format, ...)
    {
        std::string buffer;
        buffer.reserve(kBufferSize);
        char prefix[1024];
        formateTime();
        int len = snprintf(prefix, sizeof(prefix), "[%s] %s %s:%d:%s %s - ",
                            thread_tag, times, file, line,
                            func ? func : "", logLevelString[level]);
        buffer.append(prefix, len);
        va_list valist;
        va_start(valist, format);
        char log[4000];
        if ((len = vsnprintf(log, sizeof(log), format, valist)) > static_cast<int>(sizeof(log)))
        {
            char *log = static_cast<char*>(malloc(len));
            if (log)
            {
                vsnprintf(log, len, format, valist);
                buffer.append(log, len);
                free(log);
            }
        }
        else
        {
            buffer.append(log, len);
        }
        va_end(valist);
        buffer.append("\n");
        asynLogger_.append(buffer.c_str(), buffer.length());
    }


    void Logger::formateTime()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        time_t seconds = tv.tv_sec;
        if (seconds != lasttime_)
        {
            struct tm tm_time;
            ::gmtime_r(&seconds, &tm_time);
            snprintf(times, sizeof(times), "%04d%02d%02d-%02d%02d%02d",
                    tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
            lasttime_ = seconds;
        }
    }

    void Logger::setThreadTag(const char *tag)
    {
        snprintf(thread_tag, sizeof(thread_tag), "%s", tag);
    }

    void Logger::clearThreadTag()
    {
        memset(thread_tag, sizeof(thread_tag), 0);
    }
}
