
## Introduction  

本项目为C++11编写的Web服务器，解析了get、head请求，可处理静态资源，支持HTTP长连接，并实现了异步日志，记录服务器运行状态。  

## Envoirment  
* OS: Ubuntu 18.04
* Complier: g++ 7.5.0

## Build

	./build.sh

## Usage

	./WebServer [-t thread_numbers] [-p port] [-l log_file_path(should begin with '/')]

## Technical points
* 采用Reactor+半同步/半异步的模式框架，结合Epoll(ET)的I/O多路复用技术和非阻塞式模型
* 使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销
* 小根堆+定时器剔除超时非活跃连接
* 主线程中epoll_wait监听，负责listenfd和accept请求，并以Round Robin的方式分发给其它IO线程(兼计算线程)，锁的争用只会出现在主线程和某一工作线程中
* 使用eventfd实现了线程的异步唤醒
* 参考muduo的日志库，使用双缓冲区技术实现了简单的异步日志系统
* 使用智能指针，mutex_guard等RAII机制,减少内存泄漏的发生
* 使用状态机解析了HTTP请求,支持长连接和短连接
 

## 代码统计

![cloc](https://github.com/xujiajie-jiangnan/myWeb_server/blob/master/Test_pics/cloc.png)


## Others
除了项目基本的代码，进服务器进行压测时，copy_by(https://github.com/linyacool/WebBench);  对开源测试工具Webbench增加了Keep-Alive选项和测试功能;


