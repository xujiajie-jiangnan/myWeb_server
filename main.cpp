#include "requestData.h"
#include "epoll.h"
#include "threadpool.h"
#include "util.h"
#include <sys/epoll.h>
#include <queue>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <memory>
#include "Logging.h"
#include<getopt.h>

using namespace std;

static const int MAXEVENTS = 5000;
static const int LISTENQ = 2048;

const int QUEUE_SIZE = 65535;

const int ASK_STATIC_FILE = 1;
const int ASK_IMAGE_STITCH = 2;

const int TIMER_TIME_OUT = 500;

void acceptConnection(int listen_fd, int epoll_fd, const string &path);

int socket_bind_listen(int port)
{
    // 检查port值，取正确区间范围
    if (port < 1024 || port > 65535)
        return -1;

    // 创建socket(IPv4 + TCP)，返回监听描述符
    int listen_fd = 0;
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    // 消除bind时"Address already in use"错误
    int optval = 1;
    if(setsockopt(listen_fd, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        return -1;

    // 设置服务器IP和Port，和监听描述副绑定
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);
    if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        return -1;

    // 开始监听，最大等待队列长为LISTENQ
    if(listen(listen_fd, LISTENQ) == -1)
        return -1;

    // 无效监听描述符
    if(listen_fd == -1)
    {
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}



int main(int argc,char* argv[]){
    int THREADPOOL_THREAD_NUM = 4;
    string logPath = "./LogFiles/WebServer.log";
    int PORT = 8888;

    // parse args
    // 运行指令和参数：./WebServer [-t thread_numbers] [-p port] [-l log_file_path(should begin with '/')]   "/LogFiles/xjj_web_server.log"
    int opt;
    const char *str = "t:l:p:";
    while ((opt = getopt(argc, argv, str)) != -1) {
        switch (opt) {
        case 't': {
            THREADPOOL_THREAD_NUM = atoi(optarg);
            break;
        }
        case 'l': {
            logPath = optarg;
            if (logPath.size() < 2 || optarg[0] != '/') {
            printf("logPath should start with \"/\"\n");
            abort();
            }
            break;
        }
        case 'p': {
            PORT = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
    
    //日志之后，想加的时候再加吧，现在就先做个简单的测试
    //LOG << "yingyingying";
    //LOG << 654 << 3.2 << 0 << string("fg") << true;
    // #ifndef _PTHREADS
    //     LOG << "_PTHREADS is not defined !";
    // #endif
    handle_for_sigpipe();
    if (Epoll::epoll_init(MAXEVENTS, LISTENQ) < 0)
    {
        perror("epoll init failed");
        return 1;
    }
    if (ThreadPool::threadpool_create(THREADPOOL_THREAD_NUM, QUEUE_SIZE) < 0)
    {
        printf("Threadpool create failed\n");
        return 1;
    }
    int listen_fd = socket_bind_listen(PORT);
    if (listen_fd < 0) 
    {
        perror("socket bind failed");
        return 1;
    }
    if (setSocketNonBlocking(listen_fd) < 0)
    {
        perror("set socket non block failed");
        return 1;
    }
    shared_ptr<RequestData> request(new RequestData());
    request->setFd(listen_fd);
    if (Epoll::epoll_add(listen_fd, request, EPOLLIN | EPOLLET) < 0)
    {
        perror("epoll add error");
        return 1;
    }
    
    while (true)
    {
        //sleep(10);
        Epoll::my_epoll_wait(listen_fd, MAXEVENTS, -1);
        
        //ThreadPool::threadpool_destroy();
        //break;
    }
    return 0;
}