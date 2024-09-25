#include "FestUtils.hpp"


uint32_t GetHighBytes(uint64_t titleID) {
    return (uint32_t)(titleID >> 32);
}

uint32_t GetLowBytes(uint64_t titleID) {
    return (uint32_t)(titleID & 0xFFFFFFFF);
}
    
namespace FestUtils
{
    const char* g_TempPath = "sfdltemp/";

    enum Version : uint8_t
    {
        Version_USA = 1 << 0,
        Version_EUR = 1 << 1,
        Version_JPN = 1 << 2,
    };

    constexpr uint64_t TitleID_USA = 0x0005000010176900;
    constexpr uint64_t TitleID_EUR = 0x0005000010176A00;
    constexpr uint64_t TitleID_JPN = 0x0005000010162B00;


    FestInstaller::FestInstaller()
    {
        WHBLogPrintf("Initializing FestInstaller...");
        FindInstalledVersions();
    }

    FestInstaller::~FestInstaller()
    {
        WHBLogPrintf("Destroying FestInstaller...");
    }

    void FestInstaller::FindInstalledVersions()
    {
        #ifdef WIIU
        WHBLogPrintf("Finding installed versions...");
        m_InstalledGameVersions = 0;
        if (IsFoundUSA()) {
            m_InstalledGameVersions |= Version_USA;
            WHBLogPrintf("USA version found.");
        }
        if (IsFoundEUR()) {
            m_InstalledGameVersions |= Version_EUR;
            WHBLogPrintf("EUR version found.");
        }
        if (IsFoundJPN()) {
            m_InstalledGameVersions |= Version_JPN;
            WHBLogPrintf("JPN version found.");
        }
        WHBLogPrintf("Installed game versions detected: %d", m_InstalledGameVersions);
        #else
        m_InstalledGameVersions =  Version_USA | Version_EUR | Version_JPN;
        WHBLogPrintf("Defaulting to all regions available in non-Wii U environment.");
        #endif
    }

    bool FestInstaller::IsFoundUSA()
    {
        #ifdef WIIU
        WHBLogPrintf("Checking for USA version...");
        return SYSCheckTitleExists(TitleID_USA);
        #else
        WHBLogPrintf("USA version assumed found (non-Wii U environment).");
        return true;
        #endif
    }

    bool FestInstaller::IsFoundEUR()
    {
        #ifdef WIIU
        WHBLogPrintf("Checking for EUR version...");
        return SYSCheckTitleExists(TitleID_EUR);
        #else
        WHBLogPrintf("EUR version assumed found (non-Wii U environment).");
        return true;
        #endif
    }

    bool FestInstaller::IsFoundJPN()
    {
        #ifdef WIIU
        WHBLogPrintf("Checking for JPN version...");
        return SYSCheckTitleExists(TitleID_JPN);
        #else
        WHBLogPrintf("JPN version assumed found (non-Wii U environment).");
        return true;
        #endif
    }


    constexpr FestFileIdx festFileIdx_USA {
        .festivalByamlID = 0x00000544,
        .panelTextureID = 0x00000545,
        .hapTextureID = 0x00000543,
    };

    constexpr FestFileIdx festFileIdx_EUR {
        .festivalByamlID = 0x0000054d,
        .panelTextureID = 0x0000054c,
        .hapTextureID = 0x0000054e,
    };

    constexpr FestFileIdx festFileIdx_JPN {
        .festivalByamlID = 0x000005d2,
        .panelTextureID = 0x000005d4,
        .hapTextureID = 0x000005d3,
    };



    void inline GetFestFilePathFromStorage(char* buffer, const char* storagePath, uint64_t titleID, uint32_t fileID)
    {
        snprintf(buffer, 0x100, "%s:/usr/boss/%08X/%08X/user/common/data/optdat2/%08x", 
                storagePath, GetHighBytes(titleID), GetLowBytes(titleID), fileID);
    }
    void GetMLCFestFilePath(char* buffer, uint64_t titleID, uint32_t fileID) {
        GetFestFilePathFromStorage(buffer, "storage_mlc01", titleID, fileID);
    }
    void GetUSBFestFilePath(char* buffer, uint64_t titleID, uint32_t fileID) {
        GetFestFilePathFromStorage(buffer, "storage_mlc01", titleID, fileID);
    }

    bool CheckForFestFilesInStorage(char* storage, uint64_t titleID, FestFileIdx idxStruct)
    {
        bool allFound = true;
        char path[0x100];
        GetFestFilePathFromStorage(path, storage, titleID, idxStruct.festivalByamlID);
        // ScrUtils::printf("Checking for %s\n", path);

        if (!FsUtils::CheckFileExists(path))
            allFound = false;

        GetFestFilePathFromStorage(path, storage, titleID, idxStruct.panelTextureID);
        if (!FsUtils::CheckFileExists(path))
            allFound = false;

        GetFestFilePathFromStorage(path, storage, titleID, idxStruct.hapTextureID);
        if (!FsUtils::CheckFileExists(path))
            allFound = false;

        return allFound;
    }


    bool FestInstaller::InstallFestToTitle(uint64_t titleID, FestFileIdx idxStruct)
    {
        WHBLogPrintf("Attempting to install Fest to Title ID: %016llX", titleID);

        // ScrUtils::printf("Full titleID: %016llX\n", titleID);
        // ScrUtils::printf("High bytes: %08X\n", GetHighBytes(titleID));
        // ScrUtils::printf("Low bytes:  %08X\n", GetLowBytes(titleID));
        // ScrUtils::printf("Festival Byaml ID: %08x\n", idxStruct.festivalByamlID);
        // ScrUtils::printf("Panel Texture ID:  %08x\n", idxStruct.panelTextureID);
        // ScrUtils::printf("Hap Texture ID:    %08x\n", idxStruct.hapTextureID);

        // Check if USB storage is mounted before checking MLC
        enum StorageType {
            StorageType_NOT_FOUND,
            StorageType_USB,
            StorageType_MLC,
        };

        StorageType storageType = StorageType_MLC;
        bool isFestOnUSB = false;

        // if (FsUtils::IsUSBMounted()) // USB storage mounted
        // {
        //     WHBLogPrintf("USB storage is mounted. Checking for Fest files on USB...");
        //     if (CheckForFestFilesInStorage("storage_usb", titleID, idxStruct))
        //     {
        //         WHBLogPrintf("Fest files found on USB storage.");
        //         isFestOnUSB = true;
        //         storageType = StorageType_USB;  // Splatfest is on USB
        //     }
        // }
        // if (!isFestOnUSB) // Fest is not on USB, check MLC
            WHBLogPrintf("Fest not found on USB, checking MLC storage...");
            if (CheckForFestFilesInStorage("storage_mlc01", titleID, idxStruct))
            {
                WHBLogPrintf("Fest files found on MLC storage.");
                storageType = StorageType_MLC; // Splatfest is on MLC
            }
            else // Files not found anywhere
            {
                WHBLogPrintf("\nFest files not found on USB or MLC. Forcing installation to MLC.");
                storageType = StorageType_MLC;
            }

        WHBLogPrintf("\nCopying Fest files from SD temp folder to storage...");

        const char* storageDevice = "";
        switch (storageType)
        {
            case StorageType_USB:
                storageDevice = "storage_usb";
                break;
            case StorageType_MLC:
                storageDevice = "storage_mlc01";
                break;
            case StorageType_NOT_FOUND:
                storageDevice = "storage_mlc01";
                break;
        }

        char path[0x100];
        std::string sdPathBase = g_TempPath;
        std::string sdPath = sdPathBase;

        GetFestFilePathFromStorage(path, storageDevice, titleID, idxStruct.festivalByamlID);
        sdPath = sdPathBase + "Festival.byaml";
        FsUtils::CopyFile(sdPath.c_str(), path);

        GetFestFilePathFromStorage(path, storageDevice, titleID, idxStruct.panelTextureID);
        sdPath = sdPathBase + "PanelTexture.bfres";
        FsUtils::CopyFile(sdPath.c_str(), path);

        GetFestFilePathFromStorage(path, storageDevice, titleID, idxStruct.hapTextureID);
        sdPath = sdPathBase + "HapTexture.bfres";
        FsUtils::CopyFile(sdPath.c_str(), path);

        // ScrUtils::printf("Done!\n");
        return true;
    }

    bool FestInstaller::InstallFest()
    {
        WHBLogPrintf("Starting Fest installation process...");
        if (m_InstalledGameVersions == 0) // No games found
        {
            WHBLogPrintf("No game versions installed. Aborting Fest installation.");
            return false;
        }

        if (m_InstalledGameVersions & Version_USA) {
            WHBLogPrintf("Installing Fest for USA version...");
            InstallFestToTitle(TitleID_USA, festFileIdx_USA);
        }
        if (m_InstalledGameVersions & Version_EUR) {
            WHBLogPrintf("Installing Fest for EUR version...");
            InstallFestToTitle(TitleID_EUR, festFileIdx_EUR);
        }
        if (m_InstalledGameVersions & Version_JPN) {
            WHBLogPrintf("Installing Fest for JPN version...");
            InstallFestToTitle(TitleID_JPN, festFileIdx_JPN);
        }

        WHBLogPrintf("Fest installation process completed.");
        return true;
    }
}
