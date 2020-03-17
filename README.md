# networkExample
### 源文件说明

1. `simpleServer.cpp`为阻塞的方式
2. `selectServer.cpp`为用`select`的方式
3. `epollServer.cpp`为用`epoll`的方式
4. `libeventServer.cpp`为用`libevent`的方式
5. `libevent`文件夹为更多的`libevent`示例。

### libevent的缺点

1.  libevent在windows下支持iocp，但还不完善，（性能不高），但是可以使用select。[出处](https://blog.csdn.net/tjm1017/article/details/88219576?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task)
2. 性能不如其他网路库如muduo。[出处](https://blog.csdn.net/Solstice/article/details/5864889)

### libevent的优点

1. 更成熟、稳定，有较成熟的产品在用。如`chromium`，`memcached`。[出处1](https://www.zhihu.com/question/20278441/answer/20226308)、[出处2](http://libevent.org/)

### 设置libevent使用某种方法

[event_config_avoid_method()](https://github.com/guangxuanliu/networkExample/blob/6bdfd5ea5ec3e8e2aac89b329859a4ef887430d2/echoServer/server/libeventTest.cpp#L29)：通过设置避免某种方法来使用其他的方法。

### 注意：

1. 修改Linux下的最大打开文件个数

   ```
   vi /etc/security/limits.conf 
   *       soft    nofile  65535
   *       hard    nofile  65535
   
   
   vi /etc/pam.d/login
   session    required     pam_limits.so
   
   注销系统，重新登陆即可
   ```

2. 测试连接数的工具[TCP_UDP_PerformanceTest](https://www.cnblogs.com/smark/p/4496660.html)

