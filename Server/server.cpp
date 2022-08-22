#include <locale.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "demo_h.h"
#include "util.h"

#pragma comment(lib, "Rpcrt4.lib")
#pragma warning(disable : 4996)

using namespace std;

HANDLE g_hEvent    = NULL;
wchar_t g_msg[128] = { 0 };
DWORD WINAPI ThreadMakeMsg(LPVOID argv);

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

int server_GetContact(PRPCCONTACT pRpcContact)
{
    wprintf(L"服务器收到pRpcContact：%p\n", pRpcContact);
    pRpcContact->age     = 100;
    pRpcContact->name    = SysAllocString(L"Name from Server");
    pRpcContact->mobile  = SysAllocString(L"123456789");
    pRpcContact->address = SysAllocString(L"Beijing China");

    return 0;
}

int server_GetContactList(int *pNum, PPRPCCONTACT *rpcContact)
{
    vector<Contact_t> vContact;
    *pNum = 10;
    for (int i = 0; i < *pNum; i++) {
        Contact_t tmp = { i, L"name" + to_wstring(i), L"mobile" + to_wstring(i), L"address" + to_wstring(i) };
        vContact.push_back(tmp);
    }

    PPRPCCONTACT pp = (PPRPCCONTACT)midl_user_allocate(*pNum * sizeof(RpcContact_t));
    if (pp == NULL) {
        wprintf(L"内存分配失败！\n");
        return -1;
    }
    int index = 0;
    for (auto it = vContact.begin(); it != vContact.end(); it++) {
        PRPCCONTACT p = (PRPCCONTACT)midl_user_allocate(sizeof(RpcContact_t));
        if (p == NULL) {
            wprintf(L"内存分配失败！\n");
            return -2;
        }
        SetRpcContact(*it, p);
        pp[index++] = p;
    }

    *rpcContact = pp;

    return 0;
}

void server_EnableReceiveMsg()
{
    unsigned long ulCode = 0;
    wprintf(L"消息接收已打开\n");
    RpcTryExcept
    {
        // 调用客户端的回调函数
        while (true) {
            WaitForSingleObject(g_hEvent, INFINITE);     // 等待消息
            client_ReceiveMsgCb((const wchar_t *)g_msg); // 调用接收消息回调
            ResetEvent(g_hEvent);                        // 重置消息状态
        }
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
    }
    RpcEndExcept
}

DWORD WINAPI ThreadMakeMsg(LPVOID argv)
{
    DWORD count = 0;
    while (TRUE) {
        count = 100 + rand() % 2000;
        wsprintf(g_msg, L"Message from Server(%ld)", count); // 生成消息

        SetEvent(g_hEvent); // 发送消息通知
        Sleep(count);       // 模拟消息生成间隔
    }

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

    g_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);     // 创建消息句柄
    CreateThread(NULL, 0, ThreadMakeMsg, NULL, 0, NULL); // 新建线程，模拟消息生成
    while (1) {
        Sleep(10000); // 休眠，释放CPU
    }

    return 0;
}
