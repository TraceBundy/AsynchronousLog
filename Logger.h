#pragma once
#include "AsynchronousLogger.h"

namespace snow
{
    enum Level
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL,
        NUM_OF_LEVEL
    };
    class Logger
    {
        public:
            Logger(std::string basename, size_t rollSize, Level level);
            ~Logger();
            void writeLine(Level level, const char *file, int line, const char *func, const char *format, ...);
            void setLogLevel(Level level) { level_ = level; }
            void formateTime();
            void setThreadTag(const char *tag);
            void clearThreadTag();
        private:
            const static int kBufferSize = 4000;
            Level level_;
            time_t lasttime_;
            AsynchronousLogger asynLogger_;
    };
}
