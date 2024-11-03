#ifndef H_2C74B13B_7E6F_4065_8FBB_6303E886BE04
#define H_2C74B13B_7E6F_4065_8FBB_6303E886BE04

#include "query.h"
#include "query_options.h"

namespace srv
{
namespace db
{

/**
 * @brief static class for constructing IQuery objects for known Options and Values types
 */
class QueryFactory
{
public:
    QueryFactory() = delete;

    template <typename OptionsT, typename ValuesT>
    static std::unique_ptr<IQuery> Create(std::shared_ptr<srv::ITracer> tracer,
        std::unique_ptr<OptionsT>&& options,
        std::unique_ptr<ValuesT>&& values);
};

}  // namespace db
}  // namespace srv

#endif  // H_2C74B13B_7E6F_4065_8FBB_6303E886BE04