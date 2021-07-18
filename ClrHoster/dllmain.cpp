// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "CoreCLRLoader.h"
#include "Toolkit.hpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Perhaps unlink the hoster dll from peb even user doesnt manual map this
        //Tools::UnlinkModuleFromPEB(hModule);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

bool __declspec(dllexport) _stdcall RunExecutable(LPCSTR runtime_version,
                                                  LPCSTR net_file_directory,
                                                  LPCSTR net_file_name,

                                                  LPCSTR net_namespace_name,
                                                  LPCSTR net_class_name,
                                                  LPCSTR net_method_name,

                                                  BYTE unlink_after_execution = 0)
{

    if (strlen(runtime_version) < 1) return false;
    if (strlen(net_file_directory) < 1) return false;
    if (strlen(net_file_name) < 1) return false;
    if (strlen(net_namespace_name) < 1) return false;
    if (strlen(net_class_name) < 1) return false;
    if (strlen(net_method_name) < 1) return false;

    // Load runtime once only
    static bool runtimeLoaded = CoreCLRLoader::GetInstance()->LoadCoreCLRRuntime(runtime_version, net_file_directory, net_file_name);
    static bool etw_patched = false;

    if (!runtimeLoaded) {
        dprintf("Failed loading runtime");
        return false;
    }
    else {
        if (!etw_patched) {
            Tools::PatchETW();
            etw_patched = true;
        }
    }

    if (unlink_after_execution > 0) 
    {
        return CoreCLRLoader::GetInstance()->InvokeEntryPointMethod(net_namespace_name, net_class_name, net_method_name);
    }
    else 
    {
        bool ret = CoreCLRLoader::GetInstance()->InvokeEntryPointMethod(net_namespace_name, net_class_name, net_method_name);
        if (ret) {
            HMODULE module = GetModuleHandleA(net_file_name);
            if (module) {
                Tools::UnlinkModuleFromPEB(module);
            }
        }

        return ret;
    }
}

