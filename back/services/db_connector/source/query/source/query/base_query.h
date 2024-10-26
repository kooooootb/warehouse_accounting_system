#ifndef H_2AFC8974_96E9_4DC8_8588_41287C4DBAB2
#define H_2AFC8974_96E9_4DC8_8588_41287C4DBAB2

#include <tracer/tracer_provider.h>

#include <db_connector/query/query.h>
#include <db_connector/query/query_factory.h>

namespace srv
{
namespace db
{

/**
 * @brief helper class for copy paste functionalities
 */
template <typename OptionsT, typename ValuesT>
class BaseQuery : public srv::tracer::TracerProvider, public IQuery
{
public:
    BaseQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<OptionsT>&& options, std::unique_ptr<ValuesT>&& values);

    std::unique_ptr<IQueryOptions> ExtractOptions() override;

protected:
    std::unique_ptr<OptionsT> m_options;
    std::unique_ptr<ValuesT> m_values;
};

template <typename OptionsT, typename ValuesT>
BaseQuery<OptionsT, ValuesT>::BaseQuery(std::shared_ptr<srv::ITracer> tracer,
    std::unique_ptr<OptionsT>&& options,
    std::unique_ptr<ValuesT>&& values)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_options(std::move(options))
    , m_values(std::move(values))
{
}

template <typename OptionsT, typename ValuesT>
std::unique_ptr<IQueryOptions> BaseQuery<OptionsT, ValuesT>::ExtractOptions()
{
    return std::move(m_options);
}

}  // namespace db
}  // namespace srv

#endif  // H_2AFC8974_96E9_4DC8_8588_41287C4DBAB2