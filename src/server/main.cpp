#include "ChatServer.hpp"
#include "ChatService.hpp"
#include <iostream>
#include <signal.h>

using namespace std;

// 处理服务器异常退出结束后 重置user的状态
void resetHandler(int)
{
    ChatService::instance()->reset();
}

int main()
{
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);          // 暂时写死
    ChatServer server(&loop, addr, "ChatServer"); // 构造对象
    server.start();                               // 启动服务
    loop.loop();                                  // 开始事件循环
    return 0;
}