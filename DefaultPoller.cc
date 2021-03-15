#include "Poller.h"
#include "EPollPoller.h"

#include <stdlib.h>

Poller *Poller::newDefaultPoller(EventLoop *Loop)
{
    if (::getenv("MUDUO_USE_POLL"))
    {
        return nullptr; //生成poll的实例
    }
    else
    {
        return new EPollPoller(Loop); //生成epoll的实例
    }
}