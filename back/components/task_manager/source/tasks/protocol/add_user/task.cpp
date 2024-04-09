#include <db_connector/data/dependency.h>
#include <db_connector/data/user.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

AddUser::AddUser(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result AddUser::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    result = json().dump();
    auto accessor = depManager.GetAccessor();

    return accessor->AddUser(m_initiativeUser, m_user);
}

void AddUser::ParseInternal(json&& json)
{
    m_user.name = json.at(LOGIN_ID_KEY).get<std::string>();
    m_user.firstName = json.at(FIRST_NAME_ID_KEY).get<std::string>();
    m_user.lastName = json.at(SECOND_NAME_ID_KEY).get<std::string>();
    m_user.hashPassword = json.at(PERPASSWORD_ID_KEY).get<std::string>();
    m_user.role = static_cast<db::data::User::Role>(json.at(PERROLE_ID_KEY).get<uint64_t>());
}

}  // namespace tasks
}  // namespace taskmgr
