#ifndef H_DABCA060_B73E_4ECA_A036_9025F233F4FF
#define H_DABCA060_B73E_4ECA_A036_9025F233F4FF

#include <db_connector/query/query_options.h>
#include <instrumental/common.h>
#include <instrumental/interface.h>

#include <locator/service_locator.h>

#include <db_connector/settings.h>

#include <db_connector/query/query.h>

namespace srv
{
namespace db
{
namespace qry
{

/**
 * @brief query identificator
 */
using queryid_t = uint64_t;

/**
 * @brief holds array of 'supported' queries
 * @details supported queries are those that should be prepared on all connections
 */
struct IQueryManager : public ufa::IBase
{
    /**
     * @brief expose query id and iface
     */
    class QueriesIterator
    {
    public:
        struct Field
        {
            queryid_t id;
            IQueryOptions* queryOptions;
        };

        struct IQueriesIteratorInternal
        {
            virtual Field GetField() = 0;
            virtual void Next() = 0;
            virtual bool Equals(const IQueriesIteratorInternal& it) = 0;
        };

        QueriesIterator(std::unique_ptr<IQueriesIteratorInternal>&& internalIt)
            : m_internalIt(std::move(internalIt))
            , m_curField(m_internalIt->GetField())
        {
        }

        Field operator*() const
        {
            return m_curField;
        }

        Field* operator->()
        {
            return &m_curField;
        }

        QueriesIterator& operator++()
        {
            m_internalIt->Next();
            m_curField = m_internalIt->GetField();

            return *this;
        }

        bool operator==(const QueriesIterator& b)
        {
            return m_internalIt->Equals(*b.m_internalIt);
        };

        bool operator!=(const QueriesIterator& b)
        {
            return !m_internalIt->Equals(*b.m_internalIt);
        };

    private:
        std::unique_ptr<IQueriesIteratorInternal> m_internalIt;
        Field m_curField;
    };

    /**
     * @brief allows iterating through queries props
     * @warning implementation locks queries internal mutex so dispose is asayc
     */
    struct IQueriesLock : ufa::IBase
    {
        virtual QueriesIterator begin() = 0;
        virtual QueriesIterator end() = 0;
        virtual QueriesIterator rbegin() = 0;
        virtual QueriesIterator rend() = 0;
    };

    /**
     * @brief used primarily by connections to refresh their prepared queries
     */
    virtual std::unique_ptr<IQueriesLock> GetQueries() = 0;

    /**
     * @brief will try to retrive queryid if it was supported, if not - add it to supported array
     * @warning eliminates query options functionality
     */
    virtual uint64_t GetOrSupportQueryId(std::unique_ptr<IQuery>&& query) = 0;

    static std::unique_ptr<IQueryManager> Create(const DBConnectorSettings& settings, const std::shared_ptr<IServiceLocator>& locator);
};

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_DABCA060_B73E_4ECA_A036_9025F233F4FF