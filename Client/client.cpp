#include <locale.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "demo_h.h"
#include "util.h"

#pragma comment(lib, "Rpcrt4.lib")

using namespace std;

int SendString(const wchar_t *msg)
{
    int ret              = -1;
    unsigned long ulCode = 0;
    RpcTryExcept { ret = client_SendString(msg); }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("SendString: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;
    if (ret != 0) {
        wprintf(L"消息发送失败: %d\n", ret);
    }
    return ret;
}

int GetString(const wchar_t *inStr, wchar_t *outStr)
{
    int ret              = -1;
    unsigned long ulCode = 0;
    RpcTryExcept { ret = client_GetString(inStr, outStr); }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("GetString: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;
    if (ret != 0) {
        wprintf(L"获取消息失败: %d\n", ret);
    }
    return ret;
}

PRPCSTRING innerGetVarString()
{
    int ret              = -1;
    PRPCSTRING outStr    = NULL;
    unsigned long ulCode = 0;
    RpcTryExcept { ret = client_GetVarString(L"GetVarString from 客户端", &outStr); }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("GetVarString: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;
    if (ret != 0) {
        wprintf(L"GetVarString失败: %d\n", ret);
        return NULL;
    }

    return outStr;
}

int GetVarString()
{
    PRPCSTRING p    = innerGetVarString();
    p->str[p->size] = L'\0';
    wstring str     = wstring(p->str);
    midl_user_free(p);
    wprintf(L"获取结果：[%d-%d-%d]%s\n", str.size(), str.length(), str.capacity(), str.c_str());

    return 0;
}

PPRPCSTRING innerGetVarStringList(int *size)
{
    int ret                = -1;
    unsigned long ulCode   = 0;
    PPRPCSTRING outStrList = NULL;
    RpcTryExcept { ret = client_GetVarStringList(L"GetVarStringList from 客户端", size, &outStrList); }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("GetVarStringList: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;
    if (ret != 0) {
        wprintf(L"GetVarStringList失败: %d\n", ret);
        return NULL;
    }

    return outStrList;
}

int GetVarStringList()
{
    int size       = 0;
    PPRPCSTRING pp = innerGetVarStringList(&size);
    vector<wstring> strVector;
    for (int i = 0; i < size; i++) {
        PRPCSTRING p    = (PRPCSTRING)pp[i];
        p->str[p->size] = L'\0';
        wstring str     = wstring(p->str);
        midl_user_free(p);
        strVector.push_back(str);
    }
    midl_user_free(pp);

    wprintf(L"收到字符串列表[%d]：\n", size);
    for (auto it = strVector.begin(); it != strVector.end(); it++) {
        wprintf(L"[%d-%d-%d]%s\n", (*it).size(), (*it).length(), (*it).capacity(), (*it).c_str());
    }

    return 0;
}

int innerGetContact(RpcContact_t *contact)
{
    int ret              = -1;
    unsigned long ulCode = 0;
    RpcTryExcept { ret = client_GetContact(contact); }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("GetContact: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;
    if (ret != 0) {
        wprintf(L"GetContact失败: %d\n", ret);
        return NULL;
    }
    return ret;
}

int GetContact()
{
    RpcContact_t rpcContact = { 0 };
    innerGetContact(&rpcContact);

    Contact_t contact = { 0 };
    GetRpcContact(&contact, rpcContact);
    wprintf(L"rpcContact: %p, age: %d, name: %s, mobile: %s, address: %s\n", &rpcContact, contact.age,
            contact.name.c_str(), contact.mobile.c_str(), contact.address.c_str());

    return 0;
}

PPRPCCONTACT innerGetContactList(int *size)
{
    int ret                   = -1;
    unsigned long ulCode      = 0;
    PPRPCCONTACT ppRpcContact = NULL;
    RpcTryExcept { ret = client_GetContactList(size, &ppRpcContact); }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("GetContactList: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;
    if (ret != 0) {
        wprintf(L"GetContactList失败: %d\n", ret);
        return NULL;
    }

    return ppRpcContact;
}

int GetContactList()
{
    int size        = 0;
    PPRPCCONTACT pp = innerGetContactList(&size);
    vector<Contact_t> vContact;
    for (int i = 0; i < size; i++) {
        Contact_t contact;
        GetRpcContact(&contact, *pp[i]);
        vContact.push_back(contact);
        midl_user_free(pp[i]);
    }
    midl_user_free(pp);

    wprintf(L"收到contact列表[%d]：\n", size);
    for (auto it = vContact.begin(); it != vContact.end(); it++) {
        wprintf(L"%d, %s, %s, %s\n", (*it).age, (*it).name.c_str(), (*it).mobile.c_str(), (*it).address.c_str());
    }

    return 0;
}

int server_ReceiveMsgCb(RpcContact_t rpcContact)
{
    wprintf(L"客户端收到消息: \n");
    Contact_t contact = { 0 };
    GetRpcContact(&contact, rpcContact);
    wprintf(L"rpcContact: %p, age: %d, name: %s, mobile: %s, address: %s\n", &rpcContact, contact.age,
            contact.name.c_str(), contact.mobile.c_str(), contact.address.c_str());

    return 0;
}

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

    return true;
}

int main()
{
    RPC_STATUS status;
    unsigned long ulCode        = 0;
    RPC_WSTR pszStringBinding   = NULL;
    wchar_t buffer[BUFFER_SIZE] = { 0 };

    setlocale(LC_ALL, "chs");                  // 这是个大坑，不设置中文直接不见了。。。
    SetConsoleCtrlHandler(ctrlCHandler, TRUE); // 注册 Ctrl+C 处理函数
    // Creates a string binding handle.
    // This function is nothing more than a printf.
    // Connection is not done here.
    status = RpcStringBindingCompose(NULL,                                              // UUID to bind to
                                     reinterpret_cast<RPC_WSTR>((RPC_WSTR)RPC_PROTSEQ), // Use TCP/IP protocol
                                     NULL,                                              // TCP/IP network address to use
                                     reinterpret_cast<RPC_WSTR>((RPC_WSTR)RPC_ENDPOINT), // TCP/IP port to use
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

    SendString(L"Hello from 客户端");
    GetString(L"GetString from 客户端", buffer);
    GetVarString();
    GetVarStringList();
    GetContact();
    GetContactList();

    RpcTryExcept
    {
        // 建立RPC通道，让服务端能够调用客户端的回调函数。（该接口会被服务端阻塞直到异常或者Ctrl+C退出）
        client_EnableReceiveMsg();
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("main: Runtime reported exception 0x%lX (%ld)\n", ulCode, ulCode);
    }
    RpcEndExcept;

    // Free the memory allocated by a string
    RpcStringFree(&pszStringBinding);

    // Releases binding handle resources and disconnects from the server
    RpcBindingFree(&hDemoBinding);
    return 0;
}
