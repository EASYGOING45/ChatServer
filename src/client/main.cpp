#include "json.hpp"
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono> //处理时间相关
#include <ctime>
#include <unordered_map>
#include <functional> //定义了C++标准中多个用于表示函数对象(function object)的类模板
using namespace std;
using json = nlohmann::json;

// Linux网络编程常用头文件
#include <unistd.h> //提供对 POSIX 操作系统 API 的访问功能的头文件的名称
#include <sys/socket.h>
#include <sys/types.h> //基本系统数据类型
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <atomic>

#include "group.hpp"
#include "user.hpp"
#include "public.hpp"

// 记录当前系统登录的用户信息
User g_currentUser;

// 记录当前登录用户的好友列表信息
vector<User> g_currentUserFriendList;

// 记录当前登录用户的群组列表信息
vector<Group> g_currentUserGroupList;

// 控制主菜单的页面程序
bool isMainMenuRunning = false; // 初始为false

// 读写线程间通信
sem_t rwsem; // sem_t为信号量类型

// 记录登录状态
atomic_bool g_isLoginSuccess{false}; // sta::atomic 最基本的原子整数类型

// 接收线程
void readTaskHandler(int clientfd);

// 获取系统时间 用于聊天服务前缀
string getCurrentTime();

// 主聊天页面程序
void mainMenu(int);

// 显示当前登录成功的用户的基本信息
void showCurrentUserData();

// 聊天客户端程序实现 main线程用于发送（写）线程 子线程为接收（读）线程
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        // 错误输入
        cerr << "Input Command Invalid! Example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递来的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建client端的socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        cerr << "Socket Create Error,Please Check" << endl;
        exit(-1);
    }

    // 填写client需要连接的server的信息  ip  port
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    // client和server进行连接
    if (-1 == connect(clientfd, (sockaddr *)&server, sizeof(sockaddr_in)))
    {
        cerr << "Connect Server Error" << endl;
        close(clientfd);
        exit(-1);
    }

    // 初始话读写线程通信所使用的信号量
    sem_init(&rwsem, 0, 0);

    // 连接服务器成功，启动接收子线程
    std::thread readTask(readTaskHandler, clientfd); // pthread_create
    readTask.detach();                               // 分离 pthread_detach

    // main线程
    return 0;
}