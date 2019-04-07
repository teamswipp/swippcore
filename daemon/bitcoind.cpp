// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017-2018 The Swipp developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "rpcserver.h"
#include "rpcclient.h"
#include "init.h"

#include <boost/algorithm/string/predicate.hpp>

void WaitForShutdown(boost::thread_group* threadGroup)
{
    bool fShutdown = ShutdownRequested();
    // Tell the main threads to shutdown.
    while (!fShutdown)
    {
        MilliSleep(200);
        fShutdown = ShutdownRequested();
    }

    if (threadGroup)
    {
        threadGroup->interrupt_all();
        threadGroup->join_all();
    }
}

bool AppInit(int argc, char* argv[])
{
    boost::thread_group threadGroup;
    bool fRet = false;

    try
    {
        // If Qt is used, parameters/bitcoin.conf are parsed in qt/bitcoin.cpp's main()
        ParseParameters(argc, argv);

        if (!boost::filesystem::is_directory(GetDataDir(false)))
        {
            fprintf(stderr, "Error: Specified directory does not exist\n");
            Shutdown();
        }

        ReadConfigFile(mapArgs, mapMultiArgs);

        if (mapArgs.count("-?") || mapArgs.count("--help"))
        {
            // First part of help message is specific to bitcoind / RPC client
            std::string command = "swippd";
            std::ostringstream help;

            if (argc > 0)
                command = argv[0];

            help << _("Swipp version") << " " << FormatFullVersion() << "\n\n" << _("Usage:") << "\n";
            help << "  " << command << " [options]                     " << "\n";
            help << "  " << command << " [options] <command> [params]  " << _("Send command to -server or daemon") << "\n";
            help << "  " << command << " [options] help                " << _("List commands") << "\n";
            help << "  " << command << " [options] help <command>      " << _("Get help for a command") << "\n";
            help << "\n" << HelpMessage();

            fprintf(stdout, "%s", help.str().c_str());
            return false;
        }

        for (int i = 1; i < argc; i++)
            if (!IsSwitchChar(argv[i][0]) && !boost::algorithm::istarts_with(argv[i], "Swipp:"))
                fCommandLine = true;

        if (fCommandLine)
        {
            if (!SelectParamsFromCommandLine())
            {
                fprintf(stderr, "Error: invalid combination of -regtest and -testnet.\n");
                return false;
            }

            int ret = CommandLineRPC(argc, argv);
            exit(ret);
        }

#if !defined(WIN32)
        fDaemon = GetBoolArg("-daemon", false);

        if (fDaemon)
        {
            pid_t pid = fork();

            if (pid < 0)
            {
                fprintf(stderr, "Error: fork() returned %d errno %d\n", pid, errno);
                return false;
            }

            if (pid > 0)
            {
                CreatePidFile(GetPidFile(), pid);
                return true;
            }

            // Child process falls through to rest of initialization
            pid_t sid = setsid();

            if (sid < 0)
                fprintf(stderr, "Error: setsid() returned %d errno %d\n", sid, errno);
        }
#endif

        fRet = AppInit2(threadGroup);
    }

    catch (std::exception& e)
    {
        PrintException(&e, "AppInit()");
    }
    catch (...)
    {
        PrintException(NULL, "AppInit()");
    }

    if (!fRet)
    {
        threadGroup.interrupt_all();
        // threadGroup.join_all(); was left out intentionally here, because we didn't re-test all of
        // the startup-failure cases to make sure they don't result in a hang due to some
        // thread-blocking-waiting-for-another-thread-during-startup case
    }
    else
        WaitForShutdown(&threadGroup);

    Shutdown();
    return fRet;
}

extern void noui_connect();

int main(int argc, char* argv[])
{
    bool fRet = false;
    fHaveGUI = false;

    noui_connect();
    fRet = AppInit(argc, argv);

    if (fRet && fDaemon)
        return 0;

    return (fRet ? 0 : 1);
}
