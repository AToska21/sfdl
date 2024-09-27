#include "main.hpp"
#include <mocha/mocha.h>
#include <sysapp/launch.h>
#include <coreinit/exit.h>

#include <coreinit/debug.h>
#include <proc_ui/procui.h>
#include <coreinit/foreground.h>
#include <coreinit/systeminfo.h>
#include <whb/proc.h>
// Global variables
constexpr int g_AppVersion = 1;

void saveProcessCallback() {
    OSSavesDone_ReadyToRelease();
}

void inline init()
{
    #ifdef WIIU
    OSEnableHomeButtonMenu(1);
    ProcUIInit(&saveProcessCallback);
    WHBLogConsoleInit();
    #endif
}

void inline deinit()
{
    #ifdef WIIU
    WHBLogConsoleFree();
    SYSLaunchMenu();
    ProcUIShutdown();
    #endif
}

void mainloop()
{
    ScrUtils::printf("[DEBUG] Running main loop.");
    while (WHBProcIsRunning()) {
                    ScrUtils::printf("\n");
                    sleep(4);
                    // Your application logic goes here
                    ScrUtils::printf("~~ SplatfestDL++ v%d ~~\n", g_AppVersion);
                    ScrUtils::printf("[SFDL-A] Loaded SFDL Aroma patches by andrea, showing credits now\n");
                    ScrUtils::printf("special thanks to scraps for testing love you <3\n");
                    ScrUtils::printf("purplepote for wii u rain\n");
                    ScrUtils::printf("sheldon for providing this code\n");
                    ScrUtils::printf("shadowdoggo for retiring\n");
                    ScrUtils::printf("DISCLAIMER: It will appear as if\n");
                    ScrUtils::printf("the script runs twice. This is due to\n");
                    ScrUtils::printf("Aroma limitations. Sorry.\n");
                    ScrUtils::printf("Press A to start.\n");
                    ScrUtils::waitForKeyPress();

                    // Check if under maintenance
                    // if (Utilities::CheckMaintenance())
                    // {
                    //      ScrUtils::printf("Under Maintenance.\n");
                    //      sleep(10);
                    //      OSFatal("Servers under maintenance. System halted. Manually reboot your Wii U.");
                    // }

                    // // Check for updates
                    // int latestVersion = Utilities::GetLatestVersionNum();
                    // if (g_AppVersion != latestVersion)
                    // {
                    //     if (g_AppVersion < latestVersion)
                    //         ScrUtils::printf("New version available!\n");
                    //     else
                    //         ScrUtils::printf("You may be using a pre-release version\n");
                    // }
                    // else
                    // {
                    //     ScrUtils::printf("You are using the latest version\n");
                    // }

                    // // Prompt for update download
                    // if (g_AppVersion < latestVersion)
                    // {
                    //     if (ScrUtils::AskForYesNo("Do you want to download the latest version?"))
                    //     {
                    //         ScrUtils::printf("Downloading latest version...\n");
                    //         ScrUtils::printf("[!] Feature not implemented yet\n");
                    //     }
                    //     else
                    //     {
                    //         ScrUtils::printf("Continuing without downloading the latest version\n");
                    //     }
                    // }

                    #ifdef WIIU
                    ScrUtils::printf("\n");
                    #endif

                    ScrUtils::printf("\n");

                    Tools::Downloader d;
                    std::string tempDir = FestUtils::g_TempPath;
                    if (FsUtils::EnsureDirExists(tempDir.c_str()))
                    {
                        ScrUtils::printf("Downloading Splatfest data...\n");
                        d.DownloadToFile(Urls::FestivalByaml, tempDir + "Festival.byaml");
                        d.DownloadToFile(Urls::PanelTextureBfres, tempDir + "PanelTexture.bfres");
                        d.DownloadToFile(Urls::HapTextureBfres, tempDir + "HapTexture.bfres");
                    }
                    else
                    {
                        ScrUtils::printf("Failed to create temporary folder on the SD card!\n");
                        ScrUtils::printf("(Is your SD card locked?)\n");
                    }

                    #ifdef WIIU
                    VirtualPathUtils::MountMLC();
                    FestUtils::FestInstaller fest;
                    ScrUtils::printf("Found: %s%s%s\n", 
                        fest.IsFoundUSA() ? "USA " : "",
                        fest.IsFoundEUR() ? "EUR " : "",
                        fest.IsFoundJPN() ? "JPN " : "");

                    fest.InstallFest();
                    ScrUtils::printf("Splatfest installed\n");
                    VirtualPathUtils::UnmountMLC();
                    #endif

                    ScrUtils::printf("Checking Splatfest info...\n");
                    // FsUtils::RemoveDirRecursive(FestUtils::g_TempPath);
                    ScrUtils::printf("Installation done! Remove the sfdltemp path on your SD card when the fest is over.");
                    ScrUtils::printf("\nPress A to exit...");
                    ScrUtils::waitForKeyPress();
                    SYSLaunchMenu();
    }
}


int main() {
    WHBProcInit();
    OSEnableHomeButtonMenu(1);
    WHBLogConsoleInit();
    if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS)
    {
        OSFatal("[SFDL--] Failed to init Mocha! Please contact the developers on Discord!");
    }
    mainloop();
    Mocha_DeInitLibrary();
    WHBLogConsoleFree();
    WHBProcShutdown();
    return 0;
}