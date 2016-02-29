#include "HashBucket.h"
#include <unistd.h>
#include <sys/eventfd.h>
namespace snow
{
    Bucket::Bucket()
        :curBuffer_(new std::string),
        nextBuffer_(new std::string)
    {
        curBuffer_->reserve(kBufferSize);
        nextBuffer_->reserve(kBufferSize);
        efd_ = eventfd(0, EFD_NONBLOCK);
    }

    Bucket::~Bucket()
    {
        close(efd_);
    }

    void Bucket::sendEvent()
    {
        uint64_t u = 100;
        write(efd_, &u, sizeof(u));
    }

    void Bucket::readEvent()
    {
        uint64_t u = 0;
        read(efd_, &u, sizeof(u));
    }
}

