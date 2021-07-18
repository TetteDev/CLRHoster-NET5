#include "pch.h"
#include <vector>
#include <Windows.h>
#include <algorithm>

#define UNLINK(x)					\
	(x).Flink->Blink = (x).Blink;	\
	(x).Blink->Flink = (x).Flink;

#define RELINK(x, real)			\
	(x).Flink->Blink = (real);	\
	(x).Blink->Flink = (real);	\
	(real)->Blink = (x).Blink;	\
	(real)->Flink = (x).Flink;

typedef struct _PEB_LDR_DATA {
    UINT8 _PADDING_[12];
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
}
PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
#ifdef _WIN64
    UINT8 _PADDING_[24];
#else
    UINT8 _PADDING_[12];
#endif
    PEB_LDR_DATA* Ldr;
}
PEB, * PPEB;

struct _UNICODE_STRING
{
    USHORT Length;                                                          //0x0
    USHORT MaximumLength;                                                   //0x2
    WCHAR* Buffer;                                                          //0x4
};

struct _RTL_BALANCED_NODE
{
    union
    {
        struct _RTL_BALANCED_NODE* Children[2];                             //0x0
        struct
        {
            struct _RTL_BALANCED_NODE* Left;                                //0x0
            struct _RTL_BALANCED_NODE* Right;                               //0x4
        };
    };
    union
    {
        struct
        {
            UCHAR Red : 1;                                                    //0x8
            UCHAR Balance : 2;                                                //0x8
        };
        ULONG ParentValue;                                                  //0x8
    };
};

typedef struct _LDR_DATA_TABLE_ENTRY {

#if defined (_WIN64) 
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20
    VOID* DllBase;                                                          //0x30
    VOID* EntryPoint;                                                       //0x38
    ULONG SizeOfImage;                                                      //0x40
    struct _UNICODE_STRING FullDllName;                                     //0x48
    struct _UNICODE_STRING BaseDllName;                                     //0x58
    union
    {
        UCHAR FlagGroup[4];                                                 //0x68
        ULONG Flags;                                                        //0x68
        struct
        {
            ULONG PackagedBinary : 1;                                         //0x68
            ULONG MarkedForRemoval : 1;                                       //0x68
            ULONG ImageDll : 1;                                               //0x68
            ULONG LoadNotificationsSent : 1;                                  //0x68
            ULONG TelemetryEntryProcessed : 1;                                //0x68
            ULONG ProcessStaticImport : 1;                                    //0x68
            ULONG InLegacyLists : 1;                                          //0x68
            ULONG InIndexes : 1;                                              //0x68
            ULONG ShimDll : 1;                                                //0x68
            ULONG InExceptionTable : 1;                                       //0x68
            ULONG ReservedFlags1 : 2;                                         //0x68
            ULONG LoadInProgress : 1;                                         //0x68
            ULONG LoadConfigProcessed : 1;                                    //0x68
            ULONG EntryProcessed : 1;                                         //0x68
            ULONG ProtectDelayLoad : 1;                                       //0x68
            ULONG ReservedFlags3 : 2;                                         //0x68
            ULONG DontCallForThreads : 1;                                     //0x68
            ULONG ProcessAttachCalled : 1;                                    //0x68
            ULONG ProcessAttachFailed : 1;                                    //0x68
            ULONG CorDeferredValidate : 1;                                    //0x68
            ULONG CorImage : 1;                                               //0x68
            ULONG DontRelocate : 1;                                           //0x68
            ULONG CorILOnly : 1;                                              //0x68
            ULONG ChpeImage : 1;                                              //0x68
            ULONG ReservedFlags5 : 2;                                         //0x68
            ULONG Redirected : 1;                                             //0x68
            ULONG ReservedFlags6 : 2;                                         //0x68
            ULONG CompatDatabaseProcessed : 1;                                //0x68
        };
    };
    USHORT ObsoleteLoadCount;                                               //0x6c
    USHORT TlsIndex;                                                        //0x6e
    struct _LIST_ENTRY HashLinks;                                           //0x70
    ULONG TimeDateStamp;                                                    //0x80
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88
    VOID* Lock;                                                             //0x90
    struct _LDR_DDAG_NODE* DdagNode;                                        //0x98
    struct _LIST_ENTRY NodeModuleLink;                                      //0xa0
    struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0xb0
    VOID* ParentDllBase;                                                    //0xb8
    VOID* SwitchBackContext;                                                //0xc0
    struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0xc8
    struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0xe0
    ULONGLONG OriginalBase;                                                 //0xf8
    union _LARGE_INTEGER LoadTime;                                          //0x100
    ULONG BaseNameHashValue;                                                //0x108
    enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x10c
    ULONG ImplicitPathOptions;                                              //0x110
    ULONG ReferenceCount;                                                   //0x114
    ULONG DependentLoadFlags;                                               //0x118
    UCHAR SigningLevel;                                                     //0x11c
#else
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x8
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x10
    VOID* DllBase;                                                          //0x18
    VOID* EntryPoint;                                                       //0x1c
    ULONG SizeOfImage;                                                      //0x20
    struct _UNICODE_STRING FullDllName;                                     //0x24
    struct _UNICODE_STRING BaseDllName;                                     //0x2c
    union
    {
        UCHAR FlagGroup[4];                                                 //0x34
        ULONG Flags;                                                        //0x34
        struct
        {
            ULONG PackagedBinary : 1;                                         //0x34
            ULONG MarkedForRemoval : 1;                                       //0x34
            ULONG ImageDll : 1;                                               //0x34
            ULONG LoadNotificationsSent : 1;                                  //0x34
            ULONG TelemetryEntryProcessed : 1;                                //0x34
            ULONG ProcessStaticImport : 1;                                    //0x34
            ULONG InLegacyLists : 1;                                          //0x34
            ULONG InIndexes : 1;                                              //0x34
            ULONG ShimDll : 1;                                                //0x34
            ULONG InExceptionTable : 1;                                       //0x34
            ULONG ReservedFlags1 : 2;                                         //0x34
            ULONG LoadInProgress : 1;                                         //0x34
            ULONG LoadConfigProcessed : 1;                                    //0x34
            ULONG EntryProcessed : 1;                                         //0x34
            ULONG ProtectDelayLoad : 1;                                       //0x34
            ULONG ReservedFlags3 : 2;                                         //0x34
            ULONG DontCallForThreads : 1;                                     //0x34
            ULONG ProcessAttachCalled : 1;                                    //0x34
            ULONG ProcessAttachFailed : 1;                                    //0x34
            ULONG CorDeferredValidate : 1;                                    //0x34
            ULONG CorImage : 1;                                               //0x34
            ULONG DontRelocate : 1;                                           //0x34
            ULONG CorILOnly : 1;                                              //0x34
            ULONG ChpeImage : 1;                                              //0x34
            ULONG ReservedFlags5 : 2;                                         //0x34
            ULONG Redirected : 1;                                             //0x34
            ULONG ReservedFlags6 : 2;                                         //0x34
            ULONG CompatDatabaseProcessed : 1;                                //0x34
        };
    };
    USHORT ObsoleteLoadCount;                                               //0x38
    USHORT TlsIndex;                                                        //0x3a
    struct _LIST_ENTRY HashLinks;                                           //0x3c
    ULONG TimeDateStamp;                                                    //0x44
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x48
    VOID* Lock;                                                             //0x4c
    struct _LDR_DDAG_NODE* DdagNode;                                        //0x50
    struct _LIST_ENTRY NodeModuleLink;                                      //0x54
    struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0x5c
    VOID* ParentDllBase;                                                    //0x60
    VOID* SwitchBackContext;                                                //0x64
    struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0x68
    struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0x74
    ULONG OriginalBase;                                                     //0x80
    union _LARGE_INTEGER LoadTime;                                          //0x88
    ULONG BaseNameHashValue;                                                //0x90
    enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x94
    ULONG ImplicitPathOptions;                                              //0x98
    ULONG ReferenceCount;                                                   //0x9c
    ULONG DependentLoadFlags;                                               //0xa0
    UCHAR SigningLevel;
#endif
}
LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

namespace Tools {
    static void _stdcall PatchETW()
    {
        HMODULE ntdll = GetModuleHandleA("ntdll.dll");

        FARPROC etw_event_write = GetProcAddress(ntdll, "EtwEventWrite");
        if (!etw_event_write) return;

        DWORD old = 0;
        VirtualProtect(etw_event_write, 5, PAGE_EXECUTE_READWRITE, &old);

#ifdef _WIN64
        memcpy(etw_event_write, "\x33\xc0\xc2\x14\x00", 5);
#else
        memcpy(etw_event_write, "\x48\x33\xc0\xc3", 4);
#endif

        VirtualProtect(etw_event_write, 5, old, &old);
        FlushInstructionCache(GetCurrentProcess(), etw_event_write, 5);
    }

    static void UnlinkModuleFromPEB(HMODULE hModule)
    {
#ifdef _WIN64
        PPEB pPEB = (PPEB)__readgsqword(0x60);
#else
        PPEB pPEB = (PPEB)__readfsdword(0x30);
#endif

        PLIST_ENTRY CurrentEntry = pPEB->Ldr->InLoadOrderModuleList.Flink;
        PLDR_DATA_TABLE_ENTRY Current = NULL;

        while (CurrentEntry != &pPEB->Ldr->InLoadOrderModuleList && CurrentEntry != NULL)
        {
            Current = CONTAINING_RECORD(CurrentEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
            if (Current->DllBase == hModule)
            {
                UNLINK(Current->InLoadOrderLinks);
                UNLINK(Current->InInitializationOrderLinks);
                UNLINK(Current->InMemoryOrderLinks);
                UNLINK(Current->HashLinks);

                break;
            }

            CurrentEntry = CurrentEntry->Flink;
        }
    }
}