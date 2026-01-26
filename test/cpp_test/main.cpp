#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include "cpp/chooseInterface.h"

#include "cpp/outputstream.h"

OutputStream gOutput;

bool gConnected = false;
bool gAutoRun = false;

void connectionChanged(const bool connected, const Firebolt::Error error)
{
    std::cout << "Change in connection: connected: " << connected << " error: " << static_cast<int>(error) << std::endl;
    gConnected = connected;
}

void createFireboltInstance(const std::string& url)
{
    Firebolt::Config config;
    config.wsUrl = url;
    config.waitTime_ms = 1000;
    config.log.level = Firebolt::LogLevel::Debug;

    gConnected = false;
    Firebolt::IFireboltAccessor::Instance().Connect(config, connectionChanged);
}

void destroyFireboltInstance()
{
    Firebolt::IFireboltAccessor::Instance().Disconnect();
}

bool waitOnConnectionReady()
{
    uint32_t waiting = 10000;
    static constexpr uint32_t SLEEPSLOT_TIME = 100;

    // Right, a wait till connection is closed is requested..
    while ((waiting > 0) && (gConnected == false))
    {

        uint32_t sleepSlot = (waiting > SLEEPSLOT_TIME ? SLEEPSLOT_TIME : waiting);
        // Right, lets sleep in slices of 100 ms
        usleep(sleepSlot);
        waiting -= sleepSlot;
    }
    return gConnected;
}

int main(int argc, char** argv)
{
    gOutput = OutputStream("firebolt_test_output.txt");

    gOutput << "Logging to file instead of console" << std::endl;

    // check args for -auto option
    for (int i = 1; i < argc; ++i)
    {
        std::cout << "Arg: " << argv[i] << std::endl;
        if (std::string(argv[i]) == "-auto")
        {
            gAutoRun = true;
            break;
        }
    }

    std::string url = "ws://127.0.0.1:9998";
    createFireboltInstance(url);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    //  ::testing::InitGoogleTest(&argc, argv);
    if (!waitOnConnectionReady())
    {
        std::cout << "Test not able to connect with server..." << std::endl;
        return -1;
    }

    if (gAutoRun)
    {
        gOutput = OutputStream("firebolt_test_output.txt");
        ChooseInterface chooseInterface;
        chooseInterface.autoRun();
        return 0;
    }

    gOutput = OutputStream();

    ChooseInterface chooseInterface;

    for (;;)
    {
        int interfaceIndex = chooseInterface.chooseOption();

        if (interfaceIndex == -1)
        {
            break; // Exit the program
        }

        chooseInterface.runOption(interfaceIndex);
    }

    destroyFireboltInstance();

    return 0;
}
