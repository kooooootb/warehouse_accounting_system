#include <db_connector/query/utilities.h>
#include <instrumental/string_converters.h>

#include <tracer/tracer_provider.h>

#include <db_connector/query/query_factory.h>
#include <db_connector/query/query_identificator.h>
#include <db_connector/query/query_options.h>

#include "raw_query.h"

namespace srv
{
namespace db
{

/**
 * @brief this class is autotransformed from parametrized query so no public iface
 */
class RawQueryOptions : public srv::tracer::TracerProvider, public IQueryOptions
{
public:
    RawQueryOptions(std::shared_ptr<srv::ITracer> tracer, std::string parametrizedQuery)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_parametrizedQuery(std::move(parametrizedQuery))
        , m_hashValue(std::hash<std::string>{}(m_parametrizedQuery))
    {
        TRACE_DBG << TRACE_HEADER;
    }

    QueryIdentificator GetIdentificator() const override
    {
        return QueryIdentificator::RAW;
    }

    std::string SerializeParametrized(placeholder_t& placeholders) override
    {
        // i really didn't think of it but we can check if given placeholders are default and shift values in stored query otherwise
        if (placeholders.count() > 1)
        {
            // this is such rare and hard scenario
            TRACE_INF << TRACE_HEADER << "Shifting placeholders in parametrized query, count(): " << placeholders.count();

            // we work on m_parametrizedQuery which will exist in all this function
            std::queue<std::string_view> results;
            std::queue<std::string> savedPlaceholders;  // to use view to this
            size_t resultSize = 0;

            placeholder_t refPlaceholders;  // will search in query for this as we assume passed parametrized query starts with $1
            size_t currentIndex = 0;
            size_t prevIndex = 0;

            while ((currentIndex = m_parametrizedQuery.find(refPlaceholders.view(), prevIndex)) != m_parametrizedQuery.npos)
            {
                // save parts of query
                results.emplace(std::string_view(m_parametrizedQuery.data() + prevIndex, currentIndex - prevIndex));
                resultSize += results.back().size();

                // save placeholders
                resultSize += placeholders.view().size();
                savedPlaceholders.emplace(placeholders.get());
                results.emplace(savedPlaceholders.back());  // careful as this natively stores raw pointers

                // shift indices
                currentIndex += refPlaceholders.view().size();
                prevIndex = currentIndex;
                refPlaceholders.next();
                placeholders.next();
            }

            // dont forget last part
            results.emplace(std::string_view(m_parametrizedQuery.data() + prevIndex, m_parametrizedQuery.size() - prevIndex));
            resultSize += results.back().size();

            std::string result;
            result.reserve(resultSize);

            while (!results.empty())
            {
                result.append(results.front());
                results.pop();
            }

            return result;
        }
        else
        {
            return m_parametrizedQuery;
        }
    }

    bool Equals(const IQueryOptions& options) override
    {
        // this options are not planned to use in QueryManager so Equals is long but can be optimized by calculating hash
        TRACE_DBG << TRACE_HEADER;

        if (GetIdentificator() != options.GetIdentificator())
        {
            return false;
        }
        const auto& rawQueryOptions = static_cast<const RawQueryOptions&>(options);

        return m_hashValue == rawQueryOptions.m_hashValue && m_parametrizedQuery == rawQueryOptions.m_parametrizedQuery;
    }

private:
    const std::string m_parametrizedQuery;
    size_t m_hashValue;
};

RawQuery::RawQuery(std::shared_ptr<srv::ITracer> tracer, std::string parametrizedQuery, params_t params)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_queryOptions(std::make_unique<RawQueryOptions>(GetTracer(), std::move(parametrizedQuery)))
    , m_params(std::move(params))
{
    TRACE_DBG << TRACE_HEADER;
}

std::unique_ptr<IQuery> QueryFactory::CreateRaw(std::shared_ptr<srv::ITracer> tracer, std::string parametrizedString, params_t params)
{
    return std::make_unique<RawQuery>(std::move(tracer), std::move(parametrizedString), std::move(params));
}

QueryIdentificator RawQuery::GetIdentificator()
{
    return QueryIdentificator::RAW;
}

params_t RawQuery::ExtractParams()
{
    TRACE_INF << TRACE_HEADER << "Extracing params";

    return std::move(m_params);
}

std::unique_ptr<IQueryOptions> RawQuery::ExtractOptions()
{
    TRACE_INF << TRACE_HEADER << "Extracing options";

    return std::move(m_queryOptions);
}

}  // namespace db
}  // namespace srv
