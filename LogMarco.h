#pragma once
#include <memory>
#include "Logger.h"

namespace snow
{
    extern std::shared_ptr<Logger> g_logger;

#define LOG_INIT(name, size, level) \
            g_logger = std::make_shared<Logger>(name, size, level);

#define LOG_DEBUG(format, ...) \
            g_logger->writeLine(DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);

#define LOG_INFO(format, ...) \
            g_logger->writeLine(DEBUG, __FILE__, __LINE__, NULL, format, ##__VA_ARGS__);

#define LOG_WARNING(format, ...) \
            g_logger->writeLine(WARNING, __FILE__, __LINE__, NULL, format, ##__VA_ARGS__);

#define LOG_ERROR(format, ...) \
            g_logger->writeLine(ERROR, __FILE__, __LINE__, NULL, format, ##__VA_ARGS__);

#define LOG_FATAL(format, ...) \
            g_logger->writeLine(FATAL, __FILE__, __LINE__, NULL, format, ##__VA_ARGS__);
}
