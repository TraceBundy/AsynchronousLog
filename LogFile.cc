#include "LogFile.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
namespace snow
{
    LogFile::LogFile(std::string &basename, size_t rollSize)
        :count_(-1),
        basename_(basename),
        rollSize_(rollSize)
    {
        rollFile();
    }

    LogFile::~LogFile()
    {
        fclose(fp_);
    }

    void LogFile::append(const char *log, int len)
    {
        fwrite_unlocked(log, 1, len, fp_);
        curSize_ += len;
        if (curSize_ > rollSize_)
        {
            rollFile();
            curSize_ = 0;
        }
    }


    void LogFile::flush()
    {
        fflush(fp_);
    }


    bool LogFile::rollFile()
    {
        if (fp_) { fclose(fp_); }
        ++count_;
        std::string filename = getFileName();
        fp_ = ::fopen(filename.c_str(), "ae");
        if (!fp_)
        {
            fprintf(stderr, "open logfile failed errno:%d, error:%s", errno, strerror(errno));
            return false;
        }
        return true;
    }

    std::string LogFile::getFileName()
    {
        std::string filename = basename_;
        time_t now = time(NULL);
        struct tm tm;
        char timebuf[20] = {0};
        localtime_r(&now, &tm);
        strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm);
        filename += timebuf;
        return filename;
    }
}
