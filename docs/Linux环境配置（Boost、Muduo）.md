# Linux环境配置（Boost、Muduo、Redis）

## PSCP传输文件到服务器

- 下载pscp.exe 下载地址`https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html`
- 将下载的pscp.exe拷贝到C:\Windows\System32（直接安装msi文件即可）
- 查看服务器的ip地址
- `ifconfig`
  - ![image-20230108153416939](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108153416939.png)
- 在windows命令行中进行文件传输指令
  - ![image-20230108153448135](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108153448135.png)
  - 输入y确认
  - 输入服务器密码
  - `pscp U:\boost源码\boost_1_69_0.tar.gz huan@192.168.2.31:/home/huan/`
  - 传输成功
  - ![image-20230108153516020](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108153516020.png)

## Boost

Linux上安装boost库的步骤大致和Windows系统上一样。先

把Linux系统下的boost源码包boost_1_69_0.tar.gz拷贝到某一指定路径下，然后解压，如下：

1. `tar -zxvf boost_1_69_0.tar.gz `

2. 进入源码目录，查看内容 `ls`

3. 运行booststrap.sh工程编译构建程序（等待一段时间）

   1. `cd boost_1_69_0/`
   2. `./bootstrap.sh `

4. 源码根目录下生成了b2程序，运行b2程序如下（boost源码比较大，这里编译需要花费一些时间）：

   1. `tony@tony-virtual-machine:~/package/boost_1_69_0$ ./b2`

5. 把上面的boost库头文件和lib库文件安装在默认的Linux系统头文件和库文件的搜索路径下

   1. `root@tony-virtual-machine:/home/tony/package/boost_1_69_0# ./b2 install`

6. 测试

   1. ```C++
      #include <iostream>
      #include <boost/bind.hpp>
      #include <string>
      using namespace std;
      
      class Hello
      {
      public:
      	void say(string name) 
      	{ cout << name << " say: hello world!" << endl; }
      };
      
      int main()
      {
      	Hello h;
      	auto func = boost::bind(&Hello::say, &h, "zhang san");
      	func();
      	return 0;
      }
      ```

   2. `zhang san say: hello world!`

## muduo

- 先拷贝文件

  - ![image-20230108153942124](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108153942124.png)
  - ![image-20230108154030858](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108154030858.png)

- 解压

  - `unzip muduo.....zip`
  - ![image-20230108154133481](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108154133481.png)

- 解压完成后，进入muduo库的解压目录里面

  - muduo库源码编译会编译很多unit_test测试用例代码，编译耗时长，我们也用不到，vim编辑上面源码目录里面的CMakeLists.txt文件
  - ![image-20230108154514953](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108154514953.png)
  - 保存并退出 wq

- 执行build.sh程序

  - ![image-20230108154740480](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108154740480.png)
  - ![image-20230108154719632](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108154719632.png)

- 安装

  - ![image-20230108154827610](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108154827610.png)

  - 这个./build.sh install实际上把muduo的头文件和lib库文件放到了muduo-master同级目录下的build目录下的release-install-cpp11文件夹下面了：

  - 所以上面的install命令并没有把它们拷贝到系统路径下，导致我们每次编译程序都需要指定muduo库的头文件和库文件路径，很麻烦，所以我们选择直接把inlcude（头文件）和lib（库文件）目录下的文件拷贝到系统目录下：

  - ```
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11# ls
    include  lib
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11# cd include/
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11/include# ls
    muduo
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11/include# mv muduo/ /usr/include/
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11/include# cd ..
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11# ls
    include  lib
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11# cd lib/
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11/lib# ls
    libmuduo_base.a  libmuduo_http.a  libmuduo_inspect.a  libmuduo_net.a
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11/lib# mv * /usr/local/lib/
    root@tony-virtual-machine:/home/tony/package/build/release-install-cpp11/lib# 
    
    ```

    拷贝完成以后使用muduo库编写C++网络程序，不用在指定头文件和lib库文件路径信息了，因为g++会自动从/usr/include和/usr/local/lib路径下寻找所需要的文件

- 测试

  - 把muduo库的头文件和lib库文件拷贝完成以后，使用muduo库编写一个简单的echo回显服务器，测试muduo库是否可以正常使用

  - ```C++
    #include <muduo/net/TcpServer.h>
    #include <muduo/base/Logging.h>
    #include <boost/bind.hpp>
    #include <muduo/net/EventLoop.h>
    
    // 使用muduo开发回显服务器
    class EchoServer
    {
     public:
      EchoServer(muduo::net::EventLoop* loop,
                 const muduo::net::InetAddress& listenAddr);
    
      void start(); 
    
     private:
      void onConnection(const muduo::net::TcpConnectionPtr& conn);
    
      void onMessage(const muduo::net::TcpConnectionPtr& conn,
                     muduo::net::Buffer* buf,
                     muduo::Timestamp time);
    
      muduo::net::TcpServer server_;
    };
    
    EchoServer::EchoServer(muduo::net::EventLoop* loop,
                           const muduo::net::InetAddress& listenAddr)
      : server_(loop, listenAddr, "EchoServer")
    {
      server_.setConnectionCallback(
          boost::bind(&EchoServer::onConnection, this, _1));
      server_.setMessageCallback(
          boost::bind(&EchoServer::onMessage, this, _1, _2, _3));
    }
    
    void EchoServer::start()
    {
      server_.start();
    }
    
    void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
    {
      LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
               << conn->localAddress().toIpPort() << " is "
               << (conn->connected() ? "UP" : "DOWN");
    }
    
    void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                               muduo::net::Buffer* buf,
                               muduo::Timestamp time)
    {
      // 接收到所有的消息，然后回显
      muduo::string msg(buf->retrieveAllAsString());
      LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
               << "data received at " << time.toString();
      conn->send(msg);
    }
    
    
    int main()
    {
      LOG_INFO << "pid = " << getpid();
      muduo::net::EventLoop loop;
      muduo::net::InetAddress listenAddr(8888);
      EchoServer server(&loop, listenAddr);
      server.start();
      loop.loop();
    }
    
    ```

  使用g++进行编译，注意链接muduo和pthread的库文件，编译命令如下：

  `g++ main.cpp -lmuduo_net -lmuduo_base -lpthread -std=c++11`

  等待客户端连接，可以打开一个新的shell命令行用netcat命令模拟客户端连接echo服务器进行功能测试，成功：

  ![image-20230108160041292](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230108160041292.png)


## Redis

- 由于 redis 是用 C 语言开发，安装之前必先确认是否安装 gcc 环境（gcc -v）

- 下载并解压安装包

  - ```shell
    [root@localhost local]# wget http://download.redis.io/releases/redis-6.2.5.tar.gz
     
    [root@localhost local]# tar -zxvf redis-6.2.5.tar.gz
    ```

- 安装并指定安装目录

  - `[root@localhost redis-6.2.5]# make install PREFIX=/usr/local/redis`

- 启动服务

  - 前台启动

    - ```
      [root@localhost redis-6.2.5]# cd /usr/local/redis/bin/
       
      [root@localhost bin]# ./redis-server
      ```

  - 后台启动

    - 从 redis 的源码目录中复制 redis.conf 到 redis 的安装目录
    - [root@localhost bin]# cp /usr/local/redis-6.2.5/redis.conf /usr/local/redis/bin/
    - 修改 redis.conf 文件，把 daemonize no 改为 daemonize yes
    - [root@localhost bin]# vi redis.conf
    - ![image-20230109115946298](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230109115946298.png)
    - [root@localhost bin]# ./redis-server redis.conf
    - ![image-20230109115958018](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230109115958018.png)

- 设置开机和全局启动服务

  - 复制粘贴以下内容

  - ```
    [Unit]
    Description=redis-server
    After=network.target
     
    [Service]
    Type=forking
    ExecStart=/usr/local/redis/bin/redis-server /usr/local/redis/bin/redis.conf
    PrivateTmp=true
     
    [Install]
    WantedBy=multi-user.target
    ```

- 设置开机启动

  - ```
    [root@localhost bin]# systemctl daemon-reload
     
    [root@localhost bin]# systemctl start redis.service
     
    [root@localhost bin]# systemctl enable redis.service
    ```

- 创建 redis 命令软链接

  - [root@localhost ~]# ln -s /usr/local/redis/bin/redis-cli /usr/bin/redis

- 测试

  - ![image-20230109120056016](https://happygoing.oss-cn-beijing.aliyuncs.com/img/image-20230109120056016.png)

- 服务操作命令

  systemctl start redis.service   #启动redis服务

  systemctl stop redis.service   #停止redis服务

  systemctl restart redis.service   #重新启动服务

  systemctl status redis.service   #查看服务当前状态

  systemctl enable redis.service   #设置开机自启动

  systemctl disable redis.service   #停止开机自启动

## MySQL

> https://blog.csdn.net/m0_66557301/article/details/123730313?ops_request_misc=&request_id=&biz_id=102&utm_term=centos7%E5%AE%89%E8%A3%85mysql8.0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-0-123730313.142^v70^pc_new_rank,201^v4^add_ask&spm=1018.2226.3001.4187