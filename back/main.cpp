#include <iostream>
#include <memory>

#include <db_connector/accessor.h>
#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer.h>
#include <webserver/server.h>

int main(int argc, char* argv[])
{
    std::cout << "start" << std::endl;

    std::shared_ptr<srv::IServiceLocator> serviceLocator;
    CHECK_SUCCESS(srv::IServiceLocator::Create(serviceLocator));

    std::shared_ptr<srv::ITracer> tracer;
    CHECK_SUCCESS(serviceLocator->GetInterface(tracer));
    LOCAL_TRACER(tracer);

    TRACE_INF << "Services initialized";

    std::shared_ptr<db::IAccessor> accessor;

    std::shared_ptr<taskmgr::ITaskManager> taskManager;
    CHECK_SUCCESS(taskmgr::ITaskManager::Create(serviceLocator, accessor, taskManager));

    std::shared_ptr<ws::IServer> server;
    CHECK_SUCCESS(ws::IServer::Create(serviceLocator, taskManager, server));

    server->Start();

    int a;
    std::cin >> a;

    TRACE_INF << "Exiting main";
    return 0;
}
