#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

/*存储用户离线消息*/
#include <string>
#include <vector>
using namespace std;

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

#endif