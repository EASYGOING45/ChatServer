#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
// 用来进行 线程同步 ， 保护数据 的， 防止不同线程对同一数据同时进行处理
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
#include "usermodel.hpp"
using json = nlohmann::json;

// 处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

class ChatService
{
public:
    // 获得单例对象的接口函数
    static ChatService *instance();

    // 登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);

private:
    ChatService(); // 构造函数 放在私有中

    // 数据操作类对象
    UserModel _userModel;

    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;
};
#endif