#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"

// 群组用户 多了一个role作为角色权限 从User类直接继承 复用User
class GroupUser : public User
{
public:
    void setRole(string role) { this->role = role; }
    string getRole() { return this->role; }

private:
    string role;
};

#endif