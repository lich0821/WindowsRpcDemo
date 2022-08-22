#include "util.h"

void SetRpcContact(Contact_t contact, RpcContact_t *RpcContact)
{
    RpcContact->age     = contact.age;
    RpcContact->name    = SysAllocStringLen(contact.name.data(), contact.name.size());
    RpcContact->mobile  = SysAllocStringLen(contact.mobile.data(), contact.mobile.size());
    RpcContact->address = SysAllocStringLen(contact.address.data(), contact.address.size());
}

void GetRpcContact(Contact_t *contact, RpcContact_t RpcContact)
{
    contact->age     = RpcContact.age;
    contact->name    = std::wstring(RpcContact.name);
    contact->mobile  = std::wstring(RpcContact.mobile);
    contact->address = std::wstring(RpcContact.address);

    SysFreeString(RpcContact.name);
    SysFreeString(RpcContact.mobile);
    SysFreeString(RpcContact.address);
}
