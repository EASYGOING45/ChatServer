# Muduo开发-Part1

## Muduo网络库开发简单回响服务器

- muduo网络库开发编程，提供了两个主要的类
  - TcpServer：用于编写服务器程序
  - TcpClient：用于编写客户端程序
- epoll+线程池
  - 能够把网络IO代码与业务逻辑代码区分开
    - 用户的连接和断开  用户的可读写事件



- 基于Muduo网络库开发服务器程序
- 1：组合TcpServer对象
- 1：组合TcpServer对象
- 2:创建EventLoop事件循环对象的指针
- 3.明确TcpServer的构造函数所需的参数，输出ChatServer的构造函数
- 4.在当前服务器类的构造函数当中，注册处理连接和处理读写事件的回调函数
- 5.设置合适的服务端线程数量 muduo库会自己分配I/O线程和worker线程

```C++
/*
muduo网络库开发编程，提供了两个主要的类
TcpServer：用于编写服务器程序
TcpClient：用于编写客户端程序

epoll+线程池
能够把网络IO代码与业务逻辑代码区分开
                    用户的连接和断开  用户的可读写事件
*/
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <string>
#include <iostream>
#include <functional>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders; // 参数占位符

// 基于muduo网络库开发服务器程序
/*
1：组合TcpServer对象
2:创建EventLoop事件循环对象的指针
3.明确TcpServer的构造函数所需的参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数当中，注册处理连接和处理读写事件的回调函数
5.设置合适的服务端线程数量 muduo库会自己分配I/O线程和worker线程
*/
class ChatServer
{
public:
    ChatServer(EventLoop *loop,               // 事件循环
               const InetAddress &listenAddr, // IP+Port
               const string &nameArg)         // 服务器名
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        // 给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
        // 给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        // 设置服务器端的线程数量 1个IO线程 3个worker线程
        _server.setThreadNum(4);
    }

    // 开启事件循环
    void start()
    {
        _server.start();
    }

private:
    // 专门处理用户的连接创建和断开 epoll listenfd accept
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:online " << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:offline " << endl;
            conn->shutdown(); // 关闭
            //_loop_>quit();
        }
    }

    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,               // 缓冲区
                   Timestamp time)               // 接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << " time:" << time.toString() << endl;
        conn->send(buf);
    }
    TcpServer _server; // 1
    EventLoop *_loop;  // 2 epoll
};

int main()
{
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatSever");
    server.start(); // listenfd epoll_ctl=epoll
    loop.loop();    // epoll_wait以阻塞方式等待新用户连接，或已连接用户的读写事件
    return 0;
}
```

![image-20230110035145851](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230110035145851.png)

![image-20230110040004245](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230110040004245.png)

![image-20230110040219974](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230110040219974.png)