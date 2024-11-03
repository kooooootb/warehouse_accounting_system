#include <connection/connection.h>
#include <tracer/tracer_provider.h>

#include "raw_transaction.h"

namespace srv
{
namespace db
{
namespace trsct
{

namespace
{

pqxx::write_policy ConvertToPqxx(WritePolicy writePolicy)
{
    switch (writePolicy)
    {
        case WritePolicy::ReadWrite:
            return pqxx::write_policy::read_write;
        case WritePolicy::ReadOnly:
            return pqxx::write_policy::read_only;
        default:
            CHECK_SUCCESS(ufa::Result::ERROR);
    }
}

pqxx::isolation_level ConvertToPqxx(Isolation isolation)
{
    switch (isolation)
    {
        case Isolation::ReadCommited:
            return pqxx::isolation_level::read_committed;
        case Isolation::RepeatableRead:
            return pqxx::isolation_level::repeatable_read;
        case Isolation::Serializable:
            return pqxx::isolation_level::serializable;
        default:
            CHECK_SUCCESS(ufa::Result::ERROR);
    }
}

template <pqxx::isolation_level t_isolationLevel = pqxx::isolation_level::read_committed,
    pqxx::write_policy t_writePolicy = pqxx::write_policy::read_write>
class RawTransaction : public srv::tracer::TracerProvider, public IRawTransaction
{
public:
    RawTransaction(std::shared_ptr<srv::ITracer> tracer, conn::IConnection* connection)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_connection(connection)
    {
        TRACE_INF << TRACE_HEADER;

        ReInitialize(connection);
    }

    void ReInitialize(conn::IConnection* connection) override
    {
        TRACE_INF << TRACE_HEADER;

        m_transactionImpl = std::make_unique<pqxx::transaction<t_isolationLevel, t_writePolicy>>(m_connection->GetRaw());
    }

    pqxx::transaction_base* Get() override
    {
        TRACE_INF << TRACE_HEADER;

        return m_transactionImpl.get();
    }

private:
    conn::IConnection* m_connection;
    std::unique_ptr<pqxx::transaction<t_isolationLevel, t_writePolicy>> m_transactionImpl;
};

template <pqxx::isolation_level t_isolationLevel, pqxx::write_policy t_writePolicy>
std::unique_ptr<IRawTransaction> CreateTransaction(std::shared_ptr<srv::ITracer> tracer, conn::IConnection* connection)
{
    return std::make_unique<RawTransaction<t_isolationLevel, t_writePolicy>>(tracer, connection);
}

}  // namespace

std::unique_ptr<IRawTransaction> IRawTransaction::Create(std::shared_ptr<srv::ITracer> tracer,
    conn::IConnection* connection,
    Isolation isolation,
    WritePolicy writePolicy)
{
    using wp = pqxx::write_policy;
    using il = pqxx::isolation_level;

    // pqxx uses this parameters as template arguments so we parse all values and hide it under iface
    const auto writePolicyPqxx = ConvertToPqxx(writePolicy);
    const auto isolationPqxx = ConvertToPqxx(isolation);
    switch (writePolicyPqxx)
    {
        case wp::read_write:
            switch (isolationPqxx)
            {
                case il::read_committed:
                    return CreateTransaction<il::read_committed, wp::read_write>(std::move(tracer), connection);
                case il::repeatable_read:
                    return CreateTransaction<il::repeatable_read, wp::read_write>(std::move(tracer), connection);
                case il::serializable:
                    return CreateTransaction<il::serializable, wp::read_write>(std::move(tracer), connection);
                default:
                    CHECK_SUCCESS(ufa::Result::ERROR);
            }
        case wp::read_only:
            switch (isolationPqxx)
            {
                case il::read_committed:
                    return CreateTransaction<il::read_committed, wp::read_only>(std::move(tracer), connection);
                case il::repeatable_read:
                    return CreateTransaction<il::repeatable_read, wp::read_only>(std::move(tracer), connection);
                case il::serializable:
                    return CreateTransaction<il::serializable, wp::read_only>(std::move(tracer), connection);
                default:
                    CHECK_SUCCESS(ufa::Result::ERROR);
            }
        default:
            CHECK_SUCCESS(ufa::Result::ERROR);
    }
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
