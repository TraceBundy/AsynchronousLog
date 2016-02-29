#pragma once
#include <string>

namespace snow
{
    class LogFile
    {
        public:
            LogFile(std::string &basename, size_t rollSize);
            ~LogFile();
            void append(const char *log, int len);
            void flush();
        private:
            bool rollFile();
            std::string getFileName();
        private:
            FILE *fp_;
            int count_;
            std::string basename_;
            size_t curSize_;
            size_t rollSize_;
    };
}
