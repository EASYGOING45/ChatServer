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
    // main用于接收用户输入 负责发送数据
    for (;;)
    {
        // 显示首页面菜单 登录、注册、退出等操作
        cout << "========================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "========================" << endl;
        cout << "Your Choice:";
        int choice = 0;
        cin >> choice; // 注意 需要清楚回车
        cin.get();     // 处理掉缓冲区残留的回车

        switch (choice)
        {
        case 1:
        {
            // login业务
            int id = 0;
            char pwd[50] = {0};
            cout << "userid:";
            cin >> id;
            cin.get(); // 处理回车
            cout << "user's password:";
            cin.getline(pwd, 50); // 使用getline读取

            // 序列化 封装json
            json js;
            js["msgid"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = pwd;
            string request = js.dump();

            g_isLoginSuccess = false;

            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (len == -1)
            {
                cerr << "Send Login Msg Error:" << request << endl;
            }

            sem_wait(&rwsem); // 等待信号量由子线程处理完登录的响应消息后 通知这里 继续

            if (g_isLoginSuccess)
            {
                // 进入聊天主菜单页面
                isMainMenuRunning = true;
                mainMenu(clientfd);
            }
        }
        break;
        case 2:
        {
            // register业务
            char name[50] = {0};
            char pwd[50] = {0};
            cout << "username:";
            cin.getline(name, 50);
            cout << "user's password:";
            cin.getline(pwd, 50);

            // 序列化 封装json
            json js;
            js["msgid"] = REG_MSG;
            js["name"] = name;
            js["password"] = pwd;
            string request = js.dump();

            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (len == -1)
            {
                cerr << "Send Reg Msg Error:" << request << endl;
            }
            sem_wait(&rwsem); // 等待信号量，子线程处理完注册消息会通知
        }
        break;
        case 3: // quit业务
            close(clientfd);
            sem_destroy(&rwsem); // 销毁信号量
            exit(0);
        default:
            cerr << "Invalid Input,Try Again";
            break;
        }
    }
    return 0;
}
