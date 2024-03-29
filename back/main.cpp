#include <iostream>
#include <memory>

#include <ifaces/service_locator.h>
#include <ifaces/tracer.h>
#include <locator/service_locator_factory.h>

int main(int argc, char* argv[])
{
    std::cout << "start" << std::endl;

    std::unique_ptr<srv::IServiceLocator> serviceLocator;

    const auto result = srv::CreateServiceLocator(serviceLocator);

    srv::ITracer* tracer;
    serviceLocator->GetInterface(&tracer);

    tracer->Trace();

    std::cout << "end" << std::endl;

    return 0;
}
