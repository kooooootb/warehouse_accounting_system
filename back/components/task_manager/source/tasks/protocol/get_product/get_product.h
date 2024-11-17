#ifndef H_113200B1_FFF2_4913_9643_3E60B20912B0
#define H_113200B1_FFF2_4913_9643_3E60B20912B0

#include <string_view>

#include <instrumental/common.h>

#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/product.h>

namespace taskmgr
{
namespace tasks
{

class GetProduct : public BaseTask
{
public:
    GetProduct(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetProduct;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetProduct(srv::IAccessor& accessor);

private:
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view PRODUCT_ID_KEY = "product_id";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view PRETTY_NAME_KEY = "pretty_name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view MAIN_COLOR_KEY = "main_color";

private:
    Product m_product;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_113200B1_FFF2_4913_9643_3E60B20912B0