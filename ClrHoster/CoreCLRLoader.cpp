#include <iostream>
#include <Windows.h>
#include "pch.h"
#include "CoreCLRLoader.h"

constexpr auto FS_SEPARATOR = "\\";
constexpr auto PATH_DELIMITER = ";";
constexpr auto CoreCLRRuntimePrePathX64 = "C:\\Program Files\\dotnet\\shared\\Microsoft.NETCore.App\\";
constexpr auto CoreCLRRuntimePrePath = "C:\\Program Files (x86)\\dotnet\\shared\\Microsoft.NETCore.App\\";
constexpr auto CORECLR_FILE_NAME = "coreclr.dll";


static CoreCLRLoader* thisHandle;

CoreCLRLoader* CoreCLRLoader::GetInstance()
{
    if (thisHandle == NULL)
        thisHandle = new CoreCLRLoader();
    else
        return thisHandle;
}

bool CoreCLRLoader::LoadCoreCLRRuntime(LPCSTR runtimeVersion, LPCSTR dllPathDirectory, LPCSTR dllName)
{
    char runtimePath[MAX_PATH];

#if defined (_WIN64) 
    GetFullPathNameA(CoreCLRRuntimePrePathX64, MAX_PATH, runtimePath, NULL);
#else
    GetFullPathNameA(CoreCLRRuntimePrePath, MAX_PATH, runtimePath, NULL);
#endif

    char* last_slash = strrchr(runtimePath, FS_SEPARATOR[0]);
    if (last_slash != NULL)
        *last_slash = 0;

    DynamicCLRRuntimePath = std::string(runtimePath);
    DynamicCLRRuntimePath.append(FS_SEPARATOR);
    DynamicCLRRuntimePath.append(runtimeVersion);
    DynamicCLRRuntimePath.append(FS_SEPARATOR);
    DynamicCLRRuntimePath.append("coreclr.dll");

    LoadedCoreCLRRuntime = LoadLibraryExA(DynamicCLRRuntimePath.c_str(), NULL, 0);

    if (LoadedCoreCLRRuntime == NULL)
    {
        dprintf("[CoreCLRLoader]: Failed to load CoreCLR from %s\n", DynamicCLRRuntimePath.c_str());
        return false;
    }

    return InitializeCoreCLRRuntime(dllPathDirectory, dllName);
}

bool CoreCLRLoader::InitializeCoreCLRRuntime(LPCSTR dllPathDirectory, LPCSTR dllName)
{
    //Get CoreCLR hosting functions
    InitialzeCoreCLR = (coreclr_initialize_ptr)GetProcAddress(LoadedCoreCLRRuntime, "coreclr_initialize");
    CreateManagedDelegate = (coreclr_create_delegate_ptr)GetProcAddress(LoadedCoreCLRRuntime, "coreclr_create_delegate");
    ShutdownCoreCLR = (coreclr_shutdown_ptr)GetProcAddress(LoadedCoreCLRRuntime, "coreclr_shutdown");

    if (InitialzeCoreCLR == NULL)
    {
        dprintf("[CoreCLRLoader]: coreclr_initialize not found");
        return false;
    }

    if (CreateManagedDelegate == NULL)
    {
        dprintf("[CoreCLRLoader]: coreclr_create_delegate not found");
        return false;
    }

    if (ShutdownCoreCLR == NULL)
    {
        dprintf("[CoreCLRLoader]: coreclr_shutdown not found");
        return false;
    }

    //Construct the trusted platform assemblies (TPA) list
    std::string tpaList;

    tpaList.append(dllPathDirectory);
    tpaList.append(FS_SEPARATOR);
    tpaList.append(dllName);
    tpaList.append(PATH_DELIMITER);

    //Removing string "coreclr.dll" from DynamicCLRRuntimePath, so it can have a full base directory string
    size_t sPos = DynamicCLRRuntimePath.find("coreclr.dll");
    if (sPos != std::string::npos) DynamicCLRRuntimePath.erase(sPos, DynamicCLRRuntimePath.length());
    //End removal

    BuildTPAList(DynamicCLRRuntimePath.c_str(), ".dll", tpaList);

    // Define CoreCLR properties
    const char* propertyKeys[] = { "TRUSTED_PLATFORM_ASSEMBLIES" };// Trusted assemblies 
    const char* propertyValues[] = { tpaList.c_str() };

    //Start the CoreCLR runtime
    int hr = InitialzeCoreCLR(
                    DynamicCLRRuntimePath.c_str(),        // App base path
                    "CoreCLRBootStrapper",       // AppDomain friendly name
                    sizeof(propertyKeys) / sizeof(char*),   // Property count
                    propertyKeys,       // Property names
                    propertyValues,     // Property values
                    &HostHandle,        // Host handle
                    &DomainID);         // AppDomain ID

    if (hr >= 0) {
        dprintf("[CoreCLRLoader]: CoreCLR started\n");
    }
    else
    {
        dprintf("[CoreCLRLoader]: coreclr_initialize failed - status: 0x%08x\n", hr);
        return false;
    }

    return true;
}

bool CoreCLRLoader::UnloadCoreCLRRuntime()
{
    int hr = ShutdownCoreCLR(HostHandle, DomainID);

    if (hr >= 0)
    {
        dprintf("[CoreCLRLoader]: CoreCLR successfully shutdown\n");
        return true;
    }
    else
        dprintf("[CoreCLRLoader]: coreclr_shutdown failed - status: 0x%08x\n", hr);

    return false;
}

void CoreCLRLoader::BuildTPAList(const char* directory, const char* extension, std::string& tpaList)
{
    // This will add all files with a .dll extension to the TPA list.
    // This will include unmanaged assemblies (coreclr.dll, for example) that don't
    // belong on the TPA list. In a real host, only managed assemblies that the host
    // expects to load should be included. Having extra unmanaged assemblies doesn't
    // cause anything to fail, though, so this function just enumerates all dll's in
    // order to keep this sample concise.
    std::string searchPath(directory);
    searchPath.append(FS_SEPARATOR);
    searchPath.append("*");
    searchPath.append(extension);

    WIN32_FIND_DATAA findData;
    HANDLE fileHandle = FindFirstFileA(searchPath.c_str(), &findData);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            // Append the assembly to the list
            tpaList.append(directory);
            tpaList.append(FS_SEPARATOR);
            tpaList.append(findData.cFileName);
            tpaList.append(PATH_DELIMITER);

            // Note that the CLR does not guarantee which assembly will be loaded if an assembly
            // is in the TPA list multiple times (perhaps from different paths or perhaps with different NI/NI.dll
            // extensions. Therefore, a real host should probably add items to the list in priority order and only
            // add a file if it's not already present on the list.
            //
            // For this simple sample, though, and because we're only loading TPA assemblies from a single path,
            // and have no native images, we can ignore that complication.

            //std::cout << "dlls: " + std::string(directory) + FS_SEPARATOR + std::string(findData.cFileName) + PATH_DELIMITER << std::endl;
        }
        while (FindNextFileA(fileHandle, &findData));
        FindClose(fileHandle);
    }
}

bool CoreCLRLoader::InvokeEntryPointMethod(LPCSTR dllNamespace, LPCSTR methodClass, LPCSTR methodName)
{
    EntryPointMethod_ptr managedDelegate;

    int hr = CreateManagedDelegate(HostHandle, DomainID, dllNamespace, methodClass, methodName, (void**)&managedDelegate);

    if (hr >= 0) {
        dprintf("[CoreCLRLoader]: Managed delegate created\n");
    }
    else
    {
        dprintf("[CoreCLRLoader]: coreclr_create_delegate failed - status: 0x%08x\n", hr);
        return false;
    }

    // Invoke the managed delegate and write the returned string to the console
    managedDelegate();

    return true;
}
