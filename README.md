# MxcServer

## 介绍

MxcServer是使用 C++14 编写的一个在 GNU/Linux 平台上运行的的高性能 Web 服务器，能够高效地处理HTTP/1.1请求，包括静态GET、HEAD、OPTIONS 请求和 FastCGI 支持。

## 主要特点

1. 使用半同步半异步的**reactor**模型
2. 使用epoll边缘触发的IO多路复用技术
3. 使用**无锁事件分发模型**
4. 使用非阻塞connect实现简单客户端,支持**用户态重连**
5. 支持长连接，采用**Timerfd**支持以事件驱动的多轮盘**Timewheel**，以实现应用层TCP保活机制
6. 基于**双缓冲区技术**与细粒度锁实现的**线程安全hashmap**实现了简单的异步日志系统
7. 引入**绑核机制**最大化CPU缓存命中率
8. 大量应用constexper，noexcept，__attribute__等机制以增加代码的优化潜能
9. 引入**套接字slab层**，最小化内存分配，提高性能
10. 引入全局**无锁队列**，实现基于吞吐量和每线程长连接数量的**特殊加权轮询负载均衡**算法 

## 安装与运行 

软件安装命令以**Arch Linux**为例，其他操作系统自行查询

1. 安装构建工具：

    ```bash
    sudo pacman -S cmake gcc
    ```

2. 安装 boost 库：

    ```bash
    sudo pacman -S boost
    ```

3. 安装 gperftools 性能优化工具：

    ```bash
    sudo pacman -S gperftools
    ```

4. 运行：

    ```
    mkdir build
    cd build
    cmake ..
    make
    ./Web_Server
    ```

## 性能测试

### 测试环境：

| 环境名称  |                              值                              |
| :-------: | :----------------------------------------------------------: |
|   系统    |                    6.7.8-arch1-1 (64 位)                     |
|  处理器   |         16 × AMD Ryzen 7 5800H with Radeon Graphics          |
| 逻辑核数  |                              16                              |
| 系统负载  |                       0.64, 0.69, 0.76                       |
| 内存占用  | 6.5Gi(used) / 744M(fraee) / 6.7Gi(buffcache) / 7.0Gi(available) |
|   Swap    |                  904Ki(used) / 15Gi(freee)                   |
| Cmake版本 |                            3.28.3                            |
|  Gcc版本  |                            13.2.1                            |

### 测试步骤：

0. 安装工具：gperftool（前面已装）、htop、ApacheBench2.3

     ```bash
     sudo pacman -S gperftools htop http
     ```

1. 在 build 目录下执行`./Web_Server`开启服务器，记下服务器 pid ：$PID。

2. 打开另一个终端，执行`htop -p $PID`，可以实时查看 CPU 占用情况。

3. 再打开一个终端，执行 `kill -s SIGUSR1 $PID`，开始性能分析。

4. 使用 ApacheBench2.3 的 ab 命令对服务器进行执行压力测试：

    ```bash
    ab -n 1000000 -c 1000  127.0.0.1:8888/ # -n：指定总请求数，-c：指定并发请求数
    ```

5. 执行 `kill -s SIGUSR1 $PID`，结束性能分析，此时已经在 build 目录下自动生成 PerformanceAnalysis.prof 分析文件。

6. 执行以下命令根据 prof 文件生成 pdf 和 txt 文件：

    ```bash
    pprof ./Web_Server PerformanceAnalysis.prof  --text > PerformanceAnalysis.txt
    pprof ./Web_Server PerformanceAnalysis.prof  --pdf > PerformanceAnalysis.pdf 
    ```

### 测试结果

[测试前 CPU 占用]()，[测试中 CPU 占用]()

[性能分析结果]()

[压力测试结果]()

### 性能对比

对比对象：Apache/2.4.58，nginx/1.24.0



### 性能分析





