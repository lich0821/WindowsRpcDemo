#pragma once

#include <string>

#include "demo_h.h"

typedef struct Contact {
    int age;
    std::wstring name;
    std::wstring mobile;
    std::wstring address;
} Contact_t;

void SetRpcContact(Contact_t contact, RpcContact_t *RpcContact);
void GetRpcContact(Contact_t *contact, RpcContact_t RpcContact);
