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
 * @brief helper class for copy paste functionalities, handles options, values are transformed and stored in implementations
 */
template <typename OptionsT>
class BaseQuery : public srv::tracer::TracerProvider, public IQuery
{
public:
    BaseQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<OptionsT>&& options);

    std::unique_ptr<IQueryOptions> ExtractOptions() override;

protected:
    std::unique_ptr<OptionsT> m_options;
};

template <typename OptionsT>
BaseQuery<OptionsT>::BaseQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<OptionsT>&& options)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_options(std::move(options))
{
}

template <typename OptionsT>
std::unique_ptr<IQueryOptions> BaseQuery<OptionsT>::ExtractOptions()
{
    TRACE_INF << TRACE_HEADER << "Extracting options";
    return std::move(m_options);
}

}  // namespace db
}  // namespace srv

#endif  // H_2AFC8974_96E9_4DC8_8588_41287C4DBAB2