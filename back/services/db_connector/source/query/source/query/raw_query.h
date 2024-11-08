#ifndef H_A9323958_45AB_4159_85D6_B1AAA442F670
#define H_A9323958_45AB_4159_85D6_B1AAA442F670

#include <tracer/tracer_provider.h>

#include <db_connector/query/query.h>
#include <db_connector/query/query_options.h>

namespace srv
{
namespace db
{

class RawQuery : public srv::tracer::TracerProvider, public IQuery
{
public:
    RawQuery(std::shared_ptr<srv::ITracer> tracer, std::string parametrizedQuery, params_t params);

    QueryIdentificator GetIdentificator() override;
    params_t ExtractParams() override;
    std::unique_ptr<IQueryOptions> ExtractOptions() override;

private:
    std::unique_ptr<IQueryOptions> m_queryOptions;
    params_t m_params;
};

}  // namespace db
}  // namespace srv

#endif  // H_A9323958_45AB_4159_85D6_B1AAA442F670