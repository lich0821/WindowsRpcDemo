#include <locale.h>
#include <stdio.h>

#include "demo_h.h"
#pragma comment(lib, "Rpcrt4.lib")

BOOL WINAPI ctrlCHandler(DWORD /*signal*/)
{
    unsigned long ulCode = 0;
    RpcTryExcept { client_Shutdown(); }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("ctrlCHandler: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;

    exit(0);
}

int main()
{
    int ret = -1;
    RPC_STATUS status;
    RPC_WSTR pszStringBinding = NULL;
    unsigned long ulCode      = 0;

    setlocale(LC_ALL, "chs");                  // 这是个大坑，不设置中文直接不见了。。。
    SetConsoleCtrlHandler(ctrlCHandler, TRUE); // 注册 Ctrl+C 处理函数
    // Creates a string binding handle.
    // This function is nothing more than a printf.
    // Connection is not done here.
    status = RpcStringBindingCompose(NULL,                                             // UUID to bind to
                                     reinterpret_cast<RPC_WSTR>((RPC_WSTR)L"ncalrpc"), // Use TCP/IP protocol
                                     NULL,                                             // TCP/IP network address to use
                                     reinterpret_cast<RPC_WSTR>((RPC_WSTR)L"tmp_endpoint"), // TCP/IP port to use
                                     NULL,             // Protocol dependent network options to use
                                     &pszStringBinding // String binding output
    );

    if (status)
        exit(status);

    // Validates the format of the string binding handle and converts
    // it to a binding handle.
    // Connection is not done here either.
    status
        = RpcBindingFromStringBinding(pszStringBinding, // The string binding to validate
                                      &hDemoBinding); // Put the result in the implicit binding(defined in the IDL file)

    if (status)
        exit(status);

    RpcTryExcept
    {
        ret = client_SendString(L"Hello from 客户端");
        if (ret == 0) {
            wprintf(L"消息发送成功！\n");
        } else {
            wprintf(L"消息发送失败！\n");
        }

        // 发送完关闭通道
        client_Shutdown();
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("main: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;

    // Free the memory allocated by a string
    status = RpcStringFree(&pszStringBinding);
    if (status)
        exit(status);

    // Releases binding handle resources and disconnects from the server
    status = RpcBindingFree(&hDemoBinding);
    if (status)
        exit(status);

    exit(0);
}