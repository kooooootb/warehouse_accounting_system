#include <iostream>
#include <memory>
#include "instrumental/include/instrumental/check.h"

#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

int main(int argc, char* argv[])
{
    std::cout << "start" << std::endl;

    std::unique_ptr<srv::IServiceLocator> serviceLocator;
    CHECK_SUCCESS(srv::CreateServiceLocator(serviceLocator));

    std::shared_ptr<srv::ITracer> tracer;
    CHECK_SUCCESS(serviceLocator->GetInterface(tracer));
    LOCAL_TRACER(tracer);

    srv::tracer::TracerSettings settings;
    settings.traceFolder = "/home/kotb/notes/rms";
    settings.traceLevel = srv::tracer::TraceLevel::DEBUG;
    tracer->SetSettings(std::move(settings));

    TRACE_INF << "asd " << std::to_string((long)tracer.get());

    std::cout << "end" << std::endl;

    return 0;
}
