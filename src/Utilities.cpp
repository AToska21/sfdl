#include "Utilities.hpp"
#include <coreinit/debug.h>

namespace Utilities
{
    bool CheckMaintenance()
    {
        Tools::Downloader d;
        std::string webResponse;

        d.DownloadToString(URL_MAINTENANCE, webResponse);

        // Convert the string to an integer
        int response = 1;
        response = std::atoi(webResponse.c_str());

        if (response != 0)
            return true;

        return false;
    }

    int GetLatestVersionNum()
    {
        Tools::Downloader d;
        std::string webResponse;

        d.DownloadToString(URL_VERSION, webResponse);

        // Convert the string to an integer
        int response = 0;
        response = std::atoi(webResponse.c_str());

        return response;
    }

    void DownloadLatestVersion()
    {
        if (!FsUtils::EnsureDirExists("wiiu/apps"))
        {
            OSFatal("wiiu/apps does not exist, is your SD card feeling okay?");
            return;
        }

        Tools::Downloader d;

        d.DownloadToFile(URL_SFDL_APPRPX,  "wiiu/apps/SFDL.wuhb");
    }

    bool DownloadModLoader()
    {
        if (!FsUtils::EnsureDirExists("wiiu/apps/SplatfestModLoader/"))
        {
            ScrUtils::printf("Failed to create directories\n");
            return false;
        }

        Tools::Downloader d;

        d.DownloadToFile(URL_SFML_APPEXEC,  "wiiu/apps/SplatfestModLoader/SFML.elf");
        d.DownloadToFile(URL_SFML_APPICON,  "wiiu/apps/SplatfestModLoader/icon.png");
        d.DownloadToFile(URL_SFML_APPMETA,  "wiiu/apps/SplatfestModLoader/meta.xml");

        return true;
    }
}