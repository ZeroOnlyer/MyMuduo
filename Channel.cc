#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include <sys/epoll.h>

const int Channel::KNoneEvent = 0;
const int Channel::KReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::KWriteEvent = EPOLLOUT;

//EventLoop:ChannelList Poller
Channel::Channel(EventLoop *loop, int fd) : loop_(loop),
                                            fd_(fd),
                                            events_(0),
                                            revents_(0),
                                            index_(-1),
                                            tied_(false)
{
}

Channel::~Channel()
{
}

//channel的tie方法什么时候调用过? 一个TcpConnection新连接创建的时候 TcpConnection=》channel
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

//当改变channel所表示fd的events事件后，update负责在poller里面更改fd相应的事件epoll_ct1
//EventLoop=》ChannelList Poller
void Channel::update()
{
    /*
    通过channel所属的eventloop，调用poller的相应方法，注册fd的events事件
    add code...*/
    loop_->updateChannel(this);
}

//在channel所属的Eventloop中，把当前的channel删除掉
void Channel::remove()
{
    //add code
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents: %d\n", revents_);

    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }
    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }
    if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }
    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
}