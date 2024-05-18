#ifndef H_5E4D1777_BEBB_4CAA_A2AA_0999758E99BF
#define H_5E4D1777_BEBB_4CAA_A2AA_0999758E99BF

#include <boost/asio.hpp>

#include <instrumental/interface.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer.h>
#include <webserver/server.h>

#include "session.h"

using tcp = boost::asio::ip::tcp;

namespace ws
{

class ISessionFactory : public ufa::IBase
{
public:
    static std::unique_ptr<ISessionFactory> CreateSessionFactory(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        bool isSecured,
        std::string documentRoot);

    virtual std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) = 0;
};

}  // namespace ws

#endif  // H_5E4D1777_BEBB_4CAA_A2AA_0999758E99BF