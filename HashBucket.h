#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace snow
{
    struct Bucket
    {
        public:
            typedef std::shared_ptr<std::string> Buffer;
            typedef std::vector<Buffer> BufferVector;
            const static int kBufferSize = 4*1024*1024;
        public:
            Bucket();
            ~Bucket();
            void sendEvent();
            void readEvent();
        public:
            int efd_;
            std::mutex mutex_;
            std::condition_variable cond_;
            Buffer curBuffer_;
            Buffer nextBuffer_;
            BufferVector buffers_;
    };

    typedef std::vector<Bucket> HashBucket;
}
