#include "Core.h"
#include "Configurator.h"
#include "Communicator.h"
#include "Log.h"

#include <thread>

int main(int argc, char **argv)
{
    CLog::Log(LOG_INFO, "Starting application...");

    QApplication app(argc, argv);

    if(!g_pCore->Init())
    {
        CLog::Log(LOG_WARNING, "CCore is not valid, closing the program");
        return 0;
    }

    CConfigurator::Instance();

    std::thread coreThread(&CCore::Run, g_pCore);

    int uiResult = app.exec();
    CLog::Log(LOG_INFO, "Application is done with code %d, closing CCore...", uiResult);

    g_pCore->Invalidate();
    coreThread.join();

    CLog::Log(LOG_INFO, "Destroying singletons...");

    CCore::Destroy();
    CConfigurator::Destroy();
    CCommunicator::Destroy();

    CLog::Log(LOG_INFO, "Done!");

    return uiResult;
 }