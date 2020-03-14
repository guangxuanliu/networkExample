# networkExample
1. server.cpp为阻塞的方式
2. selectServer.cpp为用select的方式
3. libeventServer.cpp为用libevent的方式



#### 注意：

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

