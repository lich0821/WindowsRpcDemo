# Yet Another Demo for Windows RPC
开发环境：
* VS2019 社区版（16.11.4）
* Windows 10 企业版 LTSC（1809）【PVE 虚拟机】

## 快速开始
### 克隆项目
```sh
git clone https://gitee.com/lch0821/RpcDemo.git
```

### 编译项目
双击 `RpcDemo.sln` 打开工程，右键，选择 `生成解决方案`（快捷键：`Ctrl+Shift+B`）。

### 运行项目
进入项目目录下的 `Debug`，先打开 `Server.exe`，再打开 `Client.exe`，即可看到消息发送情况。

也可以分别在两个命令窗口里执行 `Server.exe` 和 `Client.exe`。

## 版本说明
### V1
V1 实现了由 Client 向 Server 发送字符串，IDL 文件如下：
```C
[
    uuid(50C2504F-1062-4CC3-B9F1-9329A3BE82F9),
    version(1.0),
    implicit_handle(handle_t hDemoBinding)
]
interface demo
{
    import "demo.h";

    int SendString([in, string] const wchar_t* msg);
    void Shutdown(void);
}
```

### V2
V2 实现了由 Client 向 Server 发送字符串并获取字符串，IDL 文件新增：
```C
    int GetString([in, string] const wchar_t* inStr, [out, string] wchar_t outStr[128]);
```

### V3
V3 实现了由 Client 向 Server 发送字符串并获取字符串，IDL 文件新增：
```C
    typedef struct RPCSTRING {
        unsigned long size;
        [ ptr, size_is(size), length_is(size) ] wchar_t str[*];
    } RPCSTRING_t;
    typedef RPCSTRING_t* PRPCSTRING;

    int GetVarString([ in, string ] const wchar_t *inStr, [ out ] PRPCSTRING *outStr);
```

### V4
V4 实现了由 Client 向 Server 发送字符串并获取字符串，IDL 文件新增：
```C
    typedef RPCSTRING_t** PPRPCSTRING;

    int GetVarStringList([ in, string ] const wchar_t *inStr, [out] int *pNum,
                         [ out, size_is(, *pNum) ] PPRPCSTRING *outStrList);
```

### V5
V5 实现了由 Client 向 Server 获取 `contact` 结构体（整型和字符串），IDL 文件新增：
```C
    typedef struct Contact {
        int age;
        BSTR name;
        BSTR mobile;
        BSTR address;
    }Contact_t;
    typedef Contact_t* PContact_t;

    int GetContact([out] PContact_t contact);
```

### V6
V6 实现了由 Client 向 Server 获取 `contact` 结构体（整型和字符串），IDL 文件新增：
```C
    typedef Contact_t** PPContact_t;

    int GetContactList([out] int* pNum, [out, size_is(, *pNum)] PPContact_t *contact);
```

### V7
V7 添加了 Rpc 结构体（BSTR）和 vector 结构体（wstring）的转换方法，IDL 文件修改：
```C
    typedef struct RpcContact {
        int age;
        BSTR name;
        BSTR mobile;
        BSTR address;
    }RpcContact_t;
    typedef RpcContact_t* PRPCCONTACT;
    typedef RpcContact_t** PPRPCCONTACT;

    int GetContact([out] PRPCCONTACT contact);
    int GetContactList([out] int* pNum, [out, size_is(, *pNum)] PPRPCCONTACT *contact);
```

## 从零创建工程
1. 创建一个解决方案：`RpcDemo`，并保存到 `RpcDemo` 目录
2. 创建两个项目：`Client` 和 `Server`，分别保存到 `RpcDemo/Client` 和 `RpcDemo/Server`
3. 创建目录 `RpcDemo/Rpc`
4. 在 `Server` 项目下添加筛选器 `Rpc`，并在 `Rpc` 下添加 `demo.idl`
5. 修改 IDL 文件 `属性`（文件右键）：
    * `输出目录`：`$(SolutionDir)Rpc`
    * `附加包含目录`：`$(SolutionDir)Rpc`
    * `命令行`：`/ms_ext /prefix client "client_" server "server_" /app_config`
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
