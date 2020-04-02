### 加解密算法

#### 对称加密

密钥管理：比较难，不适合互联网，一般用于内部系统

安全性：中

加密速度：比较快，适合大数据量的加解密处理



#### 非对称加密

密钥管理：比较容易

安全性：高

加密速度：比较慢，适合小数据量加解密或数据签名



#### 对称加密算法比较

| **名称** | 密钥名称        | 运行速度 | 安全性 | 资源消耗 |
| -------- | --------------- | -------- | ------ | -------- |
| DES      | 56位            | 较快     | 低     | 中       |
| 3DES     | 112位或168位    | 慢       | 中     | 高       |
| AES      | 128、192、256位 | 快       | 高     | 低       |



#### C++ 加密库

1. [Crypto++](http://www.cryptopp.com/)
2. [OpenSSL](http://www.openssl.org/)
3. [Botan](http://botan.randombit.net/)
4. [Cryptlib](http://www.cryptlib.com/)



#### 常用开源工具加解密方式

1. [ShadowSocks推荐方式](https://github.com/softwaredownload/openwrt-fanqiang/blob/master/ebook/03.8.md)

> [目前推荐使用 AEAD 加密方式](https://shadowsocks.org/en/spec/AEAD-Ciphers.html)
>
> - xchacha20-ietf-poly1305
> - chacha20-ietf-poly1305
> - aes-256-gcm
> - aes-192-gcm
> - aes-128-gcm



#### AES 加密模式

1. ECB和CBC模式下，需要处理的数据必须是`block size`的整数倍。可以用`StreamTransformationFilter`来进行指定填充。

   其他模式不需要是整数倍。

   ```
   StringSource(data, true, new StreamTransformationFilter(encryptor, new StringSink(result), NO_PADDING))
   ```

2. 推荐使用CTR模式。![img](https://wumansgy.github.io/2018/11/03/AES%E7%9A%84CTR%E6%A8%A1%E5%BC%8F%E5%8A%A0%E5%AF%86%E8%A7%A3%E5%AF%86%E8%AF%A6%E8%A7%A3/8.png)

3. 推荐使用CTR或者GCM。



#### AES性能测试

**以下为CTR模式下测试结果（release模式）：**

| 大小（KB） | 加密用时（微秒） | 解密用时（微秒） |
| ---------- | ---------------- | ---------------- |
| 1          | 13.77            | 12.3             |
| 2          | 21.33            | 17.2             |
| 4          | 8.34             | 8.25             |
| 8          | 29.05            | 28.12            |
| 16         | 46.87            | 35.91            |
| 32         | 73.86            | 61.57            |
| 64         | 149.66           | 117.04           |
| 128        | 337.92           | 266.74           |
| 256        | 537.28           | 422.71           |
| 512        | 1075.14          | 1046.48          |

**结论：**对`512KB`左右的数据，在release模式下大约需要1毫秒