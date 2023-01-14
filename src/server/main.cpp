#include "ChatServer.hpp"
#include <iostream>

using namespace std;

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);          // 暂时写死
    ChatServer server(&loop, addr, "ChatServer"); // 构造对象
    server.start();                               // 启动服务
    loop.loop();                                  // 开始事件循环
    return 0;
}