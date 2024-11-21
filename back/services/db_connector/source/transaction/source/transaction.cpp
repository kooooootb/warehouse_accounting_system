#include <db_connector/transaction_entry/exceptions/not_found_exception.h>

#include "raw_transaction.h"
#include "transaction.h"

namespace srv
{
namespace db
{
namespace trsct
{

Transaction::Transaction(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<qry::IQueryManager> queryManager,
    conn::ConnectionProxy&& connection,
    uint32_t commitAttempts,
    Isolation isolation,
    WritePolicy writePolicy)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_queryManager(std::move(queryManager))
    , m_connection(std::move(connection))
    , m_transactionImpl(IRawTransaction::Create(GetTracer(), &m_connection.GetConnection(), isolation, writePolicy))
    , m_entryFactory(std::make_unique<TransactionEntryFactory>(GetTracer(),
          m_queryManager,
          &m_connection.GetConnection(),
          m_transactionImpl.get()))
    , m_commitAttempts(commitAttempts)
{
    TRACE_INF << TRACE_HEADER << "Isolation level: " << isolation << ", Write policy: " << writePolicy;
}

void Transaction::SetRootEntry(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER << "ptr: " << entry.get();

    m_entry = std::move(entry);
}

ufa::Result Transaction::Execute()
{
    TRACE_DBG << TRACE_HEADER;

    if (m_entry == nullptr)
    {
        // there is nothing we can do
        TRACE_WRN << TRACE_HEADER << "Called Execute with empty entry";
        return ufa::Result::SUCCESS;
    }

    uint32_t attemptsRemaining = m_commitAttempts == 0 ? 1 : m_commitAttempts;

    do
    {
        try
        {
            auto* currentEntry = m_entry.get();

            while (currentEntry != nullptr)
            {
                currentEntry->Execute();
                currentEntry = currentEntry->GetNext();
            }

            m_transactionImpl->Get()->commit();

            return ufa::Result::SUCCESS;
        }
        catch (const pqxx::serialization_failure& ex)
        {
            TRACE_WRN << TRACE_HEADER << "Serialization conflict, will retry, remaining attempts: " << attemptsRemaining
                      << "what(): " << ex.what();

            m_transactionImpl->ReInitialize(&m_connection.GetConnection());
            if (m_commitAttempts > 0)
            {
                attemptsRemaining -= 1;
            }
        }
        catch (const pqxx::integrity_constraint_violation& ex)
        {
            TRACE_ERR << TRACE_HEADER << "Constraint violated, ex.what(): " << ex.what();
            return ufa::Result::VIOLATION;
        }
        catch (const exps::NotFound& ex)
        {
            TRACE_ERR << TRACE_HEADER << "Received not found exception, what(): " << ex.what();
            return ufa::Result::NOT_FOUND;
        }
        catch (const pqxx::in_doubt_error& ex)
        {
            TRACE_ERR << TRACE_HEADER << "Received in_doubt_error, what(): " << ex.what();
            return ufa::Result::ERROR;
        }
        catch (const std::exception& ex)
        {
            // can be recoverable but from docs seems that other errors shouldnt be retried
            TRACE_ERR << TRACE_HEADER << "Exception on commit, what(): " << ex.what();
            return ufa::Result::ERROR;
        }
    } while (attemptsRemaining > 0);

    // we ran out of attempts
    return ufa::Result::ERROR;
}

ITransactionEntryFactory& Transaction::GetEntriesFactory()
{
    return *m_entryFactory;
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
