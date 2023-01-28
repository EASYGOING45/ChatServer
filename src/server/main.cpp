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

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer"); // 构造对象
    server.start();                               // 启动服务
    loop.loop();                                  // 开始事件循环
    return 0;
}