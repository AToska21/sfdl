#include "main.hpp"
#include <mocha/mocha.h>
#include <sysapp/launch.h>
#include <coreinit/exit.h>

#include <nn/erreula.h>
#include <coreinit/debug.h>
#include <coreinit/filesystem.h>
#include <coreinit/memdefaultheap.h>
#include <proc_ui/procui.h>
#include <coreinit/foreground.h>
#include <coreinit/systeminfo.h>
#include <whb/proc.h>
#include <whb/log_console.h>
#include <rpxloader/rpxloader.h>
// Global variables
constexpr int g_AppVersion = 1;


void exitloop()
{
    SYSLaunchMenu();
}


void mainloop()
{
    // ScrUtils::printf("[DEBUG] Running main loop.");
    while (WHBProcIsRunning()) {
                    ScrUtils::printf("\n");
                    ScrUtils::printf("Ho ho ho, Merry Squidmas everyone!");
                    ScrUtils::printf("Here's to another 100 years of Splatfestival!");
                    ScrUtils::printf("~~ SplatfestDL v%d ~~\n", g_AppVersion);
                    ScrUtils::printf("Special Thanks to scraps for testing, love you <3\n");
                    ScrUtils::printf("and Sheldon for providing this code.\n");
                    ScrUtils::printf("Please note, due to recent Inkay changes, the program will run itself twice.");
                    ScrUtils::printf("Starting in 10 seconds.\n");
                    sleep(10);

                    // Check if under maintenance
                    if (Utilities::CheckMaintenance())
                    {
                        ScrUtils::printf("Under Maintenance.\n");
                        sleep(10);
                        exitloop();
                        SYSLaunchMenu();
                        WHBProcShutdown();
                    }

                    // Check for updates
                    int latestVersion = Utilities::GetLatestVersionNum();
                    if (g_AppVersion != latestVersion)
                    {
                        if (g_AppVersion < latestVersion)
                            ScrUtils::printf("New version available!\n");
                        else
                            ScrUtils::printf("You may be using a pre-release version.\n");
                    }
                    else
                    {
                        ScrUtils::printf("You are using the latest version!\n");
                    }

                    // Prompt for update download
                    if (g_AppVersion < latestVersion)
                    {
                        if (ScrUtils::AskForYesNo("Do you want to download the latest version?"))
                        {
                            ScrUtils::printf("Downloading latest version...\n");
                            Utilities::DownloadLatestVersion();
                            ScrUtils::printf("Finished downloading! Sending you to the HOME Menu.\n");
                            sleep(3);
                            exitloop();
                            SYSLaunchMenu();
                            WHBProcShutdown();
                        }
                        else
                        {
                            ScrUtils::printf("Continuing without downloading the latest version.\n");
                        }
                    }

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

                    VirtualPathUtils::MountMLC();
                    FestUtils::FestInstaller fest;
                    ScrUtils::printf("Found: %s%s%s\n", 
                        fest.IsFoundUSA() ? "USA " : "",
                        fest.IsFoundEUR() ? "EUR " : "",
                        fest.IsFoundJPN() ? "JPN " : "");

                    fest.InstallFest();
                    ScrUtils::printf("Splatfest installed\n");
                    VirtualPathUtils::UnmountMLC();

                    SYSLaunchMenu();
                    exitloop();
                    SYSLaunchMenu();
                    return 0;
    }
}

int main() {
    WHBProcInit();
    OSEnableHomeButtonMenu(true);
    WHBLogConsoleInit();
    if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS)
    {
        OSFatal("[SFDL] Failed to init Mocha! Please contact the developers on Discord!");
    }
    RPXLoaderStatus resRPX;
    if ((resRPX = RPXLoader_InitLibrary()) == RPX_LOADER_RESULT_SUCCESS) {
        if ((resRPX = RPXLoader_UnmountCurrentRunningBundle()) != RPX_LOADER_RESULT_SUCCESS) {
            OSFatal("[SFDL] Failed to unmount WUHB.");
        }
        RPXLoader_DeInitLibrary();
    } else {
        OSFatal("RPXLoader_InitLibrary failed.");
    }
    mainloop();
    SYSLaunchMenu();
    Mocha_DeInitLibrary();
    WHBLogConsoleFree();
    WHBProcShutdown();
    return 0;
}