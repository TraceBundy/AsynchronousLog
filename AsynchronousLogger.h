#pragma once
#include "LogFile.h"
#include "HashBucket.h"
#include <string>
#include <thread>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
namespace snow
{
    class AsynchronousLogger
    {
        public:
            AsynchronousLogger(const std::string &basename, int rollsize, int interval = 3);
            ~AsynchronousLogger();
            void start();
            void stop();
            void append(const char *log, int len);
            void writeAll();
            void writeLog(Bucket &bucket, bool waitfull = true);
        private:
            void threadFunc();
            typedef std::shared_ptr<std::string> Buffer;
            typedef std::vector<Buffer> BufferVector;
            const static int kBufferSize = 1024*1024*4;
            const static int kHashBucketSize = 4;
        private:
            bool running_;
            std::string basename_;
            int rollSize_;
            int intervalTime_;
            Buffer newBuffer1_;
            Buffer newBuffer2_;
            BufferVector buffers_;
            std::thread thread_;
            LogFile file_;
            HashBucket bucket_;
            std::map<int , Bucket*> bucketMap_;
    };
}
