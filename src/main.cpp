#include "main.hpp"
#include <mocha/mocha.h>
#include <sysapp/launch.h>
#include <coreinit/thread.h>
#include <thread>

// Global variables
constexpr int g_AppVersion = 1;


void inline init()
{
    #ifdef WIIU
    // Initialize Process & Logger
    WHBProcInit();
    WHBLogConsoleInit();
    #endif
}

void inline deinit()
{
    #ifdef WIIU
    // Deinitialize Logger & Shutdown Process
    WHBLogConsoleFree();
    SYSLaunchMenu();
    WHBProcShutdown();
    #endif
}

int
main_thread()
{
        while (WHBProcIsRunning()) {
        if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS) {
            ScrUtils::printf("[SFDL--] Failed to init Mocha! Please contact the developers on Discord!\n");
        }

        // ScrUtils::printf("SplatfestDL++\n");
        ScrUtils::printf("~~ Welcome to SplatfestDL++ v%da ~~\n", g_AppVersion);
        // sleep(4);
        ScrUtils::printf("[SFDL--] Loaded SFDL-- patches by andrea, showing credits now\n");
        ScrUtils::printf("special thanks to scraps for testing and giving me false hope </3\n");
        ScrUtils::printf("purplepote for wii u rain\n");
        ScrUtils::printf("sheldon for providing this code\n");
        ScrUtils::printf("shadowdoggo for retiring\n");
        sleep(1);

        // Check if under maintenance
        if (Utilities::CheckMaintenance())
        {
            ScrUtils::printf("Under Maintenance.\n");
            SYSLaunchMenu();
        }

        // Check if the current version is the latest version
        int latestVersion = Utilities::GetLatestVersionNum();
        if (g_AppVersion != latestVersion)
        {
            if (g_AppVersion < latestVersion)
                ScrUtils::printf("New version available!\n");
            else
                ScrUtils::printf("You may be using a pre-release version\n");
        }
        else {
            ScrUtils::printf("You are using the latest version\n");
        }

        // Ask the user if they want to download the latest version if they aren't already.
        if (g_AppVersion < latestVersion)
        {
            if (ScrUtils::AskForYesNo("Do you want to download the latest version?"))
            {
                ScrUtils::printf("Downloading latest version...\n");
                ScrUtils::printf("[!] Feature not implemented yet\n");
                // Utilities::DownloadLatestVersion();
            }
            else ScrUtils::printf("Continuing without downloading the latest version\n");
        }


        #ifdef WIIU
        // 
        ScrUtils::printf("[SFDL--] Skipping modloader because this is the aroma build.\n");
        // Utilities::DownloadModLoader();
        #endif


        // ScrUtils::AskForAPressToContinue();
        ScrUtils::printf("\n");
        
        Tools::Downloader d;

        std::string tempDir = FestUtils::g_TempPath;
        if (FsUtils::EnsureDirExists(tempDir.c_str()))
        {
            ScrUtils::printf("Downloading Splatfest data...\n");
            d.DownloadToFile(Urls::FestivalByaml, tempDir + "Festival.byaml");
            d.DownloadToFile(Urls::PanelTextureBfres, tempDir + "PanelTexture.bfres");
            d.DownloadToFile(Urls::HapTextureBfres, tempDir + "HapTexture.bfres");
            // d.DownloadToFile(Urls::GeckoCodes, tempDir + "SFDL.gctu");
        }
        else
        {
            ScrUtils::printf("Failed to create temporary folder on the SD card!\n");
            ScrUtils::printf("(Is your SD card locked?)\n");
        }

    #ifdef WIIU
        // FsUtils::OpenIOSU();
        // if (FsUtils::IsExploitedIOSU())
        // {
            // Mount MLC
            VirtualPathUtils::MountMLC();

            // Copy the files from the SD card to their correct locations
            FestUtils::FestInstaller fest;
            ScrUtils::printf("Found: %s%s%s\n", 
                fest.IsFoundUSA() ? "USA " : "",
                fest.IsFoundEUR() ? "EUR " : "",
                fest.IsFoundJPN() ? "JPN " : "");
                                
            fest.InstallFest();
            ScrUtils::printf("Splatfest installed\n");

            // Unmount the MLC
            VirtualPathUtils::UnmountMLC();
            //FsUtils::CloseIOSU();
        // }
    #endif

        // Check if special mod files need updated
        // ScrUtils::printf("Checking if speical mod files need updated...\n");

        // Check Splatfest info
        ScrUtils::printf("Checking Splatfest info...\n");
        //Utils::SpecialFestInstaller sf;

        // Steps:
        // 1. Download Fest info
        // 2. Check if splatfest is special
        // 3. If it is, check if we are within the special fest time
        // 4. If we are, download the special mod files.
        // 5. Otherwise, do not download the special splatfest files yet.
        // If we are past the endTime, we can remove the special fest files.
        // (If we are not in the open period, we can remove the special fest files.)


        // 1. Download Fest info
        // sf.GetLatestFestInfo();

        // 2. Check if 

        // sf.GetLatestFestInfo();

        // if (sf.IsDownloadActive() && sf.IsSpecialFest())
        // {
        //     ScrUtils::printf("Special fest is active\n");
        // }



        ScrUtils::printf("[SFDL--] Not removing temporary files due to Aroma limits!\n");
        // FsUtils::RemoveDirRecursive(FestUtils::g_TempPath);

        ScrUtils::printf("\nPress A to exit...");
        ScrUtils::waitForKeyPress();
        return 0;
    }
    return 0;
}

int main()
{
    // Initialize (Wii U)
    WHBProcInit();
    WHBLogConsoleInit();

    std::thread t(main_thread);
    t.join();

    // Deinitialize (Wii U)
    WHBLogConsoleFree();
    WHBProcShutdown();
    return 0;
}

