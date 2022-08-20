#include <locale.h>
#include <stdio.h>

#include "demo_h.h"
#pragma comment(lib, "Rpcrt4.lib")

void server_Shutdown(void)
{
    RPC_STATUS status = RPC_S_OK;

    status = RpcMgmtStopServerListening(NULL);
    if (status) {
        exit(status);
    }

    status = RpcServerUnregisterIf(NULL, NULL, FALSE);
    if (status) {
        exit(status);
    }
}

int server_SendString(const wchar_t *msg)
{
    wprintf(L"服务器收到消息：%s\n", msg);

    return 0;
}

// Naive security callback.
RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void * /*pBindingHandle*/)
{
    return RPC_S_OK; // Always allow anyone.
}

int main()
{
    RPC_STATUS status;
    unsigned long ulCode = 0;
    setlocale(LC_ALL, "chs"); // 这是个大坑，不设置中文直接不见了。。。
    // Uses the protocol combined with the endpoint for receiving
    // remote procedure calls.
    status = RpcServerUseProtseqEp(reinterpret_cast<RPC_WSTR>((RPC_WSTR)RPC_PROTSEQ), // Use TCP/IP protocol
                                   RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Backlog queue length for TCP/IP.
                                   reinterpret_cast<RPC_WSTR>((RPC_WSTR)RPC_ENDPOINT), // TCP/IP port to use
                                   NULL                                                // No security
    );

    if (status)
        exit(status);

    // Registers the hello interface and auto listen
    // Equal to RpcServerRegisterIf + RpcServerListen
    status = RpcServerRegisterIf2(server_demo_v1_0_s_ifspec, // Interface to register.
                                  NULL,                      // Use the MIDL generated entry-point vector.
                                  NULL,                      // Use the MIDL generated entry-point vector.
                                  RPC_IF_ALLOW_LOCAL_ONLY | RPC_IF_AUTOLISTEN, // Forces use of security callback.
                                  RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Use default number of concurrent calls.
                                  (unsigned)-1,                   // Infinite max size of incoming data blocks.
                                  SecurityCallback);              // Naive security callback.

    while (1) {
        Sleep(10000); // 休眠，释放CPU
    }

    return 0;
}
