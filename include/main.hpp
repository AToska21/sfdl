#include <iostream>

#ifdef WIIU
#include <wut.h>
#include <whb/log.h>
#include <whb/log_console.h>
#include <whb/proc.h>
#include <coreinit/mcp.h>
#include <mocha/mocha.h>
#include <sysapp/launch.h>
#include <coreinit/exit.h>
#include <coreinit/debug.h>
#include <proc_ui/procui.h>
#include <coreinit/foreground.h>
#include <coreinit/systeminfo.h>
#endif

#include "Downloader.hpp"
#include "FestUtils.hpp"
#include "FsUtils.hpp"      // FileSystem Utilities
#include "ScrUtils.hpp"     // Screen Utilities
#include "Utilities.hpp"    // Utilities (maintenance, version, etc)
#include "Urls.hpp"