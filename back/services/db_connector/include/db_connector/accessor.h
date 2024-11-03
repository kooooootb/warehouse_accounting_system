#ifndef H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8
#define H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8

#include <db_connector/transaction_policies.h>
#include <instrumental/types.h>
#include <locator/service.h>
#include <locator/service_locator.h>

#include "settings.h"
#include "transaction.h"

namespace srv
{

struct IAccessor : public srv::IService
{
    DECLARE_IID(0X67B00E33);

    /**
     * @brief create transaction
     * @param writePolicy specify if transaction should be read-write or read-only, default = read-write
     * @param isolation specify transaction isolation level, default = repeatable read
     */
    virtual std::unique_ptr<db::ITransaction> CreateTransaction(db::WritePolicy writePolicy = db::WritePolicy::ReadWrite,
        db::Isolation isolation = db::Isolation::RepeatableRead) = 0;

    virtual void SetSettings(const db::DBConnectorSettings& settings) = 0;
};

}  // namespace srv

#endif  // H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8