#include "AsynchronousLogger.h"
#include "HashBucket.h"
#include <string.h>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <sys/time.h>

namespace snow
{
    __thread pid_t tid = 0;


    AsynchronousLogger::AsynchronousLogger(const std::string &basename, int rollSize, int interval)
        :basename_(basename),
        rollSize_(rollSize),
        intervalTime_(interval),
        newBuffer1_(new std::string()),
        newBuffer2_(new std::string()),
        file_(basename_, rollSize),
        bucket_(5)
    {
        newBuffer1_->reserve(kBufferSize);
        newBuffer2_->reserve(kBufferSize);
        buffers_.reserve(16);
        for (size_t i = 0; i < bucket_.size(); ++i)
        {
            bucketMap_[bucket_[i].efd_] = &bucket_[i];
        }
    }

    AsynchronousLogger::~AsynchronousLogger()
    {
        if (running_)
        {
            stop();
        }
    }

    void AsynchronousLogger::start()
    {
        std::thread thread(std::bind(&AsynchronousLogger::threadFunc, this));
        thread_.swap(thread);
        running_ = true;
    }

    void AsynchronousLogger::stop()
    {
        running_ = false;
        thread_.join();
    }

    void AsynchronousLogger::append(const char *log, int len)
    {
        if (tid == 0)
        {
        tid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
        int bucketNum = static_cast<int>(tid % bucket_.size());

        Bucket &bucket = bucket_[bucketNum];
        std::unique_lock<std::mutex> lock(bucket.mutex_);

        if (kBufferSize - static_cast<int>(bucket.curBuffer_->length()) > len)
        {
            bucket.curBuffer_->append(log, len);
        }
        else
        {
            bucket.buffers_.push_back(bucket.curBuffer_);
            if (bucket.nextBuffer_.get())
            {
                bucket.curBuffer_ = std::move(bucket.nextBuffer_);
            }
            else
            {
                bucket.curBuffer_.reset(new std::string());
                bucket.curBuffer_->reserve(kBufferSize);
            }
            bucket.curBuffer_->append(log,len);
            bucket.sendEvent();
            bucket.cond_.notify_one();
        }
    }

    void AsynchronousLogger::threadFunc()
    {
        std::vector<pollfd> pollfds;
        pollfds.resize(bucket_.size());
        for (size_t i = 0; i < bucket_.size(); ++i)
        {
            pollfds[i].fd = bucket_[i].efd_;
            pollfds[i].events = POLLIN;
        }
        while(running_)
        {
            int ret = poll(&*pollfds.begin(), 5, 3000);
            if (ret < 0)
            {
                fprintf(stderr, "poll log error:%d\n", errno);
            }
            if (ret == 0)
            {
                for (int i = 0; i< 5; i++)
                {
                printf("fd:%d, re:%d\n", pollfds[i].fd, pollfds[i].revents);
                bucket_[i].readEvent();
                }
                writeAll();
            }
            else
            {
                for (int num = ret, i = 0; num > 0 && i < static_cast<int>(pollfds.size()); i++)
                {
                    if (pollfds[i].revents & POLLIN)
                    {
                        Bucket &bucket = *bucketMap_[pollfds[i].fd];
                        bucket.readEvent();
                        writeLog(bucket);
                        --num;
                    }
                }
            }
        }
        writeAll();
    }

    void AsynchronousLogger::writeAll()
    {
        for (size_t i = 0; i < bucket_.size(); ++i)
        {
            writeLog(bucket_[i], false);
        }
    }

    void AsynchronousLogger::writeLog(Bucket &bucket, bool waitfull)
    {
        {
            std::unique_lock<std::mutex> lock(bucket.mutex_);
            if (waitfull)
            {
                while (bucket.buffers_.empty())
                {
                    bucket.cond_.wait(lock);
                }
            }
            bucket.buffers_.push_back(bucket.curBuffer_);
            bucket.curBuffer_ = std::move(newBuffer1_);
            if (!bucket.nextBuffer_.get())
            {
                bucket.nextBuffer_ = std::move(newBuffer2_);
            }

            buffers_.swap(bucket.buffers_);
        }

        if (buffers_.size() > 10)
        {
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log messages %zd largger buffers\n", buffers_.size()-2);
            fputs(buf, stderr);
            file_.append(buf, strlen(buf));
            buffers_.reserve(2);
        }

        for (size_t i = 0; i < buffers_.size(); ++i)
        {
            file_.append(buffers_[i]->c_str(), buffers_[i]->length());
            Buffer empty(new std::string());
            empty->reserve(kBufferSize);
            buffers_[i] = std::move(empty);
        }

        if (buffers_.size() > 2)
        {
            buffers_.reserve(2);
        }
        if (!newBuffer1_.get())
        {
            newBuffer1_ = buffers_[buffers_.size()-1];
            buffers_.pop_back();
        }
        if (!newBuffer2_.get())
        {
            newBuffer2_ = buffers_[buffers_.size()-1];
            buffers_.pop_back();
        }
    }
}
