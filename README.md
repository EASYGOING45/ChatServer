# README

## 基于C++11和Muduo网络库的仿制QQ聊天服务器

## 项目简介

- 项目：基于C++11和Muduo网络库的仿制QQ聊天服务器
- 开发者：刘易行 24届毕业生 博客地址：www.happygoing.cc
- 主要功能：实现类似于QQ聊天的基本后端功能，包括但不限于：注册、登录、单对单聊天、创建群组、群聊等功能
- 平台：Linux、Windows
- 编译器版本：g++7.3及以上、cmake
- 集群聊天服务器 使用Nginx中的TCP负载均衡模块
- 数据库：MySQL5.27、Redis作缓存（发布订阅消息队列）
- 支持跨服务器通信

## 技术栈

-  Json序列化和反序列化
- muduo网络库二次开发
- nginx源码编译安装与TCP负载均衡模块配置部署
- redis缓存服务器的了解与实践
- 基于发布-订阅的服务器中间件redis消息队列编程实践
- MySQL的C++ API编程
- CMake与Linux C++网络编程开发

## Json序列化与反序列化

​	Json是一种轻量级的数据交换格式（也叫数据序列化方式）。

​	本项目中使用nlohmann的`JSON For Modern C++`进行实现

​	包含头文件`json.hpp`后即可，以封装注册报文为例：

```C++
// 注册业务 name pwd
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "User Doing Reg Service!";

    // 解析传送来的js序列化数据 反序列化
    string name = js["name"];
    string pwd = js["password"];

    // 构造User对象
    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        // 如果注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}
```

## muduo网络库编程

### 网络服务器编程模型概要

1. accept + read/write  非并发
2. accept + fork - process-pre-connection  适合并发连接数不大，计算任务工作量大于fork的开销
3. accept + thread thread-pre-connection  比方案2的开销小了一点，但是并发造成线程堆积过多
4. reactors in threads - one loop per thread
   1. one loop per thread，
   2. 有一个main reactor负载accept连接，然后把连接分发到某个sub reactor（采用round-robin的方式来选择sub reactor）
   3. 该连接的所用操作都在那个sub reactor所处 的线程中完成。多个连接可能被分派到多个线程中，以充分利用CPU。
5. reactors in process - one loop pre process
   1. nginx服务器的网络模块设计，基于进程设计，采用多个Reactors充当I/O进程和工作进程，通过一把 accept锁，完美解决多个Reactors的“惊群现象”。

### reactor模型

- 事件驱动（event handling）
- 可以处理一个或多个输入源（one or more inputs）
- 通过Service Handler同步的将输入事件（Event）采用多路复用分发给相应的Request Handler（多个）处理
- ![image-20230129132536299](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230129132536299.png)

### 服务器编程实现

```C++
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

// 聊天服务器ChatServer主类
class ChatServer
{
public:
    // 构造函数 初始化ChatServer
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);

    // 启动服务器接口
    void start();

private:
    // 上报链接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &);

    // 上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &, Buffer *, Timestamp);
    TcpServer _server; // 组合muduo库，实现服务器功能的对象
    EventLoop *_loop;  // 指向事件循环对象的指针
};

#endif
```

```C++
#include "ChatServer.hpp"
#include "json.hpp"
#include "ChatService.hpp"
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

// 初始化聊天服务器对象
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg),
      _loop(loop)
{
    // 注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 客户端断开链接
    if (!conn->connected())
    {
        // 需要进行处理
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown(); // 断开
    }
}

// 上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();

    // 测试 添加json打印代码
    cout << buf << endl;

    // 数据反序列化
    json js = json::parse(buf);
    // 目的：完全解耦网络模块的代码和业务模块的代码
    // 通过js["msgid"]获取->业务handler句柄-> conn js time
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());

    // 回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn, js, time);
}
```

## 服务器集群

### nginx配置TCP负载均衡模块

```shell
~/package/nginx-1.12.2# ./configure --with-stream
~/package/nginx-1.12.2# make && make install
```

编译完成后，默认安装在了/usr/local/nginx目录。

```shell
~/package/nginx-1.12.2$ cd /usr/local/nginx/ 
/usr/local/nginx$ ls
```

![.](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230127213721381.png)

![image-20230127213704041](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230127213704041.png)

![image-20230127213608749](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230127213608749.png)

## 服务器中间件-基于发布-订阅的Redis

![image-20230128211343580](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230128211343580.png)

![image-20230129132817252](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230129132817252.png)

## 数据库表设计

![image-20230129133055275](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230129133055275.png)

![image-20230129133116868](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230129133116868.png)

## 业务测试

### 注册业务

![image-20230118111612181](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230118111612181.png)

### 登录业务

登录Json

`{"msgid":1,"id":23,"password":"123456"}`

![image-20230119103901514](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230119103901514.png)

![image-20230119103843333](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230119103843333.png)

## 异常退出

登录json`{"msgid":1,"id":16,"password":"123456"}`

处理客户端异常退出

![image-20230122213801594](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230122213801594.png)

{"msgid":1,"id":21,"password":"123456"}

{"msgid":5,"id":19,"from":"liu shuo","to":21,"msg":"hello there"}

## 点对点聊天

![image-20230123113511395](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230123113511395.png)

## 离线消息存储

`{"msgid":1,"id":21,"password":"123456"}`

> 测试json
>
> - 用户一
>   - 登录:`{"msgid":1,"id":21,"password":"123456"}`
>   - 发消息:`{"msgid":5,"id":21,"from":"gao yang","to":19,"msg":"hello there"}`
> - 用户二
>   - 登录`{"msgid":1,"id":19,"password":"123456"}`
>   - ![image-20230123223119093](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230123223119093.png)

```C++
// 提供离线消息表的操作接口方法
class OfflineMsgModel
{
public:
    // 存储用户的离线消息
    void insert(int userid, string msg); // userid为目标id

    // 删除用户的离线消息 转发完后删除
    void remove(int userid);

    // 查询用户的离线消息
    vector<string> query(int userid);
};
```

注意线程安全问题

离线消息存储业务功能实现

![image-20230123223037946](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230123223037946.png)

![image-20230123223059210](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230123223059210.png)

## 添加好友业务

用户登录报文：`{"msgid":1,"id":13,"password":"123456"}`

添加好友报文:`{"msgid":6,"id":13,"friendid":23}`

![image-20230124211625979](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230124211625979.png)