#ifndef H_D756230B_A69E_4E61_96D3_CCF56124A8CA
#define H_D756230B_A69E_4E61_96D3_CCF56124A8CA

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

class GetProductList : public BaseTask
{
public:
    GetProductList(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetProductList;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetProductList(srv::IAccessor& accessor);

private:
    static constexpr std::string_view PRODUCT_ID_KEY = "product_id";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view PRETTY_NAME_KEY = "pretty_name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view MAIN_COLOR_KEY = "main_color";
    static constexpr std::string_view RESULT_KEY = "result";
    static constexpr std::string_view LIMIT_KEY = "limit";
    static constexpr std::string_view OFFSET_KEY = "offset";

private:
    std::vector<Product> m_products;
    int64_t m_limit;
    int64_t m_offset;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_D756230B_A69E_4E61_96D3_CCF56124A8CA