#ifndef H_37028FFA_8E9A_4EDD_8916_F30BD46BDCD6
#define H_37028FFA_8E9A_4EDD_8916_F30BD46BDCD6

#include <tracer/tracer_provider.h>

#include <db_connector/transaction_policies.h>

#include <connection/connection.h>

namespace srv
{
namespace db
{
namespace trsct
{

/**
 * @brief this class will hold pqxx transaction and reinitialize it for retry if needed
 */
struct IRawTransaction : public ufa::IBase
{
    virtual void ReInitialize(conn::IConnection* connection) = 0;

    virtual pqxx::transaction_base* Get() = 0;

    static std::unique_ptr<IRawTransaction> Create(std::shared_ptr<srv::ITracer> tracer,
        conn::IConnection* connection,
        Isolation isolation,
        WritePolicy writePolicy);
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_37028FFA_8E9A_4EDD_8916_F30BD46BDCD6