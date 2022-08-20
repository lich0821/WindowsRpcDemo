# Yet Another Demo for Windows RPC
开发环境：
* VS2019 社区版（16.11.4）
* Windows 10 企业版 LTSC（1809）【PVE 虚拟机】

## 从零创建工程
1. 创建一个解决方案：`RpcDemo`，并保存到 `RpcDemo` 目录
2. 创建两个项目：`Client` 和 `Server`，分别保存到 `RpcDemo/Client` 和 `RpcDemo/Server`
3. 创建目录 `RpcDemo/Rpc`
4. 在 `Server` 项目下添加筛选器 `Rpc`，并在 `Rpc` 下添加 `demo.idl`
5. 修改 IDL 文件 `属性`（文件右键）：
    `输出目录`：`$(SolutionDir)Rpc`
    `命令行`：`/ms_ext /prefix client "client_" server "server_" /app_config`
6. 编译 IDL 文件（右键）
7. 给 `server` 添加文件：
    * `Rpc` 下添加：
        * `rpc_memory.cpp`
        * `demo_s.c`
    * `源文件` 下添加：
        * `server.c`
    * `头文件` 下添加：
        * `demo_h.h`
8. 给 `client` 添加文件：
    * `Rpc`（参考 4 新增筛选器）下添加：
        * `rpc_memory.cpp`
        * `demo_c.c`
    * `源文件` 下添加：
        * `client.c`
    * `头文件` 下添加：
        * `demo_h.h`
9. 添加 `附加包含目录`
    * `server`，`属性`，`C/C++`，`常规`，`附加包含目录`：`$(SolutionDir)Rpc`
    * `client`，`属性`，`C/C++`，`常规`，`附加包含目录`：`$(SolutionDir)Rpc`
10. 配置依赖关系
`解决方案` --> `属性` --> `通用属性` --> `项目依赖项`：`Client` 依赖于 `Server`
