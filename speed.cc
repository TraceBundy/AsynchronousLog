#include "AsynchronousLogger.h"
#include "Logging.h"
#include <stdio.h>
#include <string.h>
#include <thread>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
using namespace snow;
int main()
{
    {
        LOG_INIT("./hello", 1024*1024*1024, INFO);
    }
    char buf[220] = "1234567890123456789012345678901234567890123456789012345678901234567890\n";
    int line = 10000;
    while (1)
    {
        struct timeval start, end;
        gettimeofday(&start, NULL);
        for (int i = 0; i < line; ++i)
        {
            LOG_INFO("%s", buf);
        }
        gettimeofday(&end, NULL);
        printf("speed :%lfus/s\n", static_cast<float>(line)/((end.tv_sec*1000000+end.tv_usec)-(start.tv_sec *1000000+start.tv_usec)));
        usleep(100000);
    }
    return 0;
}
