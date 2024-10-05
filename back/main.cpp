#include <cstdlib>
#include <iostream>
#include <memory>

#include <authorizer/authorizer.h>
#include <db_connector/accessor.h>
#include <environment/environment.h>
#include <instrumental/common.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer.h>
#include <utilities/document_manager.h>
#include <webserver/server.h>

int main(int argc, char* argv[])
{
    std::cout << "start" << std::endl;

    std::shared_ptr<srv::IServiceLocator> serviceLocator;
    CHECK_SUCCESS(srv::IServiceLocator::Create(serviceLocator));

    if (const auto res = serviceLocator->RegisterDefaultInterface<srv::IEnvironment>()->HandleCommandLine(argc, argv);
        res != ufa::Result::SUCCESS)
    {
        if (res == ufa::Result::DUPLICATE)
        {
            std::cout << "Input key duplicated, fix it then return" << std::endl;
            return EXIT_FAILURE;
        }
        else if (res == ufa::Result::WRONG_FORMAT)
        {
            std::cout << "Extra key in command line parameters, ignoring" << std::endl;
        }
    }

    serviceLocator->RegisterDefaults();

    std::shared_ptr<srv::ITracer> tracer;
    CHECK_SUCCESS(serviceLocator->GetInterface(tracer));
    LOCAL_TRACER(tracer);

    TRACE_INF << TRACE_HEADER << "Services initialized";

    try
    {
        std::shared_ptr<docmgr::IDocumentManager> documentManager = docmgr::IDocumentManager::Create(tracer);

        std::shared_ptr<db::IAccessor> accessor = db::IAccessor::Create(serviceLocator);
        std::shared_ptr<auth::IAuthorizer> authorizer = auth::IAuthorizer::Create(serviceLocator, accessor);
        std::shared_ptr<taskmgr::ITaskManager> taskManager =
            taskmgr::ITaskManager::Create(serviceLocator, accessor, authorizer, documentManager);
        auto server = ws::IServer::Create(serviceLocator, taskManager, authorizer, documentManager);

        server->Start();

        int a;
        std::cin >> a;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception in main: " << ex.what();
    }

    TRACE_INF << TRACE_HEADER << "Exiting main";
    return EXIT_SUCCESS;
}
