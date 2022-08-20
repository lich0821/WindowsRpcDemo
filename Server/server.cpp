#include <locale.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "demo_h.h"
#pragma comment(lib, "Rpcrt4.lib")
#pragma warning(disable : 4996)

using namespace std;

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

int server_GetString(const wchar_t *inStr, wchar_t *outStr)
{
    wprintf(L"服务器收到消息：%s\n", inStr);
    wsprintf(outStr, L"这是服务器返回的消息！");
    return 0;
}

int server_GetVarString(const wchar_t *inStr, PRPCSTRING *outStr)
{
    wstring rsp = wstring(inStr) + L"+我是你不知道长度的字符串";
    wprintf(L"服务器收到消息：%s\n", inStr);

    size_t len   = rsp.length() * sizeof(wchar_t) + sizeof(RPCSTRING_t);
    PRPCSTRING p = (PRPCSTRING)midl_user_allocate(len);
    if (p == NULL) {
        wprintf(L"内存分配失败！\n");
        return -1;
    }

    p->size = rsp.size();
    wcsncpy(p->str, rsp.c_str(), rsp.size());
    *outStr = p;

    return 0;
}

int server_GetVarStringList(const wchar_t *inStr, int *pNum, PPRPCSTRING *outStrList)
{
    wprintf(L"服务器收到消息：%s\n", inStr);
    *pNum = 10;
    vector<wstring> strVector;
    for (int i = 0; i < *pNum; i++) {
        strVector.push_back(wstring(inStr) + L"+我是你不知道长度的字符串+" + to_wstring(i));
    }
    PPRPCSTRING pp = (PPRPCSTRING)midl_user_allocate(*pNum * sizeof(RPCSTRING_t));
    if (pp == NULL) {
        wprintf(L"内存分配失败！\n");
        return -1;
    }
    int index = 0;
    for (auto it = strVector.begin(); it != strVector.end(); it++) {
        size_t len   = (*it).length() * sizeof(wchar_t) + sizeof(RPCSTRING_t);
        PRPCSTRING p = (PRPCSTRING)midl_user_allocate(len);
        if (p == NULL) {
            wprintf(L"内存分配失败！\n");
            return -2;
        }

        p->size = (*it).size();
        wcsncpy(p->str, (*it).c_str(), (*it).size());
        pp[index++] = p;
    }

    *outStrList = pp;

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

    // Registers the demo interface and auto listen
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
