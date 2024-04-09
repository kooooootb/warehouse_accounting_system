#ifndef H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198
#define H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer_provider.h>
#include <utilities/document_manager.h>
#include <webserver/server.h>

#include "listener.h"
#include "session_factory.h"

namespace asio = boost::asio;  // dumb clangd cant find asio ns w/out using

namespace ws
{

class Server : public IServer, public srv::tracer::TracerProvider
{
public:
    Server(std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<auth::IAuthorizer> authorizer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager);

    ~Server() noexcept override;

    ufa::Result Start() override;
    ufa::Result Stop() override;
    void SetSettings(ServerSettings&& settings) override;

private:
    void SetWorkers(int numWorkers);
    void RunWorker();

    void FillEmptySettings(ServerSettings& settings);

private:
    std::shared_ptr<asio::io_context> m_ioContext;

    std::vector<std::unique_ptr<std::thread>> m_workers;
    asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;

    std::shared_ptr<Listener> m_listener;

    std::shared_ptr<docmgr::IDocumentManager> m_documentManager;
    bool m_savedIsSecured{DEFAULT_IS_SECURED};

    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
    std::shared_ptr<auth::IAuthorizer> m_authorizer;
};

}  // namespace ws

#endif  // H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198