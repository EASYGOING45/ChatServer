#include "ChatService.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <vector>
using namespace std;
using namespace muduo;

// 获取单例对象的接口函数
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 注册消息以及对应的Handler回调操作
ChatService::ChatService()
{
    // 用户基本业务管理相关事件处理回调注册
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志 msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid); // 寻找相关句柄
    if (it == _msgHandlerMap.end())
    {
        // 如果没有找到 返回一个默认的处理器 空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp)
        {
            LOG_ERROR << "msgid:" << msgid << "can't find suitable handler! Error!";
        };
    }
    else
    {
        // 找到了对应的Handler
        return _msgHandlerMap[msgid];
    }
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "User Doing Login Service!";
    int id = js["id"].get<int>();
    string pwd = js["password"];
    User user = _userModel.query(id); // 查询
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            // 该用户已经登录 不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "This Account is already login,please try again!";
            conn->send(response.dump());
        }
        else
        {
            // 登录成功，记录用户连接信息
            {
                // 加锁 保证线程安全
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // 登录成功 更新用户状态信息 state offline=>online
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                // 读取该用户的离线消息后 把该用户的所有离线消息删除掉
                _offlineMsgModel.remove(id);
            }

            conn->send(response.dump());
        }
    }
    else
    {
        LOG_INFO << "User ID not exist or incorrect password!";
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 3;
        response["errmsg"] = "User ID not exist or incorrect password!";
        conn->send(response.dump());
    }
}

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

// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>(); // 获取目的id
    {
        // 线程安全
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid); // 寻找toid是否存在对应用户
        if (it != _userConnMap.end())
        {
            // 说明存在对应用户 且其在线 服务器推送消息给对应用户
            it->second->send(js.dump());
            return;
        }
    }
    // toid不在线则存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        // 线程安全
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表中删除用户的链接信息
                user.setId(it->first);
                _userConnMap.erase(it); // 擦除
                break;
            }
        }
    }

    // 用户注销 相当于下线 需要取消订阅和setState

    // 更新用户的状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}
