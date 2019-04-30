// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2019 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

#include "localization.h"
#include "rpcserver.h"
#include "rpcclient.h"
#include "init.h"

#include <boost/algorithm/string/predicate.hpp>

void WaitForShutdown(boost::thread_group* threadGroup)
{
    bool fShutdown = ShutdownRequested();

    // Tell the main threads to shutdown.
    while (!fShutdown) {
        MilliSleep(200);
        fShutdown = ShutdownRequested();
    }

    if (threadGroup) {
        threadGroup->interrupt_all();
        threadGroup->join_all();
    }
}

int main(int argc, char* argv[])
{
    boost::thread_group threadGroup;
    int fRet = 0;

    try {
        ParseParameters(argc, argv);

        if (!boost::filesystem::is_directory(GetDataDir(false))) {
            fprintf(stderr, "Error: Specified directory does not exist\n");
            Shutdown();
        }

        ReadConfigFile(mapArgs, mapMultiArgs);

        if (mapArgs.count("-?") || mapArgs.count("--help")) {
            // First part of help message is specific to bitcoind / RPC client
            std::string command = "swippd";
            std::ostringstream help;

            if (argc > 0)
                command = argv[0];

            help << _("Swipp version") << " " << FormatFullVersion() << "\n\n" << _("Usage:") << "\n";
            help << "  " << command << " [options]                     " << "\n";
            help << "  " << command << " [options] <command> [params]  " << _("Send command to daemon") << "\n";
            help << "  " << command << " [options] help                " << _("List commands") << "\n";
            help << "  " << command << " [options] help <command>      " << _("Get help for a command") << "\n";
            help << "\n" << HelpMessage();

            fprintf(stdout, "%s", help.str().c_str());
            return 0;
        } else if (mapArgs.count("-version") || mapArgs.count("--version")) {
            std::ostringstream help;
            help << _("Swipp version") << " " << FormatFullVersion() << "\n";

            fprintf(stdout, "%s", help.str().c_str());
            return 0;
        }

        for (int i = 1; i < argc; i++)
            if (!IsSwitchChar(argv[i][0]) && !boost::algorithm::istarts_with(argv[i], "Swipp:"))
                fCommandLine = true;

        if (fCommandLine) {
            if (!SelectParamsFromCommandLine()) {
                fprintf(stderr, "Error: invalid combination of -regtest and -testnet.\n");
                return 1;
            }

            exit(CommandLineRPC(argc, argv));
        }

#if !defined(WIN32)
        fDaemon = GetBoolArg("-daemon", false);

        if (fDaemon) {
            pid_t pid = fork();

            if (pid < 0) {
                fprintf(stderr, "Error: fork() returned %d errno %d\n", pid, errno);
                return 1;
            }

            if (pid > 0)  {
                CreatePidFile(GetPidFile(), pid);
                return 0;
            }

            // Child process falls through to rest of initialization
            pid_t sid = setsid();

            if (sid < 0)
                fprintf(stderr, "Error: setsid() returned %d errno %d\n", sid, errno);
        }
#endif

        fRet = AppInit2(threadGroup);
    }

    catch (std::exception& e) {
        PrintException(&e, "AppInit()");
    } catch (...) {
        PrintException(NULL, "AppInit()");
    }

    if (fRet > 1) {
        threadGroup.interrupt_all();
        // threadGroup.join_all(); was left out intentionally here, because we didn't re-test all of
        // the startup-failure cases to make sure they don't result in a hang due to some
        // thread-blocking-waiting-for-another-thread-during-startup case
    } else {
        WaitForShutdown(&threadGroup);
    }

    Shutdown();

    if (fRet > 1)
        exit(fRet);
    if (fRet == 0)
        exit(1);

    return 0;
}
