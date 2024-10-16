#ifndef H_5E4D1777_BEBB_4CAA_A2AA_0999758E99BF
#define H_5E4D1777_BEBB_4CAA_A2AA_0999758E99BF

#include <boost/asio.hpp>

#include <instrumental/interface.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <webserver/server.h>

#include "session.h"

using tcp = boost::asio::ip::tcp;

namespace ws
{
namespace session
{

class ISessionFactory : public ufa::IBase
{
public:
    virtual std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) = 0;

    static std::unique_ptr<ISessionFactory> Create(const ServerSettings& settings,
        std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<taskmgr::ITaskManager> taskManager);
};

}  // namespace session
}  // namespace ws

#endif  // H_5E4D1777_BEBB_4CAA_A2AA_0999758E99BF