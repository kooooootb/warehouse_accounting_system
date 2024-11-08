#ifndef H_7357A7FA_86A7_4BE8_9A3C_8803C530B1A4
#define H_7357A7FA_86A7_4BE8_9A3C_8803C530B1A4

#include <pqxx/pqxx>

#include <instrumental/common.h>
#include <instrumental/serialized_enum.h>

#include <tracer/tracer.h>

#include "query_options.h"

namespace srv
{
namespace db
{

/**
 * @details query consists of: options and values.
 * Options are immutable part of query, so they are used in its indentification
 * Values are changing between uses
 */
struct IQuery : public ufa::IBase
{
    /**
     * @brief identify query implementation
     */
    virtual QueryIdentificator GetIdentificator() = 0;

    /**
    * @brief retrieve pqxx's params for current query to use in actual querying
    */
    virtual params_t GetParams() = 0;

    /**
    * @warning this eliminates query options functionality
    */
    virtual std::unique_ptr<IQueryOptions> ExtractOptions() = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_7357A7FA_86A7_4BE8_9A3C_8803C530B1A4