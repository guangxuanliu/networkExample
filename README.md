# networkExample
### 源文件说明

1. `simpleServer.cpp`为阻塞的方式
2. `selectServer.cpp`为用`select`的方式
3. `epollServer.cpp`为用`epoll`的方式
4. `libeventServer.cpp`为用`libevent`的方式

### libevent的缺点

1.  libevent在windows下支持iocp，但还不完善，（性能不高），但是可以使用select。[出处](https://blog.csdn.net/tjm1017/article/details/88219576?depth_1-utm_source=distribute.pc_relevant.none-task&utm_source=distribute.pc_relevant.none-task)
2. 性能不如其他网路库如muduo。[出处](https://blog.csdn.net/Solstice/article/details/5864889)

### libevent的优点

1. 更成熟、稳定，有较成熟的产品在用。如`chromium`，`memcached`。[出处1](https://www.zhihu.com/question/20278441/answer/20226308)、[出处2](http://libevent.org/)

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

