#pragma once

#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

#include <whb/log.h>
#include <whb/log_console.h>
#include <vpad/input.h>

namespace ScrUtils
{
    int printf(const char *format, ...);
    bool waitForKeyPress();
    void AskForAPressToContinue();
    bool AskForYesNo(const char *question);
}