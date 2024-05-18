#ifndef H_B84D8523_CB61_488B_ABB8_B9564FCEB263
#define H_B84D8523_CB61_488B_ABB8_B9564FCEB263

#include <nlohmann/json.hpp>

#include <instrumental/interface.h>

#include <db_connector/accessor.h>

namespace taskmgr
{

struct ITask : public ufa::IBase
{
    virtual void Execute(std::unique_ptr<db::IAccessor>&& accessor) = 0;

    static ufa::Result ParseTask(std::string_view target,
        std::string&& json,
        std::function<void(std::string&&)>&& callback,
        std::unique_ptr<ITask>& task);
};

}  // namespace taskmgr

#endif  // H_B84D8523_CB61_488B_ABB8_B9564FCEB263